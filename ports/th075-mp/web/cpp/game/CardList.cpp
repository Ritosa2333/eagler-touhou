#include "CardList.hpp"
namespace th075 {
bool CardList::load(const void* source,std::size_t size,bool encrypted){names.clear();error.clear();if(!source&&size){error="Missing card names";return false;}const auto* p=static_cast<const u8*>(source);std::string bytes(size,0);u8 key=0x60,step=0x61;for(std::size_t i=0;i<size;i++){bytes[i]=char(encrypted?p[i]^key:p[i]);key=u8(key+step);step=u8(step+0x41);}std::size_t start=0;for(std::size_t i=0;i<size;i++)if(bytes[i]==char(10)){if(i>start)names.emplace_back(bytes.data()+start,i-start-1);else names.emplace_back();start=i+1;}if(start<size){error="Unterminated card name row";names.clear();return false;}return true;}
}
