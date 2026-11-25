#pragma once

#include "hiscore.hpp"
#include <algorithm>
#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <windows.h>

namespace soew {

struct NativeScoreEntry {
    static constexpr unsigned all_stages = 127, extra_stage = 6;
    std::string name;
    unsigned difficulty = 1; // 0–3: Easy through Lunatic, 4: Extra.
    unsigned shot_type = 0;  // 0–2: A, B, C.
    unsigned continues_used = 0;
    std::uint32_t score = 0; // Live score, before encoding the continue digit.
    bool good_clear = false; // Main game good ending, not Extra or a continued clear.
    unsigned stage = 0;     // 0: unspecified (v1), 1–6: stage reached, 127: ALL.
    bool gaiji_name = false; // v3 can store six original MIKOFT glyph indices.
};

class NativeScoreStore {
public:
    static constexpr unsigned difficulty_count = 5, shot_type_count = 3;
    static constexpr std::size_t places = 10, name_length = 6;
    static constexpr std::string_view default_name = "PLAYER";
    static constexpr wchar_t filename[] = L"soew-native-scores.dat";
    enum class Status { Ok, Missing, Invalid, UnsupportedVersion, IoError, NotRanked, InvalidEntry };

    // The caller supplies an existing native data directory, not a filename.
    explicit NativeScoreStore(const std::filesystem::path& directory)
        : path_(std::filesystem::absolute(directory) / filename) {}

    const std::vector<NativeScoreEntry>& entries(unsigned difficulty) const {
        return difficulty < difficulty_count ? data_.boards[difficulty] : empty_;
    }

    Status load() {
        Data read;
        const auto status = read_file(read);
        if (status == Status::Ok || status == Status::Missing) data_ = std::move(read);
        return status;
    }

    bool has_good_clear(unsigned shot_type) const {
        return shot_type < shot_type_count && (data_.good_clears & (1u << shot_type)) != 0;
    }

    bool has_extra_clear(unsigned shot_type) const {
        return shot_type < shot_type_count && (data_.extra_clears & (1u << shot_type)) != 0;
    }

    // The host records genuine clears independently of qualification and name entry.
    Status record_good_clear(unsigned shot_type) {
        return record_clear(shot_type, &Data::good_clears);
    }

    Status record_extra_clear(unsigned shot_type) {
        return record_clear(shot_type, &Data::extra_clears);
    }

    // A UI hint based on the last loaded table. Submit rechecks the current file.
    bool qualifies(const NativeScoreEntry& entry) const {
        return entry.stage != 0 && valid(entry, true) && qualifies(data_.boards[entry.difficulty], entry);
    }

    struct DisplayRow {
        const NativeScoreEntry* entry = nullptr;
        std::uint32_t points = 0; // Original displayed score, with continue digit.
        unsigned stage = 0;
        bool signing = false;
    };
    static constexpr std::uint32_t points(const NativeScoreEntry& entry) {
        return entry.score * std::uint32_t{10} + entry.continues_used;
    }
    static std::array<DisplayRow,places> display_rows(const std::vector<NativeScoreEntry>& stored,
                                                      const NativeScoreEntry* signing = nullptr) {
        std::array<DisplayRow,places> rows{};
        const auto retained = std::min(stored.size(), places - (signing ? 1 : 0));
        const auto defaults = places - retained - (signing ? 1 : 0);
        for(std::size_t i=0;i<defaults;++i)rows[i]={nullptr,default_points(i),5u-unsigned(i/2),false};
        for(std::size_t i=0;i<retained;++i)rows[defaults+i]={&stored[i],points(stored[i]),stored[i].stage,false};
        if(signing)rows.back()={signing,points(*signing),signing->stage,true};
        std::stable_sort(rows.begin(),rows.end(),[](const auto& a,const auto& b){
            if(a.points!=b.points)return ordered_points(a.points)>ordered_points(b.points);
            if(a.signing!=b.signing)return a.signing;
            return bool(a.entry)>bool(b.entry);
        });
        return rows;
    }

    // The host calls this only after confirmation, never on cancellation or practice.
    Status submit(NativeScoreEntry entry) {
        if (entry.name.empty()) entry.name = default_name;
        if (entry.stage == 0 || !valid(entry, false)) return Status::InvalidEntry;

        const File lock(open_writer_lock());
        if (!lock.ok()) return Status::IoError;

        Data next;
        const auto status = read_file(next);
        if (status != Status::Ok && status != Status::Missing) return status;
        auto& board = next.boards[entry.difficulty];
        if (!qualifies(board, entry)) {
            data_ = std::move(next);
            return Status::NotRanked;
        }
        const auto key = rank_key(entry);
        // Insert before an exact tie, including a tie at the last place.
        const auto at = std::find_if(board.begin(), board.end(), [key](const auto& old) {
            return rank_key(old) <= key;
        });
        board.insert(at, std::move(entry));
        if (board.size() > places) board.pop_back();
        if (!write_file(next)) return Status::IoError;
        data_ = std::move(next);
        return Status::Ok;
    }

private:
    using Board = std::vector<NativeScoreEntry>;
    struct Data {
        std::array<Board, difficulty_count> boards;
        unsigned good_clears = 0, extra_clears = 0;
    };
    static constexpr unsigned clear_mask = (1u << shot_type_count) - 1;
    static constexpr std::array<std::uint8_t, 8> magic{'S', 'O', 'E', 'W', 'N', 'S', 'C', 'R'};
    static constexpr std::uint8_t legacy_version = 1, ascii_version = 2, gaiji_version = 3;
    // Header: magic, version, entry count, per-shot good-clear bits, per-shot
    // Extra-clear bits (zero in v1). Record: rank, shot, continues, flags,
    // LE uint32 score, six NUL-padded ASCII bytes (or six MIKOFT indices if
    // v3 flag bit 1 is set), stage (v2+). Flag bit 0 means good clear in every
    // version. LE CRC32 follows the last record.
    static constexpr std::size_t header_size = 12, legacy_record_size = 8 + name_length;
    static constexpr std::size_t record_size = legacy_record_size + 1;
    static constexpr std::size_t max_records = difficulty_count * places;
    using Bytes = std::array<std::uint8_t, header_size + record_size * max_records + 4>;
    static inline std::atomic<unsigned> next_temp_{0};
    static inline const Board empty_{};
    std::filesystem::path path_;
    Data data_{};

    Status record_clear(unsigned shot_type, unsigned Data::*clears) {
        if (shot_type >= shot_type_count) return Status::InvalidEntry;
        const File lock(open_writer_lock());
        if (!lock.ok()) return Status::IoError;

        Data next;
        const auto status = read_file(next);
        if (status != Status::Ok && status != Status::Missing) return status;
        const auto bit = 1u << shot_type;
        if ((next.*clears & bit) == 0) {
            next.*clears |= bit;
            if (!write_file(next)) return Status::IoError;
        }
        data_ = std::move(next);
        return Status::Ok;
    }

    // Serialize cooperating writers before reading and publishing a new snapshot.
#ifdef __EMSCRIPTEN__
    // Each browser game owns its own in-memory filesystem and dispatches writes on one thread.
    struct File { bool ok() const { return true; } };
    File open_writer_lock() const { return {}; }
#else
    HANDLE open_writer_lock() const {
        auto lock_path = path_;
        lock_path += L".lock";
        return CreateFileW(lock_path.c_str(), DELETE, 0, nullptr, OPEN_ALWAYS,
            FILE_ATTRIBUTE_NORMAL | FILE_FLAG_DELETE_ON_CLOSE | FILE_FLAG_OPEN_REPARSE_POINT, nullptr);
    }

    struct File {
        HANDLE handle;
        explicit File(HANDLE handle) : handle(handle) {}
        File(const File&) = delete;
        File& operator=(const File&) = delete;
        ~File() { close(); }
        bool ok() const { return handle != INVALID_HANDLE_VALUE; }
        bool close() {
            return !ok() || CloseHandle(std::exchange(handle, INVALID_HANDLE_VALUE)) != 0;
        }
    };
#endif

    // ReC98 th02/hiscore/regist.cpp:257–274 compares the signed result of
    // score_t * 10 + continues_used. XOR biases its modulo-32 bits for unsigned ordering.
    static constexpr std::uint32_t ordered_points(std::uint32_t score) {return score ^ 0x80000000u;}
    static constexpr std::uint32_t default_points(std::size_t place) {return 10000u-1000u*unsigned(place);}
    static constexpr std::uint32_t rank_key(const NativeScoreEntry& entry) {return ordered_points(points(entry));}

    // The lowest remaining placeholder is displaced first. Retain all old
    // records, including legacy scores below the placeholder threshold, until
    // the saved board is full and a new entry would replace the last record.
    static bool qualifies(const Board& board, const NativeScoreEntry& entry) {
        const auto floor=board.size()<places?ordered_points(default_points(places-1-board.size())):
            std::max(ordered_points(default_points(places-1)),rank_key(board.back()));
        return rank_key(entry)>=floor;
    }

    static bool valid(const NativeScoreEntry& entry, bool allow_empty) {
        if (entry.difficulty >= difficulty_count || entry.shot_type >= shot_type_count || entry.continues_used > 3) {
            return false;
        }
        const bool is_extra = entry.difficulty == difficulty_count - 1;
        const bool is_all = entry.stage == NativeScoreEntry::all_stages;
        if ((is_extra && entry.continues_used != 0) ||
            (entry.good_clear && (is_extra || entry.continues_used != 0 || !is_all))) return false;
        if (entry.stage != 0 && !is_all &&
            (is_extra ? entry.stage != NativeScoreEntry::extra_stage : entry.stage >= NativeScoreEntry::extra_stage)) {
            return false;
        }
        if (entry.name.empty()) return allow_empty;
        if (entry.gaiji_name) return entry.name.size()==name_length &&
            std::all_of(entry.name.begin(),entry.name.end(),[](unsigned char c){return score_gaiji::name_glyph(c);});
        return entry.name.size() <= name_length && entry.name.find_first_not_of(' ') != std::string::npos &&
            std::all_of(entry.name.begin(), entry.name.end(), [](unsigned char c) {
                return c >= 0x20 && c <= 0x7e;
            });
    }

    static std::uint32_t get_u32(const std::uint8_t* bytes) {
        std::uint32_t value = 0;
        for (unsigned i = 0; i < 4; ++i) value |= std::uint32_t(bytes[i]) << (8 * i);
        return value;
    }

    static void set_u32(std::uint8_t* bytes, std::uint32_t value) {
        for (unsigned i = 0; i < 4; ++i) bytes[i] = static_cast<std::uint8_t>(value >> (8 * i));
    }

    static std::uint32_t crc32(const Bytes& bytes, std::size_t size) {
        std::uint32_t crc = 0xffffffffu;
        for (std::size_t i = 0; i < size; ++i) {
            crc ^= bytes[i];
            for (unsigned bit = 0; bit < 8; ++bit) {
                crc = (crc >> 1) ^ ((crc & 1) ? 0xedb88320u : 0u);
            }
        }
        return ~crc;
    }

    Status read_file(Data& out) const {
#ifdef __EMSCRIPTEN__
        std::error_code error;
        const auto size=std::filesystem::file_size(path_,error);
        if(error)return error==std::errc::no_such_file_or_directory?Status::Missing:Status::IoError;
        Bytes bytes{};
        if(size>bytes.size()||size<magic.size()+1)return Status::Invalid;
        std::ifstream input(path_,std::ios::binary);
        if(!input.read(reinterpret_cast<char*>(bytes.data()),size))return Status::IoError;
        if(!std::equal(magic.begin(),magic.end(),bytes.begin()))return Status::Invalid;
#else
        const File file(CreateFileW(path_.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_DELETE,
            nullptr, OPEN_EXISTING, FILE_FLAG_OPEN_REPARSE_POINT, nullptr));
        if (!file.ok()) return GetLastError() == ERROR_FILE_NOT_FOUND ? Status::Missing : Status::IoError;

        BY_HANDLE_FILE_INFORMATION info{};
        if (!GetFileInformationByHandle(file.handle, &info)) return Status::IoError;
        if ((info.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_REPARSE_POINT)) ||
            info.nNumberOfLinks != 1) return Status::Invalid;
        Bytes bytes{};
        if (info.nFileSizeHigh || info.nFileSizeLow > bytes.size() || info.nFileSizeLow < magic.size() + 1) {
            return Status::Invalid;
        }
        const auto size = info.nFileSizeLow;
        DWORD got = 0;
        if (!ReadFile(file.handle, bytes.data(), size, &got, nullptr)) return Status::IoError;
        if (got != size || !std::equal(magic.begin(), magic.end(), bytes.begin())) return Status::Invalid;
#endif
        const bool legacy = bytes[8] == legacy_version;
        const bool gaiji = bytes[8] == gaiji_version;
        if (!legacy && !gaiji && bytes[8] != ascii_version) return Status::UnsupportedVersion;
        const auto count = bytes[9];
        const auto stride = legacy ? legacy_record_size : record_size;
        if (count > max_records || (bytes[10] & ~clear_mask) != 0 ||
            (legacy ? bytes[11] != 0 : (bytes[11] & ~clear_mask) != 0) ||
            size != header_size + count * stride + 4 ||
            get_u32(bytes.data() + size - 4) != crc32(bytes, size - 4)) return Status::Invalid;
        out.good_clears = bytes[10];
        out.extra_clears = bytes[11];

        for (std::size_t i = 0; i < count; ++i) {
            const auto p = bytes.data() + header_size + i * stride;
            if (p[3] > (gaiji ? 3 : 1)) return Status::Invalid;
            const bool good_clear = (p[3] & 1) != 0;
            const auto stage = legacy ? (good_clear ? NativeScoreEntry::all_stages : 0u) : p[legacy_record_size];
            NativeScoreEntry entry{ {}, p[0], p[1], p[2], get_u32(p + 4), good_clear, stage, (p[3] & 2) != 0 };
            const auto name = reinterpret_cast<const char*>(p + 8);
            const auto end = std::find(name, name + name_length, '\0');
            if (std::any_of(end, name + name_length, [](char c) { return c != '\0'; })) return Status::Invalid;
            entry.name.assign(name, end);
            if (!valid(entry, false)) return Status::Invalid;
            auto& board = out.boards[entry.difficulty];
            if (board.size() == places || (!board.empty() && rank_key(board.back()) < rank_key(entry))) {
                return Status::Invalid;
            }
            board.push_back(std::move(entry));
        }
        return Status::Ok;
    }

    bool write_file(const Data& data) const {
        Bytes bytes{};
        std::copy(magic.begin(), magic.end(), bytes.begin());
        bytes[8] = ascii_version; // Do not upgrade an ASCII-only file just to record a clear.

        bytes[10] = static_cast<std::uint8_t>(data.good_clears);
        bytes[11] = static_cast<std::uint8_t>(data.extra_clears);
        std::size_t offset = header_size;
        for (const auto& board : data.boards) {
            for (const auto& entry : board) {
                const auto p = bytes.data() + offset;
                p[0] = static_cast<std::uint8_t>(entry.difficulty);
                p[1] = static_cast<std::uint8_t>(entry.shot_type);
                p[2] = static_cast<std::uint8_t>(entry.continues_used);
                if(entry.gaiji_name)bytes[8]=gaiji_version;
                p[3] = (entry.good_clear ? 1 : 0) | (entry.gaiji_name ? 2 : 0);
                set_u32(p + 4, entry.score);
                std::copy(entry.name.begin(), entry.name.end(), p + 8);
                p[legacy_record_size] = static_cast<std::uint8_t>(entry.stage);
                offset += record_size;
                ++bytes[9];
            }
        }
        set_u32(bytes.data() + offset, crc32(bytes, offset));
#ifdef __EMSCRIPTEN__
        auto temp=path_;
        temp+=".tmp";
        std::ofstream file(temp,std::ios::binary|std::ios::trunc);
        file.write(reinterpret_cast<const char*>(bytes.data()),offset+4);
        file.close();
        if(!file)return false;
        std::error_code error;
        std::filesystem::rename(temp,path_,error);
        return !error;
#else
        const auto size = static_cast<DWORD>(offset + 4);

        // Write beside the destination on the same volume; MoveFileExW never copies.
        for (unsigned attempt = 0; attempt < 16; ++attempt) {
            auto temp = path_;
            temp += L".tmp." + std::to_wstring(GetCurrentProcessId()) + L"." + std::to_wstring(next_temp_++);
            File file(CreateFileW(temp.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nullptr));
            if (!file.ok()) {
                const auto error = GetLastError();
                if (error == ERROR_FILE_EXISTS || error == ERROR_ALREADY_EXISTS) continue;
                return false;
            }
            DWORD written = 0;
            const bool flushed = WriteFile(file.handle, bytes.data(), size, &written, nullptr) &&
                written == size && FlushFileBuffers(file.handle);
            const bool closed = file.close();
            if (flushed && closed && MoveFileExW(temp.c_str(), path_.c_str(),
                MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) return true;
            DeleteFileW(temp.c_str());
            return false;
        }
        return false;
#endif
    }
};

} // namespace soew
