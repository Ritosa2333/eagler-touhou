#include "stage_script.hpp"
#include "stage_background.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <set>
std::vector<std::byte> read(const std::filesystem::path&p){std::ifstream f(p,std::ios::binary|std::ios::ate);if(!f)throw std::runtime_error("Missing stage file");auto n=f.tellg();if(n<1||n>1000000)throw std::runtime_error("Bad stage size");std::vector<std::byte>b(static_cast<std::size_t>(n));f.seekg(0);if(!f.read(reinterpret_cast<char*>(b.data()),n))throw std::runtime_error("Read failed");return b;}
unsigned checks=0;void check(bool b){++checks;if(!b)throw std::runtime_error("All-stage DT1 contract failed");}
int wmain(int argc,wchar_t**argv){try{
    if(argc!=2)throw std::runtime_error("Usage: soew-stage-script-tests <original-assets>");const std::filesystem::path root=argv[1];
    const std::array<unsigned,6> expected_templates={13,10,17,12,10,40};
    for(int stage=0;stage<6;++stage){auto prefix="STAGE"+std::to_string(stage);auto data=read(root/(prefix+".DT1"));auto script=soew::StageScript::decode(data,true);check(script.enemies.size()==expected_templates[stage]);
        unsigned references=0,inversions=0;for(auto &e:script.enemies)references+=e.path_reference;for(unsigned i=1;i<script.formations.size();++i)inversions+=script.formations[i].trigger<script.formations[i-1].trigger;
        bool strict_accepted=true;try{soew::StageScript::decode(data);}catch(const std::runtime_error&){strict_accepted=false;}check(strict_accepted==(stage!=3));check(script.complete()==(stage!=3));
        if(stage==3){
            // Both locally archived editions contain this same incomplete payload.
            check(data.size()==14025&&script.declared_formations==270&&script.formations.size()==266);
            check(script.formations.back().trigger==2174);
            check(script.has_missing_formations(266));
            script.enable_stage4_compatibility();
            check(!script.has_missing_formations(266)&&!script.complete());
            auto altered=data;altered.back()=std::byte{0};
            auto other=soew::StageScript::decode(altered,true);
            bool refused=false;
            try{other.enable_stage4_compatibility();}catch(const std::runtime_error&){refused=true;}
            check(refused&&other.has_missing_formations(266));
        }
        if(stage==5)check(inversions>0);
        auto bg=soew::StageBackground::decode(read(root/(prefix+".MPN")),read(root/(prefix+".MAP")),data);check(bg.map==script.map);
        // Deliberately truncated structures must reject, with consistent header.
        for(unsigned length:{4u,5u,6u,7u,static_cast<unsigned>(script.timeline_offset-1)}){auto short_data=data;short_data.resize(length);if(length>=4){short_data[0]=std::byte((length-4)&255);short_data[1]=std::byte((length-4)>>8);}bool rejected=false;try{soew::StageScript::decode(short_data);}catch(const std::runtime_error&){rejected=true;}check(rejected);}
        auto bad=data;const auto row=script.timeline_offset;bad[row+2]=std::byte{254};bool rejected=false;try{soew::StageScript::decode(bad,true);}catch(const std::runtime_error&){rejected=true;}check(rejected);
        bad=data;const auto path=script.enemies.front().file_offset+28;bad[path]=std::byte{254};bad[path+1]=std::byte{255};rejected=false;try{soew::StageScript::decode(bad,true);}catch(const std::runtime_error&){rejected=true;}check(rejected);
        std::cout<<prefix<<" map="<<script.map.size()<<" tiles="<<bg.tiles.size()<<" templates="<<script.enemies.size()<<" paths="<<script.paths.size()<<" references="<<references<<" formations="<<script.formations.size()<<" declared="<<script.declared_formations<<" trigger_inversions="<<inversions<<" complete="<<script.complete()<<'\n';
    }
    std::cout<<"stage_script_checks="<<checks<<" failures=0\n";return 0;
}catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 1;}}
