#include "FileResources.hpp"
#include "LanguagePatches.hpp"
#include <SDL3/SDL.h>
#include <zlib.h>
#include <cstdio>
#include <emscripten.h>
EM_ASYNC_JS(int, th075_fetch_file, (const char* path), {
 const name=UTF8ToString(path);if(!name.startsWith("/resources/")&&!name.startsWith("/fonts/")&&!name.startsWith("/music/"))return 0;
 if(Module.resourceFiles&&!Module.resourceFiles.has(name.substring(1)))return 0;
 try{Module.onAssetLoading?.(name,true);const buffer=Module.readAsset?await Module.readAsset(name):await (async()=>{const response=await fetch((Module.assetBase||".")+name);if(!response.ok)throw Error("Asset HTTP "+response.status);return response.arrayBuffer();})();const bytes=new Uint8Array(buffer);FS.mkdirTree(name.substring(0,name.lastIndexOf("/")));FS.writeFile(name,bytes);return 1;}catch(e){Module.onAssetError?.(name,String(e));return 0;}finally{Module.onAssetLoading?.(name,false);}
});
EM_JS(int,th075_chinese,(),{return Module.gameLanguage==="lang_zh-hant";});
EM_JS(int,th075_has_resource,(const char* path),{return !!Module.resourceFiles?.has(UTF8ToString(path).substring(1));});
namespace th075::sdl {
bool FileResources::chinese(){return th075_chinese()!=0;}
bool FileResources::read_file(const std::string& path,std::vector<u8>& data,u32 limit){SDL_IOStream* f=SDL_IOFromFile(path.c_str(),"rb");if(!f&&th075_fetch_file(path.c_str()))f=SDL_IOFromFile(path.c_str(),"rb");if(!f)return false;const auto size=SDL_GetIOSize(f);if(size<0||std::uint64_t(size)>limit){SDL_CloseIO(f);return false;}data.resize(std::size_t(size));const bool ok=SDL_ReadIO(f,data.data(),data.size())==data.size();SDL_CloseIO(f);return ok;}
bool FileResources::write_file(const std::string& path,const void* bytes,u32 size){const auto temporary=path+".tmp";auto* f=SDL_IOFromFile(temporary.c_str(),"wb");if(!f)return false;const bool written=SDL_WriteIO(f,bytes,size)==size;const bool closed=SDL_CloseIO(f);return written&&closed&&std::rename(temporary.c_str(),path.c_str())==0;}
bool FileResources::retain(ResourceEdition edition,const std::string& path){std::vector<u8> bytes;if(!read(edition,path,bytes))return false;retained[{edition,path}]=std::move(bytes);return true;}
bool FileResources::read(ResourceEdition edition,const std::string& resource,std::vector<u8>& out){const auto cached=retained.find({edition,resource});if(cached!=retained.end()){out=cached->second;return true;}std::string path=resource;for(auto& c:path)if(c==char(92))c='/';if(path.empty()||path[0]=='/'||path.find("..")!=std::string::npos||path.find(':')!=std::string::npos){error="Invalid resource path";return false;}std::vector<u8> compressed;const bool patch=edition==ResourceEdition::Patched;std::string packed="/resources/"+std::string(patch?"patched":"base")+"/"+path+".z";const std::string translated="/resources/chinese/"+path+".z";const bool overlay=chinese()&&th075_has_resource(translated.c_str());if((overlay&&!read_file(packed=translated,compressed))||(!overlay&&!read_file(packed,compressed)&&(!patch||!read_file(packed="/resources/base/"+path+".z",compressed)))){error="Missing resource: "+path;return false;}std::remove(packed.c_str());if(compressed.size()<4){error="Truncated resource: "+path;return false;}const u32 n=u32(compressed[0])|u32(compressed[1])<<8|u32(compressed[2])<<16|u32(compressed[3])<<24;if(n>134217728){error="Oversized resource: "+path;return false;}out.resize(n);uLongf actual=n;if(uncompress(out.data(),&actual,compressed.data()+4,compressed.size()-4)!=Z_OK||actual!=n){out.clear();error="Corrupt resource: "+path;return false;}return apply_language_patch(path,out,error);}
}
