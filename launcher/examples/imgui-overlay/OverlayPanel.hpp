#pragma once
#include <eagler/overlay/ConnectionStatus.hpp>
#include <SDL3/SDL.h>
#include <eagler/thprac/PracticePointer.hpp>
#include <imgui.h>
#include <imgui_internal.h>
#include <nlohmann/json.hpp>
#include <algorithm>
#include <array>
#include <cfloat>
#include <cmath>
#include <deque>
#include <functional>
#include <limits>
#include <string>
#include <variant>
#include <vector>

using Json = nlohmann::json;
namespace th09::overlay {
constexpr auto flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove |
    ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings;

inline std::string stringField(const Json& obj, const char* field, std::string fallback = {}) {
    if (!obj.is_object()) return fallback;
    const auto found = obj.find(field);
    return found != obj.end() && found->is_string() ? found->get<std::string>() : fallback;
}

template<class T>
T field(const Json& obj, const char* key, T fallback) {
    if (!obj.is_object()) return fallback;
    const auto found = obj.find(key);
    if (found == obj.end() || found->is_null()) return fallback;
    try { return found->get<T>(); }
    catch (const Json::exception&) { return fallback; }
}

inline std::string label(const std::string& text, const char* id) { return text + "###" + id; }
inline void text(const std::string& value) { ImGui::TextUnformatted(value.c_str()); }
inline void dim(const std::string& value) { ImGui::TextDisabled("%s", value.c_str()); }

inline void theme() {
    ImGui::StyleColorsLight();
    auto& s = ImGui::GetStyle();
    s.WindowPadding = {10, 8};
    s.WindowMinSize = {1, 1};
    s.FramePadding = {8, 4};
    s.ItemSpacing = {6, 5};
    s.FrameBorderSize = 1;
    s.ScrollbarSize = 9;
    // Everforest by DestroyerDarkNess, from Patitotective/ImThemes.
    auto* colors = s.Colors;
    colors[ImGuiCol_Text] = {0.8745f, 0.8706f, 0.8392f, 1.0000f};
    colors[ImGuiCol_TextDisabled] = {0.5843f, 0.5725f, 0.5216f, 1.0000f};
    colors[ImGuiCol_WindowBg] = {0.2353f, 0.2196f, 0.2118f, 1.0000f};
    colors[ImGuiCol_ChildBg] = {0.2353f, 0.2196f, 0.2118f, 1.0000f};
    colors[ImGuiCol_PopupBg] = {0.2353f, 0.2196f, 0.2118f, 1.0000f};
    colors[ImGuiCol_Border] = {0.3137f, 0.2863f, 0.2706f, 1.0000f};
    colors[ImGuiCol_BorderShadow] = {0.2353f, 0.2196f, 0.2118f, 0.0000f};
    colors[ImGuiCol_FrameBg] = {0.3137f, 0.2863f, 0.2706f, 1.0000f};
    colors[ImGuiCol_FrameBgHovered] = {0.4000f, 0.3608f, 0.3294f, 1.0000f};
    colors[ImGuiCol_FrameBgActive] = {0.4863f, 0.4353f, 0.3922f, 1.0000f};
    colors[ImGuiCol_TitleBg] = {0.2353f, 0.2196f, 0.2118f, 1.0000f};
    colors[ImGuiCol_TitleBgActive] = {0.3137f, 0.2863f, 0.2706f, 1.0000f};
    colors[ImGuiCol_TitleBgCollapsed] = {0.2353f, 0.2196f, 0.2118f, 1.0000f};
    colors[ImGuiCol_MenuBarBg] = {0.3137f, 0.2863f, 0.2706f, 1.0000f};
    colors[ImGuiCol_ScrollbarBg] = {0.2353f, 0.2196f, 0.2118f, 1.0000f};
    colors[ImGuiCol_ScrollbarGrab] = {0.4863f, 0.4353f, 0.3922f, 1.0000f};
    colors[ImGuiCol_ScrollbarGrabHovered] = {0.4000f, 0.3608f, 0.3294f, 1.0000f};
    colors[ImGuiCol_ScrollbarGrabActive] = {0.3137f, 0.2863f, 0.2706f, 1.0000f};
    colors[ImGuiCol_CheckMark] = {0.5961f, 0.5922f, 0.1020f, 1.0000f};
    colors[ImGuiCol_SliderGrab] = {0.5961f, 0.5922f, 0.1020f, 1.0000f};
    colors[ImGuiCol_SliderGrabActive] = {0.7412f, 0.7176f, 0.4196f, 1.0000f};
    colors[ImGuiCol_Button] = {0.4000f, 0.3608f, 0.3294f, 1.0000f};
    colors[ImGuiCol_ButtonHovered] = {0.4863f, 0.4353f, 0.3922f, 1.0000f};
    colors[ImGuiCol_ButtonActive] = {0.7412f, 0.7176f, 0.4196f, 1.0000f};
    colors[ImGuiCol_Header] = {0.4000f, 0.3608f, 0.3294f, 1.0000f};
    colors[ImGuiCol_HeaderHovered] = {0.4863f, 0.4353f, 0.3922f, 1.0000f};
    colors[ImGuiCol_HeaderActive] = {0.7412f, 0.7176f, 0.4196f, 1.0000f};
    colors[ImGuiCol_Separator] = {0.7412f, 0.7176f, 0.4196f, 1.0000f};
    colors[ImGuiCol_SeparatorHovered] = {0.4863f, 0.4353f, 0.3922f, 1.0000f};
    colors[ImGuiCol_SeparatorActive] = {0.7412f, 0.7176f, 0.4196f, 1.0000f};
    colors[ImGuiCol_ResizeGrip] = {0.4000f, 0.3608f, 0.3294f, 1.0000f};
    colors[ImGuiCol_ResizeGripHovered] = {0.4863f, 0.4353f, 0.3922f, 1.0000f};
    colors[ImGuiCol_ResizeGripActive] = {0.7412f, 0.7176f, 0.4196f, 1.0000f};
    colors[ImGuiCol_Tab] = {0.3137f, 0.2863f, 0.2706f, 1.0000f};
    colors[ImGuiCol_TabHovered] = {0.4000f, 0.3608f, 0.3294f, 1.0000f};
    colors[ImGuiCol_TabActive] = {0.4863f, 0.4353f, 0.3922f, 1.0000f};
    colors[ImGuiCol_TabUnfocused] = {0.2353f, 0.2196f, 0.2118f, 0.9725f};
    colors[ImGuiCol_TabUnfocusedActive] = {0.3137f, 0.2863f, 0.2706f, 1.0000f};
    colors[ImGuiCol_PlotLines] = {0.7412f, 0.7176f, 0.4196f, 1.0000f};
    colors[ImGuiCol_PlotLinesHovered] = {0.8392f, 0.7490f, 0.4000f, 1.0000f};
    colors[ImGuiCol_PlotHistogram] = {0.7412f, 0.7176f, 0.4196f, 1.0000f};
    colors[ImGuiCol_PlotHistogramHovered] = {0.8392f, 0.7490f, 0.4000f, 1.0000f};
    colors[ImGuiCol_TableHeaderBg] = {0.8392f, 0.7490f, 0.4000f, 0.6094f};
    colors[ImGuiCol_TableBorderStrong] = {0.3098f, 0.3098f, 0.3490f, 1.0000f};
    colors[ImGuiCol_TableBorderLight] = {0.2275f, 0.2275f, 0.2471f, 1.0000f};
    colors[ImGuiCol_TableRowBg] = {0.0000f, 0.0000f, 0.0000f, 0.0000f};
    colors[ImGuiCol_TableRowBgAlt] = {1.0000f, 1.0000f, 1.0000f, 0.0600f};
    colors[ImGuiCol_TextSelectedBg] = {0.8392f, 0.7490f, 0.4000f, 0.4314f};
    colors[ImGuiCol_DragDropTarget] = {0.8392f, 0.7490f, 0.4000f, 0.9020f};
    colors[ImGuiCol_NavHighlight] = {0.2353f, 0.2196f, 0.2118f, 1.0000f};
    colors[ImGuiCol_NavWindowingHighlight] = {1.0000f, 1.0000f, 1.0000f, 0.7000f};
    colors[ImGuiCol_NavWindowingDimBg] = {0.8000f, 0.8000f, 0.8000f, 0.2000f};
    colors[ImGuiCol_ModalWindowDimBg] = {0.8000f, 0.8000f, 0.8000f, 0.3500f};
    s.WindowRounding = 6.0f;
    s.ChildRounding = 0.0f;
    s.PopupRounding = 0.0f;
    s.FrameRounding = 3.0f;
    s.ScrollbarRounding = 16.0f;
    s.GrabRounding = 2.0f;
}

inline float buttonWidth(const char* label) {
    return std::ceil(ImGui::CalcTextSize(label, nullptr, true).x + 2 * ImGui::GetStyle().FramePadding.x);
}

inline void sameLineIfFits(float nextWidth) {
    const auto right = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
    if (ImGui::GetItemRectMax().x + ImGui::GetStyle().ItemSpacing.x + nextWidth <= right)
        ImGui::SameLine();
}

inline void textLine(const std::string& value, float width, ImGuiCol tint = ImGuiCol_Text) {
    const auto pos = ImGui::GetCursorScreenPos();
    const auto height = ImGui::GetTextLineHeight();
    width = std::max(1.f, width);
    ImGui::Dummy({width, height});
    if (!ImGui::IsItemVisible()) return;
    ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(tint));
    ImGui::RenderTextEllipsis(ImGui::GetWindowDrawList(), pos, {pos.x + width, pos.y + height},
                              pos.x + width, pos.x + width, value.data(), value.data() + value.size(), nullptr);
    ImGui::PopStyleColor();
}

inline void cardText(const std::string& name, const std::string& detail, float actionWidth, float leading = 0) {
    constexpr float inset = 8;
    const float width = ImGui::GetWindowWidth() - 2 * inset - leading -
        (actionWidth > 0 ? actionWidth + ImGui::GetStyle().ItemSpacing.x : 0);
    ImGui::SetCursorPos({inset + leading, 4});
    textLine(name, width);
    ImGui::SetCursorPos({inset + leading, 5 + ImGui::GetTextLineHeight()});
    textLine(detail, width);
    if (actionWidth > 0)
        ImGui::SetCursorPos({ImGui::GetWindowWidth() - actionWidth - inset,
                             (ImGui::GetWindowHeight() - ImGui::GetFrameHeight()) / 2});
}

inline void beginDisabled(bool disabled) {
    if (disabled) {
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, .5f * ImGui::GetStyle().Alpha);
    }
}
inline void endDisabled(bool disabled) {
    if (disabled) {
        ImGui::PopStyleVar();
        ImGui::PopItemFlag();
    }
}

inline void heading(const char* value, float scale = 1.35f) {
    ImGui::SetWindowFontScale(scale);
    text(value);
    ImGui::SetWindowFontScale(1);
}

struct Rect {
    float x = 0, y = 0, w = 0, h = 0;
    bool contains(float px, float py) const { return px >= x && py >= y && px < x + w && py < y + h; }
};
inline void place(Rect rect) {
    ImGui::SetNextWindowPos({rect.x, rect.y});
    ImGui::SetNextWindowSize({rect.w, rect.h});
}

class Overlay {
public:
    Json state = Json::object();
    bool visible = true;
    bool reconnecting = false;
    bool escapeAvailable = false;
    bool hasWaitNotice() const {
        return (inGame() && (reconnecting || escapeAvailable)) || (!visible && escapeAvailable);
    }
    ImTextureID cover = nullptr;
    std::function<void(const Json&)> dispatch;
    std::function<ImTextureID(const std::string&)> avatarFor;

    bool event(const SDL_Event& e, SDL_Window* window) {
        auto& io = ImGui::GetIO();
        if (e.type == SDL_EVENT_WINDOW_FOCUS_LOST) {
            releaseInput();
            SDL_StopTextInput(window);
            return false;
        }
        if (e.type == SDL_EVENT_KEY_DOWN || e.type == SDL_EVENT_KEY_UP) {
            const bool down = e.type == SDL_EVENT_KEY_DOWN;
            if (e.key.scancode == SDL_SCANCODE_F2) {
                if (down && !e.key.repeat) {
                    if (visible) {
                        hide();
                        SDL_StopTextInput(window);
                    } else visible = true;
                }
                return true;
            }
            if (editing() && !composition.empty() &&
                (e.key.scancode == SDL_SCANCODE_RETURN || e.key.scancode == SDL_SCANCODE_KP_ENTER ||
                 e.key.scancode == SDL_SCANCODE_ESCAPE)) return true;
            if (inGame() && e.key.scancode == SDL_SCANCODE_LALT) {
                if (!editing()) {
                    if (!down && !e.key.repeat && !(e.key.mod & (SDL_KMOD_CTRL | SDL_KMOD_SHIFT | SDL_KMOD_GUI))) {
                        if (!visible) {
                            visible = true;
                            chatOpen = true;
                        } else toggleChat();
                    }
                    return true;
                }
            }
            if (visible && inGame() && chatOpen && !editing() && down && !e.key.repeat) {
                const auto index = static_cast<int>(e.key.scancode) - SDL_SCANCODE_1;
                if (index >= 0 && index < phrasesPerPage) {
                    const auto position = phrasePageOffset() + index;
                    if (const auto& values = phrases(); position < static_cast<int>(values.size()))
                        sendPhrase(values[position]);
                    return true;
                }
            }
            const bool captured = capturesKeyboard();
            const auto scan = e.key.scancode;
            if (scan > SDL_SCANCODE_UNKNOWN && static_cast<unsigned>(scan) < std::size(io.KeysDown)) {
                const bool mapped = modifierKey(scan) || std::find(std::begin(io.KeyMap), std::end(io.KeyMap), scan) != std::end(io.KeyMap);
                if (captured && !e.key.repeat && mapped) keyboardEvents.emplace_back(KeyChange{scan, e.key.mod, down});
                else if (!down && !captured) io.KeysDown[scan] = false;
            }
            return captured;
        }
        if (!visible) return false;
        if (e.type == SDL_EVENT_TEXT_EDITING && editing()) {
            composition = e.edit.text ? e.edit.text : "";
            return true;
        }
        if (e.type == SDL_EVENT_TEXT_INPUT && editing()) {
            composition.clear();
            if (e.text.text && *e.text.text) keyboardEvents.emplace_back(std::string(e.text.text));
            return true;
        }
        if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN || e.type == SDL_EVENT_MOUSE_BUTTON_UP) {
            if (e.button.button < 1 || e.button.button > 3) return false;
            const bool down = e.type == SDL_EVENT_MOUSE_BUTTON_DOWN;
            const bool consumed = pointer.route(down ? 0 : 2, e.button.button, hit(e.button.x, e.button.y));
            if (down && e.button.button == SDL_BUTTON_LEFT) {
                keyboardOwned = consumed;
                if (!keyboardOwned) {
                    composition.clear();
                    resetKeys();
                    ImGui::ClearActiveID();
                }
            }
            mousePosition = {e.button.x, e.button.y};
            const auto button = e.button.button == SDL_BUTTON_RIGHT ? 1 : e.button.button == SDL_BUTTON_MIDDLE ? 2 : 0;
            mouseDown[button] = down && consumed;
            if (mouseDown[button]) mousePressed[button] = true;
            return consumed;
        }
        if (e.type == SDL_EVENT_MOUSE_MOTION) {
            mousePosition = {e.motion.x, e.motion.y};
            for (unsigned button = SDL_BUTTON_LEFT; button <= SDL_BUTTON_RIGHT; ++button)
                if (e.motion.state & SDL_BUTTON_MASK(button)) return pointer.route(1, button, false);
            return hit(e.motion.x, e.motion.y);
        }
        if (e.type == SDL_EVENT_MOUSE_WHEEL && hit(e.wheel.mouse_x, e.wheel.mouse_y)) {
            io.MouseWheel += e.wheel.y;
            io.MouseWheelH += e.wheel.x;
            return true;
        }
        return false;
    }

    void beginFrame() {
        auto& io = ImGui::GetIO();
        io.MousePos = mousePosition;
        // ImGui 1.82 samples button state once per frame.
        for (size_t i = 0; i < mouseDown.size(); ++i) {
            io.MouseDown[i] = mouseDown[i] || mousePressed[i];
            mousePressed[i] = false;
        }
        // ImGui applies committed text before key commands, so drain them in order across frames.
        bool hasText = false;
        while (!keyboardEvents.empty()) {
            if (const auto* text = std::get_if<std::string>(&keyboardEvents.front())) {
                io.AddInputCharactersUTF8(text->c_str());
                keyboardEvents.pop_front();
                hasText = true;
                continue;
            }
            if (hasText) break;
            const auto change = std::get<KeyChange>(keyboardEvents.front());
            const auto scan = change.scancode;
            const bool editPress = change.down && !modifierKey(scan);
            if (editPress && !io.KeysDown[scan] && io.KeysDownDuration[scan] >= 0.0f) break;
            keyboardEvents.pop_front();
            io.KeysDown[scan] = change.down;
            io.KeyShift = !!(change.modifiers & SDL_KMOD_SHIFT);
            io.KeyCtrl = !!(change.modifiers & SDL_KMOD_CTRL);
            io.KeyAlt = !!(change.modifiers & SDL_KMOD_ALT);
            io.KeySuper = !!(change.modifiers & SDL_KMOD_GUI);
            if (editPress) break;
        }
        ImGui::NewFrame();
    }

    void update(Json data, bool initial = false) {
        if (initial) chatOpen = field(data, "chatOpen", false);
        if (quickPhrases.empty()) {
            const auto rows = data.find("quickChat");
            if (rows != data.end() && rows->is_array()) {
                for (const auto& row : *rows)
                    for (const auto& phrase : row) quickPhrases.push_back(phrase);
            }
        }
        const auto chat = data.find("chat");
        const auto previous = state.find("chat");
        if (chat != data.end() && chat->is_array() && !chat->empty() &&
            (previous == state.end() || *previous != *chat)) chatUpdatedAt = SDL_GetTicks();
        const auto room = field(data, "room", Json(nullptr));
        const auto lastRoom = field(state, "room", Json(nullptr));
        const bool switchedRoom = stringField(room, "code") != stringField(lastRoom, "code");
        if (switchedRoom) {
            releaseInput();
            roomIssue.clear();
            settings = false;
            draft[0] = '\0';
            reportId = nullptr;
        }
        const auto receipt = field(data, "chatReceipt", Json(nullptr));
        const auto receiptId = field(receipt, "id", 0u);
        if (receiptId > lastReceipt) {
            lastReceipt = receiptId;
            if (stringField(receipt, "text") == draft.data()) {
                draft[0] = '\0';
                if (editing() && ImGui::GetActiveID() == draftInputId) ImGui::ClearActiveID();
            }
        }
        if (!field(data, "freeChat", true) && editing() && ImGui::GetActiveID() == draftInputId) ImGui::ClearActiveID();
        if (switchedRoom ||
            field(room, "adonisMode", 0) != field(lastRoom, "adonisMode", 0) ||
            field(room, "timingChoice", Json("auto")) != field(lastRoom, "timingChoice", Json("auto"))) {
            rollback = field(room, "adonisMode", 0) != 1;
            const auto choice = field(room, "timingChoice", Json("auto"));
            autoDelay = choice == "auto";
            timing = choice.is_number_integer() ? choice.get<int>() : 0;
        }
        state = std::move(data);
    }

    void setRuntimeProgress(const char* label, unsigned completed, unsigned total) {
        if (*label && runtimeProgress.empty()) roomIssue.clear();
        runtimeProgress = label;
        progressCompleted = completed;
        progressTotal = total;
    }

    void setRoomIssue(const char* message) { roomIssue = message; }

    void render(float width, float height) {
        area = {};
        trigger = {};
        inputArea = {};
        if (width < 180 || height < 120) return;
        if (hasWaitNotice()) {
            const auto message = escapeAvailable ? (inGame() ? "对手无响应 | Esc 返回标题" : "按 Esc 返回标题") : "正在重新连接对手…";
            place({6, 6, std::min(width - 12, buttonWidth(message) + 8), ImGui::GetFrameHeightWithSpacing() + 6});
            ImGui::Begin("###reconnecting", nullptr, flags | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoScrollbar);
            text(message);
            ImGui::End();
        }
        if (!visible) return;
        if (inGame()) {
            playChat(width, height);
            finishInputFrame();
            return;
        }
        constexpr float margin = 6;
        const float panelWidth = std::min(width - 2 * margin, std::clamp(width * .58f, 320.f, 420.f));
        area = {width - panelWidth - margin, margin, panelWidth, height - 2 * margin};
        const auto room = field(state, "room", Json(nullptr));
        const auto launchProgress = stringField(state, "launchStage");
        const bool joining = stringField(state, "scene") == "room" && !room.is_object();
        const bool connecting = room.is_object() && !field(state, "roomConnected", true);
        const bool closingRun = room.is_object() && field(state, "finishingRun", false) &&
            stringField(room, "phase") != "lobby";
        const bool showProgress = !runtimeProgress.empty() || !launchProgress.empty() || joining || connecting;
        const bool showComposer = room.is_object() && !showProgress;
        place(area);
        ImGui::Begin("联机###panel", nullptr, flags | ImGuiWindowFlags_NoScrollbar);
        const bool compact = height < 360;
        header(room.is_object(), compact);
        const float footer = ImGui::GetFrameHeightWithSpacing() + 2 +
            (showComposer ? ImGui::GetFrameHeightWithSpacing() : 0);
        const auto problem = stringField(state, "runtimeError", roomIssue);
        if (!problem.empty()) {
            textLine(problem, ImGui::GetContentRegionAvail().x);
            if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", problem.c_str());
            if (ImGui::SmallButton("关闭提示")) {
                roomIssue.clear();
                escapeAvailable = false;
                state.erase("runtimeError");
                send("dismiss-runtime-error");
            }
            if (escapeAvailable) {
                ImGui::SameLine();
                dim("按 Esc 返回标题");
            }
        } else if (escapeAvailable) dim("按 Esc 返回标题");
        ImGui::BeginChild(room.is_object() ? "RoomBody" : "LobbyBody", {0, -footer}, false, ImGuiWindowFlags_NoBackground);
        if (closingRun && !showProgress) dim("等待本局结束，可以继续在房间聊天。");
        if (showProgress) {
            const auto status = !runtimeProgress.empty() ? runtimeProgress : !launchProgress.empty() ? launchProgress :
                stringField(state, "status", "正在连接等待室…");
            ImGui::TextWrapped("%s", status.c_str());
            if (progressTotal) ImGui::ProgressBar(float(progressCompleted) / progressTotal, {-1, 0});
            if (!runtimeProgress.empty()) {
                if (ImGui::Button("取消并回房")) send("return-room");
            } else if ((joining || connecting || !problem.empty()) && ImGui::Button("返回大厅")) send("leave");
        } else if (room.is_object()) roomPanel(room);
        else lobby(compact);
        ImGui::EndChild();
        if (showComposer) composer();
        ImGui::Separator();
        constexpr auto close = "F2 收起";
        const float statusWidth = ImGui::GetContentRegionAvail().x - buttonWidth(close) - ImGui::GetStyle().ItemSpacing.x;
        textLine(stringField(state, "chatNotice", stringField(state, "status", "等待连接")), statusWidth, ImGuiCol_TextDisabled);
        ImGui::SameLine();
        if (ImGui::SmallButton(close)) hide();
        ImGui::End();
        finishInputFrame();
    }

    void setGameplay(bool active) {
        if (gameplay == active) return;
        gameplay = active;
        releaseInput();
    }
    void hide() {
        visible = false;
        releaseInput();
    }
    bool editing() const {
        const auto active = ImGui::GetActiveID();
        return visible && keyboardOwned && active && ImGui::GetInputTextState(active);
    }
    bool capturesKeyboard() const { return editing() || (visible && keyboardOwned && ImGui::GetIO().WantCaptureKeyboard); }
    SDL_Rect textArea() const { return inputArea; }

private:
    static bool modifierKey(SDL_Scancode scan) {
        return scan >= SDL_SCANCODE_LCTRL && scan <= SDL_SCANCODE_RGUI;
    }

    void finishInputFrame() {
        if (!editing()) composition.clear();
        if (keyboardOwned && !capturesKeyboard()) resetKeys();
    }

    void toggleChat() {
        chatOpen = !chatOpen;
        if (!chatOpen) releaseInput();
    }

    void releaseInput() {
        keyboardOwned = false;
        composition.clear();
        pointer.reset();
        auto& io = ImGui::GetIO();
        resetKeys();
        mouseDown.fill(false);
        mousePressed.fill(false);
        for (bool& down : io.MouseDown) down = false;
        ImGui::ClearActiveID();
        if (!ImGui::GetCurrentContext()->OpenPopupStack.empty()) ImGui::ClosePopupToLevel(0, false);
    }
    eagler::thprac::PracticePointer pointer;
    struct KeyChange { SDL_Scancode scancode; SDL_Keymod modifiers; bool down; };
    std::deque<std::variant<KeyChange, std::string>> keyboardEvents;
    ImVec2 mousePosition{-FLT_MAX, -FLT_MAX};
    std::array<bool, 3> mouseDown{}, mousePressed{};
    Rect area, trigger;
    SDL_Rect inputArea{};
    std::string composition;
    std::array<char, 9> roomCode{};
    std::array<char, 128> nickname{};
    std::array<char, 1024> draft{};
    unsigned lastReceipt = 0;
    ImGuiID draftInputId = 0;
    std::array<char, 128> filter{};
    std::array<char, 256> reportReason{};
    std::array<char, 512> musicQuery{};
    int musicPlatform = 0;
    Json reportId;
    int timing = 0;
    int chatRows = 0;
    Uint64 chatUpdatedAt = 0;
    std::vector<Json> quickPhrases;
    bool privateRoom = false;
    bool chatOpen = false;
    static constexpr int phrasesPerPage = 9;
    int phrasePage = 0;
    bool mutedChat = false;
    bool rollback = true;
    bool autoDelay = true;
    bool disableCharge = false;
    bool unrated = false;
    bool swarm = false;
    bool preferTurn = false;
    bool keyboardOwned = false;
    bool gameplay = false;
    bool settings = false;
    std::string roomIssue, runtimeProgress;
    unsigned progressCompleted = 0, progressTotal = 0;

    void resetKeys() {
        keyboardEvents.clear();
        auto& io = ImGui::GetIO();
        std::fill(std::begin(io.KeysDown), std::end(io.KeysDown), false);
        io.KeyShift = io.KeyCtrl = io.KeyAlt = io.KeySuper = false;
        io.ClearInputCharacters();
    }

    const Json& collection(const char* key) const {
        static const Json empty = Json::array();
        const auto found = state.find(key);
        return found != state.end() && found->is_array() ? *found : empty;
    }
    bool inGame() const { return gameplay; }
    void send(const Json& value) { if (dispatch) dispatch(value); }
    void send(const char* type) { send(Json{{"type", type}}); }
    bool hit(float x, float y) const {
        if (area.contains(x, y) || trigger.contains(x, y)) return true;
        const auto& gui = *ImGui::GetCurrentContext();
        for (const auto& popup : gui.OpenPopupStack) {
            if (popup.Window && popup.Window->LastFrameActive == gui.FrameCount &&
                popup.Window->OuterRectClipped.Contains(ImVec2(x, y))) return true;
        }
        return false;
    }
    Json room() const { return field(state, "room", Json(nullptr)); }
    int serial() const { return field(room(), "startSerial", 0); }

    void header(bool inRoom, bool compact) {
        if (cover && !compact) {
            ImGui::Image(cover, {36, 36});
            ImGui::SameLine(0, 8);
        }
        ImGui::BeginGroup();
        heading("東方花映塚", 1.22f);
        if (compact) ImGui::SameLine(0, 8);
        dim(inRoom ? "联机房间" : "联机大厅");
        ImGui::EndGroup();
        ImGui::Separator();
    }

    void lobby(bool compact) {
        if (!compact) heading("公开房间", 1.13f);
        if (field(state, "canOpenAccount", false) && !field(state, "accountRegistered", false)) {
            if (ImGui::SmallButton("登录")) send("open-account");
        }
        ImGui::SetNextItemWidth(-1);
        ImGui::InputTextWithHint("##filter", "搜索房间号或玩家", filter.data(), filter.size());
        setInputArea();
        if (ImGui::Button("创建房间")) ImGui::OpenPopup("创建房间");
        sameLineIfFits(buttonWidth("输入房间号"));
        if (ImGui::Button("输入房间号")) ImGui::OpenPopup("加入房间");
        sameLineIfFits(buttonWidth("刷新"));
        if (ImGui::Button("刷新")) send("directory");
        const auto& rooms = collection("rooms");
        int shown = 0;
        for (const auto& entry : rooms) {
            const auto code = stringField(entry, "code", stringField(entry, "roomCode"));
            const auto name = stringField(entry, "name", stringField(entry, "hostName", "花映塚"));
            if (!code.size() || (*filter.data() && code.find(filter.data()) == std::string::npos &&
                                name.find(filter.data()) == std::string::npos)) continue;
            const bool playing = stringField(entry, "phase") == "playing";
            const auto seated = field(entry, "seated", field(entry, "occupied", 0));
            const auto seats = field(entry, "playerCount", 2);
            ImGui::PushID(code.c_str());
            ImGui::BeginChild("Room", {0, ImGui::GetTextLineHeight() * 2 + 10}, true, ImGuiWindowFlags_NoScrollbar);
            const auto action = playing ? "旁观" : "加入";
            const float actionWidth = buttonWidth(action);
            const auto detail = (playing ? "对局中" : "等待玩家") + std::string("  |  ") +
                std::to_string(seated) + " / " + std::to_string(seats);
            cardText(code + "  |  " + name, detail, actionWidth);
            if (ImGui::Button(action, {actionWidth, 0}))
                send({{"type", "join"}, {"code", code}, {"spectator", playing}});
            ImGui::EndChild();
            ImGui::PopID();
            ++shown;
        }
        if (!shown) dim("没有符合条件的房间");
        if (ImGui::Button("网络诊断")) send("diagnostic");
        if (ImGui::BeginPopup("创建房间")) {
            ImGui::Checkbox("私人房间", &privateRoom);
            ImGui::Checkbox("休闲对战", &unrated);
            if (ImGui::Button("创建")) {
                send({{"type", "create"}, {"visibility", privateRoom ? "private" : "public"}, {"unrated", unrated}});
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
        if (ImGui::BeginPopup("加入房间")) {
            ImGui::SetNextItemWidth(160);
            if (ImGui::InputTextWithHint("##code", "4 至 8 位房间号", roomCode.data(), roomCode.size(),
                    ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_EnterReturnsTrue))
                join();
            setInputArea();
            if (ImGui::Button("加入")) join();
            ImGui::SameLine();
            if (ImGui::Button("旁观")) join(true);
            ImGui::EndPopup();
        }
    }

    void join(bool spectate = false) {
        send({{"type", "join"}, {"code", roomCode.data()}, {"spectator", spectate}});
        ImGui::CloseCurrentPopup();
    }

    void roomPanel(const Json& info) {
        const auto code = stringField(info, "code");
        const auto people = field(info, "seats", Json::array());
        const auto client = stringField(state, "clientId");
        const bool waiting = stringField(info, "phase") == "lobby";
        int own = -1;
        bool ready = waiting && people.size() >= 2;
        for (size_t i = 0; i < std::min<size_t>(2, people.size()); ++i) {
            if (stringField(people[i], "clientId") == client) own = static_cast<int>(i);
            ready = ready && field(people[i], "ready", false) && !field(people[i], "offline", false);
        }
        if (ImGui::Button("返回大厅")) send("leave");
        const auto codeLabel = "房间号  " + code;
        sameLineIfFits(buttonWidth(codeLabel.c_str()));
        if (ImGui::Button(codeLabel.c_str())) SDL_SetClipboardText(code.c_str());
        if (waiting && own == 0) {
            sameLineIfFits(buttonWidth("开始对战"));
            beginDisabled(!ready);
            if (ImGui::Button("开始对战")) send("start");
            endDisabled(!ready);
        }
        for (int i = 0; i < 2; ++i) {
            const auto player = i < static_cast<int>(people.size()) ? people[i] : Json(nullptr);
            ImGui::PushID(i);
            ImGui::BeginChild("Seat", {0, ImGui::GetTextLineHeight() * 2 + 10}, true, ImGuiWindowFlags_NoScrollbar);
            const bool seated = player.is_object();
            const bool prepared = field(player, "ready", false);
            const bool offline = field(player, "offline", false);
            std::string detail = "等待玩家入座";
            if (seated) {
                const auto resource = field(player, "resource", Json(nullptr));
                const auto preparation = stringField(resource, "status");
                std::string status = prepared ? "已准备" : "未准备";
                if (offline) status = "正在重连";
                else if (preparation == "failed") status = "资源准备失败";
                else if (preparation == "cancelled") status = "已取消下载";
                else if (preparation == "preparing" || preparation == "importing") {
                    status = preparation == "importing" ? "正在导入" :
                        stringField(resource, "stage") == "runtime" ? "准备运行包" : "准备游戏资源";
                    const auto percent = field(resource, "percent", Json(nullptr));
                    if (percent.is_number()) status += " " + std::to_string(
                        std::lround(std::clamp(percent.get<double>(), 0.0, 100.0))) + "%";
                }
                detail = status + "  |  " + (field(player, "nativeRuntime", false) ? "原生" : "网页");
            }
            const char* action = nullptr;
            if (waiting) {
                if (player.is_null()) action = "入座";
                else if (i == own) action = prepared ? "取消准备" : "准备";
                else if (own == 0 && seated) action = "管理";
            }
            const float actionWidth = action ? buttonWidth(action) : 0;
            const auto profile = field(player, "profile", Json(nullptr));
            const auto username = stringField(profile, "username");
            float leading = 0;
            if (!username.empty()) {
                const auto ratings = field(state, "ratings", Json::object());
                const auto rating = field(ratings, username.c_str(), Json(nullptr));
                const auto score = field(rating, "rating", Json(nullptr));
                const auto avatar = avatarFor ? avatarFor(stringField(profile, "avatar")) : nullptr;
                ImGui::SetCursorPos({8, (ImGui::GetWindowHeight() - 30) / 2});
                if (avatar ? ImGui::ImageButton(avatar, {26, 26}, {0, 0}, {1, 1}, 2) :
                             ImGui::Button("员##profile", {30, 30})) ImGui::OpenPopup("玩家资料");
                leading = 36;
                if (score.is_number_integer()) detail += "  |  " + score.dump();
                if (ImGui::BeginPopup("玩家资料")) {
                    text("@" + username);
                    if (ImGui::SmallButton("查看主页")) send({{"type", "open-profile"}, {"username", username}});
                    if (score.is_number_integer()) text("等级分 " + score.dump());
                    const auto match = field(player, "stats", Json::object());
                    const auto winsHere = field(match, "wins", -1);
                    if (winsHere >= 0) {
                        text("本房间 " + std::to_string(winsHere) + " 胜 " +
                             std::to_string(field(match, "losses", 0)) + " 负");
                    }
                    const auto summary = field(profile, "th09", Json::object());
                    const auto games = field(summary, "games", field(rating, "games", 0));
                    const auto wins = field(summary, "wins", -1);
                    if (games > 0) {
                        const auto played = std::to_string(games) + " 场";
                        text(wins >= 0 ? played + "  |  胜率 " + std::to_string(std::lround(100.0 * wins / games)) + "%" : played);
                    }
                    const auto cards = field(state, "ratingCards", Json::object());
                    const auto& loadouts = collection("loadouts");
                    for (const auto& pair : field(cards, username.c_str(), Json::array())) {
                        if (!pair.is_array() || pair.size() != 2 || !pair[0].is_number_integer() ||
                            !pair[1].is_number_integer()) continue;
                        const auto character = pair[0].get<int>();
                        if (character >= 0 && character < static_cast<int>(loadouts.size()))
                            text(stringField(loadouts[character], "label") + "  |  " + pair[1].dump());
                    }
                    ImGui::EndPopup();
                }
            }
            cardText("P" + std::to_string(i + 1) + "  |  " + stringField(player, "name", "空位"),
                     detail, actionWidth, leading);
            if (action && ImGui::Button(action, {actionWidth, 0})) {
                if (player.is_null()) {
                    send({{"type", "take-seat"}, {"seat", i}, {"name", *nickname.data() ? nickname.data() : "东方玩家"},
                          {"loadout", i}, {"movementMode", "joystick"}, {"touchEnabled", false}});
                } else if (i == own) send({{"type", "set-ready"}, {"ready", !prepared}});
                else ImGui::OpenPopup("席位管理");
            }
            if (ImGui::BeginPopup("席位管理")) {
                if (ImGui::MenuItem("移到旁观席")) send({{"type", "move-player-to-spectator"}, {"seat", i}, {"clientId", stringField(player, "clientId")}});
                if (ImGui::MenuItem("移出房间")) send({{"type", "remove-player"}, {"seat", i}, {"clientId", stringField(player, "clientId")}});
                ImGui::EndPopup();
            }
            ImGui::EndChild();
            ImGui::PopID();
        }
        const auto watchers = field(info, "spectators", Json::array());
        auto selfWatching = std::any_of(watchers.begin(), watchers.end(), [&](const Json& player) { return stringField(player, "clientId") == client; });
        if (own >= 0) {
            if (waiting && ImGui::Button("离开席位")) send("stand-up");
        } else if (ImGui::Button(selfWatching ? "取消旁观" : "旁观")) {
            if (selfWatching) send("leave-spectator");
            else send({{"type", "spectate"}, {"name", *nickname.data() ? nickname.data() : "东方玩家"}});
        }
        const auto watchLabel = "旁观 " + std::to_string(watchers.size());
        if (own < 0 || waiting) sameLineIfFits(buttonWidth(watchLabel.c_str()));
        if (ImGui::Button(watchLabel.c_str())) ImGui::OpenPopup("旁观名单");
        if (ImGui::BeginPopup("旁观名单")) {
            if (watchers.empty()) dim("暂无旁观玩家");
            for (const auto& player : watchers) {
                ImGui::PushID(stringField(player, "clientId").c_str());
                text(stringField(player, "name", "东方玩家"));
                if (own == 0) {
                    ImGui::SameLine();
                    if (ImGui::SmallButton("移出")) send({{"type", "remove-spectator"}, {"clientId", stringField(player, "clientId")}});
                }
                ImGui::PopID();
            }
            ImGui::EndPopup();
        }
        if (waiting && own > 0 && field(people[own], "ready", false) &&
            !people[0].is_null() && !field(people[0], "offline", false)) {
            sameLineIfFits(buttonWidth("提醒房主"));
            if (ImGui::Button("提醒房主")) send("room-nudge");
        }
        const auto settingsLabel = settings ? "收起更多" : "更多";
        sameLineIfFits(buttonWidth(settingsLabel));
        if (ImGui::Button(settingsLabel)) {
            settings = !settings;
            if (settings) {
                privateRoom = stringField(info, "visibility") == "private";
                disableCharge = field(info, "disableLocalChargePriority", false);
                unrated = field(info, "unrated", false);
            }
        }
        if (settings) roomSettings(info, own == 0 && waiting);
        ImGui::Separator();
        const float minChatHeight = ImGui::GetTextLineHeightWithSpacing() * 3 + 8;
        const float available = ImGui::GetContentRegionAvail().y - ImGui::GetFrameHeightWithSpacing();
        chatHistory(std::max(minChatHeight, available));
        const auto& list = phrases();
        if (ImGui::BeginCombo("##room-phrases", "快捷发言")) {
            for (const auto& entry : list)
                if (ImGui::Selectable(label(stringField(entry, "zh"), stringField(entry, "id").c_str()).c_str())) sendPhrase(entry);
            ImGui::EndCombo();
        }
    }

    void roomSettings(const Json& info, bool host) {
        if (host && ImGui::CollapsingHeader("对战规则")) {
            ImGui::Checkbox("私人房间", &privateRoom);
            ImGui::Checkbox("关闭本地优先蓄力", &disableCharge);
            ImGui::Checkbox("休闲对战", &unrated);
            if (ImGui::Button("应用规则"))
                send({{"type", "settings"}, {"visibility", privateRoom ? "private" : "public"},
                      {"disableLocalChargePriority", disableCharge}, {"unrated", unrated}});
            if (ImGui::Checkbox("回滚", &rollback) || (!rollback && ImGui::Checkbox("自动输入延迟", &autoDelay)))
                timingSettings();
            if (!rollback && !autoDelay &&
                ImGui::SliderInt("输入延迟 | 帧", &timing, 0, field(state, "manualDelayLimit", 9))) timingSettings();
        }
        if (ImGui::CollapsingHeader("个人")) {
            ImGui::SetNextItemWidth(-1);
            if (ImGui::InputTextWithHint("##name", "输入昵称后回车", nickname.data(), nickname.size(), ImGuiInputTextFlags_EnterReturnsTrue))
                send({{"type", "set-name"}, {"name", nickname.data()}});
            setInputArea();
        }
        if (ImGui::CollapsingHeader("网络")) {
            swarm = field(state, "preferSwarm", false);
            if (ImGui::Checkbox("优先使用 TURN", &preferTurn)) {
                if (preferTurn) swarm = false;
                send({{"type", "prefer-turn"}, {"enabled", preferTurn}});
            }
            if (field(info, "swarmAvailable", false) && ImGui::Checkbox("使用 Swarm TURN", &swarm)) {
                if (swarm) preferTurn = false;
                send({{"type", "relay-preference"}, {"swarm", swarm}});
            }
            const auto workers = field(info, "swarmWorkers", Json::array());
            const auto pings = field(state, "swarmPings", Json::array());
            struct Worker { int id; std::string name; double latency; };
            std::vector<Worker> nodes;
            Json addresses = Json::array();
            for (const auto& worker : workers) {
                const auto id = field(worker, "id", -1);
                const auto host = stringField(worker, "address", stringField(worker, "ip"));
                const auto endpoints = field(worker, "addresses", Json::array());
                const auto address = !host.empty() ? host : endpoints.empty() || !endpoints[0].is_string() ? "" : endpoints[0].get<std::string>();
                if (address.empty() || id < 0) continue;
                double latency = std::numeric_limits<double>::infinity();
                if (pings.is_array() && addresses.size() < pings.size() && pings[addresses.size()].is_number())
                    latency = pings[addresses.size()].get<double>();
                nodes.push_back({id, stringField(worker, "name", address), latency});
                addresses.push_back(address);
            }
            if (swarm && !nodes.empty()) {
                if (ImGui::Button("测量节点延迟")) send({{"type", "desktop"}, {"command", "swarm_ping"}, {"args", {{"addresses", addresses}}}});
                std::stable_sort(nodes.begin(), nodes.end(), [](const Worker& a, const Worker& b) { return a.latency < b.latency; });
                for (const auto& worker : nodes) {
                    auto suffix = std::isfinite(worker.latency) ? std::to_string(static_cast<int>(worker.latency)) + " ms" : "未测量";
                    if (ImGui::Selectable((worker.name + "  |  " + suffix + "##" + std::to_string(worker.id)).c_str()))
                        send({{"type", "relay-preference"}, {"swarm", true}, {"worker", worker.id}});
                }
            }
        }
        const auto results = field(state, "probes", Json::object());
        if (!results.empty() && ImGui::CollapsingHeader("丢包测试")) {
            bool ready = false, running = false;
            for (const char* lane : {"direct", "turn"}) {
                const auto result = field(results, lane, Json::object());
                const auto metric = field(result, "metric", Json::object());
                const auto loss = field(result, "loss", Json::object());
                const auto rtt = field(metric, "rtt", Json(nullptr));
                const auto stage = stringField(loss, "state");
                ready |= field(result, "ready", false);
                running |= stage == "running";
                const char* laneName = lane[0] == 'd' ? "直连" : "TURN";
                const auto latency = rtt.is_number() ? std::to_string(static_cast<int>(std::lround(rtt.get<double>()))) + " ms" :
                    stringField(metric, "state") == "checking" ? "正在测量" : "暂不可用";
                text(std::string(laneName) + "  |  " + latency);
                if (stage == "complete") {
                    const auto outgoing = field(loss, "outgoing", Json::object());
                    const auto incoming = field(loss, "incoming", Json::object());
                    ImGui::Text("发出 %.1f%%  |  收到 %.1f%%", field(outgoing, "percent", 0.0), field(incoming, "percent", 0.0));
                } else if (stage == "running") {
                    ImGui::Text("已发送 %d / 300", field(loss, "sent", 0));
                } else if (stage == "interrupted") {
                    dim("测试中断");
                }
            }
            beginDisabled(!ready || running);
            if (ImGui::Button("测试丢包率")) send("test-loss");
            endDisabled(!ready || running);
        }
        if (field(info, "listenTogether", false) && ImGui::CollapsingHeader("一起听")) roomMusic();
    }

    void roomMusic() {
        const auto music = field(state, "music", Json::object());
        if (music.empty()) return;
        const auto current = field(music, "current", Json::object());
        const auto offer = field(music, "offer", Json::object());
        const auto session = field(music, "session", Json::object());
        const auto status = stringField(music, "status");
        const bool joined = field(music, "joined", false);
        const bool seated = field(music, "seated", false);
        const bool readOnly = joined && !seated;
        const auto duration = field(music, "duration", 0.f);
        if (!current.empty()) text(stringField(current, "name", stringField(current, "id")));
        if (status == "loading") dim("正在加载歌曲");
        if (!stringField(music, "error").empty()) dim("歌曲暂时无法播放");
        if (joined) {
            if (ImGui::Button("退出一起听")) send("music-leave");
        } else if (!offer.empty()) {
            dim("收到一起听邀请");
            if (ImGui::Button("加入一起听")) send("music-sync");
        } else if (seated && duration > 0 && status != "loading" && stringField(music, "error").empty()) {
            if (ImGui::Button("发起一起听")) send("music-sync");
        }
        if (!session.empty() && !readOnly && status != "loading") {
            if (ImGui::Button(status == "playing" ? "暂停" : "播放")) send("music-play");
        }
        if (readOnly || !ImGui::TreeNode("搜歌")) return;
        ImGui::Combo("平台", &musicPlatform, "网易云音乐\0QQ 音乐\0");
        ImGui::SetNextItemWidth(-1);
        const bool search = ImGui::InputTextWithHint("##music-query", "歌名或歌手",
            musicQuery.data(), musicQuery.size(), ImGuiInputTextFlags_EnterReturnsTrue);
        setInputArea();
        if ((ImGui::Button("查询") || search) && *musicQuery.data())
            send({{"type", "music-search"}, {"platform", musicPlatform ? "qq" : "netease"},
                  {"query", musicQuery.data()}});
        const auto message = stringField(state, "musicSearch");
        if (!message.empty()) dim(message);
        const auto& tracks = collection("musicResults");
        for (size_t i = 0; i < tracks.size(); ++i) {
            ImGui::PushID(static_cast<int>(i));
            if (ImGui::SmallButton("播放")) send({{"type", "music-track"}, {"index", i}});
            ImGui::SameLine();
            text(stringField(tracks[i], "name", stringField(tracks[i], "id")));
            ImGui::PopID();
        }
        ImGui::TreePop();
    }

    void timingSettings() {
        send({{"type", "timing-settings"}, {"rollbackEnabled", rollback},
              {"timingChoice", autoDelay ? Json("auto") : Json(timing)}});
    }

    void setInputArea() {
        if (!ImGui::IsItemActive()) return;
        const auto lo = ImGui::GetItemRectMin();
        const auto hi = ImGui::GetItemRectMax();
        inputArea = {static_cast<int>(lo.x), static_cast<int>(lo.y), static_cast<int>(hi.x - lo.x), static_cast<int>(hi.y - lo.y)};
        if (composition.empty()) return;
        const auto caret = ImGui::GetCurrentContext()->PlatformImePos;
        const ImVec2 start{std::clamp(caret.x, lo.x, hi.x), std::clamp(caret.y, lo.y, hi.y)};
        const auto size = ImGui::CalcTextSize(composition.c_str());
        const ImVec2 end{start.x + size.x, start.y + size.y};
        auto* draw = ImGui::GetWindowDrawList();
        draw->PushClipRect(lo, hi, true);
        draw->AddRectFilled(start, end, ImGui::GetColorU32(ImGuiCol_FrameBgActive));
        draw->AddText(start, ImGui::GetColorU32(ImGuiCol_Text), composition.c_str());
        draw->AddLine({start.x, end.y}, end, ImGui::GetColorU32(ImGuiCol_Text));
        draw->PopClipRect();
    }

    const std::vector<Json>& phrases() const { return quickPhrases; }
    int phrasePageOffset() const { return std::max(0, phrasePage - 1) * phrasesPerPage; }

    void sendPhrase(const Json& phrase) {
        send({{"type", "quick-chat"}, {"phrase", stringField(phrase, "id")}, {"serial", serial()}});
    }

    void composer() {
        if (!field(state, "freeChat", true)) {
            if (!inGame() && field(state, "canOpenAccount", false) && !field(state, "accountRegistered", false)) {
                if (ImGui::Button("登录后发言")) send("open-account");
            } else dim("此房间支持快捷发言");
            return;
        }
        const bool sending = field(state, "chatSending", false);
        const auto sendLabel = sending ? "发送中" : "发送";
        const auto sendWidth = buttonWidth(sendLabel);
        ImGui::SetNextItemWidth(-sendWidth - ImGui::GetStyle().ItemSpacing.x);
        draftInputId = ImGui::GetID("##message");
        const bool submit = ImGui::InputTextWithHint("##message", "输入消息", draft.data(), draft.size(), ImGuiInputTextFlags_EnterReturnsTrue);
        setInputArea();
        ImGui::SameLine();
        const bool disabled = sending || !*draft.data();
        beginDisabled(disabled);
        if ((ImGui::Button(sendLabel, {sendWidth, 0}) || submit) && *draft.data() && !sending) {
            send({{"type", "free-chat"}, {"text", draft.data()}, {"serial", serial()}});
        }
        endDisabled(disabled);
    }

    static std::string chatText(const Json& message, const std::vector<Json>& list) {
        const auto settlement = field(message, "rating", Json(nullptr));
        if (settlement.is_array()) {
            std::string result = "等级分结算";
            for (const auto& change : settlement) {
                const auto user = stringField(change, "username");
                const auto before = field(change, "before", Json(nullptr));
                const auto after = field(change, "after", Json(nullptr));
                if (!user.empty() && after.is_number_integer()) {
                    result += "  |  @" + user + " " + (before.is_number_integer() ? before.dump() + " → " : "") + after.dump();
                }
            }
            return result;
        }
        const auto id = stringField(message, "phrase");
        auto body = stringField(message, "text", id);
        if (!id.empty()) {
            for (const auto& phrase : list) {
                if (stringField(phrase, "id") == id) {
                    body = stringField(phrase, "zh");
                    break;
                }
            }
        }
        return stringField(message, "name", stringField(message, "senderName", "玩家")) + "  " + body;
    }

    void chatHistory(float height) {
        const auto& chat = collection("chat");
        ImGui::BeginChild("Messages", {0, height}, true);
        if (chat.empty()) dim("暂无消息");
        const auto& list = phrases();
        for (const auto& message : chat) {
            if (mutedChat) break;
            ImGui::PushTextWrapPos();
            text(chatText(message, list));
            ImGui::PopTextWrapPos();
            const auto messageId = field(message, "messageId", 0ULL);
            const auto author = stringField(message, "authorId");
            if (messageId && !author.empty() && author != stringField(state, "chatAuthor") &&
                stringField(message, "clientId") != stringField(state, "clientId") &&
                ImGui::BeginPopupContextItem(std::to_string(messageId).c_str())) {
                if (ImGui::MenuItem("举报")) reportId = messageId;
                ImGui::EndPopup();
            }
        }
        if (chatRows != static_cast<int>(chat.size())) ImGui::SetScrollHereY(1);
        chatRows = static_cast<int>(chat.size());
        ImGui::EndChild();
        if (!reportId.is_null() && ImGui::CollapsingHeader("举报消息", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::InputTextWithHint("##reason", "举报原因", reportReason.data(), reportReason.size());
            setInputArea();
            if (ImGui::Button("提交举报")) {
                send({{"type", "chat-report"}, {"messageId", reportId}, {"reason", reportReason.data()}});
                reportId = nullptr;
                reportReason[0] = '\0';
            }
            ImGui::SameLine();
            if (ImGui::Button("取消")) reportId = nullptr;
        }
    }

    void playChat(float width, float height) {
        constexpr float margin = 6;
        const float rowHeight = ImGui::GetFrameHeight();
        const float gap = ImGui::GetStyle().ItemSpacing.x;
        const float chatWidth = buttonWidth("聊天 Alt");
        const float closeWidth = buttonWidth("F2");
        trigger = {margin, height - rowHeight - margin, chatWidth + gap + closeWidth, rowHeight};
        place(trigger);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0, 0});
        ImGui::Begin("打开聊天###chat-trigger", nullptr, flags | ImGuiWindowFlags_NoBackground);
        if (ImGui::Button("聊天 Alt", {chatWidth, rowHeight})) toggleChat();
        ImGui::SameLine();
        if (ImGui::Button("F2", {closeWidth, rowHeight})) hide();
        ImGui::End();
        ImGui::PopStyleVar();
        const auto chat = state.find("chat");
        const auto notice = stringField(state, "chatNotice");
        const bool recentChat = !mutedChat && chat != state.end() && chat->is_array() && !chat->empty() &&
            !field(chat->back(), "expiredInGame", false) && SDL_GetTicks() - chatUpdatedAt < 4500;
        const float noticeX = trigger.x + trigger.w + gap;
        if ((!notice.empty() || recentChat) && width - noticeX >= 64) {
            place({noticeX, trigger.y, std::min(width - noticeX - margin, 370.f), rowHeight});
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {6, 4});
            ImGui::Begin("收到发言###chat-message", nullptr, flags | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoScrollbar);
            textLine(notice.empty() ? chatText(chat->back(), phrases()) : notice, ImGui::GetContentRegionAvail().x);
            ImGui::End();
            ImGui::PopStyleVar();
        }
        if (!chatOpen) return;
        const float pickerHeight = std::min(134.f, height * .60f);
        area = {margin, trigger.y - margin - pickerHeight, std::min(width - 2 * margin, 350.f), pickerHeight};
        place(area);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {6, 5});
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, {5, 2});
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {4, 3});
        ImGui::Begin("游戏内聊天###chat", nullptr, flags | ImGuiWindowFlags_NoScrollbar);
        if (ImGui::Button("常用")) phrasePage = 0;
        ImGui::SameLine();
        if (ImGui::Button("全部") && !phrasePage) phrasePage = 1;
        const auto muteLabel = mutedChat ? "取消静音" : "静音";
        sameLineIfFits(buttonWidth(muteLabel));
        if (ImGui::Button(muteLabel)) mutedChat = !mutedChat;
        sameLineIfFits(buttonWidth("回房间"));
        if (ImGui::Button("回房间")) send("return-room");
        const auto& list = phrases();
        if (phrasePage) {
            const int pages = std::max(1, (static_cast<int>(list.size()) + phrasesPerPage - 1) / phrasesPerPage);
            const float pagerWidth = buttonWidth("<") + buttonWidth(">") + ImGui::CalcTextSize("00 / 00").x +
                2 * ImGui::GetStyle().ItemSpacing.x;
            sameLineIfFits(pagerWidth);
            const bool firstPage = phrasePage <= 1;
            beginDisabled(firstPage);
            if (ImGui::SmallButton("<##prev-phrases")) --phrasePage;
            endDisabled(firstPage);
            ImGui::SameLine();
            ImGui::Text("%d / %d", phrasePage, pages);
            ImGui::SameLine();
            const bool lastPage = phrasePage >= pages;
            beginDisabled(lastPage);
            if (ImGui::SmallButton(">##next-phrases")) ++phrasePage;
            endDisabled(lastPage);
        }
        const auto phraseHeight = std::max(ImGui::GetFrameHeight(),
            ImGui::GetContentRegionAvail().y - ImGui::GetFrameHeightWithSpacing());
        ImGui::BeginChild("Phrases", {0, phraseHeight}, false);
        const int start = phrasePageOffset();
        const int count = std::min(phrasesPerPage, static_cast<int>(list.size()) - start);
        const auto unit = (ImGui::GetContentRegionAvail().x - 2 * ImGui::GetStyle().ItemSpacing.x) / 3;
        for (int i = 0; i < count; ++i) {
            if (i % 3) ImGui::SameLine();
            const auto& phrase = list[start + i];
            const auto name = stringField(phrase, "zh");
            auto shortName = name.substr(0, name.find("（"));
            ImGui::PushID(start + i);
            const auto caption = std::to_string(i + 1) + "  " + shortName;
            if (ImGui::Button("##phrase", {unit, 0})) sendPhrase(phrase);
            const auto min = ImGui::GetItemRectMin();
            const auto max = ImGui::GetItemRectMax();
            const auto textSize = ImGui::CalcTextSize(caption.c_str());
            const float inset = ImGui::GetStyle().FramePadding.x;
            const float x = min.x + std::max(inset, (max.x - min.x - textSize.x) / 2);
            const float right = max.x - inset;
            ImGui::RenderTextEllipsis(ImGui::GetWindowDrawList(), {x, min.y + ImGui::GetStyle().FramePadding.y},
                                      {right, max.y}, right, right, caption.c_str(), nullptr, &textSize);
            if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", name.c_str());
            ImGui::PopID();
        }
        ImGui::EndChild();
        composer();
        ImGui::End();
        ImGui::PopStyleVar(3);
    }
};

inline void setupKeys(ImGuiIO& io) {
    const std::pair<int, int> keys[] = {
        {ImGuiKey_Tab, SDL_SCANCODE_TAB}, {ImGuiKey_LeftArrow, SDL_SCANCODE_LEFT},
        {ImGuiKey_RightArrow, SDL_SCANCODE_RIGHT}, {ImGuiKey_UpArrow, SDL_SCANCODE_UP},
        {ImGuiKey_DownArrow, SDL_SCANCODE_DOWN}, {ImGuiKey_PageUp, SDL_SCANCODE_PAGEUP},
        {ImGuiKey_PageDown, SDL_SCANCODE_PAGEDOWN}, {ImGuiKey_Home, SDL_SCANCODE_HOME},
        {ImGuiKey_End, SDL_SCANCODE_END}, {ImGuiKey_Insert, SDL_SCANCODE_INSERT},
        {ImGuiKey_Delete, SDL_SCANCODE_DELETE}, {ImGuiKey_Backspace, SDL_SCANCODE_BACKSPACE},
        {ImGuiKey_Space, SDL_SCANCODE_SPACE}, {ImGuiKey_Enter, SDL_SCANCODE_RETURN},
        {ImGuiKey_KeyPadEnter, SDL_SCANCODE_KP_ENTER}, {ImGuiKey_Escape, SDL_SCANCODE_ESCAPE},
        {ImGuiKey_A, SDL_SCANCODE_A}, {ImGuiKey_C, SDL_SCANCODE_C}, {ImGuiKey_V, SDL_SCANCODE_V},
        {ImGuiKey_X, SDL_SCANCODE_X}, {ImGuiKey_Y, SDL_SCANCODE_Y}, {ImGuiKey_Z, SDL_SCANCODE_Z},
    };
    for (auto [key, scancode] : keys) io.KeyMap[key] = scancode;
}

}
