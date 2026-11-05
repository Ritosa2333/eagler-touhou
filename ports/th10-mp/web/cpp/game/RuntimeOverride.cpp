#include "RuntimeOverride.hpp"
#include <string>
#if defined(TH_NATIVE_PLATFORM)&&defined(TH_ENABLE_THCRAP)
namespace th10 { bool sdl_read_file(const char*,std::vector<u8>&); }
#endif
namespace th10 {
bool RuntimeOverride::Read(const char* path,std::vector<u8>& out){
#if defined(TH_NATIVE_PLATFORM)&&defined(TH_ENABLE_THCRAP)
    if(!path||!*path)return false;
    std::string relative(path);
    for(char& c:relative)if(c=='\\')c='/';
    while(relative.rfind("./",0)==0)relative.erase(0,2);
    if(relative.empty()||relative.front()=='/'||relative.find(':')!=std::string::npos||
       relative==".."||relative.rfind("../",0)==0||relative.find("/../")!=std::string::npos||
       (relative.size()>=3&&relative.compare(relative.size()-3,3,"/..")==0))return false;
    return sdl_read_file(("/thcrap/th10/"+relative).c_str(),out);
#else
    (void)path;(void)out;return false;
#endif
}
}
