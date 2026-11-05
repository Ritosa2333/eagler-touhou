#include "ExactFloat.hpp"
#include <cstdio>
#include <initializer_list>
struct Extended {std::uint64_t significand;std::uint16_t exponent;std::uint16_t pad[3]{};};
extern "C" std::int64_t extF80M_to_i64(const Extended*,std::uint8_t,bool);
int main(){
    std::uint64_t random=0x8a7a94e7c11584acull;auto next=[&](){random^=random<<13;random^=random>>7;random^=random<<17;return random;};
    unsigned checked=0,declined=0;
    for(unsigned i=0;i<1000000;i++){
        Extended value{next()|0x8000000000000000ull,std::uint16_t((i&1?0x8000:0)|((i%4)?16300+(next()%180):next()%32768))};
        if(i%100==0)value={0,std::uint16_t(i&1?0x8000:0)};
        std::int32_t actual=0;if(!touhou::numeric::truncate_low_word(value,actual)){++declined;continue;}
        const auto reference=std::uint32_t(extF80M_to_i64(&value,1,false));std::uint32_t bits;std::memcpy(&bits,&actual,4);
        if(bits!=reference){std::printf("Mismatch at %u\n",i);return 1;}++checked;
    }
    for(auto exponent:{std::uint16_t(16446),std::uint16_t(32767),std::uint16_t(0)}){Extended value{0x8000000000000001ull,exponent};std::int32_t out;if(touhou::numeric::truncate_low_word(value,out))return 2;}
    std::printf("PASS: %u exact conversions; %u retained fallbacks\n",checked,declined);return checked>500000&&declined>10000?0:3;
}
