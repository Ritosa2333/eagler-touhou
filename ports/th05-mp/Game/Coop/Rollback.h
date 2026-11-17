#pragma once
#include <cstdint>
namespace th5w { namespace coop {
// Presentation and transport stay outside world checkpoints. Audio is published
// only after the input ledger confirms the corresponding simulated frame.
bool QueueSound(int sound);
bool QueueMusic(const char* name);
bool QueueMusicAction(int action,double value);
void CaptureRuntime(class CaptureSink& sink);
struct CaptureSink {
 virtual void Bytes(void* address,unsigned size)=0;
 template<class T> void Object(T* value){if(value)Bytes(value,sizeof(T));}
 virtual ~CaptureSink()=default;
};
} }
