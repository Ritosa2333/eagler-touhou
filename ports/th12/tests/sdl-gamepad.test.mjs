import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {runInNewContext} from 'node:vm';
import {runNativeCpp} from '../../../launcher/tests/support/native-cpp.mjs';
import {PreviewInput} from '../src/preview-input.mjs';

// Independent axes, including corners outside a radius-.5 circle but inside
// the original square. Exactly 50% stays neutral; crossing either side acts.
const cases=[
  [0,0,0],[.3,-.3,0],[.49,-.49,0],[.5,-.5,0],[-.5,.5,0],
  [.5001,0,128],[-.5001,0,64],[0,.5001,32],[0,-.5001,16],
  [.6,.4,128],[.4,-.6,16],[.6,.6,160],[-.6,-.6,80],
  [.6,-.6,144],[-.6,.6,96],[1,-1,144]
];
function pad(x,y){return {connected:true,axes:[x,y],buttons:Array.from({length:16},()=>({pressed:false}))};}

test('legacy and preview gamepads use the original square 50% deadzone',()=>{
  const source=readFileSync(new URL('../site/game.mjs',import.meta.url),'utf8');
  const start=source.indexOf('function padInput(){'),end=source.indexOf('\nasync function audioReady',start);
  assert(start>=0&&end>start);
  let device;
  const context={navigator:{getGamepads:()=>[device]},joyCaptureRelease:false,
    model:{state:{}},assets:{managed:false},profile:{settings:{joyButtons:{shoot:0,bomb:1,focus:2,pause:9,skip:5}}},
    managedInput:{pad:()=>null}};
  const sample=runInNewContext(source.slice(start,end)+';padInput',context);
  for(const [x,y,expected] of cases){
    device=pad(x,y);
    assert.equal(sample().value,expected,'legacy '+[x,y]);
    assert.equal(new PreviewInput().sample(0,device).held,expected,'preview '+[x,y]);
  }
  device=pad(.49,-.49);device.buttons[12].pressed=device.buttons[15].pressed=true;
  assert.equal(sample().value,144,'d-pad bypasses the analog deadzone');
  assert.equal(new PreviewInput().sample(0,device).held,144);
  device=pad(0,0);device.buttons[0].pressed=true;
  assert.equal(sample().value,1);assert.equal(new PreviewInput().sample(0,device).held,1);
  context.assets.managed=true;context.managedInput.pad=()=>0;
  device=pad(1,-1);assert.equal(sample().value,0,'Launcher neutral overrides local axes');
  context.managedInput.pad=()=>64;assert.equal(sample().value,64,'Launcher custom mappings keep ownership');
});

test('native SDL samples preserve the 50% boundary, diagonals, d-pad and buttons',async()=>{
  const source=readFileSync(new URL('../portable/sdl/Application.cpp',import.meta.url),'utf8');
  const start=source.indexOf('for(auto* pad:pads)if(SDL_GamepadConnected(pad)){'),end=source.indexOf('\n#endif',start);
  assert(start>=0&&end>start);
  const result=await runNativeCpp(`
    #include <cassert>
    #include <cstdint>
    #include <cstdio>
    enum {InUp=16,InDown=32,InLeft=64,InRight=128};
    enum {SDL_GAMEPAD_AXIS_LEFTX,SDL_GAMEPAD_AXIS_LEFTY,SDL_GAMEPAD_AXIS_LEFT_TRIGGER,SDL_GAMEPAD_AXIS_RIGHT_TRIGGER};
    enum SDL_GamepadButton {SDL_GAMEPAD_BUTTON_DPAD_LEFT,SDL_GAMEPAD_BUTTON_DPAD_RIGHT,
      SDL_GAMEPAD_BUTTON_DPAD_UP,SDL_GAMEPAD_BUTTON_DPAD_DOWN,SDL_GAMEPAD_BUTTON_SOUTH,SDL_GAMEPAD_BUTTON_EAST,
      SDL_GAMEPAD_BUTTON_LEFT_SHOULDER,SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER,SDL_GAMEPAD_BUTTON_START,
      SDL_GAMEPAD_BUTTON_WEST,SDL_GAMEPAD_BUTTON_NORTH};
    struct Device {bool connected=true;int16_t axes[4]{};bool buttons[11]{};};
    struct Sample {unsigned directions=0,buttons=0;};
    bool SDL_GamepadConnected(Device* p){return p->connected;}
    int16_t SDL_GetGamepadAxis(Device* p,int axis){return p->axes[axis];}
    bool SDL_GetGamepadButton(Device* p,SDL_GamepadButton button){return p->buttons[button];}
    Sample sample(Device& device){Sample out;Device* pads[]{&device};
      ${source.slice(start,end)}
      return out;
    }
    int main(){
      const double cases[][3]={${cases.map(c=>'{'+c.join(',')+'}').join(',')}};
      for(const auto& c:cases){Device p;
        p.axes[0]=int16_t(c[0]>=1?32767:c[0]*32768);
        p.axes[1]=int16_t(c[1]>=1?32767:c[1]*32768);
        assert(sample(p).directions==unsigned(c[2]));
      }
      Device p;p.axes[0]=16384;p.axes[1]=-16384;
      p.buttons[SDL_GAMEPAD_BUTTON_DPAD_LEFT]=p.buttons[SDL_GAMEPAD_BUTTON_DPAD_DOWN]=true;
      assert(sample(p).directions==96);
      p.buttons[SDL_GAMEPAD_BUTTON_SOUTH]=true;p.axes[SDL_GAMEPAD_AXIS_LEFT_TRIGGER]=9831;
      assert(sample(p).buttons==5);
      p.connected=false;assert(sample(p).directions==0&&sample(p).buttons==0);
      std::puts("SDL original square deadzone passed");
    }
  `);
  assert.match(result,/passed/);
});

