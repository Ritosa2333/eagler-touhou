#pragma once
#include "StorySequence.hpp"
namespace th075 {
void initialize_marisa_story(FighterActor&,FighterActor&,StoryCutscene&,StoryEnvironment&);
void advance_marisa_story(FighterActor&,FighterActor&,StoryCutscene&,i16&,StoryEnvironment&,StoryEvents&);
}
