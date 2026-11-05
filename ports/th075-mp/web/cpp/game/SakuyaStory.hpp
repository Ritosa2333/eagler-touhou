#pragma once
#include "StorySequence.hpp"
namespace th075 {
void initialize_sakuya_story(FighterActor&,FighterActor&,StoryCutscene&,StoryEnvironment&);
void advance_sakuya_story(FighterActor&,FighterActor&,StoryCutscene&,i16&,StoryEnvironment&,StoryEvents&);
}
