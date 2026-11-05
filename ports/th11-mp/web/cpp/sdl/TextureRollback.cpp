#include "TextureRollback.hpp"
#ifdef TH11_MULTIPLAYER
#include <eagler/netplay/RollbackJournal.hpp>
#include <algorithm>
namespace th11::sdl {
bool TextureRollback::matches(const Write& saved)const noexcept{
    if(!resolve_)return false;const auto* live=resolve_(owner_,saved.handle);
    // Resolve by handle before dereferencing the saved object: it may have
    // been removed illegally. Never touch stale pixel memory during validation.
    return live==saved.image&&live&&live->width==saved.width&&live->height==saved.height&&
        live->pitch==saved.pitch&&live->packed_format==saved.packed_format&&live->format==saved.format&&
        live->pixels.data()==saved.pixels&&live->pixels.size()==saved.size;
}
bool TextureRollback::BeginRollbackFrame(Netplay::RollbackJournal& journal,TextureRollbackFrame& frame){
    if(active_||restore_pending_||frame.history_||!resolve_||!dirty_||!journal.IsFrameOpen()||journal.Failed())return false;
    auto history=history_.lock();if(!history){history=std::make_shared<const unsigned char>(0);history_=history;}
    frame.owner_=this;frame.identity_=identity_;frame.history_=history;frame.frame_=journal.OpenFrame();
    frame.ready_=true;active_=&frame;journal_=&journal;error_="";return true;
}
bool TextureRollback::EndRollbackFrame()noexcept{
    const bool healthy=!active_||(active_->ready_&&journal_&&!journal_->Failed());
    if(active_&&!healthy)active_->ready_=false;
    journal_=nullptr;active_=nullptr;return healthy;
}
bool TextureRollback::BeforeWrite(u32 handle,TextureImage& image){
    if(restore_pending_)return fail("Texture write during rollback restore");
    if(!active_)return history_.expired()||fail("Texture write with rollback history requires an active observer");
    if(!active_->ready_||journal_->Failed())return fail("Texture rollback transaction already failed");
    if(!handle||resolve_(owner_,handle)!=&image||!image.width||!image.height||!image.pitch||
       image.pixels.empty()||u64(image.pitch)*image.height>image.pixels.size())
        return fail("Texture rollback write has invalid live identity or pixel shape");
    const auto found=std::find_if(active_->writes_.begin(),active_->writes_.end(),[&](const auto& value){return value.handle==handle;});
    if(found!=active_->writes_.end())return matches(*found)||fail("Texture backing changed during checkpoint");
    Write saved;saved.handle=handle;saved.image=&image;saved.pixels=image.pixels.data();saved.size=image.pixels.size();
    saved.width=image.width;saved.height=image.height;saved.pitch=image.pitch;saved.packed_format=image.packed_format;saved.format=image.format;
    active_->writes_.push_back(saved);
    if(!journal_->Touch(saved.pixels,saved.size))return fail("Texture pixel preimage exceeds journal capacity");
    return true;
}
bool TextureRollback::ValidateRollbackFrames(const TextureRollbackFrame* const* frames,std::size_t count,u32 restored_frame)const noexcept{
    if(active_||restore_pending_||!frames||!count)return false;bool target=false;
    for(std::size_t i=0;i<count;++i){
        const auto* frame=frames[i];
        if(!frame||!frame->valid()||frame->owner_!=this||frame->identity_.lock()!=identity_||frame->frame_<restored_frame)return false;
        target=target||frame->frame_==restored_frame;
        for(std::size_t j=0;j<i;++j)if(frames[j]==frame||frames[j]->frame_==frame->frame_)return false;
        for(const auto& write:frame->writes_)if(!matches(write))return false;
    }
    return target;
}
bool TextureRollback::PrepareRollbackRestore(const TextureRollbackFrame* const* frames,std::size_t count,u32 restored_frame){
    if(!ValidateRollbackFrames(frames,count,restored_frame))return fail("Texture rollback identity/frontier validation failed");
    prepared_.clear();
    for(std::size_t i=0;i<count;++i)for(const auto& write:frames[i]->writes_)
        if(std::none_of(prepared_.begin(),prepared_.end(),[&](const auto& value){return value.handle==write.handle;}))prepared_.push_back(write);
    restore_pending_=true;error_="";return true;
}
bool TextureRollback::FinishRollbackRestore(){
    if(active_||!restore_pending_)return fail("Texture restore has no prepared transaction");
    for(const auto& write:prepared_)if(!matches(write))return fail("Texture backing changed after restore preparation");
    // Dirty all crossed frames, not merely the target checkpoint. This is a
    // host generation increment; rolling revisions back can reuse stale GPU data.
    for(const auto& write:prepared_)dirty_(owner_,write.handle);
    prepared_.clear();restore_pending_=false;return true;
}
}
#endif
