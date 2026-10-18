#include "archive.hpp"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>

using namespace th20::source;
namespace {
Bytes read_reference(const std::filesystem::path& path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file) throw std::runtime_error("Missing reference extraction: " + path.string());
    const auto size = file.tellg();
    if (size < 0) throw std::runtime_error("Cannot get reference size");
    Bytes bytes(static_cast<std::size_t>(size));
    file.seekg(0);
    if (!bytes.empty() && !file.read(reinterpret_cast<char*>(bytes.data()), size))
        throw std::runtime_error("Cannot read reference extraction");
    return bytes;
}
std::string quoted(const std::string& s) {
    std::ostringstream out;
    out << '"';
    for (const auto byte : s) {
        const auto c = static_cast<unsigned char>(byte);
        if (c == '\\' || c == '"') out << '\\' << c;
        else if (c < 0x20 || c > 0x7e) out << "\\u00" << std::hex << std::setw(2) << std::setfill('0') << unsigned(c);
        else out << c;
    }
    out << '"';
    return out.str();
}
}
int wmain(int argc, wchar_t** argv) {
    try {
        if (argc != 4) throw std::runtime_error("Usage: th20_source_archive_verify TH20.dat EXTRACTED_RAW OUTPUT.json");
        const std::filesystem::path archive_path(argv[1]), raw(argv[2]), report_path(argv[3]);
        if (std::filesystem::weakly_canonical(archive_path) == std::filesystem::weakly_canonical(report_path))
            throw std::runtime_error("Report cannot overwrite archive");
        Archive archive(archive_path);
        if (archive.entries().empty()) throw std::runtime_error("This TH20 reference verifier requires a nonempty archive");
        std::map<std::string, unsigned> counts;
        for (const auto& e : archive.entries()) ++counts[e.name];
        std::uint64_t total = 0;
        std::ostringstream entries;
        for (std::size_t i = 0; i < archive.entries().size(); ++i) {
            const auto& e = archive.entries()[i];
            const std::filesystem::path relative(e.name);
            if (relative.is_absolute() || relative.has_root_name()) throw std::runtime_error("Unsafe archive name");
            for (const auto& component : relative)
                if (component == "..") throw std::runtime_error("Unsafe archive parent path");
            auto reference = raw / relative;
            if (counts[e.name] > 1) {
                std::ostringstream duplicate;
                duplicate << std::setw(4) << std::setfill('0') << i << '_' << e.name;
                reference = raw.parent_path() / "duplicate_entries" / duplicate.str();
            }
            const auto decoded = archive.read(i);
            if (decoded != read_reference(reference))
                throw std::runtime_error("Decoded bytes differ at index " + std::to_string(i) + ": " + e.name);
            total += decoded.size();
            if (i) entries << ",\n";
            entries << "    {\"index\":" << i << ",\"name\":" << quoted(e.name)
                    << ",\"offset\":" << e.offset << ",\"stored_size\":" << e.stored_size
                    << ",\"size\":" << e.size << ",\"extra\":" << e.extra << ",\"identical\":true}";
        }
        auto upper = archive.entries().front().name;
        for (auto& c : upper) if (c >= 'a' && c <= 'z') c -= 32;
        if (archive.find(upper) != 0) throw std::runtime_error("Case-insensitive first-match lookup failed");
        std::ofstream report(report_path, std::ios::binary);
        if (!report) throw std::runtime_error("Cannot create report");
        report << "{\n  \"status\":\"passed\",\n  \"scope\":\"Independent compiled C++ archive algorithms compared byte-for-byte with previously independently extracted records; no original EXE loaded or executed\",\n"
               << "  \"archive_cpp_sha256\":\"" << TH20_ARCHIVE_CPP_SHA256 << "\",\n"
               << "  \"archive_hpp_sha256\":\"" << TH20_ARCHIVE_HPP_SHA256 << "\",\n"
               << "  \"records\":" << archive.entries().size() << ",\n  \"unique_names\":" << counts.size()
               << ",\n  \"decoded_bytes\":" << total << ",\n  \"case_insensitive_first_match\":true,\n  \"entries\":[\n"
               << entries.str() << "\n  ]\n}\n";
        if (!report) throw std::runtime_error("Cannot finish report");
        std::cout << "Independent C++ archive: " << archive.entries().size() << " records / " << total << " bytes identical\n";
        return 0;
    } catch (const std::exception& error) { std::cerr << error.what() << '\n'; return 1; }
}
