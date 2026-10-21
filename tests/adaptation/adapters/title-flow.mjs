import {evaluateDesktopGameplayReadiness} from '../../../tools/ports/runtime-desktop-readiness.mjs';
import {practiceGameplayReady} from '../../../tools/ports/practice-observations.mjs';
import {PRODUCT_GAMES} from '../../../common/contracts/product-catalog.mjs';
import {ADAPTATION_ADAPTERS,resolveAdaptationAdapter} from './registry.mjs';
import {TITLE_FLOW_STEPS_SCHEMA,executeTitleFlowSteps,validateTitleFlow} from './flow-steps.mjs';

function registeredStrategy(flow){
  if(flow?.schema===TITLE_FLOW_STEPS_SCHEMA)return 'steps';
  return flow?.strategy;
}

const TITLE_ENTRY=Object.freeze({
  th06:'confirm',th07:'confirm',th09:'confirm',
  th10:'confirm',th11:'confirm',th15:'confirm',th20:'th20-menu',
  ...Object.fromEntries(ADAPTATION_ADAPTERS.products.filter(({variant})=>variant==='normal').map(({game,variant})=>[
    game,registeredStrategy(resolveAdaptationAdapter(game,variant).flow),
  ]).filter(([,strategy])=>strategy)),
});
export const TITLE_FLOW_GAMES=Object.freeze(Object.keys(TITLE_ENTRY));
const TITLE_MODES=Object.freeze(Object.fromEntries(Object.entries(TITLE_ENTRY).map(([game,normal])=>[
  game,Object.freeze({normal,multiplayer:PRODUCT_GAMES[game]?.multiplayerRuntime&&PRODUCT_GAMES[game]?.multiplayer?'room-owned':null}),
])));

function runtimeGameplayReadiness(state,game){
  const reasons=[];
  if(state?.capabilities?.input!==true||!Number.isFinite(state.input?.value))reasons.push('native-input-unavailable');
  if(state?.capabilities?.position!==true||!Number.isFinite(state.position?.x)||!Number.isFinite(state.position?.y))
    reasons.push('native-player-position-unavailable');
  if(state?.pause?.active===true)reasons.push('gameplay-not-unpaused');
  if(state?.nativeObservation&&state.localSeat!==0)reasons.push('local-seat-unavailable');
  if(state?.nativeObservation&&!Number.isFinite(state.frames?.simulation))reasons.push('native-simulation-frame-unavailable');
  const phaseReady=state?.capabilities?.phase===true?['stage','match'].includes(state.phase?.name):
    game==='th08'&&state.status?.supervisorActive===2&&state.status.active===1&&!state.status.loading;
  if(!phaseReady)reasons.push('native-gameplay-phase-unavailable');
  return{ready:reasons.length===0,reasons};
}

function isTitleInputReady(state,titleState){
  return state?.status?.titleState===titleState&&state.status.titlePhase===2;
}

function requireCallback(value,name){
  if(typeof value!=='function')throw new TypeError(`title flow requires ${name}()`);
  return value;
}

export function resolveTitleFlow({game,variant}={}){
  const modes=TITLE_MODES[game];
  if(!modes)throw new RangeError(`No adaptation flow is registered for ${String(game)}`);
  if(variant!=='normal'&&variant!=='multiplayer')
    throw new RangeError(`Adaptation flow requires explicit variant normal or multiplayer; received ${String(variant)}`);
  const adapterFlow=resolveAdaptationAdapter(game,variant)?.flow;
  const strategy=modes[variant];
  if(variant==='multiplayer'){
    if(!strategy)throw new Error(`${game}/multiplayer is not a registered product Runtime variant`);
    throw new Error(`${game}/multiplayer gameplay belongs to the multiplayer room acceptance driver; normal navigation is not reused`);
  }
  if(strategy!=='confirm'&&strategy!=='th20-menu'&&strategy!=='steps')
    throw new Error(`${game}/normal has no registered navigation strategy`);
  if(adapterFlow){
    const validation=validateTitleFlow(adapterFlow);
    if(validation.status!=='passed')throw new TypeError(`Invalid ${game}/${variant} title flow: ${validation.issues.map(item=>`${item.path}: ${item.reason}`).join('; ')}`);
    if(registeredStrategy(adapterFlow)!==strategy)throw new TypeError(`${game}/${variant} title flow strategy changed after registration`);
  }

  const flow={game,variant,
    needsControlWarmup:adapterFlow?.needsControlWarmup??!['th09','th11'].includes(game),
    gameplayReadiness(state,lane='runtime'){
      if(lane==='runtime')return runtimeGameplayReadiness(state,game);
      if(lane==='desktop')return evaluateDesktopGameplayReadiness(state,game);
      if(lane==='practice'){
        const ready=practiceGameplayReady({...state,game:state?.game??game});
        return{ready,reasons:ready?[]:['practice-native-gameplay-clock-unavailable']};
      }
      throw new RangeError(`Unknown gameplay-readiness lane ${String(lane)}`);
    },
    isGameplayReady(state,lane='runtime'){return flow.gameplayReadiness(state,lane).ready;},
    async enterGameplay({observe,press,waitUntil,lane='runtime',onTitleReady,epoch,label,timeoutMs=30000,
      waitAfterTh20Loadout=true}={}){
      requireCallback(observe,'observe');requireCallback(press,'press');
      const ready=state=>flow.isGameplayReady(state,lane);
      if(strategy==='steps'){
        return executeTitleFlowSteps({flow:adapterFlow,observe,press,waitUntil,isGameplayReady:ready,
          onTitleReady,labelSuffix:epoch===undefined?'':`epoch ${epoch}`,timeoutMs});
      }
      if(strategy==='th20-menu'){
        requireCallback(waitUntil,'waitUntil');
        const suffix=epoch===undefined?'':` epoch ${epoch}`;
        let state=await waitUntil(`TH20 title input ready${suffix}`,value=>isTitleInputReady(value,1));
        if(onTitleReady)await onTitleReady(state);
        for(let count=0;count<10&&state.status.titleCursor!==0;count++){
          await press('ArrowUp');state=await observe();
        }
        if(state.status?.titleCursor!==0)throw new Error('TH20 title must select ordinary game before confirmation');
        await press('KeyZ');
        await waitUntil(`TH20 difficulty input ready${suffix}`,value=>isTitleInputReady(value,5));
        await press('KeyZ');
        await waitUntil(`TH20 character input ready${suffix}`,value=>isTitleInputReady(value,6));
        await press('KeyZ');
        await waitUntil(`TH20 default stone input ready${suffix}`,value=>isTitleInputReady(value,7)&&
          value.ui?.stone?.state===3&&value.ui.stone.category===0);
        await press('KeyZ');
        state=await waitUntil(`TH20 loadout accepted${suffix}`,value=>
          value.status?.titleState===7&&value.status.titlePhase===3);
        if(waitAfterTh20Loadout&&state.capabilities?.input&&state.capabilities?.position)
          state=await waitUntil(label??`TH20 native gameplay ready${suffix}`,ready,timeoutMs);
        return state;
      }

      let state=await observe();
      for(let count=0;count<32&&!ready(state);count++){
        await press('KeyZ');state=await observe();
      }
      return state;
    },
    async restartFromPause({press,waitUntil,epoch}={}){
      requireCallback(press,'press');
      if(strategy==='steps'){
        return executeTitleFlowSteps({flow:adapterFlow,phase:'restart',press,waitUntil,
          isGameplayReady:state=>flow.isGameplayReady(state,'runtime'),
          labelSuffix:epoch===undefined?'':`epoch ${epoch}`});
      }
      await press('KeyR');
      if(strategy!=='th20-menu')return null;
      requireCallback(waitUntil,'waitUntil');
      const suffix=epoch===undefined?'':` epoch ${epoch}`;
      const confirmation=await waitUntil(`TH20 retry confirmation input ready${suffix}`,state=>
        state.ui?.pause?.retryConfirmInputReady===true);
      if(confirmation.ui?.pause?.cursor!==1)throw new Error('TH20 retry confirmation must initially select no');
      await press('ArrowUp');
      await waitUntil(`TH20 retry confirmation selects yes${suffix}`,state=>
        state.ui?.pause?.retryConfirmInputReady===true&&state.ui.pause.cursor===0);
      await press('KeyZ');
      return confirmation;
    },
  };
  return Object.freeze(flow);
}

export const adaptationTitleModes=TITLE_MODES;
