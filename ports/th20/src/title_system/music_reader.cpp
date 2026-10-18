#include "music.hpp"
#include "music_data.hpp"
#include "../archive/resource_manager.hpp"
#include <stdexcept>

namespace th20::source::title {
// 51f890 resource boundary. Its native thread callback also accepts one unused
// stack argument (ret 4); the source worker captures its TitleInf directly.
void read_music_comments(TitleInf& o) {
    auto bytes=resources::read(music_data::s_005753e0,false);
    if(!bytes)throw std::runtime_error("Missing musiccmt.txt");
    parse_music_comments(o,{reinterpret_cast<const char*>(bytes->data()),bytes->size()});
}
}
