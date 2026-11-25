#pragma once
#include "native_replay.hpp"
#if defined(_WIN32) || defined(__EMSCRIPTEN__)
#include <bcrypt.h>
#endif
#include <memory>
#if !defined(_WIN32) && !defined(__EMSCRIPTEN__)
#include <cerrno>
#include <fcntl.h>
#include <sys/file.h>
#include <unistd.h>
#endif

namespace soew {
enum class ReplaySlotStatus {Empty,Ready,Corrupt};
enum class ReplaySlotSaveResult {Saved,NeedsConfirmation,Changed};
struct ReplaySlowdownStats {
    std::uint64_t active_ms=0,lost_ms=0;
    bool operator==(const ReplaySlowdownStats&) const=default;
};
class ReplaySlotStore;
struct ReplaySlotEntry {
    ReplaySlotStatus status=ReplaySlotStatus::Empty;
    std::filesystem::path path;
    std::optional<NativeReplayListing> listing;
    std::size_t slot=0;
    bool can_replace()const{return replaceable_;}
private:
    // Exact bytes, not the replay's integrity checksum: a repaired checksum does
    // not make a changed file equivalent to the one the player confirmed.
    std::shared_ptr<const native_replay_detail::Bytes> snapshot_;
    bool replaceable_=false;
    friend class ReplaySlotStore;
};

namespace replay_slots_detail {
namespace fs=std::filesystem;
inline bool absent(const std::error_code &error){return error==std::errc::no_such_file_or_directory;}
inline fs::file_status no_follow_status(const fs::path &path){
    std::error_code error;
    const auto result=fs::symlink_status(path,error);
    if(error&&!absent(error))throw fs::filesystem_error("Cannot inspect replay path",path,error);
    return error?fs::file_status(fs::file_type::not_found):result;
}
inline auto filename_key(const fs::path &path){
    auto key=path.native();
#ifdef _WIN32
    for(auto &c:key)if(c>='A'&&c<='Z')c+=('a'-'A');
#endif
    return key;
}
inline bool numbered_filename(const fs::path &path){
    const auto s=filename_key(path.filename());
    const auto prefix=fs::path("slot-").native(),suffix=fs::path(".rep").native();
    return s.size()==11&&s.compare(0,5,prefix)==0&&s.compare(7,4,suffix)==0&&
        s[5]>='0'&&s[5]<='9'&&s[6]>='0'&&s[6]<='9';
}
inline bool is_v1(const native_replay_detail::Bytes &bytes){
    using namespace native_replay_detail;
    return bytes.size()>=10&&std::equal(magic.begin(),magic.end(),bytes.begin())&&get(bytes,8,2)==version;
}
class WriteLock {
#ifdef _WIN32
    HANDLE handle_=INVALID_HANDLE_VALUE;
#elif !defined(__EMSCRIPTEN__)
    int handle_=-1;
#endif
public:
    explicit WriteLock(const fs::path &dir){
#ifdef _WIN32
        const auto path=dir/".soew-replay-write.lock";
        handle_=CreateFileW(path.c_str(),GENERIC_WRITE|DELETE,0,nullptr,CREATE_NEW,
            FILE_ATTRIBUTE_TEMPORARY|FILE_FLAG_DELETE_ON_CLOSE|FILE_FLAG_OPEN_REPARSE_POINT,nullptr);
        if(handle_==INVALID_HANDLE_VALUE)throw fs::filesystem_error("Cannot lock SoEW replay directory",path,
            std::error_code(static_cast<int>(GetLastError()),std::system_category()));
#elif !defined(__EMSCRIPTEN__)
        handle_=open(dir.c_str(),O_RDONLY|O_DIRECTORY|O_CLOEXEC);
        if(handle_<0)throw fs::filesystem_error("Cannot open SoEW replay directory lock",dir,
            std::error_code(errno,std::generic_category()));
        if(flock(handle_,LOCK_EX|LOCK_NB)<0){
            const std::error_code error(errno,std::generic_category());close(handle_);
            throw fs::filesystem_error("Cannot lock SoEW replay directory",dir,error);
        }
#endif
    }
    ~WriteLock(){
#ifdef _WIN32
        CloseHandle(handle_);
#elif !defined(__EMSCRIPTEN__)
        close(handle_);
#endif
    }
    WriteLock(const WriteLock&)=delete;
    WriteLock& operator=(const WriteLock&)=delete;
};
inline void replace(const fs::path &temp,const fs::path &destination){
#ifdef _WIN32
    if(!MoveFileExW(temp.c_str(),destination.c_str(),MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH))
        throw fs::filesystem_error("Cannot replace replay slot",temp,destination,
            std::error_code(static_cast<int>(GetLastError()),std::system_category()));
#else
    fs::rename(temp,destination);
#endif
}
class PendingFile {
    fs::path directory_,file_;
public:
    PendingFile(const fs::path &rep_dir,const native_replay_detail::Bytes &bytes){
        static std::atomic<std::uint64_t> next_id{0};
        for(unsigned attempt=0;attempt<64;++attempt){
            const auto time=std::chrono::steady_clock::now().time_since_epoch().count();
            auto dir=rep_dir/(".soew-replay-"+std::to_string(time)+"-"+std::to_string(next_id++)+".tmp");
            auto file=dir/"data";
            if(!fs::create_directory(dir))continue;
            directory_=std::move(dir);file_=std::move(file);
            try{
                std::ofstream stream(file_,std::ios::binary);
                stream.write(reinterpret_cast<const char*>(bytes.data()),static_cast<std::streamsize>(bytes.size()));
                stream.close();
                if(!stream||native_replay_detail::read_file(file_)!=bytes)throw std::runtime_error("Cannot verify temporary SoEW replay");
            }catch(...){clean();throw;}
            return;
        }
        throw std::runtime_error("Cannot allocate a temporary SoEW replay file");
    }
    ~PendingFile(){clean();}
    PendingFile(const PendingFile&)=delete;
    PendingFile& operator=(const PendingFile&)=delete;
    const fs::path &file()const{return file_;}
private:
    void clean()noexcept{std::error_code ignored;fs::remove(file_,ignored);fs::remove(directory_,ignored);}
};

using ReplayDigest=std::array<std::uint8_t,32>;
inline std::optional<ReplayDigest> replay_digest(std::span<const std::uint8_t> bytes){
#ifdef __EMSCRIPTEN__
    if(bytes.size()>native_replay_detail::max_file_size)return {};
    ReplayDigest digest{};
    if(BCryptHash(nullptr,nullptr,0,const_cast<PUCHAR>(bytes.data()),bytes.size(),digest.data(),digest.size())<0)return {};
    return digest;
#elif defined(_WIN32)
    if(bytes.size()>native_replay_detail::max_file_size)return {};
    struct Provider {
        BCRYPT_ALG_HANDLE value=nullptr;
        ~Provider(){if(value)BCryptCloseAlgorithmProvider(value,0);}
    } provider;
    if(BCryptOpenAlgorithmProvider(&provider.value,BCRYPT_SHA256_ALGORITHM,nullptr,0)<0)return {};
    DWORD object_size=0,written=0;
    if(BCryptGetProperty(provider.value,BCRYPT_OBJECT_LENGTH,reinterpret_cast<PUCHAR>(&object_size),
                         sizeof(object_size),&written,0)<0||written!=sizeof(object_size))return {};
    std::vector<UCHAR> object(object_size);
    struct Hash {
        BCRYPT_HASH_HANDLE value=nullptr;
        ~Hash(){if(value)BCryptDestroyHash(value);}
    } hash;
    ReplayDigest digest{};
    if(BCryptCreateHash(provider.value,&hash.value,object.data(),object_size,nullptr,0,0)<0||
       (!bytes.empty()&&BCryptHashData(hash.value,const_cast<PUCHAR>(bytes.data()),static_cast<ULONG>(bytes.size()),0)<0)||
       BCryptFinishHash(hash.value,digest.data(),static_cast<ULONG>(digest.size()),0)<0)return {};
    return digest;
#else
    (void)bytes;
    return {}; // Optional Windows host telemetry; .rep remains portable.
#endif
}
inline fs::path slowdown_path(const fs::path &dir,const ReplayDigest &digest){
    constexpr char hex[]="0123456789abcdef";
    std::string filename=".soew-replay-";
    for(const auto byte:digest){filename+=hex[byte>>4];filename+=hex[byte&0x0f];}
    return dir/(filename+".slowdown");
}
inline bool valid_slowdown(ReplaySlowdownStats stats){return stats.active_ms&&stats.lost_ms<=stats.active_ms;}
// Version 1: magic, u16 version, u16 reserved, SHA-256 of REP, two u64
// millisecond counts, CRC32 of the preceding bytes. Integers are little-endian.
inline constexpr std::array<std::uint8_t,8> slowdown_magic={'S','O','E','W','S','L','O','W'};
inline constexpr std::size_t slowdown_size=64;
inline std::uint32_t slowdown_crc(std::span<const std::uint8_t> bytes){
    return static_cast<std::uint32_t>(crc32(0,bytes.data(),static_cast<uInt>(bytes.size())));
}
inline native_replay_detail::Bytes encode_slowdown(const ReplayDigest &digest,ReplaySlowdownStats stats){
    using native_replay_detail::put;
    native_replay_detail::Bytes bytes(slowdown_magic.begin(),slowdown_magic.end());
    put(bytes,1,2);put(bytes,0,2);
    bytes.insert(bytes.end(),digest.begin(),digest.end());
    put(bytes,stats.active_ms,8);put(bytes,stats.lost_ms,8);
    put(bytes,slowdown_crc(bytes),4);
    return bytes;
}
inline std::optional<ReplaySlowdownStats> read_slowdown(const fs::path &path,const ReplayDigest &digest){
    using native_replay_detail::get;
    if(!fs::is_regular_file(no_follow_status(path)))return {};
    std::ifstream stream(path,std::ios::binary);
    std::array<std::uint8_t,slowdown_size> bytes;
    if(!stream.read(reinterpret_cast<char*>(bytes.data()),bytes.size())||
       stream.peek()!=std::char_traits<char>::eof()||stream.bad()||
       !std::equal(slowdown_magic.begin(),slowdown_magic.end(),bytes.begin())||
       get(bytes,8,2)!=1||get(bytes,10,2)!=0||
       !std::equal(digest.begin(),digest.end(),bytes.begin()+12)||
       get(bytes,60,4)!=slowdown_crc(std::span(bytes).first(60)))return {};
    const ReplaySlowdownStats stats{get(bytes,44,8),get(bytes,52,8)};
    return valid_slowdown(stats)?std::optional{stats}:std::nullopt;
}
}

class ReplaySlotStore {
    std::filesystem::path dir_;
    static ReplaySlotEntry inspect_path(const std::filesystem::path &path,std::size_t slot,bool retain){
        using namespace replay_slots_detail;
        ReplaySlotEntry entry;entry.path=path;entry.slot=slot;
        const auto type=no_follow_status(path).type();
        if(type==fs::file_type::not_found)return entry;
        entry.status=ReplaySlotStatus::Corrupt;
        if(type!=fs::file_type::regular)return entry;
        native_replay_detail::Bytes bytes;
        try{bytes=native_replay_detail::read_file(path);}catch(const std::runtime_error&){return entry;}
        entry.replaceable_=!is_v1(bytes);
        try{
            entry.listing=native_replay_detail::listing(bytes);
            entry.status=ReplaySlotStatus::Ready;
        }catch(const std::runtime_error&){} // Malformed and unoccupied slots are distinct.
        if(retain&&entry.replaceable_)
            entry.snapshot_=std::make_shared<const native_replay_detail::Bytes>(std::move(bytes));
        return entry;
    }
    static bool same_target(const ReplaySlotEntry &now,const ReplaySlotEntry &confirmed){
        return now.path==confirmed.path&&now.slot==confirmed.slot&&now.status==confirmed.status&&
            (now.status==ReplaySlotStatus::Empty||
             (now.snapshot_&&confirmed.snapshot_&&*now.snapshot_==*confirmed.snapshot_));
    }
public:
    static constexpr std::size_t slot_count=100,page_size=10;
    explicit ReplaySlotStore(std::filesystem::path directory):dir_(std::move(directory)){}
    std::filesystem::path path(std::size_t slot)const{
        if(slot>=slot_count)throw std::out_of_range("Replay slot number must be 00 through 99");
        return dir_/("slot-"+std::string(slot<10?"0":"")+std::to_string(slot)+".rep");
    }
    ReplaySlotEntry inspect(std::size_t slot,bool retain=true)const{return inspect_path(path(slot),slot,retain);}
    NativeReplay load(std::size_t slot)const{
        const auto file=path(slot);
        if(!std::filesystem::is_regular_file(replay_slots_detail::no_follow_status(file)))
            throw std::runtime_error("Replay slot is not a regular file");
        return load_native_replay(file);
    }
    std::vector<ReplaySlotEntry> legacy_entries()const{
        using namespace replay_slots_detail;
        const auto type=no_follow_status(dir_).type();
        if(type==fs::file_type::not_found)return {};
        if(type!=fs::file_type::directory)throw std::runtime_error("SoEW replay folder is not a directory");
        // Bound both retained paths and the number of loose files parsed per listing.
        constexpr auto keep=slot_count;
        std::vector<fs::path> files;
        for(const auto &item:fs::directory_iterator(dir_)){
            const auto &file=item.path();
            if(filename_key(file.extension())!=fs::path(".rep").native()||numbered_filename(file)||!fs::is_regular_file(item.symlink_status()))continue;
            const auto at=std::lower_bound(files.begin(),files.end(),file,std::greater<fs::path>{});
            if(at==files.end()&&files.size()==keep)continue;
            files.insert(at,file);
            if(files.size()>keep)files.pop_back();
        }
        std::vector<ReplaySlotEntry> entries;
        for(const auto &file:files)entries.push_back(inspect_path(file,slot_count,false));
        return entries;
    }
    std::optional<ReplaySlowdownStats> slowdown_stats(const ReplaySlotEntry &entry)const noexcept{
        using namespace replay_slots_detail;
        if(entry.status!=ReplaySlotStatus::Ready||!entry.listing)return {};
        try{
            if(entry.path!=dir_/entry.path.filename()||!fs::is_regular_file(no_follow_status(entry.path)))return {};
            const auto bytes=native_replay_detail::read_file(entry.path);
            // Validate exactly the bytes being hashed, not a separate read of the REP.
            if(entry.snapshot_?*entry.snapshot_!=bytes:native_replay_detail::listing(bytes)!=*entry.listing)return {};
            const auto digest=replay_digest(bytes);
            if(digest)return read_slowdown(slowdown_path(dir_,*digest),*digest);
        }catch(const std::exception&){} // Missing or unreadable metadata is not a replay error.
        return {};
    }
    ReplaySlotSaveResult save(std::size_t slot,const NativeReplay &run,const ReplaySlotEntry *confirmed=nullptr,
                              std::optional<ReplaySlowdownStats> slowdown=std::nullopt)const{
        using namespace replay_slots_detail;
        auto before=inspect(slot,confirmed!=nullptr);
        if(confirmed&&!same_target(before,*confirmed))return ReplaySlotSaveResult::Changed;
        if(before.status!=ReplaySlotStatus::Empty){
            if(!before.can_replace())throw std::runtime_error("Cannot replace this replay file (legacy, unreadable, or not a regular file)");
            if(!confirmed)return ReplaySlotSaveResult::NeedsConfirmation;
        }
        if(!run.summary)throw std::runtime_error("Only a finalized SoEW replay can be saved to a slot");
        const auto bytes=native_replay_detail::encode(run);
        fs::create_directories(dir_);
        const WriteLock lock(dir_);
        if(!same_target(inspect(slot,before.status!=ReplaySlotStatus::Empty),before))return ReplaySlotSaveResult::Changed;
        const PendingFile pending(dir_,bytes);
        if(!same_target(inspect(slot,before.status!=ReplaySlotStatus::Empty),before))return ReplaySlotSaveResult::Changed;
        if(before.status==ReplaySlotStatus::Empty)native_replay_detail::publish(pending.file(),before.path);
        else replace(pending.file(),before.path);
        if(slowdown&&valid_slowdown(*slowdown))try{
            if(const auto digest=replay_digest(bytes)){
                const PendingFile metadata(dir_,encode_slowdown(*digest,*slowdown));
                replace(metadata.file(),slowdown_path(dir_,*digest));
            }
        }catch(const std::exception&){} // The REP is already safely published.
        return ReplaySlotSaveResult::Saved;
    }
    ReplaySlotSaveResult save(std::size_t slot,const NativeReplay &run,const ReplaySlotEntry &confirmed,
                              std::optional<ReplaySlowdownStats> slowdown=std::nullopt)const{
        return save(slot,run,&confirmed,slowdown);
    }
};
}
