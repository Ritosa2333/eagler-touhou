#pragma once

#include "ending_renderer.hpp"
#include "enemy_math.hpp"
#include "th03/assets/decode.hpp"

#include <cstdlib>
#include <functional>
#include <string>

namespace soew {

class EndingSurface;
enum class EndingSurfaceText { Cp932, Utf8 };

struct EndingSurfaceHooks {
    std::function<std::vector<std::byte>(std::string_view)> read_asset;
    // Draw a transparent 8/16×16 text mask with EndingSurface::put_pixel().
    // PC-98 ROM glyphs and FX_WEIGHT_BOLD are host responsibilities; a GDI font
    // is a visual approximation. The string view is valid only for this call.
    std::function<void(EndingSurface&, EndingSurfaceText, std::string_view,
                       int, int, std::uint8_t)> draw_text;
    std::function<void(std::string_view)> play_song;
    std::function<void(const EndingVerdict&)> register_score;
    std::function<std::string_view(std::string_view, std::size_t)> chinese_line;
};

// Indexed pixels preserve the original global palette behavior, including
// recoloring already-drawn artwork/credits. One instance per ending playback.
class EndingSurface {
public:
    static constexpr int width = 640, height = 400;

    explicit EndingSurface(EndingSurfaceHooks hooks, Language language=Language::Japanese) : hooks_(std::move(hooks)), language_(language) {
        if (!hooks_.read_asset || !hooks_.draw_text)
            throw std::invalid_argument("Ending surface needs asset and text hooks");
    }

    [[nodiscard]] std::span<const std::uint8_t> indices() const { return page_; }
    [[nodiscard]] int tone() const { return tone_; }

    void put_pixel(int x, int y, std::uint8_t ink) {
        if (x >= 0 && x < width && y >= 0 && y < height && ink < 16)
            page_[y * width + x] = ink;
    }

    // Destination is the host Frame pixel array (0x00RRGGBB, top-left origin).
    void render(std::span<std::uint32_t> destination) const {
        if (destination.size() != page_.size())
            throw std::invalid_argument("Ending surface requires a 640x400 destination");
        std::array<std::uint32_t, 16> rgb{};
        const auto channel = [this](int c) {
            // master.lib/palette_show.asm scales the hardware nibble before
            // expansion to RGB8, rather than fading an already expanded pixel.
            return std::uint32_t(17 * (tone_ <= 100 ? c * tone_ / 100
                : 15 - ((15 - c) * (200 - tone_) / 100)));
        };
        for (std::size_t i = 0; i < rgb.size(); ++i) {
            const auto c = palette_.colors[i];
            rgb[i] = (channel(c.r) << 16) | (channel(c.g) << 8) | channel(c.b);
        }
        std::transform(page_.begin(), page_.end(), destination.begin(),
            [&rgb](auto index) { return rgb[index]; });
    }

    void apply(const EndingDrawStep& step, const EndingVerdict& verdict) {
        using D = EndingDrawStep::Kind;
        const auto& e = step.event;
        switch (step.kind) {
        case D::Event: instant(e, verdict); break;
        case D::Tone: tone_ = std::clamp(step.value, 0, 200); break;
        case D::RevealText:
            if (e.kind == EndingEventKind::TypeLine) {
                narrative_line(e.asset, e.a, step.value, ending_line_rect.x,
                    ending_line_rect.y, static_cast<std::uint8_t>(e.ink));
            } else {
                const auto line = ending_staff_lines(EndingStaffText::Title,language_).front();
                if (is_chinese(language_)) {
                    const auto prefix = chinese_text_prefix(bytes(line.text), step.value * 16);
                    hooks_.draw_text(*this, EndingSurfaceText::Utf8, prefix, line.x, line.y, 15);
                } else {
                    // The legacy titles contain only three-byte full-width characters.
                    text(line.text.substr(0, std::size_t(step.value) * 3), line.x, line.y);
                }
            }
            break;
        case D::ClearLine: clear(ending_line_rect); break;
        case D::ScrollRows: scroll_rows(e.a, step.value, step.aux); break;
        case D::SlidePicture: slide_picture(step.aux, step.value); break;
        case D::TitleCel: title_cel(step.value, 184); break;
        case D::MoveTitle:
            clear({528, step.value + 4, 64, 16});
            title_cel(50, step.value);
            text(ending_version, 528, step.value + 8);
            break;
        case D::ClearStaffText: clear(ending_staff_clear_rect); break;
        case D::RotateStaff: rotate_staff(step.value, e.b, e.c); break;
        case D::FinishStaffRotate:
            clear({0, 0, 385, height});
            line(28, 96, 356, 96, 15);
            line(356, 96, 356, 304, 15);
            line(356, 304, 28, 304, 15);
            line(28, 304, 28, 96, 15);
            copy_quarter(e.a, 32, 100);
            tone_ = 200;
            break;
        case D::VerdictMask: verdict_mask(e, step.value, verdict); break;
        }
    }

private:
    using Palette = th03::assets::Palette16Rgb4;
    using Sheet = th03::assets::BfntSheet;
    using Page = std::vector<std::uint8_t>;
    EndingSurfaceHooks hooks_;
    Language language_;
    std::u8string_view verdict_label(int row) const {
        return (language_==Language::ChineseSimplified ? ending_verdict_labels_hans :
            language_==Language::ChineseTraditional ? ending_verdict_labels_hant :
            language_==Language::English ? ending_verdict_labels_en : ending_verdict_labels).at(std::size_t(row));
    }
    Page page_ = Page(width * height);
    Page verdict_page_;
    Palette palette_{};
    std::optional<th03::assets::PiImage> picture_;
    std::optional<Sheet> gaiji_, title_;
    EndingText text_;
    std::string loaded_text_;
    int tone_ = 100;

    static std::runtime_error asset_error(std::string_view name, std::string_view reason) {
        return std::runtime_error(std::string(name) + ": " + std::string(reason));
    }

    void clear(EndingRect r) {
        for (int y = r.y; y < r.y + r.height; ++y)
            std::fill_n(page_.begin() + y * width + r.x, r.width, std::uint8_t{0});
    }

    void load_font(std::string_view asset) {
        const auto bytes = hooks_.read_asset(asset);
        using namespace th03::assets;
        auto data = std::span<const std::byte>(bytes);
        const bool gaiji = asset == "MIKOFT.BFT";
        // TH02's original MIKOFT contains the same verified trailing 0x42
        // already excluded by the main frontend's strict decoder call.
        if (gaiji && data.size() == 8225 && data.back() == std::byte{0x42})
            data = data.first(8224);
        auto font = decode_bfnt(data, gaiji ? BfntConsumerPolicy::gaiji
            : BfntConsumerPolicy::superimpose);
        if (!font) throw asset_error(asset, font.error().message);
        if (font->header.pixel_width != 16 || font->header.pixel_height != (gaiji ? 16 : 32)
            || font->header.first_code != 0 || font->glyphs.size() < (gaiji ? 256u : 55u))
            throw asset_error(asset, "unexpected ending font geometry");
        (gaiji ? gaiji_ : title_) = std::move(*font);
    }

    void load_picture(std::string_view asset, bool set_palette) {
        auto image = th03::assets::decode_pi(hooks_.read_asset(asset));
        if (!image) throw asset_error(asset, image.error().message);
        // BUT.PI and EXTRA.PI replace only the top-left rectangle of ALL.PI.
        const int expected_width = asset == "BUT.PI" ? 288 : asset == "EXTRA.PI" ? 320 : width;
        const int expected_height = expected_width == width ? height : 160;
        if (image->image.width != expected_width || image->image.height != expected_height)
            throw asset_error(asset, "unexpected ending image geometry");
        picture_ = std::move(*image);
        if (set_palette) apply_picture_palette();
    }

    void apply_picture_palette() {
        palette_ = picture_.value().palette.value();
    }

    void instant(const EndingEvent& e, const EndingVerdict& verdict) {
        using K = EndingEventKind;
        switch (e.kind) {
        case K::LoadText:
            text_ = EndingText::decode(hooks_.read_asset(e.asset), e.asset);
            loaded_text_ = e.asset;
            break;
        case K::PlaySong:
            if (hooks_.play_song) hooks_.play_song(e.asset);
            break;
        case K::LoadFont: load_font(e.asset); break;
        case K::LoadPicture: load_picture(e.asset, e.a != 0); break;
        case K::ShowQuarter: copy_quarter(e.a, e.b, 100); break;
        case K::VerdictBackdrop:
        case K::FullPicture: {
            load_picture(e.asset, true);
            const auto& image = picture_->image;
            for (int y = 0; y < image.height; ++y)
                std::copy_n(image.indices.begin() + y * image.width, image.width,
                    page_.begin() + y * width);
            break;
        }
        case K::ApplyPicturePalette: apply_picture_palette(); break;
        case K::LoadPalette: {
            auto palette = th03::assets::decode_rgb48(hooks_.read_asset(e.asset));
            if (!palette) throw asset_error(e.asset, palette.error().message);
            palette_ = *palette;
            break;
        }
        case K::StaffVersion: text(ending_version, 528, 192); break;
        case K::StaffText:
            for (const auto label : ending_staff_lines(static_cast<EndingStaffText>(e.a),language_))
                text(label.text, label.x, label.y);
            break;
        case K::ClearScreen: std::fill(page_.begin(), page_.end(), std::uint8_t{0}); break;
        case K::RegisterScore:
            if (hooks_.register_score) hooks_.register_score(verdict);
            break;
        default: throw std::logic_error("Ending surface received a timed event instead of a draw step");
        }
    }

    void copy_quarter(int quarter, int x, int y, int source_row = 0, int rows = 200) {
        const auto q = ending_quarter_rect(quarter);
        const auto& source = picture_.value().image.indices;
        for (int dy = 0; dy < rows; ++dy)
            std::copy_n(source.begin() + (q.y + source_row + dy) * width + q.x,
                q.width, page_.begin() + (y + dy) * width + x);
    }

    void scroll_rows(int quarter, int from, int rows) {
        const auto [x, y, w, h] = ending_picture_rect;
        for (int row = h - 1; row >= rows; --row)
            std::copy_n(page_.begin() + (y + row - rows) * width + x, w,
                page_.begin() + (y + row) * width + x);
        copy_quarter(quarter, x, y, from, rows);
    }

    void slide_picture(int old_x, int new_x) {
        const auto [x, y, w, h] = ending_picture_rect;
        const int delta = old_x - new_x;
        for (int dy = 0; dy < h; ++dy) {
            auto row = page_.begin() + (y + dy) * width;
            std::copy_n(row + old_x, w - delta, row + new_x);
            std::fill(row + new_x + w - delta, row + old_x + w, std::uint8_t{0});
        }
    }

    static std::string_view bytes(std::u8string_view label) {
        return {reinterpret_cast<const char*>(label.data()), label.size()};
    }
    void text(std::u8string_view label, int x, int y) {
        hooks_.draw_text(*this, EndingSurfaceText::Utf8, bytes(label), x, y, 15);
    }

    void narrative_line(std::string_view asset, int record, int count,
                        int x, int y, std::uint8_t ink) {
        if (asset != loaded_text_) throw std::logic_error("Ending text asset is not active");
        if (is_chinese(language_) && hooks_.chinese_line) {
            const auto label = hooks_.chinese_line(asset, std::size_t(record));
            const auto prefix = chinese_text_prefix(label, count * 8);
            if (!prefix.empty()) hooks_.draw_text(*this, EndingSurfaceText::Utf8, prefix, x, y, ink);
            return;
        }
        const auto& line = text_.lines.at(std::size_t(record));
        count = std::min<int>(count, int(line.size()));
        // Track encoded characters separately from the typewriter pair count.
        // GDI/CP932 cannot draw a half-decoded lead byte at the revealed edge.
        int end = 0;
        while (end < count) {
            const auto c = std::to_integer<unsigned>(line[end]);
            if (!c) break;
            const int bytes = ((c >= 0x81 && c <= 0x9f) || (c >= 0xe0 && c <= 0xfc)) ? 2 : 1;
            if (end + bytes > count) break;
            end += bytes;
        }
        if (end) hooks_.draw_text(*this, EndingSurfaceText::Cp932,
            {reinterpret_cast<const char*>(line.data()), std::size_t(end)}, x, y, ink);
    }

    void title_cel(int base, int top) {
        // over_put_8 writes all four color planes, including index 0.
        for (int segment = 0; segment < 5; ++segment) {
            const auto& glyph = title_.value().glyphs.at(std::size_t(base + segment));
            const auto& bitmap = std::get<th03::assets::PlanarBitmap4>(glyph.pixels);
            for (int y = 0; y < 32; ++y) for (int x = 0; x < 16; ++x) {
                std::uint8_t ink = 0;
                for (unsigned plane = 0; plane < 4; ++plane)
                    if (bitmap.color_planes[plane][y * 2 + x / 8] & (0x80 >> (x % 8)))
                        ink |= std::uint8_t(1u << plane);
                put_pixel(432 + segment * 16 + x, top + y, ink);
            }
        }
    }

    // ReC98 staff.cpp's geometry and trailing rectangles; software Bresenham
    // and clipping approximate GRCG line pixels at ties and screen edges.
    void line(int x, int y, int x2, int y2, std::uint8_t ink) {
        const int dx = std::abs(x2 - x), dy = -std::abs(y2 - y);
        const int sx = x < x2 ? 1 : -1, sy = y < y2 ? 1 : -1;
        int error = dx + dy;
        for (;;) {
            put_pixel(x, y, ink);
            if (x == x2 && y == y2) break;
            const int e = 2 * error;
            if (e >= dy) { error += dy; x += sx; }
            if (e <= dx) { error += dx; y += sy; }
        }
    }

    void rotating_rectangle(int step, int speed, int initial_angle, std::uint8_t ink) {
        const int n = std::clamp(step, 0, 16);
        const unsigned angle = unsigned(initial_angle + n * speed);
        const int radius = 256 - n * 4;
        constexpr std::array<unsigned, 4> offsets{0, 0x2f, 0x80, 0xaf};
        std::array<std::array<int, 2>, 4> points{};
        for (std::size_t i = 0; i < points.size(); ++i) points[i] = enemy_vector(angle + offsets[i], radius);
        for (std::size_t i = 0; i < points.size(); ++i) {
            const auto a = points[i], b = points[(i + 1) % points.size()];
            line(192 + a[0], 200 + a[1], 192 + b[0], 200 + b[1], ink);
        }
    }

    void rotate_staff(int frame, int speed, int start) {
        if (!frame) clear({0, 0, 385, height});
        rotating_rectangle(frame - 4, speed, start, 0);
        for (int i = frame - 3; i < frame; ++i) rotating_rectangle(i, speed, start, 4);
        rotating_rectangle(frame + 1, speed, start, 15);
    }

    void value(std::string_view label, int x, int y) {
        const auto& font = gaiji_.value();
        for (const char c : label) {
            if (c != ' ') {
                const int code = c >= '0' && c <= '9' ? 0xa0 + c - '0'
                    : c == 'M' ? 0xb7 : c == 'N' ? 0xb6 : 0xaa + c - 'A';
                const auto& mono = std::get<th03::assets::MonochromeBitmap1>(font.glyphs.at(code).pixels);
                for (int dy = 0; dy < 16; ++dy) for (int dx = 0; dx < 16; ++dx)
                    if (mono.bits[dy * 2 + dx / 8] & (0x80 >> (dx % 8))) put_pixel(x + dx, y + dy, 15);
            }
            x += 16;
        }
    }

    static constexpr int verdict_y(int row) { return 96 + row * 32; }
    static constexpr int label_x = 64, value_x = 208, single_value_x = 272;

    static int text_width(std::u8string_view label) {
        int pixels = 0;
        for (const auto byte : label) {
            const auto c = static_cast<unsigned char>(byte);
            pixels += c < 0x80 ? 8 : c >= 0xc0 ? 16 : 0;
        }
        return pixels;
    }

    EndingRect verdict_rect(const EndingEvent& e) const {
        const int y = verdict_y(e.a);
        if (e.kind == EndingEventKind::VerdictSkillValue)
            return {value_x, y, (3 + (44 - 1) / 2) * 16, 16};
        const auto label = verdict_label(e.a);
        const bool chinese = is_chinese(language_);
        const int label_width = chinese ? chinese_text_width(bytes(label)) : text_width(label);
        if (e.a == 0) return {label_x + 136 - label_width / 2, y, label_width, 16};
        if (e.a == 6 || e.a == 8) return {label_x, y, chinese ? label_width : label_width / 16 * 16, 16};
        return {label_x, y, 272, 16};
    }

    void draw_verdict(const EndingEvent& e, const EndingVerdict& data, int x, int y) {
        if (e.kind == EndingEventKind::VerdictSkillValue) {
            const auto skill = std::to_string(data.skill);
            value(skill, value_x + (3 - int(skill.size())) * 16, y);
            narrative_line(e.asset, e.b, 44, value_x + 48, y, 15);
            return;
        }
        text(verdict_label(e.a), x, y);
        switch (e.a) {
        case 1: {
            if (const auto score = data.score % 100000000u) {
                const auto str = std::to_string(score);
                value(str, value_x + (8 - int(str.size())) * 16, y);
            }
            break;
        }
        // Real TH02 gameplay supplies these in 0..9. Larger host counters are
        // shown as decimal text instead of indexing arbitrary sprite glyphs.
        case 2: value(std::to_string(data.continues_used), single_value_x, y); break;
        case 3:
            if (is_chinese(language_)) {
                const auto& labels=language_==Language::ChineseSimplified?ending_verdict_rank_hans:ending_verdict_rank_hant;
                text(labels.at(data.rank), single_value_x-16, y);
            } else value(ending_verdict_rank.at(data.rank), 224, y);
            break;
        case 4: value(std::to_string(std::max(0, data.initial_players)), single_value_x, y); break;
        case 5: value(std::to_string(std::max(0, data.initial_bombs)), single_value_x, y); break;
        default: break;
        }
    }

    void verdict_mask(const EndingEvent& e, int mask, const EndingVerdict& data) {
        const auto [x, y, w, h] = verdict_rect(e);
        if (mask == 0) {
            // Render the completed row once, then copy only the newly exposed
            // pixels over the backdrop, like the two original VRAM pages.
            verdict_page_ = page_;
            std::swap(page_, verdict_page_);
            try { draw_verdict(e, data, x, y); }
            catch (...) { std::swap(page_, verdict_page_); throw; }
            std::swap(page_, verdict_page_);
        }
        // ReC98 th02/sprites/verdict.bmp: three 16×16 one-bit cels; each
        // repeats every four rows. BMP row ordering is bottom-to-top.
        constexpr std::array<std::array<unsigned, 4>, 3> rows{{
            {{0xaaaa, 0x5555, 0xaaaa, 0x5555}},
            {{0x3333, 0x3333, 0xcccc, 0xcccc}},
            {{0xcccc, 0xcccc, 0x3333, 0x3333}}
        }};
        for (int dy = 0; dy < h; ++dy) for (int dx = 0; dx < w; ++dx) {
            const int i = (y + dy) * width + x + dx;
            if (rows.at(std::size_t(mask))[dy % 4] & (0x8000u >> (dx % 16)))
                page_[i] = verdict_page_[i];
        }
    }
};

} // namespace soew
