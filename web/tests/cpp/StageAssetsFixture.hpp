#pragma once
#include "../../cpp/game/StageAssets.hpp"
struct AssetSourceFixture:AssetSource {
    std::unordered_map<std::string,std::vector<u8>> files;
    bool read(const std::string& name,std::vector<u8>& out)override{auto found=files.find(name);if(found==files.end())return false;out=found->second;return true;}
};
struct StageAssetsFixture {StageAssets assets;explicit StageAssetsFixture(AssetSourceFixture& s,AnmManagerFixture& a,StageAssetsFixture* common=nullptr):assets(s,a.manager,common?&common->assets:nullptr){};};
API(asset_source_create) AssetSourceFixture* asset_source_create(){return new AssetSourceFixture;}
API(asset_source_delete) void asset_source_delete(AssetSourceFixture* p){delete p;}
API(asset_source_add) void asset_source_add(AssetSourceFixture* p,const char* name,const u8* bytes,u32 size){p->files[name]=std::vector<u8>(bytes,bytes+size);}
API(stage_assets_create) StageAssetsFixture* stage_assets_create(AssetSourceFixture* s,AnmManagerFixture* a,StageAssetsFixture* common){return new StageAssetsFixture(*s,*a,common);}
API(stage_assets_delete) void stage_assets_delete(StageAssetsFixture* p){delete p;}
API(stage_assets_load) u32 stage_assets_load(StageAssetsFixture* p,i32 stage,i32 character,i32 animal,u32 ascii){return p->assets.load(stage,character,animal,ascii==1?"ascii_960.anm":ascii==2?"ascii_1280.anm":"ascii.anm");}
API(stage_assets_error) const char* stage_assets_error(StageAssetsFixture* p){return p->assets.error.c_str();}
API(stage_assets_program) EclProgram* stage_assets_program(StageAssetsFixture* p){return &p->assets.scripts;}
API(stage_assets_shots) ShtResource* stage_assets_shots(StageAssetsFixture* p){return &p->assets.shots;}
API(stage_assets_messages) MessageResource* stage_assets_messages(StageAssetsFixture* p){return &p->assets.messages;}
API(stage_assets_background) StageResource* stage_assets_background(StageAssetsFixture* p){return &p->assets.background;}
API(stage_assets_enemy_banks) const i32* stage_assets_enemy_banks(StageAssetsFixture* p){return p->assets.enemy_banks.data();}
API(stage_assets_bank_name) const char* stage_assets_bank_name(StageAssetsFixture* p,i32 bank){for(const auto& entry:p->assets.loaded_animations())if(entry.second==bank)return entry.first.c_str();return "";}
API(stage_assets_bank) i32 stage_assets_bank(StageAssetsFixture* p,u32 kind){const auto& a=p->assets;const i32 banks[]={a.ascii,a.front,a.text,a.bullet,a.effect,a.beast,a.player,a.scenery,a.logo};return kind<9?banks[kind]:-1;}
