import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import crypto from 'node:crypto';
import {parseReplay} from '../src/replay.mjs';
import {initializeCandidate,restoreStage,state} from '../tools/replay-verifier/capture-candidate.mjs';
import {readCoreAnimationPoses,readCoreMeshes} from '../src/renderer.mjs';
import {readCoreDrawSchedule,orderPresentation} from '../src/draw-scheduler.mjs';

test('retail first midboss spell keeps its body above the backdrop and draws the full timed laser length',async()=>{
  const bytes=fs.readFileSync(new URL('../site/replays/th12_14.rpy',import.meta.url));
  assert.equal(crypto.createHash('sha256').update(bytes).digest('hex'),
    'c92747c90ceacc9907440dd03b00afd88ef206bc76abfe1e64b7bc1de24ab516');
  const replay=parseReplay(bytes),stage=replay.stages.find(s=>s.number===1),core=await initializeCandidate(1);
  restoreStage(core,replay,stage);core._th12_oracle_fixture(0);
  for(let tick=0;tick<=4115;tick++){
    const input=replay.input(1,tick);core._th12_tick(input.held,input.pressed);
    if(![3980,4069,4100,4115].includes(tick))continue;
    const snapshot=state(core),poses=readCoreAnimationPoses(core),meshes=readCoreMeshes(core);
    const schedule=readCoreDrawSchedule(core),ordered=orderPresentation([...poses,...meshes],schedule);
    const boss=snapshot.enemies.find(e=>e.boss),body=ordered.find(p=>p.owner===boss.id&&p.a.name==='stgenm01'&&p.a.sprite===11);
    const backdrop=ordered.filter(p=>p.a.name==='stgenm01'&&[31,32].includes(p.a.sprite));
    assert.ok(body?.a.visible,'real midboss body input'+tick);assert.equal(backdrop.length,2);
    // Native413230 defaults the enemy offset to1;415b70/41c6a0 adds7.
    // MBoss has no452 override. The backdrop scripts set their own layer3;
    // all retain normal4611d0 layer dispatch and its ordering.
    assert.equal(body.a.layer,8);assert.equal(body.drawPriority,18);
    for(const background of backdrop)assert.ok(ordered.indexOf(background)<ordered.indexOf(body),
      'spell background precedes the body input'+tick);
    if(tick===3980){
      const ringNodes=snapshot.animations.nodes.filter(n=>[125,126].includes(n.script)&&n.bank===0);
      assert.equal(ringNodes.length,2,'both native spell-start ring VMs');
      for(const node of ringNodes){
        assert.ok(!poses.some(p=>p.id===node.id),'mode9 must not produce a gigantic sprite quad');
        const ring=ordered.find(p=>p.mesh&&p.id===node.id);
        assert.ok(ring,'mode9 ring mesh reaches the registered draw schedule');
        assert.equal(ring.vertices.length,64);assert.equal(ring.renderPass,0);
        assert.equal(ring.drawPriority,11);assert.equal(ring.clip,2);
        assert.ok(ring.vertices.every(v=>Number.isFinite(v.x)&&Number.isFinite(v.y)));
        // The actual ring spans hundreds of pixels, not the old40000px
        // scaled14x128 rectangle. It closes with an exact copied first pair.
        const ys=ring.vertices.map(v=>v.y);
        assert.ok(Math.max(...ys)-Math.min(...ys)<700);
        assert.equal(ring.vertices[0].x,ring.vertices.at(-2).x);
        assert.equal(ring.vertices[0].y,ring.vertices.at(-2).y);
      }
      const portrait=ordered.find(p=>!p.mesh&&p.a.name==='stgenm01'&&p.a.sprite===34);
      assert.ok(portrait?.a.visible,'native Nazrin spell portrait remains visible');
      assert.ok(ringNodes.every(n=>ordered.findIndex(p=>p.id===n.id)<ordered.indexOf(portrait)));
    }
    if(tick<4069)continue;
    assert.equal(snapshot.lasers.nodes.length,2);
    for(const laser of snapshot.lasers.nodes){
      const mainNode=snapshot.animations.nodes.find(n=>n.owner===0xb0000000+laser.serial&&n.script===42);
      assert.ok(mainNode,'native main ANM42');
      const main=ordered.find(p=>p.mesh&&p.id===mainNode.id);
      assert.ok(main,'timed laser mesh input'+tick);assert.equal(main.renderPass,1);
      assert.equal(main.drawPriority,29);assert.equal(main.vertices.length,6);
      assert.ok(ordered.indexOf(body)<ordered.indexOf(main));
      // Actual458066 skips duration0 scale interpolation after the laser
      // manager writes geometry. The shader must receive the480px quad,
      // rather than script42's ordinary16x14 sprite reset to scale1.
      const [topLeft,topRight,bottomLeft]=main.vertices;
      const distance=(a,b)=>Math.hypot(a.x-b.x,a.y-b.y);
      assert.ok(Math.abs(distance(topLeft,bottomLeft)-laser.geometry[1])<.0001,
        'full laser length input'+tick);
      assert.ok(Math.abs(distance(topLeft,topRight)-laser.geometry[2])<.0001,
        'full laser width input'+tick);
      assert.ok(main.vertices.some(v=>v.color>>>24),'native alpha remains visible');
    }
  }
});
