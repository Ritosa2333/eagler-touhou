import test from 'node:test';
import assert from 'node:assert/strict';
import {resolve} from 'node:path';
import {mountSdlContent} from '../src/sdl-content.mjs';
import {RuntimeAssets} from '../src/managed-data.mjs';
import {prepareLocalization} from '../src/localization/loader.mjs';
import {th12MessagePatch} from '../../../launcher/server/th12-thcrap-compiler.mjs';
import {createStaticThcrapPack} from '../../../launcher/server/thcrap-static-pack.mjs';
import {runNativeCpp} from '../../../launcher/tests/support/native-cpp.mjs';
const utf8=new TextEncoder();
const command=(time,op,data=Buffer.alloc(0))=>{const h=Buffer.alloc(4);h.writeUInt16LE(time);h[2]=op;h[3]=data.length;return Buffer.concat([h,data]);};
const encrypted=data=>{const raw=Buffer.concat([data,Buffer.of(0)]),out=Buffer.alloc((raw.length+3)&~3);let key=0x77,step=7;for(let i=0;i<out.length;i++){out[i]=(raw[i]??0)^key;key=(key+step)&255;step=(step+16)&255;}return out;};
const header=Buffer.alloc(12);header.writeUInt32LE(1);header.writeUInt32LE(12,4);
const original=Buffer.concat([header,command(0,7),command(0,17,encrypted(Buffer.from('82a0','hex'))),command(0,17,encrypted(Buffer.from('82a2','hex'))),command(0,11,Buffer.alloc(4)),command(1,0)]);
const resource='st01_00a.msg';
async function provider(){
 const result=th12MessagePatch(original,{'0':{'0_0':{lines:['|前文,字,• •','中文台词','第二句']}}});
 const overlay={schema:'th12-localization-overlay/2',game:'th12',language:'lang_zh-hans',sourceKind:'thcrap',sourceExeSha256:'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417',dialogue:{[resource]:result.lookup},dialoguePatches:{[resource]:result.patch},endings:{},endingPatches:{},spells:{},textures:{},musicComments:{}};
 const bytes=Buffer.from(JSON.stringify(overlay)),pack=createStaticThcrapPack({pack:{game:'th12',language:overlay.language,assets:[]},resources:[{targetPath:'/thcrap/th12/overlay.json',bytes}]});
 return prepareLocalization({language:overlay.language,manifest:pack.manifest,readFile:()=>bytes});
}
async function mount(localization,source=original){
 const files=new Map(),assets=new RuntimeAssets({location:{href:'http://localhost/'}});
 assets.localization=localization;assets.content={files:new Map([['assets/'+resource,{}]])};
 const types=Array.from({length:31},()=>({script:0,colors:Array.from({length:16},()=>({sprite:0,spawnSprite:0,cancelSprite:0})),size:1,flags:0}));
 const json={'atlases.json':{bullet:{types,entries:[],sprites:{},scripts:{}}},'menu-resources.json':{originalExeSha256:'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417',atlases:{},spellNames:{},endings:[],credits:{file:'staff.msg',commands:[]},musicComments:[]},'sound-index.json':Array.from({length:60},(_,id)=>({id,volume:1,loop:false,file:'s'+id+'.wav'})),'music-index.json':[],'stage01-background.json':{}};
 assets.fetchBase=async path=>{const name=path.slice('/assets/'.length);return name===resource?new Response(source):Object.hasOwn(json,name)?Response.json(json[name]):new Response(null,{status:404});};
 await mountSdlContent({FS:{mkdirTree(){},writeFile:(path,data)=>files.set(path,Uint8Array.from(data)),analyzePath:path=>({exists:files.has(path)})}},assets);
 return files;
}
test('SDL mounts verified translated MSG and sends exact UTF-8 byte keys and ruby widths to native lookup',async()=>{
 const p=await provider(),files=await mount(p);
 assert.deepEqual(files.get('/game/'+resource),await p.patchMessage('/assets/'+resource,original));
 const data=files.get('/game/native-translations.bin');assert.equal(new TextDecoder().decode(data.subarray(0,8)),'TH12LOC2');
 const expected=Buffer.from('中文台词').toString('hex');
 const source=`#include "message.hpp"\n#include <cassert>\n#include <cstring>\n#include "Localization.cpp"\nnamespace th12host::host {Paths& paths(){static Paths p;return p;}void log(const char*,...){}bool read_file(const std::string&,std::vector<u8>& out,u64){out={${[...data]}};return true;}}\nint main(){using namespace th12host;load_translations();translation_context("${resource}");message_system::File file;const std::vector<u8> message={${[...files.get('/game/'+resource)]}};assert(file.load(message.data(),int(message.size())));message_system::VM vm;assert(vm.start(file,0));vm.tick(0,0,[](const auto&){});assert(translated_dialogue(vm.textRecords[0].bytes)->text==U"中文台词");assert(translated_dialogue(vm.textRecords[1].bytes)->text==U"第二句");assert(translated_dialogue(vm.textRecords[2].bytes)->text==U"• •");const std::string bytes("${[...Buffer.from(expected,'hex')].map(n=>'\\x'+n.toString(16).padStart(2,'0')).join('')}",${Buffer.from(expected,'hex').length});auto* line=translated_dialogue(bytes);assert(line&&line->text==U"中文台词");assert(line->byte_widths==std::vector<u32>({2,2,2,2}));const auto* ruby=translated_dialogue("• •");assert(ruby&&ruby->byte_widths==std::vector<u32>({2,1,2}));translation_context("st02_00a.msg");assert(!translated_dialogue(bytes));load_translations();assert(!translated_dialogue(bytes));}`;
 await runNativeCpp(source,{includes:[resolve(import.meta.dirname,'../portable/sdl'),resolve(import.meta.dirname,'../src')]});
});
test('SDL Japanese content stays byte-identical and corrupt translated sources are rejected',async()=>{
 const files=await mount(await prepareLocalization());assert.deepEqual(files.get('/game/'+resource),Uint8Array.from(original));
 const corrupt=Buffer.from(original);corrupt[18]^=1;
 await assert.rejects(mount(await provider(),corrupt),/源文件不匹配/);
});

test('SDL accepts legacy translation tables and clears every scoped view after a rejected reload',async()=>{
 const word=n=>{const b=Buffer.alloc(4);b.writeUInt32LE(n);return b;};
 const text=s=>{const b=Buffer.from(s);return Buffer.concat([word(b.length),b]);};
 const data=Buffer.concat([Buffer.from('TH12LOC1'),word(1),text(''),text('霊符'),text('灵符')]);
 await runNativeCpp(`#include <cassert>\n#include "Localization.cpp"\nstatic std::vector<th12host::u8> input{${[...data]}};\nnamespace th12host::host {Paths& paths(){static Paths p;return p;}void log(const char*,...){}bool read_file(const std::string&,std::vector<u8>& out,u64){out=input;return true;}}\nint main(){using namespace th12host;load_translations();translation_context("st01_00a.msg");assert(translate_text(U"霊符")==U"灵符");assert(!translated_message(0,12));input.push_back(0);load_translations();assert(translate_text(U"霊符")==U"霊符");assert(!translated_dialogue("x"));}`,{includes:[resolve(import.meta.dirname,'../portable/sdl')]});
});
