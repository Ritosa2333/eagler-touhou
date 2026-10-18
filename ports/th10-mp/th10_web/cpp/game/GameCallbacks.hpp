#include "CallbackNames.hpp"
#pragma once
#include "AnmLayers.hpp"
#include "AnmDistortion.hpp"
#include "AnmRenderer.hpp"
#include "ApplicationFrame.hpp"
#include "ApplicationSystem.hpp"
#include "CommonResources.hpp"
#include "StartupScreen.hpp"
#include "StageRenderer.hpp"
#include "ScreenEffect.hpp"
#include "SpellCard.hpp"
#include "EnemyManager.hpp"
#include "BulletFrame.hpp"
#include "LaserManager.hpp"
#include "Bomb.hpp"
#include "Item.hpp"
#include "Player.hpp"
#include "Gui.hpp"
#include "GameSession.hpp"
#include "StageHints.hpp"
#include "Ending.hpp"
#include "TitleLoop.hpp"
#include "TitleDraw.hpp"
#include "Results.hpp"
#include "ScorePopups.hpp"
#include "FrameStatistics.hpp"
#include "Replay.hpp"
namespace th10 {
struct GameCallbackWorld {
    ApplicationState* application;
    AnmManager** animations;
    GameSession** session;
    float* rate;
    u32* fog_enabled;
};
// Tokens identify callbacks stored in game objects. They are never executed as
// machine-code addresses here. Platform supplies typed environments; all game
// calls below are ordinary C++ calls and require no CPU or register adapter.
template<class Platform> struct GameCallbacks {
    Platform& platform;GameCallbackWorld& world;
    bool invoke(CallbackToken token,void* owner,i32& result){
        const u32 layers[]={callback_id::AnimationLayer0,callback_id::AnimationLayer1,callback_id::AnimationLayer2,callback_id::AnimationLayer3,callback_id::AnimationLayer4,callback_id::AnimationLayer5,callback_id::AnimationLayer6,callback_id::AnimationLayer7,callback_id::AnimationLayer8,callback_id::AnimationLayer9,callback_id::AnimationLayer10,callback_id::AnimationLayer11,callback_id::AnimationLayer12,callback_id::AnimationLayer13,callback_id::AnimationLayer14,callback_id::AnimationLayer15,callback_id::AnimationLayer16,0,0,callback_id::AnimationLayer19};
        for(u32 layer=0;layer<20;layer++)if(layers[layer]&&token==layers[layer]){
            auto camera=platform.camera();auto frame=platform.animation_frame();auto& app=*world.application;
            result=AnmLayers{app.world_camera,app.ui_camera,app.active_camera,app.screen_space,*world.fog_enabled,camera,frame}.draw(*static_cast<AnmManager*>(owner),layer);return true;
        }
        switch(token){
        case callback_id::AnimationsWorld:case callback_id::AnimationsUI:{auto env=platform.animation_frame();auto& manager=*static_cast<AnmManager*>(owner);result=token==callback_id::AnimationsWorld?manager.update_world(env):manager.update_ui(env);break;}
        case callback_id::DistortionUpdate:case callback_id::DistortionDraw:{auto& vm=*static_cast<AnmVm*>(owner);auto& effect=*static_cast<AnmDistortion*>(vm.geometry);if(token==callback_id::DistortionUpdate)result=effect.update(vm);else{auto env=platform.animation_render();AnmRenderer{**world.animations,env}.draw_textured_fan(vm,effect.vertices,33);result=0;}break;}
        case 0x4201b0:{auto env=platform.application_system();result=ApplicationSystem{env}.initialize();break;}
        case callback_id::ApplicationUpdate:case callback_id::ApplicationBeginDraw:case callback_id::ApplicationFinishDraw:{auto env=platform.application_frame();ApplicationFrame frame{token==callback_id::ApplicationFinishDraw?*world.application:*static_cast<ApplicationState*>(owner),env};result=token==callback_id::ApplicationUpdate?frame.update():token==callback_id::ApplicationBeginDraw?frame.begin_draw():frame.finish_draw();break;}
        case callback_id::CommonUpdate:result=static_cast<CommonResources*>(owner)->update();break;
        case callback_id::CommonDraw:case callback_id::CommonEarlyDraw:{auto env=platform.ascii_render();result=static_cast<CommonResources*>(owner)->draw(token==callback_id::CommonEarlyDraw,env);break;}
        case callback_id::StartupUpdate:case callback_id::StartupDraw:{auto env=platform.startup();auto& screen=*static_cast<StartupScreen*>(owner);result=token==callback_id::StartupUpdate?screen.update(env):screen.draw(env);break;}
        case callback_id::StageUpdate:{auto env=platform.stage();result=static_cast<Stage*>(owner)->update(env);break;}
        case callback_id::StageBackground:case callback_id::StageForeground:{auto env=platform.stage_render();StageRenderer renderer{*static_cast<Stage*>(owner),env};result=token==callback_id::StageBackground?renderer.draw_background():renderer.draw_foreground();break;}
        case callback_id::ScreenFadeUpdate:case callback_id::ScreenFlashUpdate:case callback_id::ScreenArcadeUpdate:case callback_id::ScreenCircleUpdate:case callback_id::ScreenShakeUpdate:case callback_id::ScreenViewShakeUpdate:{auto env=platform.screen_effect();auto& effect=*static_cast<ScreenEffect*>(owner);result=token==callback_id::ScreenFadeUpdate?effect.reveal(env):token==callback_id::ScreenFlashUpdate?effect.hide(env):token==callback_id::ScreenArcadeUpdate?effect.dim():token==callback_id::ScreenCircleUpdate?effect.flash(env):token==callback_id::ScreenShakeUpdate?effect.shake_linear(env):effect.shake_envelope(env);break;}
        case callback_id::ScreenFadeDraw:case callback_id::ScreenFlashDraw:case callback_id::ScreenArcadeFadeDraw:case callback_id::ScreenArcadeFlashDraw:case callback_id::ScreenCircleDraw:{auto env=platform.screen_effect();result=static_cast<ScreenEffect*>(owner)->draw_region(env,token==callback_id::ScreenFadeDraw||token==callback_id::ScreenArcadeFadeDraw,token==callback_id::ScreenFadeDraw,token==callback_id::ScreenCircleDraw);break;}
        case callback_id::ScreenEffectDelete:{auto env=platform.screen_effect();auto* effect=static_cast<ScreenEffect*>(owner);if(effect){effect->release(env);env.destroy(effect);}result=0;break;}
        case callback_id::SpellUpdate:case callback_id::SpellBackground:case callback_id::SpellForeground:{auto env=platform.spell();auto& spell=*static_cast<SpellCard*>(owner);result=token==callback_id::SpellUpdate?spell.update(env):token==callback_id::SpellBackground?spell.draw_backgrounds(env):spell.draw_digits(env);break;}
        case callback_id::EnemiesUpdate:{auto env=platform.enemies();result=static_cast<EnemyManager*>(owner)->update(env);break;}
        case callback_id::BulletsUpdate:case callback_id::BulletsDraw:{auto env=platform.bullets();auto& bullets=*static_cast<EnemyBulletManager*>(owner);result=token==callback_id::BulletsUpdate?bullets.tick(env):bullets.render(env);break;}
        case callback_id::LasersUpdate:case callback_id::LasersDraw:{auto env=platform.lasers();auto& lasers=*static_cast<LaserManager*>(owner);const u32 flags=(*world.session)->session_flags;result=token==callback_id::LasersUpdate?lasers.tick(flags,*world.rate,env):lasers.render(flags,env);break;}
        case callback_id::BombUpdate:{auto env=platform.bomb();result=static_cast<Bomb*>(owner)->update(env);break;}
        case callback_id::ItemsUpdate:case callback_id::ItemsDraw:{
            const auto* session=*world.session;const u32 flags=session?session->session_flags:0;result=1;
            if(token==callback_id::ItemsUpdate?(flags&0x405)==0:(flags&4)==0){auto env=platform.items();auto& items=*static_cast<ItemManager*>(owner);result=token==callback_id::ItemsUpdate?items.update(env):items.draw(env);}break;
        }
        case callback_id::PlayerUpdate:{auto env=platform.player_frame();result=static_cast<Player*>(owner)->update(env);break;}
        case callback_id::PlayerDraw:{auto env=platform.player_draw();result=static_cast<Player*>(owner)->draw(env);break;}
        case callback_id::HudUpdate:case callback_id::HudDraw:{auto env=platform.gui();auto& gui=*static_cast<Gui*>(owner);result=token==callback_id::HudUpdate?gui.update(env):gui.draw(env);break;}
        case callback_id::SessionUpdate:{auto env=platform.session();result=static_cast<GameSession*>(owner)->update(env);break;}
        case callback_id::SessionDraw:result=static_cast<GameSession*>(owner)->draw(**world.animations);break;
        case callback_id::HintsUpdate:{auto env=platform.hints();result=static_cast<StageHints*>(owner)->update(env);break;}
        case callback_id::EndingUpdate:{auto env=platform.ending();result=static_cast<Ending*>(owner)->update(env);break;}
        case callback_id::TitleUpdate:case callback_id::TitleDraw:{auto env=platform.title();auto& title=*static_cast<TitleMenu*>(owner);result=token==callback_id::TitleUpdate?update_title(title,env):draw_title(title,env);break;}
        case callback_id::ResultsUpdate:{auto env=platform.results();result=static_cast<Results*>(owner)->update(env);break;}
        case callback_id::ResultsDraw:{auto env=platform.results_draw();result=static_cast<Results*>(owner)->draw(env);break;}
        case callback_id::ReplayUpdate:case callback_id::ReplayFrame:case callback_id::ReplayDraw:{auto env=platform.replay();auto& replay=*static_cast<Replay*>(owner);result=token==callback_id::ReplayUpdate?replay.update_input(env):token==callback_id::ReplayFrame?replay.frame_action(env,true):replay.draw(env,true);break;}
        case callback_id::PopupsUpdate:result=static_cast<ScorePopups*>(owner)->update(world.rate);break;
        case callback_id::PopupsDraw:{auto env=platform.score_popups();result=static_cast<ScorePopups*>(owner)->draw(env);break;}
        case callback_id::FrameStatisticsDraw:{auto env=platform.statistics();result=static_cast<FrameStatistics*>(owner)->draw(env);break;}
        case callback_id::BombDraw:case callback_id::EffectsUpdate:case callback_id::EffectsDraw:case callback_id::EnemiesDraw:case callback_id::HintsDraw:case callback_id::EndingDraw:case callback_id::ApplicationDrawBarrier:result=1;break;
        default:return false;
        }
        return true;
    }
};
}
