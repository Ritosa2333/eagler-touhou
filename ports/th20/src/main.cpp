#include "th20/binary.hpp"
#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>

namespace fs = std::filesystem;
namespace {
void help() {
    std::cout << "th20_inspect - evidence-preserving binary inspection (not a game runtime)\n"
        "  th20_inspect pe INPUT.exe [OUTPUT.json]\n"
        "  th20_inspect ecl INPUT.ecl [OUTPUT.json]\n"
        "  th20_inspect ecl-summary INPUT.ecl [OUTPUT.json]\n"
        "  th20_inspect ecl-scan DIRECTORY [OUTPUT.json]\n"
        "  th20_inspect ecl-roundtrip INPUT.ecl OUTPUT.ecl\n"
        "ECL commands read unpacked SCPT/ECLH containers. Unknown opcodes remain raw.\n"
        "ecl-scan recursively validates all .ecl files; it exits 2 if any fail.\n"
        "ecl-roundtrip re-encodes instruction headers/payloads and demands byte equality.\n";
}
void guard_output(const fs::path& input, const fs::path& output) {
    if (fs::weakly_canonical(input) == fs::weakly_canonical(output)
        || (fs::exists(output) && fs::equivalent(input, output)))
        throw std::runtime_error("Input and output must be different files");
}
void emit(const std::string& text, const std::vector<fs::path>& args, std::size_t output_index) {
    if (args.size() <= output_index) { std::cout << text; return; }
    guard_output(args[2], args[output_index]);
    std::ofstream out(args[output_index], std::ios::binary | std::ios::trunc);
    if (!out || !(out << text)) throw std::runtime_error("Cannot write JSON output");
}
int run(const std::vector<fs::path>& args) {
    if (args.size() == 1 || args[1] == "--help" || args[1] == "-h") { help(); return 0; }
    if (args.size() < 3 || args.size() > 4) { help(); return 1; }
    const auto command = args[1].u8string();
    std::ostringstream json;
    if (command == "pe") {
        th20::write_pe_json(json, th20::parse_pe(th20::read_file(args[2])), args[2].u8string());
    } else if (command == "ecl" || command == "ecl-summary") {
        th20::write_ecl_json(json, th20::parse_ecl(th20::read_file(args[2])), args[2].u8string(), command == "ecl");
    } else if (command == "ecl-roundtrip") {
        if (args.size() != 4) throw std::runtime_error("ecl-roundtrip requires an output file");
        guard_output(args[2], args[3]);
        const auto input = th20::read_file(args[2]);
        const auto doc = th20::parse_ecl(input);
        const auto output = th20::serialize_ecl(doc);
        if (input != output) throw std::runtime_error("Structural round trip changed bytes");
        th20::write_file(args[3], output);
        std::cout << "Byte-identical structural round trip: " << input.size() << " bytes, "
            << doc.subroutines.size() << " subroutines, " << doc.instruction_count() << " instructions.\n";
        return 0;
    } else if (command == "ecl-scan") {
        std::vector<fs::path> files;
        for (const auto& entry : fs::recursive_directory_iterator(args[2])) {
            auto extension = entry.path().extension().u8string();
            std::transform(extension.begin(), extension.end(), extension.begin(), [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });
            if (entry.is_regular_file() && extension == ".ecl") files.push_back(entry.path());
        }
        std::sort(files.begin(), files.end());
        std::size_t valid = 0, errors = 0, subroutines = 0, instructions = 0;
        std::map<std::uint16_t, std::size_t> histogram;
        json << "{\n  \"schema\":\"th20.ecl.scan.v1\",\n  \"root\":" << th20::json_string(args[2].u8string()) << ",\n  \"files\":[";
        for (std::size_t n = 0; n < files.size(); ++n) {
            if (n) json << ',';
            json << "\n    {\"path\":" << th20::json_string(fs::relative(files[n], args[2]).generic_u8string());
            try {
                const auto doc = th20::parse_ecl(th20::read_file(files[n]));
                if (th20::serialize_ecl(doc) != doc.original) throw std::runtime_error("Structural round trip changed bytes");
                ++valid; subroutines += doc.subroutines.size(); instructions += doc.instruction_count();
                for (const auto& pair : doc.opcode_histogram()) histogram[pair.first] += pair.second;
                json << ",\"valid\":true,\"roundtrip_identical\":true,\"size\":" << doc.original.size()
                    << ",\"subroutine_count\":" << doc.subroutines.size() << ",\"instruction_count\":" << doc.instruction_count();
            } catch (const std::exception& e) { ++errors; json << ",\"valid\":false,\"error\":" << th20::json_string(e.what()); }
            json << '}';
        }
        json << "\n  ],\n  \"valid_files\":" << valid << ",\n  \"invalid_files\":" << errors
            << ",\n  \"subroutine_count\":" << subroutines << ",\n  \"instruction_count\":" << instructions << ",\n  \"opcode_histogram\":{";
        bool first = true;
        for (const auto& pair : histogram) { if (!first) json << ','; first = false; json << '"' << pair.first << "\":" << pair.second; }
        json << "}\n}\n";
        emit(json.str(), args, 3);
        return errors ? 2 : 0;
    } else throw std::runtime_error("Unknown command: " + command);
    emit(json.str(), args, 3);
    return 0;
}
}
#ifdef _WIN32
int wmain(int argc, wchar_t** argv) {
#else
int main(int argc, char** argv) {
#endif
    try {
        std::vector<fs::path> args;
        for (int i = 0; i < argc; ++i) args.emplace_back(argv[i]);
        return run(args);
    } catch (const std::exception& e) { std::cerr << "error: " << e.what() << '\n'; return 1; }
}
