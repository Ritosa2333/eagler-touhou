#pragma once
#include "Platform.hpp"
#include <array>
namespace th5w {
class CPMDPlayer {
public:
 static bool Initialize(HWND){return true;}
 static bool LoadPMDData(unsigned char*,int);
 static bool UnloadPMDData(){web::MusicAction(0);return true;}
 static std::array<int,8> PianoNotes(){std::array<int,8> notes;EM_ASM({const notes=Module.eaglerAudio?.piano?.();for(let i=0;i<8;i++)HEAP32[($0>>2)+i]=notes?.[i]??255;},notes.data());return notes;}
 static int Measure(){return EM_ASM_INT({return Module.eaglerAudio?.measure?.()??-1;});}
 static void Play(){web::MusicAction(1);}
 static void Pause(){web::MusicAction(2);}
 static void Resume(){web::MusicAction(3);}
 static void Finalize(){web::MusicAction(0);}
 static void SetFrequency(int hz){web::MusicAction(4,hz/44100.);}
 static void SetVolume(float volume){web::MusicAction(5,volume/100.);}
 static void OnDeactivate(){EM_ASM({Module.eaglerAudio?.focus(false);});}
 static void OnActivate(){EM_ASM({Module.eaglerAudio?.focus(true);});}
 static void SetPlayThreadPriority(int){}
 static bool FillSoftwareBuffer(int){return true;}
};
class CSoundEffect {
public:
 bool Initialize(HWND){return true;}
 void PlaySound(int index){web::PlaySound(index);}
};
}
