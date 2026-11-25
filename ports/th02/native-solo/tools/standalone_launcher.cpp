#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define _WIN32_WINNT 0x0A00
#include <windows.h>
#include <bcrypt.h>
#include <compressapi.h>
#include <shlobj.h>
#include <shellapi.h>
#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <set>
#include <string>
#include <vector>

namespace {
namespace fs = std::filesystem;
using Bytes = std::vector<unsigned char>;
using Digest = std::array<unsigned char, 32>;
constexpr DWORD bad = FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_REPARSE_POINT;
constexpr unsigned maxFile = 64 * 1024 * 1024;
constexpr size_t footerSize = 56;
struct Error { std::wstring message; };
[[noreturn]] void fail(std::wstring message) { throw Error{std::move(message)}; }
void check(bool condition, const std::wstring& operation) {
    if (!condition) {
        const auto error = GetLastError();
        fail(operation + L" (Windows error " + std::to_wstring(error) + L").");
    }
}
struct Handle {
    HANDLE value;
    explicit Handle(HANDLE h) : value(h) {}
    ~Handle() { if (value && value != INVALID_HANDLE_VALUE) CloseHandle(value); }
    Handle(const Handle&) = delete;
    Handle& operator=(const Handle&) = delete;
    bool valid() const { return value && value != INVALID_HANDLE_VALUE; }
};
struct Sha256 {
    BCRYPT_ALG_HANDLE provider = nullptr;
    BCRYPT_HASH_HANDLE hash = nullptr;
    Sha256() {
        if (BCryptOpenAlgorithmProvider(&provider, BCRYPT_SHA256_ALGORITHM, nullptr, 0) < 0)
            fail(L"Cannot open the Windows SHA-256 provider.");
        if (BCryptCreateHash(provider, &hash, nullptr, 0, nullptr, 0, 0) < 0) {
            BCryptCloseAlgorithmProvider(provider, 0);
            fail(L"Cannot initialize SHA-256.");
        }
    }
    ~Sha256() { BCryptDestroyHash(hash); BCryptCloseAlgorithmProvider(provider, 0); }
    void add(const unsigned char* data, unsigned size) {
        if (BCryptHashData(hash, const_cast<unsigned char*>(data), size, 0) < 0)
            fail(L"Cannot compute SHA-256.");
    }
    Digest finish() {
        Digest result{};
        if (BCryptFinishHash(hash, result.data(), result.size(), 0) < 0)
            fail(L"Cannot finish SHA-256.");
        return result;
    }
};
Digest sha(const Bytes& data) { Sha256 hash; hash.add(data.data(), static_cast<unsigned>(data.size())); return hash.finish(); }
std::wstring hex(const Digest& digits) {
    constexpr wchar_t alphabet[] = L"0123456789abcdef";
    std::wstring out;
    for (auto byte : digits) { out += alphabet[byte >> 4]; out += alphabet[byte & 15]; }
    return out;
}
HANDLE open_regular(const fs::path& path) {
    return CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING,
                       FILE_FLAG_OPEN_REPARSE_POINT | FILE_FLAG_SEQUENTIAL_SCAN, nullptr);
}
std::uint64_t file_size(HANDLE file) {
    LARGE_INTEGER out{};
    check(GetFileSizeEx(file, &out), L"Cannot get file size");
    return static_cast<std::uint64_t>(out.QuadPart);
}
void seek(HANDLE file, std::uint64_t offset) {
    LARGE_INTEGER out{}; out.QuadPart = static_cast<LONGLONG>(offset);
    check(SetFilePointerEx(file, out, nullptr, FILE_BEGIN), L"Cannot seek in the standalone EXE");
}
void read_bytes(HANDLE file, unsigned char* bytes, unsigned size) {
    DWORD got = 0;
    check(ReadFile(file, bytes, size, &got, nullptr) && got == size, L"Cannot read the expected file bytes");
}
Digest hash_file(HANDLE file, std::uint64_t size) {
    Sha256 hash;
    Bytes buffer(256 * 1024);
    while (size) {
        const auto count = static_cast<unsigned>(std::min<std::uint64_t>(size, buffer.size()));
        read_bytes(file, buffer.data(), count);
        hash.add(buffer.data(), count);
        size -= count;
    }
    return hash.finish();
}
struct Reader {
    const Bytes& data;
    size_t offset = 0;
    const unsigned char* take(size_t count) {
        if (count > data.size() - offset) fail(L"Truncated standalone package metadata.");
        auto ptr = data.data() + offset;
        offset += count;
        return ptr;
    }
    template<typename T> T get() { T out{}; std::memcpy(&out, take(sizeof(out)), sizeof(out)); return out; }
    void magic(const char* value) { if (std::memcmp(take(8), value, 8)) fail(L"Unsupported standalone package format."); }
};
struct Entry {
    std::wstring directory, name;
    std::uint64_t offset;
    std::uint32_t unpacked, stored;
    Digest hash;
    fs::path path(const fs::path& root) const { return root / directory / name; }
};
struct Package { unsigned mode; Digest id; std::vector<Entry> entries; bool localized = false; };
constexpr std::array<const char*, 10> localeText = {"STAGE0.TXT", "STAGE1.TXT", "STAGE2.TXT", "STAGE3.TXT",
    "STAGE4.TXT", "STAGE5.TXT", "END1.TXT", "END2.TXT", "END3.TXT", "MUSIC.TXT"};
constexpr size_t localeCount = 2 * localeText.size() + 3;
constexpr const char* chinesePack = "chinese-v1.txt";
constexpr unsigned maxChinesePack = 256 * 1024;
bool locale_entry(const std::string& directory, const std::string& name) {
    if (directory == "content/locale") return name == "manifest.json" || name == chinesePack;
    if (directory == "content/locale/common") return name == "OP2.PI" || name == "OP.RGB";
    if (directory == "content/locale/ja" || directory == "content/locale/en")
        return std::find(localeText.begin(), localeText.end(), name) != localeText.end();
    return false;
}
std::wstring filename(const std::string& name) {
    if (name.empty() || name.size() > 100 || name.front() == '.' || name.back() == '.' || name.find("..") != std::string::npos)
        fail(L"Invalid name in standalone package.");
    std::wstring result;
    std::string folded;
    for (unsigned char c : name) {
        if (!(c == '.' || c == '-' || c == '_' || (c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')))
            fail(L"Invalid character in standalone package path.");
        result += static_cast<wchar_t>(c);
        folded += static_cast<char>((c >= 'a' && c <= 'z') ? c - 'a' + 'A' : c);
    }
    folded.resize(folded.find('.') == std::string::npos ? folded.size() : folded.find('.'));
    if (folded == "CON" || folded == "PRN" || folded == "AUX" || folded == "NUL" ||
        (folded.size() == 4 && (folded.substr(0, 3) == "COM" || folded.substr(0, 3) == "LPT") && folded.back() >= '0' && folded.back() <= '9'))
        fail(L"Reserved Windows device name in standalone package.");
    return result;
}
Package load_package(HANDLE file) {
    const auto size = file_size(file);
    if (size < footerSize || size > 128 * 1024 * 1024) fail(L"Unexpected standalone EXE size.");
    seek(file, size - footerSize);
    Bytes footer(footerSize); read_bytes(file, footer.data(), footer.size());
    Reader last{footer}; last.magic("SOEWEX01");
    const auto start = last.get<std::uint64_t>();
    const auto manifestSize = last.get<std::uint32_t>();
    if (last.get<std::uint32_t>() != COMPRESS_ALGORITHM_LZMS || manifestSize < 16 || manifestSize > 256 * 1024 || manifestSize > size - footerSize)
        fail(L"Invalid standalone overlay metadata.");
    const auto id = last.get<Digest>();
    const auto manifestStart = size - footerSize - manifestSize;
    if (start < 4096 || start >= manifestStart) fail(L"Invalid standalone payload bounds.");
    seek(file, start);
    if (hash_file(file, size - footerSize - start) != id) fail(L"Standalone overlay SHA-256 mismatch.");
    Bytes manifest(manifestSize); seek(file, manifestStart); read_bytes(file, manifest.data(), manifest.size());
    Reader reader{manifest}; reader.magic("SOEWMF01");
    Package package{reader.get<std::uint32_t>(), id, {}};
    const auto count = reader.get<std::uint32_t>();
    if (package.mode > 3 || !count || count > 1024) fail(L"Invalid standalone package manifest.");
    std::uint64_t offset = start;
    std::set<std::wstring> seen;
    bool app = false;
    size_t localized = 0;
    bool chinese = false;
    for (unsigned i = 0; i < count; ++i) {
        const auto length = reader.get<std::uint16_t>();
        const auto unpacked = reader.get<std::uint32_t>();
        const auto stored = reader.get<std::uint32_t>();
        const auto hash = reader.get<Digest>();
        const auto nameBytes = reader.take(length);
        const std::string relative(reinterpret_cast<const char*>(nameBytes), length);
        const auto separator = relative.rfind('/');
        if (separator == std::string::npos || !unpacked || unpacked > maxFile || !stored || stored > unpacked || stored > manifestStart - offset)
            fail(L"Invalid standalone package entry.");
        const auto directory = relative.substr(0, separator);
        const auto name = relative.substr(separator + 1);
        const bool isLocale = locale_entry(directory, name);
        if (!isLocale && directory != "app" && directory != "content" && directory != "licenses")
            fail(L"Invalid standalone package entry.");
        Entry entry{std::wstring(directory.begin(), directory.end()), filename(name), offset, unpacked, stored, hash};
        if (isLocale) {
            if (directory == "content/locale" && name == chinesePack) {
                if (unpacked > maxChinesePack) fail(L"Invalid Chinese sidecar size in standalone package.");
                chinese = true;
            } else ++localized;
        }
        auto folded = entry.directory + L"/" + entry.name;
        for (auto& c : folded) if (c >= 'A' && c <= 'Z') c += 'a' - 'A';
        if (folded == L"content/locale" || !seen.insert(folded).second) fail(L"Conflicting standalone package entry.");
        if (directory == "app") {
            if (entry.name != L"SoEW-Native.exe") fail(L"Unexpected native application file.");
            app = true;
        }
        package.entries.emplace_back(std::move(entry));
        offset += stored;
    }
    if (!app || ((localized != 0 || chinese) && localized != localeCount) ||
        offset != manifestStart || reader.offset != manifest.size())
        fail(L"Incomplete standalone package manifest.");
    package.localized = localized != 0;
    return package;
}

constexpr const wchar_t* directories[] = {L"app", L"content", L"licenses"};
constexpr const wchar_t* localeDirectories[] = {L"content/locale", L"content/locale/common", L"content/locale/en", L"content/locale/ja"};
bool safe_dir(const fs::path& path) {
    const auto attr = GetFileAttributesW(path.c_str());
    return attr != INVALID_FILE_ATTRIBUTES && (attr & bad) == FILE_ATTRIBUTE_DIRECTORY;
}
void ensure_directory(const fs::path& path) {
    if (safe_dir(path)) return;
    if (!CreateDirectoryW(path.c_str(), nullptr) && GetLastError() != ERROR_ALREADY_EXISTS)
        check(false, L"Cannot create " + path.wstring());
    if (!safe_dir(path)) fail(L"Cannot use linked or non-directory path: " + path.wstring());
}
void ensure_tree(const fs::path& path) {
    if (!path.is_absolute() || path.root_name().native().size() != 2 || path.root_name().native().back() != L':')
        fail(L"SoEW data must use an absolute local drive path.");
    auto current = path.root_path();
    for (const auto& part : path.relative_path()) {
        if (part.empty()) continue;
        if (part == L".." || part == L".") fail(L"Invalid data path component.");
        current /= part; ensure_directory(current);
    }
}
fs::path default_root(const fs::path& executable) {
    const auto workspace = executable.root_path() / L"WorkBuddy AI";
    if (safe_dir(workspace)) return workspace / L"_game-profiles" / L"SoEW";
    // A redistributed copy may have no WorkBuddy workspace. Keep its profile
    // on the executable's drive and outside the installation directory.
    return executable.root_path() / L"TouhouReverse-UserData" / L"SoEW";
}
bool verify(const fs::path& root, const Package& package) {
    try {
        if (!safe_dir(root)) return false;
        unsigned children = 0;
        for (const auto& child : fs::directory_iterator(root)) {
            if (!safe_dir(child.path()) || std::none_of(std::begin(directories), std::end(directories),
                [&](const wchar_t* name) { return child.path().filename() == name; })) return false;
            ++children;
        }
        if (children != std::size(directories)) return false;
        size_t files = 0;
        const auto count_files = [&](const wchar_t* name) {
            if (!safe_dir(root / name)) return false;
            for (const auto& child : fs::directory_iterator(root / name)) {
                const auto relative = fs::path(name) / child.path().filename();
                if (package.localized && std::find_if(std::begin(localeDirectories), std::end(localeDirectories),
                    [&](const wchar_t* locale) { return relative == fs::path(locale); }) != std::end(localeDirectories)) {
                    if (!safe_dir(child.path())) return false;
                } else {
                    const auto attributes = GetFileAttributesW(child.path().c_str());
                    if (attributes == INVALID_FILE_ATTRIBUTES || (attributes & bad)) return false;
                    ++files;
                }
            }
            return true;
        };
        for (const auto* name : directories) if (!count_files(name)) return false;
        if (package.localized) for (const auto* name : localeDirectories) if (!count_files(name)) return false;
        if (files != package.entries.size()) return false;
        for (const auto& entry : package.entries) {
            Handle file(open_regular(entry.path(root)));
            BY_HANDLE_FILE_INFORMATION info{};
            if (!file.valid() || !GetFileInformationByHandle(file.value, &info) || (info.dwFileAttributes & bad) ||
                info.nNumberOfLinks != 1 || file_size(file.value) != entry.unpacked || hash_file(file.value, entry.unpacked) != entry.hash) return false;
        }
        return true;
    } catch (const Error&) { return false; }
      catch (const fs::filesystem_error&) { return false; }
}
struct Decompressor {
    DECOMPRESSOR_HANDLE value = nullptr;
    Decompressor() { check(CreateDecompressor(COMPRESS_ALGORITHM_LZMS, nullptr, &value), L"Cannot initialize the Windows decompressor"); }
    ~Decompressor() { CloseDecompressor(value); }
};
void extract(HANDLE source, const Package& package, const fs::path& staging) {
    for (const auto* name : directories) ensure_directory(staging / name);
    if (package.localized) for (const auto* name : localeDirectories) ensure_directory(staging / name);
    Decompressor decoder;
    for (const auto& entry : package.entries) {
        Bytes packed(entry.stored);
        seek(source, entry.offset); read_bytes(source, packed.data(), entry.stored);
        Bytes data;
        if (entry.stored != entry.unpacked) {
            data.resize(entry.unpacked);
            SIZE_T written = 0;
            check(Decompress(decoder.value, packed.data(), packed.size(), data.data(), data.size(), &written) && written == data.size(),
                  L"Cannot decompress " + entry.name);
        } else { data = std::move(packed); }
        if (sha(data) != entry.hash) fail(L"Standalone file SHA-256 mismatch: " + entry.name);
        const auto path = entry.path(staging);
        Handle file(CreateFileW(path.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nullptr));
        check(file.valid(), L"Cannot extract " + path.wstring());
        DWORD wrote = 0;
        check(WriteFile(file.value, data.data(), entry.unpacked, &wrote, nullptr) && wrote == entry.unpacked,
              L"Cannot finish writing " + path.wstring());
    }
}

struct Cache { fs::path root; bool reused; };
Cache resolve_cache(HANDLE source, const Package& package, const fs::path& cache) {
    ensure_directory(cache);
    const auto key = hex(package.id);
    struct Stage {
        fs::path path;
        ~Stage() { if (!path.empty()) { std::error_code ignored; fs::remove_all(path, ignored); } }
    } staging;
    for (unsigned slot = 0; slot < 32; ++slot) {
        auto root = cache / (slot ? key + L"-" + std::to_wstring(slot) : key);
        auto attributes = GetFileAttributesW(root.c_str());
        if (attributes != INVALID_FILE_ATTRIBUTES) {
            if (verify(root, package)) return {std::move(root), true};
            // Preserve the damaged or foreign path. A later slot can safely repair the launch.
            continue;
        }
        check(GetLastError() == ERROR_FILE_NOT_FOUND, L"Cannot inspect the SoEW cache path");
        if (staging.path.empty()) {
            GUID id{}; wchar_t name[39]{};
            if (FAILED(CoCreateGuid(&id)) || StringFromGUID2(id, name, std::size(name)) != 39)
                fail(L"Cannot allocate a unique SoEW staging path.");
            auto temporary = cache / (std::wstring(L".stage-") + name);
            check(CreateDirectoryW(temporary.c_str(), nullptr), L"Cannot create SoEW extraction staging");
            staging.path = std::move(temporary);
            extract(source, package, staging.path);
            if (!verify(staging.path, package)) fail(L"Newly extracted SoEW files did not verify.");
        }
        if (MoveFileExW(staging.path.c_str(), root.c_str(), MOVEFILE_WRITE_THROUGH)) {
            staging.path.clear();
            return {std::move(root), false};
        }
        const auto error = GetLastError();
        if (GetFileAttributesW(root.c_str()) == INVALID_FILE_ATTRIBUTES) {
            SetLastError(error); check(false, L"Cannot publish verified SoEW cache");
        }
        if (verify(root, package)) return {std::move(root), true};
    }
    fail(L"SoEW cache has too many damaged paths. Move the damaged cache folders aside and retry.");
}
std::wstring quote(const std::wstring& value) {
    std::wstring out = L"\"";
    size_t backslashes = 0;
    for (auto c : value) {
        if (c == '\\') { ++backslashes; continue; }
        out.append(c == '"' ? backslashes * 2 + 1 : backslashes, L'\\');
        out += c; backslashes = 0;
    }
    return out + std::wstring(backslashes * 2, L'\\') + L'"';
}
void output(DWORD stream, const std::wstring& message) {
    HANDLE h = GetStdHandle(stream);
    if (!h || h == INVALID_HANDLE_VALUE) return;
    const auto count = WideCharToMultiByte(CP_UTF8, 0, message.data(), static_cast<int>(message.size()), nullptr, 0, nullptr, nullptr);
    if (!count) return;
    std::string bytes(count, '\0');
    WideCharToMultiByte(CP_UTF8, 0, message.data(), static_cast<int>(message.size()), bytes.data(), count, nullptr, nullptr);
    DWORD written = 0; WriteFile(h, bytes.data(), bytes.size(), &written, nullptr);
}
bool is_redirected(HANDLE handle) {
    const auto type = GetFileType(handle);
    return type == FILE_TYPE_PIPE || type == FILE_TYPE_DISK;
}
HANDLE inheritable_copy(HANDLE handle) {
    if (!is_redirected(handle)) return nullptr;
    HANDLE copy = nullptr;
    check(DuplicateHandle(GetCurrentProcess(), handle, GetCurrentProcess(), &copy, 0, TRUE, DUPLICATE_SAME_ACCESS),
          L"Cannot forward redirected native output");
    return copy;
}
struct Arguments { std::vector<std::wstring> child; fs::path root; bool check = false; bool diagnostic = false; };
Arguments parse_arguments() {
    int count = 0;
    auto argv = CommandLineToArgvW(GetCommandLineW(), &count);
    check(argv != nullptr, L"Cannot read standalone launch arguments");
    struct Free { LPWSTR* p; ~Free() { LocalFree(p); } } free{argv};
    Arguments args;
    for (int i = 1; i < count; ++i) {
        const std::wstring flag = argv[i];
        if (flag == L"--package-check") { args.check = true; continue; }
        if (flag == L"--package-user-root") {
            if (++i >= count || !args.root.empty() || !fs::path(argv[i]).is_absolute())
                fail(L"--package-user-root requires one absolute local drive path.");
            args.root = fs::path(argv[i]).lexically_normal(); continue;
        }
        if (flag == L"--assets" || flag == L"--settings-dir") fail(L"The standalone EXE manages assets and settings. Use --package-user-root to select another profile.");
        args.child.push_back(flag);
        if (flag == L"--dump" || flag == L"--smoke-evidence") {
            if (++i >= count || !*argv[i]) fail(flag + L" needs a path.");
            args.child.push_back(fs::absolute(argv[i]).lexically_normal().wstring());
        }
    }
    if (args.check && !args.child.empty()) fail(L"--package-check cannot be combined with game arguments.");
    args.diagnostic = args.check || std::any_of(args.child.begin(), args.child.end(), [](const auto& arg) {
        return arg == L"--self-test" || arg == L"--settings-probe" || arg == L"--smoke-test" || arg == L"--dump" ||
               arg == L"--timed-test" || arg == L"--score-timed-test" || arg == L"--async-practice-test" || arg == L"--async-exit-test" ||
               arg == L"--retry-benchmark";
    });
    return args;
}
int run(const Arguments& args) {
    std::wstring self(32768, L'\0');
    const auto length = GetModuleFileNameW(nullptr, self.data(), self.size());
    check(length && length < self.size(), L"Cannot locate the standalone EXE");
    self.resize(length);
    // A copied or moved launcher may itself be reached through a symlink; cache children may not.
    Handle source(CreateFileW(self.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr));
    check(source.valid(), L"Cannot open the standalone EXE");
    const auto package = load_package(source.value);
    const auto root = args.root.empty() ? default_root(fs::path(self)) : args.root;
    ensure_tree(root);
    const auto cache = resolve_cache(source.value, package, root / L"cache-v1");
    if (args.diagnostic) {
        output(STD_OUTPUT_HANDLE, L"package-cache=" + std::wstring(cache.reused ? L"hit" : L"extracted") +
               L" package-sha256=" + hex(package.id) + L"\r\ncache-root=" + cache.root.wstring() + L"\r\n");
        if (args.check) return 0;
    }
    // Portable installations keep the existing save/rep pair next to the
    // launcher, but verified extracted assets stay outside the game directory.
    const bool portable_profile = package.mode == 3 && args.root.empty();
    const auto user = portable_profile ? fs::path(self).parent_path() / L"save" : root / L"user";
    if (portable_profile) {
        ensure_tree(user);
        ensure_directory(user.parent_path() / L"rep");
    } else ensure_directory(user);
    const auto appDir = cache.root / L"app";
    const auto app = appDir / L"SoEW-Native.exe";
    auto command = quote(app.wstring()) + L" --assets " + quote((cache.root / L"content").wstring()) + L" --settings-dir " + quote(user.wstring());
    if (std::all_of(args.child.begin(), args.child.end(), [](const auto& arg) { return arg == L"--skip-opening"; })) {
        if (package.mode == 1) command += L" --stage5-mainline";
        if (package.mode == 2 || package.mode == 3) command += L" --extra-candidate";
    }
    for (const auto& arg : args.child) command += L" " + quote(arg);
    // Keep the child search path free of the caller's working directory.
    check(SetDllDirectoryW(L""), L"Cannot initialize safe native DLL lookup");
    STARTUPINFOW startup{}; startup.cb = sizeof(startup);
    SECURITY_ATTRIBUTES security{sizeof(security), nullptr, TRUE};
    Handle out(inheritable_copy(GetStdHandle(STD_OUTPUT_HANDLE))), err(inheritable_copy(GetStdHandle(STD_ERROR_HANDLE)));
    const bool redirected = out.valid() || err.valid();
    Handle nullDevice(redirected ? CreateFileW(L"NUL", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
                                                &security, OPEN_EXISTING, 0, nullptr) : nullptr);
    if (redirected) {
        check(nullDevice.valid(), L"Cannot initialize redirected native streams");
        startup.dwFlags = STARTF_USESTDHANDLES;
        startup.hStdInput = nullDevice.value;
        startup.hStdOutput = out.valid() ? out.value : nullDevice.value;
        startup.hStdError = err.valid() ? err.value : nullDevice.value;
    }
    PROCESS_INFORMATION process{};
    check(CreateProcessW(app.c_str(), command.data(), nullptr, nullptr, redirected, CREATE_NO_WINDOW,
                         nullptr, appDir.c_str(), &startup, &process), L"Cannot launch SoEW");
    Handle child(process.hProcess), thread(process.hThread);
    check(WaitForSingleObject(child.value, INFINITE) == WAIT_OBJECT_0, L"Cannot wait for SoEW");
    DWORD code = 1;
    check(GetExitCodeProcess(child.value, &code), L"Cannot read SoEW exit code");
    if (code && !args.diagnostic) {
        const auto message = L"SoEW exited with code " + std::to_wstring(code) + L".";
        output(STD_ERROR_HANDLE, message + L"\r\n");
        if (!is_redirected(GetStdHandle(STD_ERROR_HANDLE)))
            MessageBoxW(nullptr, message.c_str(), L"SoEW Standalone", MB_OK | MB_ICONERROR);
    }
    return static_cast<int>(code);
}
} // namespace

int WINAPI wWinMain(HINSTANCE, HINSTANCE, PWSTR, int) {
    bool diagnostic = false;
    std::wstring message;
    try { const auto args = parse_arguments(); diagnostic = args.diagnostic; return run(args); }
    catch (const Error& e) { message = e.message; }
    catch (const std::exception&) { message = L"Cannot open or prepare the standalone SoEW files."; }
    output(STD_ERROR_HANDLE, message + L"\r\n");
    if (!diagnostic && !is_redirected(GetStdHandle(STD_ERROR_HANDLE)))
        MessageBoxW(nullptr, message.c_str(), L"SoEW Standalone", MB_OK | MB_ICONERROR);
    return 1;
}
