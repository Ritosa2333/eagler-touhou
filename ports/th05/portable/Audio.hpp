#pragma once
#include "Platform.hpp"
#include <array>
namespace th5w {
class CPMDPlayer {
public:
 static bool Initialize(HWND){return true;}
 static bool LoadPMDData(unsigned char*,int);
 static bool UnloadPMDData(){web::MusicAction(0);return true;}
 static std::array<int,8> PianoNotes(){std::array<int,8> notes;notes.fill(-1);return notes;}
 static int Measure(){return -1;}
 static void Play(){web::MusicAction(1);}
 static void Pause(){web::MusicAction(2);}
 static void Resume(){web::MusicAction(3);}
 static void Finalize(){web::MusicAction(0);}
 static void SetFrequency(int hz){web::MusicAction(4,hz/44100.);}
 static void SetVolume(float volume){web::MusicAction(5,volume/100.);}
 static void OnDeactivate(){web::MusicAction(2);}
 static void OnActivate(){web::MusicAction(3);}
 static void SetPlayThreadPriority(int){}
 static bool FillSoftwareBuffer(int){return true;}
};
class CSoundEffect {
public:
 bool Initialize(HWND){return true;}
 void PlaySound(int index){web::PlaySound(index);}
};
}
