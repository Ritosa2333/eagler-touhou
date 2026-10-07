#pragma once
#include "AnmTypes.hpp"

namespace th12::anm_logic {
class Registry {
  std::unordered_map<int, std::shared_ptr<Bank>> banks_;

public:
  std::shared_ptr<Bank> bank(int id) const;
  bool loadBank(int id, const uint8_t* bytes, size_t length, std::string& error);
  void unloadBank(int id) { banks_.erase(id); }
};
class VM {
  enum Field {
    X,
    Y,
    Z,
    RX,
    RY,
    RZ,
    SX,
    SY,
    UVEL,
    VVEL,
    ALPHA,
    RED,
    GREEN,
    BLUE,
    ALPHA2,
    RED2,
    GREEN2,
    BLUE2,
    USCALE,
    VSCALE
  };
  struct Interpolation {
    bool active = false;
    int mode = 0;
    float time = 0, duration = 0;
    std::vector<Field> fields;
    std::vector<float> from, to, a, b;
  };
  std::shared_ptr<Bank> bank_;
  Registry* registry_ = nullptr;
  Script* script_ = nullptr;
  State state_;
  size_t pc_ = 0, savedPc_ = 0;
  float timer_ = 0, savedTimer_ = 0;
  int pendingInterrupt_ = 0;
  bool saved_ = false;
  std::array<float, 3> rotationVelocity_{};
  std::array<float, 2> scaleVelocity_{}, scrollVelocity_{};
  std::vector<Interpolation> interpolations_;
  void report(Runtime& r, int op, const char* why);
  uint32_t random32(Runtime& r, bool forceGame = false);
  float unit(Runtime& r);
  float signedUnit(Runtime& r);
  void selectSprite(int sprite, Runtime& runtime);
  static int integerIndex(int id);
  float* floatVariable(int id);
  int32_t readInt(Command& c, size_t i, Runtime& r);
  float readFloat(Command& c, size_t i, Runtime& r);
  uint32_t* integerDestination(Command& c, size_t i);
  void writeInt(Command& c, size_t i, int32_t n);
  int32_t destinationInt(Command& c, size_t i);
  void writeFloat(Command& c, size_t i, float n);
  float destinationFloat(Command& c, size_t i);
  float field(Field f) const;
  void setField(Field f, float n);
  void interpolate(std::initializer_list<Field> fields, std::vector<float> to, int duration,
                   int mode, std::vector<float> a = {}, std::vector<float> b = {});
  void jump(uint32_t offset, int clock, Runtime& r);
  bool handleInterrupt();
  bool command(Command& c, Runtime& r);
  void advance(Runtime& r);

public:
  const State& state() const;
  State& control(); // explicit native custom-update hooks, never rendering
  Diagnostic diagnostics() const;
  bool bind(Registry& registry, int bank, int script, Runtime& runtime,
            const std::array<float, 3>* initialScriptPosition = nullptr);
  bool rebindWithoutReset(Registry& registry, int bank, int script, Runtime& runtime);
  void interrupt(int code);
  bool tick(Runtime& runtime);
};
} // namespace th12::anm_logic
