#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <utility>

namespace thprac::portable {
struct PatchContext {
    std::int32_t warpCount = 0;
    std::int32_t warpTime = 0;
    std::int32_t fakeShot = -1;
    bool nameFix = false;
    bool stage3BackgroundFix = false;
    bool ranBoundaryFix = false;
};

class ECLHelper {
public:
    ECLHelper(std::uint8_t* data, std::size_t size) : data_(data), size_(size) {}
    void SetPos(std::size_t position) { position_ = position; valid_ = valid_ && position <= size_; }
    bool Valid() const { return valid_; }

    template <typename T> ECLHelper& operator<<(T value)
    {
        if (!valid_ || position_ > size_ || sizeof(T) > size_ - position_) {
            valid_ = false;
            return *this;
        }
        std::memcpy(data_ + position_, &value, sizeof(T));
        position_ += sizeof(T);
        return *this;
    }
    template <typename Offset, typename T> ECLHelper& operator<<(std::pair<Offset, T> value)
    {
        SetPos(static_cast<std::size_t>(value.first));
        return *this << value.second;
    }

private:
    std::uint8_t* data_;
    std::size_t size_;
    std::size_t position_ = 0;
    bool valid_ = true;
};

inline void SetWarp(PatchContext& context, std::int32_t count, std::int32_t time)
{
    context.warpCount = count;
    context.warpTime = time;
}
} // namespace thprac::portable
