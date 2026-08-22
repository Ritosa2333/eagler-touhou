#include "MusicRoom.hpp"
#include "AnmManager.hpp"
#include "AsciiManager.hpp"
#include "Chain.hpp"
#include "ChainPriorities.hpp"
#include "Controller.hpp"
#include "FileSystem.hpp"
#include "Localization.hpp"
#include "utils.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

namespace
{
void RenderDescription(MusicRoom *musicRoom)
{
    if (Localization::Active())
    {
        const u32 track = static_cast<u32>(musicRoom->selectedSongIndex + 1);
        const char *title = Localization::MusicTitle(
            track, musicRoom->trackDescriptors[musicRoom->selectedSongIndex].title);
        for (i32 line = 0; line < 8; line++)
        {
            AnmVm &textVm = musicRoom->descriptionSprites[line * 2];
            AnmVm &unusedVm = musicRoom->descriptionSprites[line * 2 + 1];
            const char *fallback = musicRoom->trackDescriptors[musicRoom->selectedSongIndex].description[line];
            const char *text = Localization::MusicComment(track, static_cast<u16>(line), fallback);
            std::string numberedTitle;
            if (std::strcmp(text, "@") == 0)
            {
                char buffer[512];
                std::snprintf(buffer, sizeof(buffer), "No. %2u  %s", track, title);
                numberedTitle = buffer;
                text = numberedTitle.c_str();
            }
            textVm.flags.flag1 = text[0] != '\0';
            unusedVm.flags.flag1 = 0;
            if (textVm.flags.flag1)
                g_AnmManager->DrawVmTextFmt(&textVm, COLOR_MUSIC_ROOM_SONG_DESC_TEXT,
                                            COLOR_MUSIC_ROOM_SONG_DESC_SHADOW, "%s", text);
            textVm.pos = ZunVec3(96.0f, 320.0f + line * 16.0f, 0.0f);
            textVm.flags.anchor = AnmVmAnchor_TopLeft;
        }
        return;
    }

    char lineCharBuffer[64];
    for (i32 index = 0; index < ARRAY_SIZE_SIGNED(musicRoom->descriptionSprites); index++)
    {
        std::memset(lineCharBuffer, 0, sizeof(lineCharBuffer));
        const char *description = musicRoom->trackDescriptors[musicRoom->selectedSongIndex].description[index / 2];
        if (index % 2 == 0 || std::strlen(description) > 32)
            std::memcpy(lineCharBuffer, description + (index % 2) * 32, 32);
        musicRoom->descriptionSprites[index].flags.flag1 = lineCharBuffer[0] != '\0';
        if (lineCharBuffer[0] != '\0')
            g_AnmManager->DrawVmTextFmt(&musicRoom->descriptionSprites[index], COLOR_MUSIC_ROOM_SONG_DESC_TEXT,
                                        COLOR_MUSIC_ROOM_SONG_DESC_SHADOW, "%s", lineCharBuffer);
        musicRoom->descriptionSprites[index].pos =
            ZunVec3((index % 2) * 248.0f + 96.0f, 320.0f + (index / 2) * 16.0f, 0.0f);
        musicRoom->descriptionSprites[index].flags.anchor = AnmVmAnchor_TopLeft;
    }
}
}

ZunResult MusicRoom::CheckInputEnable()
{
    if (this->waitFramesCount >= 8)
    {
        this->enableInput = 1;
    }

    return ZUN_SUCCESS;
}

bool MusicRoom::ProcessInput()
{
    i32 listPos;

    // This variable is never used after this?
    listPos = this->listingOffset;

    if (WAS_PRESSED(TH_BUTTON_UP))
    {
        this->cursor--;
        // Vertical wrap-around
        if (this->cursor < 0)
        {
            this->cursor = this->numDescriptors - 1;
            this->listingOffset = this->numDescriptors - 10;
        }
        // Scroll list up
        else if (this->listingOffset > this->cursor)
        {
            this->listingOffset = this->cursor;
        }
    }

    if (WAS_PRESSED(TH_BUTTON_DOWN))
    {
        this->cursor++;
        // Vertical wrap-around
        if (this->cursor >= this->numDescriptors)
        {
            this->cursor = 0;
            this->listingOffset = 0;
        }
        else
        {
            // Scroll list down
            if (this->listingOffset <= this->cursor - 10)
            {
                this->listingOffset = this->cursor - 9;
            }
        }
    }

    if (WAS_PRESSED(TH_BUTTON_SELECTMENU))
    {
        this->selectedSongIndex = this->cursor;
        g_Supervisor.PlayAudio(this->trackDescriptors[this->selectedSongIndex].path);

        RenderDescription(this);
    }

    if (WAS_PRESSED(TH_BUTTON_RETURNMENU))
    {
        g_Supervisor.curState = SUPERVISOR_STATE_MAINMENU;
        return true;
    }

    return false;
}

static MusicRoom g_MusicRoom;
ZunResult MusicRoom::RegisterChain()
{
    MusicRoom *musicRoom;

    musicRoom = &g_MusicRoom;
    std::memset(musicRoom, 0, sizeof(MusicRoom));

    musicRoom->calc_chain = g_Chain.CreateElem((ChainCallback)MusicRoom::OnUpdate);
    musicRoom->calc_chain->arg = musicRoom;
    musicRoom->calc_chain->addedCallback = (ChainAddedCallback)MusicRoom::AddedCallback;
    musicRoom->calc_chain->deletedCallback = (ChainDeletedCallback)MusicRoom::DeletedCallback;

    if (g_Chain.AddToCalcChain(musicRoom->calc_chain, TH_CHAIN_PRIO_CALC_MAINMENU))
    {
        return ZUN_ERROR;
    }

    musicRoom->draw_chain = g_Chain.CreateElem((ChainCallback)MusicRoom::OnDraw);
    musicRoom->draw_chain->arg = musicRoom;
    g_Chain.AddToDrawChain(musicRoom->draw_chain, TH_CHAIN_PRIO_DRAW_MAINMENU);

    return ZUN_SUCCESS;
}

ChainCallbackResult MusicRoom::OnUpdate(MusicRoom *musicRoom)
{
    musicRoom->mainVm[0].UpdatePrev();
    for (AnmVm &vm : musicRoom->titleSprites)
    {
        vm.UpdatePrev();
    }
    for (AnmVm &vm : musicRoom->descriptionSprites)
    {
        vm.UpdatePrev();
    }
    i32 oldInputSetting = musicRoom->enableInput;
    for (;;)
    {
        switch (musicRoom->enableInput)
        {
        case false:
            if (!musicRoom->CheckInputEnable())
            {
                break;
            }

            continue;

        case true:
            if (musicRoom->ProcessInput())
            {
                return CHAIN_CALLBACK_RESULT_CONTINUE_AND_REMOVE_JOB;
            }
        }
        break;
    }

    if (oldInputSetting != musicRoom->enableInput)
    {
        musicRoom->waitFramesCount = 0;
    }
    else
    {
        musicRoom->waitFramesCount++;
    }
    g_AnmManager->ExecuteScript(musicRoom->mainVm);
    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

ChainCallbackResult MusicRoom::OnDraw(MusicRoom *musicRoom)
{
    i32 i;
    ZunVec3 textPos;
    char rightArrowStr[4];

    rightArrowStr[0] = TEXT_RIGHT_ARROW;
    rightArrowStr[1] = '\0';

    g_AnmManager->SetCurrentTexture(0);
    g_AnmManager->CopySurfaceToBackBuffer(0, 0, 0, 0, 0);
    g_AnmManager->DrawInterpNoRotation(musicRoom->mainVm);

    // Draw the 10 songs in the song select window, and list indices
    for (i = musicRoom->listingOffset; i < musicRoom->listingOffset + 10; i++)
    {
        if (musicRoom->cursor != i)
        {
            musicRoom->titleSprites[i].color = COLOR_SET_ALPHA(COLOR_GREY, 0xe0);
            g_AsciiManager.color = COLOR_SET_ALPHA(COLOR_GREY, 0xe0);
        }
        else
        {
            musicRoom->titleSprites[i].color = COLOR_WHITE;
            g_AsciiManager.color = COLOR_WHITE;
        }

        musicRoom->titleSprites[i].pos.x = 93.0f;
        musicRoom->titleSprites[i].pos.y = 104.0f + (((i + 1) - musicRoom->listingOffset) * 18) - 20.0f;
        musicRoom->titleSprites[i].pos.z = 0.0f;
        g_AnmManager->DrawNoRotation(&musicRoom->titleSprites[i]);

        textPos = musicRoom->titleSprites[i].pos;
        textPos.x -= 60.0f;

        if (musicRoom->cursor == i)
        {
            g_AsciiManager.AddString(&textPos, rightArrowStr);
        }

        textPos.x += 15.0f;
        g_AsciiManager.AddFormatText(&textPos, "%2d.", i + 1);
    }

    i++; // ???

    for (i = 0; i < ARRAY_SIZE_SIGNED(musicRoom->descriptionSprites); i++)
    {
        g_AnmManager->DrawInterpNoRotation(&musicRoom->descriptionSprites[i]);
    }

    g_AsciiManager.color = COLOR_WHITE;

    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

ZunResult MusicRoom::AddedCallback(MusicRoom *musicRoom)
{
    u32 charIndex;
    char *currChar;
    char *fileBase;
    i32 i;
    i32 lineIndex;

    if (g_AnmManager->LoadSurface(0, "data/result/music.jpg") != ZUN_SUCCESS)
    {
        return ZUN_ERROR;
    }

    if (g_AnmManager->LoadAnm(ANM_FILE_MUSIC00, "data/music00.anm", ANM_OFFSET_MUSIC00) != ZUN_SUCCESS)
    {
        return ZUN_ERROR;
    }

    if (g_AnmManager->LoadAnm(ANM_FILE_MUSIC01, "data/music01.anm", ANM_OFFSET_MUSIC01) != ZUN_SUCCESS)
    {
        return ZUN_ERROR;
    }

    if (g_AnmManager->LoadAnm(ANM_FILE_MUSIC02, "data/music02.anm", ANM_OFFSET_MUSIC02) != ZUN_SUCCESS)
    {
        return ZUN_ERROR;
    }

    g_AnmManager->SetAndExecuteScriptIdx(musicRoom->mainVm, ANM_OFFSET_MUSIC00);
    musicRoom->waitFramesCount = 0;
    currChar = (char *)FileSystem::OpenPath("data/musiccmt.txt", 0);
    fileBase = currChar;

    if (currChar == NULL)
    {
        return ZUN_ERROR;
    }

    musicRoom->trackDescriptors = new TrackDescriptor[ARRAY_SIZE_SIGNED(musicRoom->titleSprites)]();

    i = -1;
    while (currChar - fileBase < (i32)g_LastFileSize)
    {
        if (*currChar == '@')
        {
            currChar++;
            i++;
            charIndex = 0;

            while (*currChar != '\n' && *currChar != '\r')
            {
                musicRoom->trackDescriptors[i].path[charIndex] = *currChar;
                currChar++;
                charIndex++;
                if (currChar - fileBase >= (i32)g_LastFileSize)
                {
                    goto finishMusiccmtRead;
                }
            }

            while (*currChar == '\n' || *currChar == '\r')
            {
                currChar++;
                if (currChar - fileBase >= (i32)g_LastFileSize)
                {
                    goto finishMusiccmtRead;
                }
            }

            charIndex = 0;
            while (*currChar != '\n' && *currChar != '\r')
            {
                musicRoom->trackDescriptors[i].title[charIndex] = *currChar;
                currChar++;
                charIndex++;
                if (currChar - fileBase >= (i32)g_LastFileSize)
                {
                    goto finishMusiccmtRead;
                }
            }

            // Dead code. Is it a bug? Was it an intentional quick and dirty change? Who knows?
            // Has the effect of offsetting the description text by a line
            while (*currChar == '\n' && *currChar == '\r')
            {
                currChar++;
                if (currChar - fileBase >= (i32)g_LastFileSize)
                {
                    goto finishMusiccmtRead;
                }
            }

            for (lineIndex = 0; lineIndex < 8; lineIndex++)
            {
                if (*currChar == '@')
                {
                    break;
                }

                std::memset(musicRoom->trackDescriptors[i].description[lineIndex], 0,
                            sizeof(musicRoom->trackDescriptors[i].description[lineIndex]));
                charIndex = 0;
                while (*currChar != '\n' && *currChar != '\r')
                {
                    musicRoom->trackDescriptors[i].description[lineIndex][charIndex] = *currChar;
                    currChar++;
                    charIndex++;
                    if (currChar - fileBase >= (i32)g_LastFileSize)
                    {
                        goto finishMusiccmtRead;
                    }
                }

                while (*currChar == '\n' || *currChar == '\r')
                {
                    currChar++;
                    if (currChar - fileBase >= (i32)g_LastFileSize)
                    {
                        goto finishMusiccmtRead;
                    }
                }
            }
        }
        else
        {
            currChar++;
        }
    }

finishMusiccmtRead:
    musicRoom->numDescriptors = i + 1;

    for (i = 0; i < musicRoom->numDescriptors; i++)
    {
        g_AnmManager->InitializeAndSetSprite(&musicRoom->titleSprites[i], ANM_OFFSET_MUSIC01 + i);
        const char *displayTitle = Localization::MusicTitle(
            static_cast<u32>(i + 1), musicRoom->trackDescriptors[i].title);
        g_AnmManager->DrawVmTextFmt(&musicRoom->titleSprites[i], COLOR_MUSIC_ROOM_SONG_TITLE_TEXT,
                                    COLOR_MUSIC_ROOM_SONG_TITLE_SHADOW, "%s",
                                    displayTitle);
        musicRoom->titleSprites[i].pos.x = 93.0f;
        musicRoom->titleSprites[i].pos.y = 104.0f + ((i + 1) * 18) - 20.0f;
        musicRoom->titleSprites[i].pos.z = 0.0f;
        musicRoom->titleSprites[i].flags.anchor = AnmVmAnchor_TopLeft;
    }

    // Vanilla uses two sprites per line, split at byte 32. Localized UTF-8 text
    // uses one wide sprite per line, matching thcrap's widened text surface.
    for (i = 0; i < ARRAY_SIZE_SIGNED(musicRoom->descriptionSprites); i++)
    {
        g_AnmManager->InitializeAndSetSprite(&musicRoom->descriptionSprites[i], ANM_SCRIPT_TEXT_MUSIC_ROOM_DESC + i);
    }
    RenderDescription(musicRoom);

    std::free(fileBase);

    return ZUN_SUCCESS;
}

ZunResult MusicRoom::DeletedCallback(MusicRoom *musicRoom)
{
    delete[] musicRoom->trackDescriptors;
    musicRoom->trackDescriptors = NULL;

    g_AnmManager->ReleaseSurface(0);
    g_AnmManager->ReleaseAnm(ANM_FILE_MUSIC00);
    g_AnmManager->ReleaseAnm(ANM_FILE_MUSIC01);
    g_AnmManager->ReleaseAnm(ANM_FILE_MUSIC02);
    g_Chain.Cut(musicRoom->draw_chain);
    musicRoom->draw_chain = NULL;

    return ZUN_SUCCESS;
}
