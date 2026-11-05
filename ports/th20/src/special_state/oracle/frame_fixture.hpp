#include "../environment.hpp"
namespace sp=th20::source::sprite;namespace eff=th20::source::effects;namespace st=th20::source::state;
using Event=std::array<unsigned,24>;
std::vector<Event> events;
bool charge=false,boss=false,blocked=false,controller_exists=true;
int session_mode=0;std::array<bool,16> exists;sp::Vec3 origin;
unsigned bits(float f){return float_bits(f);}
void event(unsigned id,unsigned a=0,unsigned b=0){Event e{};e[0]=id;e[1]=a;e[2]=b;events.push_back(e);}
void* __cdecl enemy_owner_boundary(int){return controller_exists?reinterpret_cast<void*>(1):nullptr;}
bool __fastcall charge_boundary(void*,void*){return charge;}
bool __fastcall mode_boundary(void*,void*){return session_mode==2;}
int __fastcall boss_boundary(void*,void*){return int(boss);}
void* __fastcall find_enemy_boundary(unsigned* h,void*){return exists[*h%exists.size()]?reinterpret_cast<void*>(1):nullptr;}
sp::Vec3* __fastcall enemy_position_boundary(unsigned*,void*){return &origin;}
sp::Vec3* __fastcall position_boundary(void*,void*){return &origin;}
void* __fastcall draw_find_boundary(void*,void*,unsigned){return reinterpret_cast<void*>(1);}
void __fastcall spawn_boundary(void*,void*,const char* name,const th20::source::gameplay::SpawnParameters* p,void*){Event e{};e[0]=1;const char chars[]{'R','B','Y','G'};for(unsigned i=0;i<4;++i)if(name[5]==chars[i])e[1]=i;std::memcpy(e.data()+2,p,sizeof(*p));events.push_back(e);}
unsigned* __fastcall effect_boundary(void*,void*,unsigned* output,int type,const eff::Parameters* p,void*){Event e{};e[0]=2;e[1]=unsigned(type);std::memcpy(e.data()+2,p,sizeof(*p));auto* at=reinterpret_cast<unsigned char*>(e.data()+2);at[0x29]=at[0x2a]=at[0x2b]=0;events.push_back(e);*output=0x500;return output;}
void __fastcall delete_boundary(unsigned* handle,void*){event(3,*handle);*handle=0;}
void __fastcall free_boundary(void*,void*,ss::Entry* entry){event(4,entry->enemy_handle,entry->animation_handle);}
int __fastcall bullets_boundary(void*,void*,const sp::Vec3* p,float radius,int mode,int limit,unsigned kind){Event e{};e[0]=11;e[1]=bits(p->x);e[2]=bits(p->y);e[3]=bits(p->z);e[4]=bits(radius);e[5]=unsigned(mode);e[6]=unsigned(limit);e[7]=kind;events.push_back(e);return 0;}
int __fastcall lasers_boundary(void*,void*,const sp::Vec3* p,float radius,int mode,int kind){Event e{};e[0]=12;e[1]=bits(p->x);e[2]=bits(p->y);e[3]=bits(p->z);e[4]=bits(radius);e[5]=unsigned(mode);e[6]=unsigned(kind);events.push_back(e);return 0;}
unsigned text_color=0xffffffff,text_font=0;float text_scale_x=1,text_scale_y=1;int text_x=1,text_y=1,text_blend=0;
void __fastcall text_style_boundary(void*,void*,int x,int y){text_x=x;text_y=y;event(20,unsigned(x),unsigned(y));}
void __fastcall text_font_boundary(void*,void*,int n){text_font=unsigned(n);event(21,text_font);}
void __fastcall text_color_boundary(void*,void*,unsigned n){text_color=n;event(22,n);}
void __fastcall text_alpha_boundary(void*,void*,std::uint8_t n){text_color=(text_color&0x00ffffff)|(unsigned(n)<<24);event(23,n);}
void __fastcall text_scale_boundary(void*,void*,float x,float y){text_scale_x=x;text_scale_y=y;event(24,bits(x),bits(y));}
void __fastcall text_save_boundary(void*,void*,int n){text_blend=n;event(25,unsigned(n));}
void __fastcall text_restore_boundary(void*,void*){event(26);}
void __fastcall line_boundary(void*,void*,const sp::Vec3* p,std::string_view s){Event e{};e[0]=27;e[1]=bits(p->x);e[2]=bits(p->y);e[3]=bits(p->z);e[4]=text_color;e[5]=text_font;e[6]=bits(text_scale_x);e[7]=bits(text_scale_y);e[8]=unsigned(text_x);e[9]=unsigned(text_y);e[10]=unsigned(text_blend);e[11]=unsigned(s.size());events.push_back(e);}
void __fastcall level_boundary(void*,void*,const sp::Vec3* p,std::string_view s,unsigned n){line_boundary(nullptr,nullptr,p,s);events.back()[0]=28;events.back()[12]=n;}
struct Fixture final:ss::Environment {
    gs::Player& player() override{return ::player;}
    int session_mode() override{return ::session_mode;}
    bool enemies_present() override{return controller_exists&&charge;}
    bool boss_collecting() override{return boss;}
    bool special_blocked() override{return blocked;}
    void spawn_enemy(const char* name,const th20::source::gameplay::SpawnParameters& p) override{spawn_boundary(nullptr,nullptr,name,&p,nullptr);}
    bool enemy_exists(unsigned& h) override{return find_enemy_boundary(&h,nullptr)!=nullptr;}
    sp::Vec3 enemy_position(unsigned& h) override{return *enemy_position_boundary(&h,nullptr);}
    void effect(int type,const eff::Parameters& p) override{unsigned out;effect_boundary(nullptr,nullptr,&out,type,&p,nullptr);}
    void delete_animation(unsigned& h) override{delete_boundary(&h,nullptr);}
    void free_entry(ss::Entry& p) override{free_boundary(nullptr,nullptr,&p);}
    void initialize_entry(ss::Entry&,unsigned,int) override{throw std::logic_error("unexpected initialize_entry");}
    void clear_bullets(const sp::Vec3& p) override{bullets_boundary(nullptr,nullptr,&p,320,0,99999,0);}
    void clear_lasers(const sp::Vec3& p) override{lasers_boundary(nullptr,nullptr,&p,320,0,1);}
    sp::Vec3 position_of(void* p) override{return *position_boundary(p,nullptr);}
    void text_style(int x,int y) override{text_style_boundary(nullptr,nullptr,x,y);}
    void text_font(int n) override{text_font_boundary(nullptr,nullptr,n);}
    void text_color(unsigned n) override{text_color_boundary(nullptr,nullptr,n);}
    void text_alpha(std::uint8_t n) override{text_alpha_boundary(nullptr,nullptr,n);}
    void text_scale(float x,float y) override{text_scale_boundary(nullptr,nullptr,x,y);}
    void text_save(int n) override{text_save_boundary(nullptr,nullptr,n);}
    void text_restore() override{text_restore_boundary(nullptr,nullptr);}
    void text_line(const sp::Vec3& p,const char* s) override{line_boundary(nullptr,nullptr,&p,s);}
    void text_level(const sp::Vec3& p,unsigned n) override{level_boundary(nullptr,nullptr,&p,"Level ",n);}
} host;
namespace th20::source::special_state {Environment& environment(){return host;}}
namespace th20::source::gameplay {void construct_spawn_parameters(SpawnParameters& p) noexcept{std::memset(&p,0,sizeof(p));}}
