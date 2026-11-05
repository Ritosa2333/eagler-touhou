#include "FileSystem.hpp"
#include "EaglerOptions.hpp"

#include <cstdio>
#include <limits>

#include "GameErrorContext.hpp"
#include "Supervisor.hpp"
#include "pbg4/Pbg4Archive.hpp"

u32 g_LastFileSize;
bool g_LastFileWasRuntimeOverride;

static u8 *ReadRuntimeOverrideFile(const std::string &path)
{
    SDL_IOStream *file = SDL_IOFromFile(path.c_str(), "rb");
    if (!file)
        return NULL;
    const Sint64 length = SDL_GetIOSize(file);
    if (length < 0 || static_cast<Uint64>(length) > std::numeric_limits<u32>::max() ||
        SDL_SeekIO(file, 0, SDL_IO_SEEK_SET) < 0)
    {
        SDL_CloseIO(file);
        return NULL;
    }
    const size_t size = static_cast<size_t>(length);
    u8 *data = static_cast<u8 *>(malloc(size == 0 ? 1 : size));
    if (!data || (size != 0 && SDL_ReadIO(file, data, size) != size))
    {
        free(data);
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
#ifndef TH_ENABLE_THCRAP
    // A localization-disabled build is the strict Japanese regression
    // baseline. Do not let a stale/adjacent thcrap directory affect any file
    // load in that configuration; the whole override namespace is disabled at
    // its single filesystem entry point.
    (void)filepath;
    return NULL;
#else
    if (!filepath || !*filepath)
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
    const std::string root = "/thcrap/th07/";
#else
    const std::string root = "thcrap/th07/";
#endif
    if (u8 *data = ReadRuntimeOverrideFile(root + relative))
        return data;
    const size_t separator = relative.find_last_of('/');
    if (separator != std::string::npos)
        return ReadRuntimeOverrideFile(root + relative.substr(separator + 1));
    return NULL;
#endif
}

u8 *FileSystem::OpenFile(const char *filepath, i32 isExternalResource)
{
    SDL_IOStream *file;
    u8 *buf;
    i64 fsize;
    const char *filename;

    g_LastFileWasRuntimeOverride = false;
    if (u8 *overrideData = OpenRuntimeOverride(filepath))
        return overrideData;

    if (!isExternalResource)
    {
        filename = strrchr(filepath, '\\');
        if (!filename)
        {
            filename = filepath;
        }
        else
        {
            filename++;
        }

        filename = strrchr(filename, '/');
        if (!filename)
        {
            filename = filepath;
        }
        else
        {
            filename++;
        }
        fsize = g_Pbg4Archive.GetEntrySize(filename);
        g_LastFileSize = fsize;
        if (fsize == 0)
        {
            g_GameErrorContext.Fatal("error : %s is not found in arcfile.\n", filename);
            return NULL;
        }
        if (fsize != 0)
        {
            Supervisor::DebugPrint("%s Decode ... \n", filename);
            buf = (u8 *)malloc(fsize);
            if (!buf)
            {
                return NULL;
            }

            g_Pbg4Archive.ReadDecompressEntry(filename, buf);
            return buf;
        }
    }
    Supervisor::DebugPrint("%s Load ... \n", filepath);
    file = SDL_IOFromFile(filepath, "rb");
    if (!file)
    {
        Supervisor::DebugPrint("error : %s is not found.\n", filepath);
        return NULL;
    }

    SDL_SeekIO(file, 0, SDL_IO_SEEK_END);
    fsize = SDL_TellIO(file);
    buf = (u8 *)malloc(fsize);
    if (!buf)
    {
        SDL_CloseIO(file);
        return NULL;
    }

    SDL_SeekIO(file, 0, SDL_IO_SEEK_SET);
    if (SDL_ReadIO(file, buf, fsize) != fsize)
    {
        SDL_CloseIO(file);
        return NULL;
    }
    g_LastFileSize = fsize;
    SDL_CloseIO(file);
    return buf;
}

i32 FileSystem::CheckFileExists(const char *file)
{
    SDL_IOStream *fp;

    fp = SDL_IOFromFile(FileSystem::GetPrefPath(file).c_str(), "rb");
    if (fp)
    {
        SDL_CloseIO(fp);
        return true;
    }
    return false;
}

i32 FileSystem::WriteDataToFile(const char *filename, const void *out, u32 bytesToWrite)
{
    SDL_IOStream *file;
    u32 bytesWritten;

    file = SDL_IOFromFile(FileSystem::GetPrefPath(filename).c_str(), "wb");
    if (!file)
    {
        Supervisor::DebugPrint("error : %s write error\n", filename);
        return -1;
    }

    bytesWritten = SDL_WriteIO(file, out, bytesToWrite);
    if (bytesToWrite != bytesWritten)
    {
        SDL_CloseIO(file);
        Supervisor::DebugPrint("error : %s write error\n", filename);
        return -2;
    }
    SDL_CloseIO(file);
    Supervisor::DebugPrint("%s write ...\n", filename);
    return 0;
}

std::string FileSystem::GetBasePath(const char *filename)
{
#if defined(TH_EXTERNAL_ASSETS)
    const char *path = nullptr;
#if defined(__ANDROID__)
    path = SDL_GetAndroidExternalStoragePath();
#elif defined(__APPLE__) && TARGET_OS_IPHONE
    path = SDL_GetUserFolder(SDL_FOLDER_DOCUMENTS);
#endif
    if (path)
    {
        return std::string(path) + filename;
    }
#endif
    const char *basePath = SDL_GetBasePath();
    if (basePath)
    {
        return std::string(basePath) + filename;
    }
    return std::string(filename);
}

std::string FileSystem::GetPrefPath(const char *filename)
{
#if defined(__EMSCRIPTEN__)
    return std::string(EaglerOptions::MultiplayerStorageEnabled() ? "/savesth07-multiplayer/" : "/savesth07/") + filename;
#elif defined(TH_EXTERNAL_ASSETS)
    return GetBasePath(filename);
#elif defined(__ANDROID__) || defined(__APPLE__)
    static char *prefPath = SDL_GetPrefPath("TeamShanghaiAlice", "th07");
    if (prefPath)
    {
        return std::string(prefPath) + filename;
    }
    return std::string(filename);
#else
    return std::string(filename);
#endif
}
