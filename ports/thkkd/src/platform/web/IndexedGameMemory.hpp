#pragma once
#include <memory>
#include <set>

namespace kaikeidou {
class GameStartEntryMemory {
    using U=std::uint32_t;using Q=std::uint64_t;
    struct Binding {U address;std::size_t size;std::uint8_t* data;};
    struct Span {Q first,last;std::vector<std::size_t> bindings;};
    struct Layout {U signature;std::vector<std::pair<U,std::size_t>> schema;std::vector<Span> spans;};
    struct Alias {U address;std::size_t offset,count;};
    struct AliasRun {std::size_t first,count;std::vector<std::uint8_t*> sources;};
    std::vector<Binding> bindings_;
    std::vector<Alias> aliases_;
    std::vector<std::uint8_t*> alias_values_;
    std::vector<std::uint8_t> previous_;
    std::vector<AliasRun> runs_;
    Random& random_;U random_word_;
    bool sealed_=false,runs_dirty_=false;
    U signature_=2166136261u;
    mutable std::shared_ptr<const Layout> layout_;
    mutable std::size_t recent_=0;
    static constexpr std::size_t missing=std::size_t(-1);

    void changed_layout(U address,std::size_t size){signature_=(signature_^address)*16777619u;signature_=(signature_^U(size))*16777619u;layout_.reset();recent_=0;}
    const Layout& layout()const{
        if(layout_)return *layout_;
        // 缓存只保存地址、长度和绑定序号；实际指针始终属于当前实例。
        static std::vector<std::shared_ptr<const Layout>> cache;
        for(const auto& item:cache){
            if(item->signature!=signature_||item->schema.size()!=bindings_.size())continue;
            bool same=true;for(std::size_t i=0;i<bindings_.size();++i)if(item->schema[i]!=std::make_pair(bindings_[i].address,bindings_[i].size)){same=false;break;}
            if(same){layout_=item;return *layout_;}
        }
        auto result=std::make_shared<Layout>();result->signature=signature_;
        struct Event {Q address;std::size_t binding;bool begin;};std::vector<Event> events;
        for(std::size_t i=0;i<bindings_.size();++i){const auto& b=bindings_[i];result->schema.push_back({b.address,b.size});events.push_back({b.address,i,true});events.push_back({Q(b.address)+b.size,i,false});}
        std::sort(events.begin(),events.end(),[](const auto&a,const auto&b){return a.address<b.address;});std::set<std::size_t> active;
        for(std::size_t i=0;i<events.size();){const auto at=events[i].address;std::size_t end=i;while(end<events.size()&&events[end].address==at)++end;
            for(auto j=i;j<end;++j)if(!events[j].begin)active.erase(events[j].binding);
            for(auto j=i;j<end;++j)if(events[j].begin)active.insert(events[j].binding);
            if(end<events.size()&&!active.empty())result->spans.push_back({at,events[end].address,{active.begin(),active.end()}});i=end;
        }
        layout_=result;if(cache.size()==32)cache.erase(cache.begin());cache.push_back(result);return *layout_;
    }
    std::size_t find(Q at)const{
        const auto& spans=layout().spans;
        if(recent_<spans.size()&&spans[recent_].first<=at&&at<spans[recent_].last)return recent_;
        auto it=std::upper_bound(spans.begin(),spans.end(),at,[](Q address,const Span& span){return address<span.first;});
        if(it==spans.begin())return missing;--it;if(at>=it->last)return missing;return recent_=std::size_t(it-spans.begin());
    }
    std::size_t range(U address,std::size_t size,bool writing)const{
        Q at=address,end=at+size;if(end>0x100000000ull)throw GameStartEntryAccessFault{address,U(size),writing};if(at>=end)return missing;
        const auto first=find(at);auto index=first;const auto& spans=layout().spans;
        while(at<end){if(index==missing||index>=spans.size()||spans[index].first>at)throw GameStartEntryAccessFault{U(at),U(end-at),writing};at=spans[index].last;++index;}
        return first;
    }
    void copy_out(U address,std::uint8_t* out,std::size_t size,std::size_t index)const{
        std::size_t done=0;while(done<size){const auto at=Q(address)+done;const auto& span=layout().spans[index];const auto& b=bindings_[span.bindings.front()];
            // 与原实现一致：从当前位置首个注册绑定复制到该绑定末尾。
            const auto count=std::min(size-done,b.size-std::size_t(at-b.address));std::memcpy(out+done,b.data+at-b.address,count);done+=count;if(done<size)index=find(Q(address)+done);
        }
    }
    void build_runs(){
        if(!runs_dirty_)return;runs_.clear();
        for(std::size_t i=0;i<aliases_.size();++i){const auto& alias=aliases_[i];bool extend=!runs_.empty();
            if(extend){const auto& run=runs_.back();extend=alias.count==run.sources.size()&&alias.address==aliases_[i-1].address+1;
                for(std::size_t j=0;extend&&j<alias.count;++j)extend=alias_values_[alias.offset+j]==run.sources[j]+run.count;
            }
            if(extend)++runs_.back().count;else runs_.push_back({i,1,{alias_values_.begin()+alias.offset,alias_values_.begin()+alias.offset+alias.count}});
        }runs_dirty_=false;
    }
    void commit_random(){static_assert(std::is_trivially_copyable_v<Random>&&sizeof(Random)==4,"Original Random cursor representation");if(random_.cursor()!=random_word_)std::memcpy(&random_,&random_word_,4);}
    void accept_aliases(){build_runs();for(const auto& run:runs_)std::memcpy(previous_.data()+run.first,run.sources.front(),run.count);}
    template<class F>void affected(U address,std::size_t size,std::size_t index,F operation){
        if(size&&Q(address)+size<=layout().spans[index].last){for(auto i:layout().spans[index].bindings)operation(bindings_[i]);}
        else for(auto& b:bindings_)operation(b);
    }
public:
    explicit GameStartEntryMemory(Random& random):random_(random),random_word_(random.cursor()){bind(0xa74c08,random_word_);}
    void bind_bytes(U address,void* data,std::size_t size){if(sealed_)throw std::logic_error("Game start mapping already sealed");if(size){bindings_.push_back({address,size,static_cast<std::uint8_t*>(data)});changed_layout(address,size);}}
    template<class T>void bind(U address,T& value){bind_bytes(address,&value,sizeof(value));}
    void bind_runtime_bytes(U address,void* data,std::size_t size){for(const auto& b:bindings_)if(Q(address)<Q(b.address)+b.size&&Q(b.address)<Q(address)+size)throw std::logic_error("Game start allocation overlaps owned memory");if(size){bindings_.push_back({address,size,static_cast<std::uint8_t*>(data)});changed_layout(address,size);}}
    void seal(){
        std::vector<std::uint8_t*> pointers;
        for(const auto& span:layout().spans){if(span.bindings.size()<2)continue;
            for(Q at=span.first;at<span.last;++at){pointers.clear();for(auto i:span.bindings){const auto& b=bindings_[i];pointers.push_back(b.data+at-b.address);}std::sort(pointers.begin(),pointers.end());pointers.erase(std::unique(pointers.begin(),pointers.end()),pointers.end());
                const auto byte=*pointers.front();for(auto* p:pointers)if(*p!=byte)throw std::logic_error("Game start initial aliases disagree");
                runs_dirty_=true;aliases_.push_back({U(at),alias_values_.size(),pointers.size()});previous_.push_back(byte);alias_values_.insert(alias_values_.end(),pointers.begin(),pointers.end());
            }
        }sealed_=true;
    }
    std::vector<std::array<U,2>> regions()const{
        std::vector<std::array<U,2>> input;for(const auto& b:bindings_)input.push_back({b.address,U(b.size)});std::sort(input.begin(),input.end());
        std::vector<std::array<U,2>> out;for(auto r:input){if(!out.empty()&&r[0]<=out.back()[0]+out.back()[1])out.back()[1]=std::max(out.back()[1],r[0]+r[1]-out.back()[0]);else out.push_back(r);}return out;
    }
    void reconcile(){
        if(random_.cursor()!=random_word_)random_word_=random_.cursor();build_runs();
        for(const auto& run:runs_){bool unchanged=true;for(auto* p:run.sources)if(std::memcmp(p,previous_.data()+run.first,run.count)!=0){unchanged=false;break;}if(unchanged)continue;
            for(auto i=run.first;i<run.first+run.count;++i){const auto& alias=aliases_[i];auto value=previous_[i];bool changed=false;
                for(std::size_t j=0;j<alias.count;++j){auto* p=alias_values_[alias.offset+j];if(*p!=previous_[i]){if(changed&&*p!=value)throw std::logic_error("Game start callback aliases disagree");value=*p;changed=true;}}
                if(changed){for(std::size_t j=0;j<alias.count;++j)*alias_values_[alias.offset+j]=value;previous_[i]=value;}
            }
        }commit_random();
    }
    std::vector<std::uint8_t> read(U address,std::size_t size)const{const auto index=range(address,size,false);std::vector<std::uint8_t> out(size);copy_out(address,out.data(),size,index);return out;}
    void write(U address,const std::uint8_t* bytes,std::size_t size){const auto index=range(address,size,true);const Q end=Q(address)+size;affected(address,size,index,[&](const Binding& b){const auto first=std::max(Q(address),Q(b.address)),last=std::min(end,Q(b.address)+b.size);if(first<last)std::memcpy(b.data+first-b.address,bytes+first-address,std::size_t(last-first));});accept_aliases();commit_random();}
    void clear(U address,std::size_t size){const auto index=range(address,size,true);const Q end=Q(address)+size;affected(address,size,index,[&](const Binding& b){const auto first=std::max(Q(address),Q(b.address)),last=std::min(end,Q(b.address)+b.size);if(first<last)std::memset(b.data+first-b.address,0,std::size_t(last-first));});accept_aliases();commit_random();}
    void write(U address,const std::vector<std::uint8_t>& bytes){write(address,bytes.data(),bytes.size());}
    std::uint8_t byte(U address)const{const auto index=range(address,1,false);const auto& b=bindings_[layout().spans[index].bindings.front()];return b.data[address-b.address];}
    U word(U address)const{std::uint8_t bytes[4];const auto index=range(address,4,false);copy_out(address,bytes,4,index);return read_u32(bytes);}
    Q quad(U address)const{return Q(word(address))|(Q(word(address+4))<<32);}
    void word(U address,U value){std::uint8_t bytes[4];for(unsigned i=0;i<4;++i)bytes[i]=std::uint8_t(value>>(8*i));write(address,bytes,4);}
    void quad(U address,Q value){std::uint8_t bytes[8];for(unsigned i=0;i<8;++i)bytes[i]=std::uint8_t(value>>(8*i));write(address,bytes,8);}
    void byte(U address,std::uint8_t value){write(address,&value,1);}
};
}
