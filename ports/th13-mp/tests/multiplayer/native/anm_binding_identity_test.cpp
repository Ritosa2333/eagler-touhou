// Production VM binding identities flow through the actual host registry
// digest, so this suite can also share the cumulative m6 binary.
#include "../../anm/anm_test_fixtures.hpp"
#include "multiplayer/WorldState.hpp"
#include "multiplayer/HostState.hpp"

using namespace th13;
using namespace th13test;

namespace {
u64 vm_digest(const AnmVm& vm) {
    multiplayer::HostState host;
    host.state_fields.loading_registry = std::make_shared<AnmRegistry::SnapshotData>();
    host.state_fields.loading_registry->entries.resize(1);
    host.state_fields.loading_registry->entries[0].vm = vm;
    return multiplayer::WorldState::digest_host(host);
}

std::vector<u8> archive(i32 marker = 0) {
    std::vector<u8> script;
    push_insn(script, 2, 0); // valid template that stops without destroying
    push_terminator(script);
    SyntheticTexture texture;
    texture.name = "@identity" + std::to_string(marker);
    texture.has_data = false;
    // Identical sprite values make table identity, rather than geometry, the
    // only distinction when the actual pointer is deliberately rebound.
    return build_anm(texture, {{0,0,0,2,2}, {0,0,0,2,2}},
                     {{0,script}, {1,script}});
}

TEST(content_identity_is_address_independent_and_complete) {
    const auto bytes = archive();
    AnmResource a, b;
    CHECK(a.open(bytes.data(), bytes.size()));
    CHECK(b.open(bytes.data(), bytes.size()));
    CHECK(&a != &b);
    CHECK(a.content_identity() == b.content_identity());
    const u64 original = a.content_identity();
    b.textures[0].priority++;
    b.refresh_identity();
    CHECK(original != b.content_identity());
    b = a; b.sprites[0].u0 = -0.f; b.refresh_identity();
    CHECK(original != b.content_identity());
    b = a; b.scripts[0].script_base++; b.refresh_identity();
    CHECK(original != b.content_identity());
    b = a; b.scripts[0].bytes.back() ^= 1; b.refresh_identity();
    CHECK(original != b.content_identity());
    b = a; b.textures[0].pixels.push_back(1); b.refresh_identity();
    CHECK(original != b.content_identity());
    // Synthetic resources have no asset-file identity to fall back to.
    AnmResource synthetic;
    synthetic.scripts = a.scripts;
    CHECK(synthetic.content_identity() != AnmResource{}.content_identity());
}

TEST(resource_and_actual_sprite_bindings_change_the_vm_digest) {
    const auto bytes = archive(), different = archive(1);
    AnmResource a, b, other;
    CHECK(a.open(bytes.data(), bytes.size()));
    CHECK(b.open(bytes.data(), bytes.size()));
    CHECK(other.open(different.data(), different.size()));
    AnmVm x, y;
    CHECK(x.bind(a, 0)); CHECK(y.bind(b, 0));
    CHECK(x.select_sprite(0)); CHECK(y.select_sprite(0));
    CHECK(vm_digest(x) == vm_digest(y));
    const auto original = vm_digest(x);
    x.resource = &other;
    CHECK(vm_digest(x) != original);
    x = y; x.sprite_resource = &other; x.sprite = &other.sprites[0];
    CHECK(vm_digest(x) != original);
    x = y; x.sprite = &b.sprites[1]; // same content, different real table entry
    CHECK(x.visual.sprite == y.visual.sprite);
    CHECK(vm_digest(x) != original);
    x = y; x.sprite = nullptr;
    CHECK(vm_digest(x) != original);
    CHECK(a.sprite_index(nullptr) == -1);
    CHECK(a.sprite_index(&b.sprites[0]) == -2);
}

TEST(actual_script_binding_and_raw_script_content_are_visible) {
    const auto bytes = archive();
    AnmResource resource;
    CHECK(resource.open(bytes.data(), bytes.size()));
    AnmVm a, b;
    CHECK(a.bind(resource, 0)); CHECK(b.bind(resource, 1));
    b.source_script = a.source_script; // retain equal old digest numerics
    CHECK(vm_digest(a) != vm_digest(b));
    CHECK(resource.script_index(&resource.scripts[0].bytes) == 0);
    CHECK(resource.script_index(&resource.scripts[1].bytes) == 1);
    CHECK(resource.script_index(nullptr) == -1);
    AnmScript raw_a = resource.scripts[0], raw_b = raw_a;
    CHECK(a.bind(raw_a)); CHECK(b.bind(raw_b));
    CHECK(vm_digest(a) == vm_digest(b));
    raw_b.bytes.back() ^= 1;
    CHECK(vm_digest(a) != vm_digest(b)); // actual bytes, no stale bind cache
    CHECK(resource.script_index(&raw_a.bytes) == -2);
}

TEST(append_order_and_packed_file_sprite_domains_are_visible) {
    const auto first = archive(1), second = archive(2);
    AnmResource ab, ba, ab_copy;
    CHECK(ab.open(first.data(), first.size()));
    const auto initial = ab.content_identity();
    CHECK(ab.append(second.data(), second.size()));
    CHECK(initial != ab.content_identity());
    CHECK(ba.open(second.data(), second.size()));
    CHECK(ba.append(first.data(), first.size()));
    CHECK(ab.content_identity() != ba.content_identity());
    CHECK(ab_copy.open(first.data(), first.size()));
    CHECK(ab_copy.append(second.data(), second.size()));
    CHECK(ab.content_identity() == ab_copy.content_identity());
    AnmVm vm;
    CHECK(vm.bind(ab, 2)); CHECK(vm.select_sprite(0));
    CHECK(vm.visual.sprite == 0);
    CHECK(ab.sprite_index(vm.sprite) == 2);
    CHECK(ab.script_index(&ab.scripts[2].bytes) == 2);
}

TEST(retired_resource_identity_and_registry_restore_keep_bindings) {
    const auto bytes = archive();
    GameRandom random(123);
    AnmEnvironment environment;
    AnmManager manager(random, environment);
    CHECK(manager.load(7, bytes.data(), bytes.size()));
    auto* resource = manager.resource(7);
    const auto identity = resource->content_identity();
    auto* vm = manager.registry.allocate();
    CHECK(vm->bind(*resource, 0)); CHECK(vm->select_sprite(1));
    const auto handle = manager.registry.submit(*vm);
    CHECK(handle != 0);
    const auto digest = vm_digest(*vm);
    AnmRegistry::SnapshotData saved;
    CHECK(manager.registry.capture(saved));
    vm->sprite = &resource->sprites[0];
    CHECK(vm_digest(*vm) != digest);
    CHECK(manager.registry.restore(saved));
    vm = manager.registry.find(handle);
    CHECK(vm != nullptr); CHECK(vm_digest(*vm) == digest);
    manager.unload(7);
    CHECK(manager.resource(7) == nullptr);
    CHECK(manager.resource_id(resource) == -1);
    CHECK(manager.retired_resource_count() == 1);
    CHECK(vm->resource == resource);
    CHECK(vm->resource->content_identity() == identity);
    // A retired VM may freeze numeric presentation state; its bindings must
    // remain identifiable after the active bank map has forgotten it.
    CHECK(resource->sprite_index(vm->sprite) == 1);
    const auto different = archive(9);
    CHECK(manager.load(7, different.data(), different.size()));
    CHECK(manager.resource(7)->content_identity() != identity);
}
}

int __attribute__((weak)) main() { return run_all("ANM production binding identities"); }
