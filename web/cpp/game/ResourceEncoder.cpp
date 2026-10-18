#include "ResourceCodec.hpp"
namespace th10 {
// 0x436000 / 0x436210..0x436330. Equal-length matches select the last visited
// node. Node zero's parent is written during replacement, just as in the tree.
void LzssSearch::initialize() noexcept {std::memset(dictionary,0,8192);std::memset(nodes,0,8193*sizeof(LzssSearchNode));nodes[8192].right=1;nodes[1].parent=8192;}
void LzssSearch::replace(u32 destination,u32 source) noexcept {auto& from=nodes[source];auto& parent=nodes[from.parent];if(parent.left==source)parent.left=destination;else parent.right=destination;nodes[destination]=from;nodes[nodes[destination].left].parent=destination;nodes[nodes[destination].right].parent=destination;from.parent=0;}
void LzssSearch::promote(u32 destination,u32 source) noexcept {nodes[destination].parent=nodes[source].parent;auto& parent=nodes[nodes[source].parent];if(parent.right==source)parent.right=destination;else parent.left=destination;nodes[source].parent=0;}
u32 LzssSearch::predecessor(u32 index) const noexcept {u32 result=nodes[index].left;while(nodes[result].right)result=nodes[result].right;return result;}
void LzssSearch::remove(u32 index) noexcept {if(!nodes[index].parent)return;if(!nodes[index].right)promote(nodes[index].left,index);else if(!nodes[index].left)promote(nodes[index].right,index);else{const u32 previous=predecessor(index);remove(previous);replace(previous,index);}}
u32 LzssSearch::insert(u32 index,u32& match) noexcept {if(!index)return 0;u32 best=0,current=nodes[8192].right;for(;;){u32 length=0;i32 difference=0;while(length<18){difference=static_cast<i32>(dictionary[(index+length)&8191])-dictionary[(current+length)&8191];if(difference)break;++length;}if(length>=best){match=current;best=length;if(length>=18){replace(index,current);return length;}}auto& next=difference<0?nodes[current].left:nodes[current].right;if(next){current=next;continue;}next=index;nodes[index].parent=current;nodes[index].right=nodes[index].left=0;return best;}}
struct ResourceWriter {
    u8* begin;u32 used=0;u8 byte=0,mask=128;
    void bit(u32 value) noexcept {if(value)byte|=mask;mask>>=1;if(!mask){begin[used++]=byte;byte=0;mask=128;}}
    void bits(u32 value,u32 count) noexcept {while(count)bit((value>>--count)&1);}
};
// 0x4359b0. Only complete output bytes are returned. The decoder supplies
// zero bits past EOF, completing the trailing 14-bit zero terminator.
u8* encode_lzss(const u8* source,i32 length,u32& packed_length,LzssSearch search,CodecMemory& memory){
    auto* output=memory.allocate_bytes(static_cast<u32>(length)*2);if(!output)return nullptr;packed_length=0;search.initialize();ResourceWriter writer{output};i32 cursor=0,available=0;while(cursor<length&&available<18)search.dictionary[++available]=source[cursor++];
    u32 head=1,match=0,match_length=0;
    while(available>0){if(match_length>static_cast<u32>(available))match_length=available;u32 consumed;if(match_length<3){writer.bit(1);writer.bits(search.dictionary[head],8);consumed=1;}else{writer.bit(0);writer.bits(match,13);writer.bits(match_length-3,4);consumed=match_length;}
        while(consumed--){const auto tail=(head+18)&8191;search.remove(tail);if(cursor<length)search.dictionary[tail]=source[cursor++];else --available;head=(head+1)&8191;if(available)match_length=search.insert(head,match);}
    }
    writer.bit(0);writer.bits(0,13);packed_length=writer.used;return output;
}
}
