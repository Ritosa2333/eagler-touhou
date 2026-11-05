#pragma once
#include "ResultScreen.hpp"
#include "AsciiManager.hpp"
namespace th08 {
class ResultView {
    ResultScreen& result;ResultState& state;ResultContext& context;GameplaySession& session;ScoreStore& scores;
    AsciiManager& ascii;AnmRenderer& renderer;
    template<class... Args>void add(const Vec3& pos,const char* format,Args... args){ascii.add_format(pos,context.software_texturing,format,args...);}
    void high_scores(Vec3 position);void spell_cards(Vec3 position);void keyboard();void replays();
public:
    ResultView(ResultScreen& result,AsciiManager& ascii,AnmRenderer& renderer):result(result),state(result.state),context(result.context),session(result.session),scores(result.scores),ascii(ascii),renderer(renderer){}
    i32 draw();i32 final_statistics();
};
}
