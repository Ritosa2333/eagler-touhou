#define wmain unused_native_cpu_main
#include "../../tests/native_cpu_compare.cpp"
#undef wmain
#include "session.hpp"
namespace g=th20::source::game_session;
template<class R,class... A> R cpu(std::uint32_t va,void* self,A... args) {using F=R(__thiscall*)(void*,A...);return reinterpret_cast<F>(mapped_image_base+va-0x400000)(self,args...);}
int wmain(int argc,wchar_t** argv) {try {
    if(argc!=3)throw std::runtime_error("Usage: th20_session_cpu_compare ORIGINAL.exe OUTPUT.json");
    const auto bytes=th20::read_file(argv[1]);if(sha256(bytes)!=expected_sha)throw std::runtime_error("Original SHA mismatch");
    Mapping image(bytes,th20::parse_pe(bytes));mapped_image_base=image.address();std::mt19937 rng(0x422e40);
    unsigned passed=0,failed=0;std::vector<std::string> failures;
    auto check=[&](const char* name,const void* a,const void* b,std::size_t size) {if(!std::memcmp(a,b,size)){++passed;return;}++failed;if(failures.size()<20){std::ostringstream text;text<<name;for(std::size_t i=0;i<size;++i)if(static_cast<const unsigned char*>(a)[i]!=static_cast<const unsigned char*>(b)[i]){text<<" +0x"<<std::hex<<i;break;}failures.push_back(text.str());}};
    auto randomize=[&](auto& value) {auto* data=reinterpret_cast<std::uint8_t*>(&value);for(std::size_t i=0;i<sizeof(value);++i)data[i]=static_cast<std::uint8_t>(rng());};
    for(unsigned test=0;test<2048;++test) {
        g::Session a,b;randomize(a);b=a;cpu<void>(0x422e40,&a);g::construct_session(b);check("session_constructor",&a,&b,sizeof(a));
        g::Player pa,pb;randomize(pa);pb=pa;cpu<void>(0x423050,&pa);g::construct_player(pb);check("player_constructor",&pa,&pb,sizeof(pa));
        g::PlayerTable ta,tb;randomize(ta);tb=ta;cpu<void>(0x422f20,&ta);g::construct_player_table(tb);check("table_constructor",&ta,&tb,sizeof(ta));
        g::Context ca,cb;randomize(ca);cb=ca;cpu<void>(0x423320,&ca);g::construct_context(cb);check("context_constructor",&ca,&cb,sizeof(ca));
        randomize(a);b=a;const std::uint32_t value=rng();cpu<void>(0x4be220,&a,value);g::set_flag0(b,value);check("set_flag0",&a,&b,sizeof(a));
        cpu<void>(0x4bdd60,&a,value);g::set_flag1(b,value);check("set_flag1",&a,&b,sizeof(a));
        randomize(ta);ta.continue_count=test%3==0?static_cast<std::int32_t>(rng()%20)-5:static_cast<std::int32_t>(rng());tb=ta;
        const std::int32_t delta=test%2?static_cast<std::int32_t>(rng()):static_cast<std::int32_t>(rng()%40)-20;
        cpu<void>(0x4bccf0,&ta,delta);g::add_continue_count(tb,delta);check("continue_wrap_and_clamp",&ta,&tb,sizeof(ta));
    }
    g::Session initial;auto* original=reinterpret_cast<g::Session*>(mapped_image_base+0x1ba568);std::memset(original,0,sizeof(*original));cpu<void>(0x422e40,original);
    check("real_global_default_storage",original,&g::session,sizeof(g::session));
    g::bind_default_player();auto* first=&g::session.player_table.players[0];check("default_player_binding",&first,&g::session.contexts[0].current_player,sizeof(first));
    for(int i=0;i<2;++i){g::session.contexts[i].primary_owner=reinterpret_cast<th20::source::runtime::CallbackOwner*>(0x1000+i*16);g::session.contexts[i].overlay_owner=reinterpret_cast<th20::source::runtime::CallbackOwner*>(0x2000+i*16);check("primary_alias",&g::session.contexts[i].primary_owner,&g::primary_owner(i),4);check("overlay_alias",&g::session.contexts[i].overlay_owner,&g::overlay_owner(i),4);}
    std::ofstream report(argv[2]);report<<"{\n  \"passed\": "<<passed<<",\n  \"failed\": "<<failed<<",\n  \"original_cpu_cases\": 14337,\n  \"source_alias_checks\": 5,\n  \"original_sha256\": \""<<expected_sha<<"\",\n  \"full_game_equivalence\": false,\n  \"failures\": [";
    for(unsigned i=0;i<failures.size();++i)report<<(i?", ":"")<<'"'<<failures[i]<<'"';report<<"]\n}\n";std::cout<<passed<<" passed, "<<failed<<" failed\n";return failed?1:0;
}catch(const std::exception& ex){std::cerr<<ex.what()<<'\n';return 2;}}
