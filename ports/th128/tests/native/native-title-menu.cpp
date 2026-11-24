#include "../../src/runtime/Exports.cpp"
#include <cassert>
#include <cstring>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

static std::vector<unsigned char> read(const char* path) {
    std::ifstream file(path, std::ios::binary);
    assert(file);
    return {std::istreambuf_iterator<char>(file), {}};
}

static bool hasNode(int owner, int script) {
    for (const auto& [identity, node] : scene.nodes())
        if (int(node->owner) == owner && node->alive && node->vm.state().script == script) return true;
    return false;
}

static th128::anm::Node* node(int owner, int script) {
    for (const auto& [identity, value] : scene.nodes())
        if (int(value->owner) == owner && value->alive && value->vm.state().script == script) return value.get();
    return nullptr;
}

static bool nearAngle(float actual, float expected) {
    constexpr float kPi = 3.14159265358979323846f;
    float difference = actual - expected;
    while (difference > kPi) difference -= 2 * kPi;
    while (difference < -kPi) difference += 2 * kPi;
    return std::fabs(difference) < 0.02f;
}

static bool alphaToWhiteActive(th128::anm::Node* card) {
    if (!card) return false;
    const auto diagnostic = card->vm.diagnostics();
    for (const auto& track : diagnostic.interpolations) {
        if (!track.active) continue;
        for (std::size_t index = 0; index < track.fields.size(); ++index)
            if (track.fields[index] == 10 && index < track.from.size() && index < track.to.size()
                && track.from[index] == 0 && track.to[index] == 255)
                return true;
    }
    return false;
}

static int drawCount() {
    const int count = th128_draw_count();
    assert(errorText.empty());
    return count;
}

static const DrawRecord* drawScript(int script) {
    for (const auto& record : draws)
        if (record.script == script) return &record;
    return nullptr;
}

static bool stateHas(const char* fragment) {
    return std::strstr(th128_title_menu_state(), fragment) != nullptr;
}

static void tick(int count) {
    for (int index = 0; index < count; ++index)
        assert(th128_tick() == 1);
}

int main(int argc, char** argv) {
    assert(argc == 3);
    const auto title = read(argv[1]);
    const auto version = read(argv[2]);
    assert(th128_load_anm(24, title.data(), std::uint32_t(title.size())) == 1);
    assert(th128_load_anm(25, version.data(), std::uint32_t(version.size())) == 1);
    assert(th128_boot_title(0x1234) == 1);
    tick(72);
    assert(stateHas("\"page\":0"));
    assert(stateHas("\"ready\":1"));
    assert(stateHas("\"supportedPages\":[0,1,2]"));

    assert(th128_title_menu_select(5) == 1);
    assert(selection == 5);
    assert(th128_title_menu_select(6) == 1);
    assert(selection == 6);
    assert(hasNode(3, 8) && hasNode(3, 15));
    assert(hasNode(3, 9) && hasNode(3, 16));

    assert(th128_title_menu_page(3, 0) == 0);
    assert(stateHas("unsupported title menu page"));
    assert(stateHas("\"page\":0"));

    assert(th128_title_menu_page(1, 2) == 1);
    assert(stateHas("\"page\":1"));
    assert(stateHas("\"transitioning\":1"));
    assert(stateHas("\"ready\":0"));
    assert(th128_title_menu_page(2, 0) == 0);
    assert(stateHas("transition is still in progress"));
    tick(11);
    assert(!hasNode(3, 0) && !hasNode(3, 10));
    assert(hasNode(10, 129));
    for (int script = 116; script <= 123; ++script) assert(hasNode(10, script));
    assert(hasNode(10, 131));
    tick(9);
    assert(stateHas("\"page\":1"));
    assert(stateHas("\"ready\":1"));
    assert(selection == 2);
    assert(hasNode(10, 116) && hasNode(10, 119));
    assert(node(10, 131) && nearAngle(node(10, 131)->vm.state().rotation, 3.14159265f));
    assert(node(10, 120) && nearAngle(node(10, 120)->vm.state().rotation, 3.14159265f));
    auto* page1Root = node(10, 129);
    assert(page1Root && page1Root->vm.state().visible);
    assert(page1Root->vm.state().sprite == -1 && page1Root->vm.state().spriteBank < 0);
    assert(page1Root->vm.state().geometryCount == 0);
    assert(drawCount() > 0);
    const auto* page1Card = drawScript(118);
    assert(page1Card && std::fabs(page1Card->x - 320) < 0.5f && std::fabs(page1Card->y - 270) < 0.5f);
    assert(drawScript(131));
    assert(!drawScript(129));

    const auto oldHardRevision = node(10, 118)->vm.presentationRevision();
    assert(th128_title_menu_select(0) == 1);
    tick(1);
    assert(alphaToWhiteActive(node(10, 116)));
    assert(node(10, 118)->vm.presentationRevision() > oldHardRevision);
    assert(node(10, 131) && (node(10, 131)->vm.continuousFields() & 2));
    tick(12);
    assert(nearAngle(node(10, 131)->vm.state().rotation, 0));
    assert(nearAngle(node(10, 120)->vm.state().rotation, 0));
    assert(!alphaToWhiteActive(node(10, 116)));
    assert(drawCount() > 0 && drawScript(116));

    const auto oldEasyRevision = node(10, 116)->vm.presentationRevision();
    assert(th128_title_menu_select(3) == 1);
    tick(1);
    assert(alphaToWhiteActive(node(10, 119)));
    assert(node(10, 116)->vm.presentationRevision() > oldEasyRevision);
    tick(12);
    assert(nearAngle(node(10, 131)->vm.state().rotation, -1.5707963f));
    assert(nearAngle(node(10, 120)->vm.state().rotation, -1.5707963f));
    assert(!alphaToWhiteActive(node(10, 119)));
    assert(drawCount() > 0 && drawScript(119));

    assert(th128_title_menu_page(2, 1) == 1);
    tick(20);
    assert(stateHas("\"page\":2"));
    assert(stateHas("\"ready\":1"));
    assert(selection == 1);
    assert(hasNode(11, 130));
    for (int script = 124; script <= 128; ++script) assert(hasNode(11, script));
    assert(node(11, 131) && nearAngle(node(11, 131)->vm.state().rotation, 1.5707963f));
    assert(node(11, 120) && nearAngle(node(11, 120)->vm.state().rotation, 1.5707963f));
    auto* page2Root = node(11, 130);
    assert(page2Root && page2Root->vm.state().visible);
    assert(page2Root->vm.state().sprite == -1 && page2Root->vm.state().spriteBank < 0);
    assert(page2Root->vm.state().geometryCount == 0);
    assert(drawCount() > 0);
    const auto* page2Card = drawScript(125);
    assert(page2Card && std::fabs(page2Card->x - 430) < 0.5f && std::fabs(page2Card->y - 270) < 0.5f);
    assert(drawScript(131));
    assert(!drawScript(130));

    const auto oldMediumRevision = node(11, 125)->vm.presentationRevision();
    assert(th128_title_menu_select(2) == 1);
    tick(1);
    assert(alphaToWhiteActive(node(11, 126)));
    assert(node(11, 125)->vm.presentationRevision() > oldMediumRevision);
    tick(12);
    assert(nearAngle(node(11, 131)->vm.state().rotation, 3.14159265f));
    assert(nearAngle(node(11, 120)->vm.state().rotation, 3.14159265f));
    assert(!alphaToWhiteActive(node(11, 126)));
    assert(drawCount() > 0 && drawScript(126));

    assert(th128_title_menu_page(0, 6) == 1);
    tick(12);
    assert(stateHas("\"page\":0"));
    assert(stateHas("\"ready\":1"));
    assert(selection == 6);
    assert(!hasNode(11, 130));
    assert(hasNode(3, 9) && hasNode(3, 16));
    assert(th128_title_menu_select(5) == 1);
    assert(selection == 5);
    assert(th128_title_menu_select(49) == 0);
    assert(stateHas("outside the page range"));

    assert(th128_title_menu_page(1, 4) == 0);
    assert(stateHas("outside the page range"));
    assert(stateHas("\"page\":0"));

    th128_select(2);
    assert(selection == 2);
    std::puts("PASS native title pages 0/1/2 roots, IRQ selection, transition and return");
    return 0;
}
