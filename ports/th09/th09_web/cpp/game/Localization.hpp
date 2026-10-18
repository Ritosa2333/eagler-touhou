// thcrap-style offline language pack lookup for the TH09 Web port. Mirrors
// the TH07 contract: prepared ETL1/EAS1/EST1 tables plus localization/
// options.json under /thcrap/th09/, with every lookup falling back to the
// original Japanese game data when the pack is absent or incomplete.
#pragma once
#include <cstddef>
#include <cstdint>
namespace th09 {
namespace Localization {
struct AsciiEntryView {
    const char* id;
    const char* text;
    const char* baseline;
    float extraX;
    bool hasTranslation;
    bool hasAlignment;
};
bool Active();
const char* FontFile();
const char* FontName();
const char* SpellName(std::uint32_t id,const char* fallback);
const char* StageName(std::uint32_t id,const char* fallback);
const char* MusicTitle(std::uint32_t track,const char* fallback);
const char* MusicComment(std::uint32_t track,std::uint16_t line,const char* fallback);
// thcrap TSA "spellcomments.js" table: comment_N line <line> for comment field N
// (1-based, matching thcrap's comment_num), and the per-spell owner override.
const char* SpellComment(std::uint32_t number,std::uint16_t line,const char* fallback);
const char* SpellCommentOwner(std::uint32_t number,const char* fallback);
bool LookupAscii(const char* fallback,AsciiEntryView& view);
const char* AsciiString(const char* fallback);
const char* AsciiStringById(const char* id,const char* fallback);
const char* StringById(const char* id,const char* fallback);
const char* FormatStringById(const char* id,const char* fallback);
// Re-encodes original CP932 text as UTF-8 so it can be substituted into a
// translated (UTF-8) format string without corrupting either half. Returns the
// input pointer when the text is already valid UTF-8. A small rotating pool
// keeps several results alive for one formatted call.
const char* Utf8(const char* text);
void CopyText(char* destination,std::size_t capacity,const char* source);
void CopyCodepointChunk(char* destination,std::size_t capacity,const char* source,
                        std::size_t firstCodepoint,std::size_t maximumCodepoints);
void CopyDisplayColumnChunk(char* destination,std::size_t capacity,const char* source,
                            std::size_t firstColumn,std::size_t maximumColumns);
}
}
