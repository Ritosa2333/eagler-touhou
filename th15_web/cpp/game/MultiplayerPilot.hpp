#pragma once
#if TH15_MULTIPLAYER
#include "Player.hpp"
#include "Multiplayer.hpp"
#include "BombController.hpp"
#include "ItemCollection.hpp"
#include "GameInput.hpp"
namespace th15 {
class GameBattle;
// Stable owners: every shot/life/item/bomb callback addresses its own pilot.
// Seat zero keeps the retail session owner used by stage flow and HUD.
struct MultiplayerPilot final:PlayerHost,BombHost,ItemCollectionHost {
    GameBattle& world;unsigned seat;int character;
    PlayerLifeSession owned_session;i32 owned_life_piece_tier;
    PlayerLifeSession& session;ItemScoreState& score;
    std::unique_ptr<Player> owned_player;Player* player=nullptr;
    std::unique_ptr<BombContext> context;std::unique_ptr<BombController> owned_bomb;BombController* bomb=nullptr;
    std::unique_ptr<ItemCollection> owned_collection;ItemCollection* collection=nullptr;
    GameInput keys;MultiplayerInput input;bool ghost=false;
    MultiplayerPilot(GameBattle&,unsigned,int);
    bool initialize(const ShtResource&,int bank);
    bool rebuild_bomb();bool tick();bool tick_bomb();bool reset_stage();
    bool alive()const noexcept;
    bool allow_bomb()override;bool begin_bomb()override;
    bool cancel_lasers_near(const Vec3&,float,i32,bool)override;
    bool cancel_bullets_near(const Vec3&,float,i32)override;bool cancel_lasers(i32,bool)override;
    bool item(i32,const Vec3&,float,float)override;bool options_changed()override;
    bool game_over()override;bool respawn_damage(const Vec3&,float,float,i32,i32)override;
    bool bomb_hud(i32,i32)override;bool life_hud(i32,i32)override;
    bool stop_sound(i32)override;bool refresh(PlayerFrameContext&)override;
    bool sound(i32,float,PlayerShots::SoundAction)override;bool hit_sound(i32)override;
    bool sound(i32,bool)override;bool popup(const Vec3&,i32,u32)override;bool notice(i32)override;
    bool cancel_bullets(const Vec3&,float,i32)override;bool cancel_lasers(const Vec3&,float,i32,bool)override;
    bool cancel_bullets_rectangle(const Vec3&,const Vec2&,float,i32)override;
    bool cancel_lasers_rectangle(const Vec3&,const Vec2&,float,i32,bool)override;
    bool shake(const ScreenShakeSpec&)override;bool nudge(const ScreenNudgeSpec&)override;
};
}
#endif
