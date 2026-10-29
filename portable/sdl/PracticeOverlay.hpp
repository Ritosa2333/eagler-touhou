#pragma once
#include <array>
#include <cstdint>
#include <vector>
namespace th12host {
class Renderer;
class PracticeOverlay {
    bool initialized=false;unsigned texture=0;
    std::vector<unsigned char> font;
public:
    void initialize(Renderer&);
    void shutdown(Renderer&);
    void key(int code,bool down);
    void clear(bool pointers);
    void mobile(bool enabled,bool visible);
    int pointer(int type,int id,float x,float y,bool desktop=true);
    void wheel(float x,float y,float horizontal,float vertical);
    int tick(int scene,bool ready,bool replay,int difficulty,unsigned held,std::array<int32_t,20>& parameters);
    bool captures()const;
    void render(Renderer&);
};
}
