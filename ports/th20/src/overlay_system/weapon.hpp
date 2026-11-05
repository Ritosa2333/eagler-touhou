#pragma once
#include "../runtime_core/callback_owner.hpp"
#include "../runtime_state/state.hpp"
#include "../player_entity/owner.hpp"
namespace th20::source::overlay {
struct WeaponStoneInf;
// Exact 30-slot vtable575810. The destructor is deliberately non-virtual:
// 532740 always invokes the common532950 destructor, outside this vtable.
class Weapon {
public:
    virtual void reset();                                    //00,52fb80
    virtual void initialize_main();                          //04,40e5e0
    virtual void shoot_main(int,int,int);                    //08,52fbc0
    virtual void shoot_focused(int,int,int);                 //0c,52fbc0
    virtual void shoot_unfocused(int,int,int);               //10,52fbc0
    virtual const sprite::Vec2* focused_offset(int,int);      //14,477ce0
    virtual const sprite::Vec2* unfocused_offset(int,int);    //18,477ce0
    virtual void activate_main();                            //1c,40e5e0
    virtual void activate_focused();                         //20,40e5e0
    virtual void activate_unfocused();                       //24,40e5e0
    virtual void initialize_focused_option(player_entity::Option*,int); //28,414b60
    virtual void initialize_unfocused_option(player_entity::Option*,int); //2c,414b60
    virtual void update_focused_option(player_entity::Option*,int); //30,414b60
    virtual void update_unfocused_option(player_entity::Option*,int); //34,414b60
    virtual void initialize_passive();                       //38,40e5e0
    virtual void update_main();                              //3c,40e5e0
    virtual void update_focused();                           //40,40e5e0
    virtual void update_unfocused();                         //44,40e5e0
    virtual void update_passive();                           //48,52fb60
    virtual int script_variant();                           //4c,412540
    virtual void start_phase();                              //50,40e5e0
    virtual int update_phase();                              //54,412540
    virtual int end_phase();                                 //58,532f20
    virtual int cancel_phase();                              //5c,412540
    virtual const sprite::Vec3* phase_position();             //60,412540
    virtual int shot_script_index();                         //64,534180
    virtual bool phase_active();                             //68,530500
    virtual bool unfocused_shooting();                       //6c,5343b0
    virtual bool focused_shooting();                         //70,534320
    virtual bool passive_active();                          //74,530540
    std::int32_t stone_id,field_08,role;
    std::uint32_t field_10;
    recovered::Timer phase_age,idle_age;
    std::uint8_t active,passive,padding_36[2];
    Weapon() noexcept;                                      //52fad0
    ~Weapon()=default;                                       //532950: vptr transition only
};
static_assert(sizeof(Weapon)==0x38&&offsetof(Weapon,phase_age)==0x14&&offsetof(Weapon,active)==0x34);
class StandardWeapon:public Weapon {
public:
    void shoot_main(int,int,int) override;                   //52fbd0
    void shoot_focused(int,int,int) override;                //52fd10
    void shoot_unfocused(int,int,int) override;              //52ff70
    const sprite::Vec2* focused_offset(int,int) override;     //5301c0
    const sprite::Vec2* unfocused_offset(int,int) override;   //530370
    void start_phase() override;                            //52ffd0
    int update_phase() override;                            //52fd70
};
int phase_duration(game_session::Player&);                   //534260
void fire_player_shots(player_entity::ShotController&,int,int,int); //504d40, actual Player firing source adapter
}
