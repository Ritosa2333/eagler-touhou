import test from 'node:test';
import assert from 'node:assert/strict';
import {execFileSync} from 'node:child_process';
import {mkdtempSync,mkdirSync,writeFileSync,rmSync} from 'node:fs';
import {tmpdir} from 'node:os';
import {resolve,join} from 'node:path';

test('production SDL mixer preserves output and no-BGM lifecycle in stream and full modes',t=>{
  const temporary=mkdtempSync(join(tmpdir(),'th12-sdl-audio-'));
  t.after(()=>rmSync(temporary,{recursive:true,force:true}));
  const source=resolve(import.meta.dirname,'../portable/sdl'),include=join(temporary,'include/SDL3');
  mkdirSync(include,{recursive:true});
  writeFileSync(join(include,'SDL.h'),`
    #pragma once
    struct SDL_AudioStream {};
    struct SDL_AudioSpec {int format,channels,freq;};
    constexpr int SDL_INIT_AUDIO=1,SDL_AUDIO_F32=1,SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK=1;
    constexpr const char* SDL_HINT_AUDIO_DEVICE_SAMPLE_FRAMES="frames";
    inline bool SDL_InitSubSystem(int){return false;}
    inline bool SDL_SetHint(const char*,const char*){return true;}
    inline SDL_AudioStream* SDL_OpenAudioDeviceStream(int,const SDL_AudioSpec*,void*,void*){return nullptr;}
    inline const char* SDL_GetError(){return "test uses offline PCM";}
    inline void SDL_DestroyAudioStream(SDL_AudioStream*){}
    inline int SDL_GetAudioStreamQueued(SDL_AudioStream*){return 0;}
    inline bool SDL_PutAudioStreamData(SDL_AudioStream*,const void*,int){return true;}
    inline bool SDL_ClearAudioStream(SDL_AudioStream*){return true;}
    inline bool SDL_PauseAudioStreamDevice(SDL_AudioStream*){return true;}
    inline bool SDL_ResumeAudioStreamDevice(SDL_AudioStream*){return true;}
  `);
  const program=join(temporary,process.platform==='win32'?'audio.exe':'audio');
  execFileSync(process.env.CXX||'g++',['-std=c++17','-O2','-DNOMINMAX','-I',join(temporary,'include'),'-I',source,
    '-I',resolve(import.meta.dirname,'../third_party/stb_vorbis'),resolve(import.meta.dirname,'native/sdl-audio.cpp'),
    ...['Audio.cpp','Archive.cpp','BgmStream.cpp'].map(name=>join(source,name)),'-o',program],
    {encoding:'utf8',windowsHide:true,timeout:120000});
  assert.match(execFileSync(program,[temporary],{encoding:'utf8',windowsHide:true,timeout:30000}),/passed/);
});
