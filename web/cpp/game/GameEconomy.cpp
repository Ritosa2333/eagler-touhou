#include "GameEconomy.hpp"
#include <algorithm>
namespace th11 {
void GameEconomy::add_score(i32 amount) noexcept {score_units=std::min(wrapping_add(score_units,amount/10),999999999);}
void GameEconomy::add_point_value(i32 amount) noexcept {point_value=std::min(wrapping_add(point_value,amount),max_point_value);}
void GameEconomy::add_rank(i32 amount) noexcept {rank=std::max(-1024,std::min(1024,wrapping_add(rank,amount)));}
i32 GameEconomy::point_item_score() const noexcept {
    const i32 percent=communication>=10000?100:signed_bits(u32(communication)*100)/10000;
    const i32 graze_factor=std::min(graze/100,899),value=point_value/100-(point_value/100)%10;
    const i32 product=signed_bits(u32(wrapping_add(percent,graze_factor))*u32(value));
    const i32 result=signed_bits(u32(product/1000)*10);
    return communication<10000&&result<=0?10:result;
}
}
