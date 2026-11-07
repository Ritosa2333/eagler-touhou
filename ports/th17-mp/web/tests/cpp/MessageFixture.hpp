#pragma once
#include "../../cpp/game/MessageResource.hpp"
#include "../../cpp/game/StageConfiguration.hpp"
API(message_create) MessageResource* message_create(){return new MessageResource;}
API(message_delete) void message_delete(MessageResource* p){delete p;}
API(message_open) u32 message_open(MessageResource* p,const u8* data,u32 size){return p->open(data,size);}
API(message_error) const char* message_error(MessageResource* p){return p->error.c_str();}
API(message_count) u32 message_count(MessageResource* p){return p->scripts.size();}
API(message_entry) u32 message_entry(MessageResource* p,u32 index,u32 field){if(index>=p->scripts.size())return 0;const auto& s=p->scripts[index];return field==0?s.offset:field==1?s.flags:s.instructions.size();}
API(message_instruction) const MessageInstruction* message_instruction(MessageResource* p,u32 entry,u32 index){return entry<p->scripts.size()&&index<p->scripts[entry].instructions.size()?&p->scripts[entry].instructions[index]:nullptr;}
API(message_instruction_field) u32 message_instruction_field(const MessageInstruction* p,u32 field){switch(field){case 0:return p->offset;case 1:return p->time;case 2:return p->opcode;case 3:return p->data.size();case 4:return p->text.size();default:return 0;}}
API(message_instruction_data) const u8* message_instruction_data(const MessageInstruction* p){return p->data.data();}
API(message_instruction_text) const char* message_instruction_text(const MessageInstruction* p){return p->text.c_str();}
API(stage_config_name) const char* stage_config_name(i32 stage,u32 field){const auto* s=StageConfiguration::get(stage);if(!s)return nullptr;switch(field){case 0:return s->background.c_str();case 1:return s->enemy_script.c_str();case 2:return s->stage_music.c_str();case 3:return s->boss_music.c_str();case 13:return s->logo.c_str();default:return field>=4&&field<13?s->messages[field-4].c_str():nullptr;}}
API(stage_config_value) i32 stage_config_value(i32 stage,u32 field){const auto* s=StageConfiguration::get(stage);if(!s)return -1;switch(field){case 0:return s->stage;case 1:return s->stage_music_index;case 2:return s->boss_music_index;case 3:return s->flags;default:return -1;}}
API(stage_config_visuals) const StageVisualSet* stage_config_visuals(i32 stage){const auto* s=StageConfiguration::get(stage);return s?s->visuals.data():nullptr;}
