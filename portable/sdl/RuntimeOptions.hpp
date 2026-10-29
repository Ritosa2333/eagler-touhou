#pragma once
namespace th12host {
struct RuntimeOptions {
    bool limit_presentation=true,always_hitbox=false,thprac=true;
    int language=0;
    bool music=true;
    bool full_music=false;
};
inline RuntimeOptions runtime_options;
}
