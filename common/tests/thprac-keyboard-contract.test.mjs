import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
const read=path=>readFileSync(new URL('../../'+path,import.meta.url),'utf8');

test('every native thprac renderer feeds numeric editing before NewFrame',()=>{
  for(const title of ['th06','th07','th08','th08-mp','th10','th10-mp','th11','th11-mp','th15','th15-mp']){
    const path=['th06','th07'].includes(title)?`ports/${title}/src/ThpracImGui.cpp`:`ports/${title}/web/cpp/sdl/ThpracUi.cpp`;
    const source=read(path),feed=source.indexOf('eagler::thprac::sdl_keyboard_frame(io);');
    assert.ok(feed>=0,title);
    assert.ok(source.indexOf('ImGui::NewFrame();',feed)>feed,title);
    assert.equal(source.includes('io.AddInputCharacter('),false,`${title}: no duplicate menu-only queue`);
  }
});
test('TH06 pause buttons use the existing three-language labels and stable IDs',()=>{
  const source=read('ports/th06/src/PracticeRuntime.cpp'),labels=read('ports/th06/src/ThpracImGui.cpp');
  for(const name of ['Resume','Exit','Restart','Settings']){
    assert.ok(source.includes(`GuiPauseButton("${name}",`));
    assert.match(source,new RegExp(`strcmp\\(label, "${name}"\\).*ThpracImGui::Text\\(Id::${name}\\)`));
    assert.match(labels,new RegExp(`\\{"[^"\\x00-\\x7f][^"]*", "${name}", "[^"\\x00-\\x7f][^"]*"\\}`));
  }
  assert.ok(source.includes('std::string(visible) + "###" + label'));
  assert.ok(source.includes('SetItemFocusAlt("###Resume", true)'));
});
test('older TH08 and TH10 multiplayer shells also bind the canonical native and hosted keyboard paths',()=>{
  for(const title of ['th08','th10-mp']){
    const source=read(`ports/${title}/web/sdl-runtime/shell.mjs`);
    assert.ok(source.includes('createBrowserKeyboard({hostContext:{protocol,game,epoch,'),title);
    assert.ok(source.includes("keyboard.mapNative(event,name==='keydown')"),title);
    assert.ok(source.includes("keyboard.event(message,!!message.down,'hosted')"),title);
  }
});

test('TH15 optional keyboard dependencies stay inside the enabled implementation',()=>{
  for(const title of ['th15','th15-mp']){
    const source=read(`ports/${title}/web/cpp/sdl/ThpracUi.cpp`);
    assert.ok(source.indexOf('#if TH_ENABLE_THPRAC')<source.indexOf('#include <eagler/thprac/PracticeSdlKeyboard.hpp>'),title);
  }
});
test('TH07 retains logical reaction keys after physical keyboard sampling',()=>{
  const source=read('ports/th07/src/ThpracImGui.cpp');
  const frame=source.slice(source.indexOf('void BeginFrame('),source.indexOf('bool IsFrameOpen('));
  assert.ok(frame.indexOf('sdl_keyboard_frame(io)')<frame.indexOf('ApplyGameInput()'));
});
