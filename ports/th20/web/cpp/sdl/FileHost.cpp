#include "../platform/Files.hpp"
#include <SDL3/SDL.h>
#include <emscripten.h>
#include <dirent.h>
#include <sys/stat.h>
#include <algorithm>
#include <cstring>
#include <map>
#include <set>

EM_JS(void, browser_save_changed, (), { Module['runtimeFileChanged']?.(); });

// Provided by AudioHost.cpp: the retail thbgm.dat byte space backed by OGG.
extern "C" SDL_IOStream* th20_music_stream();

namespace th20::web::files {
namespace {
std::string root = "/savesth20/jp";
std::map<std::uint32_t, SDL_IOStream*> handles;
std::map<std::uint32_t, std::string> names;
std::set<std::uint32_t> writers;
std::uint32_t next = 1;

std::string normalize(const char* value) {
    if (!value || !*value) return {};
    std::string out, part;
    const bool absolute = *value == '/' || *value == '\\';
    auto flush = [&]() {
        if (part == "..") return false;
        if (!part.empty() && part != ".") { if (!out.empty()) out += '/'; out += part; }
        part.clear();
        return true;
    };
    for (const auto* p = value; *p; ++p) {
        const char c = *p;
        if (c == ':') return {};
        if (c == '/' || c == '\\') { if (!flush()) return {}; }
        else part += c >= 'A' && c <= 'Z' ? char(c + 32) : c;
    }
    if (!flush()) return {};
    // Restore the leading separator consumed as an empty first part so the
    // root-prefix stripping below can recognize absolute paths.
    if (absolute) out.insert(out.begin(), '/');
    // Strip any leading root the caller embedded; routing is owned here.
    const std::string prefixes[]{"/savesth20/jp/", "/savesth20/chs/", "/game/", "/"};
    for (const auto& prefix : prefixes)
        if (out.rfind(prefix, 0) == 0) return out.substr(prefix.size());
    return out;
}
void parents(const std::string& path) {
    for (std::size_t i = 1; i < path.size(); ++i)
        if (path[i] == '/') mkdir(path.substr(0, i).c_str(), 0777);
}
bool match(const char* pattern, const char* text) {
    if (std::strcmp(pattern, "*.*") == 0) pattern = "*";
    const char* star = nullptr;
    const char* retry = nullptr;
    while (*text) {
        if (*pattern == '?' || *pattern == *text) { ++pattern; ++text; }
        else if (*pattern == '*') { star = pattern++; retry = text; }
        else if (star) { pattern = star + 1; text = ++retry; }
        else return false;
    }
    while (*pattern == '*') ++pattern;
    return !*pattern;
}
bool glob_match(const std::string& pattern, const std::string& name) {
    return match(pattern.c_str(), name.c_str());
}
}

void set_language(const char* language) {
    root = language && std::strcmp(language, "chs") == 0 ? "/savesth20/chs" : "/savesth20/jp";
    parents(root + "/replay/");
    parents(root + "/snapshot/");
}
const std::string& save_root() { return root; }

bool write_allowed(const std::string& name) {
    if (name == "th20.cfg" || name == "scoreth20.dat" || name == "scoreth20bak.dat" || name == "log.txt") return true;
    if (name.rfind("replay/", 0) == 0 && name.size() > 7) {
        const auto leaf = name.substr(7);
        return leaf.size() >= 4 && leaf.compare(leaf.size() - 4, 4, ".rpy") == 0;
    }
    if (name.rfind("snapshot/", 0) == 0 && name.size() > 9) {
        const auto leaf = name.substr(9);
        return leaf.size() >= 4 && leaf.compare(leaf.size() - 4, 4, ".png") == 0;
    }
    return false;
}

std::uint32_t open(const char* cp932_path, bool write) {
    const auto name = normalize(cp932_path);
    if (name.empty()) return 0;
    SDL_IOStream* io = nullptr;
    if (write) {
        if (!write_allowed(name)) return 0;
        const auto path = root + "/" + name;
        parents(path);
        io = SDL_IOFromFile(path.c_str(), "wb");
    } else if (name == "thbgm.dat") {
        // The canonical BGM source is the /bgm-ogg OGG set, not a retail
        // thbgm.dat on disk; mirror the TH07/TH08/TH10 web file hosts.
        io = th20_music_stream();
    } else {
        io = SDL_IOFromFile((root + "/" + name).c_str(), "rb");
        if (!io) io = SDL_IOFromFile(("/game/" + name).c_str(), "rb");
    }
    if (!io) return 0;
    const auto id = next++;
    handles[id] = io;
    names[id] = name;
    if (write) writers.insert(id);
    return id;
}

void close(std::uint32_t handle) {
    auto it = handles.find(handle);
    if (it == handles.end()) return;
    SDL_CloseIO(it->second);
    handles.erase(it);
    names.erase(handle);
    if (writers.erase(handle)) browser_save_changed();
}
std::uint32_t size(std::uint32_t handle) {
    auto it = handles.find(handle);
    return it == handles.end() ? ~0u : std::uint32_t(SDL_GetIOSize(it->second));
}
std::uint32_t seek(std::uint32_t handle, std::int32_t offset, std::uint32_t origin) {
    auto it = handles.find(handle);
    if (it == handles.end() || origin > 2) return ~0u;
    return std::uint32_t(SDL_SeekIO(it->second, offset, static_cast<SDL_IOWhence>(origin)));
}
std::uint32_t read(std::uint32_t handle, void* out, std::uint32_t bytes) {
    auto it = handles.find(handle);
    return it == handles.end() ? 0 : std::uint32_t(SDL_ReadIO(it->second, out, bytes));
}
std::uint32_t write(std::uint32_t handle, const void* data, std::uint32_t bytes) {
    auto it = handles.find(handle);
    return it == handles.end() ? 0 : std::uint32_t(SDL_WriteIO(it->second, data, bytes));
}
std::uint32_t tell(std::uint32_t handle) { return seek(handle, 0, 1); }

std::optional<std::vector<std::uint8_t>> read_all(const char* cp932_path) {
    const auto handle = open(cp932_path, false);
    if (!handle) return std::nullopt;
    const auto length = size(handle);
    if (length == ~0u || length > 0xffffffe0u) { close(handle); return std::nullopt; }
    std::vector<std::uint8_t> out(length);
    const auto got = read(handle, out.data(), length);
    close(handle);
    out.resize(got);
    return out;
}

bool write_all(const char* cp932_path, const void* data, std::uint32_t bytes) {
    const auto handle = open(cp932_path, true);
    if (!handle) return false;
    const auto done = write(handle, data, bytes);
    close(handle);
    return done == bytes;
}

bool exists(const char* cp932_path) {
    const auto handle = open(cp932_path, false);
    if (!handle) return false;
    close(handle);
    return true;
}
std::uint32_t list(const char* directory, const char* pattern, std::uint32_t index,
                   char* out, std::uint32_t capacity) {
    const auto dir = normalize(directory);
    std::vector<std::string> names_found;
    for (const auto& base : {root, std::string("/game")}) {
        const auto full = base + "/" + dir;
        if (auto* d = opendir(full.c_str())) {
            while (auto* e = readdir(d)) {
                const std::string name = e->d_name;
                if (name == "." || name == ".." || !match(pattern, name.c_str())) continue;
                struct stat st {};
                if (!stat((full + "/" + name).c_str(), &st) && S_ISREG(st.st_mode)) names_found.push_back(name);
            }
            closedir(d);
        }
    }
    std::sort(names_found.begin(), names_found.end());
    names_found.erase(std::unique(names_found.begin(), names_found.end()), names_found.end());
    if (index >= names_found.size() || names_found[index].size() + 1 > capacity) return 0;
    std::memcpy(out, names_found[index].c_str(), names_found[index].size() + 1);
    return 1;
}
}
