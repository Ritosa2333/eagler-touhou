// Real TextRaster + TextureImage + common journal oracle. No SDL/GL emulation.
// CPU pixels are compared exactly, including regions untouched by later text.
#include "../../th11_web/cpp/game/TextRaster.hpp"
#include "../../th11_web/cpp/sdl/TextureRollback.hpp"
#ifdef TH11_MULTIPLAYER
#include <eagler/netplay/RollbackJournal.hpp>
#endif
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <memory>
#include <vector>
namespace {
using namespace th11;
unsigned checks=0,groups=0;
void check(bool ok,const char* why){++checks;if(!ok){std::fprintf(stderr,"FAIL texture group %u check %u: %s\n",groups,checks,why);std::exit(1);}}
void group(const char* name){++groups;std::printf("GROUP %u %s\n",groups,name);}
void load_fonts(TextRaster& raster){
    for(u32 slot=0;slot<6;++slot){
        const char* names[]={"font0.bin","font1.bin","font2.bin","font3.bin","cp932.bin","blend4444.bin"};
        const std::string path=std::string("/fonts/")+names[slot];auto* file=std::fopen(path.c_str(),"rb");
        check(file!=nullptr,"real original font fixture opened");
        check(std::fseek(file,0,SEEK_END)==0,"font size seek");const auto length=std::ftell(file);
        check(length>0&&u64(length)<=32*1024*1024,"bounded font fixture");std::rewind(file);
        std::vector<u8> bytes(static_cast<std::size_t>(length));
        check(std::fread(bytes.data(),1,bytes.size(),file)==bytes.size(),"complete font fixture read");std::fclose(file);
        check(raster.glyphs.load(slot,bytes.data(),u32(bytes.size())),"real GlyphAtlas accepts original fixture");
    }
    check(raster.glyphs.ready(),"all six font dependencies loaded");
}
TextureImage image(u16 format,u8 seed=0){
    AnmTexture source;source.kind=AnmTexture::Kind::Blank;source.width=128;source.height=64;source.format=format;
    TextureImage result;check(result.load(source),"real TextureImage allocation");
    for(std::size_t i=0;i<result.pixels.size();++i)result.pixels[i]=u8(i*37+(i>>7)+seed);
    return result;
}
struct ImageState {
    const TextureImage* object;const u8* pointer;std::size_t capacity,size;
    u32 width,height,pitch;u16 packed;touhou::graphics::PixelFormat format;std::vector<u8> pixels;
    explicit ImageState(const TextureImage& value):object(&value),pointer(value.pixels.data()),capacity(value.pixels.capacity()),size(value.pixels.size()),
        width(value.width),height(value.height),pitch(value.pitch),packed(value.packed_format),format(value.format),pixels(value.pixels){}
    void same(const TextureImage& actual)const{
        check(&actual==object&&actual.pixels.data()==pointer,"original object and pixel allocation identities");
        check(actual.pixels.size()==size&&actual.pixels.capacity()==capacity,"vector ownership metadata unchanged");
        check(actual.width==width&&actual.height==height&&actual.pitch==pitch&&actual.packed_format==packed&&actual.format==format,"complete image scalar metadata unchanged");
        check(actual.pixels==pixels,"entire old CPU texture exact, including untouched regions");
    }
};
const ImageRect first_rect{4,5,100,29},second_rect{20,32,119,58};
const std::string japanese="\x82\xa0\x82\xa2\x82\xa4\x82\xa6"; // authored CP932 Hiragana
TextStyle style(unsigned n=0){TextStyle value;value.font=i32(n%4);value.color=n?0x1357c9:0xffffff;value.outline=0x020304;value.offset=i32(n%3);value.plain=(n%2)!=0;return value;}
void untouched(const ImageState& before,const TextureImage& after,const ImageRect& rect){
    const u32 stride=after.pitch/after.width;bool exact=true;
    for(u32 y=0;y<after.height;++y)for(u32 x=0;x<after.width;++x)
        if(x<u32(rect.left)||x>=u32(rect.right)||y<u32(rect.top)||y>=u32(rect.bottom))
            for(u32 byte=0;byte<stride;++byte){const auto at=std::size_t(y)*after.pitch+x*stride+byte;exact=exact&&before.pixels[at]==after.pixels[at];}
    check(exact,"real text write leaves every byte outside its rectangle untouched");
}
void ordinary(TextRaster& raster){
    group("ordinary real font, raster, resample, packed formats and rejected bounds");
    load_fonts(raster);
    for(u16 format=0;format<6;++format){
        auto texture=image(format,u8(format*11));const ImageState original(texture);
        check(raster.write(texture,first_rect,japanese,style(format)),"real CP932 raster write succeeds");
        check(texture.pixels!=original.pixels,"real raster actually changes pixels");untouched(original,texture,first_rect);
        const ImageState written(texture);check(!raster.write(texture,{-1,0,10,20},japanese,style()),"invalid destination rejected");written.same(texture);
        auto small=style();small.height=8;check(raster.write(texture,second_rect,japanese,small),"original small-height early success");written.same(texture);
    }
}
}
#ifdef TH11_MULTIPLAYER
namespace {
using th11::sdl::TextureRollback;using th11::sdl::TextureRollbackFrame;
struct Host {
    struct Texture {TextureImage image;u32 revision=0,dirty_calls=0;};
    std::map<u32,std::unique_ptr<Texture>> textures;
    static TextureImage* resolve(void* owner,u32 handle){auto& values=static_cast<Host*>(owner)->textures;auto found=values.find(handle);return found==values.end()?nullptr:&found->second->image;}
    static void dirty(void* owner,u32 handle){auto& value=*static_cast<Host*>(owner)->textures.at(handle);++value.revision;++value.dirty_calls;}
    TextureRollback rollback{this,resolve,dirty};
    TextureImage& add(u32 handle,u16 format=0){check(rollback.ResourceChangeAllowed(),"resource allocation only at empty confirmed history");auto value=std::make_unique<Texture>();value->image=image(format,u8(handle));auto& out=value->image;textures.emplace(handle,std::move(value));return out;}
    TextureImage& at(u32 handle){return textures.at(handle)->image;}
    bool write(TextRaster& raster,u32 handle,const ImageRect& rect,const std::string& text,const TextStyle& value){
        auto& target=at(handle);if(!rollback.BeforeWrite(handle,target))return false;
        if(!raster.write(target,rect,text,value))return false;++textures.at(handle)->revision;return true;
    }
};
Netplay::RollbackJournalConfig config(){Netplay::RollbackJournalConfig c;c.maxFrames=8;c.maxBytesPerFrame=2*1024*1024;c.maxBlocksPerFrame=32;return c;}
void setup(Netplay::RollbackJournal& journal){check(journal.Reset(config()),"bounded real common journal configured");}
std::unique_ptr<TextureRollbackFrame> begin(Host& host,Netplay::RollbackJournal& journal,u32 frame){
    auto out=std::make_unique<TextureRollbackFrame>();check(journal.BeginFrame(frame),"common frame begins");
    check(host.rollback.BeginRollbackFrame(journal,*out),"texture observer begins");
    check(out->valid()&&out->frame()==frame&&host.rollback.RollbackFrameActive(),"frame identity and observer state");
    check(!host.rollback.ResourceChangeAllowed(),"active frame prevents resource allocation or release");return out;
}
void end(Host& host,Netplay::RollbackJournal& journal){
    check(host.rollback.EndRollbackFrame(),"healthy observer detaches");check(!host.rollback.RollbackFrameActive(),"observer inactive before journal commit");
    check(journal.EndFrame(),"common frame commits");check(!host.rollback.ResourceChangeAllowed(),"retained checkpoint keeps resource barrier");
}
void undo(Host& host,Netplay::RollbackJournal& journal,std::initializer_list<const TextureRollbackFrame*> frames,u32 target){
    const std::vector<const TextureRollbackFrame*> crossed(frames);check(host.rollback.ValidateRollbackFrames(crossed.data(),crossed.size(),target),"all crossed live identities preflight");
    check(host.rollback.PrepareRollbackRestore(crossed.data(),crossed.size(),target),"prepare complete dirty union before common Undo");
    check(host.rollback.RestorePending(),"prepare retains resource barrier");u32 restored=~0u;
    check(journal.UndoTo(target,&restored)&&restored==target,"all raw pixel owners undo at exact frontier");
    check(host.rollback.FinishRollbackRestore(),"after complete restore notify dirty union");check(!host.rollback.RestorePending(),"restore transaction finished");
}
void first_write(TextRaster& raster){
    group("all packed formats exact whole-atlas preimages and duplicate first-write capture");
    for(u16 format=0;format<6;++format){
        Host host;auto& texture=host.add(3,format);Netplay::RollbackJournal journal;setup(journal);const ImageState original(texture);
        auto checkpoint=begin(host,journal,10);
        check(host.write(raster,3,first_rect,japanese,style(format)),"first real raster captured");
        check(host.write(raster,3,second_rect,"Rollback",style(format+1)),"second disjoint real raster captured");
        check(host.write(raster,3,first_rect,"A",style(format+2)),"shorter replacement keeps original untouched atlas bytes");
        check(checkpoint->texture_count()==1&&journal.BlocksForFrame(10)==1,"one unique texture and one whole-pixel journal block");
        check(journal.BytesForFrame(10)==texture.pixels.size(),"exact CPU bytes only, no vector header or GPU generation");
        check(texture.pixels!=original.pixels,"test modifies original pixels");end(host,journal);
        check(!host.write(raster,3,first_rect,"illegal",style()),"retained history requires active observer for writes");
        undo(host,journal,{checkpoint.get()},10);original.same(texture);
        check(host.textures.at(3)->revision==4&&host.textures.at(3)->dirty_calls==1,"restore increments current host revision instead of rewinding it");
        checkpoint.reset();check(host.rollback.ResourceChangeAllowed(),"last frame lease release admits resource boundary");
    }
}
void multi_frame(TextRaster& raster){
    group("all crossed checkpoints dirty their union and sequential rewind preserves intermediate text");
    Host host;auto& a=host.add(3);auto& b=host.add(4,5);auto& c=host.add(5,4);
    const ImageState initial_a(a),initial_b(b),initial_c(c);Netplay::RollbackJournal journal;setup(journal);
    auto first=begin(host,journal,20);check(host.write(raster,3,first_rect,japanese,style()),"oldest frame writes A");end(host,journal);
    auto second=begin(host,journal,21);check(host.write(raster,4,second_rect,"B",style(1)),"middle frame writes only B");end(host,journal);
    auto third=begin(host,journal,22);check(host.write(raster,3,second_rect,"A2",style(2)),"newest frame also writes A");check(host.write(raster,5,first_rect,"C",style(3)),"newest frame only owner of C");end(host,journal);
    undo(host,journal,{first.get(),second.get(),third.get()},20);initial_a.same(a);initial_b.same(b);initial_c.same(c);
    for(u32 handle:{3u,4u,5u})check(host.textures.at(handle)->dirty_calls==1,"every crossed handle dirtied exactly once including absent from target frame");
    check(host.textures.at(3)->revision==3&&host.textures.at(4)->revision==2&&host.textures.at(5)->revision==2,"GPU generations preserve all prior writes then advance");
    third.reset();second.reset();first.reset();check(host.rollback.ResourceChangeAllowed(),"complete crossed history released only after typed restore");
    first=begin(host,journal,30);check(host.write(raster,3,first_rect,"first",style()),"new oldest text");end(host,journal);const ImageState middle(a);
    second=begin(host,journal,31);check(host.write(raster,3,second_rect,"second",style(1)),"newer text");end(host,journal);
    undo(host,journal,{second.get()},31);middle.same(a);second.reset();
    undo(host,journal,{first.get()},30);initial_a.same(a);first.reset();
}
void admission(TextRaster& raster){
    group("closed journal, active observers, wrong owner/frontier, duplicate and consumed frame admission");
    Host host,foreign;host.add(3);foreign.add(3);Netplay::RollbackJournal journal;setup(journal);TextureRollbackFrame unused;
    check(!host.rollback.BeginRollbackFrame(journal,unused)&&!unused.valid(),"closed journal rejects observer");
    check(!host.rollback.FinishRollbackRestore(),"unprepared finish rejected");auto checkpoint=begin(host,journal,40);
    check(!host.rollback.BeginRollbackFrame(journal,unused)&&!unused.valid(),"duplicate Begin rejected without attaching unrelated frame");
    const TextureRollbackFrame* frames[]={checkpoint.get()};
    check(!host.rollback.ValidateRollbackFrames(frames,1,40),"active observer refuses pre-Undo validation");
    check(host.write(raster,3,first_rect,"live",style()),"valid write after admission rejection");end(host,journal);
    check(!foreign.rollback.ValidateRollbackFrames(frames,1,40),"foreign owner rejects frame");
    check(!host.rollback.ValidateRollbackFrames(nullptr,0,40),"empty crossed list rejected");
    check(!host.rollback.ValidateRollbackFrames(frames,1,39)&&!host.rollback.ValidateRollbackFrames(frames,1,41),"missing exact target and older checkpoint rejected");
    const TextureRollbackFrame* twice[]={checkpoint.get(),checkpoint.get()};
    check(!host.rollback.ValidateRollbackFrames(twice,2,40),"duplicate crossed checkpoint rejected");
    check(journal.BeginFrame(41),"new common frame begins");check(!host.rollback.BeginRollbackFrame(journal,*checkpoint),"already consumed frame cannot Begin again");
    check(host.rollback.BeginRollbackFrame(journal,unused),"fresh frame can attach after consumed-frame refusal");
    end(host,journal);undo(host,journal,{checkpoint.get(),&unused},40);
}
void identity(TextRaster& raster){
    group("all identity and pixel-shape checks reject before raw Undo without dereferencing retired memory");
    Host host;auto& target=host.add(3);Netplay::RollbackJournal journal;setup(journal);const ImageState original(target);
    auto checkpoint=begin(host,journal,50);check(host.write(raster,3,first_rect,"identity",style()),"capture live texture");end(host,journal);
    const TextureRollbackFrame* frames[]={checkpoint.get()};
    auto valid=[&](){return host.rollback.ValidateRollbackFrames(frames,1,50);};
    auto saved=std::move(host.textures.at(3));host.textures.erase(3);check(!valid(),"missing handle fails lookup before stale object dereference");
    auto impostor=std::make_unique<Host::Texture>();impostor->image=image(0);host.textures.emplace(3,std::move(impostor));
    check(!valid(),"same handle and shape but different TextureImage object rejected");host.textures.erase(3);host.textures.emplace(3,std::move(saved));check(valid(),"original live object restored");
    std::vector<u8> alternate=target.pixels;target.pixels.swap(alternate);check(!valid(),"different backing allocation rejected with old allocation still alive");
    target.pixels.swap(alternate);check(valid(),"original backing reinstated before any Undo");const auto size=target.pixels.size();
    target.pixels.pop_back();check(!valid(),"changed vector size rejected");target.pixels.resize(size);
    ++target.width;check(!valid(),"width drift rejected");--target.width;
    ++target.height;check(!valid(),"height drift rejected");--target.height;
    ++target.pitch;check(!valid(),"pitch drift rejected");--target.pitch;
    ++target.packed_format;check(!valid(),"packed format drift rejected");--target.packed_format;
    const auto format=target.format;target.format=touhou::graphics::PixelFormat::Bgr8;check(!valid(),"pixel format drift rejected");target.format=format;
    check(valid(),"entire live identity restored before raw byte undo");undo(host,journal,{checkpoint.get()},50);original.same(target);
}
void capacity(TextRaster& raster){
    group("byte/block exhaustion and another owner failure invalidate complete texture frame");
    for(unsigned mode=0;mode<3;++mode){
        Host host;auto& a=host.add(3);auto& b=host.add(4);const ImageState original_a(a),original_b(b);
        Netplay::RollbackJournal journal;auto settings=config();if(mode==0)settings.maxBytesPerFrame=1;else settings.maxBlocksPerFrame=1;
        check(journal.Reset(settings),"explicit bounded failure configuration");auto checkpoint=begin(host,journal,60+mode);
        if(mode==0)check(!host.write(raster,3,first_rect,"blocked",style()),"insufficient bytes prevent raster write");
        else{
            check(host.write(raster,3,first_rect,"A",style()),"first block succeeds");
            if(mode==1)check(!host.write(raster,4,second_rect,"blocked",style()),"block exhaustion prevents second texture write");
            else {u32 independent=123;check(!journal.Touch(&independent,sizeof independent),"independent owner fails common frame");}
        }
        check(journal.Failed(),"common journal marks failure");check(!host.rollback.EndRollbackFrame()&&!checkpoint->valid(),"End invalidates frame even when another owner caused failure");
        check(!journal.EndFrame(),"failed common frame cannot commit");const TextureRollbackFrame* frames[]={checkpoint.get()};
        check(!host.rollback.ValidateRollbackFrames(frames,1,60+mode),"invalid complete transaction cannot enter restore");
        original_b.same(b);if(mode==0)original_a.same(a);
        journal.Clear();checkpoint.reset();check(host.rollback.ResourceChangeAllowed(),"explicit failure cleanup releases resource lease");
    }
}
void resource_and_prepare(TextRaster& raster){
    group("empty-frame leases, prepare lock, cancel, late identity drift and owner expiry");
    Host host;auto& target=host.add(3);Netplay::RollbackJournal journal;setup(journal);
    auto empty=begin(host,journal,70);end(host,journal);check(empty->texture_count()==0,"zero text writes still form a valid history boundary");
    auto checkpoint=begin(host,journal,71);check(host.write(raster,3,first_rect,"prepared",style()),"real text before prepare");end(host,journal);
    const TextureRollbackFrame* frames[]={empty.get(),checkpoint.get()};
    check(host.rollback.PrepareRollbackRestore(frames,2,70),"prepare zero and nonzero checkpoint union");
    check(!host.rollback.ResourceChangeAllowed()&&!host.write(raster,3,second_rect,"blocked",style()),"prepared restore rejects resources and CPU writes");
    TextureRollbackFrame another;check(!host.rollback.BeginRollbackFrame(journal,another),"prepared restore rejects new observer");
    ++target.width;check(!host.rollback.FinishRollbackRestore()&&host.rollback.RestorePending(),"late identity change refuses dirty and retains preparation");
    --target.width;check(host.textures.at(3)->dirty_calls==0,"failed finish cannot dirty a partial union");
    host.rollback.CancelRollbackRestore();check(!host.rollback.RestorePending(),"explicit cancellation clears prepared transaction only");
    undo(host,journal,{empty.get(),checkpoint.get()},70);checkpoint.reset();check(!host.rollback.ResourceChangeAllowed(),"old empty frame retains global resource boundary");
    empty.reset();check(host.rollback.ResourceChangeAllowed(),"all typed frame leases finally retired");
    TextureRollbackFrame expired;Netplay::RollbackJournal other_journal;setup(other_journal);
    {Host temporary;temporary.add(7);check(other_journal.BeginFrame(0),"temporary owner's common frame");check(temporary.rollback.BeginRollbackFrame(other_journal,expired),"temporary observer");
        check(temporary.rollback.EndRollbackFrame()&&other_journal.EndFrame(),"temporary checkpoint closes");check(expired.valid(),"live temporary owner validates frame");}
    check(!expired.valid(),"owner identity expiration invalidates retained frame");other_journal.Clear();
}
void failed_raster(TextRaster& raster){
    group("raster failure and native no-op preserve valid old whole-texture preimages");
    Host host;auto& target=host.add(3);Netplay::RollbackJournal journal;setup(journal);const ImageState original(target);
    auto checkpoint=begin(host,journal,80);auto invalid=style();invalid.height=40;
    check(!host.write(raster,3,first_rect,"too tall",invalid),"real scratch height failure reaches host without pixel write");original.same(target);
    auto small=style();small.height=8;check(host.write(raster,3,first_rect,"native no-op",small),"real early return keeps original semantics");original.same(target);
    check(host.write(raster,3,second_rect,japanese,style(3)),"later valid raster uses same first preimage");
    check(checkpoint->texture_count()==1&&journal.BlocksForFrame(80)==1,"failed and no-op raster never replace first preimage");
    end(host,journal);undo(host,journal,{checkpoint.get()},80);original.same(target);
}
}
#endif
int main(){
    TextRaster raster;ordinary(raster);
#ifdef TH11_MULTIPLAYER
    first_write(raster);multi_frame(raster);admission(raster);identity(raster);capacity(raster);resource_and_prepare(raster);failed_raster(raster);
    std::printf("PASS texture rollback multiplayer: %u groups, %u checks\n",groups,checks);
#else
    std::printf("PASS texture ordinary: %u groups, %u checks\n",groups,checks);
#endif
    return 0;
}
