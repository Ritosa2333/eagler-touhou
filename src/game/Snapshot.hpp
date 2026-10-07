#pragma once
#include <string>
namespace th12 {
extern std::string snapshotBuffer, messageSnapshotBuffer, spellSnapshotBuffer,
    backgroundSnapshotBuffer, ufoSnapshotBuffer;
const std::string& captureState();
const std::string& captureMessage();
const std::string& captureSpell();
const std::string& captureBackground();
const std::string& captureUfo();
} // namespace th12
