#include "PurpleRandom.hpp"
#include <cassert>
#include <iostream>
int main() {
    std::uint32_t hash=2166136261u;
    for(unsigned seed=0;seed<=65535;++seed) {
        const int value=PracticeRuntime::PurpleDollsRandomInsert(std::uint16_t(seed));
        assert(value>=0 && value<=640);
#ifdef _MSC_VER
        std::default_random_engine engine(seed);
        std::uniform_int_distribution<int> distribution(0,640);
        assert(value==distribution(engine));
#endif
        hash=(hash^std::uint32_t(value))*16777619u;
    }
    assert(hash==2316479866u);
    std::cout << "65536 seeds, compatibility digest=" << hash << '\n';
}
