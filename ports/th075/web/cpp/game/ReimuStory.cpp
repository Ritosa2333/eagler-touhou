#include "ReimuStory.hpp"
namespace th075 {
namespace {
void add(i16& value,i16 delta=1){value=i16(u16(value)+u16(delta));}
void fade_down(u8& value,u8 step,u8 threshold){value=value<threshold?0:u8(value-step);}
void fade_up(u8& value,u8 step,u8 threshold){value=value<threshold?u8(value+step):255;}
void title_positions(StoryCutscene& s){s.x={-192,-192,0,320};s.y={160,336,176,280};}
void title_drift(StoryCutscene& s){s.x[0]=float(double(s.x[0])+.25);s.x[1]=float(double(s.x[1])-.25);}
void title_fade_in(StoryCutscene& s){if(s.alpha[1]<250){s.alpha[0]=u8(s.alpha[0]+5);s.alpha[1]=u8(s.alpha[1]+5);}else{s.alpha[0]=s.alpha[1]=255;}}
void title_fade_out(StoryCutscene& s){if(s.alpha[0]<5)s.alpha[0]=s.alpha[1]=s.alpha[2]=0;else for(u32 i=0;i<3;i++)s.alpha[i]=u8(s.alpha[i]-5);}
}
void advance_reimu_story(FighterActor& a,FighterActor& other,StoryCutscene& s,i16& wait,StoryEnvironment& env,StoryEvents& events){
    const auto hide=[&]{events.hide_portrait(a);events.hide_portrait(other);};
    const auto finish=[&](u8 next){s.alpha[3]=0;a.scriptCursor.waiting=false;events.show_dialogue(false);a.dialogueFinished=true;env.camera.override_view(false,0,0,0);s.step=next;wait=0;};
    const auto opening=[&](u8 base){
        env.camera.override_view(true,0,float((120.0-double(i32(wait)-50)*.5)*double(wait<291)),320);
        if(wait==0){title_positions(s);events.begin_dialogue(i16(100+base/10));events.show_dialogue(false);events.advance_dialogue();events.advance_dialogue();}
        if(wait>49){title_drift(s);fade_down(env.fadeAlpha,5,5);}
        if(wait<301)fade_up(s.alpha[2],5,250);if(wait>49&&wait<301)title_fade_in(s);
        if(wait==230){a.select_action(9);a.motion.velocityX=7.75f;a.motion.velocityY=18;a.motion.accelerationY=.6f;}
        if(wait>300)title_fade_out(s);if(a.animation.cursor.action!=9&&wait>230)a.motion.x=480;
        if(wait<361)add(wait);else{s.step=u8(base+1);wait=0;}
    };
    const auto enter_other=[&](u8 current){
        if(wait==0){other.select_action(9);other.motion.velocityX=current==3||current==33?7.75f:7.8f;other.motion.velocityY=18;other.motion.accelerationY=.6f;}
        if(other.animation.cursor.action==9)add(wait);else{other.motion.x=800;s.step=u8(current+1);wait=0;}
    };
    const auto finish_dialogue=[&](u8 next,i16 frames=30){if(events.advance_dialogue()){if(wait==0)hide();if(wait<=frames)add(wait);else finish(next);}};
    const auto ready_portraits=[&](u8 portrait,u8 direction,i16 label,u8 next){
        if(wait==0){events.show_portrait(a,portrait,direction);events.show_portrait(other,8,u8(1-direction));}
        if(wait==30){events.begin_dialogue(label);s.step=next;wait=0;}else add(wait);
    };
    switch(s.step){
    case 0:case 10:case 20:case 30:case 40:case 50:opening(s.step);break;
    case 1:events.begin_dialogue(0);s.step=2;break;
    case 2:if(events.advance_dialogue())s.step=3;break;
    case 3:case 13:case 23:case 33:case 43:enter_other(s.step);break;
    case 4:events.begin_dialogue(1);s.step=5;break;
    case 5:case 15:case 25:case 37:case 45:case 55:finish_dialogue(u8(s.step+1));break;
    case 6:ready_portraits(2,1,2,7);break;
    case 7:if(events.advance_dialogue()){a.dialogueFinished=true;s.step=10;s.nextStage=2;}break;
    case 11:events.begin_dialogue(3);s.step=12;break;
    case 12:if(events.advance_dialogue())s.step=13;break;
    case 14:events.begin_dialogue(4);s.step=15;break;
    case 16:ready_portraits(4,1,5,17);break;
    case 17:if(events.advance_dialogue()){a.dialogueFinished=true;s.step=20;s.nextStage=3;}break;
    case 21:events.begin_dialogue(6);s.step=22;break;
    case 22:if(events.advance_dialogue())s.step=23;break;
    case 24:events.begin_dialogue(7);s.step=25;break;
    case 26:ready_portraits(2,1,8,27);break;
    case 27:if(events.advance_dialogue()){a.dialogueFinished=true;s.step=30;s.nextStage=4;env.completedStages=u8(env.completedStages+1);}break;
    case 31:events.begin_dialogue(9);s.step=32;break;
    case 32:if(events.advance_dialogue())s.step=33;break;
    case 34:events.begin_dialogue(10);s.step=35;wait=0;break;
    case 35:
        if(events.advance_dialogue()&&wait==0){s.alpha[3]=0;a.scriptCursor.waiting=false;events.show_dialogue(false);hide();wait=1;}
        if(wait>0){add(wait);env.fadeColor=0;if(wait<30)fade_up(env.fadeAlpha,15,240);
            if(wait>30&&wait<90&&wait==60){env.background=12;events.reload_stage(12);other.motion.x=800;a.motion.x=480;}
            if(wait>89){fade_down(env.fadeAlpha,5,5);if(env.fadeAlpha==0){wait=0;s.step=36;}}
        }break;
    case 36:events.begin_dialogue(11);s.step=37;break;
    case 38:ready_portraits(2,1,12,39);break;
    case 39:if(events.advance_dialogue()){a.dialogueFinished=true;s.step=40;s.nextStage=5;}break;
    case 41:events.begin_dialogue(13);s.step=42;break;
    case 42:if(events.advance_dialogue()){s.step=43;events.hide_portrait(other);}break;
    case 44:events.begin_dialogue(14);s.step=45;break;
    case 46:case 56:
        if(i8(env.route)<3){if(wait<120)add(wait);else{s.step=s.step==46?70:80;s.nextStage=s.step==70?8:9;a.dialogueFinished=true;}}
        else ready_portraits(s.step==46?1:4,0,s.step==46?15:18,u8(s.step+1));break;
    case 47:if(events.advance_dialogue()){a.dialogueFinished=true;s.step=50;s.nextStage=6;}break;
    case 51:events.begin_dialogue(16);s.step=52;break;
    case 52:if(events.advance_dialogue()){s.step=53;events.hide_portrait(a);wait=0;}break;
    case 53:
        env.fadeColor=0xffffff;if(wait<30)add(wait);
        if(wait==30){fade_up(env.fadeAlpha,15,240);if(env.fadeAlpha==255)wait=31;}
        if(wait>30&&wait<90){if(wait==60){events.show_dialogue(false);env.background=8;events.reload_stage(8);other.motion.x=800;a.motion.x=480;}add(wait);}
        if(wait==90){fade_down(env.fadeAlpha,5,5);if(env.fadeAlpha==0){wait=0;s.step=54;}}break;
    case 54:events.begin_dialogue(17);s.step=55;break;
    case 57:if(events.advance_dialogue()){a.dialogueFinished=true;s.step=60;s.nextStage=7;}break;
    case 60:
        if(wait==0){title_positions(s);add(wait,60);other.select_action(2);events.begin_dialogue(106);events.show_dialogue(false);events.advance_dialogue();events.advance_dialogue();}
        if(wait>159)title_drift(s);if(wait>119&&wait<361)fade_up(s.alpha[2],5,250);if(wait>159&&wait<361)title_fade_in(s);if(wait>360)title_fade_out(s);
        if(wait==450){s.step=61;wait=0;}else add(wait);break;
    case 61:
        env.camera.override_view(true,0,float(320-i32(wait)),320);add(wait);if(wait>319){s.step=62;wait=0;}break;
    case 62:other.controls.current.vertical=1;env.camera.override_view(true,0,0,320);events.begin_dialogue(19);s.step=63;break;
    case 63:if(events.advance_dialogue()){if(wait==0)hide();if(wait<=120)add(wait);else{finish(64);env.fadeAlpha=0;}}break;
    case 64:env.fadeColor=0xffffff;if(env.fadeAlpha<255)env.fadeAlpha++;else{s.nextStage=10;a.dialogueFinished=true;}break;
    case 70:case 80:case 90:
        s.x.fill(0);s.y.fill(0);env.fadeAlpha=0;events.begin_dialogue(i16(107+(s.step-70)/10));events.show_dialogue(false);events.advance_dialogue();events.advance_dialogue();s.step++;break;
    case 71:case 81:fade_up(s.alpha[0],5,251);if(s.alpha[0]==255){s.step++;wait=0;}break;
    case 72:events.begin_dialogue(20);s.step=73;break;
    case 73:case 83:if(events.advance_dialogue()){fade_down(s.alpha[0],5,6);if(s.alpha[0]==0){events.show_dialogue(false);a.dialogueFinished=true;}}break;
    case 82:events.begin_dialogue(21);s.step=83;break;
    case 91:s.step=92;wait=0;break;
    case 92:events.begin_dialogue(22);s.step=93;wait=0;break;
    case 93:case 95:
        env.fadeColor=0;if(events.advance_dialogue()){
            if(wait==0){if(env.fadeAlpha<250)env.fadeAlpha=u8(env.fadeAlpha+5);else{env.fadeAlpha=255;wait=1;}}
            if(wait==1){events.show_dialogue(false);events.replace_text(s.step==95);const u32 image=s.step==93?0:1;s.alpha[image]=0;s.alpha[image+1]=255;wait=2;}
            if(wait==2){if(env.fadeAlpha<6){env.fadeAlpha=0;wait=0;s.step++;}else env.fadeAlpha=u8(env.fadeAlpha-5);}
        }break;
    case 94:events.begin_dialogue(23);s.step=95;wait=0;break;
    case 96:events.begin_dialogue(24);s.step=97;break;
    case 97:if(events.advance_dialogue()){env.fadeColor=0xffffff;if(events.advance_dialogue()){
        if(wait==0){events.stop_music();if(env.fadeAlpha<252)env.fadeAlpha=u8(env.fadeAlpha+3);else{env.fadeAlpha=255;wait=1;}}
        if(wait>0)add(wait);if(wait>60){events.show_dialogue(false);a.dialogueFinished=true;}
    }}break;
    }
    if(a.scriptCursor.musicDelay==0){events.select_music(a.scriptCursor.musicId);events.start_music();}
    if(a.scriptCursor.musicDelay>=0)add(a.scriptCursor.musicDelay,-1);
    events.advance_portrait(a);events.advance_portrait(other);
}
}
