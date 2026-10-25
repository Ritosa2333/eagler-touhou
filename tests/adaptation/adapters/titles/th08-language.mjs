import assert from 'node:assert/strict';
import {TITLE_FLOW_STEPS_SCHEMA} from '../flow-steps.mjs';

const uiSource='ports/th08/web/cpp/platform/BrowserExports.cpp | language_ui_snapshot_v1';
const titleSource='ports/th08/web/cpp/game/TitleMenus.cpp | TITLE_MENU_ITEM_START_RESULT';
const resultSource='ports/th08/web/cpp/game/SupervisorFrame.hpp | Scene::Results | ResultControls.cpp | HandleCategorySelectScreen | ResultStatistics.cpp | HandleOtherStatsScreen';
const eq=(name,value)=>({path:[name],op:'eq',value});
const predicate=(condition,source)=>({condition,source});
const all=(...values)=>({all:values});
const titleReady=all(eq('scene',1),eq('titleReady',true),eq('loading',false));
const resultReady=all(eq('scene',5),eq('resultActive',true),eq('resultState',1),eq('resultDepth',1));
const statsReady=all(eq('scene',5),eq('resultActive',true),eq('resultState',20));
const flow=(enter)=>({schema:TITLE_FLOW_STEPS_SCHEMA,needsControlWarmup:false,budget:{timeoutMs:30000,maxSteps:12,maxPresses:20},predicates:{
  titleReady:predicate(titleReady,titleSource),titleChoice:predicate(all(titleReady,eq('titleCursor',5)),titleSource),
  titleStart:predicate(all(titleReady,eq('titleCursor',0)),titleSource),resultReady:predicate(resultReady,resultSource),
  statsChoice:predicate(all(resultReady,eq('resultCursor',2)),resultSource),exitChoice:predicate(all(resultReady,eq('resultCursor',3)),resultSource),
  statsReady:predicate(statsReady,resultSource)},enter});
const wait=(until,source)=>({op:'wait',until,source});
const press=(input,when,source)=>({op:'press',input,when,source});
const seek=(input,when,until,maxPresses,source)=>({op:'press-until',input,when,until,maxPresses,source});
export const TH08_LANGUAGE_CONSUMERS={
  uiSnapshot:{export:'language_ui_snapshot_v1',wordCount:20,wordType:'f64',app:true,source:uiSource,booleans:['active','loading','titleReady','resultActive','inGame'],fields:{
    scene:2,active:3,loading:4,titleScreen:5,titlePhase:6,titleCursor:7,titleReady:8,resultActive:9,resultState:10,resultDepth:11,resultCursor:12,resultFrame:13,resultFrame2:14,inGame:15,
    inputCurrent:16,inputPrevious:17,dialogueAlpha0:18,dialogueAlpha1:19}},
  navigation:{inputField:'inputCurrent',transitionFields:['scene','titleScreen','resultState','titleCursor','resultCursor'],
    masks:{KeyZ:1,KeyX:2,ArrowUp:16,ArrowDown:32},source:'TitleContext::input | ResultContext::input | InputFrame current'},
  consumers:{statistics:{source:resultSource,
    enter:flow([wait('titleReady',titleSource),seek('ArrowDown','titleReady','titleChoice',8,titleSource),press('KeyZ','titleChoice',titleSource),
      wait('resultReady',resultSource),seek('ArrowDown','resultReady','statsChoice',3,resultSource),press('KeyZ','statsChoice',resultSource),wait('statsReady',resultSource)]),
    exit:flow([wait('statsReady',resultSource),press('KeyX','statsReady',resultSource),wait('resultReady',resultSource),
      seek('ArrowDown','resultReady','exitChoice',3,resultSource),press('KeyZ','exitChoice',resultSource),wait('titleReady',titleSource),
      seek('ArrowUp','titleReady','titleStart',8,titleSource),wait('titleStart',titleSource)]),
    samples:[{operation:'string-by-id',key:'th08 Stats Play Count',originalHex:'8376838c834389f19094814081408140208140456173792081404e6f726d208140486172642081404c756e61202045787472612020546f74616c',
      source:'ports/th08/web/cpp/game/ResultText.hpp | play_count | ResultStatistics.cpp | textVms[2]'}]},
    dialogue:{source:'ports/th08/web/cpp/game/Dialogue.cpp | line | MessageProgram.cpp | text | PlayerLife.cpp | alive',archiveKind:'pbgz',mountRoot:'/thcrap/th08/',gameScene:2,maxReadyTicks:400,
      captureWaitTicks:30,textAlphaFields:['dialogueAlpha0','dialogueAlpha1'],minimumTextAlpha:200,
      assets:['ecldata1.ecl','msg1a.dat','msg1b.dat','msg1c.dat','msg1d.dat']},
  },
};

/** Decode original source MSG text independently of the native MessageProgram. */
export function parseTh08DialogueText(input,entry){
  const bytes=Buffer.from(input);assert.ok(bytes.length>=8&&bytes.length<=16*1024*1024);
  const count=bytes.readUInt32LE(0);assert.ok(count>0&&count<=4096&&4+count*4<=bytes.length);
  assert.ok(Number.isSafeInteger(entry)&&entry>=0&&entry<count);
  const offsets=Array.from({length:count},(_,i)=>bytes.readUInt32LE(4+i*4));
  assert.ok(offsets.every(at=>at>=4+count*4&&at<=bytes.length-4));
  const start=offsets[entry],end=Math.min(bytes.length,...offsets.filter(at=>at>start)),rows=[];
  for(let at=start;at<end;){
    assert.ok(at+4<=end);const time=bytes.readUInt16LE(at),opcode=bytes[at+2],size=bytes[at+3],next=at+4+size;assert.ok(next<=end);
    if([3,16,19,20].includes(opcode)){
      const prefix=opcode===3?4:0;assert.ok(size>prefix);const text=Buffer.from(bytes.subarray(at+4+prefix,next));
      for(let i=0;i<text.length;i++)text[i]^=0x77;
      const nul=text.indexOf(0);assert.ok(nul>=0,'Original dialogue string is not terminated');
      if(nul)rows.push({offset:at,time,opcode,textHex:text.subarray(0,nul).toString('hex')});
    }
    if(opcode===4)break;at=next;
  }
  assert.ok(rows.length,'Selected source dialogue has no text before its initial Wait');return{entry,rows};
}

/** Parse only diagnostics emitted by TH08's localization activation and font fallback owners. */
export function classifyTh08LanguageDiagnostic({text,fontPaths=[]}={}){
  if(typeof text!=='string')return null;
  const activation=text.match(/^th08 thcrap localization active: font=(.+?) \((.+)\) utf8=(ok|FAILED) spells=(\d+) stages=(\d+) themes=(\d+) musiccmt=(\d+) spellcomments=(\d+) ascii=(\d+) strings=(\d+)$/);
  if(activation){
    const [,font,fontName,utf8,...counts]=activation;
    if(!font.trim()||!fontName.trim())return null;
    const diagnostic={font,fontName,utf8,counts:Object.fromEntries(['spells','stages','themes','musicComments','spellComments','ascii','strings']
      .map((name,index)=>[name,Number(counts[index])]))};
    return{classification:'localization-activation',disposition:utf8==='ok'?'expected':'blocking',diagnostic};
  }
  const fallback=text.match(/^th08 thcrap font fallback: U\+([0-9A-F]{4}) from pack font (\/thcrap\/th08\/fonts\/[A-Za-z0-9_.-]+\.(?:otf|ttf|ttc))$/);
  if(!fallback)return null;
  const codepoint=Number.parseInt(fallback[1],16),font=fallback[2];
  if(codepoint<32||(codepoint>=0xd800&&codepoint<=0xdfff)||!Array.isArray(fontPaths)||!fontPaths.includes(font))
    return{classification:'pack-font-fallback',disposition:'blocking',codepoint,font};
  return{classification:'pack-font-fallback',disposition:'expected',codepoint,font};
}
