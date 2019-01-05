[Demo](https://youtu.be/2MsxAMS_z18)

[![Build Status](https://travis-ci.com/ddovod/jet-live.svg?branch=master)](https://travis-ci.com/ddovod/jet-live)

# jet-live

**jet-live** is a library for c++ "hot code reloading". It works on linux on 64 bit systems powered by cpu with x86-64 instruction set. Apart from reloading of functions it is able to transfer static state (please refer to "How it works" for what is it and why it is important).
Tested on Ubuntu 18.04 with clang 6.0.1/7.0.1, cmake 3.10.2, ninja 1.8.2/make 4.1.

**Important:** this library doesn't force you to organize your code in some special way (like in RCCPP or cr), you don't need to separate reloadable code into some shared library, **jet-live** should work with any project in the least intrusive way.

If you need something similar for windows, please try [blink](https://github.com/crosire/blink), I have no plans to support windows.

\* macos port is not completed yet, but will be ready in a couple of weeks or so.

### Prerequisites
You need `c++11` compliant compiler. Also there're several dependencies which are bundled in:
- `nlohmann json` (header only): needed to parse `compile_commands.json` file
- `argh` (header only): needed to parse compilation options
- `teenypath` (1 .h/1 .cpp): needed to deal with filesystem
- `tiny-process-library` (1 .h/2 .cpp): needed to run compilation processes
- `efsw` (the only big dependency): needed to watch for source file changes
- `whereami` (1 .h/1 .cpp): needed to find path to this executable
- `subhook` (1 .h/2 .c): needed for function hooking
- `ELFIO` (header only, for linux): needed to parse elf headers and sections data

### Getting started
This library is best suited for projects based on cmake and make or ninja build systems, defaults are fine-tuned for these tools. The CMakeLists.txt will add `set(CMAKE_EXPORT_COMPILE_COMMANDS ON)` option for `compile_commands.json` and alter compiler (adding `-MD`) and linker (adding `-rdynamic` for linux and `-Wl,-export_dynamic` for macos) flags. This is important and not avoidable. if you use ninja, add `-d keepdepfile` ninja flag when running ninja, this is needed to track dependencies between source and header files
1. In your project CMakeLists.txt file:
```
set(JET_LIVE_BUILD_EXAMPLE OFF)
set(JET_LIVE_SHARED ON) # if you want to
add_subdirectory(path/to/jet-live)
target_link_libraries(your-app-target jet-live)
```
2. Create an instance of `jet::Live` class
3. In your app runloop call `liveInstance->update()`
4. When you need to reload code, call `liveInstance->tryReload()`

**Important:** This library is not thread safe. It uses threads under the hood to run compiler, but you should call all library methods from the same thread.

Also I use this library only with debug builds (`-O0`, not stripped, without `-fvisibility=hidden` and things like that) to not deal with optimized and inlined functions and variables. I don't know how it works on highly optimized release builds, but you can try, probably it will fit your needs.

Personally I use it like this. I have a `Ctrl+r` shortcut to which `tryReload` is assigned in my application. Also app calls `update` in the main runloop and listens for `onCodePreLoad` and `onCodePostLoad` events to recreate some objects or re-evaluate some functions:
1. I start my application
2. I edit some files, save it, and now I know that I'm ready to reload new code (here previously I recompiled application)
3. I press `Ctrl+r`
4. I'm watching for output of my application to see if there're any compilation/linkage errors (see "Customizations")
5. Both for success or fail, go to 2

**jet-live** will monitor for file changes, recompile changed files and only when `tryReload` is called it will wait for all current compilation processes to finish and reload new code. Please don't call `tryReload` on each update, it will not work, call it only when your source code is ready to be reloaded.

### Features
Implemented:
- Reloading of functions
- Transferring of static variables
- Tracking of dependencies
- Working with code from this executable and loaded shared libraries

Will be ready soon (in the importance order):
- macos implementation
- Ability to add new compilations units on the fly (see "How it works")
- Unit tests

### Customizations
**jet-live** is fine-tuned to work with cmake and make/ninja tools, but if you want to adopt it to another build tool, or to receive logs from it, there's a way to customize its' behaviour in some aspects. Please refer to documentation of `LiveDelegate`, `ICompilationUnitsParser` and `IDependenciesHandler` for more info and examples.

**Important:** it is highly recommended to log all messages from the library to see if something went wrong. Please see `LiveDelegate::onLog` method.

### How it works (for curious ones)
The library reads elf headers and sections of this executable, finds all symbols and tries to find out which of them can either be hooked (functions) or should be transferred (static variables). Also it finds symbols size and "real" address. You can configure the behaviour of filtering, but defaults should work fine.

Apart from that **jet-live** tries to find `compile_commands.json` near your executable or in its' parent directories recursively. You can configure this behaviour. Using this file it distinguishes:
- compilation command
- source file path
- `.o` (object) file path
- `.d` (depfile) files path
- compiler path
- working directory - directory from which the compiler was run
- some compiler flags for further processing.

Right now if you create new `.cpp` file and even recreate `compile_commands.json` (i.e. by running cmake after that), library will not find it. This feature is in my TODO list.

When all compilation units are parsed, it distinguishes the most common directory for all source files and starts watching it recursively using filesystem watcher. This path can, and probably should, be configured cause if you don't want to touch your project dependencies, you should avoid watching excess files to reduce your system load. So please point this directory explicitly to your projects source root without libraries you use. Or just do nothing if you don't care, default behaviour is fine.

After that the library tries to find all dependencies for each compilation unit. By default it will read depfiles near the object files (see `-MD` compiler option). Suppose the object file is located at:
```
/home/coolhazker/projects/some_project/build/main.cpp.o
```
**jet-live** will try to find depfile at:
```
/home/coolhazker/projects/some_project/build/main.cpp.o.d
or
/home/coolhazker/projects/some_project/build/main.cpp.d
```
It will pick up all dependencies which are under the watching directories, so things like `/usr/include/elf.h` will not be treated as dependency even if this file really use it cause most likely you'll not touch any system headers and libraries your project depends on.

You can alter this behaviour and provide your own implementation of dependencies handler.

Now the library is initialized

Next, when you edit some source file and save it, **jet-live** immediately starts compilation of all dependent files in the background. By default the number of simultaneous compilation processes is 4, but you can configure it. It will write to log about successes and errors using `LiveDelegate::onLog` method of delegate. If you trigger compilation of some file when it is already compiling (or waiting in the queue), old compilation process will be killed and new one will be added to the queue, so its kinda safe to not wait for compilation to finish and make new changes of the code. Also after each file was compiled, it will update dependencies for compiled file since compiler can recreate depfile for it if new version of compilation unit has new dependencies.

When you call `Live::tryReload`, the library will wait for unfinished compilation processes and then all accumulated new object files will be linked together in shared library and placed near your executable with name `lib_reloadXXX.so`, where `XXX` is a number of "reloads" during this session. So `lib_reloadXXX.so` contains all new code. The most interesting part starts here.

**jet-live** loads this library using `dlopen`, reads elf headers and sections, finds all symbols and:
- For all hookable functions it transfers the control flow from old version to new one
- For all static variables it just `memcpy` memory from old location to new one

**Important:** `LiveDelegate::onCodePreLoad` event is fired right before `lib_reloadXXX.so` is loaded into the process memory. `LiveDelegate::onCodePostLoad` event is fired right after all functions was hooked and all variables was transferred.

##### About functions hooking
You can read more about function hooking [here](https://jbremer.org/x86-api-hooking-demystified/). This library uses awesome [subhook](https://github.com/Zeex/subhook) library to redirect function flow from old to new ones. You can see that on 32 bit platforms your functions should be at least 5 bytes long to be hookable. On 64 bit you need at least 14 bytes which is a lot, and for example empty stub function will probably not fit into 14 bytes. I don't know the truth, but if you disassemble your program compiled with modern compiler, you will see that all functions are 16 bytes aligned (at least on debug without optimizations). That means the spacing between begins of any 2 functions is not less that 16 bytes, which makes possible to hook any function. Probably it somehow connected to [this](https://stackoverflow.com/questions/22235236/how-much-does-function-alignment-actually-matter-on-modern-processors). If you have some problems with it, just declare a couple of variables in the empty stub function to make its' size >= 14 bytes.

##### About state transfer
By default all static variables are transferred from old code to new one. Why is it important?
Suppose you have (a bit synthetic example, but anyway):
```cpp
// Singleton.hpp
class Singletor
{
public:
    static Singleton& instance();
};

int veryUsefulFunction(int value);

// Singleton.cpp
Singleton& Singletor::instance()
{
    static Singleton ins;
    return ins;
}

int veryUsefulFunction(int value)
{
    return value * 2;
}
```
Then you want to update `veryUsefulFunction` to smth like this:
```cpp
int veryUsefulFunction(int value)
{
    return value * 3;
}
```
Great, it will now multiplies argument by 3. But since whole `Singleton.cpp` will be reloaded and `Singleton::instance` function will be hooked to call new version, `lib_reloadXXX.so` will contain new static variable `static Singleton ins`, which is not initialized, and if you call `Singleton::instance()` after code was reloaded, it will initialize this variable again which is not good cause we don't want to call its constructor again. Thats why we need to transfer statics as well as their [guard variables](https://monoinfinito.wordpress.com/2013/12/03/static-initialization-in-c/).

Also your app will probably crash if you try to change memory layout of your data types in reloadable code.

Suppose you have an instance of this class allocated somewhere in the heap or on the stack:
```cpp
class SomeClass
{
public:
    void calledEachUpdate() {
        m_someVar1++;
    }
private:
    int m_someVar1 = 0;
}
```
You edit it and now it looks like:
```cpp
class SomeClass
{
public:
    void calledEachUpdate() {
        m_someVar1++;
        m_someVar2++;
    }
private:
    int m_someVar1 = 0;
    int m_someVar2 = 0;
}
```
After code is reloaded, you'll probably observe a crash because already allocated object has different data **layout**, it has no `m_someVar2` instance variable, but new version of `calledEachUpdate` will try to modify it actually modifying random data. In such cases you should delete this instance in `onCodePreLoad` callback and recreate it in `onCodePostLoad` callback. Correct transfer of its state is up to you. The same effect will take place if you'll try to change static data structures **layout**. The same also correct for polymorphic classes.

### Licence
MIT

For licences of used libraries please refer to their directories and source code.
