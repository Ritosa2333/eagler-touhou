#include "TitleScene.hpp"
namespace th075 {
void TitleScene::initialize(u32 start,u32 now,GameSelection& selection){
    *this=TitleScene{};startedAt=start;elapsed=now-start;selected=selection.titleSelection;selection.character.fill(0);selection.background=0;
}
SceneTransition TitleScene::advance(const TitleInput& input,u32 now,GameSelection& selection,ReplayRandom& random,SceneSignals& signals){
    verticalEdge=0;
    for(const auto& p:input.players)if(p.vertical==1)verticalEdge=1;
    for(const auto& p:input.players)if(p.vertical==-1)verticalEdge=-1;
    escapeHeld=input.escape?increment_wrapped(escapeHeld,1):0;
    for(u8 side=0;side<2;side++){
        if(input.players[side].buttons[0]==1&&elapsed<57000){
            selection.activePlayer=side;
            switch(selected){
                case 0:case 1:case 4:
                    if(side==0){signals.sound(45);selection.mode=selected==0?GameMode::Story:selected==1?GameMode::Arcade:GameMode::Training;return SceneTransition::CharacterSelect;}break;
                case 2:case 3:signals.sound(45);selection.mode=selected==2?GameMode::VersusCpu:GameMode::Versus;return SceneTransition::CharacterSelect;
                case 5:signals.sound(45);selection.activePlayer=0;return SceneTransition::ReplayMenu;
                case 6:signals.sound(45);return SceneTransition::MusicRoom;
                case 7:signals.sound(45);return SceneTransition::ResultList;
                case 8:signals.sound(45);return SceneTransition::Options;
                case 9:return SceneTransition::Exit;
            }
        }
        if(input.players[side].buttons[1]==1){if(selected==9)return SceneTransition::Exit;signals.sound(40);selected=9;}
    }
    if(escapeHeld==1){if(selected==9)return SceneTransition::Exit;signals.sound(40);selected=9;}
    if(verticalEdge==1){signals.sound(39);selected=u8(selected+1);if(i8(selected)>9)selected=0;}
    else if(verticalEdge==-1){signals.sound(39);selected=u8(selected-1);if(i8(selected)<0)selected=9;}
    for(u32 i=0;i<highlight.size();i++){if(i==selected)highlight[i]=fade[3];else highlight[i]=highlight[i]<11?0:u8(highlight[i]-10);}
    if(fade[0]!=255)fade[0]=u8(fade[0]+10);
    for(u32 i=1;i<fade.size();i++)if(fade[i]!=255&&fade[i-1]>150)fade[i]=u8(fade[i]+10);
    if(elapsed<60001){elapsed=now-startedAt;return SceneTransition::Stay;}
    if(selection.demoCycle==0){
        for(u32 side=0;side<2;side++){selection.character[side]=u8(random.next()%6);selection.alternatePalette[side]=0;selection.firstSpell[side]=u8(random.next()%3);selection.secondSpell[side]=u8(random.next()%3);}
        if(selection.character[0]==selection.character[1])selection.character[1]=u8((selection.character[1]+1)%6);
        const u8 stage=u8(random.next()%6),variant=u8(random.next()&1);selection.background=u8(stage+variant*10);
        selection.mode=GameMode::Demonstration;selection.demoCycle++;signals.fade_music(-10000,-300,1,0);return SceneTransition::Demonstration;
    }
    selection.demoCycle=0;signals.fade_music(-10000,-35,1,0);return SceneTransition::Opening;
}
void TitleScene::render(ZunGraphics& graphics,const std::array<u32,4>& texture)const{
    graphics.depth(DepthMode::Disabled);graphics.depth(DepthMode::WriteEnabled);graphics.blend(SpriteBlend::Alpha);
    graphics.textured_quad(rectangle_quad(0,0,640,480),{0,0,640,480},u32(fade[0])<<24|0xffffff,texture[0]);
    graphics.textured_quad(rectangle_quad(64,32,576,224),{0,0,512,192},u32(fade[1])<<24|0xffffff,texture[2]);
    graphics.textured_quad(rectangle_quad(64,448,576,480),{0,0,512,32},u32(fade[2])<<24|0xffffff,texture[3]);
    graphics.filter(Filter::Linear);
    for(u32 i=0;i<10;i++){
        auto points=rectangle_quad(0,0,25,256);rotate_quad(points,5,0,0,0,0,-90);translate_quad(points,220,float(i*23+240),0);translate_quad(points,offset[i],0,0);
        graphics.textured_quad(points,{0,0,25,256},u32(highlight[i])<<24|0xffffff,texture[1]);
        points=rectangle_quad(245,float(i*23+225),476,float(i*23+250));translate_quad(points,offset[i],0,0);
        const u32 shade=selected==i?255:128;
        graphics.textured_quad(points,{25,i32(i*25),256,i32(i*25+25)},u32(fade[3])<<24|shade*0x010101,texture[1]);
    }
    graphics.filter(Filter::Nearest);
}
}
