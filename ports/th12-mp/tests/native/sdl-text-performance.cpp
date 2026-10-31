// Use the production rasterizer with real FreeType faces; pressure its bounded
// cache while keeping a frequently rendered ASCII line live.
#include <ft2build.h>
#include FT_FREETYPE_H
#include <cstdio>
#include <chrono>
#include <fstream>
#include <iterator>
#include <cassert>
static unsigned loads=0;
static FT_Error counted_load(FT_Face face,FT_UInt glyph,FT_Int32 flags){++loads;return FT_Load_Glyph(face,glyph,flags);}
#define FT_Load_Glyph counted_load
#include "TextRaster.cpp"
#undef FT_Load_Glyph
namespace th12host::host {
Paths& paths(){static Paths value;return value;}
void log(const char*,...){}
bool read_file(const std::string& path,std::vector<u8>& out,u64 limit){std::ifstream f(path,std::ios::binary);if(!f)return false;out.assign(std::istreambuf_iterator<char>(f),{});return out.size()<=limit;}
}
int main(int argc,char** argv){
 using namespace th12host;if(argc!=2)return 2;host::paths().data=argv[1];TextRaster raster;if(!raster.initialize())return 3;
 std::u32string hot;for(char32_t c=33;c<127;++c)hot+=c;
 raster.measure(hot,false,400,32);unsigned hot_misses=0,peak_hot_misses=0;u64 digest=1469598103934665603ull;
 const auto start=std::chrono::steady_clock::now();
 for(unsigned i=0;i<8192;++i){
  raster.measure(U"M",false,i<4096?400:700,32,float(i%4096+1)/4096);
  const auto before=loads;raster.measure(hot,false,400,32);hot_misses+=loads-before;peak_hot_misses=std::max(peak_hot_misses,loads-before);
  if(i%512==0){Canvas canvas;canvas.reset(1024,48);raster.fill(canvas,hot,0,2,false,400,32,0xffffff);for(u8 b:canvas.rgba){digest^=b;digest*=1099511628211ull;}}
 }
 const double ms=std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-start).count();
 std::printf("{\"iterations\":8192,\"hotGlyphs\":%zu,\"hotReloads\":%u,\"peakHotReloads\":%u,\"glyphLoads\":%u,\"elapsedMs\":%.3f,\"pixelHash\":\"%016llx\"}\n",hot.size(),hot_misses,peak_hot_misses,loads,ms,(unsigned long long)digest);
}
