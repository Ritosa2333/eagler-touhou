#pragma once
#include <string_view>
#include <array>
namespace jp {
// Verbatim CP932-decoded ReC98 th02/shiftjis/m_char.cpp and op_main.hpp.
inline constexpr std::array<std::array<std::wstring_view,3>,3> descriptions={{
    {L" 陰陽玉の力を使わない ",L" 広範囲でかつ機動力に ",L"　強い高機動力タイプ　"},
    {L" 　靈撃が優れている 　",L"　 バランスの取れた　 ",L"　　防御重視タイプ　　"},
    {L" 　陰陽玉の力で戦う   ",L"　攻撃力が優れている　",L"　　攻撃重視タイプ　　"}
}};
inline constexpr std::array<std::array<std::wstring_view,3>,3> descriptions_en={{
    {L"No Yin-Yang Orb power",L"Wide spread",L"High mobility"},
    {L"Strong Spirit Attack",L"Balanced performance",L"Defense focused"},
    {L"Uses the Yin-Yang Orb",L"Strong attack power",L"Offense focused"}
}};
inline constexpr std::wstring_view choose=L"靈夢の戦闘スタイルを、下の３つからえらんでね";
inline constexpr std::wstring_view choose_en=L"Choose one of Reimu's three fighting styles.";
inline constexpr std::array<std::wstring_view,2> extra_note={
    L"注）　エキストラステージでは、難易度、プレイヤー、ボム数は変更出来ません",
    L"　　　それぞれ、難易度ＥＸＴＲＡ、プレイヤー３人、ボム１個となります    "};
inline constexpr std::array<std::wstring_view,2> extra_note_en={
    L"Extra Stage uses fixed difficulty and starting stocks:",
    L"Extra difficulty, three players, and one Bomb."};
inline constexpr std::wstring_view cleared=L"  ☆☆ＣＬＥＡＲＥＤ☆☆  ";
inline constexpr std::wstring_view effect=L"演出";
inline constexpr std::array<std::wstring_view,2> effect_values={L"　通常  ",L"一部軽減"};
inline int cell_width(std::wstring_view text) {int n=0;for(wchar_t c:text)n+=(c<128 || (c>=0xff61&&c<=0xff9f))?8:16;return n;}
}
