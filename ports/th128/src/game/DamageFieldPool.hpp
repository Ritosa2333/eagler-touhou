#pragma once
#include <array>
#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <vector>

namespace th128::damagefield {
inline constexpr std::uint32_t Capacity=2000;
using Image=std::array<std::uint8_t,0x78>;
struct Position {float x=0,y=0,z=0;};
struct Contact {Image next{};int damage=0;std::uint32_t outFlag=0;};
struct FinalDamage {int damage=0,score=0;};
struct Birth {std::uint32_t slot=0;bool created=false;};
struct PoolContact {int damage=0;std::uint32_t outFlag=0;};

std::optional<Image> circularBirth(Position position,float radius,float growth,int duration,int damage,std::uint32_t tag) noexcept;
std::optional<Image> growAndExpireUnit(const Image& image) noexcept;
std::optional<Contact> queryCircular(const Image& image,Position enemy,std::uint32_t outFlag) noexcept;
std::optional<FinalDamage> finishDamageQuery(int shotDamage,int sourceDamage,int score) noexcept;

class DamageFieldPool {
public:
    bool configure(std::span<const Image> images,std::uint32_t cursor,std::string& error);
    std::optional<Birth> spawn(Position position,float radius,float growth,int duration,int damage,std::uint32_t tag,std::string& error);
    bool tickUnit(std::string& error);
    std::optional<PoolContact> query(Position enemy,std::uint32_t outFlag,std::string& error);
    const Image& image(std::uint32_t slot) const;
    std::uint32_t cursor() const;
    std::array<std::uint32_t,Capacity> flags() const;
private:
    std::vector<Image> images_=std::vector<Image>(Capacity);
    std::uint32_t cursor_=0;
    bool configured_=false;
    std::string failure_;
    bool fail(std::string reason,std::string& error);
};
}
