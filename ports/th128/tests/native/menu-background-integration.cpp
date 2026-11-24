#include "../../src/runtime/Exports.cpp"
#include <cassert>
#include <cmath>
#include <cstdio>
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

static int countOwner(int owner) {
    int count = 0;
    for (const auto& [identity, node] : scene.nodes())
        if (int(node->owner) == owner && node->alive) ++count;
    return count;
}

static th128::anm::Node* findNode(int owner, int script) {
    for (const auto& [identity, node] : scene.nodes())
        if (int(node->owner) == owner && node->alive && node->vm.state().script == script)
            return node.get();
    return nullptr;
}

static void tick(int count) {
    for (int index = 0; index < count; ++index) assert(th128_tick() == 1);
}

static const DrawRecord* drawScript(int script) {
    for (const auto& record : draws)
        if (record.script == script) return &record;
    return nullptr;
}

static bool drawn(int script) {
    assert(th128_draw_count() >= 0);
    assert(errorText.empty());
    return drawScript(script) != nullptr && (drawScript(script)->primary >> 24) != 0;
}

static bool invisible(int script) {
    assert(th128_draw_count() >= 0);
    assert(errorText.empty());
    const auto* record = drawScript(script);
    return !record || (record->primary >> 24) == 0;
}

static const DrawRecord* headerRecord(int script) {
    assert(th128_draw_count() >= 0);
    return drawScript(script);
}

static std::vector<std::string> captureSignature() {
    assert(th128_draw_count() >= 0);
    std::vector<std::string> signature;
    for (const auto& record : draws) {
        char row[256];
        std::snprintf(row, sizeof(row), "%u|%d|%d|%d|%d|%.6f|%.6f|%.6f|%.6f|%u",
            record.id, record.script, record.entry, record.sprite, record.layer,
            record.x, record.y, record.width, record.height, record.primary);
        signature.emplace_back(row);
    }
    return signature;
}

static void assertHeader(int owner, int script, int sprite) {
    assert(countOwner(owner) == 1);
    assert(hasNode(owner, script));
    const auto* record = headerRecord(script);
    assert(record && record->entry == 6 && record->sprite == sprite);
    assert(record->layer == 26);
    assert(std::fabs(record->x - 32.0f) < 0.75f);
    assert(std::fabs(record->y - 8.0f) < 0.75f);
    assert(std::fabs(record->width - 256.0f) < 0.01f);
    assert(std::fabs(record->height - 48.0f) < 0.01f);
    assert((record->primary >> 24) != 0);
}

static void assertNoHeader(int owner, int script) {
    assert(countOwner(owner) == 0);
    assert(!hasNode(owner, script));
    assert(th128_draw_count() >= 0);
    assert(drawScript(script) == nullptr);
}

static bool stateReady(int page) {
    const char* state = th128_title_menu_state();
    char expected[64];
    std::snprintf(expected, sizeof(expected), "\"page\":%d", page);
    return std::strstr(state, expected) && std::strstr(state, "\"ready\":1");
}

static bool stateError() {
    return std::strstr(th128_title_menu_state(), "\"error\":\"\"") == nullptr;
}

static void assertMain() {
    assert(hasNode(1, 97) && hasNode(2, 101));
    assert(drawn(98) && drawn(99) && drawn(100) && drawn(103) && drawn(104));
    assert(invisible(98) == false);
}

static void assertRetiredBackground() {
    assert(invisible(98) && invisible(103) && invisible(104));
    const auto* floral = headerRecord(99);
    assert(floral && drawScript(100));
    const int rgb = int(floral->primary & 0xffffffu);
    assert(rgb > 0x808080 && rgb < 0xe0e0e0);
}

static th128::anm::Script savedHeader;
static th128::anm::Script savedBoot;

static void injectInvalidScript(int script, th128::anm::Script& saved) {
    auto bank = scene.registry.bank(24);
    assert(bank);
    saved = bank->scripts[script];
    th128::anm::Script invalid;
    th128::anm::Command command;
    command.op = 115;
    command.time = 0;
    invalid.commands.push_back(command);
    bank->scripts[script] = invalid;
}

static void restoreScript(int script, const th128::anm::Script& saved) {
    auto bank = scene.registry.bank(24);
    assert(bank);
    bank->scripts[script] = saved;
}

int main(int argc, char** argv) {
    assert(argc == 3);
    const auto title = read(argv[1]);
    const auto version = read(argv[2]);
    assert(th128_load_anm(24, title.data(), std::uint32_t(title.size())) == 1);
    assert(th128_load_anm(25, version.data(), std::uint32_t(version.size())) == 1);
    assert(th128_boot_title(0x1234) == 1);
    tick(80);
    assert(stateReady(0));
    assertMain();

    const auto mainCapture = captureSignature();
    injectInvalidScript(105, savedHeader);
    assert(th128_title_menu_page(1, 1) == 0);
    assert(titlePage == 0 && titleReady && !titleTransitioning);
    assert(selection == 0 && titleSelection[0] == 0);
    assert(hasNode(3, 0) && hasNode(4, 0) && hasNode(1, 97) && hasNode(2, 101));
    assert(countOwner(10) == 0 && countOwner(12) == 0);
    assert(stateReady(0) && stateError());
    assert(captureSignature() == mainCapture);
    restoreScript(105, savedHeader);

    assert(th128_title_menu_page(1, 1) == 1);
    tick(1);
    const auto* early = headerRecord(105);
    assert(early && early->x > -256.0f && early->x < 32.0f);
    tick(9);
    const auto* mid = headerRecord(105);
    assert(mid && mid->x > early->x && mid->x < 32.0f);
    tick(10);
    assert(stateReady(1));
    assertHeader(12, 105, 69);
    tick(100);
    assertHeader(12, 105, 69);
    assertRetiredBackground();
    assert(drawn(117) && drawn(131));

    assert(th128_title_menu_page(2, 1) == 1);
    tick(5);
    const auto* leaving = headerRecord(105);
    assert(leaving && leaving->x < 32.0f);
    const auto* entering = headerRecord(113);
    assert(entering && entering->x > -256.0f && entering->x < 32.0f);
    tick(15);
    assert(stateReady(2));
    assertHeader(13, 113, 77);
    assert(countOwner(12) == 1 && countOwner(13) == 1);
    std::printf("route-ready boot101 alive: %d\n", hasNode(2, 101) ? 1 : 0);

    const int routeSelection = selection;
    const std::array<int, 2> oldHeaderFrames = titleHeaderRemoveFrame;
    const std::array<int, 2> oldHeaderOwners = titleHeaderRemoveOwner;
    th128::anm::Node* oldHeader = findNode(12, 105);
    assert(oldHeader && oldHeader->vm.state().x < 32.0f);
    const uint32_t oldHeaderId = oldHeader->id;
    const uint64_t oldHeaderKey = oldHeader->key;
    const int oldHeaderClock = oldHeader->vm.state().clock;
    const auto oldHeaderDraws = captureSignature();

    injectInvalidScript(105, savedHeader);
    assert(th128_title_menu_page(1, 1) == 0);
    assert(titlePage == 2 && titleReady && !titleTransitioning);
    assert(stateReady(2) && selection == routeSelection);
    assert(hasNode(11, 130) && hasNode(13, 113));
    assert(countOwner(20) == 0 && countOwner(10) == 0 && countOwner(12) == 1);
    th128::anm::Node* kept = findNode(12, 105);
    assert(kept && kept->id == oldHeaderId && kept->key == oldHeaderKey);
    assert(kept->vm.state().clock == oldHeaderClock);
    assert(captureSignature() == oldHeaderDraws);
    assert(titleHeaderRemoveFrame == oldHeaderFrames);
    assert(titleHeaderRemoveOwner == oldHeaderOwners);
    assert(stateError());
    restoreScript(105, savedHeader);

    assert(th128_title_menu_page(1, 1) == 1);
    tick(100);
    assert(stateReady(1));
    assertHeader(12, 105, 69);
    assertNoHeader(13, 113);
    assert(countOwner(20) == 0);
    th128::anm::Node* oldBoot = findNode(2, 101);
    const bool bootAlive = oldBoot != nullptr;
    const uint32_t oldBootId = oldBoot ? oldBoot->id : 0;
    const int oldBootClock = oldBoot ? oldBoot->vm.state().clock : 0;
    std::printf("return-main boot101 alive before failed return: %d\n", bootAlive ? 1 : 0);
    const auto bootCapture = captureSignature();
    const auto oldBootFrames = titleHeaderRemoveFrame;
    const auto oldBootOwners = titleHeaderRemoveOwner;
    injectInvalidScript(101, savedBoot);
    assert(th128_title_menu_page(0, 6) == 0);
    assert(titlePage == 1 && titleReady && !titleTransitioning);
    assert(stateReady(1));
    assert(hasNode(10, 129) && hasNode(12, 105));
    assert(countOwner(3) == 0 && countOwner(4) == 0 && countOwner(21) == 0);
    assert(captureSignature() == bootCapture);
    assert(titleHeaderRemoveFrame == oldBootFrames);
    assert(titleHeaderRemoveOwner == oldBootOwners);
    assert(stateError());
    if (bootAlive) {
        th128::anm::Node* keptBoot = findNode(2, 101);
        assert(keptBoot && keptBoot->id == oldBootId && keptBoot->vm.state().clock == oldBootClock);
    }
    restoreScript(101, savedBoot);

    assert(th128_title_menu_page(0, 6) == 1);
    tick(140);
    assert(stateReady(0));
    assertNoHeader(12, 105);
    assertNoHeader(13, 113);
    assert(countOwner(20) == 0 && countOwner(21) == 0);
    assertMain();

    assert(th128_title_menu_page(1, 3) == 1);
    tick(100);
    assertHeader(12, 105, 69);
    assert(th128_title_menu_page(0, 5) == 1);
    assert(stateReady(1) == false);

    th128_reset();
    assert(th128_boot_title(0x1234) == 1);
    tick(80);
    assertMain();
    assert(th128_title_menu_page(1, 0) == 1);
    tick(300);
    assert(stateReady(1));
    assertHeader(12, 105, 69);
    assert(countOwner(12) == 1);

    assert(th128_title_menu_page(2, 2) == 1);
    tick(100);
    assertHeader(13, 113, 77);
    assertNoHeader(12, 105);
    assert(th128_title_menu_page(0, 2) == 1);
    tick(140);
    assertMain();
    assertNoHeader(12, 105);
    assertNoHeader(13, 113);

    std::puts("PASS production menu background retirement and authored headers");
    std::puts("PASS production rejection preserves state/owners and reset clears header timers");
    return 0;
}
