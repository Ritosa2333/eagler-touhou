import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import test from 'node:test';
import {fileURLToPath} from 'node:url';
const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
const site=process.env.TH128_DEBUG_STAGE_SITE?path.resolve(process.env.TH128_DEBUG_STAGE_SITE):path.join(root,'site');
const source=fs.readFileSync(path.join(site,'play.mjs'),'utf8');
const html=fs.readFileSync(path.join(site,'play.html'),'utf8');
const policy=new Function(source.slice(0,source.indexOf('const {installCheckpointAdapter'))+'\nreturn {assertStageStartAllowed};')();
function body(name){
    const match=new RegExp('(?:async )?function '+name+'\\(').exec(source);assert.ok(match,'missing production function '+name);
    const start=match.index,open=source.indexOf('{',source.indexOf(')',start));let depth=0,quote=null,escaped=false;
    for(let index=open;index<source.length;++index){const character=source[index];if(quote){if(escaped)escaped=false;else if(character==='\\')escaped=true;else if(character===quote)quote=null;continue;}if(character==="'"||character==='"'||character==='`'){quote=character;continue;}if(character==='{')++depth;if(character==='}'&&--depth===0)return source.slice(start,index+1);}
    throw Error('unterminated '+name);
}
const catalog=JSON.parse(fs.readFileSync(path.join(root,'local/research/stage-table.json'),'utf8'));
const state={stageCatalog:catalog};
function api(){return new Function('state','assertStageStartAllowed','resolveStage',
    ['prepareStage','debugThirdStageSelection','enemyAnimationBankLimit','verifyFreshExtraOwner'].map(body).join('\n')+'\nreturn {prepareStage,debugThirdStageSelection,enemyAnimationBankLimit,verifyFreshExtraOwner};')(state,policy.assertStageStartAllowed,(catalog,id)=>catalog.stages.find(stage=>stage.id===id));}
test('EX is an explicit visible experimental entry without normal unlock',()=>{assert.match(html,/<option value="EX">EX（调试中）<\/option>/);assert.match(html,/EX.*尚未通关/);assert.match(source,/debug-extra-start/);assert.match(source,/startDebugThirdStage\('EX'\)/);});
test('ordinary EX and foreign rank4 cannot bypass normal-clear eligibility',()=>{for(const [stage,rank,options]of [[16,1,{}],[16,4,{}],[1,4,{debugExtra:true}],[15,4,{debugExtra:true}]])assert.throws(()=>policy.assertStageStartAllowed(stage,rank,options));assert.equal(policy.assertStageStartAllowed(16,4,{debugExtra:true}),true);assert.equal(policy.assertStageStartAllowed(1,3),true);});
test('explicit debug EX uses the original stage16 rank4 and native-taint path',()=>{const selection=api().debugThirdStageSelection('EX');assert.equal(selection.stage.id,16);assert.equal(selection.stage.ecl,'st_ex.ecl');assert.equal(selection.difficulty,4);assert.equal(selection.seed,34772);assert.equal(selection.progress,0);assert.equal(selection.debugExtra,true);assert.equal(selection.debugJump.normalModeClear,false);assert.equal(selection.debugJump.route,'EX');assert.equal(selection.carryToken,undefined);assert.match(body('applyDebugInfiniteRespawn'),/runTainted/);});
test('only actual EX may load the exact four authored ANM banks in order',()=>{const helpers=api(),selection=helpers.debugThirdStageSelection('EX');const names=['enemy.anm','enemy_ll.anm','boss04.anm','enemy_bf.anm'];assert.equal(helpers.enemyAnimationBankLimit(selection,names),4);assert.throws(()=>helpers.enemyAnimationBankLimit(selection,[...names].reverse()));assert.throws(()=>helpers.enemyAnimationBankLimit(selection,[...names,'front.anm']));assert.equal(helpers.enemyAnimationBankLimit(helpers.prepareStage(1),names.slice(0,3)),3);assert.throws(()=>helpers.enemyAnimationBankLimit(helpers.prepareStage(1),names));assert.match(body('checkWorld'),/enemyAnimationBankLimit\(selection,includes\.ANIM\)/);});
function fresh(){return {frame:0,cumulativeFrozenArea:0,debug:{runTainted:true},flow:{currentId:16,difficulty:4,shotDifficulty:4,playerShotGroupOwner:{loaded:true,configured:true,nativeDefault:true,group:0,selector:0,failed:false,retired:false,activeHandles:4}}};}
test('fresh EX requires actual rank4 SHT group0 area0 and never guesses another group',()=>{const helpers=api(),selection=helpers.debugThirdStageSelection('EX');assert.equal(helpers.verifyFreshExtraOwner(fresh(),selection),true);for(const change of [value=>value.flow.difficulty=1,value=>value.flow.shotDifficulty=1,value=>value.flow.playerShotGroupOwner.group=4,value=>value.cumulativeFrozenArea=50,value=>value.debug.runTainted=false,value=>value.flow.playerShotGroupOwner.activeHandles=0,value=>value.flow.playerShotGroupOwner.failed=true]){const snapshot=fresh();change(snapshot);assert.throws(()=>helpers.verifyFreshExtraOwner(snapshot,selection));}assert.match(body('checkWorld'),/verifyFreshExtraOwner/);});
test('ordinary start remains rank0..3 and cannot acquire debug EX accidentally',()=>{const helpers=api();for(let rank=0;rank<=3;++rank){const selection=helpers.prepareStage(1,{difficulty:rank});assert.equal(selection.debugExtra,undefined);assert.equal(selection.debugJump,undefined);}assert.throws(()=>helpers.prepareStage(16,{difficulty:4}));assert.throws(()=>helpers.prepareStage(16,{difficulty:4,debugExtra:true,progress:1}));});
