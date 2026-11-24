#include "native_replay.hpp"
#include "replay_slots.hpp"
#include "dialogue_core.hpp"
#include "player_motion.hpp"
#include <iostream>

namespace {
using namespace soew;
using namespace native_replay_detail;
namespace fs=std::filesystem;
unsigned checks=0;

void check(bool ok){
    ++checks;
    if(!ok)throw std::runtime_error("Native replay check "+std::to_string(checks)+" failed");
}
template<class F> void check_rejected(F &&operation){
    try{operation();}catch(const std::exception&){check(true);return;}
    check(false);
}
struct TestDirectory {
    fs::path path;
    explicit TestDirectory(const fs::path &base){
        if(!fs::is_directory(base))throw std::runtime_error("Replay test root must already exist");
        const auto stamp=std::chrono::steady_clock::now().time_since_epoch().count();
        for(unsigned n=0;n<64;++n){
            auto candidate=base/("soew-replay-test-"+std::to_string(stamp)+"-"+std::to_string(n));
            if(fs::create_directory(candidate)){path=std::move(candidate);return;}
        }
        throw std::runtime_error("Cannot create test directory");
    }
    ~TestDirectory(){std::error_code ignored;fs::remove_all(path,ignored);}
};
void write_bytes(const fs::path &path,const Bytes &bytes){
    std::ofstream file(path,std::ios::binary|std::ios::trunc);
    file.write(reinterpret_cast<const char*>(bytes.data()),static_cast<std::streamsize>(bytes.size()));
    file.close();if(!file)throw std::runtime_error("Could not create replay fixture");
}
void reseal(Bytes &bytes){
    const auto sum=checksum(bytes);
    for(unsigned i=0;i<4;++i)bytes[28+i]=static_cast<std::uint8_t>(sum>>(i*8));
}
void set_count(Bytes &bytes,std::uint32_t count){
    for(unsigned i=0;i<4;++i)bytes[24+i]=static_cast<std::uint8_t>(count>>(i*8));
}
Bytes zlib_payload(const Bytes &events){
    const auto size=static_cast<uLong>(events.size());
    uLongf compressed_size=compressBound(size);
    Bytes compressed(compressed_size);
    check(compress2(compressed.data(),&compressed_size,events.data(),size,Z_BEST_SPEED)==Z_OK);
    compressed.resize(compressed_size);
    return compressed;
}
void check_rejected_bytes(const Bytes &bytes){
    check_rejected([&]{decode(bytes);});
    check_rejected([&]{listing(bytes);});
}

// Golden files from the documented little-endian layouts, including fixed FNV-1a checksums.
void fixed_legacy_layout(const NativeReplayConfig &config){
    const Bytes golden{
        0x53,0x4f,0x45,0x57,0x52,0x50,0x4c,0x59,0x01,0x00,0x13,0x00,0x21,0x43,0x65,0x87,
        0x00,0x02,0x03,0x04,0x00,0x02,0x00,0x00,0x01,0x00,0x00,0x00,0x4f,0x6c,0xfe,0xc5,
        0x00,0x03,0x10,0x10};
    NativeReplay run;
    run.config=config;
    run.events={{NativeReplayEventKind::Tick,UpLeft|Shot,true,true}};
    check(decode(golden)==run);
    check(encode(run)==golden&&encode(run,version)==golden);
    check(listing(golden)==NativeReplayListing{config,{},0,{}});
}

void fixed_v2_layout(const NativeReplay &run,const Bytes &bytes){
    const Bytes golden{
        0x53,0x4f,0x45,0x57,0x52,0x50,0x4c,0x59,0x02,0x00,0x13,0x00,0x21,0x43,0x65,0x87,
        0x00,0x02,0x03,0x04,0x00,0x02,0x00,0x00,0x05,0x00,0x00,0x00,0x68,0x09,0x74,0x42,
        0xb2,0x84,0xa3,0x6a,0x00,0x00,0x00,0x00,0x57,0x49,0x4e,0x20,0x54,0x45,0x53,0x54,
        0x08,0x02,0x01,0x03,0x00,0x02,0x01,0x03,0x01,0x00,0x01,0x00,0x03,0x00,0x00,0x00,
        0x55,0x66,0x77,0x88,0xdd,0xcc,0xbb,0xaa,0xdf,0x31,0x00,0x00,0x01,0x00,0x01,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x02,0x00,0x00,0x00,0x21,0x43,0x65,0x87,0x78,0x56,0x34,0x12,0x03,0x00,0x00,0x00,
        0x55,0x66,0x77,0x88,0xdd,0xcc,0xbb,0xaa,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x10,0x00,0x00,0x13,0x80,0x00,
        0x01,0x00,0x00,0x00,0x00,0x0c,0x00,0x00,0x02,0x00,0x00,0x00};
    check(bytes==golden);
    check(decode(golden)==run);
    check(encode(decode(golden),v2_version)==golden);
}

NativeReplay repeated_run(NativeReplay run,std::uint32_t count){
    run.events.assign(count,NativeReplayEvent{NativeReplayEventKind::Tick,Shot,false,false,NativeReplayContext::Gameplay});
    NativeReplaySummary summary;
    const auto stage=std::uint8_t(run.config.stage);
    summary.final_stage=stage;
    summary.final_score=17000;
    summary.ticks=count;
    summary.state_hash=0xfedcba9876543210ull;
    summary.stage_scores[stage]=summary.final_score;
    summary.stage_checks[stage]=NativeReplayBoundary{count,summary.state_hash};
    run.summary=summary;
    return run;
}

void v3_payload_cases(const NativeReplay &run,const Bytes &bytes_v2){
    constexpr std::size_t payload_offset=168,mode_offset=22;
    check(v2_header_size==payload_offset&&current_version==3&&v2_version==2);
    const auto raw=encode(run);
    auto expected_raw=bytes_v2;expected_raw[8]=3;reseal(expected_raw);
    check(raw==expected_raw&&encode(run,3)==raw&&raw[mode_offset]==0&&raw[mode_offset+1]==0);
    check(decode(raw)==run&&listing(raw)==listing(bytes_v2));
    check_rejected([&]{encode(run,1);});
    check_rejected([&]{encode(run,4);});

    const auto repeated=repeated_run(run,2048);
    const auto v2=encode(repeated,v2_version),compressed=encode(repeated);
    check(get(compressed,8,2)==3&&get(compressed,mode_offset,2)==1);
    check(get(compressed,24,4)==repeated.events.size()&&compressed.size()<v2.size());
    check(get(compressed,28,4)==checksum(compressed));
    check(std::equal(v2.begin()+32,v2.begin()+payload_offset,compressed.begin()+32));
    check(decode(compressed)==repeated&&listing(compressed)==listing(v2));
    const auto metadata_only=decode(compressed,false);
    check(metadata_only.events.empty()&&metadata_only.config==repeated.config&&metadata_only.summary==repeated.summary);

    const Bytes events(v2.begin()+payload_offset,v2.end());
    Bytes inflated(events.size());
    uLongf inflated_size=static_cast<uLongf>(inflated.size());
    check(uncompress(inflated.data(),&inflated_size,compressed.data()+payload_offset,
        static_cast<uLong>(compressed.size()-payload_offset))==Z_OK);
    check(inflated_size==events.size()&&inflated==events);

    // Frame an independently compressed stream without asking the replay encoder.
    const auto frame=[&](const Bytes &payload){
        Bytes framed(compressed.begin(),compressed.begin()+payload_offset);
        framed.insert(framed.end(),payload.begin(),payload.end());
        reseal(framed);
        return framed;
    };
    const auto fixture_payload=zlib_payload(events);
    const auto fixture=frame(fixture_payload);
    check(decode(fixture)==repeated&&listing(fixture)==listing(v2));
    auto other_raw=frame(events);other_raw[mode_offset]=0;reseal(other_raw);
    check(decode(other_raw)==repeated&&listing(other_raw)==listing(v2)); // valid even when compressible

    auto invalid=fixture;
    invalid[40]='X'; // Valid metadata, but not with the original checksum.
    check_rejected_bytes(invalid);
    reseal(invalid);
    check(listing(invalid).name.front()=='X');
    invalid=fixture;invalid[mode_offset]=2;reseal(invalid);check_rejected_bytes(invalid);
    invalid=fixture;invalid[mode_offset+1]=1;reseal(invalid);check_rejected_bytes(invalid);
    invalid=fixture;invalid.pop_back();reseal(invalid);check_rejected_bytes(invalid);
    invalid=fixture;invalid.push_back(0);reseal(invalid);check_rejected_bytes(invalid);
    invalid=fixture;invalid.insert(invalid.end(),fixture_payload.begin(),fixture_payload.end());
    reseal(invalid);check_rejected_bytes(invalid);
    invalid=fixture;invalid.back()^=1;reseal(invalid);check_rejected_bytes(invalid); // zlib trailer checksum
    for(auto count:{0u,2047u,2049u,static_cast<unsigned>(native_replay_max_events+1)}){
        invalid=fixture;set_count(invalid,count);reseal(invalid);check_rejected_bytes(invalid);
    }
    auto bad_events=events;bad_events[0]=0xff;
    check_rejected_bytes(frame(zlib_payload(bad_events)));
    bad_events=events;bad_events[1]=0; // Legacy context is invalid in v3.
    check_rejected_bytes(frame(zlib_payload(bad_events)));
    bad_events=events;bad_events[3]=0x02; // Unused input bit.
    check_rejected_bytes(frame(zlib_payload(bad_events)));
    invalid=fixture;invalid[60]^=1;invalid[96]^=1; // Self-consistent summary count, wrong for the expanded events.
    reseal(invalid);check_rejected_bytes(invalid);
    invalid=raw;invalid.push_back(0);reseal(invalid);check_rejected_bytes(invalid);
    invalid=raw;invalid.pop_back();reseal(invalid);check_rejected_bytes(invalid);

    auto maximum=repeated_run(run,static_cast<std::uint32_t>(native_replay_max_events));
    const auto max_v3=encode(maximum);
    check(get(max_v3,mode_offset,2)==1&&get(max_v3,24,4)==native_replay_max_events);
    check(decode(max_v3)==maximum&&listing(max_v3)==NativeReplayListing{maximum.config,maximum.name,maximum.played_at,maximum.summary});
    maximum.events.push_back(maximum.events.back());
    ++maximum.summary->ticks;++maximum.summary->stage_checks[maximum.summary->final_stage]->ticks;
    check_rejected([&]{encode(maximum);});
}

void v4_practice_cases(){
    PracticeSpec spec;spec.stage=1;spec.section=PracticeSection::Chapter;spec.chapter_id=50;
    spec.lives=5;spec.bombs=5;spec.respawn_bombs=0;spec.dynamic_rank=-6;spec.power=17;
    spec.seed=0xfedcba98;spec.seed_mode=PracticeSeedMode::Random;spec.dialogue=PracticeDialogue::Show;
    NativeReplay run;run.config=practice_replay_config(spec,2,true,true);
    run.practice=NativeReplayPractice{spec,Language::English,0x123456789abcdef0ull};
    for(auto count:{1u,2048u}){
        run=repeated_run(run,count);
        const auto bytes=encode(run);
        check(get(bytes,8,2)==4&&get(bytes,22,2)==(count==1?0:1));
        check(bytes[168]==1&&bytes[169]==51&&bytes[171]==0&&bytes[172]==17&&get(bytes,184,8)==run.practice->start_hash);
        check(decode(bytes)==run&&listing(bytes)==NativeReplayListing{run.config,run.name,run.played_at,run.summary,run.practice,0});
        auto malformed=bytes;malformed.push_back(0);reseal(malformed);check_rejected_bytes(malformed);
        malformed=bytes;malformed.pop_back();reseal(malformed);check_rejected_bytes(malformed);
        malformed=bytes;set_count(malformed,static_cast<unsigned>(native_replay_max_events+1));reseal(malformed);check_rejected_bytes(malformed);
        for(auto [at,value]:{std::pair{168,4},std::pair{169,0},std::pair{170,1},std::pair{171,23},
                              std::pair{172,0},std::pair{173,4},std::pair{174,2},std::pair{175,2},
                              std::pair{176,2},std::pair{176,3},std::pair{177,1},std::pair{17,3}}){
            malformed=bytes;malformed[at]=std::uint8_t(value);reseal(malformed);check_rejected_bytes(malformed);
        }
        check_rejected([&]{encode(run,3);});
    }
    for(auto kind:{NativeReplayEventKind::NextStage,NativeReplayEventKind::CancelContinue}){
        auto invalid=run;invalid.events.push_back({kind});check_rejected([&]{encode(invalid);});
        NativeReplayRecorder recorder(run.config,*run.practice);
        check(!recorder.can_save()&&recorder.record(run.events.front())&&!recorder.record({kind})&&!recorder.can_save());
    }
    auto invalid=run;invalid.events[0].context=NativeReplayContext::Continue;check_rejected([&]{encode(invalid);});
    invalid=run;invalid.config.stage4_trial=true;check_rejected([&]{encode(invalid);});
    invalid=run;invalid.practice->spec.chapter_id=99;check_rejected([&]{encode(invalid);});
    NativeReplayRecorder recorder(run.config,*run.practice);
    for(auto event:run.events)check(recorder.record(event));
    check(recorder.finalize(*run.summary)&&decode(encode(recorder.replay()))==recorder.replay());
}

struct CoreProbe {
    int stage;
    PlayerMotion player;
    StageDialogue dialogue;
    IRand rng;
    unsigned cancels=0;
    explicit CoreProbe(NativeReplayConfig config):stage(config.stage),player(config.shot),rng(config.seed){
        dialogue.boxes.resize(24);dialogue.start_preboss();
    }
    void apply(NativeReplayEvent e){
        switch(e.kind){
        case NativeReplayEventKind::Tick:
            player.tick(e.input);dialogue.tick(e.input!=0,e.dialogue_group,e.dialogue_fast_forward);rng.next();break;
        case NativeReplayEventKind::NextStage:
            ++stage;player=PlayerMotion(player.shot);rng=IRand(rng.state);break;
        case NativeReplayEventKind::CancelContinue:
            ++cancels;break;
        }
    }
    auto snapshot() const{
        return std::array<std::uint64_t,10>{static_cast<unsigned>(stage),static_cast<unsigned>(player.x),
            static_cast<unsigned>(player.y),player.frame,player.back_page,rng.state,dialogue.current,dialogue.reveal.cursor,
            static_cast<unsigned>(dialogue.phase),cancels};
    }
};
}

int main(int argc,char **argv){
    try{
        const TestDirectory storage(argc>1?fs::path(argv[1]):fs::current_path());
        const auto rep_dir=storage.path/"data"/"rep";
        NativeReplayConfig config{0x87654321u,0,2,3,4,0,2,true,true,false,false,true,false};
        fixed_legacy_layout(config);
        NativeReplayRecorder recorder(config);
        check(!recorder.can_save());
        check_rejected([&]{recorder.save(rep_dir);});check(!fs::exists(rep_dir));
        CoreProbe live(config);
        auto tick=[&](std::uint16_t input,bool group,bool fast){
            check(recorder.tick(input,group,fast));
            live.apply({NativeReplayEventKind::Tick,input,group,fast});
        };
        for(int n=0;n<23;++n)tick(0,false,false);
        tick(UpLeft|Shot,false,false);
        check(live.dialogue.current==0&&live.dialogue.phase==StageDialogue::Phase::KeyDelay);
        for(int n=0;n<2;++n)tick(0,false,false);
        for(int n=0;n<2;++n)tick(0,true,false);
        check(live.dialogue.current==1);
        tick(0,false,true);
        check(live.dialogue.current==2);
        check(recorder.next_stage());live.apply({NativeReplayEventKind::NextStage});
        tick(DownRight|Bomb,true,true);
        check(recorder.cancel_continue());live.apply({NativeReplayEventKind::CancelContinue});
        tick(Q|OK|Cancel,false,false);
        check(recorder.can_save());
        const auto first=recorder.save(rep_dir),second=recorder.save(rep_dir);
        check(first.parent_path()==rep_dir&&first.extension()==".rep"&&first!=second);
        check(load_native_replay(first)==recorder.replay());check(load_native_replay(second)==recorder.replay());
        check(std::distance(fs::directory_iterator(rep_dir),fs::directory_iterator{})==2);
        const auto saved=load_native_replay(first);
        CoreProbe replayed(saved.config);
        for(auto event:saved.events)replayed.apply(event);
        check(live.snapshot()==replayed.snapshot());check(replayed.stage==1&&replayed.cancels==1);

        auto bytes=encode(recorder.replay());
        check(bytes.size()==32+4*recorder.replay().events.size());
        check(bytes[8]==1&&bytes[9]==0&&bytes[12]==0x21&&bytes[15]==0x87);
        const auto malformed=storage.path/"bad.rep";
        auto corrupt=[&](Bytes broken){write_bytes(malformed,broken);check_rejected([&]{load_native_replay(malformed);});};
        auto change=[&](std::size_t pos,std::uint8_t value,bool repair){
            auto broken=bytes;broken[pos]=value;if(repair)reseal(broken);corrupt(std::move(broken));
        };
        change(0,'X',true); // magic
        change(8,2,true); // version
        change(10,0x80,true); // unknown launch flag
        change(16,6,true); // stage
        change(22,1,true); // reserved
        change(27,0xff,true); // excessive count
        change(28,bytes[28]^1,false); // checksum
        change(32,18,true); // unknown event kind
        change(32,1,true); // stage transition before a tick
        change(33,4,true); // unknown event flag
        change(35,0x02,true); // unknown tick input flag
        auto truncated=bytes;truncated.pop_back();corrupt(truncated);
        auto extended=bytes;extended.push_back(0);corrupt(extended);
        corrupt(Bytes(header_size+event_size*native_replay_max_events+1));
        check_rejected([&]{load_native_replay(storage.path/"absent.rep");});

        const auto bad_publish=storage.path/"collision";
        write_bytes(bad_publish,bytes);
        check_rejected([&]{publish(bad_publish,first);});
        check(load_native_replay(first)==saved&&load_native_replay(bad_publish)==saved);
        check_rejected([&]{recorder.save(malformed);}); // Existing file is not a directory.
        NativeReplayRecorder bad_input(config);
        check(bad_input.tick(0,false));check(!bad_input.tick(512,false));check(!bad_input.can_save());
        NativeReplayRecorder bad_order(config);
        check(!bad_order.next_stage());check(!bad_order.tick(0,false));check(!bad_order.can_save());
        auto extra_config=config;extra_config.stage=5;extra_config.rank=4;extra_config.extra=true;
        NativeReplayRecorder extra(extra_config);
        check(extra.tick(0,false));check(!extra.next_stage());check(!extra.can_save());
        auto invalid_config=config;invalid_config.lives=-1;
        NativeReplayRecorder invalid(invalid_config);
        check(!invalid.tick(0,false));check(!invalid.can_save());
        NativeReplayRecorder limit(config);
        bool all=true;
        for(std::size_t n=0;n<native_replay_max_events;++n)all=all&&limit.tick(0,false);
        check(all&&limit.can_save());check(!limit.tick(0,false));check(!limit.can_save());
        check_rejected([&]{limit.save(rep_dir);});

        NativeReplayRecorder modern(config);
        check(modern.tick(Shot,false,false,NativeReplayContext::Gameplay));
        check(modern.tick(OK,true,true,NativeReplayContext::StageClear));
        check(modern.next_stage());
        check(modern.tick(0,false,false,NativeReplayContext::Continue));
        check(modern.cancel_continue());
        NativeReplaySummary summary;
        summary.outcome=NativeReplayOutcome::GameOver;summary.final_stage=1;
        summary.final_score=65537;summary.state_hash=0xaabbccdd88776655ull;
        summary.stage_scores[0]=12767;summary.stage_checks[0]=NativeReplayBoundary{2,0x1234567887654321ull};
        summary.final_lives=0;summary.final_bombs=2;summary.ticks=3;
        summary.stage_scores[1]=summary.final_score;
        summary.stage_checks[1]=NativeReplayBoundary{summary.ticks,summary.state_hash};
        auto wrong=summary;wrong.ticks=4;
        check(!modern.finalize(wrong));check(modern.finalize(summary));
        check(!modern.tick(0,false)&&!modern.finalize(summary));
        auto run=modern.replay();run.name="WIN TEST";run.played_at=1'789'101'234;
        const auto bytes_v2=encode(run,v2_version);
        fixed_v2_layout(run,bytes_v2);
        v3_payload_cases(run,bytes_v2);
        v4_practice_cases();
        check_rejected([&]{encode(saved,v2_version);});check_rejected([&]{encode(saved,current_version);});
        check(!inspect_native_replay(first).summary&&inspect_native_replay(first).name.empty());
        const auto meta=listing(bytes_v2);
        check(meta==NativeReplayListing{config,run.name,run.played_at,summary,{},1u<<1});
        check(meta.summary->stage_scores[0]==12767&&meta.summary->stage_scores[1]==65537&&!meta.summary->stage_scores[2]);
        auto corrupt_v2=[&](std::size_t pos,std::uint8_t value){
            auto broken=bytes_v2;broken[pos]=value;reseal(broken);
            check_rejected([&]{decode(broken);});check_rejected([&]{listing(broken);});
        };
        corrupt_v2(40,31); // non-printable name
        corrupt_v2(48,9); // name capacity
        corrupt_v2(49,0); // unspecified outcome
        corrupt_v2(50,0); // inconsistent final stage
        corrupt_v2(51,0); // missing previous stage split
        corrupt_v2(76,2); // mismatched final stage score
        corrupt_v2(52,6); // invalid stock
        corrupt_v2(54,0); // rules revision
        corrupt_v2(55,0); // boundary present without flag
        corrupt_v2(60,4); // terminal sample counter
        corrupt_v2(80,1); // unflagged split data
        corrupt_v2(96,1); // inconsistent boundary sample counter
        corrupt_v2(v2_header_size+1,0); // v2 cannot silently skip context validation
        corrupt_v2(v2_header_size+1,255); // unknown context
        corrupt_v2(v2_header_size+3,0x02); // unused input bits
        auto after_decline=run;after_decline.events.push_back(run.events[3]);++after_decline.summary->ticks;
        check_rejected([&]{encode(after_decline);});
        auto clear=run;clear.summary->outcome=NativeReplayOutcome::Clear;
        check_rejected([&]{encode(clear);}); // explicit Continue decline cannot mean Clear.
        clear.events.pop_back();
        check(decode(encode(clear))==clear);
        auto no_final_split=run;no_final_split.summary->stage_scores[1].reset();
        check_rejected([&]{encode(no_final_split);});
        auto no_final_boundary=run;no_final_boundary.summary->stage_checks[1].reset();
        check_rejected([&]{encode(no_final_boundary);});
        auto bad_final_digest=run;bad_final_digest.summary->stage_checks[1]->state_hash^=1;
        check_rejected([&]{encode(bad_final_digest);});
        auto oversized=Bytes(max_file_size+1);
        corrupt(std::move(oversized));

        const auto numbered=storage.path/"numbered";
        const ReplaySlotStore slots(numbered);
        check(ReplaySlotStore::slot_count==100&&ReplaySlotStore::page_size==10);
        check(slots.path(0).filename()=="slot-00.rep"&&slots.path(99).filename()=="slot-99.rep");
        check_rejected([&]{slots.inspect(100);});
        check(slots.inspect(0).status==ReplaySlotStatus::Empty&&slots.legacy_entries().empty()&&!fs::exists(numbered));
        check_rejected([&]{slots.save(0,saved);});check(!fs::exists(numbered));
        check(slots.save(0,run)==ReplaySlotSaveResult::Saved&&slots.load(0)==run);
        check(get(read_file(slots.path(0)),8,2)==3);
        const auto overwritten=slots.inspect(0);
        check(overwritten.status==ReplaySlotStatus::Ready&&overwritten.listing==meta&&overwritten.can_replace());
        check(slots.save(0,clear)==ReplaySlotSaveResult::NeedsConfirmation&&slots.load(0)==run);
        check(slots.save(0,clear,slots.inspect(0,false))==ReplaySlotSaveResult::Changed&&slots.load(0)==run);
        check(slots.save(1,clear,overwritten)==ReplaySlotSaveResult::Changed);
        write_bytes(slots.path(0),bytes_v2); // Equivalent replay, different exact file between prompt and confirm.
        const auto legacy_format=slots.inspect(0);
        check(legacy_format.status==ReplaySlotStatus::Ready&&legacy_format.listing==meta&&legacy_format.can_replace());
        check(slots.save(0,clear,overwritten)==ReplaySlotSaveResult::Changed&&read_file(slots.path(0))==bytes_v2);
        check(slots.save(0,run,legacy_format)==ReplaySlotSaveResult::Saved&&read_file(slots.path(0))==encode(run));
        const auto rewritten=slots.inspect(0);
        auto replacement=run;replacement.name="REPLACED";
        write_bytes(slots.path(0),encode(replacement)); // external writer between prompt and confirm
        check(slots.save(0,clear,rewritten)==ReplaySlotSaveResult::Changed&&slots.load(0)==replacement);
        const auto occupied=slots.inspect(0);
        auto invalid_run=clear;invalid_run.name="NINECHARS";
        check_rejected([&]{slots.save(0,invalid_run,occupied);});check(slots.load(0)==replacement);
        {
            const replay_slots_detail::WriteLock other(numbered);
            check_rejected([&]{slots.save(0,clear,occupied);});check(slots.load(0)==replacement);
        }
#ifdef _WIN32
        { // Another reader permits our verification reads but denies deletion/replacement.
            const auto target=slots.path(0);
            const auto handle=CreateFileW(target.c_str(),GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,
                nullptr,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,nullptr);
            check(handle!=INVALID_HANDLE_VALUE);
            try{
                bool replace_failed=false;
                try{slots.save(0,clear,occupied);}catch(const fs::filesystem_error &e){
                    replace_failed=e.path2()==target;
                }
                check(replace_failed&&slots.load(0)==replacement);
                check(std::distance(fs::directory_iterator(numbered),fs::directory_iterator{})==1);
            }catch(...){CloseHandle(handle);throw;}
            CloseHandle(handle);
        }
        check(replay_slots_detail::numbered_filename("SLOT-09.REP"));
#endif
        check(slots.save(0,clear,occupied)==ReplaySlotSaveResult::Saved&&slots.load(0)==clear);
        const auto blank=slots.inspect(1);
        const auto compressed_run=repeated_run(run,2048);
        check(slots.save(1,compressed_run,blank)==ReplaySlotSaveResult::Saved);
        const auto compressed_slot=slots.inspect(1);
        auto broken_compressed=read_file(slots.path(1));
        check(get(broken_compressed,22,2)==1&&slots.load(1)==compressed_run);
        check(compressed_slot.status==ReplaySlotStatus::Ready&&compressed_slot.can_replace());
        broken_compressed.push_back(0);reseal(broken_compressed);
        write_bytes(slots.path(1),broken_compressed);
        check(slots.save(1,run,blank)==ReplaySlotSaveResult::Changed);
        check(slots.save(1,run,compressed_slot)==ReplaySlotSaveResult::Changed);
        const auto broken_slot=slots.inspect(1);
        check(broken_slot.status==ReplaySlotStatus::Corrupt&&!broken_slot.listing&&broken_slot.can_replace());
        check(slots.save(1,run)==ReplaySlotSaveResult::NeedsConfirmation);
        check_rejected([&]{slots.load(1);});
        check(slots.save(1,run,broken_slot)==ReplaySlotSaveResult::Saved&&slots.load(1)==run);
        write_bytes(slots.path(2),bytes);
        check(slots.inspect(2).status==ReplaySlotStatus::Ready&&!slots.inspect(2).can_replace());
        check_rejected([&]{slots.save(2,run);});check(read_file(slots.path(2))==bytes);
        fs::create_directory(slots.path(3));
        check(slots.inspect(3).status==ReplaySlotStatus::Corrupt&&!slots.inspect(3).can_replace());
        check_rejected([&]{slots.save(3,run);});check(fs::is_directory(slots.path(3)));
        fs::remove(slots.path(3));
        fs::copy_file(first,numbered/"legacy.rep");
        write_bytes(numbered/"bad-loose.rep",{1,2,3});
        const auto legacy=slots.legacy_entries();
        check(legacy.size()==2&&legacy[0].path.filename()=="legacy.rep"&&legacy[0].listing&&!legacy[0].listing->summary);
        check(legacy[1].status==ReplaySlotStatus::Corrupt&&!legacy[1].listing);
        check(read_file(numbered/"legacy.rep")==bytes);
        check_rejected([&]{ReplaySlotStore(malformed).save(0,run);});
        for(std::size_t slot=3;slot<ReplaySlotStore::slot_count;++slot)check(slots.save(slot,run)==ReplaySlotSaveResult::Saved);
        for(std::size_t slot=0;slot<ReplaySlotStore::slot_count;++slot)check(slots.inspect(slot,false).status==ReplaySlotStatus::Ready);
        check(slots.save(99,clear)==ReplaySlotSaveResult::NeedsConfirmation);
        check(std::distance(fs::directory_iterator(numbered),fs::directory_iterator{})==102); // no locks or temporary artifacts remain
        std::cout<<"native_replay_checks="<<checks<<" failures=0\n";
        return 0;
    }catch(const std::exception &e){std::cerr<<e.what()<<'\n';return 1;}
}
