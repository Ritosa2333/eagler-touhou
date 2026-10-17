#include "../th08_web/cpp/game/PracticeRsqrtTables.hpp"
#include <cassert>
#include <cstring>
using th08::PracticeRsqrtTables;
int main(){
    PracticeRsqrtTables::initialize();
    for(char cpu:{'i','a'}){
        assert(PracticeRsqrtTables::lookup_bits(0,cpu)==0x7f800000);
        assert(PracticeRsqrtTables::lookup_bits(0x80000000,cpu)==0xff800000);
        assert(PracticeRsqrtTables::lookup_bits(0x7f800000,cpu)==0);
        assert(PracticeRsqrtTables::lookup_bits(0xff800000,cpu)==0xffc00000);
        assert(PracticeRsqrtTables::lookup_bits(0xbf800000,cpu)==0xffc00000);
        assert(PracticeRsqrtTables::lookup_bits(0x7fc00000,cpu)==0x7fc00000);
        for(unsigned mant=0;mant<4096;++mant){
            const unsigned input=0x3f800000+(mant<<11);
            const unsigned result=PracticeRsqrtTables::lookup_bits(input,cpu);
            assert(result>=0x3f34f000&&result<=0x3f800000);
            assert(PracticeRsqrtTables::lookup_bits(input+2047,cpu)==result);
            assert(PracticeRsqrtTables::lookup_bits(input+0x1000000,cpu)==result-0x800000);
        }
    }
    std::vector<char> native(PracticeRsqrtTables::custom_size);unsigned value=0x3f123456;
    std::memcpy(native.data()+size_t(0x3f800000>>11)*4,&value,4);
    assert(!PracticeRsqrtTables::import_custom(native.data(),native.size()-1));
    assert(PracticeRsqrtTables::import_custom(native.data(),native.size()));
    assert(PracticeRsqrtTables::lookup_bits(0x3f800000,'c')==value);
    assert(!PracticeRsqrtTables::import_custom(nullptr,native.size()));
    assert(PracticeRsqrtTables::lookup_bits(0x3f800000,'c')==value);
    assert(PracticeRsqrtTables::custom()==native);
}
