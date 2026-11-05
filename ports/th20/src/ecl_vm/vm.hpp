#pragma once
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include <mutex>
#include <memory>
#include "th20/binary.hpp"
#include "math.hpp"

namespace th20::source::ecl {
std::uint32_t float_bits(float value) noexcept;
float bits_float(std::uint32_t value) noexcept;
std::int32_t bits_int(std::uint32_t value) noexcept;
// SSE CVTTSS2SI semantics, including the integer-indefinite result.
std::int32_t truncate_float(float value) noexcept;

// Owning C++ storage. Original stack: vector proxy/begin/end/capacity at
// +00/+04/+08/+0c, byte stack pointer +10, frame base +14.
struct Stack {
    std::vector<std::uint32_t> words;
    std::int32_t pointer = 0;
    std::int32_t frame_base = 0;
    std::uint32_t& absolute(std::int32_t byte_offset); // 0x53e630
    std::uint32_t& local(std::int32_t byte_offset);    // 0x53e6a0
    void push(std::uint32_t bits, char type = 0);     // 0x53f260 (4-byte path)
    std::uint32_t pop(char type = 0);                // 0x53f0b0 (4-byte path)
    std::uint32_t peek(std::int32_t relative, char type); // 0x540450
    bool enter_frame(std::int32_t bytes);            // 0x5405b0
    void leave_frame();                             // 0x540300
};

struct Subroutine {
    std::string name;
    std::vector<std::uint8_t> bytes; // instruction bytes after ECLH's 16-byte header
    // Actual game loaders retain shared writable files. Borrowing preserves
    // cross-player/script mutation; the loader/cache must outlive this view.
    std::uint8_t* borrowed_data = nullptr;
    std::size_t borrowed_size = 0;
    std::uint8_t* data() noexcept {return borrowed_data?borrowed_data:bytes.data();}
    const std::uint8_t* data() const noexcept {return borrowed_data?borrowed_data:bytes.data();}
    std::size_t size() const noexcept {return borrowed_data?borrowed_size:bytes.size();}
};
struct Program {
    std::vector<Subroutine> subroutines;
    // Append already structurally checked SCPT files in resource load order.
    void append(const EclDocument& document);
    void append_borrowed(std::string name,std::uint8_t* data,std::size_t bytes);
    std::int32_t find(const std::string& name) const;
};
struct Instruction {
    std::uint8_t* bytes;
    std::int32_t time() const;
    std::uint16_t opcode() const;
    std::uint16_t size() const;
    std::uint16_t mask() const;
    std::uint8_t rank() const;
    std::uint8_t argument_count() const;
    std::uint8_t& stack_drop_bytes() const;
    std::uint32_t argument(std::int32_t index) const;
};
struct Runtime;
class Scheduler;
struct RandomStream {
    std::uint32_t& state;
    std::uint32_t& last;
    const std::uint32_t& maximum;
    std::recursive_mutex& lock; // original shared lock registry slot 10
    std::uint32_t next(); // 0x423ee0 using recovered 0x4235f0
    float signed_unit(); // 0x4298e0
};
// These are real missing engine interfaces. They have no default implementation.
// Their implementations must eventually recover the corresponding original
// vtable methods, entity dispatcher and interpolation operations.
struct Engine {
    virtual ~Engine() = default;
    virtual std::int32_t read_integer(std::int32_t variable) = 0; // manager vtable +08
    virtual std::uint32_t& integer_destination(std::int32_t variable) = 0; // +0c
    virtual float read_float(std::int32_t variable) = 0; // +10
    virtual std::uint32_t& float_destination(std::int32_t variable) = 0; // +14
    // Same return contract as original manager vtable +04: -1 stops this tick;
    // 1 repeats the cached instruction's time check; other values advance.
    virtual std::int32_t execute_entity_opcode(Runtime&, Instruction) = 0; // game 0x48c010
    // Known core cases not yet recovered must not be mistaken for entity cases.
};
// Semantic layout corresponding to the original runtime: time +00, sub +04,
// byte IP +08, stack +0c, async id +24, manager +28, field +2c, rank +30,
// interpolator vector +34, async flags +44. This owning API is not its ABI.
struct Runtime {
    Program* program = nullptr;
    Engine* engine = nullptr;
    Scheduler* scheduler = nullptr;
    RandomStream* random = nullptr;
    const float* clock_rate = nullptr;
    float time = 0;
    std::int32_t subroutine = -1;
    std::int32_t instruction_offset = -1;
    Stack stack;
    std::int32_t async_id = 0;
    std::int32_t async_field_2c = 0;
    std::uint8_t rank_mask = 0xff;
    std::uint32_t async_flags = 0;
    std::vector<math::Interpolator> interpolators;
    bool active() const noexcept { return subroutine != -1 && instruction_offset != -1; }
    Instruction current(); // 0x5403d0 + resource 0x53e8e0
    std::int32_t integer_argument(Instruction, std::int32_t index, bool pop_reference = false);
    float float_argument(Instruction, std::int32_t index, bool pop_reference = false);
    std::uint32_t& integer_destination(Instruction, std::int32_t index);
    std::uint32_t& float_destination(Instruction, std::int32_t index);
    bool call(Instruction instruction); // synchronous 0x53f3b0 variant (0,0)
    bool call_into(Instruction instruction,Runtime& target,std::int32_t argument_skip); // asynchronous variant
    void tick_interpolators(); // 0x53e00c
    // No instruction budget: preserving original script timing and infinite-loop
    // behavior requires the caller to supply correct scripts.
    std::int32_t tick(float delta); // 0x53b5c0: -1 inactive/ended, otherwise 0
};
// Owning equivalent of manager 0x53e2b0/0x53e390/0x53e920/0x53e560.
// A new async task is inserted after the main head; each dispatch saves next
// before executing, so newly spawned tasks do not run in that same traversal.
class Scheduler {
    struct Node {
        Runtime* runtime;
        Node* previous=nullptr;
        Node* next=nullptr;
        std::unique_ptr<Runtime> owned;
        explicit Node(Runtime* value):runtime(value){}
    };
    Node head_;
    void erase(Node* node);
public:
    Runtime main;
    Runtime* current;
    Scheduler(Program& program,Engine& engine,RandomStream* random=nullptr,const float* rate=nullptr);
    ~Scheduler();
    Scheduler(const Scheduler&)=delete;
    Scheduler& operator=(const Scheduler&)=delete;
    Runtime& spawn(Runtime& caller,Instruction call,std::int32_t id,std::int32_t argument_skip);
    Runtime* find(std::int32_t id); // includes main node, first matching list entry
    void terminate_async(); // skips main node
    std::int32_t tick(float delta);
    std::vector<Runtime*> task_order() const;
};
bool implemented_opcode(std::uint16_t opcode) noexcept;
}
