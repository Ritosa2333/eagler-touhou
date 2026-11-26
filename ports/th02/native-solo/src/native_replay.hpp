#pragma once
#include "core_contract.hpp"
#include "player_motion.hpp"
#include "practice_targets.hpp"
#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <optional>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>
#include <vector>
#include <zlib.h>
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

namespace soew {
struct NativeReplayConfig {
    std::uint32_t seed=1234;
    int stage=0,shot=1,rank=1,lives=2,bombs=3,music=1;
    bool reduced=false,autofire=false,stage4_trial=false,stage5_trial=false,stage5_mainline=false,extra=false;
    bool operator==(const NativeReplayConfig&) const=default;
};
enum class NativeReplayEventKind : std::uint8_t {Tick=0,NextStage=1,CancelContinue=2};
enum class NativeReplayContext : std::uint8_t {Legacy=0,Gameplay=1,Dialogue=2,Continue=3,StageClear=4};
struct NativeReplayEvent {
    NativeReplayEventKind kind=NativeReplayEventKind::Tick;
    std::uint16_t input=0;
    bool dialogue_group=false,dialogue_fast_forward=false;
    NativeReplayContext context=NativeReplayContext::Legacy;
    std::uint64_t pointer=0;
    bool operator==(const NativeReplayEvent&) const=default;
};
enum class NativeReplayOutcome : std::uint8_t {MenuReturn=1,Aborted=MenuReturn,GameOver=2,Clear=3};
struct NativeReplayBoundary {
    std::uint32_t ticks=0; // Cumulative simulation samples at this boundary.
    std::uint64_t state_hash=0;
    bool operator==(const NativeReplayBoundary&) const=default;
};
struct NativeReplaySummary {
    NativeReplayOutcome outcome=NativeReplayOutcome::Aborted;
    std::uint8_t final_stage=0;
    std::uint32_t final_score=0;
    std::uint64_t state_hash=0;
    std::array<std::optional<std::uint32_t>,6> stage_scores{};
    std::uint8_t final_lives=0,final_bombs=0;
    std::uint32_t ticks=0;
    std::array<std::optional<NativeReplayBoundary>,6> stage_checks{};
    bool operator==(const NativeReplaySummary&) const=default;
};
struct NativeReplayPractice {
    PracticeSpec spec; // The seed is the actual seed; playback never rerolls it.
    Language language=Language::Japanese; // Select the same dialogue assets.
    std::uint64_t start_hash=0;
    PracticeStartRule start_rule=PracticeStartRule::LegacyV4;
    bool operator==(const NativeReplayPractice&) const=default;
};
struct NativeReplay {
    NativeReplayConfig config;
    std::vector<NativeReplayEvent> events;
    std::string name;
    std::uint64_t played_at=0; // Unix seconds, UTC.
    std::optional<NativeReplaySummary> summary;
    std::optional<NativeReplayPractice> practice;
    bool operator==(const NativeReplay&) const=default;
};
struct NativeReplayListing {
    NativeReplayConfig config;
    std::string name;
    std::uint64_t played_at=0;
    std::optional<NativeReplaySummary> summary;
    std::optional<NativeReplayPractice> practice;
    std::uint8_t seek_stages=0; // Later Story stages containing at least one tick.
    bool operator==(const NativeReplayListing&) const=default;
};
inline NativeReplayConfig practice_replay_config(const PracticeSpec &spec,int music,bool reduced,bool autofire){
    return {spec.seed,spec.stage,spec.shot,spec.rank,spec.lives,spec.bombs,music,reduced,autofire};
}
inline constexpr std::size_t native_replay_max_events=1'000'000,native_replay_max_name=8;

namespace native_replay_detail {
using Bytes=std::vector<std::uint8_t>;
inline constexpr std::array<std::uint8_t,8> magic={'S','O','E','W','R','P','L','Y'};
inline constexpr std::size_t header_size=32,v2_header_size=168,v4_header_size=192,event_size=4;
inline constexpr std::size_t v5_header_size=v4_header_size+1;
inline constexpr std::size_t pointer_event_size=event_size+6;
inline constexpr std::size_t max_file_size=v5_header_size+pointer_event_size*native_replay_max_events;
inline constexpr std::uint16_t version=1,v2_version=2,current_version=3,practice_version=4,calibrated_practice_version=5;
inline constexpr std::uint16_t raw_payload=0,deflate_payload=1;
inline constexpr std::uint16_t pointer_payload=2;
inline constexpr std::uint8_t rules_revision=1;
inline constexpr std::uint64_t max_played_at=253402300799ull; // 9999-12-31 23:59:59 UTC.
inline constexpr std::uint16_t launch_bits=0x3f;
inline bool valid_config(const NativeReplayConfig &v,bool practice=false){
    return v.stage>=0&&v.stage<=5&&v.shot>=0&&v.shot<=2&&v.rank>=0&&v.rank<=4&&
        v.lives>=0&&v.lives<=(practice?5:4)&&v.bombs>=0&&v.bombs<=(practice?5:3)&&v.music>=0&&v.music<=2&&
        (!v.stage4_trial||v.stage==3)&&(!v.stage5_trial||v.stage==4)&&
        (!v.extra||(v.stage==5&&v.rank==4));
}
inline bool valid_practice_config(const NativeReplayConfig &config,const NativeReplayPractice &practice){
    static const auto capabilities=practice_capabilities();
    const auto &s=practice.spec;
    return valid_config(config,true)&&config==practice_replay_config(s,config.music,config.reduced,config.autofire)&&
        (practice.language==Language::Japanese||practice.language==Language::English)&&
        (practice.start_rule==PracticeStartRule::LegacyV4||
         (practice.start_rule==PracticeStartRule::Calibrated&&practice_start_rule(s)==PracticeStartRule::Calibrated))&&
        validate_practice(s,capabilities).empty();
}
struct EventSequence {
    int stage;
    bool stage_has_tick=false;
    bool practice=false;
    bool accept(const NativeReplayEvent &e){
        if(e.kind==NativeReplayEventKind::Tick){
            if((e.input&~native_input_mask)||e.context>NativeReplayContext::StageClear||
               (e.pointer&&(e.context!=NativeReplayContext::Gameplay||!valid_pointer_motion(e.pointer)))||
               (practice&&(e.context==NativeReplayContext::Legacy||e.context==NativeReplayContext::Continue)))return false;
            stage_has_tick=true;
            return true;
        }
        if(practice)return false;
        if(e.input||e.dialogue_group||e.dialogue_fast_forward||e.pointer||e.context!=NativeReplayContext::Legacy||!stage_has_tick)return false;
        if(e.kind==NativeReplayEventKind::CancelContinue)return true;
        if(e.kind!=NativeReplayEventKind::NextStage||stage>=4)return false;
        ++stage;stage_has_tick=false;
        return true;
    }
};
inline void put(Bytes &out,std::uint64_t value,unsigned width){
    for(unsigned i=0;i<width;++i)out.push_back(static_cast<std::uint8_t>(value>>(i*8)));
}
inline std::uint64_t get(std::span<const std::uint8_t> in,std::size_t at,unsigned width){
    std::uint64_t value=0;
    for(unsigned i=0;i<width;++i)value|=std::uint64_t(in[at+i])<<(i*8);
    return value;
}
inline std::uint32_t checksum(const Bytes &bytes){
    std::uint32_t hash=2166136261u;
    for(std::size_t i=0;i<bytes.size();++i)if(i<28||i>=header_size)hash=(hash^bytes[i])*16777619u;
    return hash;
}
inline bool valid_name(std::string_view name){
    return name.size()<=native_replay_max_name&&std::all_of(name.begin(),name.end(),[](unsigned char c){return c>=32&&c<=126;});
}
struct ReplaySequence {
    EventSequence sequence;
    std::uint32_t ticks=0;
    std::array<std::uint32_t,6> boundary_ticks{};
    bool v2,continue_declined=false;
    std::uint8_t tick_stages=0;
    bool accept(const NativeReplayEvent &e){
        if(v2&&continue_declined)return false;
        if(e.kind==NativeReplayEventKind::Tick){
            if((e.context!=NativeReplayContext::Legacy)!=v2)return false;
        }else if(e.kind==NativeReplayEventKind::NextStage){
            boundary_ticks[sequence.stage]=ticks;
        }
        if(!sequence.accept(e))return false;
        if(e.kind==NativeReplayEventKind::Tick){++ticks;tick_stages|=std::uint8_t(1u<<sequence.stage);}
        if(e.kind==NativeReplayEventKind::CancelContinue)continue_declined=true;
        return true;
    }
    bool matches(const NativeReplaySummary &s)const{
        if(s.outcome<NativeReplayOutcome::MenuReturn||s.outcome>NativeReplayOutcome::Clear||
           s.final_stage!=sequence.stage||s.final_lives>5||s.final_bombs>5||s.ticks!=ticks||
           (continue_declined&&s.outcome!=NativeReplayOutcome::GameOver))return false;
        for(std::size_t i=0;i<s.stage_scores.size();++i){
            if(boundary_ticks[i]){
                if(!s.stage_scores[i]||!s.stage_checks[i]||s.stage_checks[i]->ticks!=boundary_ticks[i])return false;
            }else if(i==s.final_stage){
                if(!s.stage_scores[i]||*s.stage_scores[i]!=s.final_score||!s.stage_checks[i]||
                   *s.stage_checks[i]!=NativeReplayBoundary{s.ticks,s.state_hash})return false;
            }else if(s.stage_scores[i]||s.stage_checks[i])return false;
        }
        return true;
    }
};
inline Bytes encode(const NativeReplay &run,std::uint16_t file_version){
    const auto &v=run.config;
    const bool v5=file_version==calibrated_practice_version;
    const bool v2=file_version!=version,v4=file_version==practice_version||v5;
    const bool motion=std::ranges::any_of(run.events,[](const auto &event){return event.pointer!=0;});
    if(motion&&file_version!=current_version&&!v4)throw std::runtime_error("This replay layout cannot store touch motion");
    const auto events_start=v5?v5_header_size:v4?v4_header_size:v2?v2_header_size:header_size;
    if(run.practice && run.practice->start_rule!=(v5?PracticeStartRule::Calibrated:PracticeStartRule::LegacyV4))
        throw std::runtime_error("Practice replay start rule does not match the format");
    if((file_version!=version&&file_version!=v2_version&&file_version!=current_version&&!v4)||
       v2!=run.summary.has_value()||v4!=run.practice.has_value())throw std::runtime_error("Invalid native replay version for this recording");
    if(!(v4?valid_practice_config(v,*run.practice):valid_config(v))||!valid_name(run.name)||
       run.played_at>max_played_at||run.events.empty()||run.events.size()>native_replay_max_events||
       (!v2&&(!run.name.empty()||run.played_at)))throw std::runtime_error("Invalid or empty native replay");
    // All layouts are little-endian. V4 appends the Practice start recipe to
    // the existing fixed listing and terminal header. Digital-only streams
    // keep their layout; the pointer payload adds a six-byte field per event.
    Bytes out;out.reserve(events_start+(motion?pointer_event_size:event_size)*run.events.size());
    out.insert(out.end(),magic.begin(),magic.end());put(out,file_version,2);
    put(out,unsigned(v.reduced)|(unsigned(v.autofire)<<1)|(unsigned(v.stage4_trial)<<2)|
        (unsigned(v.stage5_trial)<<3)|(unsigned(v.stage5_mainline)<<4)|(unsigned(v.extra)<<5),2);
    put(out,v.seed,4);
    for(int n:{v.stage,v.shot,v.rank,v.lives,v.bombs,v.music})put(out,static_cast<unsigned>(n),1);
    put(out,motion?pointer_payload:raw_payload,2);put(out,run.events.size(),4);put(out,0,4);
    if(v2){
        const auto &s=*run.summary;
        put(out,run.played_at,8);
        out.insert(out.end(),run.name.begin(),run.name.end());
        out.resize(48,0);
        put(out,run.name.size(),1);put(out,static_cast<unsigned>(s.outcome),1);put(out,s.final_stage,1);
        unsigned score_bits=0,check_bits=0;
        for(unsigned i=0;i<s.stage_scores.size();++i){
            if(s.stage_scores[i])score_bits|=1u<<i;
            if(s.stage_checks[i])check_bits|=1u<<i;
        }
        put(out,score_bits,1);put(out,s.final_lives,1);put(out,s.final_bombs,1);
        put(out,rules_revision,1);put(out,check_bits,1);
        put(out,s.final_score,4);put(out,s.ticks,4);put(out,s.state_hash,8);
        for(const auto score:s.stage_scores)put(out,score.value_or(0),4);
        for(const auto &check:s.stage_checks){
            const auto boundary=check.value_or(NativeReplayBoundary{});
            put(out,boundary.ticks,4);put(out,boundary.state_hash,8);
        }
    }
    if(v4){
        const auto &p=*run.practice;
        const auto &s=p.spec;
        // Signed index/rank use biases; zero chapter code means no chapter.
        for(int n:{int(s.section),s.chapter_id+1,s.boss_phase,s.dynamic_rank+6,s.power,s.respawn_bombs,
                   int(s.dialogue),int(s.seed_mode),int(p.language)})put(out,static_cast<unsigned>(n),1);
        out.resize(184,0);put(out,p.start_hash,8);
        if(v5)put(out,1,1); // Explicit calibrated entry rule; v4 reserved bytes stay zero.
    }
    ReplaySequence sequence{{v.stage,false,v4},0,{},v2};
    for(const auto &e:run.events){
        if(!sequence.accept(e))throw std::runtime_error("Invalid native replay event sequence");
        put(out,static_cast<unsigned>(e.kind),1);
        put(out,unsigned(e.dialogue_group)|(unsigned(e.dialogue_fast_forward)<<1)|(unsigned(e.context)<<2),1);
        put(out,e.input,2);
        if(motion)put(out,e.pointer,6);
    }
    if(v2&&!sequence.matches(*run.summary))throw std::runtime_error("Invalid native replay terminal or split summary");
    if(file_version==current_version||v4){
        const auto size=static_cast<uLong>(out.size()-events_start);
        // Limit the output to strictly less than the original event payload.
        // Z_BUF_ERROR here simply means that raw storage is smaller or equal.
        Bytes compressed(size-1);
        auto written=static_cast<uLongf>(compressed.size());
        const auto result=compress2(compressed.data(),&written,out.data()+events_start,size,Z_BEST_COMPRESSION);
        if(result==Z_OK){
            out.resize(events_start);
            out[22]|=deflate_payload;
            out.insert(out.end(),compressed.begin(),compressed.begin()+written);
        }else if(result!=Z_BUF_ERROR)throw std::runtime_error("Cannot compress native replay");
    }
    const auto sum=checksum(out);
    for(unsigned i=0;i<4;++i)out[28+i]=static_cast<std::uint8_t>(sum>>(i*8));
    return out;
}
inline Bytes encode(const NativeReplay &run){
    return encode(run,run.practice?(run.practice->start_rule==PracticeStartRule::Calibrated?calibrated_practice_version:practice_version):run.summary?current_version:version);
}
inline Bytes inflate_events(std::span<const std::uint8_t> payload,std::size_t expected){
    Bytes events(expected);
    uLongf size=static_cast<uLongf>(expected);
    uLong consumed=static_cast<uLong>(payload.size());
    // uncompress2 reports the actual input consumed, unlike uncompress.
    if(uncompress2(events.data(),&size,payload.data(),&consumed)!=Z_OK||
       size!=expected||consumed!=payload.size())
        throw std::runtime_error("Invalid compressed native replay payload");
    return events;
}
inline NativeReplay decode(const Bytes &bytes,bool include_events=true,std::uint8_t *seek_stages=nullptr){
    if(bytes.size()<header_size||bytes.size()>max_file_size||!std::equal(magic.begin(),magic.end(),bytes.begin()))
        throw std::runtime_error("Not a native SoEW replay or invalid file size");
    const auto file_version=get(bytes,8,2);
    const bool v5=file_version==calibrated_practice_version;
    const bool v4=file_version==practice_version||v5;
    const bool v2=file_version==v2_version||file_version==current_version||v4;
    if(!v2&&file_version!=version)throw std::runtime_error("Unsupported native replay version");
    const auto flags=get(bytes,10,2),mode=get(bytes,22,2),count=get(bytes,24,4);
    const auto events_start=v5?v5_header_size:v4?v4_header_size:v2?v2_header_size:header_size;
    if((flags&~std::uint64_t(launch_bits))||!count||count>native_replay_max_events||
       (mode&&((file_version!=current_version&&!v4)||(mode&~(deflate_payload|pointer_payload))))||
       bytes.size()<events_start)
        throw std::runtime_error("Invalid native replay header");
    const bool compressed=mode&deflate_payload,motion=mode&pointer_payload;
    const auto stride=motion?pointer_event_size:event_size;
    const auto expected=static_cast<std::size_t>(count)*stride;
    const auto stored=std::span<const std::uint8_t>(bytes).subspan(events_start);
    if((compressed?stored.empty()||stored.size()>=expected:stored.size()!=expected)||
       get(bytes,28,4)!=checksum(bytes))
        throw std::runtime_error("Invalid native replay size or checksum");
    NativeReplay run;
    run.config={static_cast<std::uint32_t>(get(bytes,12,4)),bytes[16],bytes[17],bytes[18],bytes[19],bytes[20],bytes[21],
        bool(flags&1),bool(flags&2),bool(flags&4),bool(flags&8),bool(flags&16),bool(flags&32)};
    if(v4){
        if(std::any_of(bytes.begin()+177,bytes.begin()+184,[](std::uint8_t b){return b!=0;}))
            throw std::runtime_error("Invalid Practice replay reserved data");
        auto &p=run.practice.emplace();
        auto &s=p.spec;
        const auto &c=run.config;
        s.stage=c.stage;s.shot=c.shot;s.rank=c.rank;s.lives=c.lives;s.bombs=c.bombs;s.seed=c.seed;
        s.section=static_cast<PracticeSection>(bytes[168]);s.chapter_id=int(bytes[169])-1;
        s.boss_phase=bytes[170];s.dynamic_rank=int(bytes[171])-6;s.power=bytes[172];s.respawn_bombs=bytes[173];
        s.dialogue=static_cast<PracticeDialogue>(bytes[174]);s.seed_mode=static_cast<PracticeSeedMode>(bytes[175]);
        p.language=static_cast<Language>(bytes[176]);p.start_hash=get(bytes,184,8);
        if(v5){
            if(bytes[192]!=1)throw std::runtime_error("Unsupported Practice start rule");
            p.start_rule=PracticeStartRule::Calibrated;
        }
    }
    if(!(v4?valid_practice_config(run.config,*run.practice):valid_config(run.config)))
        throw std::runtime_error("Invalid native replay launch settings");
    if(v2){
        const auto length=bytes[48],score_bits=bytes[51],check_bits=bytes[55];
        run.played_at=get(bytes,32,8);
        if(length>native_replay_max_name||run.played_at>max_played_at||(score_bits&0xc0)||(check_bits&0xc0)||
           std::any_of(bytes.begin()+40+length,bytes.begin()+48,[](std::uint8_t b){return b!=0;}))
            throw std::runtime_error("Invalid native replay listing header");
        if(bytes[54]!=rules_revision)throw std::runtime_error("Unsupported native replay rules revision");
        run.name.assign(bytes.begin()+40,bytes.begin()+40+length);
        if(!valid_name(run.name))throw std::runtime_error("Invalid native replay name");
        auto &s=run.summary.emplace();
        s.outcome=static_cast<NativeReplayOutcome>(bytes[49]);s.final_stage=bytes[50];
        s.final_lives=bytes[52];s.final_bombs=bytes[53];s.final_score=static_cast<std::uint32_t>(get(bytes,56,4));
        s.ticks=static_cast<std::uint32_t>(get(bytes,60,4));s.state_hash=get(bytes,64,8);
        for(unsigned i=0;i<s.stage_scores.size();++i){
            const auto score=static_cast<std::uint32_t>(get(bytes,72+i*4,4));
            const NativeReplayBoundary boundary{static_cast<std::uint32_t>(get(bytes,96+i*12,4)),get(bytes,100+i*12,8)};
            if(score_bits&(1u<<i))s.stage_scores[i]=score;
            else if(score)throw std::runtime_error("Invalid empty native replay score split");
            if(check_bits&(1u<<i))s.stage_checks[i]=boundary;
            else if(boundary!=NativeReplayBoundary{})throw std::runtime_error("Invalid empty native replay boundary");
        }
    }
    Bytes inflated;
    auto events=stored;
    if(compressed){
        inflated=inflate_events(stored,expected);
        events=inflated;
    }
    if(include_events)run.events.reserve(count);
    ReplaySequence sequence{{run.config.stage,false,v4},0,{},v2};
    for(std::size_t at=0;at<events.size();at+=stride){
        const NativeReplayEvent e{static_cast<NativeReplayEventKind>(events[at]),
            static_cast<std::uint16_t>(get(events,at+2,2)),bool(events[at+1]&1),bool(events[at+1]&2),
            static_cast<NativeReplayContext>(events[at+1]>>2),motion?get(events,at+4,6):0};
        if(!sequence.accept(e))throw std::runtime_error("Invalid native replay event sequence");
        if(include_events)run.events.push_back(e);
    }
    if(v2&&!sequence.matches(*run.summary))throw std::runtime_error("Invalid native replay terminal or split summary");
    if(seek_stages)*seek_stages=v4||run.config.extra||run.config.stage==5?0:
        std::uint8_t(sequence.tick_stages&~(1u<<run.config.stage));
    return run;
}
inline void publish(const std::filesystem::path &temp,const std::filesystem::path &final){
    // The complete replay is published without replacing an existing file.
#ifdef _WIN32
    if(!MoveFileExW(temp.c_str(),final.c_str(),MOVEFILE_WRITE_THROUGH))
        throw std::filesystem::filesystem_error("Cannot publish native replay",temp,final,
            std::error_code(static_cast<int>(GetLastError()),std::system_category()));
#elif defined(__EMSCRIPTEN__)
    // IDBFS uses a private, single-threaded MEMFS tree without hard links.
    if(std::filesystem::exists(std::filesystem::symlink_status(final)))
        throw std::filesystem::filesystem_error("Cannot publish native replay",temp,final,
            std::make_error_code(std::errc::file_exists));
    std::filesystem::rename(temp,final);
#else
    std::filesystem::create_hard_link(temp,final);
#endif
}
}

namespace native_replay_detail {
inline Bytes read_file(const std::filesystem::path &path){
    std::ifstream file(path,std::ios::binary|std::ios::ate);
    if(!file)throw std::runtime_error("Cannot open native replay");
    const auto size=file.tellg();
    if(size<0||size>static_cast<std::streamoff>(max_file_size))throw std::runtime_error("Invalid native replay file size");
    Bytes bytes(static_cast<std::size_t>(size));
    file.seekg(0);
    if((!bytes.empty()&&!file.read(reinterpret_cast<char*>(bytes.data()),static_cast<std::streamsize>(bytes.size())))||
        file.peek()!=std::char_traits<char>::eof()||file.bad())throw std::runtime_error("Cannot read native replay");
    return bytes;
}
inline NativeReplayListing listing(const Bytes &bytes){
    std::uint8_t seek_stages=0;
    auto run=decode(bytes,false,&seek_stages);
    return {run.config,std::move(run.name),run.played_at,std::move(run.summary),std::move(run.practice),seek_stages};
}
}
inline NativeReplay load_native_replay(const std::filesystem::path &path){
    return native_replay_detail::decode(native_replay_detail::read_file(path));
}
inline NativeReplayListing inspect_native_replay(const std::filesystem::path &path){
    return native_replay_detail::listing(native_replay_detail::read_file(path));
}

class NativeReplayRecorder {
    NativeReplay replay_;
    native_replay_detail::EventSequence sequence_;
    bool valid_;
    bool append(NativeReplayEvent event){
        if(!valid_||replay_.summary)return false;
        auto sequence=sequence_;
        if(replay_.events.size()>=native_replay_max_events||!sequence.accept(event))return valid_=false;
        replay_.events.push_back(event);sequence_=sequence;return true;
    }
public:
    explicit NativeReplayRecorder(NativeReplayConfig config={})
        :replay_{config,{}, {},0,{},{}},sequence_{config.stage},valid_(native_replay_detail::valid_config(config)){}
    NativeReplayRecorder(NativeReplayConfig config,NativeReplayPractice practice)
        :replay_{config,{}, {},0,{},std::move(practice)},sequence_{config.stage,false,true},
         valid_(native_replay_detail::valid_practice_config(config,*replay_.practice)){}
    bool tick(std::uint16_t input,bool dialogue_group,bool dialogue_fast_forward=false,
              NativeReplayContext context=NativeReplayContext::Legacy){
        return append({NativeReplayEventKind::Tick,input,dialogue_group,dialogue_fast_forward,context});
    }
    bool next_stage(){return append({NativeReplayEventKind::NextStage});}
    bool cancel_continue(){return append({NativeReplayEventKind::CancelContinue});}
    bool record(NativeReplayEvent event){return append(event);}
    bool finalize(const NativeReplaySummary &summary){
        if(!can_save()||replay_.summary)return false;
        native_replay_detail::ReplaySequence sequence{{replay_.config.stage,false,replay_.practice.has_value()},0,{},true};
        for(const auto &event:replay_.events)if(!sequence.accept(event))return false;
        if(!sequence.matches(summary))return false;
        replay_.summary=summary;
        const auto now=std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        if(now>0&&std::uint64_t(now)<=native_replay_detail::max_played_at)replay_.played_at=std::uint64_t(now);
        return true;
    }
    bool can_save()const{return valid_&&!replay_.events.empty();}
    const NativeReplay &replay()const{return replay_;}
    std::filesystem::path save(const std::filesystem::path &rep_dir)const{
        namespace fs=std::filesystem;
        if(!can_save())throw std::runtime_error("No complete native replay to save");
        const auto bytes=native_replay_detail::encode(replay_);
        fs::create_directories(rep_dir);
        static std::atomic<std::uint64_t> next_id{0};
        for(unsigned attempt=0;attempt<64;++attempt){
            const auto stamp=std::chrono::system_clock::now().time_since_epoch().count();
            const auto name="soew-"+std::to_string(stamp)+"-"+std::to_string(next_id++);
            const auto final=rep_dir/(name+".rep"),work=rep_dir/(name+".tmp"),temp=work/"data";
            if(fs::exists(final)||!fs::create_directory(work))continue;
            auto clean=[&]{std::error_code ignored;fs::remove(temp,ignored);fs::remove(work,ignored);};
            try{
                std::ofstream file(temp,std::ios::binary);
                file.write(reinterpret_cast<const char*>(bytes.data()),static_cast<std::streamsize>(bytes.size()));
                file.close();
                if(!file)throw std::runtime_error("Cannot write native replay");
                native_replay_detail::publish(temp,final);
            }catch(...){clean();throw;}
            clean();return final;
        }
        throw std::runtime_error("Cannot allocate a unique native replay name");
    }
};
}
