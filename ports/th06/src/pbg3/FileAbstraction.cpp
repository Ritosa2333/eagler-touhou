#include "pbg3/FileAbstraction.hpp"
#include "FileSystem.hpp"
#include <cstdio>
#include <new>

FileAbstraction::FileAbstraction()
{
    handle = NULL;
    access = ACCESS_INVALID;
}

i32 FileAbstraction::Open(const char *filename, const char *mode)
{
    char openMode[] = "*b";

    this->Close();

    const char *curMode;
    for (curMode = mode; *curMode != '\0'; curMode += 1)
    {
        if (*curMode == 'r')
        {
            this->access = ACCESS_READ;
            openMode[0] = 'r';
            break;
        }
        else if (*curMode == 'w')
        {
            this->access = ACCESS_WRITE;
            openMode[0] = 'w';
            break;
        }
        else if (*curMode == 'a')
        {
            this->access = ACCESS_WRITE;
            openMode[0] = 'a';
            break;
        }
    }

    if (*curMode == '\0')
    {
        return 0;
    }

    this->handle = FileSystem::OpenFileStream(filename, openMode);

    if (this->handle == NULL)
        return 0;

    return 1;
}

void FileAbstraction::Close()
{
    if (this->handle != NULL)
    {
        SDL_CloseIO(this->handle);
        this->handle = NULL;
        this->access = ACCESS_INVALID;
    }
}

i32 FileAbstraction::Read(u8 *data, u32 dataLen, u32 *numBytesRead)
{
    if (this->access != ACCESS_READ || this->handle == NULL || numBytesRead == NULL ||
        (data == NULL && dataLen != 0))
    {
        return false;
    }

    *numBytesRead = SDL_ReadIO(this->handle, data, dataLen);

    return !(dataLen != 0 && *numBytesRead < dataLen);
}

i32 FileAbstraction::Write(const u8 *data, u32 dataLen, u32 *outWritten)
{
    if (this->access != ACCESS_WRITE || this->handle == NULL || outWritten == NULL ||
        (data == NULL && dataLen != 0))
    {
        return false;
    }

    *outWritten = SDL_WriteIO(this->handle, data, dataLen);

    return !(dataLen != 0 && *outWritten < dataLen);
}

i32 FileAbstraction::ReadByte()
{
    u8 data;
    u32 outBytesRead;

    if (!this->Read(&data, 1, &outBytesRead))
    {
        return -1;
    }
    else
    {
        if (outBytesRead == 0)
        {
            return -1;
        }
        return data;
    }
}

i32 FileAbstraction::WriteByte(u32 b)
{
    u8 outByte;
    u32 outBytesWritten;

    outByte = b;
    if (!this->Write(&outByte, 1, &outBytesWritten))
    {
        return -1;
    }
    else
    {
        if (outBytesWritten == 0)
        {
            return -1;
        }
        return b;
    }
}

i32 FileAbstraction::Seek(u32 amount, u32 seekFrom)
{
    if (this->handle == NULL)
    {
        return 0;
    }

    SDL_IOWhence whence = SDL_IO_SEEK_SET;
    if (seekFrom == SEEK_CUR) whence = SDL_IO_SEEK_CUR;
    else if (seekFrom == SEEK_END) whence = SDL_IO_SEEK_END;
    return SDL_SeekIO(this->handle, amount, whence) >= 0;
}

u32 FileAbstraction::Tell()
{
    if (this->handle == NULL)
    {
        return 0;
    }

    const Sint64 position = SDL_TellIO(this->handle);
    return position < 0 ? 0 : static_cast<u32>(position);
}

u32 FileAbstraction::GetSize()
{
    if (this->handle == NULL)
    {
        return 0;
    }

    const Sint64 size = SDL_GetIOSize(this->handle);
    return size < 0 || static_cast<Uint64>(size) > UINT32_MAX ? 0 : static_cast<u32>(size);
}

u8 *FileAbstraction::ReadWholeFile(u32 maxSize)
{
    if (this->access != ACCESS_READ)
    {
        return NULL;
    }

    u32 dataLen = this->GetSize();
    u32 outDataLen;
    if (dataLen <= maxSize)
    {
        u8 *data = new (std::nothrow) u8[dataLen == 0 ? 1 : dataLen];
        if (data != NULL)
        {
            u32 oldLocation = this->Tell();
            if (this->Seek(0, SEEK_SET) != 0)
            {
                if (this->Read(data, dataLen, &outDataLen) == 0 || outDataLen != dataLen)
                {
                    delete[] data;
                    return NULL;
                }
                this->Seek(oldLocation, SEEK_SET);
                return data;
            }
            delete[] data;
        }
    }
    return NULL;
}

FileAbstraction::~FileAbstraction()
{
    this->Close();
}
