// 与原映射共享同一物理缓冲区，逐操作恢复前态并比较返回、异常和全部写入。
#include "platform/web/WebMemory.hpp"
#include <iostream>
#include <sstream>

using namespace kaikeidou;
using U=std::uint32_t;using Bytes=std::vector<std::uint8_t>;
struct Result {Bytes bytes;std::string error;bool operator==(const Result&r)const{return bytes==r.bytes&&error==r.error;}};
template<class F>Result observe(F action){try{return {action(),{}};}catch(const GameStartEntryAccessFault& f){return {{},"fault:"+std::to_string(f.address)+":"+std::to_string(f.bytes)+":"+std::to_string(f.writing)};}catch(const std::exception& e){return {{},e.what()};}}
template<class T>Bytes encoded(T value){Bytes out(sizeof(value));std::memcpy(out.data(),&value,sizeof(value));return out;}
U seed=0x92746a1;U next(){seed=seed*1664525u+1013904223u;return seed;}
unsigned operations=0;

int main(){try{
    std::vector<std::uint8_t> data(4096);Random random(0);
    ReferenceGameStartEntryMemory reference(random);GameStartEntryMemory indexed(random);
    auto both_bind=[&](U address,std::size_t offset,std::size_t size){reference.bind_bytes(address,data.data()+offset,size);indexed.bind_bytes(address,data.data()+offset,size);};
    both_bind(0x1020,256,64);both_bind(0x1000,0,96);both_bind(0x1010,512,48);both_bind(0x1060,800,64);both_bind(0x1080,1000,16);both_bind(0x1100,1200,32);both_bind(0x1020,256,64);both_bind(0xa74c08,1600,4);both_bind(0xfffffffc,1800,4);
    auto check=[&](auto action){
        const auto before=data;const auto rng=random;const auto expected=observe([&]{return action(reference);});const auto after=data;const auto rng_after=random.cursor();
        std::copy(before.begin(),before.end(),data.begin());random=rng;const auto actual=observe([&]{return action(indexed);});++operations;
        if(!(expected==actual)||data!=after||random.cursor()!=rng_after){std::ostringstream message;message<<"memory difference at operation "<<operations<<" expected="<<expected.error<<" actual="<<actual.error;throw std::runtime_error(message.str());}
    };
    check([](auto& m){m.seal();return Bytes{};});check([](auto& m){m.seal();return Bytes{};});
    for(unsigned i=0;i<30000;++i){
        const auto kind=next()%15;const U value=next(),address=(next()%8==0?0xa74c04u:next()%23==0?0xfffffffau:0xff0u)+(next()%320);const std::size_t length=next()%65;Bytes bytes(length);for(auto& b:bytes)b=std::uint8_t(next());
        switch(kind){
        case 0:check([&](auto&m){return m.read(address,length);});break;
        case 1:check([&](auto&m){return encoded(m.word(address));});break;
        case 2:check([&](auto&m){m.word(address,value);return Bytes{};});break;
        case 3:check([&](auto&m){m.write(address,bytes);return Bytes{};});break;
        case 4:check([&](auto&m){m.clear(address,length);return Bytes{};});break;
        case 5:check([&](auto&m){m.quad(address,std::uint64_t(value)*0x100000001ull);return Bytes{};});break;
        case 6:check([&](auto&m){return encoded(m.quad(address));});break;
        case 7:check([&](auto&m){data[value%1500]=std::uint8_t(value>>16);m.reconcile();return Bytes{};});break;
        case 8:check([&](auto&m){std::memcpy(&random,&value,4);m.reconcile();return Bytes{};});break;
        case 9:check([&](auto&m){data[20]=std::uint8_t(value);data[516]=std::uint8_t(value+1);m.reconcile();return Bytes{};});break;
        case 10:check([&](auto&m){m.byte(address,std::uint8_t(value));return Bytes{};});break;
        case 11:check([&](auto&m){return encoded(m.byte(address));});break;
        case 12:check([](auto&m){Bytes out;for(auto span:m.regions()){auto b=encoded(span);out.insert(out.end(),b.begin(),b.end());}return out;});break;
        case 13:check([&](auto&m){m.bind_runtime_bytes(address,data.data()+2000,length);return Bytes{};});break;
        case 14:check([&](auto&m){m.bind_bytes(address,data.data()+2500,length);return Bytes{};});break;
        }
        if(i%500==0)check([](auto&m){return m.read(0x1000,96);});
    }
    // 超过缓存容量后，已有实例与相同布局的新实例继续使用自身指针。
    for(unsigned i=0;i<80;++i){std::array<std::uint8_t,16> storage{};Random r(0);GameStartEntryMemory m(r);const auto address=0x4000+i*64;m.bind_bytes(address,storage.data(),storage.size());m.seal();m.word(address,i+17);if(m.word(address)!=i+17||read_u32(storage.data())!=i+17)throw std::runtime_error("layout cache pointer mismatch");}
    check([](auto&m){return m.read(0x1000,96);});
    for(U address:{0xa74c08u,0xa74c09u,0xfffffffcu,0xfffffffeu,0xffffffffu}){
        check([&](auto&m){return encoded(m.word(address));});check([&](auto&m){m.word(address,0x12345678);return Bytes{};});check([&](auto&m){m.clear(address,0);return Bytes{};});
    }
    // 首次 seal 在中途失败后，已建立的别名与下一次 seal 的追加顺序保持一致。
    {
        std::array<std::uint8_t,16> shared{};Random rng(0);ReferenceGameStartEntryMemory old(rng);GameStartEntryMemory fast(rng);
        for(auto offset:{0,8}){old.bind_bytes(0x2000,shared.data()+offset,8);fast.bind_bytes(0x2000,shared.data()+offset,8);}shared[12]=1;
        auto compare=[&](auto action){const auto before=shared;const auto saved=rng;const auto expected=observe([&]{return action(old);});const auto after=shared;const auto cursor=rng.cursor();shared=before;rng=saved;const auto actual=observe([&]{return action(fast);});++operations;if(!(expected==actual)||shared!=after||cursor!=rng.cursor())throw std::runtime_error("partial seal mismatch");};
        compare([](auto&m){m.seal();return Bytes{};});shared[12]=0;shared[0]=7;
        compare([](auto&m){m.reconcile();return Bytes{};});compare([](auto&m){m.seal();return Bytes{};});
        shared[8]=9;compare([](auto&m){m.reconcile();return Bytes{};});
    }
    std::cout<<"{\"result\":\"passed\",\"operations\":"<<operations<<",\"layoutInstances\":80}\n";
    return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
