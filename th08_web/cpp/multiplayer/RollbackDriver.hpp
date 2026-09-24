#pragma once
#include "WorldJournal.hpp"
#include "TextureJournal.hpp"
#include "AudioEvents.hpp"
#include "FileEvents.hpp"
#include "NetworkConnection.hpp"

namespace th08::multiplayer {
// One production path for both real transport and deterministic fault-injection
// tests. It alone opens a speculative frame, corrects it and releases outputs.
class RollbackDriver final:private FileEventOutput {
public:
    explicit RollbackDriver(BrowserRuntime&);
    ~RollbackDriver(){Shutdown();}
    bool Step(bool render);
    bool FinishFrame();
    bool Pump();
    bool Reconcile();
    bool Connect(const char*);
    bool ConnectSpectator(const char*,const char*);
    void Shutdown();
    bool Failed()const{return failed;}
    bool FrameOpen()const{return open;}
    bool Initialized()const{return initialized;}
    bool HasPending()const{return world.HasHistory()||audio.PendingFrames()||files.IsOpen();}
    const char* Error()const;
    const NetworkConnection& Network()const{return network;}
    const AudioEvents& Audio()const{return audio;}
    const FileEvents& Files()const{return files;}
    const std::vector<u8>* PendingFile(const std::string& path)const{return files.Pending(path);}
    bool Write(const std::string& path,const u8* bytes,u32 size){return files.Write(path,bytes,size);}
    u32 Corrections()const{return corrections;}
    u32 Resimulated()const{return resimulated;}
    u32 Predicted()const{return predicted;}
    u32 MaxBytes()const{return max_bytes;}
private:
    BrowserRuntime& runtime;
    WorldJournal world;TextureJournal textures;AudioEvents audio;FileEvents files;
    NetworkConnection network;
    bool bound=false,open=false,initialized=false,failed=false,correcting=false;
    bool generation_transition_pending=false;
    bool corrected_present_pending=false;
    u32 generation=0,corrections=0,resimulated=0,predicted=0,max_bytes=0;
    const char* error="";
    bool Fail(const char* text){failed=true;error=text;return false;}
    bool Stable()const;
    bool Commit();
    bool Admit();
    bool RunFrame(bool render);
    bool Correct();
    bool PresentCorrection();
    bool apply_file_event(const char*,const u8*,u32)override;
};
}
