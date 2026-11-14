// Port of THGuiPrac and THGuiSP from thprac_purple (MIT; see PracticeLicenseTh14.txt).
template<size_t N>
bool purple_combo(const Labels& title,int& choice,const Labels* const (&list)[N],int count=N){
    if(!ImGui::BeginCombo(label(title),label(*list[choice])))return false;
    bool changed=false;
    for(int i=0;i<count;++i){
        ImGui::PushID(i);
        if(ImGui::Selectable(label(*list[i]),i==choice)){changed=choice!=i;choice=i;}
        if(i==choice)ImGui::SetItemDefaultFocus();
        ImGui::PopID();
    }
    ImGui::EndCombo();
    return changed;
}
constexpr int chapters[][2]={{3,2},{3,3},{3,3},{4,3},{4,2},{4,0},{4,5}};
int mode=1,warp=0,chapter=1,spell_setup=-1;
bool section_phase(int count,bool is_spell=false,bool timeout=false){
    if(count<=1)return false;
    if(count==5)return purple_combo(practice_TH_PHASE,params.phase,purple_TH14_SPELL_5PHASE);
    if(count==4)return purple_combo(practice_TH_PHASE,params.phase,purple_TH14_SPELL_4PHASE);
    if(timeout)return is_spell?purple_combo(practice_TH_PHASE,params.phase,purple_TH14_SPELL_PHASE_TIMEOUT):
        purple_combo(practice_TH_PHASE,params.phase,purple_TH_TIMEOUT_SETTING);
    return purple_combo(practice_TH_PHASE,params.phase,purple_TH_SPELL_PHASE1);
}
int selected_spell(){
    if(!on_title()||game->title->screen!=TitleScreen::SpellDifficulty||game->title->substate!=2||
        game->title->th14_spell_phase_choice>=0)return -1;
    const auto& menu=*game->title;
    const int id=menu.th14_selected_spell();
    return (id>=100&&id<=107)||id==119?id:-1;
}
void reset_section(){params.section=params.phase=params.angle=0;params.flags&=12;}
void finish_setup(bool accept){
    if(spell_setup>=0){
        if(!on_title())return;
        if(accept)game->title->th14_spell_phase_choice=params.phase;
        else game->title->th14_spell_phase_choice=-2;
    }else{
        if(!awaiting_setup()||(accept&&mode&&!params.valid(rank)))return;
        if(accept&&mode)game->practice_request=params;
        game->title->th14_practice_stage_choice=accept?params.stage:-1;
    }
    setup=setup_popup=false;
    clear_input();publish();
}
void practice_options(){
    const ImVec2 pos[]={{256,67.2f},{224,79.2f},{236.8f,67.2f}};
    const ImVec2 size[]={{320,388.8f},{384,360},{358.4f,388.8f}};
    ImGui::SetNextWindowPos(spell_setup>=0?ImVec2{224,216}:pos[locale],ImGuiCond_Always);
    ImGui::SetNextWindowSize(spell_setup>=0?ImVec2{243.2f,84}:size[locale],ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(.8f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding,0);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize,0);
    if(ImGui::Begin("Option###th14-purple-practice",nullptr,panel_flags)){
        ImGui::PushItemWidth(spell_setup>=0?-48:locale==2?-67.2f:-64);
        ImGui::TextUnformatted(label(spell_setup>=0?practice_TH_SPELL_PRAC:practice_TH_MENU));ImGui::Separator();
        if(spell_setup>=0)section_phase(spell_setup==119?5:spell_setup>=104?4:2,true);
        else {
            if(purple_combo(practice_TH_MODE,mode,purple_TH_MODE_SELECT)){warp=0;reset_section();}
            int stage=params.stage-1;
            if(purple_combo(practice_TH_STAGE,stage,purple_TH_STAGE_SELECT)){
                params.stage=stage+1;rank=stage==6?4:game->title->selection.difficulty;
                reset_section();params.flags=0;chapter=1;
            }
            if(mode==1){
                if(purple_combo(practice_TH_WARP,warp,purple_TH_WARP_SELECT)){reset_section();chapter=1;}
                if(warp){
                    int fake=(params.flags>>2)&3;
                    if(stage==3&&purple_combo(practice_TH14_FAKE_TYPE,fake,purple_TH14_FAKE_TYPE_LIST))
                        params.flags=(params.flags&3)|(fake<<2);
                    if(warp==1){
                        const auto& counts=chapters[stage];
                        char caption[64];
                        const auto& format=!counts[1]?practice_TH_STAGE_PORTION_N:chapter<=counts[0]?practice_TH_STAGE_PORTION_1:practice_TH_STAGE_PORTION_2;
                        std::snprintf(caption,sizeof caption,label(format),chapter<=counts[0]?chapter:chapter-counts[0]);
                        ImGui::SliderInt(label(practice_TH_CHAPTER),&chapter,1,counts[0]+counts[1],caption);
                        params.section=10000+(stage+1)*100+chapter;
                    }else{
                        std::array<const Item*,std::size(practice_labels)> choices{};
                        size_t count=0,selected=0;
                        for(const auto& item:practice_labels)
                            if(item.stage==stage+1&&*item.names[rank][locale]&&(stage!=3||item.variant==shot+1)&&
                               (warp==2?item.group==1:warp==3?item.group==2:warp==4?!item.spell:item.spell)){
                                if(item.id==params.section)selected=count;
                                choices[count++]=&item;
                            }
                        if(count){
                            auto* item=choices[selected];
                            if(ImGui::BeginCombo(label(*purple_TH_WARP_SELECT[warp]),item->names[rank][locale])){
                                for(size_t i=0;i<count;++i){
                                    ImGui::PushID(choices[i]->id);
                                    if(ImGui::Selectable(choices[i]->names[rank][locale],selected==i)){
                                        reset_section();item=choices[i];
                                    }
                                    if(selected==i)ImGui::SetItemDefaultFocus();
                                    ImGui::PopID();
                                }
                                ImGui::EndCombo();
                            }
                            params.section=item->id;
                            auto check=[](const Labels& text,int bit){bool value=(params.flags&bit)!=0;if(ImGui::Checkbox(label(text),&value))params.flags^=bit;};
                            if(item->dialogue)check(practice_TH_DLG,1);
                            if(item->angle)purple_combo(practice_TH_PHASE,params.phase,purple_TH_MODE_SELECT);
                            else section_phase(item->phases,false,item->timeout);
                            if(item->angle&&params.phase){
                                float angle=params.angle/100.f;
                                if(ImGui::SliderFloat(label(practice_TH_BT_PHASE),&angle,-1.f,1.f,"%.2f"))
                                    params.angle=int(std::lround(angle*100));
                            }
                            if(item->drops)check(practice_TH_DROP_ITEMS,2);
                        }
                    }
                }
                ImGui::SliderInt(label(practice_TH_LIFE),&params.life,0,8);
                ImGui::SliderInt(label(practice_TH_LIFE_FRAGMENT),&params.life_fragment,0,2);
                ImGui::SliderInt(label(practice_TH_BOMB),&params.bomb,0,8);
                ImGui::SliderInt(label(practice_TH_BOMB_FRAGMENT),&params.bomb_fragment,0,7);
                purple_combo(practice_TH_CYCLE,params.cycle,purple_TH14_CYCLE_LIST);
                char power[24];std::snprintf(power,sizeof power,"%.2f",params.power/100.);
                ImGui::SliderInt(label(practice_TH_POWER),&params.power,100,400,power);
                ImGui::DragInt(label(practice_TH_VALUE),&params.value,10,0,999990);
                params.value-=params.value%10;
                ImGui::DragInt(label(practice_TH_GRAZE),&params.graze,1,0,999999);
                i64 score=i64(params.score)*10;const i64 lo=0,hi=9999999990LL;
                if(ImGui::DragScalar(label(practice_TH_SCORE),ImGuiDataType_S64,&score,10,&lo,&hi,"%lld"))params.score=i32(score/10);
            }
        }
        // Native confirm/cancel is also available as tap targets.
        if(ImGui::Button(label(browser_text[0])))finish_setup(true);
        ImGui::SameLine();
        if(ImGui::Button(label(practice_THPRAC_BACK)))finish_setup(false);
        ImGui::PopItemWidth();
    }
    ImGui::End();ImGui::PopStyleVar(2);
    setup_popup=setup&&ImGui::IsPopupOpen(nullptr,ImGuiPopupFlags_AnyPopupId|ImGuiPopupFlags_AnyPopupLevel);
    if(!held[90]&&!held[13])armed=true;
    if(setup&&armed&&(pressed[90]||pressed[13])&&!ImGui::IsAnyItemActive()&&!setup_popup)finish_setup(true);
}
