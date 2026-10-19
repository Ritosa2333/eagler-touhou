#pragma once
#include "../game/StageHints.hpp"
namespace th10::browser {
struct HintData {const HintName *sections,*alignments;const char *default_file,*extra_file,*comments[3],*separator;};
const HintData& hint_data(bool chinese);
}
