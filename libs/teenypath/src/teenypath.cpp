#include "teenypath.h"

#include <sys/types.h>
#include <sys/stat.h>

#if defined(_WIN32)
#include <stdexcept>
#include <windows.h>
#elif defined(__linux__) || defined(__APPLE__)
#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <unistd.h>
#endif

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

#include <algorithm>
#include <codecvt>
#include <cstring>
#include <iostream>
#include <locale>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace TeenyPath {

#if defined(_WIN32)
    const char separator = '/';
    const wchar_t wide_separator = L'/';
    const char native_separator = '\\';
    const char dot = '.';
    const char list_separator = ';';
    const char colon = ':';
#elif defined(_POSIX_VERSION)
    constexpr char separator = '/';
    constexpr char dot = '.';
    constexpr char list_separator = ':';
#endif
    static std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> s_wcharConverter;

    namespace {
        std::string
        generic_to_native(const std::string& s) {
#if defined(_WIN32)
            std::string result;

            std::replace_copy(
                s.begin(),
                s.end(),
                std::back_inserter<std::string>(result),
                separator,
                native_separator
            );

            return result;
#elif defined(_POSIX_VERSION)
            return s;
#endif
        }

        std::string
        native_to_generic(const std::string& s) {
#if defined(_WIN32)
            std::string result;

            std::replace_copy(
                s.begin(),
                s.end(),
                std::back_inserter<std::string>(result),
                native_separator,
                separator
            );

            return result;
#elif defined(_POSIX_VERSION)
           return s;
#endif
        }
    }

    path::path(const std::string& s)
        : m_path(native_to_generic(s)) {
    }

    path::path(const char* s)
        : m_path(native_to_generic(s)) {
    }

    path::path(const std::wstring& s)
        : m_path(native_to_generic(s_wcharConverter.to_bytes(s))) {
    }

    path::path(const wchar_t* s)
        : m_path(native_to_generic(s_wcharConverter.to_bytes(std::wstring(s)))) {
    }

    std::string
    path::string() const {
        return native_string();
    }

    std::wstring
    path::wstring() const {
        return s_wcharConverter.from_bytes(native_string());
    }

    std::string
    path::extension() const {
        const std::string filename = this->filename();
        const size_t dotPos = filename.find_last_of(dot);

        return (dotPos == std::string::npos) ?
            "" :
            filename.substr(dotPos);
    }

    std::wstring
    path::wfilename() const {
        return s_wcharConverter.from_bytes(filename());
    }

    std::string
    path::filename() const {
        const size_t slashPos = m_path.find_last_of(separator);
        if (slashPos != std::string::npos) {
            return m_path.substr(slashPos + 1);
        }

        return m_path;
    }

    std::vector<std::string>
    path::split() const {
        std::vector<std::string> result;

        if (m_path.empty()) {
            return result;
        }

        const std::string root = this->get_root().string();
        if (!root.empty()) {
            result.push_back(root);
        }

        size_t pos = std::string::npos, prev_pos = root.size();
        while ((pos = m_path.find(separator, prev_pos)) != std::string::npos) {
            const std::string token = m_path.substr(prev_pos, pos - prev_pos);
            if (!token.empty()) {
                result.push_back(token);
            }
            prev_pos = pos + 1;
        }

        // Get whatever was hanging if it isn't a set of "/"s.
        if (m_path.find(separator, prev_pos) == std::string::npos) {
            const std::string hanging = m_path.substr(prev_pos);
            if (!hanging.empty()) {
                result.push_back(hanging);
            }
        }

        return result;
    }

    void
    path::replace_extension(const std::string& new_extension) {
        std::string filename = this->filename();
        const path parent = this->parent_path();

        const size_t dotPos = filename.find_last_of(dot);
        if (dotPos != std::string::npos) {
            filename.erase(dotPos);
        }

        m_path = (parent / filename).generic_string();

        if (new_extension.empty()) {
            return;
        }

        m_path.append(new_extension);
        return;
    }

    std::string
    path::native_string() const {
        return generic_to_native(m_path);
    }

    std::string
    path::generic_string() const {
        return native_to_generic(m_path);
    }

    bool
    path::exists() const {
        const path absp = this->is_absolute() ? path(*this) : resolve_absolute();
#if defined(_POSIX_VERSION)
        return access(absp.string().c_str(), F_OK) != -1;
#elif defined(_WIN32)
        return _waccess(absp.wstring().c_str(), 0) != -1;
#endif
    }

    bool
    path::is_absolute() const {
#if defined(_WIN32)
        // "c:/..." or "d:/..." or ...
        if (m_path.size() > 2
            && m_path[1] == colon) {
            return true;
        }
#endif
        // "//foo/bar" for UNC paths
        if (m_path.size() > 2
            && m_path[0] == separator
            && m_path[1] == separator
            && m_path[2] != separator) {
            return true;
        }

        // "/foo/bar" for normal UNIX paths
        if (m_path.size() > 1
            && m_path[0] == separator
            && m_path[1] != separator) {
            return true;
        }

        return false;
    }

    bool
    path::is_empty() const {
        return m_path.empty();
    }

    path
    path::parent_path() const {
        if (this->is_root()) {
            return this->get_root();
        }

        if (this->is_lexically_normal()) {
            return path(m_path.substr(0, m_path.find_last_of(separator)));
        }

        return this->lexically_normalized().parent_path();
    }

    bool
    path::is_lexically_normal() const {
        const std::vector<std::string> parts = this->split();

        return count(parts.begin(),
                     parts.end(),
                     "..") == 0
            && count(parts.begin(),
                     parts.end(),
                     ".") == 0;
    }

    bool
    path::is_root() const {
        if (this->split().size() == 1) {
#if defined(_WIN32)
            // "x:" or "y:/" or ...
            if ((m_path.size() == 2
                || m_path.size() == 3)
                && m_path[1] == colon) {
                return true;
            }
#endif
            // "//foo" for UNC paths
            if (m_path.size() > 2
                && m_path[0] == separator
                && m_path[1] == separator
                && m_path[2] != separator) {
                return true;
            }

            // "/foo" for normal UNIX paths
            if (m_path.size() > 1
                && m_path[0] == separator
                && m_path[1] != separator) {
                return true;
            }
        }

        // Just "/" or "//"
        if (m_path == "/"
            || m_path == "//") {
            return true;
        }

        return false;
    }

    bool
    path::is_directory() const {
#if defined(_POSIX_VERSION)
        struct stat sb;
        lstat(m_path.c_str(), &sb);
        return (sb.st_mode & S_IFMT) == S_IFDIR;
#elif defined(_WIN32)
        DWORD fileAttributes = GetFileAttributesW(this->wstring().c_str());
        return fileAttributes != INVALID_FILE_ATTRIBUTES
            && (fileAttributes & FILE_ATTRIBUTE_DIRECTORY);
#endif
    }

    bool
    path::is_regular_file() const {
#if defined(_POSIX_VERSION)
        struct stat sb;
        lstat(m_path.c_str(), &sb);
        return (sb.st_mode & S_IFMT) == S_IFREG;
#elif defined(_WIN32)
        DWORD fileAttributes = GetFileAttributesW(this->wstring().c_str());
        return fileAttributes != INVALID_FILE_ATTRIBUTES
            && !(fileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)
            && !(fileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            && !(fileAttributes & FILE_ATTRIBUTE_DEVICE);
#endif
    }

    bool
    path::is_symlink() const {
#if defined(_POSIX_VERSION)
        struct stat sb;
        lstat(m_path.c_str(), &sb);
        return (sb.st_mode & S_IFMT) == S_IFLNK;
#elif defined(_WIN32)
        DWORD fileAttributes = GetFileAttributesW(this->wstring().c_str());
        return fileAttributes != INVALID_FILE_ATTRIBUTES
            && (fileAttributes & FILE_ATTRIBUTE_REPARSE_POINT);
#endif
    }

    path&
    path::operator/=(const path& p) {
        // front() and back() give undefined behavior if empty() is true.
        if (p.is_empty()) {
            return *this;
        }

        if (p.m_path.front() != separator
            && !m_path.empty()
            && m_path.back() != separator) {
            m_path.push_back(separator);
        }

        m_path.append(p.m_path);
        return *this;
    }

    path&
    path::operator/=(const std::string& s) {
        return this->operator/=(path(s));
    }

    path
    path::trim_trailing_slashes() const {
        path p(*this);

        // Don't let internal representations end in a trailing slash
        // (unless it's a path to the root directory) because it
        // complicates all further logic without gain.
        while (p.m_path.back() == separator
            && p.m_path.size() > 2) {
            p.m_path.pop_back();
        }

        return p;
    }

    /**
     * This will return c: or //sharename or /foo or just "/" and "//"
     */
    path
    path::get_root() const {
#if defined(_WIN32)
        // "c:" or "d:" or ...
        if (m_path.size() >= 2
            && m_path[1] == colon) {
            return m_path.substr(0, 2);
        }
#endif

        if (m_path.size() >= 1
            && m_path[0] == separator) {

            // "//foo" for UNC paths
            if (m_path.size() >= 2
                && m_path[1] == separator
                && m_path[2] != separator) {

                // "//"
                if ((m_path.size() >= 4 &&m_path.substr(0, 4) == "//..")
                    || (m_path.size() >= 4 && m_path.substr(0, 4) == "//./")
                    || (m_path.size() == 3 && m_path == "//.")
                    || m_path == "//") {
                    return path("//");
                }

                // "//foo"
                const size_t endOfDriveName = m_path.find(separator, 2);
                return path(m_path.substr(0, endOfDriveName));
            }

            // "/"
            if ((m_path.size() >= 3 && m_path.substr(0, 3) == "/..")
                || (m_path.size() >= 3 && m_path.substr(0, 3) == "/./")
                || (m_path.size() == 2 && m_path == "/.")
                || m_path == "/") {
                return path("/");
            }

            // "/foo"
            return path(m_path.substr(0, m_path.find(separator, 1)));
        }
        return path();
    }

    path
    path::lexically_normalized() const {
        if (this->is_lexically_normal()) {
            return path(*this);
        }

        path p;
        const std::vector<std::string> parts = this->split();
        for (const auto& part : parts) {
            // Resolve ".." by moving up a level and continuing.
            if (part == "..") {
                p = p.parent_path();
                continue;
            }

            // Resolve "." by not adding it in the first place and
            // continuing.
            if (part == ".") {
                continue;
            }

            p /= part;
        }

        return p;
    }

    /**
     * This will perform filesystem operations.
     *
     * Caller beware.
     */
    path
    path::resolve_absolute() const {
#if defined(_POSIX_VERSION)
        std::unique_ptr<char, decltype(&std::free)> rpath(realpath(m_path.c_str(), NULL), std::free);
        if (!rpath) {
            return path("");
        }
        return path(rpath.get());
#elif defined(_WIN32)
        HANDLE hFile = CreateFile(this->wstring().c_str(),   // file to open
            GENERIC_READ,                          // open for reading
            FILE_SHARE_READ,                       // share for reading
            NULL,                                  // default security
            OPEN_EXISTING,                         // existing file only
            this->is_regular_file() ?
                                  FILE_ATTRIBUTE_NORMAL :             // file
                                  FILE_FLAG_BACKUP_SEMANTICS,         // directory
            NULL);

        if (hFile == INVALID_HANDLE_VALUE)
        {
            throw std::runtime_error("Could not get handle: " + m_path);
        }

        // Query the length of the output string.
        DWORD pathLength = GetFinalPathNameByHandle(hFile,              // handle to file
            NULL,               // null buffer
            0,                  // max buffer size
            VOLUME_NAME_DOS);   // path with normal volume label

        if (pathLength == 0)
        {
            CloseHandle(hFile);
            throw std::runtime_error("Failed to resolve path for: " + m_path);
        }

        std::unique_ptr<wchar_t[]> buffer = std::make_unique<wchar_t[]>(pathLength);
        DWORD result = GetFinalPathNameByHandle(hFile,             // handle to file
            buffer.get(),      // buffer that receives the resolved path
            pathLength,        // max buffer size

                        VOLUME_NAME_DOS);  // path with normal volume label
        CloseHandle(hFile);

        if (result == 0 || result > pathLength)
        {
            throw std::runtime_error("Failed to resolve path for: " + this->string());
        }

        // Need to lop "\\?\" off the front.
        return path(buffer.get() + 4);
#endif
    }

    path
    operator/(const path& lhs, const path& rhs) {
        return path(lhs) /= rhs;
    }

    path
    operator/(const path& lhs, const std::string& rhs) {
        return path(lhs) /= path(rhs);
    }

    path
    operator/(const path& lhs, const char* rhs) {
        return path(lhs) /= path(rhs);
    }

    bool
    operator==(const path& lhs, const path& rhs) {
        return lhs.lexically_normalized().string() == rhs.lexically_normalized().string();
    }

    std::vector<path>
    ls(const path& given) {
        std::vector<path> directoryContents;

        if (!given.is_directory()) {
            return directoryContents;
        }

        path p = given.trim_trailing_slashes();

#if defined(_POSIX_VERSION)
        DIR* dir;
        struct dirent* file;

        dir = opendir(p.string().c_str());
        while ((file = readdir(dir)) != NULL) {
            directoryContents.emplace_back(p.string() + separator + file->d_name);
        }
        closedir(dir);
#elif defined(_WIN32)
        WIN32_FIND_DATA findFileData;

        wchar_t preppedFilePath[MAX_PATH];
        wcsncpy_s(preppedFilePath, p.wstring().c_str(), MAX_PATH);
        wcsncat_s(preppedFilePath, L"\\*", 2);

        // On Windows we find the first file, then loop around for the others.
        HANDLE findHandle = FindFirstFile(preppedFilePath, &findFileData);

        if (findHandle == INVALID_HANDLE_VALUE) {
            return directoryContents;
        }

        directoryContents.emplace_back(p.wstring() + wide_separator + findFileData.cFileName);
        while (FindNextFile(findHandle, &findFileData) != 0) {
            directoryContents.emplace_back(p.wstring() + wide_separator + findFileData.cFileName);
        }
        FindClose(findHandle);
#endif

        return directoryContents;
    }

    std::string
    joinPathList(const std::vector<path>& pathList) {
        std::string returnValue;

        for (size_t i = 0; i < pathList.size(); i++) {
            const auto& p = pathList[i];
            returnValue += p.string();
            if (i != pathList.size() - 1) {
                returnValue += list_separator;
            }
        }

        return returnValue;
    }

    std::vector<path>
    splitPathList(const std::string& pathList) {
        std::vector<path> result;

        std::istringstream p(pathList);
        std::string s;
        while (getline(p, s, list_separator)) {
            if (!s.empty()) {
                result.emplace_back(s);
            }
        }

        return result;
    }
}
