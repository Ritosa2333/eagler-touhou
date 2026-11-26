import {playFull} from './native/full.mjs';
import {RetraceClock} from './native/clock.mjs';
import {createManagedTransport} from './native/network/managed-transport.mjs';
import {createConfirmedPeerRun,validCoopInput} from './native-coop-peers.mjs';
import {packTouch} from './touch-input.js';
import {createPc98SpectatorPublisher} from './pc98-spectator-publisher.mjs';
import {createLocalSpectator} from './pc98-local-spectator.mjs';
import {createPc98SpectatorChannel} from './pc98-spectator-channel.mjs';

const tickMs=9328*1000/526315;
const delay=3;
const roles=['host','guest','guest2'];
const own=(value,min,max)=>Number.isInteger(value)&&value>=min&&value<=max;
const sound=(music,options)=>music==='none'?0:options.musicSource==='akyu'?3:1;
const lang=language=>language==='ja'?0:2;
function roomCanvas(host){
    const canvas=document.createElement('canvas');canvas.width=640;canvas.height=400;canvas.tabIndex=0;
    host.append(canvas);return canvas;
}
const roomRules=(options,language)=>[
    options.netplayPlayerCount,options.netplayDifficulty===4?5:0,Math.min(options.netplayDifficulty,3),
    options.netplayInitialSpares??2,options.netplayInitialBombs??3,
    ...roles.map((_,i)=>options.netplayLoadouts[i]?.shot??0),
    options.netplaySeed>>>0,(options.netplayDisableFocus?0:1)|(options.netplayRapidFire===false?0:2)|
        (options.netplayDisableCheatMovement?4:0)|(options.netplayChallengeMode?8:0),lang(language),
];

export function createNativeRoomPreview(host){
    let current;
    function stop(){current?.player?.stop();current?.canvas.remove();current=null;}
    return {stop,prepare(language){
        if(current)return current.task;
        const run=current={canvas:roomCanvas(host)};
        return run.task=(async()=>{
            const player=await playFull(run.canvas,undefined,{hosted:true,manualFrames:true,titleLanguage:lang(language)});
            if(current!==run){player.stop();throw Error('标题预览已取消');}
            run.player=player;
            return {presented:true};
        })().catch(error=>{if(current===run)stop();throw error;});
    }};
}

export function createNativeRoom({input,emit}){
    let player,peer,transport,publisher,canvas,keys,closed=true;
    function stop(){
        closed=true;keys?.abort();publisher?.stop();peer?.close();player?.stop();canvas?.remove();
        if(globalThis.__th02PeerTransport===transport?.status)delete globalThis.__th02PeerTransport;
    }
    function fail(error){if(!closed){stop();emit({event:'error',message:error.message});}}
    return {
        async start({options,music,language}){
            closed=false;
            const rules=roomRules(options,language),count=rules[0],seat=options.netplayPlayer;
            const clock=new RetraceClock(tickMs),rows=[];
            let ended=false;
            canvas=roomCanvas(document.querySelector('#game'));
            player=await playFull(canvas,undefined,{
                hosted:true,confirmedCoop:{rules,seat,music:sound(music,options)},
                beforeFrame(){
                    if(!peer?.ready||ended)return;
                    clock.frame(performance.now(),()=>1,()=>peer.advance());
                    if(peer.finished){
                        ended=true;publisher?.kick();
                        void (async()=>{await publisher?.seal();await transport.finish();if(!closed){stop();emit({event:'exit',code:0});}})().catch(fail);
                    }
                },
            });
            if(closed){player.stop();return;}
            transport=createManagedTransport(options,roles,fail,'TH02');
            const status=globalThis.__th02PeerTransport=transport.status;
            const url=new URL(options.netplayUrl);
            const identity=[url.searchParams.get('room'),url.searchParams.get('run'),url.searchParams.get('runtimeGeneration'),rules.slice(0,-1)];
            if(seat===0){
                const tape={get length(){return rows.length;},slice:(a,b)=>rows.slice(a,b),get end(){return ended;}};
                publisher=createPc98SpectatorPublisher({game:'02',players:count,version:3,spectatorInputOnly:true,tickMs,rules},tape,()=>status.relay,fail);
            }
            peer=createConfirmedPeerRun({count,seat,delay,identity,transport,world:player.confirmed,fail,
                sample(){
                    const bits=input.sample();
                    const buttons=(bits&15)|((bits&16)<<1)|((bits&32)>>1)|((bits&64)<<4)|((bits&128)>>1)|((bits&256)>>1);
                    return packTouch(buttons,options.touchEnabled?input.motion():undefined);
                },
                confirmed(_frame,masks){if(seat===0){rows.push(masks);publisher.kick();}},
            });
            keys=new AbortController();
            for(const target of [window,parent])target.addEventListener('keydown',player.unlock,{capture:true,signal:keys.signal});
            canvas.focus();player.unlock();peer.start();
        },
        health(){
            const h=player?.health()??{frames:0,gameFrame:0,audio:{state:'loading'}};
            const running=!!peer?.ready,frame=peer?.frame??0;
            Object.assign(globalThis,{__eaglerNetplayLanActive:running,__eaglerNetplayLanFrame:frame,
                __eaglerNetplayLanConfirmed:frame,__eaglerNetplayInputDelayFrames:delay});
            return {...h,frames:running?h.frames:0};
        },
        stop,
    };
}

export function createNativeRoomSpectator(emit){
    let player,viewer,canvas,channel,timer,run,revision=0;
    const status={peers:new Map(),get relay(){return channel.relay;}};
    const muted=()=>viewer.paused||!viewer.following||viewer.catching||viewer.seeking;
    function report(){
        const position=viewer.frame,received=viewer.received;
        player?.pausePlayback(muted());
        emit({event:'runtime-info',spectatorPlayback:{version:1,run,revision:++revision,min:viewer.min,position,received,
            edge:received,target:viewer.target,paused:viewer.paused,seeking:viewer.seeking,following:viewer.following,ended:viewer.ended,nodes:viewer.nodes}});
    }
    function stop(){
        clearInterval(timer);viewer?.stop();viewer=null;canvas?.remove();
        if(globalThis.__th02PeerTransport===status)delete globalThis.__th02PeerTransport;
        emit({event:'runtime-info',spectatorPlayback:null});
    }
    function validateHeader(h){
            if(h?.game!=='02'||h.version!==3||h.spectatorInputOnly!==true||h.tickMs!==tickMs||![2,3].includes(h.players)||
                h.rules?.length!==11||h.rules[0]!==h.players||![0,5].includes(h.rules[1])||!own(h.rules[2],0,3)||
                h.rules.slice(3,5).some(v=>!own(v,0,5))||h.rules.slice(5,8).some(v=>!own(v,0,2))||
                !own(h.rules[8],0,0xffffffff)||!own(h.rules[9],0,15)||!own(h.rules[10],0,3))throw Error('观战开局数据无效');
    }
    return {
        async start({options,music,language}){
            run='pc98_02_'+new URL(options.netplayUrl).searchParams.get('run');
            canvas=roomCanvas(document.querySelector('#game'));
            const onError=error=>{stop();emit({event:'error',message:error.message});};
            viewer=createLocalSpectator({game:'02',options,smoothPresentation:true,stable:JSON.stringify,validateHeader,
                validateRow(row,h){if(!Array.isArray(row)||row.length!==3||row.some((v,i)=>!validCoopInput(v)||(i>=h.players&&v)))throw Error('观战输入无效');},
                channelFactory(){channel=createPc98SpectatorChannel(options,onError);globalThis.__th02PeerTransport=status;return channel;},
                async createReplica(header,{onFrame}){
                    const instance=await playFull(canvas,undefined,{hosted:true,manualFrames:true,
                        confirmedCoop:{rules:[...header.rules.slice(0,-1),lang(language)],seat:-1,music:sound(music,options)}});
                    player=instance;instance.pausePlayback(muted());
                    const gestures=new AbortController();
                    for(const target of [window,parent])for(const name of ['click','keydown'])target.addEventListener(name,instance.unlock,{capture:true,signal:gestures.signal});
                    let frame=0;
                    return {get frame(){return frame;},advance(row){instance.pausePlayback(muted());instance.confirmed.step(frame++,row);},
                        present(){instance.present();onFrame(canvas);},stop(){gestures.abort();instance.stop();if(player===instance)player=null;}};
                },onError});
            timer=setInterval(report,200);
        },
        health(){return {frames:viewer?.frames??0,gameFrame:viewer?.frame??0,audio:player?.health().audio??{state:'loading'}};},
        playback(message){
            if(message.run!==run)throw Error('观战对局已改变');
            if(message.action==='pause')viewer.pause(true);
            else if(message.action==='play')viewer.pause(false);
            else if(message.action==='live')viewer.live();
            else if(message.action==='seek')viewer.seek(message.frame);
            else throw Error('观战命令无效');
            report();return {};
        },
        stop,
    };
}
