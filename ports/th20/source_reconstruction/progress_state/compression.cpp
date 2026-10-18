#include "compression.hpp"
#include <algorithm>
#include <stdexcept>
namespace th20::source::progress {
void LzssEncoder::promote(std::uint32_t old_node,std::uint32_t new_node) {
    auto& old=tree_[old_node];auto& next=tree_[new_node];next.parent=old.parent;
    auto& parent=tree_[old.parent];if(parent.right==old_node)parent.right=new_node;else parent.left=new_node;
    old.parent=0;
}
void LzssEncoder::replace(std::uint32_t old_node,std::uint32_t new_node) {
    auto& parent=tree_[tree_[old_node].parent];if(parent.left==old_node)parent.left=new_node;else parent.right=new_node;
    tree_[new_node]=tree_[old_node];tree_[tree_[new_node].left].parent=new_node;tree_[tree_[new_node].right].parent=new_node;tree_[old_node].parent=0;
}
void LzssEncoder::remove(std::uint32_t index) {
    auto& node=tree_[index];if(!node.parent)return;
    if(!node.right)promote(index,node.left);
    else if(!node.left)promote(index,node.right);
    else {auto previous=node.left;while(tree_[previous].right)previous=tree_[previous].right;remove(previous);replace(index,previous);}
}
unsigned LzssEncoder::insert(std::uint32_t index,std::uint32_t& match) {
    if(!index)return 0;
    auto cursor=tree_[8192].right;unsigned longest=0;
    for(;;){unsigned length=0;int comparison=0;
        while(length<18){comparison=int(dictionary_[(index+length)&8191])-int(dictionary_[(cursor+length)&8191]);if(comparison)break;++length;}
        if(longest<=length){longest=length;match=cursor;if(length>=18){replace(cursor,index);return length;}}
        auto& child=comparison<0?tree_[cursor].left:tree_[cursor].right;
        if(child){cursor=child;continue;}
        child=index;tree_[index]={cursor,0,0};return longest;
    }
}
Bytes LzssEncoder::encode(std::span<const std::uint8_t> input) {
    // 5399a0 resets both work areas. 5399d0 installs node1 under sentinel8192.
    dictionary_.fill(0);tree_.fill({0,0,0});
    std::size_t position=std::min<std::size_t>(18,input.size());for(std::size_t i=0;i<position;++i)dictionary_[i+1]=input[i];
    tree_[8192].right=1;tree_[1]={8192,0,0};
    Bytes output;output.reserve(input.size()*2);unsigned accumulated=0,bits=0;
    auto emit=[&](std::uint32_t value,unsigned count){while(count){--count;accumulated=(accumulated<<1)|((value>>count)&1);if(++bits==8){output.push_back(static_cast<std::uint8_t>(accumulated));bits=0;accumulated=0;}}};
    unsigned buffered=static_cast<unsigned>(position),length=0;std::uint32_t cursor=1,match=0;
    while(buffered){length=std::min(length,buffered);unsigned consumed;
        if(length<3){emit(1,1);emit(dictionary_[cursor],8);consumed=1;}
        else {emit(0,1);emit(match,13);emit(length-3,4);consumed=length;}
        for(unsigned i=0;i<consumed;++i){const auto incoming=(cursor+18)&8191;remove(incoming);
            if(position<input.size())dictionary_[incoming]=input[position++];else --buffered;
            cursor=(cursor+1)&8191;if(buffered)length=insert(cursor,match);
        }
    }
    // Original writes only complete bytes of the 14-bit zero terminator. The
    // reader supplies zero bits beyond the declared compressed byte count.
    emit(0,14);return output;
}
void encrypt(Bytes& bytes,CryptParameters parameters) {
    if(!parameters.block||parameters.block>0x7fffffff||parameters.limit>0x7fffffff||bytes.size()>0x7fffffff)throw std::invalid_argument("Unsupported crypt parameters");
    const auto block=static_cast<std::int64_t>(parameters.block);auto remaining=static_cast<std::int64_t>(bytes.size());const auto remainder=remaining%block;
    remaining-=(remaining&1)+(remainder<block/4?remainder:0);auto limit=static_cast<std::int64_t>(parameters.limit);
    if(limit<static_cast<std::int64_t>(bytes.size())&&limit%block)throw std::invalid_argument("Non-aligned partial crypt limit exceeds original temporary buffer");
    const auto source=bytes;std::size_t cursor=0;auto key=parameters.key;
    while(remaining>0&&limit>0){const auto count=std::min(remaining,block);auto output=cursor;
        for(auto input=count-1;input>=0;input-=2){bytes[output++]=source[cursor+static_cast<std::size_t>(input)]^key;key=static_cast<std::uint8_t>(key+parameters.step);}
        for(auto input=count-2;input>=0;input-=2){bytes[output++]=source[cursor+static_cast<std::size_t>(input)]^key;key=static_cast<std::uint8_t>(key+parameters.step);}
        remaining-=count;limit-=block;cursor+=static_cast<std::size_t>(count);
    }
}
}
