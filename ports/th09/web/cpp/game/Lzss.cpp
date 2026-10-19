#include "Lzss.hpp"
#include <algorithm>
namespace th09 {
bool LzssStream::step(const u8* input,u32 size,u8* output,u32 capacity,u32 budget) noexcept {
    if(complete||failed)return !failed;const u32 start=written;
    auto bit=[&](){if(mask==0x80)byte=cursor<size?input[cursor++]:0;u32 result=bool(byte&mask);mask>>=1;if(!mask)mask=0x80;return result;};
    auto bits=[&](u32 n){u32 value=0;while(n--)value=(value<<1)|bit();return value;};
    auto put=[&](u8 value){output[written++]=value;dictionary[head]=value;head=(head+1)&8191;};
    do{if(bit()){if(written==capacity){failed=true;break;}put(u8(bits(8)));}
        else {const u32 offset=bits(13);if(!offset){complete=true;break;}const u32 count=bits(4)+3;if(count>capacity-written){failed=true;break;}for(u32 n=0;n<count;++n)put(dictionary[(offset+n)&8191]);}
    }while(written-start<budget);return !failed;
}
bool Lzss::decode(const u8* input,u32 size,u8* output,u32 capacity,u32& written) noexcept {
    u32 cursor=0,head=1;u8 mask=0x80,byte=0;written=0;
    auto bit=[&](){if(mask==0x80)byte=cursor<size?input[cursor++]:0;u32 result=bool(byte&mask);mask>>=1;if(!mask)mask=0x80;return result;};
    auto bits=[&](u32 n){u32 value=0;while(n--)value=(value<<1)|bit();return value;};
    auto put=[&](u8 value){output[written++]=value;dictionary[head]=value;head=(head+1)&8191;};
    for(;;){
        if(bit()){if(written==capacity)return false;put(bits(8));}
        else {
            const u32 offset=bits(13);if(!offset)return true;
            const u32 length=bits(4)+3;if(length>capacity-written)return false;
            for(u32 i=0;i<length;++i)put(dictionary[(offset+i)&8191]);
        }
    }
}
std::vector<u8> Lzss::encode(const u8* input,u32 size) {
    std::memset(dictionary,0,sizeof(dictionary));std::memset(tree,0,sizeof(tree));
    std::vector<u8> output;output.reserve(size+size/8+4);
    u8 byte=0,mask=0x80;
    auto bit=[&](bool value){if(value)byte|=mask;mask>>=1;if(!mask){output.push_back(byte);byte=0;mask=0x80;}};
    auto bits=[&](u32 value,u32 count){while(count--)bit(value&(1u<<count));};
    u32 cursor=0,head=1;
    i32 available=std::min<u32>(18,size),length=0,position=0;
    for(i32 i=0;i<available;++i)dictionary[head+i]=input[cursor++];
    tree[8192].right=head;tree[head].parent=8192;
    while(available>0){
        length=std::min(length,available);
        i32 consumed;
        if(length<3){consumed=1;bit(true);bits(dictionary[head],8);}
        else {consumed=length;bit(false);bits(position,13);bits(length-3,4);}
        for(i32 i=0;i<consumed;++i){
            erase((head+18)&8191);
            if(cursor<size)dictionary[(head+18)&8191]=input[cursor++];else --available;
            head=(head+1)&8191;
            if(available)length=add(head,position);
        }
    }
    bit(false);bits(0,13);
    // TH09 original 0x434020 returns complete output bytes only. Decoder zero-fills
    // beyond this size, so the trailing partial all-zero terminator is omitted.
    return output;
}
i32 Lzss::add(i32 node,i32& position) noexcept {
    if(!node)return 0;
    i32 current=tree[8192].right,best=0;
    for(;;){
        i32 count=0,delta=0;
        for(;count<18;++count){delta=i32(dictionary[(node+count)&8191])-dictionary[(current+count)&8191];if(delta)break;}
        if(count>=best){best=count;position=current;if(best>=18){replace(current,node);return best;}}
        i32& child=delta>=0?tree[current].right:tree[current].left;
        if(!child){child=node;tree[node]={current,0,0};return best;}
        current=child;
    }
}
void Lzss::erase(i32 node) noexcept {
    if(!tree[node].parent)return;
    if(!tree[node].right)contract(node,tree[node].left);
    else if(!tree[node].left)contract(node,tree[node].right);
    else {i32 next=tree[node].left;while(tree[next].right)next=tree[next].right;erase(next);replace(node,next);}
}
void Lzss::contract(i32 old_node,i32 new_node) noexcept {
    const i32 parent=tree[old_node].parent;tree[new_node].parent=parent;
    if(tree[parent].right==old_node)tree[parent].right=new_node;else tree[parent].left=new_node;
    tree[old_node].parent=0;
}
void Lzss::replace(i32 old_node,i32 new_node) noexcept {
    const i32 parent=tree[old_node].parent;
    if(tree[parent].left==old_node)tree[parent].left=new_node;else tree[parent].right=new_node;
    tree[new_node]=tree[old_node];
    tree[tree[new_node].left].parent=new_node;tree[tree[new_node].right].parent=new_node;
    tree[old_node].parent=0;
}
}
