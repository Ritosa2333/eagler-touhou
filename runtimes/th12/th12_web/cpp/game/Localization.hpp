#pragma once
#include "Types.hpp"
#include <string>
namespace th11::Localization {
// Prepared thcrap ETL1 tables share the TH10 Runtime format. All missing
// entries fall back to the unmodified Japanese archive or score file.
const char* SpellName(u32 id,const char* fallback);
const char* MusicTitle(u32 track,const char* fallback);
const char* MusicComment(u32 track,u16 line,const char* fallback);
const char* StringById(const char* id,const char* fallback);
// thcrap stringloc formats may wrap printf fields in <r$...$> width hints.
// Return a validated C printf template for the three TH11 display sites.
const char* FormatStringById(const char* id,const char* fallback);
}
