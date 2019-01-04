#pragma once

#include <string>
#include <vector>

namespace TeenyPath {

    class path {
    public:
        path() {}
        path(const std::string& s);
        path(const std::wstring& s);
        path(const char* s);
        path(const wchar_t* s);

        bool exists() const;
        bool is_absolute() const;
        bool is_directory() const;
        bool is_empty() const;
        // Checks if the path has any references to "." or ".."
        bool is_lexically_normal() const;
        bool is_regular_file() const;
        // Checks if the path is rooted to a UNC root "//", Unix root "/", or,
        // for Windows only, a Windows root "x:/"
        bool is_root() const;
        bool is_symlink() const;

        std::string extension() const;
        std::string filename() const;
        std::string string() const;

        // These are used to convert the Windows-style '\\' to '/' and back.
        std::string generic_string() const;
        std::string native_string() const;

        std::wstring wfilename() const;
        std::wstring wstring() const;

        // Will resolve references to "." or "..".  Will not resolve symlinks.
        path lexically_normalized() const;
        path parent_path() const;
        // Will take a relative path or absolute path with symlinks and resolve
        // all "." and ".." and symlinks to get the real path on disk.
        path resolve_absolute() const;
        path trim_trailing_slashes() const;

        void replace_extension(const std::string& new_extension);

        path& operator/=(const path& p);
        path& operator/=(const std::string& s);

    private:
        std::string m_path;

        path get_root() const;
        std::vector<std::string> split() const;
    };

    path operator/(const path& lhs, const path& rhs);
    path operator/(const path& lhs, const std::string& rhs);
    path operator/(const path& lhs, const char* rhs);
    bool operator==(const path& lhs, const path& rhs);

    std::string joinPathList(const std::vector<path>& pathList);
    std::vector<path> splitPathList(const std::string& pathList);
    std::vector<path> ls(const path& p);
}
