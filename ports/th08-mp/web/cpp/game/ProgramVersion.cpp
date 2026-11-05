#include "ProgramVersion.hpp"
namespace th08 {
bool compatible_program(const char* version,u32 size,u32 checksum,bool replay) noexcept {
    // Original th08_0100d.ver, plus the replay-only fingerprint supported by the
    // existing browser port. Bounded entries avoid the original failed-match loop.
    struct Build {char version[6];u32 size,checksum;};
    static constexpr Build builds[]={
        {"0009_",566784,993132091u},{"0009a",596992,489076697u},
        {"0010_",596992,3478009099u},{"0011_",596992,3545283257u},
        {"0001_",800256,1644916963u},{"0001a",807424,256801575u},
        {"0002_",809984,1745855920u},{"0003_",825344,922048924u},
        {"0003a",825344,2460321175u},{"0100_",840192,3552390786u},
        {"0100a",841216,3000360128u},{"0100b",841216,4189886502u},
        {"0100c",840704,1605176696u},{"0100c",840704,1588396223u},
        {"0100d",840704,2724749753u}};
    if(!std::memcmp(version,"debug",5))return true;
    for(const auto& build:builds)if(!std::memcmp(version,build.version,5)&&size==build.size&&checksum==build.checksum)return true;
    return replay&&!std::memcmp(version,"0100d",5)&&size==840704&&checksum==3021176207u;
}
}
