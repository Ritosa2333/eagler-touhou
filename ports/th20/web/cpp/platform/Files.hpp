// File routing for the TH_SDL3 (wasm) build, replacing the Win32 file API
// (CreateFileW/ReadFile/WriteFile/GetFileSize/SetFilePointer/CloseHandle/
// FindFirstFileW). Reads resolve against the save root first, then the
// launcher-injected /game tree (th20.dat/thbgm.dat arrive as MEMFS files).
// Writes are limited to the save-root whitelist and notify the JS side
// (runtimeFileChanged) so IDBFS can sync. Handles are small integer ids.
#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace th20::web::files {

// Language root selector (mirrors the launcher language). Call once at
// session open; affects the save root (/savesth20/<lang>).
void set_language(const char* language);
const std::string& save_root();

// Win32 CreateFileW replacement. write=false: save root, then /game.
// write=true: save-root whitelist only. Returns 0 on failure.
std::uint32_t open(const char* cp932_path, bool write);
void close(std::uint32_t handle);
std::uint32_t size(std::uint32_t handle);
std::uint32_t seek(std::uint32_t handle, std::int32_t offset, std::uint32_t origin);
std::uint32_t read(std::uint32_t handle, void* out, std::uint32_t bytes);
std::uint32_t write(std::uint32_t handle, const void* data, std::uint32_t bytes);
std::uint32_t tell(std::uint32_t handle);

// Whole-file helpers used by the game services.
std::optional<std::vector<std::uint8_t>> read_all(const char* cp932_path);
// Existence check with the same read routing as open(save root, then /game).
bool exists(const char* cp932_path);
// Whitelisted write; returns false for disallowed paths or IO failure.
bool write_all(const char* cp932_path, const void* data, std::uint32_t bytes);

// FindFirstFileW/FindNextFileW shape: enumerate regular files in directory
// (save root and /game merged, sorted, deduplicated) matching a ?/* pattern.
// Returns the count; copies match #index into out when it fits.
std::uint32_t list(const char* directory, const char* pattern, std::uint32_t index,
                   char* out, std::uint32_t capacity);

// Write whitelist check (th20.cfg, scoreth20.dat, replay/*.rpy,
// snapshot/*.png, log.txt), exposed for auditing.
bool write_allowed(const std::string& normalized_relative_path);
}
