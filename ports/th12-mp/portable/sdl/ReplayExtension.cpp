#include "ReplayExtension.hpp"
#include "ReplayJson.hpp"
#include <algorithm>
#include <cstring>
#include <sstream>

namespace th12host {
namespace {
constexpr size_t MaxTrailer=16*1024*1024;
u32 crc32(const u8* data,size_t size){u32 crc=0xffffffff;for(size_t i=0;i<size;++i){crc^=data[i];for(int b=0;b<8;++b)crc=(crc>>1)^((crc&1)?0xedb88320:0);}return crc^0xffffffff;}
u32 word(const u8* p){u32 value;std::memcpy(&value,p,4);return value;}
void require(bool condition){if(!condition)ReplayJson::bad();}
const char* names[18]{"mode","stage","section","phase","dlg","score","life","life_fragment","bomb","bomb_fragment","power","value","graze","ufo_side","ventra_1","ventra_2","ventra_3",nullptr};
std::array<i32,20> params(ReplayJson& json){
    std::array<i32,20> p{1,1,0,0,0,0,0,0,9,0,9,0,400,10000,0,0,0,0,0,1};
    const int indices[]{1,2,3,4,5,6,8,9,10,11,12,13,14,15,16,17,18};
    const i64 maximum[]{1,6,19999,4,1,9999999990LL,9,3,9,2,400,999990,999999,1,3,3,3};
    json.object([&](const std::string& key){
        int i=0;while(names[i]&&key!=names[i])++i;if(!names[i]){json.skip();return;}
        const i64 v=i==4?i64(json.boolean()):json.integer(0,maximum[i]);p[size_t(indices[i])]=i32(u32(v));if(i==5)p[7]=i32(u64(v)>>32);
    });
    const u64 score=u32(p[6])|(u64(u32(p[7]))<<32);
    require(score%10==0&&p[13]%10==0&&(!(!p[16]&&(p[17]||p[18])))&&(!(!p[17]&&p[18])));
    return p;
}
void read_practice(ReplayJson& json,ReplayExtension& extension){
    if(json.null())return;bool schema=false,game=false,parameters=false;
    json.object([&](const auto& key){
        if(key=="schema")schema=json.string()=="eagler-touhou/th12-practice/1";
        else if(key=="game")game=json.string()=="th12";
        else if(key=="params"){extension.practice=params(json);parameters=true;}
        else if(key=="assisted")extension.assisted=json.boolean();
        else json.skip();
    });require(schema&&game&&parameters);
}
void read_damage(ReplayJson& json,ReplayExtension& extension){
    if(json.null())return;bool schema=false,layout=false,character=false,shot=false,stages=false;
    json.object([&](const auto& key){
        if(key=="schema")schema=json.string()=="eagler-touhou/th12-damage-sources/1";
        else if(key=="layout")layout=json.string()=="th12-portable-damage-source/1";
        else if(key=="character")character=json.integer()==0;
        else if(key=="shot")shot=json.integer()==0;
        else if(key=="stages"){
            stages=true;json.array(5,[&](size_t){int stage=0;bool number=false,has_words=false;std::vector<i32> words;
                json.object([&](const auto& field){if(field=="number"){stage=int(json.integer(2,6));number=true;}else if(field=="words"){has_words=true;json.array(6+128*18,[&](size_t){words.push_back(i32(json.integer()));});}else json.skip();});
                require(number&&has_words&&words.size()>=6&&u32(words[0])==0x32315344&&words[1]==1&&words[2]==stage&&words[4]>=0&&words[4]<=128&&words[5]==18&&words.size()==6+size_t(words[4])*18&&u32(words[3]));
                require(extension.damage.emplace(stage,std::move(words)).second);
            });
        }else json.skip();
    });require(schema&&layout&&character&&shot&&stages);
}
}
std::array<i32,2> ReplayExtension::delta(i32 stage,u32 frame)const{
    const auto it=motion.find(stage);if(it==motion.end())return {};
    const auto& rows=it->second;const auto row=std::lower_bound(rows.begin(),rows.end(),frame,[](const auto& r,u32 frame){return r.frame<frame;});
    return row!=rows.end()&&row->frame==frame?std::array<i32,2>{row->x,row->y}:std::array<i32,2>{};
}
bool read_replay_extension(const std::vector<u8>& file,size_t at,const std::map<i32,u32>& stages,ReplayExtension& out,std::string& error){
    if(at+8>file.size()||std::memcmp(file.data()+at,"TH12WEB1",8))return true;
    try{
        require(at+16<=file.size());const u32 size=word(file.data()+at+8);
        require(size<=MaxTrailer&&at+16+size==file.size()&&crc32(file.data()+at+16,size)==word(file.data()+at+12));
        ReplayJson json(std::string_view(reinterpret_cast<const char*>(file.data()+at+16),size));ReplayExtension e;e.present=true;
        bool schema=false,scale=false,unlimited=false,records=false;
        json.object([&](const auto& key){
            if(key=="schema")schema=json.string()=="th12-web-motion/1";
            else if(key=="coordinateScale")scale=json.integer()==128;
            else if(key=="unlimited"){e.unlimited=json.boolean();unlimited=true;}
            else if(key=="practice")read_practice(json,e);
            else if(key=="damage")read_damage(json,e);
            else if(key=="stages"){
                records=true;json.array(stages.size(),[&](size_t){int stage=0;bool number=false,deltas=false;std::vector<ReplayMotion> rows;
                    json.object([&](const auto& field){
                        if(field=="number"){stage=int(json.integer(1,7));number=true;}
                        else if(field=="deltas"){deltas=true;json.array(10000000,[&](size_t){std::array<i32,3> row{};size_t count=0;
                            json.array(3,[&](size_t n){row[n]=i32(json.integer(n==0?0:-65536,n==0?9999999:65536));++count;});require(count==3&&(rows.empty()||row[0]>i32(rows.back().frame)));rows.push_back({u32(row[0]),row[1],row[2]});
                        });}else json.skip();
                    });
                    const auto original=stages.find(stage);require(number&&deltas&&original!=stages.end()&&rows.size()<=original->second&&(rows.empty()||rows.back().frame<original->second));require(e.motion.emplace(stage,std::move(rows)).second);
                });
            }else json.skip();
        });json.end();require(schema&&scale&&unlimited&&records&&e.motion.size()==stages.size());out=std::move(e);return true;
    }catch(const std::exception& e){error=e.what();return false;}
}
void append_replay_extension(std::vector<u8>& file,const ReplayExtension& e){
    bool motion=false;for(const auto& [stage,rows]:e.motion)motion|=!rows.empty();if(!motion&&!e.practice&&e.damage.empty())return;
    std::ostringstream out;out<<"{\"schema\":\"th12-web-motion/1\",\"coordinateScale\":128,\"unlimited\":"<<(e.unlimited?"true":"false")<<",\"practice\":";
    if(e.practice){const auto& p=*e.practice;const int indices[]{1,2,3,4,5,6,8,9,10,11,12,13,14,15,16,17,18};
        out<<"{\"schema\":\"eagler-touhou/th12-practice/1\",\"game\":\"th12\",\"assisted\":"<<(e.assisted?"true":"false")<<",\"params\":{";
        for(int i=0;names[i];++i){if(i)out<<',';out<<'"'<<names[i]<<"\":";if(i==4)out<<(p[5]?"true":"false");else if(i==5)out<<(u32(p[6])|(u64(u32(p[7]))<<32));else out<<p[size_t(indices[i])];}
        out<<"},\"source\":{\"referenceSha256\":\"95be43b0efd25d73931e1e71de624a5097f7c576f2f7ccd64ed94cdf59a8ee6b\",\"definitionsRevision\":\"72f20e99dbf5b74319db7aaec25202d23d584486\"}}";
    }else out<<"null";
    out<<",\"damage\":";if(e.damage.empty())out<<"null";else{
        out<<"{\"schema\":\"eagler-touhou/th12-damage-sources/1\",\"layout\":\"th12-portable-damage-source/1\",\"character\":0,\"shot\":0,\"stages\":[";bool comma=false;
        for(const auto& [stage,words]:e.damage){if(comma)out<<',';comma=true;out<<"{\"number\":"<<stage<<",\"words\":[";for(size_t n=0;n<words.size();++n){if(n)out<<',';out<<words[n];}out<<"]}";}out<<"]}";
    }
    out<<",\"stages\":[";bool comma=false;for(const auto& [stage,rows]:e.motion){if(comma)out<<',';comma=true;out<<"{\"number\":"<<stage<<",\"deltas\":[";for(size_t n=0;n<rows.size();++n){if(n)out<<',';const auto& r=rows[n];out<<'['<<r.frame<<','<<r.x<<','<<r.y<<']';}out<<"]}";}out<<"]}";
    const auto json=out.str();if(json.size()>MaxTrailer)throw std::runtime_error("TH12 replay extension exceeds 16 MiB");
    const size_t at=file.size();file.insert(file.end(),{'T','H','1','2','W','E','B','1'});file.resize(at+16);const u32 size=u32(json.size()),crc=crc32(reinterpret_cast<const u8*>(json.data()),json.size());std::memcpy(file.data()+at+8,&size,4);std::memcpy(file.data()+at+12,&crc,4);file.insert(file.end(),json.begin(),json.end());
}
}
