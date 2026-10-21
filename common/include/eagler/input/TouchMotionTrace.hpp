#pragma once
#include <array>
#include <cstdint>
#include <cstring>
#include <memory>

namespace eagler::input {
namespace detail {
class TouchMotionTraceRing {
public:
    static constexpr std::uint32_t capacity=512, stride=16, header=8;
    struct Sample {
        std::uint32_t frame=0,mode=0,flags=0;
        float before_x=0,before_y=0,target_x=0,target_y=0,after_x=0,after_y=0;
        float speed=0,scale_x=1,scale_y=1,sensitivity=1;
        std::uint32_t seat=0,execution_kind=0;
    };

    void append(const Sample& s,bool v2){
        auto& row=rows_[total_%capacity];row={};
        row[0]=++total_;row[1]=s.frame;row[2]=s.mode;row[3]=s.flags;
        const float values[]{s.before_x,s.before_y,s.target_x,s.target_y,s.after_x,s.after_y,
                             s.speed,s.scale_x,s.scale_y,s.sensitivity};
        std::memcpy(row.data()+4,values,sizeof(values));
        if(v2){row[14]=s.seat;row[15]=s.execution_kind;}
    }
    const std::uint32_t* snapshot(std::uint32_t version,
                                  std::uint32_t seat=0,
                                  std::uint32_t execution_kind=0)const{
        const auto count=total_<capacity?total_:capacity;
        copy_[0]=version;copy_[1]=header;copy_[2]=stride;copy_[3]=capacity;
        copy_[4]=total_;copy_[5]=count;
        copy_[6]=version==2?seat:0;
        copy_[7]=version==2?execution_kind:0;
        for(std::uint32_t i=0;i<count;++i){
            const auto& row=rows_[(total_-count+i)%capacity];
            std::memcpy(copy_.data()+header+i*stride,row.data(),stride*sizeof(std::uint32_t));
        }
        return copy_.data();
    }
private:
    std::uint32_t total_=0;
    std::array<std::array<std::uint32_t,stride>,capacity> rows_{};
    mutable std::array<std::uint32_t,header+capacity*stride> copy_{};
};
} // namespace detail

// Version 1 remains available to existing packages and keeps its original row
// layout. Its two reserved words stay zero.
class TouchMotionTrace {
public:
    static constexpr std::uint32_t capacity=detail::TouchMotionTraceRing::capacity;
    static constexpr std::uint32_t stride=detail::TouchMotionTraceRing::stride;
    static constexpr std::uint32_t header=detail::TouchMotionTraceRing::header;
    struct Sample {
        std::uint32_t frame=0,mode=0,flags=0;
        float before_x=0,before_y=0,target_x=0,target_y=0,after_x=0,after_y=0;
        float speed=0,scale_x=1,scale_y=1,sensitivity=1;
    };
    void append(const Sample& s){
        detail::TouchMotionTraceRing::Sample row{};
        row.frame=s.frame;row.mode=s.mode;row.flags=s.flags;
        row.before_x=s.before_x;row.before_y=s.before_y;
        row.target_x=s.target_x;row.target_y=s.target_y;
        row.after_x=s.after_x;row.after_y=s.after_y;
        row.speed=s.speed;row.scale_x=s.scale_x;row.scale_y=s.scale_y;
        row.sensitivity=s.sensitivity;
        ring_.append(row,false);
    }
    const std::uint32_t* snapshot()const{return ring_.snapshot(1);}
private:
    detail::TouchMotionTraceRing ring_;
};

// Version 2 isolates continuity by player seat and execution kind. Header words
// 6/7 and row words 14/15 carry that stream identity. Streams are allocated on
// first use; at the 16-stream maximum this costs about 1 MiB (each 512-row ring
// and its linear snapshot buffer). Invalid selectors return null instead of
// silently choosing a different stream.
struct TraceContext {
    std::uint32_t frame=0,execution_kind=0;
    float sensitivity=1;
    bool frame_known=false;
};

class TouchMotionTraceBank {
public:
    using Sample=detail::TouchMotionTraceRing::Sample;
    static constexpr std::uint32_t max_seats=4,execution_kinds=4;
    // Stream kinds: forward, replay playback, rollback resimulation, prediction.
    static constexpr std::uint32_t forward=0,replay=1,resimulation=2,speculative=3;
    // Movement modes: neutral (including keyboard), limited, unlimited.
    static constexpr std::uint32_t mode_neutral=0,mode_limited=1,mode_unlimited=2;
    // Flags identify a completed gameplay tick, duplicate callbacks, the
    // game's actual focused state, and unknown mode/sensitivity/target/frame
    // metadata (bits 0 through 6). Mode and speed describe movement rules.
    static constexpr std::uint32_t flag_gameplay_tick=1u,flag_multiple_callbacks=2u;
    static constexpr std::uint32_t flag_focused=4u,flag_mode_unknown=8u,flag_sensitivity_unknown=16u;
    static constexpr std::uint32_t flag_target_unknown=32u;
    static constexpr std::uint32_t flag_frame_unknown=64u;

    bool append(const Sample& s){
        if(s.seat>=max_seats||s.execution_kind>=execution_kinds)return false;
        const auto index=s.seat*execution_kinds+s.execution_kind;
        if(!streams_[index])streams_[index]=std::make_unique<detail::TouchMotionTraceRing>();
        streams_[index]->append(s,true);
        return true;
    }
    const std::uint32_t* snapshot(std::uint32_t seat,std::uint32_t execution_kind)const{
        if(seat>=max_seats||execution_kind>=execution_kinds)return nullptr;
        const auto index=seat*execution_kinds+execution_kind;
        if(streams_[index])return streams_[index]->snapshot(2,seat,execution_kind);
        empty_[0]=2;empty_[1]=detail::TouchMotionTraceRing::header;
        empty_[2]=detail::TouchMotionTraceRing::stride;
        empty_[3]=detail::TouchMotionTraceRing::capacity;
        empty_[4]=empty_[5]=0;
        empty_[6]=seat;
        empty_[7]=execution_kind;
        return empty_.data();
    }
    void set_context(std::uint32_t frame,std::uint32_t execution_kind,float sensitivity){
        context_={frame,execution_kind,sensitivity,true};
    }
    TraceContext context()const{return context_;}
private:
    std::array<std::unique_ptr<detail::TouchMotionTraceRing>,max_seats*execution_kinds> streams_{};
    mutable std::array<std::uint32_t,detail::TouchMotionTraceRing::header> empty_{};
    TraceContext context_{};
};

inline TouchMotionTraceBank& GlobalTouchMotionTraceBank(){
    static TouchMotionTraceBank bank;
    return bank;
}
} // namespace eagler::input
