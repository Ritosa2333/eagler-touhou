#include "lifecycle.hpp"
namespace th20::source::title {
TitleInf::TitleInf():files{},state(0),previous_state(0),phase(0),cursor(),cursor70(),cursorbc(),cursor108(),age{},handles{},handle390(0),handles394{},handles3d4{},handles454{},handle474(0),flag478(false),words47c{},data490{},datac90{},data14d0{},word56d0(0),word56d4(0),word56d8(0),value56dc(0),word56e0(0),word56e4(0),cursor56e8(),words5734{},metadata{},music_delay(0),words58d8{},selection_age{},flash_age{},mesh(nullptr),angle(0),wave(0),color{},color5940{},color5950{},previous5960(-1),previous5964(-1),worker(){
    //51d9b0 clears four bits, not the whole word. Factory51d900 zeros storage.
    *reinterpret_cast<std::uint8_t*>(&ui_flags)&=0xf0;
    //40c6b0 called with "initialize TitleInf" is literally a five-byte return
    //helper, not a logging operation; no diagnostic side effect is invented.
    lifecycle_environment().publish(this);
}
TitleInf::~TitleInf(){
    auto& e=lifecycle_environment();
    runtime::join_worker(worker);
    e.remove(update_node);e.remove(draw_node);
    e.unload_file(11);e.unload_file(12);
    for(auto* value:metadata)e.retire(value);
    e.interrupt(handle390);e.release_mesh(mesh);mesh=nullptr;
    e.rebuild_input();e.publish(nullptr);
    //Worker teardown closes/joins again, followed by five Cursors in reverse.
}
int initialize(TitleInf& o,LifecycleEnvironment& e){
    o.update_node=e.register_callback(o,11,false);
    o.draw_node=e.register_callback(o,89,true);
    o.files[0]=e.load_file(11,"title.anm");
    if(!o.files[0]){e.load_error();return -1;}
    o.files[1]=e.load_file(12,"title_v.anm");
    if(!o.files[1]){e.load_error();return -1;}
    o.cursor.wrapping=1;return 0;
}
}
