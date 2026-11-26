#include "hiscore.hpp"
#include "score_animation.hpp"
#include <vector>
#include <fstream>
#include <iostream>
#include <source_location>
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include "native_score_store.hpp"
#endif
unsigned checks=0;
void check(bool ok,std::source_location where=std::source_location::current()){
    ++checks;if(!ok)throw std::runtime_error("Score test failed at line "+std::to_string(where.line()));
}
void put(std::span<std::byte> bytes,unsigned at,std::uint32_t value,unsigned width){for(unsigned i=0;i<width;++i)bytes[at+i]=std::byte((value>>(8*i))&255);}
std::vector<std::byte> fixture(){
    std::vector<std::byte> raw(910);
    for(unsigned p=0;p<5;++p){std::array<std::byte,178> d{};unsigned sum=0,names=0,stages=0;
        for(unsigned i=0;i<10;++i){unsigned score=10000-i*100;put(d,2+i*4,score,4);sum+=score;
            for(unsigned j=0;j<6;++j)d[46+i*7+j]=std::byte(0xaa+j);
            names+=0xaa;
            d[117+i]=std::byte(i==0?127:5);stages+=i==0?127:5;
            put(d,128+i*4,1997,2);d[130+i*4]=std::byte(15);d[131+i*4]=std::byte(8);d[168+i]=std::byte(i%3);
        }
        put(d,42,sum,4);d[116]=std::byte(names&255);d[127]=std::byte(stages&255);
        unsigned section=0;for(unsigned i=0;i<178;++i){section+=std::to_integer<unsigned>(d[i]);raw[p*182+i]=std::byte((std::to_integer<unsigned>(d[i])+18)&255);}
        put(raw,p*182+178,section,4);
    }return raw;
}
void reject(std::span<const std::byte> b){bool rejected=false;try{soew::decode_scores(b);}catch(const std::runtime_error&){rejected=true;}check(rejected);}
#ifdef _WIN32
namespace fs=std::filesystem;
using Store=soew::NativeScoreStore;
using Status=Store::Status;
using Entry=soew::NativeScoreEntry;

void seal_native(std::vector<std::byte>& bytes){
    std::uint32_t crc=0xffffffffu;
    for(std::size_t i=0;i<bytes.size()-4;++i){
        crc^=std::to_integer<unsigned>(bytes[i]);
        for(unsigned bit=0;bit<8;++bit)crc=(crc>>1)^((crc&1)?0xedb88320u:0u);
    }
    put(bytes,static_cast<unsigned>(bytes.size()-4),~crc,4);
}
std::vector<std::byte> native_v1_fixture(){
    std::vector<std::byte> raw(12+3*14+4);
    constexpr std::string_view magic="SOEWNSCR";
    for(unsigned i=0;i<magic.size();++i)raw[i]=std::byte(magic[i]);
    raw[8]=std::byte(1);raw[9]=std::byte(3);raw[10]=std::byte(4);
    const auto row=[&](unsigned p,unsigned rank,unsigned shot,unsigned continues,bool clear,
                       std::uint32_t score,std::string_view name){
        raw[p]=std::byte(rank);raw[p+1]=std::byte(shot);raw[p+2]=std::byte(continues);raw[p+3]=std::byte(clear);
        put(raw,p+4,score,4);
        for(unsigned i=0;i<name.size();++i)raw[p+8+i]=std::byte(name[i]);
    };
    row(12,1,0,1,false,500,"V1-A");
    row(26,1,2,0,true,500,"V1-C");
    row(40,4,2,0,false,123,"V1-EX");
    seal_native(raw);
    return raw;
}
void write_native(const fs::path& target,const std::vector<std::byte>& bytes){
    std::ofstream out(target,std::ios::binary|std::ios::trunc);
    out.write(reinterpret_cast<const char*>(bytes.data()),static_cast<std::streamsize>(bytes.size()));
    out.close();check(static_cast<bool>(out));
}
std::vector<std::byte> read_native(const fs::path& target){
    std::ifstream in(target,std::ios::binary|std::ios::ate);check(static_cast<bool>(in));
    std::vector<std::byte> bytes(static_cast<std::size_t>(in.tellg()));
    in.seekg(0);in.read(reinterpret_cast<char*>(bytes.data()),static_cast<std::streamsize>(bytes.size()));
    check(static_cast<bool>(in));return bytes;
}
void check_native_scores(){
    const auto dir=fs::current_path()/ ("score-sandbox-"+std::to_string(GetCurrentProcessId())+"-"+std::to_string(GetTickCount64()));
    check(fs::create_directory(dir));
    const auto target=dir/Store::filename;
    Store store(dir);check(store.load()==Status::Missing);
    const Entry below_default{"LOW",1,1,0,99,false,2},tied_default{"TIE",1,1,0,100,false,2};
    const auto defaults=Store::display_rows(store.entries(1));
    check(defaults.front().points==10000&&defaults.front().stage==5&&defaults.back().points==1000&&defaults.back().stage==1);
    check(!store.qualifies(below_default)&&store.submit(below_default)==Status::NotRanked&&!fs::exists(target));
    check(store.qualifies(tied_default));
    const auto preview=Store::display_rows(store.entries(1),&tied_default);
    check(preview.back().signing&&preview.back().points==1000&&preview.back().stage==2&&!fs::exists(target));
    check(store.record_good_clear(2)==Status::Ok);
    check(store.record_good_clear(2)==Status::Ok&&store.entries(1).empty());
    check(store.record_good_clear(3)==Status::InvalidEntry&&store.record_extra_clear(3)==Status::InvalidEntry);
    // Cancel means that no score is submitted, but both clear types survive a restart.
    check(store.record_extra_clear(1)==Status::Ok&&store.entries(4).empty());
    Store reloaded(dir);check(reloaded.load()==Status::Ok&&reloaded.has_good_clear(2)&&!reloaded.has_good_clear(0));
    check(reloaded.has_extra_clear(1)&&!reloaded.has_extra_clear(2)&&!reloaded.has_extra_clear(3));
    check(reloaded.entries(1).empty()&&reloaded.entries(4).empty());

    const auto legacy=native_v1_fixture();write_native(target,legacy);
    check(store.load()==Status::Ok&&store.has_good_clear(2)&&!store.has_extra_clear(1)&&!store.has_extra_clear(2));
    check(store.entries(1).size()==2&&store.entries(4).size()==1);
    check(store.entries(1)[0].name=="V1-A"&&store.entries(1)[0].stage==0&&store.entries(1)[0].continues_used==1);
    check(store.entries(1)[1].name=="V1-C"&&store.entries(1)[1].stage==Entry::all_stages&&store.entries(1)[1].good_clear);
    check(store.entries(4)[0].name=="V1-EX"&&store.entries(4)[0].score==123&&store.entries(4)[0].stage==0);
    check(store.record_good_clear(2)==Status::Ok&&read_native(target)==legacy);
    check(store.record_extra_clear(0)==Status::Ok);
    const auto migrated=read_native(target);
    check(migrated[8]==std::byte(2)&&migrated[10]==std::byte(4)&&migrated[11]==std::byte(1));
    check(reloaded.load()==Status::Ok&&reloaded.has_extra_clear(0)&&!reloaded.has_extra_clear(2)&&reloaded.has_good_clear(2));
    check(reloaded.entries(1).size()==2&&reloaded.entries(1)[0].name=="V1-A"&&reloaded.entries(1)[1].name=="V1-C");
    check(reloaded.entries(4).size()==1&&reloaded.entries(4)[0].name=="V1-EX"&&reloaded.entries(4)[0].stage==0);

    check(reloaded.submit({"",1,2,0,500,true,Entry::all_stages})==Status::Ok);
    check(reloaded.submit({"B",1,1,1,500,false,2})==Status::Ok);
    check(reloaded.submit({"LOSS",4,1,0,777,false,Entry::extra_stage})==Status::Ok);
    check(reloaded.submit({"WIN",4,2,0,777,false,Entry::all_stages})==Status::Ok);
    for(const auto& bad:std::array<Entry,8>{{
        {"INVALID",1,1,0,500,false,1}, {"BAD",4,1,1,777,false,Entry::extra_stage},
        {"BAD",4,0,0,777,true,Entry::all_stages}, {"BAD",4,0,0,777,false,5},
        {"BAD",1,0,0,777,false,Entry::extra_stage}, {"BAD",1,0,0,777,true,5},
        {"BAD",1,0,1,777,true,Entry::all_stages}, {"BAD",4,0,0,777,false,0}
    }}){
        check(!reloaded.qualifies(bad)&&reloaded.submit(bad)==Status::InvalidEntry);
    }
    Store board(dir);check(board.load()==Status::Ok&&board.entries(1).size()==4);
    check(board.entries(1)[0].name=="B"&&board.entries(1)[1].name=="V1-A"&&
          board.entries(1)[2].name=="PLAYER"&&board.entries(1)[3].name=="V1-C");
    check(board.entries(4).size()==3&&board.entries(4)[0].name=="WIN"&&board.entries(4)[0].stage==Entry::all_stages&&
          board.entries(4)[1].name=="LOSS"&&board.entries(4)[1].stage==Entry::extra_stage);
    // Score history is not consulted for the independent Extra clear bits.
    check(board.has_extra_clear(0)&&!board.has_extra_clear(1)&&!board.has_extra_clear(2));
    for(unsigned i=0;i<7;++i)check(board.submit({"FILL",4,0,0,1000+i,false,Entry::extra_stage})==Status::Ok);
    const auto full=read_native(target);
    const Entry low{"LOW",4,1,0,99,false,Entry::all_stages};
    check(!board.qualifies(low)&&board.submit(low)==Status::NotRanked&&read_native(target)==full);
    check(!board.has_extra_clear(1)&&board.record_extra_clear(1)==Status::Ok&&board.entries(4).size()==10);
    const Entry tie{"TIE",4,2,0,123,false,Entry::extra_stage};
    check(board.qualifies(tie)&&board.submit(tie)==Status::Ok&&board.entries(4).back().name=="TIE");
    // Score ordering uses the signed encoded 32-bit value, not raw score.
    check(board.submit({"WRAP",4,0,0,214748365,false,Entry::extra_stage})==Status::NotRanked);
    const std::string glyphs{"\x02\x03\xda\xe0\xe3\xcf",6};
    const Entry signed_name{glyphs,2,1,0,100,false,3,true};
    check(board.qualifies(signed_name)&&board.submit(signed_name)==Status::Ok);
    auto malformed=signed_name;malformed.name[0]=char(0xe7); // Enter is not a name glyph.
    check(!board.qualifies(malformed)&&board.submit(malformed)==Status::InvalidEntry);
    Store mixt(dir);
    check(mixt.load()==Status::Ok&&mixt.entries(2)[0].gaiji_name&&mixt.entries(2)[0].name==glyphs);
    check(mixt.entries(1)[1].name=="V1-A"&&!mixt.entries(1)[1].gaiji_name&&mixt.entries(1)[1].continues_used==1);
    const auto saved=read_native(target);
    check(saved[8]==std::byte(3));
    // Deny delete sharing to force publication failure without changing permissions.
    const auto locked=CreateFileW(target.c_str(),GENERIC_READ,FILE_SHARE_READ,nullptr,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,nullptr);
    check(locked!=INVALID_HANDLE_VALUE);
    const Entry candidate{"C",1,1,0,999,false,1};
    const auto failed_submit=board.submit(candidate);
    const auto failed_clear=board.record_extra_clear(2);
    CloseHandle(locked);
    check(failed_submit==Status::IoError&&failed_clear==Status::IoError);
    check(board.entries(1).size()==4&&!board.has_extra_clear(2)&&read_native(target)==saved);
    check(board.load()==Status::Ok&&board.entries(1).size()==4&&board.has_extra_clear(1));

    const auto rejects=[&](const std::vector<std::byte>& bad,Status status=Status::Invalid){
        write_native(target,bad);
        check(board.load()==status&&board.record_good_clear(0)==status&&board.record_extra_clear(2)==status);
        check(board.submit(candidate)==status&&read_native(target)==bad&&board.entries(1).size()==4);
        check(board.has_extra_clear(1)&&!board.has_extra_clear(2));
    };
    auto bad=legacy;bad[11]=std::byte(1);seal_native(bad);rejects(bad); // v1 reserved byte
    bad=legacy;bad[10]=std::byte(8);seal_native(bad);rejects(bad); // v1 reserved clear bit
    bad=legacy;bad[9]=std::byte(1);seal_native(bad);rejects(bad); // v1 record-count mismatch
    bad=legacy;bad[15]=std::byte(2);seal_native(bad);rejects(bad); // malformed flag
    bad=legacy;bad[28]=std::byte(2);seal_native(bad);rejects(bad); // continued good ending
    bad=legacy;put(bad,30,999,4);seal_native(bad);rejects(bad); // unsorted board
    bad=legacy;bad[25]=std::byte('X');seal_native(bad);rejects(bad); // nonzero name padding
    bad=migrated;bad[11]=std::byte(8);seal_native(bad);rejects(bad); // v2 reserved clear bit
    bad=migrated;bad[26]=std::byte(6);seal_native(bad);rejects(bad); // main record on Extra stage
    bad=migrated;bad[41]=std::byte(5);seal_native(bad);rejects(bad); // good clear must be ALL
    bad=migrated;bad[56]=std::byte(5);seal_native(bad);rejects(bad); // Extra record on main stage
    bad=migrated;bad[44]^=std::byte(1);rejects(bad); // CRC mismatch
    bad=migrated;bad.pop_back();rejects(bad); // truncated v2
    bad=migrated;bad.push_back(std::byte(0));rejects(bad); // trailing byte
    bad=migrated;bad[8]=std::byte(4);seal_native(bad);rejects(bad,Status::UnsupportedVersion);
    bad=saved;
    // The fifth saved record is the v3 gaiji record (after Normal's four ASCII rows).
    bad[12+4*15+8]=std::byte(0xe7);seal_native(bad);rejects(bad);
    bad=saved;bad[12+4*15+3]=std::byte(4);seal_native(bad);rejects(bad);
    check(fs::remove(target));check(fs::remove(dir));
}
#endif
int main(int argc,char**argv){try{
#ifdef _WIN32
    check_native_scores();
#endif
    namespace g=soew::score_gaiji;
    using Key=soew::ScoreSignature::Key;
    const auto first=g::game_over_positions(0),last=g::game_over_positions(g::converge_frames);
    check(first.top==21&&first.bottom==3&&first.left==9&&first.right==27);
    check(last.top==12&&last.bottom==12&&last.left==18&&last.right==18&&g::animation_frames==124);
    soew::ScoreSignature signature;
    check(!signature.update(Key::Up)&&signature.row==2);
    check(!signature.update(Key::Left)&&signature.column==16&&signature.update(Key::Select));
    check(signature.name()==std::string(6,char(g::bullet)));
    signature={};
    for(unsigned i=0;i<6;++i)check(!signature.update(Key::Select));
    check(signature.row==2&&signature.column==16&&signature.position==5&&signature.update(Key::Select));
    check(signature.name()==std::string(6,char(g::alphabet[0][0])));
    check(!signature.update(Key::Backspace)&&signature.position==4&&signature.cells[4]==g::space);
    check(!signature.update(Key::Space)&&signature.position==5);
    check(!signature.update(Key::Bomb)&&signature.position==4&&signature.cells[5]==g::space);
    soew::ScoreAnimation animation;
    for(unsigned i=0;i<2000;++i)check(!animation.tick());
    check(animation.tick());animation.reset();check(animation.frame==0);
    std::array<soew::LogoCel,2> logo{};logo[0].indices.fill(1);logo[1].indices.fill(2);
    std::array<std::uint32_t,16> palette{};for(unsigned i=0;i<16;++i)palette[i]=i+100;
    std::vector<std::uint32_t> canvas(640*400,0xabcdef);
    soew::score_logo_background(canvas,logo,palette,0,0);check(canvas[96*640+128]==110&&canvas[304*640+512]==110);
    check(canvas[95*640+128]==0xabcdef&&canvas[304*640+513]==0xabcdef);
    for(unsigned f:{1u,100u,399u,400u,1000u,2001u}){
        std::fill(canvas.begin(),canvas.end(),0xabcdef);soew::score_logo_background(canvas,logo,palette,0,f);
        for(int y=0;y<400;++y)for(int x=0;x<640;++x)if(x<128||x>512||y<96||y>304)check(canvas[y*640+x]==0xabcdef);
    }
    const auto defaults=soew::default_scores();
    const auto fresh=soew::clear_status(defaults);
    check(!fresh.unlocked&&fresh.main==std::array<bool,3>{}&&fresh.extra==std::array<bool,3>{});
    for(const auto &page:defaults)for(unsigned i=0;i<page.entries.size();++i){
        const auto &entry=page.entries[i];
        check(entry.score==1000*(10-i)&&entry.stage==5-i/2&&entry.shot==1);
        check(entry.year==1900&&entry.month==1&&entry.day==1);
        for(auto glyph:entry.name)check(glyph==g::bullet);
    }
    for(unsigned main_mask=0;main_mask<8;++main_mask)for(unsigned extra_mask=0;extra_mask<8;++extra_mask){
        soew::Scores synthetic{};constexpr unsigned magic[3]={318,118,218};
        for(unsigned i=0;i<3;++i)synthetic[i].cleared=(main_mask&(1u<<i))?magic[i]:0;
        synthetic[3].cleared=extra_mask;synthetic[4].cleared=7;
        const auto status=soew::clear_status(synthetic);check(status.unlocked==(main_mask==7));
        for(unsigned i=0;i<3;++i){check(status.main[i]==bool(main_mask&(1u<<i)));check(status.extra[i]==bool(extra_mask&(1u<<i)));}
    }
    for(unsigned legacy_main=0;legacy_main<8;++legacy_main)for(unsigned native_main=0;native_main<8;++native_main)
    for(unsigned legacy_extra=0;legacy_extra<8;++legacy_extra)for(unsigned native_extra=0;native_extra<8;++native_extra){
        // Ignore stale input unlock flags and combine both clear types per shot.
        soew::ClearStatus legacy{},native{};legacy.unlocked=native.unlocked=true;
        for(unsigned i=0;i<3;++i){
            legacy.main[i]=(legacy_main&(1u<<i))!=0;native.main[i]=(native_main&(1u<<i))!=0;
            legacy.extra[i]=(legacy_extra&(1u<<i))!=0;native.extra[i]=(native_extra&(1u<<i))!=0;
        }
        const auto merged=soew::merge_clear_status(legacy,native);
        check(merged.unlocked==((legacy_main|native_main)==7));
        for(unsigned i=0;i<3;++i){
            check(merged.main[i]==bool((legacy_main|native_main)&(1u<<i)));
            check(merged.extra[i]==bool((legacy_extra|native_extra)&(1u<<i)));
        }
    }
    soew::Scores wrong_magic{};wrong_magic[0].cleared=118;wrong_magic[1].cleared=218;wrong_magic[2].cleared=318;check(!soew::clear_status(wrong_magic).unlocked);
    auto raw=fixture();auto scores=soew::decode_scores(raw);
    check(scores[0].entries[0].score==10000);check(scores[4].entries[0].stage==127);
    check(scores[1].entries[0].year==1997&&scores[1].entries[0].month==8&&scores[1].entries[0].day==15);
    for(unsigned size=0;size<910;++size)reject(std::span<const std::byte>(raw).first(size));
    auto extra=raw;extra.push_back(std::byte{0});reject(extra);
    for(unsigned i=0;i<910;++i){auto bad=raw;bad[i]^=std::byte{1};reject(bad);}
    // Damage inner score sum while recomputing outer section checksum.
    auto bad=raw;bad[42]^=std::byte{1};unsigned sum=0;for(unsigned i=0;i<178;++i)sum+=std::uint8_t(std::to_integer<unsigned>(bad[i])-18);put(bad,178,sum,4);reject(bad);
    if(argc==2){std::ifstream f(argv[1],std::ios::binary|std::ios::ate);if(!f)throw std::runtime_error("Fixture missing");auto size=f.tellg();std::vector<std::byte> bytes(static_cast<std::size_t>(size));f.seekg(0);f.read(reinterpret_cast<char*>(bytes.data()),size);
        const auto actual=soew::decode_scores(bytes);check(actual[1].entries[0].score==8042510);check(actual[0].entries[0].score==4787770);
        std::cout<<"real_records=50 real_checksums=valid\n";
    }
    std::cout<<"score_checks="<<checks<<" failures=0\n";return 0;
}catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 1;}}
