// Port of THAdvOptWnd and its shared options from thprac_purple (MIT).
bool show_license=false,show_drop_bar=false;
float boss_range=.5f;
bool opt_open[]={true,true,true,true};
bool begin_group(const Labels& name,bool& open){
    ImGui::SetNextItemOpen(open);
    open=ImGui::CollapsingHeader(label(name));
    if(open)ImGui::Indent();
    return open;
}
void help(const char* text){
    ImGui::TextDisabled("(?)");
    if(ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)){
        ImGui::BeginTooltip();ImGui::PushTextWrapPos(ImGui::GetFontSize()*35);
        ImGui::TextUnformatted(text);ImGui::PopTextWrapPos();ImGui::EndTooltip();
    }
}
bool toggle_tool(const Labels& name,u32 mask){
    bool active=(game->practice_tools&mask)!=0;
    if(!ImGui::Checkbox(label(name),&active))return false;
    game->practice_tools^=mask;return true;
}
void key_options(){
    toggle_tool(practice_TH_ADV_DISABLE_X_KEY,PrNoBomb);ImGui::SameLine();help(label(practice_TH_ADV_DISABLE_X_KEY_DESC));
    ImGui::SameLine();toggle_tool(practice_TH_ADV_DISABLE_SHIFT_KEY,PrNoFocus);ImGui::SameLine();help(label(practice_TH_ADV_DISABLE_SHIFT_KEY_DESC));
    ImGui::SameLine();toggle_tool(practice_TH_ADV_DISABLE_Z_KEY,PrNoShoot);ImGui::SameLine();help(label(practice_TH_ADV_DISABLE_Z_KEY_DESC));
    // TH14 has no native C action and browser events are already normalized.
    bool unavailable=false;
    ImGui::BeginDisabled();ImGui::Checkbox(label(practice_TH_ADV_DISABLE_C_KEY_SAMETIME),&unavailable);ImGui::EndDisabled();
    ImGui::SameLine();toggle_tool(practice_TH_ADV_FORCE_SHIFT_KEY,PrForceFocus);
    ImGui::SameLine();ImGui::BeginDisabled();ImGui::Checkbox("Hook Opt.(test)",&unavailable);ImGui::EndDisabled();
    ImGui::SameLine();help(label(browser_text[1]));
    ImGui::Checkbox(label(practice_THPRAC_FAST_RETRY),&game->purple_fast_retry);
    ImGui::SameLine();help(label(practice_THPRAC_FAST_RETRY_DESC2));
    ImGui::SameLine();toggle_tool(practice_THPRAC_AUTO_SHOOT,PrAutoShoot);
    ImGui::Separator();
}
void key_hud_options(){
    ImGui::Checkbox(label(practice_THPRAC_KB_OPEN),&key_monitor_on);
    if(!key_monitor_on)return;
    if(!key_monitor.g_record_key_aps){
        if(ImGui::Button(label(practice_THPRAC_KB_RECORD_START))){key_monitor.clear_record();key_monitor.g_record_key_aps=true;}
    }else if(ImGui::Button(label(practice_THPRAC_KB_RECORD_STOP)))key_monitor.g_record_key_aps=false;
    ImGui::SameLine();if(ImGui::Button(label(practice_THPRAC_KB_OUTPUT)))export_keys();
    const auto size=key_monitor.g_recorded_aps.size();
    if(size>=2)ImGui::PlotLines("##APS",[](void* ptr,int i){
        const auto& v=*static_cast<eagler::thprac::PracticeKeyMonitor*>(ptr);
        const auto& samples=v.g_recorded_aps;
        return float(samples[samples.size()-std::min<size_t>(600,samples.size())+i]);
    },&key_monitor,int(std::min<size_t>(600,size)),0,nullptr,FLT_MAX,FLT_MAX,{0,ImGui::GetFrameHeight()*3});
}
void about_options(){
    if(!begin_group(practice_TH_ABOUT_THPRAC,opt_open[3]))return;
    ImGui::Text(label(practice_TH_ABOUT_VERSION),purple_version);
    ImGui::TextUnformatted(label(practice_TH_ABOUT_AUTHOR));
    ImGui::TextUnformatted(label(practice_TH_ABOUT_WEBSITE));
    ImGui::NewLine();
    ImGui::PushTextWrapPos();
    ImGui::Text(label(practice_TH_ABOUT_THANKS),"Nylilsa & InceRabbit (for making the video about Marisa's replay desync bug, and for sharing the replay)\n......and you!");
    ImGui::PopTextWrapPos();
    ImGui::NewLine();
    if(ImGui::Button(label(show_license?practice_TH_ABOUT_HIDE_LICENCE:practice_TH_ABOUT_SHOW_LICENCE)))show_license=!show_license;
    if(show_license){
        ImGui::BeginChild("COPYING",{0,384},true);ImGui::TextWrapped("%s",purple_license);ImGui::EndChild();
    }
    ImGui::Unindent();
}
void marisa_laser_options(){
    auto& laser=game->purple_laser;
    ImGui::PushTextWrapPos();
    ImGui::SetNextItemWidth(ImGui::GetIO().DisplaySize.x*.25f);
    const bool locked=on_stage();
    ImGui::BeginDisabled(locked);
    purple_combo(practice_TH14_MODE,laser.mode,purple_TH14_MODE_COMBO);
    ImGui::EndDisabled(locked);
    if(on_stage()){ImGui::SameLine();ImGui::TextUnformatted(label(practice_TH14_LOCKED));}
    const Labels* desc[]={&practice_TH14_MODE_NONE_DESC,&practice_TH14_MODE_NORMAL_DESC,&practice_TH14_MODE_PLAYBACK_DESC,&practice_TH14_MODE_RECOVER_DESC};
    ImGui::TextUnformatted(label(*desc[laser.mode]));
    if(laser.mode==3){
        const bool cannot_save=laser.source.empty()||laser.recovered.empty();
        ImGui::BeginDisabled(cannot_save);
        if(ImGui::Button(label(practice_TH_REPFIX_SAVE))){
            auto result=laser.source;
            append_purple_laser_block(result,laser.recovered);
            EM_ASM({
                const blob=new Blob([HEAPU8.slice($0,$0+$1)],{type:'application/octet-stream'});
                const link=document.createElement('a');link.href=URL.createObjectURL(blob);link.download='th14-recovered.rpy';
                link.click();setTimeout(()=>URL.revokeObjectURL(link.href),1000);
            },result.data(),result.size());
        }
        ImGui::EndDisabled(cannot_save);
        ImGui::SameLine();if(ImGui::Button(label(practice_TH14_LOAD)))laser.merge_playback();
        ImGui::SameLine();if(ImGui::Button(label(practice_TH14_CLEAR_RECORDS)))laser.recovered.clear();
        ImGui::SameLine();
    }
    if(laser.mode>=2){
        if(laser.source.empty())ImGui::TextUnformatted(label(practice_TH_REPFIX_SELECTED_NONE));
        else ImGui::TextUnformatted(label(browser_text[2]));
        if(laser.mode==3)ImGui::Separator();
        auto& records=laser.mode==3?laser.recovered:laser.playback;
        if(records.empty()&&(laser.mode==3||!laser.source.empty()))
            ImGui::TextUnformatted(label(laser.mode==3?practice_TH14_NO_RECORDS:practice_TH14_REPLAY_NO_RECORDS));
        auto it=records.begin();
        int remove=-1;
        // Clip the record table; playback can contain thousands of laser callbacks.
        ImGuiListClipper clipper;
        clipper.Begin(int(records.size()));
        while(clipper.Step()){
            int n=clipper.DisplayStart;
            it=records.begin()+n;
            while(n<clipper.DisplayEnd&&it!=records.end()){
                const auto& r=*it;
                int correction=int(r.fix&0xffff),exponent=int(r.fix>>16);
                ImGui::PushID(n);
                if(laser.mode==2&&(correction==2||correction==3))
                    ImGui::Text(label(practice_TH14_RECORD_E),n+1,r.stage,r.frame,r.value,correction==3?"-":"",exponent);
                else ImGui::Text(label(practice_TH14_RECORD),n+1,r.stage,r.frame,r.value);
                ImGui::SameLine();
                if(laser.mode==3){
                    ImGui::SetNextItemWidth(100);
                    purple_combo(practice_TH14_CORRECTION_VALUE,correction,purple_TH14_CORRECTION);
                    if(correction==2||correction==3){
                        ImGui::SameLine();ImGui::SetNextItemWidth(54);ImGui::InputInt("##e",&exponent,1,1);
                    }
                    it->fix=u32(correction)|(u32(std::clamp(exponent,0,38))<<16);
                    ImGui::SameLine();
                    if(ImGui::Button(label(practice_TH14_DELETE)))remove=n;
                }else if(correction!=2&&correction!=3)ImGui::TextUnformatted(label(*purple_TH14_CORRECTION[correction]));
                ImGui::PopID();
                ++it;
                ++n;
            }
        }
        if(remove>=0)records.erase(records.begin()+remove);
    }
    ImGui::PopTextWrapPos();
}
void advanced_options(){
    ImGui::SetNextWindowPos({0,0},ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(.8f);
    const char* window="AdvOptMenu###th14-purple-advanced";
    if(!modal()){
        ImGui::SetNextWindowSize({0,0});
        Mobile::begin(Mobile::Advanced,window,panel_flags|ImGuiWindowFlags_AlwaysAutoResize,locale);ImGui::End();
        return;
    }
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize,ImGuiCond_Always);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding,0);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize,0);
    if(ImGui::Begin(window,nullptr,panel_flags)){
        ImGui::TextUnformatted(label(practice_TH_ADV_OPT));ImGui::Separator();
        ImGui::BeginChild("Adv. Options");
        if(scroll_advanced!=0.f){ImGui::SetScrollY(ImGui::GetScrollY()-scroll_advanced);scroll_advanced=0.f;}
        if(begin_group(practice_TH_GAME_SPEED,opt_open[0])){
            GameFPSOpt(speed,true,&reset_speed_ui);
            ImGui::Unindent();
        }
        if(begin_group(practice_TH_GAMEPLAY,opt_open[1])){
            key_options();
            key_hud_options();
            toggle_tool(practice_THPRAC_INFLIVES_MAP,PrLivesNoContinue);
            toggle_tool(practice_THPRAC_TH14_FORCE_FIX_BUG,PrMarisaFix);
            ImGui::SameLine();help(label(practice_THPRAC_TH14_FORCE_FIX_BUG_DESC));
            ImGui::Checkbox(label(practice_THPRAC_INGAMEINFO_TH14_SHOW_BONUS),&show_bonus);
            ImGui::Checkbox(label(practice_THPRAC_INGAMEINFO_TH14_SHOW_ITEMS),&show_items);
            ImGui::Checkbox(label(practice_THPRAC_INGAMEINFO_TH14_SHOW_DROP_BAR),&show_drop_bar);
            ImGui::SameLine();help(label(practice_THPRAC_INGAMEINFO_ADV_DESC1));
            ImGui::SameLine();help(label(practice_THPRAC_INGAMEINFO_ADV_DESC2));
            const bool changed=toggle_tool(practice_TH_BOSS_FORCE_MOVE_DOWN,PrBossDown);
            ImGui::SameLine();help(label(practice_TH_BOSS_FORCE_MOVE_DOWN_DESC));
            ImGui::SameLine();ImGui::SetNextItemWidth(90);
            if(ImGui::DragFloat(label(practice_TH_BOSS_FORCE_MOVE_DOWN_RANGE),&boss_range,.002f,0,1)||changed){
                boss_range=std::clamp(boss_range,0.f,1.f);
                game->practice_tools=(game->practice_tools&~PrBossRange)|(u32(std::lround(boss_range*500))<<16);
            }
            ImGui::Checkbox(label(practice_THPRAC_SHOW_BULLET_HITBOX),&show_hitboxes);
            ImGui::Checkbox(label(practice_TH_DISABLE_MASTER),&game->purple_show_history);
            ImGui::SameLine();help(label(practice_TH_DISABLE_MASTER_DESC));
            ImGui::Checkbox(label(practice_TH_ENABLE_LOCK_TIMER),&show_lock_timer);
            toggle_tool(practice_TH_FACTOR_ACB,PrAllClear);
            ImGui::SameLine();help(label(practice_TH_FACTOR_ACB_DESC));
            ImGui::Unindent();
        }
        if(begin_group(practice_TH14_MARISA_LASER,opt_open[2])){marisa_laser_options();ImGui::Unindent();}
        if(ImGui::CollapsingHeader(label(practice_THPRAC_TOOLS_REACTION_TEST)))reaction_test.GuiUpdate(true);
        else reaction_test.Reset();
        about_options();
        ImGui::EndChild();
    }
    ImGui::End();ImGui::PopStyleVar(2);
}
