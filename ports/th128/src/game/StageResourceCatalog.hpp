#pragma once

#include <array>
#include <cstddef>
#include <span>
#include <string>
#include <vector>

namespace th128::stage {

struct StageResourceRecord {
    int id = 0;
    std::string background;
    std::string ecl;
    std::string message;
    std::string logo;
    std::array<std::string, 2> music{};
};

class Catalog {
public:
    static constexpr int FirstStageId = 1;
    static constexpr int LastStageId = 16;
    static constexpr std::size_t StageCount = 16;

    bool load(std::span<const StageResourceRecord> records, std::string& error);

    const StageResourceRecord* find(int id) const noexcept;

    std::size_t size() const noexcept;

private:
    std::vector<StageResourceRecord> records_;
};

}
