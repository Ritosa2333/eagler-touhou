#pragma once
#include "../game/GameResources.hpp"
#include <map>
namespace th075::sdl {
class FileResources final:public ResourceStore {
 std::map<std::pair<ResourceEdition,std::string>,std::vector<u8>> retained;
public:
 std::string error;
 bool read(ResourceEdition,const std::string&,std::vector<u8>&)override;
 bool retain(ResourceEdition,const std::string&);
 static bool chinese();
 static bool read_file(const std::string&,std::vector<u8>&,u32 limit=134217728);
 static bool write_file(const std::string&,const void*,u32);
};
}
