#pragma once
#include <string>
namespace th12 {
extern std::string snapshotBuffer, messageSnapshotBuffer, spellSnapshotBuffer,
    backgroundSnapshotBuffer;
const std::string& captureState();
const std::string& captureMessage();
const std::string& captureSpell();
const std::string& captureBackground();
} // namespace th12
