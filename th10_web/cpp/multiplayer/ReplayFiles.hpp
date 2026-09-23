#pragma once
#include "ReplayArchive.hpp"
#include "../platform/ReplayFiles.hpp"

namespace th10::browser {struct GameState;}
namespace th10::multiplayer {
bool ReadReplayFile(browser::FileSystem&,const char* file,std::vector<u8>& bytes);
i32 LoadReplayPreview(browser::ReplayDocument&,const char* name);
bool SaveReplayFile(browser::FileSystem&,browser::ReplayCalendar&,browser::GameState&,
                    const char* file,const char* name,i32 score,i32 scoreUnits,i32 lastStage,
                    u32 frameCount=Netplay::INVALID_FRAME);
}
