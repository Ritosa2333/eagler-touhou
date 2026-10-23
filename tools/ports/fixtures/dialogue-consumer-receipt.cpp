#include "DialogueInputReceipt.hpp"
#include <cassert>
#include <cstdint>

namespace th08 {
using u16=std::uint16_t;
struct Context {bool time_stopped=false;u16 input=0;};
struct Scene {u16 input=0,previous_input=0;int hud_redraw=0;};
struct Gui {std::uint32_t frame=0;};
struct Display {struct {int message=0;} dialogue;};
struct Controller {
    Context context;Scene& scene;Gui gui;Display display;
    int snapshots=0,stage_updates=0;
    explicit Controller(Scene& input):scene(input){}
    void snapshot_presentation(){++snapshots;}
    void update_stage(){++stage_updates;}
};
struct Dialogue {
    Controller& controller;int updates=0;
    void update(){++updates;controller.scene.input=0x1234;controller.scene.previous_input=0x4321;}
};
struct GuiFlow {
    Controller& controller;Dialogue& dialogue;
    DialogueInputReceipt consumer_receipt;
    void update();
};
struct ReplayInputState {u16 current=0,previous=0;};
struct GameplayScene {
    struct {struct {u16 buttons=0;} input;struct {u16 previous_buttons=0;} bomb_input;} player_state;
    Scene dialogue_context;
    void publish_input(const ReplayInputState& input);
};

// These placeholders are replaced with the actual production definitions by
// dialogue-consumer-receipt.test.mjs before compilation.
/* GUI_FLOW_UPDATE_SOURCE */
/* GAMEPLAY_PUBLISH_INPUT_SOURCE */
}

int main(){
    using namespace th08;
    GameplayScene game;Controller controller{game.dialogue_context};Dialogue dialogue{controller};GuiFlow flow{controller,dialogue};
    controller.scene.input=1;controller.scene.previous_input=0;
    flow.update();
    assert(dialogue.updates==1);
    assert(flow.consumer_receipt.valid());
    assert(flow.consumer_receipt.current()==1&&flow.consumer_receipt.previous()==0);
    assert(flow.consumer_receipt.sequence()==1);
    // The actual later Replay publisher rewrites DialogueContext for the next
    // tick. Receipt fields retain the sample already passed to this consumer.
    game.publish_input({16,1});
    assert(game.dialogue_context.input==16&&game.dialogue_context.previous_input==1);
    assert(flow.consumer_receipt.current()==1&&flow.consumer_receipt.previous()==0);
    assert(flow.consumer_receipt.sequence()==1);

    controller.context.time_stopped=true;controller.scene.input=1;controller.scene.previous_input=0;
    flow.update();
    assert(dialogue.updates==1&&controller.stage_updates==1);
    assert(!flow.consumer_receipt.valid()&&flow.consumer_receipt.sequence()==1);

    controller.context.time_stopped=false;controller.scene.input=2;controller.scene.previous_input=1;
    flow.update();
    assert(dialogue.updates==2&&flow.consumer_receipt.valid());
    assert(flow.consumer_receipt.current()==2&&flow.consumer_receipt.previous()==1);
    assert(flow.consumer_receipt.sequence()==2);

    controller.display.dialogue.message=-2;controller.scene.input=1;controller.scene.previous_input=0;
    flow.update();
    assert(dialogue.updates==3&&!flow.consumer_receipt.valid());
    assert(flow.consumer_receipt.sequence()==2);
    return 0;
}
