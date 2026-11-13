// Port of TH14InGameInfo and RenderHitBar from thprac_purple (MIT).
int count_items(){
    int count=0;
    if(const auto* battle=game->battle.get())
        for(u32 n=0;n<ItemManager::ordinary_capacity;++n){
            const auto& item=battle->items.at(n);
            if(item.state&&item.type>=1&&item.type<=3)++count;
        }
    return count;
}
void information(){
    ImGui::SetNextWindowPos({445,280},ImGuiCond_Always);
    ImGui::SetNextWindowSize({180,0});ImGui::SetNextWindowBgAlpha(.9f);
    const auto flags=panel_flags|ImGuiWindowFlags_AlwaysAutoResize|ImGuiWindowFlags_NoInputs|
        ImGuiWindowFlags_NoNav|ImGuiWindowFlags_NoFocusOnAppearing|ImGuiWindowFlags_NoScrollbar;
    const bool standalone=tracker&&!Mobile::visible;
    const bool body=standalone?ImGui::Begin("igi###th14-purple-info",nullptr,flags):
        Mobile::begin(Mobile::Information,"igi###th14-purple-info",flags,locale);
    if(body&&game->battle){
        const bool compact=show_bonus||show_items;
        ImGui::SetWindowFontScale(compact?.75f:1.f);
        const ImVec2 spacing=ImGui::GetStyle().ItemSpacing;
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,{spacing.x,compact?2.f:spacing.y});
        auto& battle=*game->battle;
        char heading[96];
        std::snprintf(heading,sizeof heading,"%s (%s)",label(*purple_IGI_DIFF[game->state.difficulty]),
                      label(*purple_IGI_PL_14[game->state.character*2+game->state.subtype]));
        ImGui::SetCursorPosX(std::max(0.f,(ImGui::GetWindowSize().x-ImGui::CalcTextSize(heading).x)*.5f));
        ImGui::TextUnformatted(heading);
        ImGui::Columns(2);
        auto field=[](const Labels& title,int value){
            ImGui::TextUnformatted(label(title));ImGui::NextColumn();ImGui::Text("%8d",value);ImGui::NextColumn();
        };
        field(practice_THPRAC_INGAMEINFO_MISS_COUNT,battle.th14_practice_misses);
        field(practice_THPRAC_INGAMEINFO_BOMB_COUNT,battle.bomb_count());
        if(show_items)field(practice_THPRAC_INGAMEINFO_TH14_ITEMS_CNT,count_items());
        if(show_bonus){
            const auto& tiers=battle.th14_practice_collections;
            const int cycle=game->economy.th14_life_awards;
            const ImVec4 colors[]={{.6f,.9f,1,1},{.8f,1,.8f,1},{.7f,.9f,.4f,1},{1,.85f,.5f,1},{1,.95f,.5f,1}};
            ImGui::TextUnformatted(label(practice_THPRAC_INGAMEINFO_TH14_BONUS_NORMAL));ImGui::NextColumn();
            for(int i=0;i<4;++i){
                if(i){ImGui::SameLine(0,0);ImGui::TextUnformatted("/");ImGui::SameLine(0,0);}
                ImGui::TextColored(colors[i],"%u",tiers[i]);
            }
            ImGui::NextColumn();ImGui::TextUnformatted(label(practice_THPRAC_INGAMEINFO_TH14_BONUS_20));ImGui::NextColumn();
            ImGui::TextColored(colors[4],"%8u",tiers[4]);ImGui::NextColumn();
            field(practice_THPRAC_INGAMEINFO_TH14_BONUS_TOTAL,cycle);
            ImGui::TextUnformatted(label(practice_THPRAC_INGAMEINFO_TH14_EXTEND_COUNT));ImGui::NextColumn();
            ImGui::TextColored(battle.th14_practice_extends>=49?ImVec4(.5f,.5f,.5f,1):ImVec4(1,1,1,1),"%8u",battle.th14_practice_extends);
            ImGui::NextColumn();ImGui::TextUnformatted(label(practice_THPRAC_INGAMEINFO_TH14_BONUS_NEXT));ImGui::NextColumn();
            for(int i=0;i<5;++i){
                if(i)ImGui::SameLine(0,3);
                const bool next=cycle%5==i;
                const ImVec4 c=i==4?(next?ImVec4(1,.6f,.6f,1):ImVec4(.4f,.2f,.2f,1)):
                    (next?ImVec4(.5f,1,.5f,1):ImVec4(.2f,.4f,.2f,1));
                ImGui::TextColored(c,"%s",label(i==4?practice_THPRAC_INGAMEINFO_TH14_BONUS_LIFE:practice_THPRAC_INGAMEINFO_TH14_BONUS_BOMB));
            }
        }
        ImGui::Columns(1);
        ImGui::PopStyleVar();
    }
    ImGui::End();
}
void drop_bar(){
    const int collecting=game->economy.th14_combo.count;
    const int items=collecting?collecting:count_items();
    const int tier=items<20?0:std::min(5,(items-10)/10);
    constexpr ImU32 colors[]={0xff000000,0xff888888,0xff0000ff,0xff00ffcc,0xff00ff00,0xffffcc00,0xffffff00};
    const float ratio=tier==0?items/20.f:tier==5?1.f:(items-(tier+1)*10)/10.f;
    const auto& position=game->battle->player->motion.state.position;
    const float x=224+position.x-24,y=16+position.y-26;
    auto* p=ImGui::GetOverlayDrawList();
    p->PushClipRect({32,16},{416,464},true);
    p->AddRectFilled({x+.5f,y+.5f},{x+.5f+48*(tier?1.f:ratio),y+4.5f},colors[tier]);
    p->AddRectFilled({x,y},{x+48*ratio,y+4},colors[tier+1]);
    p->AddRect({x,y},{x+48,y+4},collecting?0xffffff00:0xffcccccc);
    p->PopClipRect();
}
