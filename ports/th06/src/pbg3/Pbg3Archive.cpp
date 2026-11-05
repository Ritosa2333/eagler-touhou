#include <cstddef>
#include <cstdlib>
#include <cstring>

#include "pbg3/Pbg3Archive.hpp"

Pbg3Archive **g_Pbg3Archives;

Pbg3Archive::Pbg3Archive()
{
    this->fileTableOffset = 0;
    this->numOfEntries = 0;
    this->entries = NULL;
    this->parser = NULL;
    this->unk = NULL;
}

i32 Pbg3Archive::ParseHeader()
{
    if (this->parser->ReadMagic() != 0x33474250)
    {
        if (this->parser != NULL)
        {
            delete this->parser;
            this->parser = NULL;
        }
        return false;
    }

    this->numOfEntries = this->parser->ReadVarInt();
    this->fileTableOffset = this->parser->ReadVarInt();
    if (this->numOfEntries == 0 || this->numOfEntries > 65536 ||
        this->fileTableOffset >= this->parser->GetFileSize())
    {
        delete this->parser;
        this->parser = NULL;
        return false;
    }
    if (!this->parser->SeekToOffset(this->fileTableOffset))
    {
        if (this->parser != NULL)
        {
            delete this->parser;
            this->parser = NULL;
        }
        return false;
    }

    this->entries = new Pbg3Entry[this->numOfEntries];
    if (this->entries == NULL)
    {
        if (this->parser != NULL)
        {
            delete this->parser;
            this->parser = NULL;
        }
        return false;
    }

    for (u32 idx = 0; idx < this->numOfEntries; idx += 1)
    {
        this->entries[idx].unk2 = this->parser->ReadVarInt();
        this->entries[idx].unk1 = this->parser->ReadVarInt();
        this->entries[idx].checksum = this->parser->ReadVarInt();
        this->entries[idx].dataOffset = this->parser->ReadVarInt();
        this->entries[idx].uncompressedSize = this->parser->ReadVarInt();
        if (this->entries[idx].dataOffset >= this->fileTableOffset ||
            this->entries[idx].uncompressedSize == 0 ||
            (idx != 0 && this->entries[idx - 1].dataOffset >= this->entries[idx].dataOffset))
        {
            this->Release();
            return false;
        }
        if (!this->parser->ReadString(this->entries[idx].filename, sizeof(this->entries[idx].filename)))
        {
            if (this->parser != NULL)
            {
                delete this->parser;
                this->parser = NULL;
            }
            if (this->entries != NULL)
            {
                delete[] this->entries;
                this->entries = NULL;
            }

            return false;
        }
    }

    return true;
}

i32 Pbg3Archive::Release()
{
    this->fileTableOffset = 0;
    this->numOfEntries = 0;
    if (this->parser != NULL)
    {
        delete this->parser;
        this->parser = NULL;
    }
    if (this->entries != NULL)
    {
        delete[] this->entries;
        this->entries = NULL;
    }
    std::free(this->unk);
    this->unk = NULL;
    return true;
}

i32 Pbg3Archive::FindEntry(const char *path)
{
    for (u32 entryIdx = 0; entryIdx < this->numOfEntries; entryIdx += 1)
    {
        char *entryFilename = this->entries[entryIdx].filename;
        i32 res = std::strcmp(path, entryFilename);
        if (res == 0)
        {
            return entryIdx;
        }
    }
    return -1;
}

u32 Pbg3Archive::GetEntrySize(u32 entryIdx)
{
    if (entryIdx >= this->numOfEntries)
    {
        return 0;
    }

    return this->entries[entryIdx].uncompressedSize;
}

u8 *Pbg3Archive::ReadEntryRaw(u32 *outSize, u32 *outChecksum, i32 entryIdx)
{
    if (this->parser == NULL)
    {
        return NULL;
    }

    if (entryIdx >= this->numOfEntries)
        return NULL;

    if (outSize == NULL)
        return NULL;

    if (outChecksum == NULL)
        return NULL;

    if (!this->parser->SeekToOffset(this->entries[entryIdx].dataOffset))
        return NULL;

    u32 size;
    const u32 dataOffset = this->entries[entryIdx].dataOffset;
    const u32 nextOffset = entryIdx == this->numOfEntries - 1 ? this->fileTableOffset
                                                              : this->entries[entryIdx + 1].dataOffset;
    if (nextOffset <= dataOffset || nextOffset > this->fileTableOffset)
    {
        return NULL;
    }
    size = nextOffset - dataOffset;

    u8 *data = (u8 *)malloc(size);
    if (data == NULL)
        return NULL;

    if (!this->parser->ReadByteAlignedData(data, size))
    {
        free(data);
        return NULL;
    }

    *outChecksum = this->entries[entryIdx].checksum;
    *outSize = size;
    return data;
}

Pbg3Archive::~Pbg3Archive()
{
    this->Release();
}

i32 Pbg3Archive::Load(const char *path)
{
    if (!this->Release())
    {
        return false;
    }

    this->parser = new Pbg3Parser();
    if (this->parser == NULL)
    {
        return false;
    }

    if (!this->parser->OpenArchive(path))
    {
        if (this->parser != NULL)
        {
            delete this->parser;
            this->parser = NULL;
        }
        return false;
    }

    return this->ParseHeader();
}

#define LZSS_DICTSIZE 0x2000
#define LZSS_DICTSIZE_MASK 0x1fff

u8 *Pbg3Archive::ReadDecompressEntry(u32 entryIdx, const char *filename)
{
    if (entryIdx >= this->numOfEntries || this->parser == NULL)
        return NULL;

    const u32 outputSize = this->GetEntrySize(entryIdx);
    u8 *out = (u8 *)malloc(outputSize);
    if (out == NULL)
        return NULL;

    u32 expectedCsum;
    u32 size = 0;
    u8 *rawData = this->ReadEntryRaw(&size, &expectedCsum, entryIdx);

    if (rawData == NULL)
    {
        if (out != NULL)
        {
            free(out);
            out = NULL;
        }
        return NULL;
    }

    size_t inputOffset = 0;
    u8 inBitMask = 0;
    u8 currByte = 0;
    u32 checksum = 0;
    u32 dictHead = 1;
    u32 outputOffset = 0;

    u8 dict[LZSS_DICTSIZE];

    // Memset doesn't produce matching assembly
    for (i32 i = 0; i < LZSS_DICTSIZE; i++)
    {
        dict[i] = 0;
    }

    auto readBit = [&](u32 &bit) -> bool {
        if (inBitMask == 0)
        {
            if (inputOffset >= size)
            {
                return false;
            }
            currByte = rawData[inputOffset++];
            checksum += currByte;
            inBitMask = 0x80;
        }
        bit = (currByte & inBitMask) != 0;
        inBitMask >>= 1;
        return true;
    };

    auto readBits = [&](u32 count, u32 &value) -> bool {
        value = 0;
        for (u32 i = 0; i < count; ++i)
        {
            u32 bit = 0;
            if (!readBit(bit))
            {
                return false;
            }
            value = (value << 1) | bit;
        }
        return true;
    };

    auto writeByte = [&](u8 value) -> bool {
        if (outputOffset >= outputSize)
        {
            return false;
        }
        out[outputOffset++] = value;
        dict[dictHead] = value;
        dictHead = (dictHead + 1) & LZSS_DICTSIZE_MASK;
        return true;
    };

    for (;;)
    {
        u32 opcode = 0;
        if (!readBit(opcode))
        {
            free(rawData);
            free(out);
            return NULL;
        }

        if (opcode != 0)
        {
            u32 literal = 0;
            if (!readBits(8, literal) || !writeByte(static_cast<u8>(literal)))
            {
                free(rawData);
                free(out);
                return NULL;
            }
        }
        else
        {
            u32 matchOffset = 0;
            if (!readBits(13, matchOffset))
            {
                free(rawData);
                free(out);
                return NULL;
            }
            if (matchOffset == 0)
            {
                break;
            }

            u32 matchLength = 0;
            if (!readBits(4, matchLength))
            {
                free(rawData);
                free(out);
                return NULL;
            }
            for (u32 i = 0; i < matchLength + 3; ++i)
            {
                if (!writeByte(dict[(matchOffset + i) & LZSS_DICTSIZE_MASK]))
                {
                    free(rawData);
                    free(out);
                    return NULL;
                }
            }
        }
    }

    free(rawData);

    if (expectedCsum != checksum || outputOffset != outputSize)
    {
        if (out != NULL)
        {
            free(out);
            out = NULL;
        }
        return NULL;
    }

    return out;
}
