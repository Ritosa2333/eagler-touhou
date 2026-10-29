// Port of upstream src/title-presentation.mjs: the title and menu pages as the
// original title.anm / front.anm / text.anm / ascii.anm scripts, steered by
// interrupts from the TitleScenes state.
#pragma once
#include "Scene.hpp"
#include "TitleScenes.hpp"

namespace th12host {
// musiccmt.txt (inside th12.dat): Music Room titles and comments.
std::vector<MusicTrackInfo> parse_music_comments(const std::vector<u8>& bytes);

class TitlePresentation {
public:
    TitlePresentation(Scene& scene,Banks& banks,Profile& profile,std::vector<std::u32string> music_warning);
    void tick(MenuState& state,const TitleScenes& scenes);
    void render(const MenuState& state,const TitleScenes& scenes,float fps);
    bool popup(const MenuState& state)const;
private:
    std::vector<Appearance> render_poses,render_backdrop;std::vector<TextDescriptor> render_texts;
    Scene& scene;Banks& banks;Profile& profile;std::vector<std::u32string> warning;
    AnimationSampler sampler;u32 seed=0x1234;
    // Insertion order, like upstream's Map: it decides draw order within a layer.
    std::vector<std::pair<std::string,std::unique_ptr<MenuAnimation>>> bindings;
    bool has(const std::string& key)const{for(const auto& b:bindings)if(b.first==key)return true;return false;}
    struct Retiring {std::unique_ptr<MenuAnimation> vm;i32 age=0;};std::vector<Retiring> retiring;
    MenuRun run;
    i32 serial=-1;bool intro=false,initial_selection=false,capture_pause=false;MenuScene current=MenuScene::Closed;bool have_scene=false;
    std::optional<i32> heading,pending_popup;
    i32 music_selection=-1;std::string music_comment_key;struct CommentUpdate {const std::vector<std::u32string>* lines;u32 color;i32 age;};std::optional<CommentUpdate> comment_update;
    std::vector<std::u32string> comment_lines;
    MenuAnimation* bind(const std::string& key,i32 script,i32 bank=BankTitle);
    MenuAnimation* get(const std::string& key){for(auto& b:bindings)if(b.first==key)return b.second.get();return nullptr;}
    void remove(const std::string& key);
    void preserve_pane(const std::string& key);
    void select_backdrop();
    void set_heading(std::optional<i32> script);
    void enter(const MenuState& state,const TitleScenes& scenes);
    void select(const MenuState& state,const MenuEvent& event);
    void position_menu_rows(const MenuState& state);
    void confirm_animation(const MenuState& state);
    void update_music(const MenuState& state,const TitleScenes& scenes);
    std::u32string track_title(const TitleScenes& scenes,i32 index)const;
    std::string data_key,data_header_key;
    void update_data(const MenuState& state,bool force=false);
    void texts(const MenuState& state,const TitleScenes& scenes,std::vector<TextDescriptor>& out)const;
};
}
