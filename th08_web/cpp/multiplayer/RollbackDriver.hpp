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
    u32 CheckpointSpan()const{return checkpoint_span;}
#ifdef TH_MULTIPLAYER_FIXTURES
    bool SetCheckpointSpan(u32 span){if(open||world.HasHistory()||span<1||span>3)return false;checkpoint_span=span;return true;}
    bool SetLiveBullets(bool enabled){return world.SetLiveBullets(enabled);}
    bool LiveBullets()const{return world.LiveBullets();}
    void AuditBullets(bool enabled){world.AuditBullets(enabled);}
    u32 BulletAuditRestores()const{return world.BulletAuditRestores();}
    bool diagnostic_always_snapshot=false;
    bool diagnostic_exact_only=false;
    bool diagnostic_early_input=true;
    double diagnostic_snapshots=0,diagnostic_skipped=0,diagnostic_capture_ms=0;
    double diagnostic_restore_ms=0,diagnostic_update_ms=0,diagnostic_draw_ms=0;
    double diagnostic_snapshot_bytes=0;
    double diagnostic_correction_ms=0,diagnostic_correction_max_ms=0;
#endif
private:
    BrowserRuntime& runtime;
    WorldJournal world;TextureJournal textures;AudioEvents audio;FileEvents files;
    NetworkConnection network;
    bool bound=false,open=false,initialized=false,failed=false,correcting=false;
    bool generation_transition_pending=false;
    bool corrected_present_pending=false;
    bool checkpoint_open=false;
    // Three-tick intervals are unfinished experimental work. Keep the proven
    // per-tick policy until interval ownership/output tests and A/B pass.
    u32 checkpoint_span=1;
    std::size_t checkpoint_previous_bytes=0;
    u32 generation=0,corrections=0,resimulated=0,predicted=0,max_bytes=0;
    const char* error="";
    char native_error[256]{};
    bool Fail(const char* text){failed=true;error=text;return false;}
    bool FailNativeUpdate(u32 frame,bool updated);
    bool Stable()const;
    bool Commit();
    bool Admit();
    bool RunFrame(bool render);
    bool CaptureLocalInput();
    bool Correct();
    bool PresentCorrection();
    bool apply_file_event(const char*,const u8*,u32)override;
};
}
