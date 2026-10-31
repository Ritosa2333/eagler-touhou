import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import {EndingTimeline} from '../src/ending-timeline.mjs';
const resources=JSON.parse(fs.readFileSync(new URL('../site/assets/menu-resources.json',import.meta.url))),atlases={...JSON.parse(fs.readFileSync(new URL('../site/assets/atlases.json',import.meta.url))),...resources.atlases};
test('native ending large waits stop MSG time until confirm and preserve five text lines',()=>{
  const music=[],e=new EndingTimeline({resources,atlases,ending:resources.endings[6],difficulty:1,onMusic:id=>music.push(id)});
  e.tick();e.tick();assert.ok(e.wait);const time=e.clock,text=e.lines.map(l=>l.text);for(let i=0;i<10;i++)e.tick();assert.equal(e.clock,time);assert.deepEqual(e.lines.map(l=>l.text),text);e.tick({pressed:1});assert.ok(e.lines.length>text.length);assert.ok(music.includes('th12_20'));assert.ok(e.poses().length);
});
test('all original endings reach their good/bad result branch and original staff roll finishes',()=>{
  for(const ending of resources.endings){const e=new EndingTimeline({resources,atlases,ending,difficulty:1});let credits=false;
    for(let i=0;i<6000&&!e.finished;i++){e.tick({held:512,pressed:1});credits||=e.scene==='credits';}
    assert.equal(e.finished,true,'ending'+ending.id);assert.equal(credits,ending.id>=6,'only native good ending invokes staff.msg');
  }
});
