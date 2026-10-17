#include "resource_manager.hpp"
#include <fstream>
#include <iostream>
#include <set>
#include <stdexcept>
#include <windows.h>
using namespace th20::source;
namespace r=th20::source::resources;
namespace {
unsigned checks;
void check(bool value,const char* why) {++checks;if(!value)throw std::runtime_error(why);}
Bytes file_bytes(const std::filesystem::path& path) {
    std::ifstream input(path,std::ios::binary|std::ios::ate);
    if(!input)throw std::runtime_error("Missing extracted reference");
    const auto length=input.tellg();Bytes result(static_cast<std::size_t>(length));input.seekg(0);
    if(!input.read(reinterpret_cast<char*>(result.data()),length))throw std::runtime_error("Short reference read");
    return result;
}
}
int wmain(int argc,wchar_t** argv) {
    try {
        if(argc!=4)throw std::runtime_error("Usage: manager_tests ARCHIVE RAW_DIR REPORT.json");
        const std::filesystem::path archive_path(argv[1]),raw(argv[2]),report_path(argv[3]);
        if(std::filesystem::weakly_canonical(archive_path)==std::filesystem::weakly_canonical(report_path))throw std::runtime_error("Report cannot replace archive");
        r::Manager owned;
        check(!owned.read("missing")&&owned.size("missing")==0&&owned.entry_count()==0,"Unopened manager");
        check(owned.open(archive_path),"Open real archive");
        Archive catalog(archive_path);check(owned.entry_count()==catalog.entries().size(),"Catalog count");
        std::set<std::string> visited;std::uint64_t bytes=0;
        for(std::size_t i=0;i<catalog.entries().size();++i) {
            const auto& entry=catalog.entries()[i];if(!visited.insert(entry.name).second)continue;
            auto expected=raw/entry.name;
            unsigned occurrences=0;for(const auto& other:catalog.entries())if(other.name==entry.name)++occurrences;
            if(occurrences>1) {char prefix[16];sprintf_s(prefix,"%04u_",static_cast<unsigned>(i));expected=raw.parent_path()/"duplicate_entries"/(prefix+entry.name);}
            auto value=owned.read(entry.name);const auto reference=file_bytes(expected);
            check(value&&*value==reference,"Manager decoded member differs from independent extraction");
            check(owned.size(entry.name)==reference.size(),"Member size");bytes+=reference.size();
            auto upper=entry.name;for(auto& c:upper)if(c>='a'&&c<='z')c-=32;
            check(owned.read(upper)==value,"Case-insensitive first member");
        }
        check(!owned.read("__does_not_exist__"),"Missing member");
        owned.close();check(!owned.read("thbgm.fmt")&&owned.entry_count()==0,"Close clears open archive");
        check(!owned.open(raw/"__absent_archive__.dat")&&!owned.error().empty(),"Failed reopen keeps manager closed");
        check(owned.open(archive_path),"Reopen with retained process dictionary");
        check(owned.read("thbgm.fmt")==file_bytes(raw/"thbgm.fmt"),"Reopened archive contents");
        check(r::archive_lookup_name("a/b.anm")=="b.anm","Forward separator");
        check(r::archive_lookup_name("a\\b.anm")=="a\\b.anm","Original backslash-only quirk");
        check(r::archive_lookup_name("a\\b/c.anm")=="c.anm","Mixed separators");
        check(r::archive_lookup_name("a/b\\c.anm")=="a/b\\c.anm","Slash preceding last backslash quirk");
        const auto isolated=report_path.parent_path()/"isolated_manager_tests"/std::to_string(GetCurrentProcessId());
        std::filesystem::create_directories(isolated);
        const auto loose=isolated/"loose_only_test.bin";
        {std::ofstream output(loose,std::ios::binary);output<<"independent-loose-data";}
        r::close();check(!r::read(loose.string().c_str()),"Default mode must not fall back to loose files");
        check(r::read(loose.string().c_str(),true)==file_bytes(loose),"Explicit loose mode");
        check(r::open(archive_path),"Global manager open");
        check(r::read("ignored/thbgm.fmt")==file_bytes(raw/"thbgm.fmt"),"Archive basename wrapper");
        check(!r::read("ignored\\thbgm.fmt"),"Preserve original Windows-name quirk");r::close();
        auto packed=[](const std::vector<std::pair<std::uint32_t,unsigned>>& fields){Bytes encoded;unsigned bit=0;for(auto [value,count]:fields)for(unsigned i=count;i;--i){if(!(bit%8))encoded.push_back(0);encoded.back()|=((value>>(i-1))&1)<<(7-bit%8);++bit;}return encoded;};
        const Bytes seed_bytes{'s','h','a','r','e','d','-','d','i','c','t','i','o','n','a','r','y'};
        std::vector<std::pair<std::uint32_t,unsigned>> literal_fields;for(auto byte:seed_bytes){literal_fields.push_back({1,1});literal_fields.push_back({byte,8});}literal_fields.push_back({0,14});
        check(r::decode_shared(packed(literal_fields),static_cast<std::uint32_t>(seed_bytes.size()))==seed_bytes,"Global dictionary literal stream");
        r::with_shared_dictionary([&](auto& dictionary){check(std::equal(seed_bytes.begin(),seed_bytes.end(),dictionary.begin()+1),"Compressor callback sees prior decoder writes");});
        r::close();check(r::decode_shared(packed({{0,1},{1,13},{14,4},{0,14}}),17)==seed_bytes,"Close preserves shared dictionary for subsequent references");
        const Bytes compressed_seed{'c','o','m','p','r','e','s','s'};
        r::with_shared_dictionary([&](auto& dictionary){std::copy(compressed_seed.begin(),compressed_seed.end(),dictionary.begin()+200);});
        check(r::decode_shared(packed({{0,1},{200,13},{5,4},{0,14}}),8)==compressed_seed,"Decoder sees compressor dictionary mutations");
        check(r::open(archive_path),"Archive opens after shared codec operations");check(r::read("thbgm.fmt")==file_bytes(raw/"thbgm.fmt"),"Archive manager retains correct stream output after shared codec operations");r::close();
        std::ofstream report(report_path);
        report<<"{\n  \"status\":\"passed\",\n  \"checks\":"<<checks<<",\n  \"unique_members\":"<<visited.size()<<",\n  \"decoded_bytes\":"<<bytes<<",\n  \"manager_cpp_sha256\":\""<<TH20_MANAGER_CPP_SHA<<"\",\n  \"scope\":\"Source-owned manager lifecycle, all unique first-match archive members compared with independent extraction, case-insensitive names, original path quirk and explicit loose-only selection\",\n  \"original_exe_executed\":false,\n  \"limitations\":[\"Manager ownership API replaces original stream/allocator ABI\",\"Malformed archive failure behavior is defined by the source decoder, not original unchecked memory accesses\"]\n}\n";
        if(!report)throw std::runtime_error("Cannot write manager report");
        std::cout<<"Resource manager: "<<checks<<" checks passed / "<<bytes<<" decoded bytes\n";
        return 0;
    } catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}
}
