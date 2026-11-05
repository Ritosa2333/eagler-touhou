#include "PopupManager.hpp"
namespace th15 {
void PopupManager::write_checkpoint_file(std::vector<u8>& out)const {out.resize(0x514);std::memcpy(out.data(),&saved_next,4);std::memcpy(out.data()+4,saved.data(),0x510);}
bool PopupManager::read_checkpoint_file(const u8* data,u32 size){error.clear();if(!data||size!=0x514){error="Invalid floating number checkpoint length";return false;}std::memcpy(&saved_next,data,4);std::memcpy(saved.data(),data+4,0x510);return true;}
}
