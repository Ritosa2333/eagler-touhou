#include "GameErrorContext.hpp"
#include "FileSystem.hpp"
#include <SDL3/SDL.h>
#include <cstdarg>
#include <cstdio>
#include <cstring>

GameErrorContext g_GameErrorContext;

const char *GameErrorContext::Log(const char *fmt, ...)
{
    char tmpBuffer[512];
    size_t tmpBufferSize;
    va_list args;

    va_start(args, fmt);
    std::vsnprintf(tmpBuffer, sizeof(tmpBuffer), fmt, args);

    tmpBufferSize = std::strlen(tmpBuffer);

    if (this->m_BufferEnd + tmpBufferSize < &this->m_Buffer[sizeof(this->m_Buffer) - 1])
    {
        std::strcpy(this->m_BufferEnd, tmpBuffer);

        this->m_BufferEnd += tmpBufferSize;
        *this->m_BufferEnd = '\0';
    }

    va_end(args);

    return fmt;
}

const char *GameErrorContext::Fatal(const char *fmt, ...)
{
    char tmpBuffer[512];
    size_t tmpBufferSize;
    va_list args;

    va_start(args, fmt);
    std::vsnprintf(tmpBuffer, sizeof(tmpBuffer), fmt, args);

    tmpBufferSize = std::strlen(tmpBuffer);

    if (this->m_BufferEnd + tmpBufferSize < &this->m_Buffer[sizeof(this->m_Buffer) - 1])
    {
        std::strcpy(this->m_BufferEnd, tmpBuffer);

        this->m_BufferEnd += tmpBufferSize;
        *this->m_BufferEnd = '\0';
    }

    va_end(args);

    this->m_ShowMessageBox = true;

    return fmt;
}

void GameErrorContext::Flush()
{
    FILE *logFile;

    if (m_BufferEnd != m_Buffer)
    {
        g_GameErrorContext.Log(TH_ERR_LOGGER_END);

        if (m_ShowMessageBox)
        {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "log", m_Buffer, NULL);
        }

        logFile = FileSystem::FopenUTF8("./log.txt", "w");

        if (logFile != NULL)
        {
            std::fprintf(logFile, "%s", m_Buffer);
            std::fclose(logFile);
        }
    }
}
