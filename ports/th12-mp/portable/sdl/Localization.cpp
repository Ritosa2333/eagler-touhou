#include "BinaryReader.hpp"
#include <unordered_map>
namespace th12host {
namespace {
using Table=std::unordered_map<std::u32string,std::u32string>;
std::unordered_map<std::string,Table> translations;
using DialogueTable=std::unordered_map<std::string,DialogueTranslation>;
std::unordered_map<std::string,DialogueTable> dialogues;
const Table *scoped=nullptr,*global=nullptr;
const DialogueTable* scoped_dialogue=nullptr;
struct MessageEntry {std::vector<DialogueRecords> frames;std::unordered_map<u32,u32> indices;};
std::unordered_map<std::string,std::vector<MessageEntry>> messages;
const std::vector<MessageEntry>* scoped_message=nullptr;
std::string context;
}
std::u32string decode_utf8(const std::string& text){
    std::u32string out;
    for(size_t i=0;i<text.size();){const u8 first=u8(text[i++]);char32_t value=first;unsigned rest=0;
        if(first>=0xc2&&first<=0xdf){value=first&31;rest=1;}else if(first>=0xe0&&first<=0xef){value=first&15;rest=2;}else if(first>=0xf0&&first<=0xf4){value=first&7;rest=3;}
        else if(first>=0x80){out+=U'\ufffd';continue;}
        if(rest>text.size()-i){out+=U'\ufffd';break;}
        bool valid=true;for(unsigned j=0;j<rest;++j){const auto b=u8(text[i]);if((b&0xc0)!=0x80){valid=false;break;}++i;value=(value<<6)|(b&63);}
        if(!valid||value>0x10ffff||(value>=0xd800&&value<=0xdfff)||(rest==1&&value<0x80)||(rest==2&&value<0x800)||(rest==3&&value<0x10000))value=0xfffd;
        out+=value;
    }
    return out;
}
void load_translations(){
    translations.clear();dialogues.clear();messages.clear();context.clear();scoped=global=nullptr;scoped_dialogue=nullptr;scoped_message=nullptr;std::vector<u8> bytes;
    if(!host::read_file(host::paths().data_file("native-translations.bin"),bytes,16*1024*1024))return;
    try{const bool extended=bytes.size()>=8&&!std::memcmp(bytes.data(),"TH12LOC2",8);
        BinaryReader r(bytes);r.magic(extended?"TH12LOC2":"TH12LOC1",8);const u32 count=r.word();if(count>100000)throw std::runtime_error("Too many translated strings");
        for(u32 i=0;i<count;++i){const auto scope=r.text();const auto original=decode_utf8(r.text());const auto replacement=decode_utf8(r.text());translations[scope][original]=replacement;}
        if(extended){const u32 lines=r.word();if(lines>100000)throw std::runtime_error("Too many dialogue strings");
            for(u32 i=0;i<lines;++i){const auto scope=r.text(),source=r.text();DialogueTranslation line;line.text=decode_utf8(r.text());
                const u32 widths=r.word();if(widths!=line.text.size())throw std::runtime_error("Dialogue width count mismatch");
                line.byte_widths.reserve(widths);for(u32 n=0;n<widths;++n){const u32 width=r.word();if(width<1||width>2)throw std::runtime_error("Invalid dialogue width");line.byte_widths.push_back(width);}
                dialogues[scope][source]=std::move(line);
            }
            const u32 files=r.word();if(files>42)throw std::runtime_error("Too many message views");
            for(u32 i=0;i<files;++i){const auto resource=r.text();const u32 entries=r.word();if(entries>256)throw std::runtime_error("Too many message entries");
                auto& views=messages[resource];views.resize(entries);
                for(auto& entry:views){const u32 count=r.word();if(count>4096)throw std::runtime_error("Too many message frames");entry.frames.resize(count);
                    for(auto& frame:entry.frames)for(auto& line:frame){line.bytes=r.text();line.offset=r.integer();line.spacing=r.integer();line.style=r.integer();line.color=r.word();}
                    const u32 indices=r.word();if(indices>4097)throw std::runtime_error("Too many message positions");
                    for(u32 n=0;n<indices;++n){const u32 pc=r.word(),index=r.word();if(index>=count)throw std::runtime_error("Invalid message frame");entry.indices[pc]=index;}
                }
            }
        }
        if(!r.ended())throw std::runtime_error("Trailing translation data");
        translation_context("");
    }catch(const std::exception& e){translations.clear();dialogues.clear();messages.clear();scoped=global=nullptr;scoped_dialogue=nullptr;scoped_message=nullptr;host::log("localization: %s",e.what());}
}
void translation_context(const std::string& resource){
    context=resource;
    const auto table=translations.find(context),fallback=translations.find("");
    scoped=table==translations.end()?nullptr:&table->second;global=fallback==translations.end()?nullptr:&fallback->second;
    const auto dialogue=dialogues.find(context);scoped_dialogue=dialogue==dialogues.end()?nullptr:&dialogue->second;
    const auto message=messages.find(context);scoped_message=message==messages.end()?nullptr:&message->second;
}
const DialogueTranslation* translated_dialogue(const std::string& bytes){
    if(scoped_dialogue)if(const auto found=scoped_dialogue->find(bytes);found!=scoped_dialogue->end())return &found->second;
    return nullptr;
}
const DialogueRecords* translated_message(i32 entry,u32 pc){
    if(!scoped_message||entry<0||size_t(entry)>=scoped_message->size())return nullptr;
    const auto& view=(*scoped_message)[entry];const auto at=view.indices.find(pc);
    return at==view.indices.end()?nullptr:&view.frames[at->second];
}
std::u32string translate_text(const std::u32string& text){
    for(const Table* table:{scoped,global})if(table)if(const auto found=table->find(text);found!=table->end())return found->second;
    return text;
}
}
