#pragma once
#include "AnmVm.hpp"
#include "StoryPlayback.hpp"
namespace th19 {
struct DialogueBubble {
 struct Item {AnmVm vm;AnmVm body[4],ruby[4];const StoryPage*page=nullptr;float width=0;int age=0;bool closing=false;};
 std::vector<Item> items;
 const StoryPage* current=nullptr;
 void tick(const StoryPlayback&play,const std::unordered_map<i32,AnmScriptLoc>&scripts,const std::unordered_map<i32,AnmScriptLoc>*texts=nullptr){
  const auto*p=play.bubble?play.page():nullptr;
  if(p!=current){
   for(auto&i:items)if(!i.closing){i.closing=true;i.vm.interrupt_all(1);for(auto&v:i.body)v.interrupt_all(3);for(auto&v:i.ruby)v.interrupt_all(3);}
   current=p;
   if(p){int id=226+p->side+std::clamp(p->bubble_style,0,3)*3+(p->line_count>1?24:0);auto s=scripts.find(id);
    if(s!=scripts.end()){Item i;i.page=p;i.width=std::max(0,((p->text_bytes/2)*16-28)*2)+16.f;
     i.vm.bind_entry(s->second.stream,s->second.len,&scripts);i.vm.tick();
     // Creation executes time 0 only in the native VM. Supply text width before
     // time 1 starts alpha/scale interpolation; do not jump past its fade-in.
     for(size_t n=0;n<i.vm.children.size();++n){auto&c=i.vm.children[n];auto stream=c.stream;auto len=c.stream_len;c={};c.bind_entry(stream,len,&scripts);c.time=-1;c.tick();if(n==0||n==2)c.fvar[4]=i.width;}
     // text.anm 0/1 own the body/ruby reveal and line placement.
     if(texts)for(int line=0;line<4;++line)for(int ruby=0;ruby<2;++ruby){
       auto t=texts->find(ruby);if(t==texts->end())continue;
       auto&v=ruby?i.ruby[line]:i.body[line];v.bind_entry(t->second.stream,t->second.len,texts);v.tick();
       if(line)v.interrupt_returning(line==1?7:line==2?10:11);
       v.interrupt(2);
     }
     items.push_back(std::move(i));
    }
   }
  }
  for(auto&i:items){++i.age;i.vm.tick();for(auto&v:i.body)v.tick();for(auto&v:i.ruby)v.tick();}
  items.erase(std::remove_if(items.begin(),items.end(),[](const Item&i){return !i.vm.visible;}),items.end());
 }
};
}
