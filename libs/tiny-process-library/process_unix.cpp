#include "process.hpp"
#include <cstdlib>
#include <signal.h>
#include <stdexcept>
#include <unistd.h>

namespace TinyProcessLib {

Process::Data::Data() noexcept : id(-1) {}

Process::Process(const std::function<void()> &function,
                 std::function<void(const char *, size_t)> read_stdout,
                 std::function<void(const char *, size_t)> read_stderr,
                 bool open_stdin, size_t buffer_size) noexcept
    : closed(true), read_stdout(std::move(read_stdout)), read_stderr(std::move(read_stderr)), open_stdin(open_stdin), buffer_size(buffer_size) {
  open(function);
  async_read();
}

Process::id_type Process::open(const std::function<void()> &function) noexcept {
  if(open_stdin)
    stdin_fd = std::unique_ptr<fd_type>(new fd_type);
  if(read_stdout)
    stdout_fd = std::unique_ptr<fd_type>(new fd_type);
  if(read_stderr)
    stderr_fd = std::unique_ptr<fd_type>(new fd_type);

  int stdin_p[2], stdout_p[2], stderr_p[2];

  if(stdin_fd && pipe(stdin_p) != 0)
    return -1;
  if(stdout_fd && pipe(stdout_p) != 0) {
    if(stdin_fd) {
      close(stdin_p[0]);
      close(stdin_p[1]);
    }
    return -1;
  }
  if(stderr_fd && pipe(stderr_p) != 0) {
    if(stdin_fd) {
      close(stdin_p[0]);
      close(stdin_p[1]);
    }
    if(stdout_fd) {
      close(stdout_p[0]);
      close(stdout_p[1]);
    }
    return -1;
  }

  id_type pid = fork();

  if(pid < 0) {
    if(stdin_fd) {
      close(stdin_p[0]);
      close(stdin_p[1]);
    }
    if(stdout_fd) {
      close(stdout_p[0]);
      close(stdout_p[1]);
    }
    if(stderr_fd) {
      close(stderr_p[0]);
      close(stderr_p[1]);
    }
    return pid;
  }
  else if(pid == 0) {
    if(stdin_fd)
      dup2(stdin_p[0], 0);
    if(stdout_fd)
      dup2(stdout_p[1], 1);
    if(stderr_fd)
      dup2(stderr_p[1], 2);
    if(stdin_fd) {
      close(stdin_p[0]);
      close(stdin_p[1]);
    }
    if(stdout_fd) {
      close(stdout_p[0]);
      close(stdout_p[1]);
    }
    if(stderr_fd) {
      close(stderr_p[0]);
      close(stderr_p[1]);
    }

    //Based on http://stackoverflow.com/a/899533/3808293
    int fd_max = static_cast<int>(sysconf(_SC_OPEN_MAX)); // truncation is safe
    for(int fd = 3; fd < fd_max; fd++)
      close(fd);

    setpgid(0, 0);
    //TODO: See here on how to emulate tty for colors: http://stackoverflow.com/questions/1401002/trick-an-application-into-thinking-its-stdin-is-interactive-not-a-pipe
    //TODO: One solution is: echo "command;exit"|script -q /dev/null

    if(function)
      function();

    _exit(EXIT_FAILURE);
  }

  if(stdin_fd)
    close(stdin_p[0]);
  if(stdout_fd)
    close(stdout_p[1]);
  if(stderr_fd)
    close(stderr_p[1]);

  if(stdin_fd)
    *stdin_fd = stdin_p[1];
  if(stdout_fd)
    *stdout_fd = stdout_p[0];
  if(stderr_fd)
    *stderr_fd = stderr_p[0];

  closed = false;
  data.id = pid;
  return pid;
}

Process::id_type Process::open(const std::vector<string_type> &arguments, const string_type &path, const environment_type *environment) noexcept {
  return open([&arguments, &path, &environment] {
    if(arguments.empty())
      exit(127);

    std::vector<const char *> argv_ptrs;
    argv_ptrs.reserve(arguments.size() + 1);
    for(auto &argument : arguments)
      argv_ptrs.emplace_back(argument.c_str());
    argv_ptrs.emplace_back(nullptr);

    if(!path.empty()) {
      if(chdir(path.c_str()) != 0)
        exit(1);
    }

    if(!environment)
      execv(arguments[0].c_str(), const_cast<char *const *>(argv_ptrs.data()));
    else {
      std::vector<std::string> env_strs;
      std::vector<const char *> env_ptrs;
      env_strs.reserve(environment->size());
      env_ptrs.reserve(environment->size() + 1);
      for(const auto &e : *environment) {
        env_strs.emplace_back(e.first + '=' + e.second);
        env_ptrs.emplace_back(env_strs.back().c_str());
      }
      env_ptrs.emplace_back(nullptr);

      execve(arguments[0].c_str(), const_cast<char *const *>(argv_ptrs.data()), const_cast<char *const *>(env_ptrs.data()));
    }
  });
}

Process::id_type Process::open(const std::string &command, const std::string &path, const environment_type *environment) noexcept {
  return open([&command, &path, &environment] {
    if(!path.empty()) {
      if(chdir(path.c_str()) != 0)
        exit(1);
    }

    if(!environment)
      execl("/bin/sh", "/bin/sh", "-c", command.c_str(), nullptr);
    else {
      std::vector<std::string> env_strs;
      std::vector<const char *> env_ptrs;
      env_strs.reserve(environment->size());
      env_ptrs.reserve(environment->size() + 1);
      for(const auto &e : *environment) {
        env_strs.emplace_back(e.first + '=' + e.second);
        env_ptrs.emplace_back(env_strs.back().c_str());
      }
      env_ptrs.emplace_back(nullptr);
      execle("/bin/sh", "/bin/sh", "-c", command.c_str(), nullptr, env_ptrs.data());
    }
  });
}

void Process::async_read() noexcept {
  if(data.id <= 0)
    return;

  if(stdout_fd) {
    stdout_thread = std::thread([this]() {
      auto buffer = std::unique_ptr<char[]>(new char[buffer_size]);
      ssize_t n;
      while((n = read(*stdout_fd, buffer.get(), buffer_size)) > 0)
        read_stdout(buffer.get(), static_cast<size_t>(n));
    });
  }
  if(stderr_fd) {
    stderr_thread = std::thread([this]() {
      auto buffer = std::unique_ptr<char[]>(new char[buffer_size]);
      ssize_t n;
      while((n = read(*stderr_fd, buffer.get(), buffer_size)) > 0)
        read_stderr(buffer.get(), static_cast<size_t>(n));
    });
  }
}

int Process::get_exit_status() noexcept {
  if(data.id <= 0)
    return -1;

  int exit_status;
  waitpid(data.id, &exit_status, 0);
  {
    std::lock_guard<std::mutex> lock(close_mutex);
    closed = true;
  }
  close_fds();

  if(exit_status >= 256)
    exit_status = exit_status >> 8;
  return exit_status;
}

bool Process::try_get_exit_status(int &exit_status) noexcept {
  if(data.id <= 0)
    return false;

  id_type p = waitpid(data.id, &exit_status, WNOHANG);
  if(p == 0)
    return false;

  {
    std::lock_guard<std::mutex> lock(close_mutex);
    closed = true;
  }
  close_fds();

  if(exit_status >= 256)
    exit_status = exit_status >> 8;

  return true;
}

void Process::close_fds() noexcept {
  if(stdout_thread.joinable())
    stdout_thread.join();
  if(stderr_thread.joinable())
    stderr_thread.join();

  if(stdin_fd)
    close_stdin();
  if(stdout_fd) {
    if(data.id > 0)
      close(*stdout_fd);
    stdout_fd.reset();
  }
  if(stderr_fd) {
    if(data.id > 0)
      close(*stderr_fd);
    stderr_fd.reset();
  }
}

bool Process::write(const char *bytes, size_t n) {
  if(!open_stdin)
    throw std::invalid_argument("Can't write to an unopened stdin pipe. Please set open_stdin=true when constructing the process.");

  std::lock_guard<std::mutex> lock(stdin_mutex);
  if(stdin_fd) {
    if(::write(*stdin_fd, bytes, n) >= 0) {
      return true;
    }
    else {
      return false;
    }
  }
  return false;
}

void Process::close_stdin() noexcept {
  std::lock_guard<std::mutex> lock(stdin_mutex);
  if(stdin_fd) {
    if(data.id > 0)
      close(*stdin_fd);
    stdin_fd.reset();
  }
}

void Process::kill(bool force) noexcept {
  std::lock_guard<std::mutex> lock(close_mutex);
  if(data.id > 0 && !closed) {
    if(force)
      ::kill(-data.id, SIGTERM);
    else
      ::kill(-data.id, SIGINT);
  }
}

void Process::kill(id_type id, bool force) noexcept {
  if(id <= 0)
    return;

  if(force)
    ::kill(-id, SIGTERM);
  else
    ::kill(-id, SIGINT);
}

} // namespace TinyProcessLib
