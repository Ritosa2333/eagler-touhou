#include "type0.hpp"
#include "type1.hpp"
#include "type2.hpp"
#include "type3.hpp"
namespace th20::source::laser {
std::uint32_t spawn_laser(Controller& owner,std::uint32_t kind,const void* p){
    switch(kind){
    case 0:return spawn_type0(owner,*static_cast<const Type0Parameters*>(p));
    case 1:return spawn_type1(owner,*static_cast<const Type1Parameters*>(p));
    case 2:return spawn_type2(owner,*static_cast<const Type2Parameters*>(p));
    case 3:return spawn_type3(owner,*static_cast<const Type3Parameters*>(p));
    default:if(recovered::signed_bits(owner.count)>511)return 0;++owner.next_handle;if(recovered::signed_bits(owner.next_handle)<0x10000)owner.next_handle=0x10000;return owner.next_handle;
    }
}
}
