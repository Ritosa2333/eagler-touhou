#include "dialogue_renderer.hpp"
#include "bomb_renderer.hpp"
#include <chrono>
#include <future>
#include <iostream>

namespace {
unsigned checks=0;
void check(bool value){++checks;if(!value)throw std::runtime_error("asset cache check failed="+std::to_string(checks));}
struct Fixture {
    std::filesystem::path root=std::filesystem::current_path()/("soew-asset-cache-"+std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
    Fixture(){if(!std::filesystem::create_directory(root))throw std::runtime_error("Cannot create cache fixture");}
    ~Fixture(){std::error_code ec;std::filesystem::remove_all(root,ec);}
};
void write(const std::filesystem::path &path,const std::vector<std::byte> &bytes){
    std::ofstream stream(path,std::ios::binary|std::ios::trunc);
    if(!stream.write(reinterpret_cast<const char*>(bytes.data()),static_cast<std::streamsize>(bytes.size())))throw std::runtime_error("Cannot write cache fixture");
}
void fake_fixture(){
    Fixture fixture;const auto path=fixture.root/"MIKO_K.MPN";
    std::vector<std::byte> bytes(54+185*128);
    bytes[0]=std::byte{'M'};bytes[1]=std::byte{'P'};bytes[2]=std::byte{'T'};bytes[3]=std::byte{'N'};bytes[4]=std::byte{184};write(path,bytes);
    const auto start=soew::DialogueRenderer::cache_stats();
    auto first=soew::DialogueRenderer::load(fixture.root);first.tiles[0][0]=15;
    auto second=soew::DialogueRenderer::load(fixture.root/".");
    auto stats=soew::DialogueRenderer::cache_stats();
    check(second.tiles[0][0]==0);check(stats.decodes==start.decodes+1);check(stats.hits==start.hits+1);check(stats.entries<=1);
    const auto modified=std::filesystem::last_write_time(path);
    bytes[54]=std::byte{128};write(path,bytes);std::filesystem::last_write_time(path,modified);
    auto changed=soew::DialogueRenderer::load(fixture.root);
    check(changed.tiles[0][0]==1);check(soew::DialogueRenderer::cache_stats().decodes==stats.decodes+1);
    bytes[54]=std::byte{0};write(path,bytes);std::filesystem::last_write_time(path,modified);
    const auto before=soew::DialogueRenderer::cache_stats();
    std::vector<std::future<soew::DialogueRenderer>> pending;
    for(unsigned i=0;i<8;++i)pending.push_back(std::async(std::launch::async,[&]{return soew::DialogueRenderer::load(fixture.root);}));
    for(auto &job:pending)check(job.get().tiles[0][0]==0);
    stats=soew::DialogueRenderer::cache_stats();check(stats.decodes==before.decodes+1);check(stats.hits==before.hits+7);
    bytes[0]=std::byte{0};write(path,bytes);bool rejected=false;
    try{soew::DialogueRenderer::load(fixture.root);}catch(const std::runtime_error&){rejected=true;}
    check(rejected);check(soew::DialogueRenderer::cache_stats().decodes==stats.decodes);
    bytes[0]=std::byte{'M'};write(path,bytes);
    check(soew::DialogueRenderer::load(fixture.root).tiles[0][0]==0);
    soew::asset_cache_detail::Cache<unsigned,2> bounded;
    for(unsigned i=0;i<4;++i){const auto file=fixture.root/("fixture"+std::to_string(i));write(file,bytes);
        const std::vector<soew::asset_cache_detail::Input> inputs={{file,bytes}};
        check(*bounded.get(inputs,[=]{return i;})==i);check(bounded.stats().entries<=2);}
}
void original_resources(const std::filesystem::path &root){
    auto first=soew::StageScene::load(root,0,0,1,2,3);
    const auto terrain=soew::StageScene::cache_stats(),art=soew::StageScene::player_art_cache_stats();
    first.ticks=99;first.paused=true;first.background.tiles[0][0]^=1;
    auto fresh=soew::StageScene::load(root,0,2,3,7,8);
    check(fresh.ticks==0&&!fresh.paused&&fresh.stage==0&&fresh.rank==3&&fresh.lives==7&&fresh.bombs==8);
    check(first.background.tiles[0][0]!=fresh.background.tiles[0][0]);
    check(soew::StageScene::cache_stats().decodes==terrain.decodes);check(soew::StageScene::cache_stats().hits==terrain.hits+1);
    check(soew::StageScene::player_art_cache_stats().decodes==art.decodes);
    auto sprites=soew::StageSprites::load(root,0);const auto bank=soew::StageSprites::cache_stats();sprites.sprites[0]->pixels[0]^=1;
    auto fresh_sprites=soew::StageSprites::load(root,0);check(sprites.sprites[0]->pixels[0]!=fresh_sprites.sprites[0]->pixels[0]);check(soew::StageSprites::cache_stats().decodes==bank.decodes);
    for(unsigned shot=0;shot<3;++shot){auto bomb=soew::BombRenderer::load(root,shot);const auto before=soew::BombRenderer::cache_stats();bomb.portrait[0]^=1;
        auto fresh_bomb=soew::BombRenderer::load(root,shot);check(bomb.portrait[0]!=fresh_bomb.portrait[0]);check(soew::BombRenderer::cache_stats().decodes==before.decodes);}
    check(soew::BombRenderer::cache_stats().entries<=3);check(soew::StageSprites::cache_stats().entries<=6);check(soew::StageScene::cache_stats().entries<=6);
}
}
int main(int argc,char **argv){try{fake_fixture();if(argc>1)original_resources(argv[1]);std::cout<<"asset_cache_checks="<<checks<<" failures=0\n";return 0;}catch(const std::exception &error){std::cerr<<error.what()<<'\n';return 1;}}
