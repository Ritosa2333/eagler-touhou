    const u32* hosted_replay_status()const{
        static u32 words[6];const auto& s=session;const auto* entry=s.playback.stage(s.state.stage);
        const bool ended=hosted_replay&&s.state.replay&&s.state.phase==GameSessionPhase::paused&&s.pause_menu&&s.pause_menu->forced_exit;
        // A forced menu can be an early replay GameOver. Preserve its real
        // cursor; successful stage clears still include the recorded ending tail.
        const bool early_game_over=ended&&s.battle&&s.battle->game_over_requested;
        words[0]=s.state.stage;words[1]=ended&&entry&&!early_game_over?entry->frames:s.playback.frame();words[2]=entry?entry->frames:0;
        words[3]=ended;words[4]=u32(s.state.phase);words[5]=hosted_replay&&s.state.replay;return words;
    }