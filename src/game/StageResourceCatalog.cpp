#include "StageResourceCatalog.hpp"

#include <array>
#include <string_view>
#include <utility>

namespace th128::stage {
namespace {

constexpr std::size_t MaxResourceNameLength = 64;

bool hasExtension(std::string_view name, std::string_view extension) {
    return name.size() > extension.size() && name.substr(name.size() - extension.size()) == extension;
}

bool isSafeBasename(std::string_view name, std::string_view extension) {
    if (name.empty() || name.size() > MaxResourceNameLength || name.front() == '.') {
        return false;
    }
    for (const char character : name) {
        const unsigned char value = static_cast<unsigned char>(character);
        const bool alphanumeric = (value >= '0' && value <= '9')
            || (value >= 'a' && value <= 'z') || (value >= 'A' && value <= 'Z');
        if (!alphanumeric && value != '_' && value != '-' && value != '.') {
            return false;
        }
    }
    if (name.find("..") != std::string_view::npos) {
        return false;
    }
    return hasExtension(name, extension);
}

bool validateRecord(const StageResourceRecord& record, std::string& error) {
    const std::string prefix = "stage " + std::to_string(record.id) + ": ";
    if (!isSafeBasename(record.background, ".std")) {
        error = prefix + "background must be a safe .std basename";
        return false;
    }
    if (!isSafeBasename(record.ecl, ".ecl")) {
        error = prefix + "ecl must be a safe .ecl basename";
        return false;
    }
    if (!isSafeBasename(record.message, ".msg")) {
        error = prefix + "message must be a safe .msg basename";
        return false;
    }
    if (!isSafeBasename(record.logo, ".anm")) {
        error = prefix + "logo must be a safe .anm basename";
        return false;
    }
    for (std::size_t index = 0; index < record.music.size(); ++index) {
        if (!isSafeBasename(record.music[index], ".wav")) {
            error = prefix + "music[" + std::to_string(index) + "] must be a safe .wav basename";
            return false;
        }
    }
    return true;
}

}

bool Catalog::load(std::span<const StageResourceRecord> records, std::string& error) {
    error.clear();
    if (records.size() != StageCount) {
        error = "stage catalog requires exactly one record per ID 1..16";
        return false;
    }
    std::vector<StageResourceRecord> candidate(StageCount);
    std::array<bool, StageCount> seen{};
    for (const StageResourceRecord& record : records) {
        if (record.id < FirstStageId || record.id > LastStageId) {
            error = "stage id " + std::to_string(record.id) + " is outside 1..16";
            return false;
        }
        const std::size_t index = static_cast<std::size_t>(record.id - FirstStageId);
        if (seen[index]) {
            error = "duplicate stage id " + std::to_string(record.id);
            return false;
        }
        seen[index] = true;
        if (!validateRecord(record, error)) {
            return false;
        }
        candidate[index] = record;
    }
    records_ = std::move(candidate);
    return true;
}

const StageResourceRecord* Catalog::find(int id) const noexcept {
    if (id < FirstStageId || id > LastStageId) {
        return nullptr;
    }
    const std::size_t index = static_cast<std::size_t>(id - FirstStageId);
    if (index >= records_.size() || records_[index].id != id) {
        return nullptr;
    }
    return &records_[index];
}

std::size_t Catalog::size() const noexcept {
    return records_.size();
}

}
