#include "stage_scene.hpp"
#include <iostream>
#include <set>
unsigned checks=0;void check(bool b){++checks;if(!b)throw std::runtime_error("Stage scene contract failed");}
int wmain(int argc,wchar_t**argv){try{
    if(argc!=2)throw std::runtime_error("Usage: soew-stage-scene-tests <assets>");
    const std::filesystem::path root=argv[1];std::vector<std::uint32_t> pixels(640*400);std::set<std::uint32_t> scene_hashes;
    for(int id=0;id<6;++id)for(int shot=0;shot<3;++shot){
        auto scene=soew::StageScene::load(root,id,shot,id==5?4:1,2,3);check(scene.player.x==208&&scene.player.y==336);scene.render(pixels);
        std::uint32_t hash=2166136261u;for(auto c:pixels){hash^=c;hash*=16777619u;}if(shot==0)scene_hashes.insert(hash);
        check(pixels[0]==0&&pixels[399*640+639]==0);check(scene.background.map==scene.script.map);check(scene.script.complete()==(id!=3));
        auto duplicate=scene;for(unsigned i=0;i<20;++i){scene.tick(soew::Up|soew::Right);duplicate.tick(soew::Up|soew::Right);}check(scene.player.x==duplicate.player.x&&scene.player.y==duplicate.player.y);scene.render(pixels);check(scene.ticks==20);
        scene.paused=true;const int x=scene.player.x,y=scene.player.y;for(int i=0;i<50;++i)scene.tick(soew::Down);check(scene.player.x==x&&scene.player.y==y&&scene.ticks==20);
    }
    check(scene_hashes.size()==6);std::cout<<"stage_scene_checks="<<checks<<" six_distinct_backgrounds=true failures=0\n";return 0;
}catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 1;}}
