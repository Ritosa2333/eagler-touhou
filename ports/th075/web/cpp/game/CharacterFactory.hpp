#pragma once
#include "CharacterController.hpp"
namespace th075 {
enum class CharacterId:u8 {Reimu,Marisa,Sakuya,Alice,Patchouli,Youmu,Remilia,Yuyuko,Yukari,Suika,Meiling};
enum class ResourceEdition:u8 {Base,Patched};
struct CharacterDefinition {
 CharacterId id;const char* name;const char* asset;u16 spellPortrait;u8 portraitCount;
 ResourceEdition sounds,sprites,portraits;std::array<u8,10> storyOpponents;
 std::string pattern_path()const;std::string script_path()const;std::string sprite_path()const;std::string portrait_path()const;std::string sound_path()const;
};
const CharacterDefinition* character_definition(u8);
std::unique_ptr<CharacterController> create_character(u8);
}
