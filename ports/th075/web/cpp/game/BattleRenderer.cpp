#include "BattleRenderer.hpp"
namespace th075 {
class BattleRenderer::Commands final:public BattleDrawCommands {
    BattleRenderer& renderer;BattleScene& scene;BattleHudView view;
    ImageRenderer stage,hud,effects;std::array<ImageRenderer,2> images,portraits;
    std::array<SpriteRenderer,2> sprites;SpriteRenderer sharedSprites;
    const CameraMotion& camera;ShadowStyle shadow;
    class Objects final:public FighterObjectCallbacks {
        Commands& owner;u8 side;
    public:
        Objects(Commands& owner,u8 side):owner(owner),side(side){}
        void update(FighterObject&)override{}
        void draw(FighterObject& object,bool alternate)override{
            if(!object.animation.frame)return;auto& sprite=owner.sprites[side];
            if(alternate)sprite.screen(*object.animation.frame,object.motion,object.pose);
            else sprite.object(*object.animation.frame,object.motion,object.pose,owner.camera.x,owner.camera.y);
        }
        void shadow(FighterObject& object)override{if(object.animation.frame)owner.sprites[side].shadow(*object.animation.frame,object.motion,object.pose,owner.shadow,owner.camera.x,owner.camera.y,owner.scene.simulation.characters[side]->groundOffset);}
    };
public:
    Commands(BattleRenderer& r,BattleScene& s):renderer(r),scene(s),view(BattleHudView::from(s)),stage(r.graphics,r.art.background),hud(r.graphics,r.art.hud),effects(r.graphics,r.art.effects),images{ImageRenderer(r.graphics,*r.art.fighters[0]),ImageRenderer(r.graphics,*r.art.fighters[1])},portraits{ImageRenderer(r.graphics,*r.art.portraits[0]),ImageRenderer(r.graphics,*r.art.portraits[1])},sprites{SpriteRenderer(r.graphics,*r.art.fighters[0]),SpriteRenderer(r.graphics,*r.art.fighters[1])},sharedSprites(r.graphics,r.art.effects),camera(s.simulation.story.camera),shadow(r.background.shadow_style()){}
    void background_back()override{renderer.background.render_back(stage,camera);}void background_middle()override{renderer.background.render_middle(stage,camera);}void background_front()override{renderer.background.render_front(stage,camera);}
    void object_shadows(u8 side)override{Objects draw(*this,side);scene.simulation.objects[side].render_shadows(draw);}
    void fighter_shadow(u8 side)override{const auto& character=*scene.simulation.characters[side];const auto& a=character.actor;if(a.animation.frame)sprites[side].shadow(*a.animation.frame,a.motion,a.pose,shadow,camera.x,camera.y,character.groundOffset);}
    void owned_objects(u8 side,u8 layer)override{Objects draw(*this,side);scene.simulation.objects[side].render(layer,draw);}
    void trails(u8 side)override{sprites[side].trails(scene.simulation.characters[side]->actor.trails,camera.x,camera.y);}
    void shared_effects(u8 layer)override{scene.simulation.effects.render(layer,[&](SharedEffect& effect){if(!effect.image)return;const auto pose=effect.drawing_pose();if(effect.alternateDraw)sharedSprites.screen(*effect.image,effect.motion,pose);else sharedSprites.object(*effect.image,effect.motion,pose,camera.x,camera.y);});}
    // Contact snapshots can remain unchanged throughout story and round recovery.
    void fighter(u8 side)override{auto& a=scene.simulation.characters[side]->actor;const auto* frame=a.animation.frame;if(frame)sprites[side].fighter(*frame,a.motion,a.pose,a.animation.cursor.action,a.combat.freeze,a.combat.flashTimer,a.combat.noticeTimer,a.trails,{renderer.disableTrails,renderer.displayStage},camera.x,camera.y);}
    void character_special(u8 side)override{scene.simulation.characters[side]->render_special(camera,renderer.graphics,renderer.art.fighterTextures[side]);}
    void recovery_markers()override{render_recovery_markers(hud,view,camera);}
    void status()override{renderer.hud.render_status(hud,images[0],images[1],view);}
    void fade_portrait(u8 side)override{render_fade_portrait(portraits[side],*view.fighters[side]);}
    void spells()override{renderer.hud.render_spells(hud,images[0],images[1],view,scene.simulation.rules,scene.simulation.difficulty);}
    void victory()override{const u8 winner=scene.simulation.rules.winner;const auto& a=*view.fighters[winner];render_victory_portrait(portraits[winner],a,a.noticePortrait,scene.progress.textSlide,0,0xffffffff);renderer.text.render();}
    void story_portrait(u8 side)override{render_story_portrait(portraits[side],*view.fighters[side]);}
    void dialogue()override{renderer.text.render();}void story_caption()override{scene.simulation.characters[0]->render_story(scene.simulation.story.stage,renderer.graphics,renderer.art.captions[0]);}
    void battle_overlay()override{renderer.overlay.frame=i32(scene.timing.frame);renderer.overlay.timer=scene.progress.overlayTime;renderer.overlay.render(hud);}
    void bonus_banner()override{renderer.hud.bonusNumber=scene.simulation.rules.bonusValue;renderer.hud.bannerTimer=scene.simulation.rules.bonusTimer;renderer.hud.render_bonus_banner(hud);}
    void pause_menu()override{renderer.hud.render_pause_menu(hud,portraits[0],portraits[1],view);}
    void fps()override{if(renderer.fpsStartedAt+1000u<renderer.now){renderer.fpsValue=renderer.fpsCount;renderer.fpsCount=0;renderer.fpsStartedAt=renderer.now;}++renderer.fpsCount;renderer.hud.number(hud,640,0,0xffffffff,i32(renderer.fpsValue),4);}
};
void BattleRenderer::render(BattleScene& scene,bool stageEnabled,u32 milliseconds){now=milliseconds;displayStage=stageEnabled;Commands commands(*this,scene);render_battle_layers(graphics,BattleDrawingState::from(scene,stageEnabled,recoveryMarkers),commands);}
}
