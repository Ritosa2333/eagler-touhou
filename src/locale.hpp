#pragma once
#include <array>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>

namespace soew {
// Only Japanese and English IDs are part of the Practice v4 wire format.
enum class Language { Japanese=0, English=1, ChineseSimplified=2, ChineseTraditional=3 };

constexpr bool is_chinese(Language language) {
    return language == Language::ChineseSimplified || language == Language::ChineseTraditional;
}
constexpr Language simulation_language(Language display) {
    return is_chinese(display) ? Language::Japanese : display;
}
constexpr Language effective_language(Language preference, bool chinese_available) {
    return !chinese_available && is_chinese(preference) ? Language::Japanese : preference;
}
constexpr Language gameplay_display_language(Language viewer, Language simulation) {
    return is_chinese(viewer) ? viewer : simulation;
}

template<class Char>
constexpr const Char* localized(Language language, const Char* ja, const Char* en,
                                const Char* hans = nullptr, const Char* hant = nullptr) {
    switch(language){
    case Language::English:return en;
    case Language::ChineseSimplified:return hans?hans:ja;
    case Language::ChineseTraditional:return hant?hant:ja;
    default:return ja;
    }
}

struct LocalizedText {
    const wchar_t* ja;
    const wchar_t* en;
    const wchar_t* hans=nullptr;
    const wchar_t* hant=nullptr;
    constexpr const wchar_t* operator()(Language language) const { return localized(language, ja, en, hans, hant); }
};

namespace text {
inline constexpr LocalizedText staff_fast_forward{L"Z長押し：4倍速", L"Hold Z: 4× speed", L"按住 Z：4 倍速", L"按住 Z：4 倍速"};
inline constexpr LocalizedText extra{L"エキストラ", L"Extra",L"额外关卡",L"額外關卡"}, yes{L"はい", L"Yes",L"是",L"是"}, no{L"いいえ", L"No",L"否",L"否"},
    back{L"戻る", L"Back",L"返回",L"返回"}, resume{L"再開", L"Resume",L"继续游戏",L"繼續遊戲"}, restart{L"やり直す", L"Restart",L"重新开始",L"重新開始"},
    name{L"名前：", L"Name: ",L"名字：",L"名字："}, pause{L"一時停止", L"Pause Menu",L"暂停菜单",L"暫停選單"},
    replay_browser{L"リプレイ一覧", L"Replay Browser",L"录像列表",L"錄影列表"}, save_replay{L"リプレイを保存", L"Save Replay",L"保存录像",L"儲存錄影"},
    game_over{L"ゲームオーバー", L"Game Over",L"游戏结束",L"遊戲結束"}, clear{L"クリア", L"Clear",L"通关",L"通關"}, menu_return{L"途中終了", L"Menu Return",L"中途退出",L"中途離開"},
    confirm_back{L"Z / Enter：決定　X / Esc：戻る", L"Z / Enter: confirm   X / Esc: back",L"Z / Enter：确认  X / Esc：返回",L"Z / Enter：確認  X / Esc：返回"},
    unavailable{L"選択不可", L"Unavailable",L"不可选",L"無法選擇"},
    legacy_content{L"旧版素材：対訳と原作タイトル画像がありません", L"Legacy content: bilingual text and clean title art unavailable",L"旧版素材：没有译文和无字标题图",L"舊版素材：沒有譯文和無字標題圖"},
    practice_start{L"練習開始", L"Practice Start",L"开始练习",L"開始練習"}, replay{L"リプレイ", L"Replay",L"录像回放",L"錄影重播"};
inline constexpr std::array<LocalizedText, 4> difficulty = {{{L"イージー", L"Easy",L"简单",L"簡單"}, {L"ノーマル", L"Normal",L"普通",L"普通"},
    {L"ハード", L"Hard",L"困难",L"困難"}, {L"ルナティック", L"Lunatic",L"疯狂",L"瘋狂"}}};
}

inline std::wstring stage_label(Language language, int stage) {
    return stage == 5 ? std::wstring(text::extra(language)) :
        localized(language, L"ステージ ", L"Stage ") + std::to_wstring(stage + 1);
}

inline constexpr std::array<std::string_view, 10> locale_text_assets = {
    "STAGE0.TXT", "STAGE1.TXT", "STAGE2.TXT", "STAGE3.TXT", "STAGE4.TXT", "STAGE5.TXT",
    "END1.TXT", "END2.TXT", "END3.TXT", "MUSIC.TXT"
};

// Read a Unicode scalar, rejecting malformed UTF-8 (including overlong forms,
// surrogates, and values above U+10FFFF). Offsets always end at glyph boundaries.
struct Utf8Scalar { std::uint32_t value; std::size_t bytes; };
constexpr Utf8Scalar utf8_scalar(std::string_view text, std::size_t start) {
    const auto lead = static_cast<unsigned char>(text.at(start));
    if (lead < 0x80) return {lead, 1};
    const std::size_t bytes = lead >= 0xf0 && lead <= 0xf4 ? 4 : lead >= 0xe0 && lead <= 0xef ? 3 : lead >= 0xc2 && lead <= 0xdf ? 2 : 0;
    if (!bytes || text.size() - start < bytes) throw std::runtime_error("Malformed UTF-8");
    auto value = std::uint32_t(lead & (0x7f >> bytes));
    for (std::size_t i = 1; i < bytes; ++i) {
        const auto c = static_cast<unsigned char>(text[start + i]);
        if ((c & 0xc0) != 0x80) throw std::runtime_error("Malformed UTF-8");
        value = (value << 6) | (c & 0x3f);
    }
    if (value < (bytes == 2 ? 0x80u : bytes == 3 ? 0x800u : 0x10000u) ||
        (value >= 0xd800 && value <= 0xdfff) || value > 0x10ffff)
        throw std::runtime_error("Malformed UTF-8");
    return {value, bytes};
}
constexpr int chinese_glyph_width(std::uint32_t value) {
    return value < 0x80 || (value >= 0xff61 && value <= 0xff9f) ? 8 : 16;
}
constexpr int chinese_text_width(std::string_view text) {
    int width = 0;
    for (std::size_t i = 0; i < text.size();) {
        const auto scalar = utf8_scalar(text, i);
        width += chinese_glyph_width(scalar.value);
        i += scalar.bytes;
    }
    return width;
}
constexpr std::string_view chinese_text_prefix(std::string_view text, int pixels) {
    std::size_t i = 0;
    while (i < text.size()) {
        const auto scalar = utf8_scalar(text, i);
        if ((pixels -= chinese_glyph_width(scalar.value)) < 0) break;
        i += scalar.bytes;
    }
    return text.substr(0, i);
}
inline constexpr std::string_view continue_token = "{continues}";
inline std::wstring wide_utf8(std::string_view text) {
    std::wstring out;
    out.reserve(text.size());
    for (std::size_t i = 0; i < text.size();) {
        const auto scalar = utf8_scalar(text, i);
        i += scalar.bytes;
        if constexpr (sizeof(wchar_t)==2) {
            if (scalar.value > 0xffff) {
                const auto cp=scalar.value-0x10000;
                out.push_back(wchar_t(0xd800+(cp>>10)));
                out.push_back(wchar_t(0xdc00+(cp&0x3ff)));
                continue;
            }
        }
        out.push_back(wchar_t(scalar.value));
    }
    return out;
}
inline std::string expand_continues(std::string_view text, unsigned count) {
    std::string result(text);
    if (const auto position = text.find(continue_token); position != std::string_view::npos)
        result.replace(position, continue_token.size(), std::to_string(count % 100));
    return result;
}

class ChineseDisplayPack {
    static constexpr std::array<std::size_t,10> counts={48,70,22,104,114,62,51,94,12,300};
    static constexpr std::size_t records_per_language=877;
    std::array<std::array<std::string,records_per_language>,2> rows_;

    static constexpr bool allowed(std::uint32_t c) {
        return (c>=0x20&&c<=0x7e)||c==0xb7||c==0x2014||c==0x2018||c==0x2019||c==0x201c||
            c==0x201d||c==0x2022||c==0x2026||c==0x2212||c==0x2605||c==0x2606||
            (c>=0x3000&&c<=0x303f)||(c>=0x3041&&c<=0x3096)||(c>=0x30a1&&c<=0x30fa)||
            (c>=0x3400&&c<=0x4dbf)||(c>=0x4e00&&c<=0x9fff)||(c>=0xff01&&c<=0xff9f)||
            (c>=0xffe0&&c<=0xffe6);
    }
public:
    static constexpr std::size_t max_size=256*1024;
    static ChineseDisplayPack parse(std::string_view data) {
        ChineseDisplayPack result;
        if(data.empty()||data.size()>max_size||!data.ends_with('\n'))throw std::runtime_error("Invalid Chinese display pack size or line endings");
        const auto next=[&] {
            const auto end=data.find('\n');
            if(end==std::string_view::npos)throw std::runtime_error("Truncated Chinese display pack");
            const auto row=data.substr(0,end);data.remove_prefix(end+1);return row;
        };
        if(next()!="SOEW-CHINESE-V1")throw std::runtime_error("Unsupported Chinese display pack header");
        for(std::size_t variant=0;variant<result.rows_.size();++variant){
            if(next()!=(variant==0?"locale\tzh-Hans":"locale\tzh-Hant"))throw std::runtime_error("Missing or out-of-order Chinese locale");
            std::size_t offset=0;
            for(std::size_t asset=0;asset<counts.size();++asset){
                const auto name=locale_text_assets[asset];
                const auto header="asset\t"+std::string(name)+"\t"+std::to_string(counts[asset]);
                if(next()!=header)throw std::runtime_error("Invalid Chinese asset header: "+std::string(name));
                for(std::size_t index=0;index<counts[asset];++index){
                    const auto prefix=std::to_string(index)+"\t";
                    const auto row=next();
                    if(!row.starts_with(prefix))throw std::runtime_error("Invalid Chinese display record index: "+std::string(name));
                    const auto text=row.substr(prefix.size());
                    const bool requires_token=asset==3&&index==84;
                    if((text.find(continue_token)!=std::string_view::npos)!=requires_token)
                        throw std::runtime_error("Invalid Chinese Continue placeholder");
                    const auto visible=expand_continues(text,12);
                    for(std::size_t at=0;at<visible.size();){
                        const auto cp=utf8_scalar(visible,at);at+=cp.bytes;
                        if(!allowed(cp.value)||cp.value=='{'||cp.value=='}')throw std::runtime_error("Unsupported Chinese display character");
                    }
                    const int max_width=asset<6?288:asset<9?352:320;
                    if(chinese_text_width(visible)>max_width)throw std::runtime_error("Chinese display record is too wide: "+std::string(name));
                    result.rows_[variant][offset++]=text;
                }
            }
            if(offset!=records_per_language)throw std::logic_error("Chinese display record layout");
        }
        if(!data.empty())throw std::runtime_error("Trailing Chinese display records");
        return result;
    }
    template<class Verify>
    static std::optional<ChineseDisplayPack> load(const std::filesystem::path& root, Verify&& verify) {
        const auto path=root/"locale"/"chinese-v1.txt";
        if(!std::filesystem::exists(path))return std::nullopt;
        if(!std::filesystem::is_regular_file(path))throw std::runtime_error("Chinese display pack is not a regular file");
        std::ifstream input(path,std::ios::binary);
        if(!input)throw std::runtime_error("Cannot read Chinese display pack");
        std::string bytes(max_size+1,'\0');
        input.read(bytes.data(),static_cast<std::streamsize>(bytes.size()));
        if(input.bad())throw std::runtime_error("Cannot read Chinese display pack");
        bytes.resize(static_cast<std::size_t>(input.gcount()));
        verify(std::string_view(bytes));
        return parse(bytes);
    }
    std::string_view line(Language language,std::string_view name,std::size_t index) const {
        if(!is_chinese(language))throw std::invalid_argument("Expected Chinese display language");
        std::size_t offset=0;
        for(std::size_t asset=0;asset<counts.size();++asset){
            if(locale_text_assets[asset]==name){
                if(index>=counts[asset])break;
                return rows_[language==Language::ChineseTraditional?1:0][offset+index];
            }
            offset+=counts[asset];
        }
        throw std::out_of_range("Unknown Chinese display record");
    }
};

inline bool has_locale_assets(const std::filesystem::path& root) {
    return std::filesystem::exists(root / "locale");
}

inline std::filesystem::path locale_asset(const std::filesystem::path& root, Language language, std::string_view name) {
    if (!has_locale_assets(root)) return root / name;
    const auto base = root / "locale";
    return base / (name == "OP2.PI" || name == "OP.RGB" ? "common" : localized(language, "ja", "en")) / name;
}

inline void validate_locale_assets(const std::filesystem::path& root) {
    if (!has_locale_assets(root)) return;
    const auto require = [](const std::filesystem::path& path) {
        if (!std::filesystem::is_regular_file(path))
            throw std::runtime_error("Incomplete locale overlay: " + path.filename().string());
    };
    require(root / "locale" / "manifest.json");
    for (const auto lang : {Language::Japanese, Language::English})
        for (const auto name : locale_text_assets) require(locale_asset(root, lang, name));
    require(locale_asset(root, Language::Japanese, "OP2.PI"));
    require(locale_asset(root, Language::Japanese, "OP.RGB"));
}
}
