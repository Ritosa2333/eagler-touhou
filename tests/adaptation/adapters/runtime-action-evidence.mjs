import assert from 'node:assert/strict';
import {resolveAdaptationAdapter} from './registry.mjs';
import {resolveTitleFlow} from './title-flow.mjs';
import {assertTouchActionEvidence} from '../../../tools/ports/touch-action-scenes.mjs';
import {assessRuntimeTouchRestart,compareRuntimeRestartIdentity} from '../../../tools/ports/runtime-touch-restart.mjs';
import {assessRestartKeyboardForwarding} from '../../../tools/ports/runtime-touch-restart-keyboard.mjs';
import {assertLauncherTouchOwnershipEvidence,assertLauncherFocusEvidence} from '../../../tools/ports/launcher-touch-scenes.mjs';
import {assertControlledScene} from './scene.mjs';
import {PRODUCT_GAMES} from '../../../common/contracts/product-catalog.mjs';
import {assessRuntimeInputEvidence} from './input-evidence.mjs';
import {assertKeyboardActionJournal} from './keyboard-layout-evidence.mjs';

export function assessRuntimeActionEvidence({driver,entry,product}={}){
  if(!['shared-touch-actions','shared-touch-restart','launcher-touch-native-consumers','shared-launcher-touch-focus','native-keyboard-actions'].includes(entry?.id))return{available:false,valid:false,kind:null};
  const result={available:true,valid:false,kind:'native-touch-actions',checks:entry.id==='native-keyboard-actions'?['INPUT-01']:entry.id==='shared-launcher-touch-focus'?['TOUCH-05','TOUCH-06']:entry.id==='launcher-touch-native-consumers'?['TOUCH-15','INPUT-05']:[entry.id==='shared-touch-actions'?'TOUCH-08':'TOUCH-18']};
  try{
    assert.equal(driver.schema,'eagler-touhou/runtime-desktop-probes/1');
    const targets=driver.targets.filter(target=>target.game===product.game&&target.variant===product.variant);assert.equal(targets.length,1);
    const target=targets[0];assert.ok(target.checks.includes(entry)&&target.checks.filter(row=>row.id===entry.id).length===1);assert.equal(entry.status,'passed');
    const adapter=resolveAdaptationAdapter(product.game,product.variant),e=entry.observation;
    if(entry.id==='shared-launcher-touch-focus'){
      assert.deepEqual(e.trials.map(trial=>trial.mode),['hold-button','toggle-button','toggle-button']);
      const epochs=e.trials.map(trial=>trial.epoch);assert.ok(epochs.every((epoch,index)=>Number.isSafeInteger(epoch)&&(!index||epoch>epochs[index-1])));
      for(const trial of e.trials){
        assert.equal(trial.transition.pagehide,true);assert.equal(trial.clock.mode,'native-manual');
        assert.equal(trial.fixture.game,product.game);assert.equal(trial.fixture.epoch,trial.epoch);assert.equal(trial.fixture.focus,trial.mode);
        assert.ok(target.gameplayReadinessByEpoch.some(row=>row.epoch===trial.epoch&&row.ready));
        const configs=target.launchOptionsByEpoch.filter(row=>row.epoch===trial.epoch);assert.equal(configs.length,1);
        assert.equal(configs[0].options.touchFocusMode,trial.mode);assert.equal(configs[0].acknowledgement.ok,true);
        let previous=assertControlledScene(trial.controlledScene.acceptance);
        for(const receipt of trial.controlledScene.receipts)previous=assertControlledScene(receipt,previous);
        assert.equal(trial.evidence.mode,trial.mode);assertLauncherFocusEvidence(trial.evidence,{inputMasks:adapter.actions.inputMasks});
        for(const operation of trial.evidence.operations.filter(row=>row.kind==='key'&&row.args[1]===true)){
          assert.equal(operation.result.acknowledged,true);assert.equal(operation.result.physicalReceipt.code,operation.args[0]);
          assert.equal(operation.result.physicalReceipt.down,true);assert.equal(operation.result.physicalReceipt.epoch,trial.epoch);
        }
      }
    }else if(['shared-touch-actions','native-keyboard-actions'].includes(entry.id)){
      assert.equal(e.clock.mode,'native-manual');assert.ok(target.gameplayReadinessByEpoch.some(row=>row.epoch===e.epoch&&row.ready));
      assertTouchActionEvidence(e.evidence,{inputMasks:adapter.actions.inputMasks,escapePulseTicks:adapter.actions.escapePulseTicks,bombResource:adapter.actions.bombResource});
      if(entry.id==='native-keyboard-actions'){
        assert.equal(e.inputSource,'physical-keyboard');
        assertKeyboardActionJournal(e.keyboard,{epoch:e.epoch,pulseTicks:adapter.actions.escapePulseTicks});
        const arrows=target.checks.filter(row=>row.id==='native-four-direction-arrows');assert.equal(arrows.length,1);assert.equal(arrows[0].status,'passed');
        const directions=arrows[0].observation.directions;
        assert.deepEqual(directions.map(row=>row.canonical),['ArrowUp','ArrowDown','ArrowLeft','ArrowRight']);
        for(const row of directions){
          assert.equal(row.held.input.value,adapter.movement.directionMasks[row.canonical]);assert.equal(row.before.input.value,0);
          assert.ok((row.held.position[row.axis]-row.before.position[row.axis])*row.sign>0);
          assert.equal(row.released.input.value,0);assert.equal(row.settled.input.value,0);assert.deepEqual(row.settled.position,row.released.position);
        }
        const menu=target.checks.filter(row=>row.id==='shared-original-menu-dialogue-input');assert.equal(menu.length,1);
        const verdict=assessRuntimeInputEvidence({driver,entry:menu[0],product});assert.equal(verdict.valid,true,verdict.reason);
      }
    }else if(entry.id==='launcher-touch-native-consumers'){
      assert.equal(e.fixture.game,product.game);assert.equal(e.fixture.epoch,e.epoch);assert.equal(e.clock.mode,'native-manual');
      assertLauncherTouchOwnershipEvidence(e,{fireMode:PRODUCT_GAMES[product.game].touchFire.mode,inputMasks:adapter.actions.inputMasks});
      for(const operation of e.operations.filter(operation=>operation.kind==='key'&&operation.args[1]===true)){
        assert.equal(operation.result?.acknowledged,true);assert.equal(operation.result.physicalReceipt.source,'physical');
        assert.equal(operation.result.physicalReceipt.code,operation.args[0]);assert.equal(operation.result.physicalReceipt.down,true);
        assert.equal(operation.result.physicalReceipt.epoch,e.epoch);
      }
      for(const operation of e.operations.filter(operation=>operation.kind==='gamepad')){
        assert.equal(operation.result.source,'production-gamepad-mapping');assert.equal(Boolean(operation.result.bits&adapter.actions.inputMasks.fire),operation.args[0]);
      }
    }else{
      assert.equal(adapter.flow.strategy,'confirm','Declare restart proof inputs for this title flow');
      const ready=resolveTitleFlow(product);
      for(const [name,source,codes] of [['keyboardBaseline','keyboard-r',['KeyR']],['touchButton','launcher-touch-button',[]]]){
        const trial=e[name];assert.equal(trial.source,source);assert.equal(trial.checkStatus,'passed');
        const epoch=trial.beforeIdentity.epoch,capture=trial.restartCapture,key=trial.before.capabilities.restartFrame;
        assert.equal(capture.operationStarted,true);assert.equal(capture.initialSample.operationStarted,false);
        assert.equal(capture.initialSample.pauseActive,true);assert.equal(capture.initialSample.frame,trial.paused.frames[key]);
        const boundary=capture.resetBoundary;assert.ok(boundary&&capture.samples.some(row=>row.index===boundary.index&&row.frame===boundary.frame&&row.ready&&row.operationStarted&&!row.pauseActive),'Native reset boundary is not in the concurrent sample ledger');
        const keyboard=assessRestartKeyboardForwarding({events:trial.keyboardEvents,epoch,expectedCodes:codes});assert.equal(keyboard.status,'passed');
        const verdict=assessRuntimeTouchRestart({...trial,resetBoundary:boundary,expectedEpoch:epoch,
          gameplayReady:{restarted:ready.isGameplayReady(trial.restarted,'desktop'),advanced:ready.isGameplayReady(trial.advanced,'desktop')}});
        assert.equal(verdict.checkStatus,'passed');
        if(source==='launcher-touch-button')assert.ok(trial.button?.down&&trial.button?.up,'Production Restart pointer action is absent');
      }
      assert.equal(compareRuntimeRestartIdentity(e.keyboardBaseline.beforeIdentity,e.touchButton.beforeIdentity,
        {expectedEpoch:e.keyboardBaseline.beforeIdentity.epoch}).status,'same');
    }
    result.valid=true;
  }catch(error){result.reason=error.message;}
  return result;
}
