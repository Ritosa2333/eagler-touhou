#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <vector>

#ifdef _WIN32
#include <direct.h>
#include <new>
#include <windows.h>
#elif __cplusplus >= 201703L
#include <filesystem>
#else // Assume POSIX
#include <sys/stat.h>
#endif

#include "FileSystem.hpp"
#include "pbg3/Pbg3Archive.hpp"
#include "utils.hpp"

u32 g_LastFileSize;
bool g_LastFileWasRuntimeOverride;

static u8 *ReadRuntimeOverrideFile(const std::string &path)
{
    SDL_IOStream *file = SDL_IOFromFile(path.c_str(), "rb");
    if (file == NULL)
        return NULL;
    const Sint64 length = SDL_GetIOSize(file);
    if (length < 0 || static_cast<Uint64>(length) > std::numeric_limits<u32>::max() ||
        SDL_SeekIO(file, 0, SDL_IO_SEEK_SET) < 0)
    {
        SDL_CloseIO(file);
        return NULL;
    }
    const size_t size = static_cast<size_t>(length);
    u8 *data = static_cast<u8 *>(std::malloc(size == 0 ? 1 : size));
    if (data == NULL || (size != 0 && SDL_ReadIO(file, data, size) != size))
    {
        std::free(data);
        SDL_CloseIO(file);
        return NULL;
    }
    SDL_CloseIO(file);
    g_LastFileSize = static_cast<u32>(size);
    g_LastFileWasRuntimeOverride = true;
    return data;
}

u8 *FileSystem::OpenRuntimeOverride(const char *filepath)
{
    g_LastFileWasRuntimeOverride = false;
    if (filepath == NULL || *filepath == '\0')
        return NULL;
    std::string relative(filepath);
    for (char &character : relative)
        if (character == '\\')
            character = '/';
    while (relative.rfind("./", 0) == 0)
        relative.erase(0, 2);
    if (relative.empty() || relative.front() == '/' || relative.find(':') != std::string::npos ||
        relative == ".." || relative.rfind("../", 0) == 0 || relative.find("/../") != std::string::npos ||
        (relative.size() >= 3 && relative.compare(relative.size() - 3, 3, "/..") == 0))
        return NULL;
#ifdef __EMSCRIPTEN__
    const std::string root = "/thcrap/th06/";
#else
    const std::string root = "thcrap/th06/";
#endif
    if (u8 *data = ReadRuntimeOverrideFile(root + relative))
        return data;
    const size_t separator = relative.find_last_of('/');
    if (separator != std::string::npos)
        return ReadRuntimeOverrideFile(root + relative.substr(separator + 1));
    return NULL;
}

std::string FileSystem::GetPrefPath(const char *filepath)
{
    while (filepath[0] == '.' && (filepath[1] == '/' || filepath[1] == '\\'))
        filepath += 2;
#ifdef __EMSCRIPTEN__
    return std::string("/savesth06/") + filepath;
#elif defined(__ANDROID__) || defined(__APPLE__)
    static char *prefPath = SDL_GetPrefPath("TeamShanghaiAlice", "th06");
    return prefPath ? std::string(prefPath) + filepath : std::string(filepath);
#else
    return std::string(filepath);
#endif
}

SDL_IOStream *FileSystem::OpenFileStream(const char *filepath, const char *mode)
{
    if (filepath == NULL || mode == NULL)
        return NULL;
#if defined(__EMSCRIPTEN__) || defined(__ANDROID__) || defined(__APPLE__)
    const bool writes = std::strchr(mode, 'w') || std::strchr(mode, 'a') || std::strchr(mode, '+');
    const std::string prefPath = GetPrefPath(filepath);
    SDL_IOStream *prefStream = SDL_IOFromFile(prefPath.c_str(), mode);
    if (prefStream || writes)
        return prefStream;
#endif
    SDL_IOStream *stream = SDL_IOFromFile(filepath, mode);
#if defined(__APPLE__)
    if (stream == NULL)
    {
        const char *basePath = SDL_GetBasePath();
        if (basePath != NULL)
        {
            stream = SDL_IOFromFile((std::string(basePath) + filepath).c_str(), mode);
        }
    }
#endif
    return stream;
}

FILE *FileSystem::FopenUTF8(const char *filepath, const char *mode)
{
#if defined(__EMSCRIPTEN__) || defined(__ANDROID__) || defined(__APPLE__)
    const std::string prefPath = GetPrefPath(filepath);
    const bool writes = std::strchr(mode, 'w') || std::strchr(mode, 'a') || std::strchr(mode, '+');
    FILE *file = std::fopen(prefPath.c_str(), mode);
    if (file || writes)
        return file;
    return std::fopen(filepath, mode);
#elif !defined(_WIN32)
    return std::fopen(filepath, mode);
#else
    const int filepathWLen = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, filepath, -1, NULL, 0);
    const int modeWLen = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, mode, -1, NULL, 0);

    if (filepathWLen == 0 || modeWLen == 0)
    {
        return NULL;
    }

    std::vector<wchar_t> filepathW(filepathWLen);
    std::vector<wchar_t> modeW(modeWLen);

    if (MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, filepath, -1, filepathW.data(), filepathWLen) == 0 ||
        MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, mode, -1, modeW.data(), modeWLen) == 0)
    {
        return NULL;
    }

    return _wfopen(filepathW.data(), modeW.data());
#endif
}

void FileSystem::CreateDir(const char *path)
{
#if defined(__EMSCRIPTEN__) || defined(__ANDROID__) || defined(__APPLE__)
    std::filesystem::create_directories(GetPrefPath(path));
#elif defined(_WIN32)
    _mkdir(path);
#elif __cplusplus >= 201703L
    auto p = std::filesystem::path(path);
    std::filesystem::create_directory(p);
#else
    mkdir(path, 0755);
#endif
}

u8 *FileSystem::OpenPath(const char *filepath, int isExternalResource)
{
    u8 *data;
    SDL_IOStream *file;
    size_t fsize;
    i32 entryIdx;
    const char *entryname;
    i32 pbg3Idx;

    g_LastFileSize = 0;
    g_LastFileWasRuntimeOverride = false;
    if (filepath == NULL || *filepath == '\0')
    {
        return NULL;
    }

    entryIdx = -1;
    if (isExternalResource == 0)
    {
        if (u8 *overrideData = OpenRuntimeOverride(filepath))
            return overrideData;
        const char *backslash = std::strrchr(filepath, '\\');
        const char *slash = std::strrchr(filepath, '/');
        const char *separator = backslash == NULL ? slash : (slash == NULL || backslash > slash ? backslash : slash);
        entryname = separator == NULL ? filepath : separator + 1;
        if (g_Pbg3Archives != NULL)
        {
            for (pbg3Idx = 0; pbg3Idx < 0x10; pbg3Idx += 1)
            {
                if (g_Pbg3Archives[pbg3Idx] != NULL)
                {
                    entryIdx = g_Pbg3Archives[pbg3Idx]->FindEntry(entryname);
                    if (entryIdx >= 0)
                    {
                        break;
                    }
                }
            }
        }
        if (entryIdx < 0)
        {
            return NULL;
        }
    }
    if (entryIdx >= 0)
    {
        utils::DebugPrint2("%s Decode ... \n", entryname);
        data = g_Pbg3Archives[pbg3Idx]->ReadDecompressEntry(entryIdx, entryname);
        if (data != NULL)
        {
            g_LastFileSize = g_Pbg3Archives[pbg3Idx]->GetEntrySize(entryIdx);
        }
    }
    else
    {
        utils::DebugPrint2("%s Load ... \n", filepath);
        file = OpenFileStream(filepath, "rb");
        if (file == NULL)
        {
            utils::DebugPrint2("error : %s is not found.\n", filepath);
            return NULL;
        }
        else
        {
            const Sint64 fileLength = SDL_GetIOSize(file);
            if (fileLength < 0 || static_cast<Uint64>(fileLength) > std::numeric_limits<u32>::max() ||
                SDL_SeekIO(file, 0, SDL_IO_SEEK_SET) < 0)
            {
                SDL_CloseIO(file);
                return NULL;
            }
            fsize = static_cast<size_t>(fileLength);
            data = (u8 *)std::malloc(fsize == 0 ? 1 : fsize);
            if (data == NULL || (fsize != 0 && SDL_ReadIO(file, data, fsize) != fsize))
            {
                std::free(data);
                SDL_CloseIO(file);
                return NULL;
            }
            SDL_CloseIO(file);
            g_LastFileSize = static_cast<u32>(fsize);
        }
    }
    return data;
}

int FileSystem::WriteDataToFile(const char *path, const void *data, size_t size)
{
    SDL_IOStream *f;

    f = OpenFileStream(path, "wb");
    if (f == NULL)
    {
        return -1;
    }
    else
    {
        if (SDL_WriteIO(f, data, size) != size)
        {
            SDL_CloseIO(f);
            return -2;
        }
        else
        {
            SDL_CloseIO(f);
            return 0;
        }
    }
}
