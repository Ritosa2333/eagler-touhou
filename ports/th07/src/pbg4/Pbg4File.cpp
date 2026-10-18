#include "Pbg4File.hpp"

#include <SDL3/SDL.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "inttypes.hpp"

const SDL_IOWhence g_SeekModes[3] = {SDL_IO_SEEK_SET, SDL_IO_SEEK_CUR, SDL_IO_SEEK_END};

// would it really not have been simpler to just type the letter where its used
// GLOBAL: TH07 0x0049ea70
const char *g_AccessModes[3] = {
    "r",
    "w",
    "a",
};

Pbg4File::Pbg4File()
{
    this->file = NULL;
    this->access = 0;
}

Pbg4File::~Pbg4File()
{
    Close();
}

bool Pbg4File::Open(const char *path, const char *mode)
{
    char local_114[1024];
    i32 local_c;
    const char *local_8;

    local_c = 0;
    this->Close();
    for (local_8 = mode; *local_8 != '\0'; ++local_8)
    {
        if (*local_8 == 'r')
        {
            this->access = "rb";
            break;
        }
        if (*local_8 == 'w')
        {
            remove(path);
            this->access = "wb";
            break;
        }
        if (*local_8 == 'a')
        {
            local_c = 1;
            this->access = "ab";
            break;
        }
    }
    if (*local_8 == '\0')
    {
        return false;
    }
    else
    {
        GetFullPath(local_114, path);
        this->file = SDL_IOFromFile(local_114, this->access);
        if (!this->file)
        {
            return false;
        }

        if (local_c != 0)
        {
            SDL_SeekIO(this->file, 0, SDL_IO_SEEK_END);
        }
        return true;
    }
}

void Pbg4File::Close()
{
    if (this->file)
    {
        SDL_CloseIO(this->file);
        this->file = NULL;
        this->access = 0;
    }
}

u32 Pbg4File::Read(void *data, u32 len)
{
    u32 local_8;

    local_8 = 0;
    if (!this->access || strcmp(this->access, "rb") != 0)
    {
        return 0;
    }

    local_8 = SDL_ReadIO(this->file, data, len);
    return local_8;
}

bool Pbg4File::Write(void *data, u32 len)
{
    u32 local_8;

    local_8 = 0;
    if (!this->access || strcmp(this->access, "wb") != 0)
    {
        return false;
    }

    local_8 = SDL_WriteIO(this->file, data, len);
    return len == local_8;
}

u32 Pbg4File::Tell()
{
    if (!this->file)
    {
        return 0;
    }
    else
    {
        return SDL_TellIO(this->file);
    }
}

u32 Pbg4File::GetSize()
{
    if (!this->file)
    {
        return 0;
    }
    else
    {
        i64 cur = SDL_TellIO(this->file);
        SDL_SeekIO(this->file, 0, SDL_IO_SEEK_END);
        u32 size = SDL_TellIO(this->file);
        SDL_SeekIO(this->file, cur, SDL_IO_SEEK_SET);
        return size;
    }
}

bool Pbg4File::Seek(u32 offset, SDL_IOWhence seekFrom)
{
    if (!this->file)
    {
        return false;
    }

    SDL_SeekIO(this->file, offset, seekFrom);
    return true;
}

void *Pbg4File::ReadRemaining(u32 max)
{
    void *hMem;
    u32 DVar2;
    u32 DVar3;

    if (!this->access || strcmp(this->access, "rb") != 0)
    {
        return NULL;
    }

    DVar2 = this->GetSize();
    if (DVar2 > max)
    {
        return NULL;
    }

    hMem = calloc(1, DVar2);
    if (!hMem)
    {
        return NULL;
    }

    DVar3 = this->Tell();
    if (!this->Seek(DVar3, g_SeekModes[0]))
    {
        return NULL;
    }

    if (this->Read(hMem, DVar2) == 0)
    {
        if (hMem)
        {
            free(hMem);
            hMem = NULL;
        }
        return NULL;
    }

    this->Seek(DVar3, g_SeekModes[0]);
    return hMem;
}

void Pbg4File::GetFullPath(char *out, const char *filename)
{
#ifdef __ANDROID__
    snprintf(out, 1024, "%s", filename);
    return;
#endif

#ifdef _WIN32
    if (strchr(filename, ':') != nullptr)
    {
        snprintf(out, 1024, "%s", filename);
        return;
    }
#else
    if (filename[0] == '/')
    {
        snprintf(out, 1024, "%s", filename);
        return;
    }
#endif

    const char *base = SDL_GetBasePath();
    if (base)
    {
        snprintf(out, 1024, "%s%s", base, filename);
    }
    else
    {
        snprintf(out, 1024, "%s", filename);
    }
}
