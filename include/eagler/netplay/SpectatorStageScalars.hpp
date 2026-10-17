#pragma once
#include <eagler/netplay/SpectatorStageProtocol.hpp>
#include <cmath>
#include <cstddef>
#include <cstring>
#include <type_traits>

namespace Netplay::SpectatorScalars {
inline constexpr std::size_t MaxLocalBytes = 8u * 1024u * 1024u;
// The title explicitly visits named scalar fields in a versioned schema.
// Native structs, pointers, padding and object ownership never enter the wire.
class Writer {
public:
    Writer() = default;
    // A title may encode a larger named state before bounded compression.
    // This local allocation budget never changes the protocol's wire limit.
    explicit Writer(std::size_t maxBytes)
        : limit_(maxBytes), good_(maxBytes > 0 && maxBytes <= MaxLocalBytes) {}
    template<class T> bool Value(const T& value) {
        static_assert(std::is_arithmetic_v<T> || std::is_enum_v<T>, "Visit named scalar fields only");
        if constexpr (std::is_enum_v<T>) return Value(static_cast<std::underlying_type_t<T>>(value));
        else if constexpr (std::is_same_v<T, bool>) return Value(std::uint8_t(value));
        else {
            static_assert(sizeof(T) == 1 || sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8);
            if constexpr (std::is_floating_point_v<T>) if (!std::isfinite(value)) return good_ = false;
            if (!good_ || bytes_.size() > limit_ || sizeof(T) > limit_ - bytes_.size()) return good_ = false;
            using Bits = std::conditional_t<sizeof(T) == 1, std::uint8_t,
                std::conditional_t<sizeof(T) == 2, std::uint16_t,
                std::conditional_t<sizeof(T) == 4, std::uint32_t, std::uint64_t>>>;
            Bits bits = 0; std::memcpy(&bits, &value, sizeof(T));
            for (unsigned n = 0; n < sizeof(T); ++n) bytes_.push_back(std::uint8_t(bits >> (8 * n)));
            return true;
        }
    }
    template<class... T> bool Values(const T&... values) {return (Value(values) && ...);}
    template<class Range> bool Array(const Range& values) {for (const auto& value : values) if (!Value(value)) return false; return true;}
    bool Good() const {return good_;}
    const std::vector<std::uint8_t>& Bytes() const {return bytes_;}
private:
    std::vector<std::uint8_t> bytes_;
    std::size_t limit_ = SpectatorStageMaxStateBytes;
    bool good_ = true;
};
class Reader {
public:
    explicit Reader(const std::vector<std::uint8_t>& bytes) : bytes_(bytes) {}
    template<class T> bool Value(T& value) {
        static_assert(std::is_arithmetic_v<T> || std::is_enum_v<T>, "Visit named scalar fields only");
        if constexpr (std::is_enum_v<T>) {std::underlying_type_t<T> raw{}; if (!Value(raw)) return false; value = static_cast<T>(raw); return true;}
        else if constexpr (std::is_same_v<T, bool>) {std::uint8_t raw{}; if (!Value(raw) || raw > 1) return good_ = false; value = raw != 0; return true;}
        else {
            static_assert(sizeof(T) == 1 || sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8);
            if (!good_ || sizeof(T) > bytes_.size() - at_) return good_ = false;
            using Bits = std::conditional_t<sizeof(T) == 1, std::uint8_t,
                std::conditional_t<sizeof(T) == 2, std::uint16_t,
                std::conditional_t<sizeof(T) == 4, std::uint32_t, std::uint64_t>>>;
            Bits bits = 0;
            for (unsigned n = 0; n < sizeof(T); ++n) bits |= Bits(bytes_[at_++]) << (8 * n);
            T decoded{}; std::memcpy(&decoded, &bits, sizeof(T));
            if constexpr (std::is_floating_point_v<T>) if (!std::isfinite(decoded)) return good_ = false;
            value = decoded; return true;
        }
    }
    template<class... T> bool Values(T&... values) {return (Value(values) && ...);}
    template<class Range> bool Array(Range& values) {for (auto& value : values) if (!Value(value)) return false; return true;}
    bool Done() const {return good_ && at_ == bytes_.size();}
private:
    const std::vector<std::uint8_t>& bytes_;
    std::size_t at_ = 0;
    bool good_ = true;
};
}
