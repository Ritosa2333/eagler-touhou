import test from 'node:test';
import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import {dirname,resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {workspacePath} from '../../launcher/lib/workspace-layout.mjs';
import {runNativeCpp} from '../../launcher/tests/support/native-cpp.mjs';

function productionMethod(source,signature){
  const start=source.indexOf(signature);
  assert.notEqual(start,-1,`Production ${signature} definition is present`);
  const open=source.indexOf('{',start);let depth=0;
  for(let index=open;index<source.length;index++){
    if(source[index]==='{')depth++;
    else if(source[index]==='}'&&--depth===0)return source.slice(start,index+1);
  }
  assert.fail('Production GuiFlow::update has a balanced body');
}

test('production GuiFlow captures Dialogue consumer input before later publishers and invalidates while stopped',async()=>{
  const here=dirname(fileURLToPath(import.meta.url));
  const [guiFlow,gameplay]=await Promise.all([
    readFile(workspacePath('th08','web/cpp/game/GuiFlow.cpp'),'utf8'),
    readFile(workspacePath('th08','web/cpp/game/GameplayScene.cpp'),'utf8'),
  ]);
  const template=await readFile(resolve(here,'fixtures/dialogue-consumer-receipt.cpp'),'utf8');
  assert.equal(template.split('/* GUI_FLOW_UPDATE_SOURCE */').length,2,'Fixture has one GuiFlow insertion point');
  assert.equal(template.split('/* GAMEPLAY_PUBLISH_INPUT_SOURCE */').length,2,'Fixture has one Replay-publisher insertion point');
  const withGuiFlow=template.replace('/* GUI_FLOW_UPDATE_SOURCE */',productionMethod(guiFlow,'void GuiFlow::update(){'));
  const withPublisher=withGuiFlow.replace('/* GAMEPLAY_PUBLISH_INPUT_SOURCE */',
    productionMethod(gameplay,'void GameplayScene::publish_input(const ReplayInputState& input){'));
  await runNativeCpp(withPublisher,{includes:[workspacePath('th08','web/cpp/game')]});
});
