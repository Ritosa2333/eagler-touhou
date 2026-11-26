#pragma once
#include "replay_session.hpp"
#include "shot_input.hpp"

namespace soew {
// Ordered physical masks, not host key events or a prediction of a remote
// seat. P1 alone controls the original shared dialogue and Continue prompts.
inline constexpr std::uint16_t coop_play_mask=Up|Down|Left|Right|Shot|Bomb|Cancel|OK|Focus;

inline std::uint16_t sample_coop_seat(LocalCoopState &coop,unsigned seat,ShotInput &fire,
                                     std::uint16_t raw,ShotInputMode mode){
    const bool held=(raw&Shot)!=0;
    fire.event(held,mode);
    coop.fire_held[seat]=held;
    return std::uint16_t((raw&~Shot)|(fire.tick(mode)?Shot:0));
}

struct ConfirmedCoop {
    std::array<ShotInput,3> fire;
    unsigned frame=0;
    std::uint32_t seed=1234;
    bool focus=true,autofire=true,restrict_speed=false,finished=false;

    // This clock is independent of painting, focus, throttling and the PC's
    // timer. A stalled transport never calls step with a fabricated row.
    ReplayStep step(EncounterScene &scene,ReplaySession &session,unsigned at,
                    const std::array<std::uint64_t,3> &masks){
        auto &coop=*scene.coop;
        if(at!=frame||finished)throw std::runtime_error("TH02 confirmed frame out of sequence");
        for(unsigned seat=0;seat<masks.size();++seat){
            if((masks[seat]&4095&~std::uint64_t(coop_play_mask))||(seat>=coop.config.count&&masks[seat]))
                throw std::runtime_error("Invalid TH02 confirmed seat input");
        }
        const auto context=replay_tick_context(scene);
        const auto mode=shot_input_mode(autofire,false,context!=NativeReplayContext::Gameplay);
        scene.sound.shot_cadence.set_autofire(autofire);
        for(unsigned seat=0;seat<coop.config.count;++seat){
            auto raw=std::uint16_t(masks[seat]&4095);
            coop.pointer[seat].sample(context==NativeReplayContext::Gameplay&&scene.seat_life(seat).input_allowed()?masks[seat]:0,restrict_speed);
            if(!focus)raw&=~Focus;
            coop.input[seat]=sample_coop_seat(coop,seat,fire[seat],raw,mode);
        }
        const auto group=(masks[0]&(Shot|Bomb|OK|Cancel))!=0;
        auto out=session.tick(scene,normalize_replay_tick(coop.input[0],group,false));
        if(out.consumed_tick)++frame;
        if(out.stage_advanced)fire={};
        finished=out.status!=ReplayStepStatus::Running;
        return out;
    }

    // Add the cooperative seats and fractional damage to the existing solo
    // digest without changing the replay file contract.
    std::uint64_t digest(const EncounterScene &scene)const{
        replay_session_detail::Digest h;
        const auto &coop=*scene.coop;
        h.fields(frame,seed,focus,autofire,restrict_speed,replay_state_digest(scene),coop.input,coop.previous,
                 coop.bomb_owner,coop.half_power,coop.fire_held,coop.hit_clear,
                 scene.midboss.damage_taken.sixths(),scene.midboss2.damage_taken.sixths(),
                 scene.midboss4.damage_taken.sixths(),scene.rika.damage_taken.sixths(),
                 scene.meira.damage_taken.sixths(),scene.marisa.damage_taken.sixths(),
                 scene.mima.damage_taken.sixths(),scene.sigma.damage_taken.sixths());
        for(const auto &a:scene.midboss3.actor)h.add(a.damage.sixths());
        for(const auto &stone:scene.stones.stones)h.add(stone.damage.sixths());
        const auto &marisa=scene.marisa;
        h.fields(marisa.shielded_selections,marisa.exposed_selections);
        for(const auto &orb:marisa.orbs)
            h.fields(orb.active,orb.dying,orb.x,orb.y,orb.angle,orb.radius,orb.angular_step,orb.hits.sixths(),orb.death_frame);
        for(unsigned seat=0;seat<coop.config.count;++seat){
            const auto &p=scene.seat_motion(seat);const auto &l=scene.seat_life(seat);
            const auto &s=scene.seat_shots(seat);const auto &k=scene.seat_pickups(seat);
            const auto &r=coop.rescue[seat];const auto &f=fire[seat];
            const auto &pointer=coop.pointer[seat];h.fields(pointer.active,pointer.unlimited,pointer.x,pointer.y);
            h.fields(p.x,p.y,p.shot,p.back_page,p.frame,p.pose,p.last_dx,p.last_dy,p.focused,p.focus_rx,p.focus_ry,
                     l.lives,l.bombs,l.invincibility,l.miss_frame,l.hit,l.miss_active,l.bomb_invincible,l.game_over,
                     k.power,k.overflow,k.points_collected,k.collected,r.presses,r.age,r.target,
                     f.physical,f.sampled,f.blocked,f.phase,f.edges.size(),s.fired,s.hit_events,s.damage_events,
                     s.burst_a,s.burst_b,s.cool_a,s.cool_b,s.counter_c,s.spread,s.option_lock,s.target_x,s.target_y);
            for(bool edge:f.edges)h.add(edge);
            for(const auto &pos:l.positions)h.fields(pos.x,pos.y);
            for(const auto &slot:s.slots){
                h.add(slot.flag);
                if(slot.flag)h.fields(slot.type,slot.pattern,slot.special,slot.age,slot.position,slot.vx,slot.vy);
            }
        }
        return h.value;
    }
};
}
