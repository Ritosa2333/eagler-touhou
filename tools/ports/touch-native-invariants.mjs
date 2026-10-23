import assert from 'node:assert/strict';
import {existsSync} from 'node:fs';
import {mkdtemp,readFile,rm,writeFile,mkdir} from 'node:fs/promises';
import {dirname,join,resolve} from 'node:path';
import {tmpdir} from 'node:os';
import {fileURLToPath} from 'node:url';
import {stat} from 'node:fs/promises';
import {runNativeCpp} from '../../launcher/tests/support/native-cpp.mjs';
import {runWithReport} from '../testing/cli-report.mjs';
import {assertCliArguments} from '../testing/cli-options.mjs';
import {PRODUCT_GAMES} from '../../common/contracts/product-catalog.mjs';
import {WORKSPACE_REPOSITORIES,workspacePath} from '../../launcher/lib/workspace-layout.mjs';

const fixturePath=fileURLToPath(new URL('./fixtures/touch-native-invariants.cpp',import.meta.url));

export function productTouchMatrix(products=PRODUCT_GAMES,repositories=WORKSPACE_REPOSITORIES){
  return Object.entries(products).flatMap(([game,product])=>{
    if(product.inputProfile==='fighting')return [];
    const normal={game,variant:'normal',sourceOwner:game};
    if(!product.multiplayerRuntime)return [normal];
    const multiplayerOwner=`${game}mp`;
    return [normal,{game,variant:'multiplayer',sourceOwner:Object.hasOwn(repositories,multiplayerOwner)?multiplayerOwner:game}];
  });
}

function occurrences(text,needle){return text.split(needle).length-1;}
function replaceExact(text,needle,replacement,count=1){
  assert.equal(occurrences(text,needle),count,`Expected ${count} exact mutation anchor(s)`);
  return text.split(needle).join(replacement);
}
function failureStderr(error){
  const stderr=error?.stderr;
  return Buffer.isBuffer(stderr)?stderr.toString('utf8'):String(stderr??'');
}
async function expectMutationToFail(label,source,includes,invariant){
  let failure;
  try{await runNativeCpp(source,{includes});}catch(error){failure=error;}
  assert.ok(failure,`${label} survived the production fixture`);
  assert.ok(failureStderr(failure).includes(`TOUCH_NATIVE_FAIL:${invariant}`),
    `${label} did not reach its fixed ${invariant} assertion: ${failureStderr(failure)}`);
}

async function withTemporaryTree(action){
  const parent=resolve(tmpdir()),directory=await mkdtemp(join(parent,'touch-native-invariants-'));
  try{return await action(directory);}
  finally{
    const absolute=resolve(directory);
    if(dirname(absolute)!==parent)throw new Error(`Unexpected native fixture temp path: ${absolute}`);
    await rm(absolute,{recursive:true,force:true});
  }
}

export function sourceOwners(matrix){
  const owners=new Map();
  for(const item of matrix){
    const variants=owners.get(item.sourceOwner)??[];
    variants.push(item.variant);
    owners.set(item.sourceOwner,variants);
  }
  return [...owners.entries()].map(([sourceOwner,variants])=>({sourceOwner,variants:[...new Set(variants)]}));
}

function legacyReference(game){
  const paths={
    th06:['src/Touch.cpp:828-837','src/Touch.cpp:1005-1045','src/Player.cpp:2170,2227-2294'],
    th07:['src/Touch.cpp:825-834','src/Touch.cpp:1002-1041','src/Player.cpp:2743,2790-2880'],
  };
  return paths[game]??null;
}

export function classifyTouchSourceOwner(owner,{controllerExists,motionExists}){
  const missing=[
    ...(!controllerExists?['portable/input/TouchController.hpp']:[]),
    ...(!motionExists?['portable/input/MotionTrack.hpp']:[]),
  ];
  if(!missing.length)return{...owner,status:'production-fixture',missing};
  const reference=missing.length===2?legacyReference(owner.sourceOwner):null;
  if(reference)return{...owner,status:'reference-only',missing,reference};
  return{...owner,status:'incomplete',missing};
}

export function requireTouchSourceCoverage(owners){
  assert.ok(owners.length>0,'Product catalog contains no STG touch source owners');
  const incomplete=owners.filter(owner=>owner.status!=='production-fixture'&&owner.status!=='reference-only');
  assert.equal(incomplete.length,0,
    `Production touch components are required for source owners: ${incomplete.map(owner=>owner.sourceOwner).join(', ')}`);
  assert.ok(owners.some(owner=>owner.status==='production-fixture'),
    'At least one product source owner must execute the production touch fixture');
  return owners;
}

async function runOwnerFixture(owner,{fixture,consumerOwner,overrides=[]}){
  const inputRoot=workspacePath(owner,'portable','input');
  const includes=[...overrides,inputRoot,workspacePath('common','include')];
  const consumer=owner===consumerOwner;
  if(consumer)includes.push(workspacePath('th08mp','web','cpp','multiplayer'),
    workspacePath('th08mp','web','cpp','game'),workspacePath('th08mp','web','cpp','third_party'));
  const source=consumer?`#define TOUCH_NATIVE_WITH_TH08_MP_CONSUMER 1\n${fixture}`:fixture;
  try{return await runNativeCpp(source,{includes});}
  catch(error){error.message=`${owner}: ${error.message}`;throw error;}
}

async function motionTrackHasUnlimitedMarker(owner){
  const source=await readFile(workspacePath(owner,'portable','input','MotionTrack.hpp'),'utf8');
  return /cheat_movement_used\s*=/.test(source)&&/mark_cheat_movement\s*\(/.test(source);
}

async function runTh10MpConsumerFixture(fixture,overrides=[]){
  const owner='th10mp';
  const includes=[...overrides,workspacePath(owner,'portable','input'),
    workspacePath(owner,'web','cpp','multiplayer'),workspacePath(owner,'web','cpp','game'),
    workspacePath('common','include'),workspacePath('common','src','netplay')];
  return runNativeCpp('#define TOUCH_NATIVE_WITH_TH10_MP_CONSUMER 1\n'+fixture,{includes});
}

async function mutations(fixture,consumerOwner,th10MpOwner){
  const baseInput=workspacePath(consumerOwner,'portable','input');
  const controllerPath=resolve(baseInput,'TouchController.hpp');
  const motionPath=resolve(baseInput,'MotionTrack.hpp');
  const controller=await readFile(controllerPath,'utf8');
  const motion=await readFile(motionPath,'utf8');
  const common=workspacePath('common','include');

  await withTemporaryTree(async directory=>{
    const includeRoot=resolve(directory,'controller');await mkdir(includeRoot,{recursive:true});
    const sensitivity=replaceExact(controller,'*sensitivity*scale,','*1.f*scale,',2);
    await writeFile(resolve(includeRoot,'TouchController.hpp'),sensitivity);
    await expectMutationToFail('sensitivity removal',`#define TOUCH_NATIVE_WITH_TH08_MP_CONSUMER 1\n${fixture}`,
      [includeRoot,baseInput,workspacePath('th08mp','web','cpp','multiplayer'),
        workspacePath('th08mp','web','cpp','game'),workspacePath('th08mp','web','cpp','third_party'),common],
      'sensitivity_target_x');
  });

  await withTemporaryTree(async directory=>{
    const includeRoot=resolve(directory,'controller');await mkdir(includeRoot,{recursive:true});
    const rounded=controller
      .replace('target_x=std::clamp(target_x+std::clamp((x-previous_x)*640,-640.f,640.f)*sensitivity*scale,s.min_x,s.max_x);',
        'target_x=std::clamp(target_x+std::round(std::clamp((x-previous_x)*640,-640.f,640.f)*sensitivity*scale),s.min_x,s.max_x);')
      .replace('target_y=std::clamp(target_y+std::clamp((y-previous_y)*480,-480.f,480.f)*sensitivity*scale,s.min_y,s.max_y);',
        'target_y=std::clamp(target_y+std::round(std::clamp((y-previous_y)*480,-480.f,480.f)*sensitivity*scale),s.min_y,s.max_y);');
    assert.notEqual(rounded,controller,'Subpixel mutation anchor was not found');
    await writeFile(resolve(includeRoot,'TouchController.hpp'),rounded);
    await expectMutationToFail('subpixel rounding',fixture,[includeRoot,baseInput,common],'subpixel_target_x');
  });

  await withTemporaryTree(async directory=>{
    const includeRoot=resolve(directory,'motion');await mkdir(includeRoot,{recursive:true});
    const limited='inline void limit_vector(float& x,float& y,float speed){const float square=x*x+y*y;if(square>speed*speed&&square>0){const float scale=speed/std::sqrt(square);x*=scale;y*=scale;}}';
    const noLimit=replaceExact(motion,limited,'inline void limit_vector(float&,float&,float){}');
    await writeFile(resolve(includeRoot,'MotionTrack.hpp'),noLimit);
    await expectMutationToFail('limit-vector removal',fixture,[includeRoot,baseInput,common],'motion_limit_helper');
  });

  await withTemporaryTree(async directory=>{
    const tempInput=resolve(directory,'portable','input');
    const tempMultiplayer=resolve(directory,'web','cpp','multiplayer');
    await mkdir(tempInput,{recursive:true});await mkdir(tempMultiplayer,{recursive:true});
    await writeFile(resolve(tempInput,'MotionTrack.hpp'),motion);
    const sourcePath=workspacePath('th08mp','web','cpp','multiplayer','AnalogMovement.cpp');
    const source=await readFile(sourcePath,'utf8');
    const weakened=replaceExact(source,'if(!input.unlimited)touhou::input::limit_vector(x,y,speed);',
      'if(!input.unlimited&&false)touhou::input::limit_vector(x,y,speed);');
    await writeFile(resolve(tempMultiplayer,'AnalogMovement.cpp'),weakened);
    await writeFile(resolve(tempMultiplayer,'AnalogMovement.hpp'),
      await readFile(workspacePath('th08mp','web','cpp','multiplayer','AnalogMovement.hpp'),'utf8'));
    const consumerIncludes=[tempInput,baseInput,tempMultiplayer,
      workspacePath('th08mp','web','cpp','multiplayer'),workspacePath('th08mp','web','cpp','game'),
      workspacePath('th08mp','web','cpp','third_party'),common];
    await expectMutationToFail('actual ResolveAnalogMovement cap removal',
      `#define TOUCH_NATIVE_WITH_TH08_MP_CONSUMER 1\n${fixture}`,consumerIncludes,'logic_step_speed_x');
  });

  await withTemporaryTree(async directory=>{
    const includeRoot=resolve(directory,'controller');await mkdir(includeRoot,{recursive:true});
    const unreleased=replaceExact(controller,'if(primary==id)clear_motion();return;','if(primary==-1)clear_motion();return;');
    await writeFile(resolve(includeRoot,'TouchController.hpp'),unreleased);
    await expectMutationToFail('release ownership regression',fixture,[includeRoot,baseInput,common],
      'pointer_up_releases_drag');
  });

  await withTemporaryTree(async directory=>{
    const includeRoot=resolve(directory,'controller');await mkdir(includeRoot,{recursive:true});
    const noDigitalMovement=replaceExact(controller,
      'else if(mode>=2){out.keys[37]=out.keys[37]||stick_x<-.25f;out.keys[39]=out.keys[39]||stick_x>.25f;out.keys[38]=out.keys[38]||stick_y<-.25f;out.keys[40]=out.keys[40]||stick_y>.25f;}',
      'else if(mode>=2){}');
    await writeFile(resolve(includeRoot,'TouchController.hpp'),noDigitalMovement);
    await expectMutationToFail('digital joystick disconnected from logical directions',
      `#define TOUCH_NATIVE_WITH_TH08_MP_CONSUMER 1\n${fixture}`,
      [includeRoot,baseInput,workspacePath('th08mp','web','cpp','multiplayer'),
        workspacePath('th08mp','web','cpp','game'),workspacePath('th08mp','web','cpp','third_party'),common],
      'digital_joystick_right_up');
  });

  await withTemporaryTree(async directory=>{
    const includeRoot=resolve(directory,'controller');await mkdir(includeRoot,{recursive:true});
    const noFocusScaling=replaceExact(controller,
      'const bool slow=focus||(two_finger&&fingers.size()>1)||key_slow;',
      'const bool slow=(two_finger&&fingers.size()>1)||key_slow;');
    await writeFile(resolve(includeRoot,'TouchController.hpp'),noFocusScaling);
    await expectMutationToFail('focus removed from native touch target scaling',
      `#define TOUCH_NATIVE_WITH_TH08_MP_CONSUMER 1\n${fixture}`,
      [includeRoot,baseInput,workspacePath('th08mp','web','cpp','multiplayer'),
        workspacePath('th08mp','web','cpp','game'),workspacePath('th08mp','web','cpp','third_party'),common],
      'focused_drag_target_x');
  });

  await withTemporaryTree(async directory=>{
    const includeRoot=resolve(directory,'controller');await mkdir(includeRoot,{recursive:true});
    const noBomb=replaceExact(controller,'out.keys[88]=s.context==1&&bomb;','out.keys[88]=false;');
    await writeFile(resolve(includeRoot,'TouchController.hpp'),noBomb);
    await expectMutationToFail('Bomb action disconnected from native logical input',
      `#define TOUCH_NATIVE_WITH_TH08_MP_CONSUMER 1\n${fixture}`,
      [includeRoot,baseInput,workspacePath('th08mp','web','cpp','multiplayer'),
        workspacePath('th08mp','web','cpp','game'),workspacePath('th08mp','web','cpp','third_party'),common],
      'fire_focus_bomb_pause_use_logical_buttons');
  });

  await withTemporaryTree(async directory=>{
    const includeRoot=resolve(directory,'controller');await mkdir(includeRoot,{recursive:true});
    const bombRequiresReady=replaceExact(controller,'out.keys[88]=s.context==1&&bomb;',
      'out.keys[88]=s.context==1&&s.ready&&bomb;');
    await writeFile(resolve(includeRoot,'TouchController.hpp'),bombRequiresReady);
    await expectMutationToFail('Bomb action incorrectly gated by player readiness',fixture,
      [includeRoot,baseInput,common],'bomb_pulse_during_blocked_movement');
  });

  await withTemporaryTree(async directory=>{
    const includeRoot=resolve(directory,'controller');await mkdir(includeRoot,{recursive:true});
    const noChoiceNavigation=replaceExact(controller,
      'if(s.choice){const int key=dialogue.navigate();if(key)out.keys[key]=true;}','if(s.choice){}');
    await writeFile(resolve(includeRoot,'TouchController.hpp'),noChoiceNavigation);
    await expectMutationToFail('choice dialogue no longer emits its one-shot logical direction',
      `#define TOUCH_NATIVE_WITH_TH08_MP_CONSUMER 1\n${fixture}`,
      [includeRoot,baseInput,workspacePath('th08mp','web','cpp','multiplayer'),
        workspacePath('th08mp','web','cpp','game'),workspacePath('th08mp','web','cpp','third_party'),common],
      'choice_swipe_one_shot');
  });

  await withTemporaryTree(async directory=>{
    const includeRoot=resolve(directory,'controller');await mkdir(includeRoot,{recursive:true});
    const modalInheritsFire=replaceExact(controller,
      'out.keys[90]=(s.context==1&&fire)||out.keys[90];','out.keys[90]=fire||out.keys[90];');
    await writeFile(resolve(includeRoot,'TouchController.hpp'),modalInheritsFire);
    await expectMutationToFail('gameplay fire leaks into modal before a new touch confirm',fixture,
      [includeRoot,baseInput,common],'modal_does_not_inherit_gameplay_fire');
  });

  await withTemporaryTree(async directory=>{
    const includeRoot=resolve(directory,'controller');await mkdir(includeRoot,{recursive:true});
    const noDoubleTapBomb=replaceExact(controller,'bomb_ticks=3;tap_armed=false;tap={};return;',
      'tap_armed=false;tap={};return;');
    await writeFile(resolve(includeRoot,'TouchController.hpp'),noDoubleTapBomb);
    await expectMutationToFail('secondary double-tap no longer emits Bomb',
      `#define TOUCH_NATIVE_WITH_TH08_MP_CONSUMER 1\n${fixture}`,
      [includeRoot,baseInput,workspacePath('th08mp','web','cpp','multiplayer'),
        workspacePath('th08mp','web','cpp','game'),workspacePath('th08mp','web','cpp','third_party'),common],
      'double_tap_bomb_keeps_primary_movement');
  });

  if(await motionTrackHasUnlimitedMarker(consumerOwner)){
    await withTemporaryTree(async directory=>{
      const includeRoot=resolve(directory,'motion');await mkdir(includeRoot,{recursive:true});
      const noUnlimitedMarker=replaceExact(motion,
        'void mark_cheat_movement(float x,float y){if(unlimited&&(x!=0||y!=0))cheat_movement_used=true;}',
        'void mark_cheat_movement(float,float){}');
      await writeFile(resolve(includeRoot,'MotionTrack.hpp'),noUnlimitedMarker);
      await expectMutationToFail('nonzero unlimited consumption no longer marks the run',fixture,
        [includeRoot,baseInput,common],
        'nonzero_unlimited_movement_marks_use');
    });
  }
  const inputLanesPath=resolve(workspacePath(th10MpOwner,'web','cpp','multiplayer'),'InputLanes.cpp');
  const inputLanes=await readFile(inputLanesPath,'utf8');
  await withTemporaryTree(async directory=>{
    const overrideRoot=resolve(directory,'multiplayer');await mkdir(overrideRoot,{recursive:true});
    const quantizeCall='if (!quantize_limited()) return false;';
    assert.equal(occurrences(inputLanes,quantizeCall),3,'TH10-MP quantization call inventory changed');
    const first=inputLanes.indexOf(quantizeCall);
    const direct=inputLanes.indexOf(quantizeCall,first+quantizeCall.length);
    assert.ok(direct>=0,'TH10-MP DirectTouch quantization mutation anchor was not found');
    const rounded=inputLanes.slice(0,direct)+
      'x=static_cast<i32>(std::round(dx)); y=static_cast<i32>(std::round(dy));'+
      inputLanes.slice(direct+quantizeCall.length);
    await writeFile(resolve(overrideRoot,'InputLanes.cpp'),rounded);
    await expectMutationToFail('TH10-MP post-round speed-cap regression',
      '#define TOUCH_NATIVE_WITH_TH10_MP_CONSUMER 1\n'+fixture,
      [overrideRoot,workspacePath(th10MpOwner,'portable','input'),
        workspacePath(th10MpOwner,'web','cpp','multiplayer'),workspacePath(th10MpOwner,'web','cpp','game'),
        workspacePath('common','include'),workspacePath('common','src','netplay')],
      'th10_mp_quantized_speed_cap');
  });
  return ['sensitivity-removal','subpixel-rounding','limit-vector-removal',
    'actual-th08-consumer-cap-removal','release-ownership-regression',
    'digital-joystick-direction-removal','focus-target-scaling-removal','bomb-logical-key-removal',
    'bomb-blocked-movement-gate','choice-swipe-removal','modal-confirm-edge-removal',
    'secondary-double-tap-bomb-removal','unlimited-processing-drop-marker-removal',
    'actual-th10-mp-post-round-cap-removal'];
}

export async function checkTouchNativeInvariants(){
  return runTouchNativeInvariants();
}

export async function runTouchNativeInvariant({output,validate=checkTouchNativeInvariants}){
  return runWithReport({output,schema:'eagler-touhou/touch-native-invariants/1'},async(report,save)=>{
    report.scope='native-production-touch-components';
    report.componentStatus='running';
    report.completeRuntimeCoverage=false;
    report.runtimeCoverageStatus='incomplete';
    try{
      const evidence=await validate();
      Object.assign(report,evidence);
      report.scope='native-production-touch-components';
      report.componentEvidenceStatus=evidence.status;
      report.status='passed';
      report.passed=true;
      report.componentStatus='passed';
      report.completeRuntimeCoverage=false;
      report.runtimeCoverageStatus='incomplete';
      await save();
    }catch(error){
      report.status='failed';
      report.passed=false;
      report.componentStatus='failed';
      report.completeRuntimeCoverage=false;
      report.runtimeCoverageStatus='incomplete';
      throw error;
    }
  });
}

async function runTouchNativeInvariants(){
  const matrix=productTouchMatrix();
  const owners=sourceOwners(matrix),fixtures=[],referenceOnlyOwners=[],unlimitedMarkerGaps=[],choiceSwipeGaps=[];
  const fixture=await readFile(fixturePath,'utf8');
  const classifiedOwners=requireTouchSourceCoverage(owners.map(owner=>classifyTouchSourceOwner(owner,{
    controllerExists:existsSync(workspacePath(owner.sourceOwner,'portable','input','TouchController.hpp')),
    motionExists:existsSync(workspacePath(owner.sourceOwner,'portable','input','MotionTrack.hpp')),
  })));
  for(const owner of classifiedOwners){
    if(owner.status==='reference-only'){
      for(const item of owner.reference){const relative=item.split(':')[0];assert.ok(existsSync(workspacePath(owner.sourceOwner,relative)),`Missing accepted reference: ${owner.sourceOwner}/${relative}`);}
      referenceOnlyOwners.push(owner);
      continue;
    }
    let output=await runOwnerFixture(owner.sourceOwner,{fixture,consumerOwner:'th08mp'});
    const markerExpected=await motionTrackHasUnlimitedMarker(owner.sourceOwner);
    const markerExecuted=output.includes('unlimited processing-drop marker API executed');
    const markerExplicitlyUnavailable=output.includes('unlimited processing-drop marker API unavailable in production MotionTrack');
    assert.equal(markerExecuted,markerExpected,
      `${owner.sourceOwner}: native MotionTrack marker behavior must execute exactly when the production API exposes it`);
    assert.equal(markerExplicitlyUnavailable,!markerExpected,
      `${owner.sourceOwner}: absent marker APIs must be reported as a component gap`);
    const choiceSwipeExecuted=output.includes('touch native behavior: choice-swipe-one-shot');
    const choiceSwipeUnavailable=output.includes('touch native behavior unavailable: choice-swipe-one-shot');
    assert.notEqual(choiceSwipeExecuted,choiceSwipeUnavailable,
      `${owner.sourceOwner}: choice swipes must execute or report the production-state capability gap`);
    const controllerBehaviors=output.split(/\r?\n/)
      .filter(line=>line.startsWith('touch native behavior: '))
      .map(line=>line.slice('touch native behavior: '.length));
    assert.deepEqual(controllerBehaviors.filter(behavior=>[
      'joystick-digital-and-free','focus-actions-and-drag-ownership','bomb-pulse-during-blocked-movement',
      'secondary-double-tap-bomb','menu-confirm-cancel-and-dialogue','modal-confirm-fresh-sample-edge',
      'unlimited-mode-tag','pointer-up-and-cancel-release',
      'choice-swipe-one-shot',
    ].includes(behavior)),[
      'joystick-digital-and-free','focus-actions-and-drag-ownership','bomb-pulse-during-blocked-movement',
      'secondary-double-tap-bomb','menu-confirm-cancel-and-dialogue','modal-confirm-fresh-sample-edge',
      ...(choiceSwipeExecuted?['choice-swipe-one-shot']:[]),
      'unlimited-mode-tag','pointer-up-and-cancel-release',
    ],`${owner.sourceOwner}: required production behaviors must reach their executable fixture assertions`);
    let actualConsumer=owner.sourceOwner==='th08mp'?'th08-mp ResolveAnalogMovement':'none';
    if(owner.sourceOwner==='th10mp'){
      const consumerOutput=await runTh10MpConsumerFixture(fixture);
      output+='\n'+consumerOutput;
      actualConsumer='th10-mp InputLanes::ResolveMovement';
    }
    if(!markerExpected)unlimitedMarkerGaps.push({sourceOwner:owner.sourceOwner,variants:owner.variants,
      behavior:'unlimited-cheat-processing-drop-marker',status:'not-present-in-production-MotionTrack',
      source:`${owner.sourceOwner}/portable/input/MotionTrack.hpp`});
    if(choiceSwipeUnavailable)choiceSwipeGaps.push({sourceOwner:owner.sourceOwner,variants:owner.variants,
      behavior:'choice-dialogue-swipe',status:'not-representable-by-production-TouchState',
      source:`${owner.sourceOwner}/portable/input/TouchController.hpp`});
    fixtures.push({sourceOwner:owner.sourceOwner,variants:owner.variants,output:output.trim(),actualConsumer,
      controllerBehaviors,
      choiceSwipe:choiceSwipeExecuted?'executed':'not-representable-by-production-state',
      unlimitedMarker:markerExpected?'executed':'not-present-in-production-MotionTrack'});
  }
  const consumerOwner='th08mp';
  const th10MpOwner='th10mp';
  const killed=await mutations(fixture,consumerOwner,th10MpOwner);
  return {
    schema:'eagler-touhou/touch-native-invariants/1',
    status:'production-components-verified-runtime-coverage-incomplete',
    completeRuntimeCoverage:false,
    productVariantCount:matrix.length,
    productVariants:matrix,
    nativeComponentOwners:fixtures,
    referenceOnlyOwners,
    reference:'th06 fixed expectations: (8,6), (16,12), (24,18) targets; speed-4 applied delta (3.2,2.4); the over-cap remainder keeps the unapplied target.',
    th10FixedPointQuantization:{
      sourceOwner:'th10mp',
      consumer:'InputLanes::ResolveMovement',
      input:{mode:'DirectTouch',targetDeltaHundredths:[293,273],speedHundredths:400,rate:1},
      preChangeObserved:{roundedVector:[293,273],length:400.472229,exceedsSpeed:true},
      acceptedVector:{fixedHundredths:[292,273],length:399.7412,withinSpeed:true},
      mutationProof:'actual-th10-mp-post-round-cap-removal',
    },
    assertions:['sensitivity-100-200-300-target-scaling','arbitrary-angle-speed-cap-per-consumer-call',
      'subpixel-movement-unchanged-below-cap','capped-target-remainder-preserved','pointer-up-release','pointer-cancel-release',
      'joystick-digital-directions-and-dead-zone','joystick-free-direction-and-focus-scaling',
      'fire-focus-bomb-pause-logical-buttons','action-pulses-do-not-retire-drag-owner','secondary-double-tap-bomb-keeps-primary-drag',
      'menu-tap-confirm-two-finger-cancel','modal-confirm-after-held-gameplay-fire','choice-swipe-one-shot','dialogue-short-confirm-long-skip',
      'unlimited-processing-drop-marker-api-limited-zero-and-nonzero-unlimited-vectors'],
    incompleteBehaviorProofs:[
      {behavior:'touch-confirm-runtime-edge',status:'runtime-consumer-observation-required',reason:'The component proves gameplay Fire is neutral in modal context and a new touch confirm emits a new sample transition; the product input consumer must prove it observes that transition as a fresh logical edge.'},
      {behavior:'deathbomb-window-preservation',status:'title-runtime-observation-required',reason:'The component verifies that Bomb remains available while player movement is blocked; each title’s original hit and Deathbomb timing is validated at its Gameplay consumer.'},
      {behavior:'unlimited-processing-drop-end-to-end',status:'runtime-consumer-observation-required',reason:'The component executes each available production MotionTrack marker API with independent limited, zero-unlimited and nonzero-unlimited vectors; packaged movement-to-result/Replay consumption is a Runtime behavior.'},
      {behavior:'touch-buttons-to-gameplay-consumption',status:'runtime-observation-required',reason:'Controller key bits do not prove each product’s native menu, Bomb, pause, Result or Replay consumer.'},
      {behavior:'gamepad-to-gameplay-effects',status:'runtime-observation-required',reason:'Existing Launcher mapping and owner tests prove configured sampling and native bridge shape; this native touch fixture does not run a packaged Runtime.'},
      {behavior:'touch-layout-restart-magnifier-game-coordinates',status:'product-consumer-observation-required',reason:'Launcher component tests cover shared UI behavior; product WASM must still prove the R-key consumer and coordinate mapping.'},
    ],
    unlimitedMarkerGaps,
    choiceSwipeGaps,
    mutationProofs:killed,
    runtimeTrace:'not-proven-by-this-component-fixture',
    gaps:[...referenceOnlyOwners,...unlimitedMarkerGaps,...choiceSwipeGaps,{sourceOwner:'all-runtime-adapters',status:'requires-root-frame-trace',
      detail:'The native fixture executes production portable components and selected production movement consumers; it does not execute every product Runtime or prove every logical frame.'}],
  };
}

if(import.meta.main){
  try{
    const argumentsList=process.argv.slice(2);
    assertCliArguments(argumentsList,{values:['output']});
    const outputArgument=argumentsList.find(value=>value.startsWith('--output='));
    assert.ok(outputArgument,'Required: --output=report-directory');
    const output=resolve(outputArgument.slice('--output='.length));
    const outputInfo=await stat(output).catch(error=>error.code==='ENOENT'?null:Promise.reject(error));
    assert.ok(!outputInfo||outputInfo.isDirectory(),'--output must name a directory');
    const report=await runTouchNativeInvariant({output});
    console.log(JSON.stringify({status:report.status,passed:report.passed,componentStatus:report.componentStatus,
      completeRuntimeCoverage:report.completeRuntimeCoverage,productVariantCount:report.productVariantCount,
      componentOwners:report.nativeComponentOwners?.length,mutationProofs:report.mutationProofs?.length,
      report:resolve(output,'report.json')}));
    if(!report.passed){console.error(report.failure||'Touch native component invariants failed');process.exitCode=1;}
  }catch(error){
    console.error(error.stack||error);process.exitCode=1;
  }
}
