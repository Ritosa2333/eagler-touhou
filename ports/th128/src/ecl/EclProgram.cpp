#include "EclProgram.hpp"

#include <algorithm>
#include <cstring>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <functional>
#include <limits>

namespace th128::ecl {
namespace {

std::uint16_t read16(const std::uint8_t* bytes) {
    return std::uint16_t(bytes[0]) | (std::uint16_t(bytes[1]) << 8);
}

std::uint32_t read32(const std::uint8_t* bytes) {
    return std::uint32_t(bytes[0]) | (std::uint32_t(bytes[1]) << 8) |
           (std::uint32_t(bytes[2]) << 16) | (std::uint32_t(bytes[3]) << 24);
}

bool fail(std::string& error, const std::string& message, std::size_t offset = 0) {
    error = message + " at byte " + std::to_string(offset);
    return false;
}

bool readName(const std::uint8_t* bytes, std::size_t& position, std::size_t limit,
              std::string& name, std::string& error) {
    const auto start = position;
    if (position >= limit)
        return fail(error, "missing identifier", position);
    const auto end = position + std::min<std::size_t>(limit - position, 4096);
    while (position < end && bytes[position]) {
        if (bytes[position] < 32 || bytes[position] > 126)
            return fail(error, "non-ASCII identifier", position);
        ++position;
    }
    if (position == start || position == end)
        return fail(error, "empty or unterminated identifier", start);
    name.assign(reinterpret_cast<const char*>(bytes + start), position - start);
    ++position;
    return true;
}

std::string folded(std::string name) {
    for (auto& character : name)
        if (character >= 'A' && character <= 'Z')
            character = static_cast<char>(character + ('a' - 'A'));
    return name;
}

bool archiveName(const std::string& name) {
    if (name.empty() || name.size() > 200 || name.back() == '.' || name.find("..") != std::string::npos)
        return false;
    const auto identifier = [](char character) {
        return (character >= 'a' && character <= 'z') || (character >= 'A' && character <= 'Z') ||
               (character >= '0' && character <= '9') || character == '_';
    };
    if (!identifier(name.front()))
        return false;
    for (char character : name)
        if (!identifier(character) && character != '.' && character != '-')
            return false;
    const auto stem = folded(name.substr(0, name.find('.')));
    if (stem == "con" || stem == "prn" || stem == "aux" || stem == "nul" ||
        stem == "conin$" || stem == "conout$")
        return false;
    return !(stem.size() == 4 && (stem.substr(0, 3) == "com" || stem.substr(0, 3) == "lpt") &&
             stem[3] >= '0' && stem[3] <= '9');
}

}

struct Program::Data {
    Resource resource;
    std::vector<Subroutine> subroutines;
    std::unordered_map<std::string, std::size_t> names;
    std::vector<std::shared_ptr<const Data>> dependencies;
    std::size_t totalBytes = 0;
    bool practiceBranches = false;
};

const Instruction* Subroutine::instructionAt(std::uint32_t offset) const {
    const auto found = std::lower_bound(instructions.begin(), instructions.end(), offset,
        [](const Instruction& instruction, std::uint32_t target) { return instruction.offset < target; });
    return found != instructions.end() && found->offset == offset ? &*found : nullptr;
}

bool Program::load(const void* input, std::size_t size, std::string& error) {
    if (!input || size < 36 || size > maxBytes)
        return fail(error, "null, truncated, or excessive SCPT resource");
    const auto* bytes = static_cast<const std::uint8_t*>(input);
    if (std::memcmp(bytes, "SCPT", 4) || read16(bytes + 4) != 1 || read32(bytes + 8) != 36)
        return fail(error, "unsupported SCPT header");
    const std::size_t table = 36 + read16(bytes + 6);
    const std::size_t count = read32(bytes + 16);
    if (table > size || count > maxSubroutines || count > (size - table) / 4)
        return fail(error, "invalid subroutine table", table);

    auto candidate = std::make_shared<Data>();
    std::size_t position = 36;
    std::size_t includeTotal = 0;
    std::unordered_set<std::string> sectionNames;
    std::unordered_set<std::string> includeNames;
    while (position < table) {
        if (table - position < 8)
            return fail(error, "truncated include section", position);
        const std::string kind(reinterpret_cast<const char*>(bytes + position), 4);
        if ((kind != "ANIM" && kind != "ECLI") || !sectionNames.insert(kind).second)
            return fail(error, "unknown or duplicate include section", position);
        const std::size_t includes = read32(bytes + position + 4);
        if (includes > maxIncludes - includeTotal)
            return fail(error, "aggregate include budget exceeded", position);
        includeTotal += includes;
        position += 8;
        auto& destination = kind == "ANIM" ? candidate->resource.animationIncludes : candidate->resource.eclIncludes;
        for (std::size_t index = 0; index < includes; ++index) {
            std::string name;
            if (!readName(bytes, position, table, name, error))
                return false;
            if (!archiveName(name) || !includeNames.insert(folded(name)).second)
                return fail(error, "unsafe, duplicate, or aliased include", position);
            destination.push_back(std::move(name));
        }
        const auto aligned = (position + 3) & ~std::size_t(3);
        if (aligned > table)
            return fail(error, "truncated include padding", position);
        while (position < aligned)
            if (bytes[position++] != 0)
                return fail(error, "nonzero include padding", position - 1);
    }

    std::vector<std::uint32_t> offsets;
    for (std::size_t index = 0; index < count; ++index)
        offsets.push_back(read32(bytes + table + index * 4));
    auto ordered = offsets;
    std::sort(ordered.begin(), ordered.end());
    if (std::adjacent_find(ordered.begin(), ordered.end()) != ordered.end())
        return fail(error, "duplicate subroutine offsets", table);
    const std::size_t namesEnd = ordered.empty() ? size : ordered.front();
    position = table + count * 4;
    if (namesEnd < position || namesEnd > size)
        return fail(error, "subroutine overlaps table or exceeds resource", namesEnd);
    for (std::size_t index = 0; index < count; ++index) {
        Subroutine sub;
        if (!readName(bytes, position, namesEnd, sub.name, error))
            return false;
        if (!candidate->names.emplace(sub.name, index).second)
            return fail(error, "duplicate subroutine name", position);
        sub.fileOffset = offsets[index];
        candidate->subroutines.push_back(std::move(sub));
    }
    if (namesEnd - position > 3)
        return fail(error, "excessive name padding", position);
    while (position < namesEnd)
        if (bytes[position++] != 0)
            return fail(error, "nonzero name padding", position - 1);

    std::size_t instructionTotal = 0;
    for (auto& sub : candidate->subroutines) {
        const std::size_t start = sub.fileOffset;
        const auto next = std::upper_bound(ordered.begin(), ordered.end(), sub.fileOffset);
        const std::size_t limit = next == ordered.end() ? size : *next;
        if ((start & 3) || start > limit || limit - start < 16 || limit > size ||
            std::memcmp(bytes + start, "ECLH", 4) || read32(bytes + start + 4) != 16)
            return fail(error, "invalid or truncated ECLH header", start);
        sub.codeSize = static_cast<std::uint32_t>(limit - start - 16);
        position = start + 16;
        while (position < limit) {
            if (limit - position < 16)
                return fail(error, "truncated instruction header", position);
            const std::size_t length = read16(bytes + position + 6);
            if (length < 16 || (length & 3) || length > limit - position)
                return fail(error, "invalid or truncated instruction length", position);
            if (++instructionTotal > maxInstructions)
                return fail(error, "aggregate instruction budget exceeded", position);
            Instruction instruction;
            instruction.offset = static_cast<std::uint32_t>(position - start - 16);
            const auto time = read32(bytes + position);
            std::memcpy(&instruction.time, &time, sizeof(time));
            instruction.opcode = read16(bytes + position + 4);
            instruction.size = static_cast<std::uint16_t>(length);
            instruction.parameterMask = read16(bytes + position + 8);
            instruction.rankMask = bytes[position + 10];
            instruction.parameterCount = bytes[position + 11];
            instruction.stackOffset = read32(bytes + position + 12);
            for (std::size_t payload = 16; payload < length; payload += 4)
                instruction.rawWords.push_back(read32(bytes + position + payload));
            sub.instructions.push_back(std::move(instruction));
            position += length;
        }
    }
    candidate->resource.bytes.assign(bytes, bytes + size);
    candidate->totalBytes = size;
    data_ = std::move(candidate);
    error.clear();
    return true;
}

bool Program::withPracticeBranches(const std::vector<std::uint8_t>& expectedSource,
        const std::vector<PracticeBranch>& branches, Program& output, std::string& error) const {
    if (!data_ || !data_->dependencies.empty() || data_->practiceBranches)
        return fail(error, "practice branches require an original unlinked Program");
    if (expectedSource.empty() || expectedSource != data_->resource.bytes)
        return fail(error, "practice branch source bytes differ from the owned Program");
    if (branches.size() > 128)
        return fail(error, "practice branch budget exceeded");
    struct Site { std::size_t subroutine, instruction; };
    std::vector<Site> sites;
    std::unordered_set<std::uint32_t> seen;
    for (const auto& branch : branches) {
        if (branch.eclTime < 0 || branch.targetTime < 0 || !seen.insert(branch.sourceOffset).second)
            return fail(error, "negative practice clock or duplicate branch site", branch.sourceOffset);
        bool found = false;
        for (std::size_t index = 0; index < data_->subroutines.size(); ++index) {
            const auto& sub = data_->subroutines[index];
            const std::uint64_t begin = std::uint64_t(sub.fileOffset) + 16;
            if (branch.sourceOffset < begin || branch.sourceOffset >= begin + sub.codeSize)
                continue;
            const auto* instruction = sub.instructionAt(std::uint32_t(branch.sourceOffset - begin));
            if (!instruction || branch.targetOffset < begin || branch.targetOffset >= begin + sub.codeSize
                || !sub.instructionAt(std::uint32_t(branch.targetOffset - begin)))
                return fail(error, "practice branch must use instruction boundaries in the same subroutine", branch.sourceOffset);
            const auto delta = std::int64_t(branch.targetOffset) - branch.sourceOffset;
            if (delta < std::numeric_limits<std::int32_t>::min() || delta > std::numeric_limits<std::int32_t>::max())
                return fail(error, "practice relative branch exceeds signed range", branch.sourceOffset);
            sites.push_back({index, std::size_t(instruction - sub.instructions.data())});
            found = true;
            break;
        }
        if (!found)
            return fail(error, "practice branch source is not an owned instruction", branch.sourceOffset);
    }
    auto candidate = std::make_shared<Data>(*data_);
    for (std::size_t index = 0; index < branches.size(); ++index) {
        const auto& branch = branches[index];
        auto& instruction = candidate->subroutines[sites[index].subroutine].instructions[sites[index].instruction];
        // Virtual executed shape: original serialized offsets and bytes stay intact.
        instruction.time = branch.eclTime;
        instruction.opcode = 12;
        instruction.size = 24;
        instruction.parameterMask = 0;
        instruction.rankMask = 255;
        instruction.parameterCount = 2;
        instruction.stackOffset = 0;
        instruction.rawWords = {std::uint32_t(std::int64_t(branch.targetOffset) - branch.sourceOffset),
            std::uint32_t(branch.targetTime)};
    }
    candidate->practiceBranches = true;
    output.data_ = std::move(candidate);
    error.clear();
    return true;
}

bool Program::withPracticeRegions(const std::vector<std::uint8_t>& expectedSource,
        const std::vector<PracticeRegion>& regions,Program& output,std::string& error) const {
    if(!data_||!data_->dependencies.empty()||data_->practiceBranches)
        return fail(error,"practice regions require original unlinked sources");
    if(expectedSource.empty()||expectedSource!=data_->resource.bytes||regions.empty()||regions.size()>512)
        return fail(error,"practice region source or count mismatch");
    struct Site {std::size_t sub,first,last;};
    std::vector<Site> sites;std::uint32_t previousEnd=0;
    for(const auto& region:regions){
        if(region.sourceOffset<previousEnd||region.sourceOffset>=region.endOffset||region.instructions.empty()
            ||region.instructions.size()>maxInstructions)return fail(error,"overlapping or empty practice region");
        previousEnd=region.endOffset;bool found=false;
        for(std::size_t si=0;si<data_->subroutines.size();++si){
            const auto& sub=data_->subroutines[si];const auto begin=std::uint64_t(sub.fileOffset)+16;
            if(region.sourceOffset<begin||region.sourceOffset>=begin+sub.codeSize)continue;
            const auto* first=sub.instructionAt(std::uint32_t(region.sourceOffset-begin));
            const auto* last=region.endOffset==begin+sub.codeSize?nullptr:sub.instructionAt(std::uint32_t(region.endOffset-begin));
            if(!first||region.endOffset>begin+sub.codeSize||(!last&&region.endOffset!=begin+sub.codeSize))
                return fail(error,"practice erase range is not one original subroutine");
            std::uint64_t prior=0;bool initial=true;
            for(const auto& ins:region.instructions){
                if((!initial&&ins.offset<=prior)||ins.offset<region.sourceOffset-begin||ins.size<16
                    ||ins.size%4||ins.rawWords.size()!=(ins.size-16)/4||ins.rawWords.size()>16379
                    ||ins.parameterCount>ins.rawWords.size()||std::uint64_t(ins.offset)+ins.size>maxBytes
                    ||(region.endOffset!=begin+sub.codeSize&&std::uint64_t(ins.offset)+ins.size>region.endOffset-begin))
                    return fail(error,"invalid virtual practice instruction shape or boundary");
                initial=false;prior=ins.offset;
            }
            sites.push_back({si,std::size_t(first-sub.instructions.data()),last?std::size_t(last-sub.instructions.data()):sub.instructions.size()});
            found=true;break;
        }
        if(!found)return fail(error,"practice region has no owned original instruction");
    }
    auto candidate=std::make_shared<Data>(*data_);
    for(std::size_t n=regions.size();n>0;--n){
        const auto& region=regions[n-1];const auto site=sites[n-1];auto& sub=candidate->subroutines[site.sub];
        sub.instructions.erase(sub.instructions.begin()+site.first,sub.instructions.begin()+site.last);
        sub.instructions.insert(sub.instructions.begin()+site.first,region.instructions.begin(),region.instructions.end());
        const auto& tail=sub.instructions.back();
        sub.codeSize=std::uint32_t(std::max<std::uint64_t>(sub.codeSize,std::uint64_t(tail.offset)+tail.size));
        if(sub.instructions.size()>maxInstructions)return fail(error,"virtual practice instruction budget");
    }
    for(std::size_t n=0;n<regions.size();++n){
        const auto& sub=candidate->subroutines[sites[n].sub];
        for(const auto& ins:regions[n].instructions)if(ins.opcode>=12&&ins.opcode<=14){
            if(ins.rawWords.size()!=2)return fail(error,"practice jump payload");
            const auto target=std::int64_t(ins.offset)+std::int32_t(ins.rawWords[0]);
            if(target<0||target>=sub.codeSize||!sub.instructionAt(std::uint32_t(target)))
                return fail(error,"virtual practice jump does not target an instruction");
        }
    }
    candidate->practiceBranches=true;output.data_=std::move(candidate);error.clear();return true;
}

bool Program::loaded() const { return bool(data_); }

const Resource& Program::resource() const {
    static const Resource empty;
    return data_ ? data_->resource : empty;
}

const std::vector<Subroutine>& Program::subroutines() const {
    static const std::vector<Subroutine> empty;
    return data_ ? data_->subroutines : empty;
}

const Subroutine* Program::find(std::string_view name) const {
    if (!data_)
        return nullptr;
    const auto found = data_->names.find(std::string(name));
    return found == data_->names.end() ? nullptr : &data_->subroutines[found->second];
}

std::size_t Program::sourceCount() const { return data_ ? data_->dependencies.size()+1 : 0; }
std::size_t Program::sourceBytes() const { return data_ ? data_->totalBytes : 0; }

bool Program::linkIncludes(const std::vector<std::pair<std::string,Program>>& providers, std::string& error) {
    if(!data_ || !data_->dependencies.empty() || providers.size()>maxIncludes)
        return fail(error,"link requires a loaded unlinked root and bounded providers");
    std::unordered_map<std::string,const Program*> available;
    std::unordered_set<std::string> foldedNames;
    for(const auto& [name,program]:providers) {
        if(!archiveName(name)||!program.loaded()||!program.data_->dependencies.empty()||!foldedNames.insert(folded(name)).second)
            return fail(error,"invalid, linked or case-ambiguous include provider: "+name);
        available.emplace(name,&program);
    }
    auto candidate=std::make_shared<Data>();
    candidate->resource=data_->resource;
    std::unordered_set<const Data*> active,completed;
    std::unordered_map<const Data*,std::size_t> subtreeDepth;
    std::size_t instructions=0;
    std::function<bool(const std::shared_ptr<const Data>&,std::size_t)> visit;
    visit=[&](const std::shared_ptr<const Data>& source,std::size_t depth) {
        if(depth>64||active.count(source.get()))return fail(error,"ECL include cycle or depth budget");
        if(completed.count(source.get()))
            return depth+subtreeDepth.at(source.get())<=64||fail(error,"ECL include longest-path depth budget");
        if(source->resource.bytes.size()>maxBytes-candidate->totalBytes)
            return fail(error,"linked ECL aggregate byte budget");
        if(source->subroutines.size()>maxSubroutines-candidate->subroutines.size())
            return fail(error,"linked ECL subroutine budget");
        active.insert(source.get());
        candidate->totalBytes+=source->resource.bytes.size();
        candidate->practiceBranches=candidate->practiceBranches||source->practiceBranches;
        for(const auto& sub:source->subroutines) {
            if(sub.instructions.size()>maxInstructions-instructions)return fail(error,"linked ECL instruction budget");
            instructions+=sub.instructions.size();
            if(!candidate->names.emplace(sub.name,candidate->subroutines.size()).second)
                return fail(error,"ambiguous linked ECL subroutine: "+sub.name);
            candidate->subroutines.push_back(sub);
        }
        if(source.get()!=data_.get())candidate->dependencies.push_back(source);
        std::size_t maximumChildDepth=0;
        for(const auto& include:source->resource.eclIncludes) {
            const auto found=available.find(include);
            if(found==available.end())return fail(error,"missing ECL include: "+include);
            if(!visit(found->second->data_,depth+1))return false;
            maximumChildDepth=std::max(maximumChildDepth,subtreeDepth.at(found->second->data_.get())+1);
        }
        subtreeDepth[source.get()]=maximumChildDepth;
        active.erase(source.get());
        completed.insert(source.get());
        return true;
    };
    if(!visit(data_,0))return false;
    data_=std::move(candidate);
    error.clear();
    return true;
}

}
