import assert from 'node:assert/strict';
import {readPracticeSnapshotRaw,practiceSnapshotRequest} from '../../tests/adaptation/adapters/practice.mjs';
import {pressObservedMenuKey} from './native-menu-key.mjs';
import {assertPracticeMenuInputReady} from '../../tests/adaptation/adapters/practice-ui.mjs';

export function practiceUiVirtualKey(code){
  if(/^Key[A-Z]$/.test(code))return code.charCodeAt(3);
  if(/^Digit[0-9]$/.test(code))return code.charCodeAt(5);
  const match=/^F([1-9]|1[0-2])$/.exec(code);if(match)return 111+Number(match[1]);
  const key={ArrowLeft:37,ArrowUp:38,ArrowRight:39,ArrowDown:40,Enter:13,Escape:27,Backspace:8,Tab:9,Space:32}[code];
  assert.ok(key!==undefined,'Register the native Practice UI key: '+code);return key;
}

/** Bind shared physical-key acknowledgement to the title's live Practice UI. */
export function createPracticeUiInput({page,frame,adapter,advance}){
  const practice=adapter.practice,evidence={schema:'eagler-touhou/practice-ui-input/1',keys:[],readiness:[]};
  const wait=advance??(ms=>page.waitForTimeout(ms));
  const uiRequest=practiceSnapshotRequest({runtime:adapter.runtime,contract:practice,kind:'ui'}),nativeRequest=practiceSnapshotRequest({runtime:adapter.runtime,contract:practice});
  const read=async()=>{
    const uiRaw=await frame.evaluate(readPracticeSnapshotRaw,uiRequest),ui=practice.uiSnapshot.decode(uiRaw);
    assert.ok(ui.keyboard&&ui.practiceInput,'Native Practice UI keyboard readiness is unavailable; rebuild its declared observer');
    const nativeRaw=await frame.evaluate(readPracticeSnapshotRaw,nativeRequest),native=practice.snapshot.decode(nativeRaw);
    const identity=await frame.evaluate(runtime=>({app:globalThis[runtime.global][runtime.app],documentTimeOrigin:performance.timeOrigin,url:location.href}),adapter.runtime);
    return{identity,uiRaw,ui,nativeRaw,native,screen:native.state.titleScreen,phase:native.state.titlePhase,cursor:native.state.titleCursor,
      menuOpen:ui.practiceMenuOpen,inGame:native.state.inGame};
  };
  const until=async(label,readState,predicate)=>{const end=Date.now()+5000;let sample;do{sample=await readState();if(predicate(sample))return sample;await wait(20);}while(Date.now()<end);
    throw Object.assign(Error(label),{practiceInputEvidence:sample});};
  return{evidence,read,
    async waitForPracticeInput(){
      const sample=await until('Native Practice menu has not armed its released keyboard input',read,s=>s.native.state.menu&&s.ui.practiceMenuOpen&&
        s.ui.inputGeneration===s.ui.renderedGeneration&&s.ui.practiceInput.armed&&!s.ui.practiceInput.widgetBusy&&!s.ui.practiceInput.confirmHeld&&!s.ui.practiceInput.cancelHeld);
      assertPracticeMenuInputReady(sample,{decodeNative:practice.snapshot.decode,decodeUi:practice.uiSnapshot.decode});evidence.readiness.push(sample);return sample;
    },
    async press(code){
      if(code==='Backspace')await until('Native Practice overlay hotkey is not ready',read,s=>s.native.state.inGame&&!s.ui.practiceMenuOpen&&!s.ui.textEditing&&s.ui.inputGeneration===s.ui.renderedGeneration);
      const vk=practiceUiVirtualKey(code),word=vk>>>5,mask=2**(vk&31),readKey=async()=>{const state=await read();return{...state,input:state.ui.keyboard.down[word]};};
      await frame.locator('canvas').focus();
      try{
        const receipt=await pressObservedMenuKey({now:Date.now,read:readKey,down:key=>page.keyboard.down(key),up:key=>page.keyboard.up(key),wait},
          {code,mask,inputField:'input',transitionFields:['screen','phase','cursor','inGame']});
        evidence.keys.push(receipt);receipt.release=await until('Native Practice key remains held: '+code,readKey,state=>(state.input&mask)===0);
      }catch(error){evidence.failure={error:String(error.stack||error),key:error.evidence??null,state:error.practiceInputEvidence??null};throw error;}
    },
  };
}
