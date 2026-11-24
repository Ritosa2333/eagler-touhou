#include "game/ShotEnemyCollisionRules.hpp"

#include <bit>
#include <charconv>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

using namespace th128::shotenemy;

static void require(bool condition) {
    if (!condition) {
        std::cerr << "shot/enemy invariant failed\n";
        std::exit(2);
    }
}

static ShotImage readImage(const std::string& text) {
    require(text.size() == ShotSlotBytes * 2);
    ShotImage image{};
    for (std::size_t index = 0; index < image.size(); ++index) {
        unsigned value = 0;
        const auto parsed = std::from_chars(text.data() + index * 2, text.data() + index * 2 + 2, value, 16);
        require(parsed.ec == std::errc{} && parsed.ptr == text.data() + index * 2 + 2);
        image[index] = static_cast<std::uint8_t>(value);
    }
    return image;
}

static void printImage(const ShotImage& image) {
    constexpr char digits[] = "0123456789abcdef";
    for (const auto value : image)
        std::cout << digits[value >> 4] << digits[value & 15];
}

static EnemyContactShape readShape() {
    EnemyContactShape shape;
    std::uint32_t value = 0;
    for (auto& component : shape.combinedPosition) {
        std::cin >> value;
        component = std::bit_cast<float>(value);
    }
    for (auto& component : shape.fullSize) {
        std::cin >> value;
        component = std::bit_cast<float>(value);
    }
    return shape;
}

static void printContact(const ShotEnemyContactPlan& plan) {
    std::cout << static_cast<int>(plan.continuation) << ' ' << plan.damagePrefix << ' '
              << plan.hitMarkerWritten << ' ' << plan.damageQueryCompleted << ' '
              << static_cast<int>(plan.required.kind) << ' ' << plan.required.callSite << ' '
              << plan.required.target << ' ' << plan.required.recordIdentity << ' ' << plan.required.handle << ' ';
    printImage(plan.next);
    if (plan.continuation != ContactContinuation::InvalidGeometry &&
        plan.continuation != ContactContinuation::NeedsShotKind &&
        plan.continuation != ContactContinuation::NeedsRecordBinding &&
        plan.continuation != ContactContinuation::Inactive) {
        for (const auto edge : plan.enemyEdges)
            std::cout << ' ' << std::bit_cast<std::uint32_t>(edge);
    }
    std::cout << '\n';
}

static void contactCase() {
    const auto shape = readShape();
    std::string imageHex;
    unsigned kind = 0;
    int binding = 0;
    std::uint32_t callback = 0;
    std::cin >> imageHex >> kind >> binding >> callback;
    const auto image = readImage(imageHex);
    std::optional<ShotCollisionRecordBinding> record;
    if (binding != 0) {
        record = ShotCollisionRecordBinding{0x390000, static_cast<std::uint8_t>(kind), std::nullopt};
        if (binding == 2)
            record->nativeCollisionTarget = callback;
    }
    printContact(planOrdinaryShotEnemyContact(shape, image, record));
}

static void queryCase() {
    ShotEnemyQueryInput input;
    input.enemy = readShape();
    int hasOutFlag = 0, slot = -1, source = -2, poolKnown = 1;
    std::uint32_t sourceInactiveFlags = 0;
    std::string imageHex;
    std::cin >> input.previousPlayerTime >> input.currentPlayerTime >> hasOutFlag >> input.outFlagBefore
             >> slot >> imageHex >> source >> sourceInactiveFlags >> poolKnown;
    input.hasOutFlag = hasOutFlag != 0;
    std::vector<ShotImage> pool(ShotSlotCount);
    if (slot >= 0) {
        require(slot < static_cast<int>(ShotSlotCount));
        pool[static_cast<std::size_t>(slot)] = readImage(imageHex);
    }
    if (poolKnown)
        input.shots = pool;
    const std::array bindings{ShotCollisionRecordBinding{0x390000, 0, 0}};
    input.records = bindings;
    std::vector<std::uint32_t> sourceFlags(DamageSourceCount, sourceInactiveFlags);
    if (source >= 0) {
        require(source < static_cast<int>(DamageSourceCount));
        sourceFlags[static_cast<std::size_t>(source)] = 1;
    }
    if (source != -2)
        input.damageSourceFlags = sourceFlags;
    const auto plan = planShotEnemyQuery(input);
    std::cout << static_cast<int>(plan.continuation) << ' ' << plan.completedDamage << ' ' << plan.outFlag << ' '
              << plan.outFlagWritten << ' ' << plan.examinedShots << ' ' << plan.examinedDamageSources << ' '
              << (plan.selectedShot ? static_cast<int>(*plan.selectedShot) : -1) << ' '
              << static_cast<int>(plan.required.kind) << ' ' << plan.required.callSite << ' '
              << plan.required.target << ' ' << plan.required.slot;
    if (plan.selectedShot) {
        std::cout << ' ' << static_cast<int>(plan.selectedContact.continuation) << ' '
                  << plan.selectedContact.damagePrefix << ' ';
        printImage(plan.selectedContact.next);
    }
    std::cout << '\n';
}

static void accountingCase() {
    ShotEnemyQueryPlan query;
    EnemyDamageSnapshot input;
    int complete = 0, spellKnown = 0;
    std::uint32_t spell = 0;
    std::cin >> complete >> query.completedDamage >> query.outFlag >> input.playerState
             >> input.controlDamageBypass >> input.enemyFlags >> input.protection;
    query.continuation = complete ? QueryContinuation::Complete : QueryContinuation::NeedsSelectedContact;
    for (auto& value : input.lifeWords)
        std::cin >> value;
    for (auto& value : input.managerStats)
        std::cin >> value;
    std::cin >> spellKnown >> spell;
    if (spellKnown)
        input.spellModeFlags = spell;
    const auto plan = planEnemyDamageAfterCompletedQuery(query, input);
    std::cout << static_cast<int>(plan.continuation) << ' ' << plan.playerScaledDamage << ' ' << plan.hpDamage << ' '
              << plan.hpSuppressed << ' ' << plan.lifeWritten << ' ' << plan.deathCandidate << ' '
              << static_cast<int>(plan.required.kind) << ' ' << plan.required.callSite << ' ' << plan.required.target;
    for (const auto value : plan.nextLifeWords)
        std::cout << ' ' << value;
    for (const auto value : plan.nextManagerStats)
        std::cout << ' ' << value;
    std::cout << '\n';
}

static void invariantCases() {
    static_assert(ShotPoolOffset == 0xfa8 && ShotSlotCount == 256 && ShotSlotBytes == 0x78);
    static_assert(DamageSourcePoolOffset == 0x8edc && DamageSourceCount == 2000);
    static_assert(OrdinaryContactAnimationCalls[0].callSite == 0x43ec8a);
    static_assert(OrdinaryContactAnimationCalls[1].target == 0x4679b0);
    static_assert(OrdinaryContactAnimationCalls[2].target == 0x4670b0);
    static_assert(PositiveQueryImpactCall.callSite == 0x43fcb3 && PositiveQueryImpactCall.target == 0x427260);
    static_assert(EnemyDeathCall.callSite == 0x416715 && EnemyDeathCall.target == 0x417370);
    EnemyDamageSnapshot enemy;
    enemy.lifeWords = {100, 17, 29, 700, 100, 1};
    enemy.managerStats = {0x12345678, 0xfedcba98};
    ShotEnemyQueryPlan pending;
    pending.continuation = QueryContinuation::NeedsSelectedContact;
    pending.completedDamage = 2000;
    auto plan = planEnemyDamageAfterCompletedQuery(pending, enemy);
    require(plan.continuation == DamageContinuation::NeedsCompletedQuery);
    require(plan.nextLifeWords == enemy.lifeWords && plan.nextManagerStats == enemy.managerStats);
    pending.continuation = QueryContinuation::Complete;
    plan = planEnemyDamageAfterCompletedQuery(pending, enemy);
    require(plan.continuation == DamageContinuation::NeedsSpellModeSnapshot);
    require(!plan.lifeWritten && plan.nextLifeWords == enemy.lifeWords);
    require(plan.required.callSite == 0x416619 && plan.required.target == 0x414e50);
    require(plan.nextManagerStats[1] == enemy.managerStats[1] + 400u);
    ShotImage shot{};
    shot[0x48] = 2;
    const auto contact = planOrdinaryShotEnemyContact({}, shot);
    require(contact.continuation == ContactContinuation::Inactive && contact.next == shot);
    ShotEnemyQueryInput noTick;
    noTick.outFlagBefore = 0x12345678;
    const auto query = planShotEnemyQuery(noTick);
    require(query.continuation == QueryContinuation::Complete && !query.outFlagWritten);
    require(query.outFlag == noTick.outFlagBefore && query.examinedShots == 0);
}

int main() {
    invariantCases();
    std::string command;
    while (std::cin >> command) {
        if (command == "J") {
            int value=0,flag=0;std::cin>>value>>flag;
            const auto plan=planShotQueryImpact(value,flag);require(bool(plan));
            std::cout<<plan->value<<' '<<plan->flag<<' '<<plan->soundRequired<<'\n';
        } else if (command == "T") {
            std::string encoded;std::uint32_t handle=0;
            std::cin>>encoded>>handle;
            ShotEnemyContactPlan pending;
            pending.continuation=ContactContinuation::NeedsAnimationCleanup;
            pending.next=readImage(encoded);pending.damagePrefix=13;pending.hitMarkerWritten=true;
            const auto next=completeOrdinaryShotContactAnimation(pending,handle);
            require(bool(next));printImage(next->next);std::cout<<' '<<next->completedDamage<<'\n';
        } else if (command == "C")
            contactCase();
        else if (command == "Q")
            queryCase();
        else if (command == "H")
            accountingCase();
        else
            require(false);
        require(!std::cin.fail());
    }
    return 0;
}
