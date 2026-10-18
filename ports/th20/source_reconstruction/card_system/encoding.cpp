#include "card.hpp"
namespace th20::source::card {
int encode_time(int seconds,int hundredths) noexcept {
    const auto s=recovered::signed_bits(std::uint32_t(seconds)+66u)%1000;
    const auto h=recovered::signed_bits(std::uint32_t(hundredths)+33u)%100;
    return recovered::signed_bits(std::uint32_t(s)*100u+std::uint32_t(h)+(std::uint32_t(seconds)+22u+std::uint32_t(hundredths))*100000u);
}
bool invalid_encoded_time(int value) noexcept {return value/100000-22!=((value/100)%1000+934)%1000+(value%100+67)%100;}
}
