#include "ImGuiOverlay.hpp"
#include "RmlGles.hpp"
#include "RmlRoomDocument.hpp"
#include "RmlQuickChat.hpp"
#include "RmlUi_Platform_SDL.h"

#include <eagler/netplay/NativeLobby.hpp>
#include <RmlUi/Core.h>
#include <RmlUi/Core/Elements/ElementFormControlInput.h>
#include <RmlUi/Core/StringUtilities.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <cstdio>
#include <filesystem>
#include <memory>
#include <random>
#include <string_view>
#include <utility>
#if TH09_RMLUI_TEST
#include <fstream>
#include <vector>
#endif

namespace Th09Overlay {
namespace {
using th09::desktop::RmlGles;
using Netplay::NativeLobby;
using Json = nlohmann::json;

SDL_Window* owner = nullptr;
std::unique_ptr<RmlGles> renderer;
std::unique_ptr<SystemInterface_SDL> systemInterface;
std::unique_ptr<NativeLobby> lobby;
TextInputMethodEditor_SDL editor;
Rml::Context* context = nullptr;
Rml::ElementDocument* document = nullptr;
Rml::Vector2i layoutSize;
Json snapshot;
RoomPhase phase = RoomPhase::idle;
std::string issue, build, generation, connectionStatus, gameChat;
std::string routeSwitchText;
std::string directStart, directPort;
bool directPage = false;
constexpr std::array<const char*, 4> startupRoutes{"auto", "direct", "swarm", "turn"};
constexpr std::array<const char*, 4> startupRouteLabels{"自动", "直连 / NAT4", "Swarm", "TURN"};
size_t startupRoute = 0;
std::int64_t refreshChatAt = 0;
std::pair<std::string, unsigned> lastRoomRun;
unsigned completed = 0, total = 0;
constexpr unsigned phrasesPerPage = 7;
unsigned phrasePage = 0;
bool visible = false, phrases = false, recovering = false, escapeAvailable = false;
bool escapeRequest = false, returnRequest = false, cursorWasVisible = false, running = false;
bool cursorOwned = false, routeSwitchAvailable = false, routeSwitchPressed = false, routeSwitchRequest = false;
bool changed = true, rendered = false;
int viewX = 0, viewY = 0;
float scale = 1;

std::string environment(const char* name) {
    if (const auto* value = SDL_getenv(name)) return value;
    return {};
}

template<class T>
T field(const Json& object, const char* key, T fallback) {
    if (!object.is_object()) return fallback;
    const auto value = object.find(key);
    if (value == object.end()) return fallback;
    try { return value->get<T>(); }
    catch (const Json::exception&) { return fallback; }
}

std::string query(std::string_view url, std::string_view name) {
    if (auto offset = url.find('?'); offset != std::string::npos) {
        url.remove_prefix(offset + 1);
        while (!url.empty()) {
            const auto separator = url.find('&');
            const auto pair = url.substr(0, separator);
            if (pair.substr(0, pair.find('=')) == name && pair.size() > name.size())
                return std::string(pair.substr(name.size() + 1));
            if (separator == std::string::npos) break;
            url.remove_prefix(separator + 1);
        }
    }
    return {};
}

std::string makeId() {
    static constexpr char hex[] = "0123456789abcdef";
    std::random_device random;
    std::string value(32, '0');
    for (char& c : value) c = hex[random() & 15];
    return value;
}

bool validId(std::string_view value, std::string_view chars, size_t minimum, size_t maximum) {
    return value.size() >= minimum && value.size() <= maximum && value.find_first_not_of(chars) == std::string::npos;
}

bool gameOwnsInput() {
    return phase == RoomPhase::selection || phase == RoomPhase::playing ||
        phase == RoomPhase::spectating || phase == RoomPhase::results;
}

void updateCursor() {
    const bool needed = visible || routeSwitchAvailable;
    if (needed == cursorOwned) return;
    cursorOwned = needed;
    if (needed) {
        cursorWasVisible = SDL_CursorVisible();
        SDL_ShowCursor();
    } else if (!cursorWasVisible) SDL_HideCursor();
}

void hide() {
    if (!visible) return;
    if (context && context->GetFocusElement()) context->GetFocusElement()->Blur();
    visible = false;
    phrases = false;
    changed = true;
    updateCursor();
    SDL_StopTextInput(owner);
}

Rml::Element* element(const char* id) { return document->GetElementById(id); }

void display(const char* id, bool show) {
    if (show) element(id)->RemoveProperty("display");
    else element(id)->SetProperty("display", "none");
}

std::string button(std::string_view action, const std::string& value, const std::string& label) {
    return "<button data-action='" + std::string(action) + "' data-value='" +
        Rml::StringUtilities::EncodeRml(value) + "'>" + Rml::StringUtilities::EncodeRml(label) + "</button>";
}

Json room() { return field(snapshot, "room", Json{}); }

const Json& seats(const Json& current) {
    static const Json empty = Json::array();
    if (!current.is_object()) return empty;
    const auto found = current.find("seats");
    return found != current.end() && found->is_array() ? *found : empty;
}

int seatOf(const Json& current) {
    if (!lobby) return -1;
    const auto& roster = seats(current);
    for (size_t i = 0; i < roster.size(); ++i) {
        if (field(roster[i], "clientId", std::string{}) == lobby->Settings().client) return static_cast<int>(i);
    }
    return -1;
}

void send(Json message) {
    if (lobby) lobby->Send(std::move(message));
}

std::string phraseText(const std::string& id) {
    const auto& items = th09::desktop::quickPhrases;
    const auto match = std::find_if(items.begin(),items.end(),[&](const auto& entry) { return entry.id == id; });
    return match == items.end() ? id : std::string(match->text);
}

void sendPhrase(const std::string& id) {
    send({{"type","quick-chat"},{"phrase",id},{"serial",field(room(),"startSerial",0u)}});
}

std::string chatText(const Json& messages, size_t maximum) {
    using Rml::StringUtilities::EncodeRml;
    if (!messages.is_array()) return {};
    const auto begin = messages.size() > maximum ? messages.size() - maximum : 0;
    std::string rows;
    for (size_t index = begin; index < messages.size(); ++index) {
        const auto& entry = messages[index];
        const auto message = field(entry,"text",phraseText(field(entry,"phrase",std::string{})));
        if (!message.empty()) rows += "<div>" + EncodeRml(field(entry,"name",std::string{})) + " " + EncodeRml(message) + "</div>";
    }
    return rows;
}

void updateGameChat() {
    refreshChatAt = 0;
    const auto messages = field(snapshot,"chat",Json::array());
    const auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    Json recent = Json::array();
    if (!directPage && gameOwnsInput() && messages.is_array()) for (auto item = messages.rbegin(); item != messages.rend() && recent.size() < 4; ++item) {
        const auto until = field(*item,"receivedAt",std::int64_t(0)) + 10000;
        if (until <= now) continue;
        recent.insert(recent.begin(),*item);
        if (!refreshChatAt || refreshChatAt > until) refreshChatAt = until;
    }
    const auto text = chatText(recent,4);
    if (gameChat != text) {
        gameChat = text;
        changed = true;
    }
}

void updateDocument() {
    using Rml::StringUtilities::EncodeRml;
    const auto current = room();
    const bool inRoom = !directPage && current.is_object();
    const bool connecting = phase == RoomPhase::connecting || phase == RoomPhase::loadingSpectator ||
        phase == RoomPhase::synchronizing;
    display("panel", visible && !gameOwnsInput());
    display("chat-hud",gameOwnsInput() && (visible || !gameChat.empty()));
    display("chat-tools",visible);
    display("game-messages",!gameChat.empty());
    element("game-messages")->SetInnerRML(gameChat);
    display("phrases", phrases && !directPage);
    display("open-phrases", !directPage);
    element("return-room")->SetInnerRML(directPage ? "结束直连" : "回房");
    display("directory", !directPage && !inRoom && !connecting);
    display("direct", directPage && !connecting);
    display("room", inRoom && !connecting);
    display("startup-routes", inRoom && !connecting && seatOf(current) >= 0 &&
        field(current, "phase", std::string{}) == "lobby");
    display("progress", connecting);
    const bool canChat = field(snapshot, "registered", false);
    display("account", !directPage && lobby && !lobby->Settings().accountEndpoint.empty() && !canChat);
    display("composer", canChat);
    display("notice", escapeAvailable || recovering);
    display("connection-status",!connectionStatus.empty());
    element("connection-status")->SetInnerRML(connectionStatus);
    display("route-switch", !routeSwitchText.empty());
    element("route-switch-button")->SetProperty("display", routeSwitchAvailable ? "inline-block" : "none");
    element("route-switch-text")->SetInnerRML(Rml::StringUtilities::EncodeRml(routeSwitchText));
    if (inRoom && !connecting && seatOf(current) >= 0 && field(current, "phase", std::string{}) == "lobby") {
        std::string routes = "<div class='sub'>开局线路</div>";
        for (size_t index = 0; index < startupRoutes.size(); ++index)
            routes += "<div class='route-row'><button class='" + std::string(index == startupRoute ? "primary" : "") +
                "' data-action='startup-route' data-value='" + std::to_string(index) + "'>" + startupRouteLabels[index] + "</button></div>";
        element("startup-routes")->SetInnerRML(routes);
    }
    element("notice")->SetInnerRML(escapeAvailable ? "按 Esc 返回标题" : "正在重新连接对手…");

    if (connecting) {
        const std::string detail = recovering ? "正在重新连接对手…" :
            phase == RoomPhase::connecting && directPage ? (directPort.empty() ? "正在连接对手的 UDP 端口…" :
                "正在 UDP " + directPort + " 端口等待对手…") :
            phase == RoomPhase::connecting ? "正在连接其他玩家…" :
            phase == RoomPhase::loadingSpectator ? "正在等待旁观数据…" :
            "正在同步对局…";
        element("progress-text")->SetInnerRML(EncodeRml(detail));
        display("progress-meter", !directPage || phase == RoomPhase::synchronizing);
        element("cancel-room")->SetInnerRML(directPage ? "取消直连" : "取消并回房");
        element("progress-meter")->SetAttribute("value", total ? double(completed) / total : 0);
    }

    if (!visible) return;
    const auto messages = field(snapshot,"chat",Json::array());
    if (gameOwnsInput()) {
        std::string cells;
        if (phrases && !directPage) {
            const auto& entries = th09::desktop::quickPhrases;
            const auto begin = phrasePage * phrasesPerPage;
            for (size_t index = begin; index < std::min<size_t>(begin + phrasesPerPage,entries.size()); ++index)
                cells += button("phrase",std::string(entries[index].id),std::string(entries[index].text));
            cells += button("phrase-page","","换一组");
        }
        element("phrases")->SetInnerRML(cells);
        return;
    }
    std::string rows;
    if (!directPage && !inRoom && !connecting) {
        auto rooms = field(snapshot, "rooms", Json::array());
        if (rooms.is_array()) for (const auto& entry : rooms) {
            const auto code = field(entry, "code", std::string{});
            if (!validId(code, "0123456789", 4, 8)) continue;
            const bool playing = field(entry, "phase", std::string{}) != "lobby";
            rows += "<div class='room-row'>" + button(playing ? "watch" : "join", code, playing ? "旁观" : "加入") +
                "<span class='name'>" + code + "</span> | " + (playing ? "对局中" : "等待玩家") + "</div>";
        }
        element("rooms")->SetInnerRML(rows.empty() ? "<span class='sub'>暂无公开房间</span>" : rows);
    }
    if (inRoom && !connecting) {
        const int side = seatOf(current);
        element("room-code")->SetInnerRML(EncodeRml(field(snapshot,"code",std::string{})));
        const auto& roster = seats(current);
        const auto assets = field(snapshot,"profileAssets",Json{});
        const auto ratings = field(assets,"ratings",Json{});
        const auto avatars = field(assets,"avatars",Json{});
        renderer->SetAvatars(avatars);
        for (size_t i = 0; i < roster.size(); ++i) {
            const auto& entry = roster[i];
            const auto name = field(entry,"name",std::string{});
            rows += "<div class='seat'>";
            if (entry.is_null() && field(current,"phase",std::string{}) == "lobby")
                rows += button("seat", std::to_string(i), "入座");
            const auto profile = field(entry,"profile",Json{});
            const auto avatar = field(profile,"avatar",std::string{});
            if (validId(avatar,"0123456789abcdef",1,64) && avatars.is_object() && avatars.contains(avatar))
                rows += "<img class='avatar' src='avatar-" + avatar + "'/>";
            rows += std::to_string(i + 1) + "P | " + EncodeRml(name.empty() ? (entry.is_null() ? "空位" : "玩家") : name);
            if (field(entry,"nativeRuntime",false)) rows += " | 原生";
            const auto username = field(profile,"username",std::string{});
            const auto rating = field(ratings,username.c_str(),field(profile,"rating",0));
            if (rating > 0) rows += " | " + std::to_string(rating);
            if (field(entry,"ready",false)) rows += " | 已准备";
            rows += "</div>";
        }
        element("seats")->SetInnerRML(rows);
        rows.clear();
        if (side < 0) rows = button("spectate", "", "旁观");
        else if (field(current,"phase",std::string{}) == "lobby") {
            const bool ready = field(roster[side],"ready",false);
            rows = button("ready", ready ? "0" : "1", ready ? "取消准备" : "准备");
            if (side == 0) rows += button("start", "", "开始");
            if (field(current,"swarmAvailable",false)) {
                const bool swarm = field(roster[side],"preferSwarm",false);
                rows += button("swarm", swarm ? "0" : "1", swarm ? "Swarm 已选" : "选择 Swarm");
            }
        }
        element("room-actions")->SetInnerRML(rows);
        element("messages")->SetInnerRML(chatText(messages,24));
    }
    const auto status = directPage ? "" : field(snapshot, inRoom ? "status" : "directoryStatus", std::string{});
    element("status")->SetInnerRML(EncodeRml(issue.empty() ? status : issue));
}

class Actions final : public Rml::EventListener {
    void ProcessEvent(Rml::Event& event) override {
        for (auto* item = event.GetTargetElement(); item; item = item->GetParentNode()) {
            const auto action = item->GetAttribute<Rml::String>("data-action", "");
            if (action.empty()) continue;
            const auto argument = item->GetAttribute<Rml::String>("data-value", "");
            if (action == "hide") hide();
            else if (action == "open-direct" || action == "close-direct") {
                directPage = action == "open-direct";
                issue.clear();
                if (!directPage) ShowLobby();
            } else if (action == "direct-listen" || action == "direct-join") {
                const bool host = action == "direct-listen";
                const auto endpoint = static_cast<Rml::ElementFormControlInput*>(
                    element(host ? "direct-port" : "direct-address"))->GetValue();
                directStart = Json{{"udp", {{"host", host}, {"endpoint", endpoint}}}}.dump();
                issue.clear();
            }
            else if (action == "startup-route" && argument.size() == 1 && argument[0] >= '0' && argument[0] <= '3')
                startupRoute = static_cast<size_t>(argument[0] - '0');
            else if (action == "return") returnRequest = true;
            else if (action == "phrases") phrases = !phrases;
            else if (action == "phrase-page") {
                constexpr auto pages = (th09::desktop::quickPhrases.size() + phrasesPerPage - 1) / phrasesPerPage;
                phrasePage = (phrasePage + 1) % pages;
            }
            else if (action == "join-code" || action == "join" || action == "watch") {
                const auto code = action == "join-code" ?
                    static_cast<Rml::ElementFormControlInput*>(element("code"))->GetValue() : argument;
                issue.clear();
                send({{"type","join"},{"code",code},{"spectator",action == "watch"}});
            } else if (action == "seat") {
                send({{"type","take-seat"},{"seat",std::stoi(argument)},{"loadout",std::stoi(argument)},{"nativeRuntime",true}});
            } else if (action == "ready" && argument == "0") send({{"type","set-ready"},{"ready",false}});
            else if (action == "ready") {
                if (build.empty() || generation.empty()) issue = "缺少与联机服务器匹配的游戏版本";
                else {
                    send({{"type","resource-progress"},{"status","ready"},{"stage","runtime"},{"percent",100},{"nativeRuntime",true}});
                    send({{"type","set-ready"},{"ready",true},{"runtimeGeneration",generation},{"nativeRuntime",true},
                        {"swarmDirect",std::filesystem::exists(std::filesystem::path(SDL_GetBasePath()) / "eagler-swarm-turn.exe")}});
                }
            } else if (action == "open-account") send({{"type","open-account"}});
            else if (action == "send-chat") {
                auto* input = static_cast<Rml::ElementFormControlInput*>(element("message"));
                if (!input->GetValue().empty())
                    send({{"type","free-chat"},{"text",input->GetValue()},{"serial",field(room(),"startSerial",0u)}});
                input->SetValue("");
            } else if (action == "swarm") send({{"type","relay-preference"},{"swarm",argument == "1"}});
            else if (action == "phrase") sendPhrase(argument);
            else if (action == "create" || action == "leave" || action == "directory" || action == "spectate" || action == "start") {
                issue.clear();
                if (action == "leave" || action == "create") lastRoomRun = {};
                send({{"type",action}});
            }
            changed = true;
            event.StopPropagation();
            return;
        }
    }
} actions;

bool ensureLobby() {
    if (lobby) return true;
    auto relay = environment("EAGLER_DESKTOP_RELAY");
    if (relay.empty()) return false;
    lobby = std::make_unique<NativeLobby>(NativeLobby::Config{std::move(relay),"th09mp",makeId(),makeId(),2,
        environment("EAGLER_DESKTOP_OVERLAY_SITE"), environment("EAGLER_DESKTOP_OVERLAY")});
    return true;
}

bool ensureDocument() {
    if (document) return true;
    if (!renderer) renderer = std::make_unique<RmlGles>();
    RmlGles::Frame frame(*renderer);
    if (!renderer->Start()) return false;
    if (!running) {
        systemInterface = std::make_unique<SystemInterface_SDL>(owner);
        Rml::SetSystemInterface(systemInterface.get());
        if (!Rml::Initialise()) return false;
        running = true;
        const auto path = std::filesystem::path(environment("SystemRoot")) / "Fonts";
        const std::array fonts{environment("EAGLER_OVERLAY_FONT"), (path / "msyh.ttc").string(),
            (path / "Deng.ttf").string(), (path / "meiryo.ttc").string(), (path / "segoeui.ttf").string()};
        for (const auto& font : fonts) {
            if (font.empty() || !std::filesystem::exists(font)) continue;
            if (Rml::LoadFontFace(font, "Room", Rml::Style::FontStyle::Normal, Rml::Style::FontWeight::Normal)) break;
        }
    }
    if (!context) context = Rml::CreateContext("th09", {640, 480}, renderer.get(), &editor);
    if (!context) return false;
    document = context->LoadDocumentFromMemory(th09::desktop::roomDocument);
    if (!document) return false;
    document->AddEventListener("click", &actions);
    document->Show(Rml::ModalFlag::None, Rml::FocusFlag::None);
    changed = true;
    return true;
}
} // namespace

bool Open(SDL_Window* window) {
    owner = window;
    build = environment("EAGLER_TH09_BUILD");
    generation = environment("EAGLER_TH09_ROOM_GENERATION");
    if (!validId(build,"0123456789abcdef",24,24)) build.clear();
    if (!validId(generation,"0123456789abcdef",64,64)) generation.clear();
    return true;
}

void Close() {
    SetRouteSwitch(nullptr, -1, nullptr, -1, false);
    hide();
    lobby.reset();
    if (running) Rml::Shutdown();
    running = false;
    context = nullptr;
    document = nullptr;
    layoutSize = {};
    renderer.reset();
    systemInterface.reset();
    owner = nullptr;
    snapshot = nullptr;
    issue.clear();
    directPage = false;
    directStart.clear();
    directPort.clear();
    connectionStatus.clear();
    gameChat.clear();
    refreshChatAt = 0;
    lastRoomRun = {};
    completed = total = 0;
    phase = RoomPhase::idle;
    rendered = recovering = escapeAvailable = escapeRequest = returnRequest = false;
    changed = true;
}

bool ShowLobby() {
    if (!owner || !ensureDocument()) return false;
    if (!directPage && !ensureLobby()) issue = "尚未配置联机服务器";
    if (!visible) {
        visible = true;
        updateCursor();
        changed = true;
    }
    return true;
}

bool HandleEvent(const SDL_Event& event) {
    if (!owner) return false;
    if (event.type == SDL_EVENT_KEY_DOWN && event.key.scancode == SDL_SCANCODE_ESCAPE && escapeAvailable) {
        if (!event.key.repeat) escapeRequest = true;
        return true;
    }
    if ((event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP) && event.key.scancode == SDL_SCANCODE_F2) {
        if (event.type == SDL_EVENT_KEY_DOWN && !event.key.repeat) {
            if (visible) hide();
            else ShowLobby();
        }
        return true;
    }
    if (!directPage && gameOwnsInput() && event.type == SDL_EVENT_KEY_UP && event.key.scancode == SDL_SCANCODE_LALT &&
        !(event.key.mod & (SDL_KMOD_CTRL | SDL_KMOD_SHIFT | SDL_KMOD_GUI))) {
        if (visible && phrases) hide();
        else if (ShowLobby()) { phrases = true; changed = true; }
        return true;
    }
    if (!context || (!visible && !routeSwitchAvailable && !routeSwitchPressed)) return false;
    if (event.type == SDL_EVENT_WINDOW_FOCUS_LOST) {
        routeSwitchPressed = false;
        if (auto* focus = context->GetFocusElement()) focus->Blur();
        context->ProcessMouseLeave();
        return false;
    }
    if (event.type == SDL_EVENT_TEXT_EDITING && CapturesKeyboard()) {
        editor.HandleEdit(event.edit);
        return true;
    }
    if (event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP || event.type == SDL_EVENT_TEXT_INPUT) {
        if (!CapturesKeyboard()) return false;
        auto translated = event;
        RmlSDL::InputEventHandler(context,owner,translated);
        return true;
    }
    if (event.type != SDL_EVENT_MOUSE_MOTION && event.type != SDL_EVENT_MOUSE_BUTTON_DOWN &&
        event.type != SDL_EVENT_MOUSE_BUTTON_UP && event.type != SDL_EVENT_MOUSE_WHEEL) return false;
    int windowW = 0, windowH = 0, pixelW = 0, pixelH = 0;
    if (!SDL_GetWindowSize(owner,&windowW,&windowH) || !SDL_GetWindowSizeInPixels(owner,&pixelW,&pixelH) ||
        !windowW || !windowH) return false;
    auto pointer = event;
    const auto translate = [&](float& x, float& y) {
        x = (x * pixelW / windowW - viewX) / scale;
        y = (y * pixelH / windowH - viewY) / scale;
    };
    if (pointer.type == SDL_EVENT_MOUSE_MOTION) translate(pointer.motion.x,pointer.motion.y);
    else if (pointer.type == SDL_EVENT_MOUSE_WHEEL) translate(pointer.wheel.mouse_x,pointer.wheel.mouse_y);
    else {
        translate(pointer.button.x,pointer.button.y);
        if (pointer.button.button == SDL_BUTTON_LEFT) {
            auto* button = element("route-switch-button");
            const bool hit = routeSwitchAvailable && rendered &&
                button->IsPointWithinElement({pointer.button.x,pointer.button.y});
            if (pointer.type == SDL_EVENT_MOUSE_BUTTON_DOWN && hit) {
                routeSwitchPressed = true;
                return true;
            }
            if (pointer.type == SDL_EVENT_MOUSE_BUTTON_UP && std::exchange(routeSwitchPressed, false)) {
                if (hit) routeSwitchRequest = true;
                return true;
            }
        }
        context->ProcessMouseMove(static_cast<int>(pointer.button.x),static_cast<int>(pointer.button.y),RmlSDL::GetKeyModifierState());
    }
    if (!visible) {
        if (pointer.type == SDL_EVENT_MOUSE_MOTION)
            context->ProcessMouseMove(static_cast<int>(pointer.motion.x),static_cast<int>(pointer.motion.y),RmlSDL::GetKeyModifierState());
        return false;
    }
    return !RmlSDL::InputEventHandler(context,owner,pointer);
}

bool CapturesKeyboard() {
    if (!visible || !context) return false;
    const auto* focused = context->GetFocusElement();
    return focused && focused->GetTagName() == "input";
}

bool RoomMusicAudible() { return false; }
bool NeedsPresentation() {
    return visible || recovering || escapeAvailable || !connectionStatus.empty() || !gameChat.empty() || !routeSwitchText.empty() || rendered;
}

void Composite(int x, int y, int width, int height) {
#if TH09_RMLUI_TEST
    static const auto automation = environment("TH09_RMLUI_AUTOMATION");
    static unsigned tick = 0;
    if (!automation.empty() && ++tick % 20 == 0) {
        const auto input = std::filesystem::path(automation) / "action.json";
        std::ifstream file(input);
        if (file) {
            auto command = Json::parse(file,nullptr,false);
            file.close();
            std::filesystem::remove(input);
            const auto kind = field(command,"type",std::string{});
            if (kind == "show") ShowLobby();
            else if (kind == "hide") hide();
            else if (kind == "resize") SDL_SetWindowSize(owner,320,240);
            else if ((kind == "udp-listen" || kind == "udp-join") && ShowLobby()) {
                const bool host = kind == "udp-listen";
                element("open-direct")->Click();
                static_cast<Rml::ElementFormControlInput*>(element(host ? "direct-port" : "direct-address"))->
                    SetValue(field(command, "endpoint", std::string{}));
                element(host ? "direct-listen" : "direct-join")->Click();
            }
            else if (kind == "esc" || kind == "confirm") {
                SDL_Event event{};
                event.type = SDL_EVENT_KEY_DOWN;
                event.key.scancode = kind == "confirm" ? SDL_SCANCODE_Z : SDL_SCANCODE_ESCAPE;
                SDL_PushEvent(&event);
            } else send(std::move(command));
        }
    }
#endif
    if (refreshChatAt) {
        const auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count();
        if (now >= refreshChatAt) updateGameChat();
    }
    rendered = document && (visible || recovering || escapeAvailable || !connectionStatus.empty() || !gameChat.empty() || !routeSwitchText.empty()) &&
        width > 0 && height > 0;
    if (!rendered) return;
    const auto frameStarted = SDL_GetTicksNS();
    int windowH = 0;
    if (!SDL_GetWindowSizeInPixels(owner,nullptr,&windowH)) return;
    viewX = x;
    viewY = y;
    scale = std::max(1.f,std::min(width / 640.f,height / 480.f));
    const Rml::Vector2i dimensions{std::max(1,int(width / scale)),std::max(1,int(height / scale))};
    RmlGles::Frame frame(*renderer);
    frame.Viewport(x,windowH - y - height,width,height,dimensions);
    if (layoutSize != dimensions) {
        layoutSize = dimensions;
        context->SetDimensions(dimensions);
        const int panelWidth = std::max(1,std::min(342,dimensions.x - 10));
        const int panelHeight = std::max(1,dimensions.y - 10);
        const auto length = [](int pixels) { return std::to_string(std::max(1,pixels)) + "px"; };
        element("panel")->SetProperty("width",length(panelWidth));
        element("panel")->SetProperty("height",length(panelHeight));
        element("main")->SetProperty("width",length(panelWidth - 16));
        element("main")->SetProperty("height",length(panelHeight - 70));
        element("footer")->SetProperty("width",length(panelWidth - 15));
    }
    if (std::exchange(changed,false)) updateDocument();
    context->Update();
    context->Render();
    const auto renderDuration = SDL_GetTicksNS() - frameStarted;
#if TH09_RMLUI_TEST
    if (!automation.empty()) {
        const auto capture = std::filesystem::path(automation) / "capture";
        if (std::filesystem::remove(capture)) {
            SDL_Log("RmlUi capture layout %dx%d panel=%.0f main=%.0f seats=%.0f",
                dimensions.x, dimensions.y, element("panel")->GetClientWidth(),
                element("main")->GetClientWidth(), element("seats")->GetClientWidth());
            std::vector<unsigned char> rgba(static_cast<size_t>(width) * height * 4);
            GLint readFramebuffer = 0;
            glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING,&readFramebuffer);
            glBindFramebuffer(GL_READ_FRAMEBUFFER,0);
            glReadPixels(x,windowH - y - height,width,height,GL_RGBA,GL_UNSIGNED_BYTE,rgba.data());
            glBindFramebuffer(GL_READ_FRAMEBUFFER,readFramebuffer);
            if (auto* surface = SDL_CreateSurfaceFrom(width,height,SDL_PIXELFORMAT_RGBA32,rgba.data(),width*4)) {
                SDL_FlipSurface(surface,SDL_FLIP_VERTICAL);
                SDL_SaveBMP(surface,(std::filesystem::path(automation) / "overlay.bmp").string().c_str());
                SDL_DestroySurface(surface);
            }
        }
    }
#endif
    static const bool benchmark = !environment("EAGLER_RMLUI_BENCH").empty();
    if (benchmark) {
        static unsigned samples = 0;
        static Uint64 sum = 0, maximum = 0;
        const auto duration = renderDuration;
        if (++samples > 30) {
            sum += duration;
            maximum = std::max(maximum,duration);
        }
        if (samples == 270) {
            SDL_Log("RmlUi overlay CPU: %.3f ms mean, %.3f ms max (240 frames)",sum / 240e6,maximum / 1e6);
        }
    }
}

void SetRoomPhase(RoomPhase next, unsigned done, unsigned count) {
    if (phase == next && completed == done && total == count) return;
    const bool wasInGame = gameOwnsInput();
    phase = next;
    updateGameChat();
    completed = done;
    total = count;
    changed = true;
    if (gameOwnsInput() && !wasInGame) hide();
    if (phase == RoomPhase::connecting || phase == RoomPhase::loadingSpectator) ShowLobby();
}

void SetConnectionStatus(ConnectionStatus status) {
    std::string text;
    if (status.route) {
        text = "<span class='peer-label'>P" + std::to_string(status.player) + "</span><span class='peer-route'>" +
            Rml::StringUtilities::EncodeRml(status.route) + "</span>";
        if (status.rtt >= 0) {
            const auto variation = status.variation >= 0 ? std::to_string(status.variation) : "--";
            text += "<span class='peer-rtt'>" + std::to_string(status.rtt) +
                "<small>ms</small><span class='peer-variation'>∿ " + variation + "<small>ms</small></span></span>";
        } else if (status.probesUnanswered) text += "<span class='peer-rtt'>延迟探测超时</span>";
    }
    if (connectionStatus != text) {
        connectionStatus = std::move(text);
        changed = true;
    }
}

void SetTransportRecovering(bool next) {
    if (recovering == next) return;
    recovering = next;
    changed = true;
}

void SetRouteSwitch(const char* from, int fromMs, const char* to, int toMs, bool switching) {
    const bool available = from && to && !switching;
    std::string text;
    if (switching) text = "正在切换…";
    else if (available) text = std::string(from) + " " + (fromMs < 0 ? "--" : std::to_string(fromMs)) +
        " ms → " + to + " " + std::to_string(toMs) + " ms";
    if (text == routeSwitchText && available == routeSwitchAvailable) return;
    routeSwitchText = std::move(text);
    routeSwitchAvailable = available;
    if (!available) routeSwitchRequest = false;
    if (!routeSwitchText.empty()) ensureDocument();
    updateCursor();
    changed = true;
}

bool TakeRouteSwitchRequest() { return std::exchange(routeSwitchRequest, false); }

void SetEscapeAvailable(bool next) {
    if (escapeAvailable == next) return;
    escapeAvailable = next;
    changed = true;
    if (next && phase != RoomPhase::idle) SDL_Log("Peer unresponsive: Esc can return to title");
}

bool TakeEscapeRequest() { return std::exchange(escapeRequest,false); }
bool TakeReturnRequest() { return std::exchange(returnRequest,false); }
const char* InitialRouteChoice() { return startupRoutes[startupRoute]; }

void FinishRoom(const char* error, bool showRoom) {
    SetRouteSwitch(nullptr, -1, nullptr, -1, false);
    SetTransportRecovering(false);
    SetRoomPhase(RoomPhase::idle);
    issue = error ? error : "";
    if (showRoom) ShowLobby();
    else {
        hide();
        if (!directPage) send({{"type","leave"}});
    }
    SetEscapeAvailable(showRoom && !issue.empty());
    changed = true;
}

void AdoptRoom(const char* options) {
    const auto config = Json::parse(options,nullptr,false);
    if (const auto udp = field(config, "udp", Json{}); udp.is_object()) {
        directPage = true;
        directPort = field(udp,"host",false) ? field(udp,"endpoint",std::string{}) : "";
        phrases = false;
        gameChat.clear();
        changed = true;
        return;
    }
    directPage = false;
    const auto url = field(config,"netplayUrl",std::string{});
    const auto client = field(config,"lobbyClientId",std::string{});
    const auto member = query(url,"member");
    const auto id = query(url,"room");
    constexpr std::string_view alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-";
    if (id.rfind("th09mp-",0) || !validId(client,alphabet,8,64) || !validId(member,alphabet,8,64)) return;
    const auto serial = field(config,"run",0u);
    const auto roomRun = std::make_pair(id.substr(7), serial);
    if (lobby && roomRun == lastRoomRun && client == lobby->Settings().client) return;
    auto relay = url.substr(0,url.find('?'));
    if (relay.rfind("ws://",0) && relay.rfind("wss://",0)) return;
    lobby = std::make_unique<NativeLobby>(NativeLobby::Config{std::move(relay),"th09mp",client,member,2,
        environment("EAGLER_DESKTOP_OVERLAY_SITE"), environment("EAGLER_DESKTOP_OVERLAY")});
    lastRoomRun = roomRun;
    send({{"type","join"},{"code",roomRun.first},{"spectator",field(config,"netplaySpectator",false)}});
}

void ReportMatch(std::uint32_t, std::uint32_t, unsigned, unsigned, unsigned) {
    // Match reports go over the existing authenticated gameplay signal socket.
}

std::string TakeRoomStart() {
    if (!directStart.empty()) return std::exchange(directStart, {});
    if (directPage) return {};
    if (!lobby) return {};
    auto next = lobby->TakeSnapshot();
    if (!next.is_object()) return {};
    snapshot = std::move(next);
    updateGameChat();
    changed = true;
    if (phase != RoomPhase::idle) return {};
    const auto current = room();
    const auto start = field(snapshot,"start",Json{});
    const auto serial = field(start,"serial",0u);
    const bool spectator = field(start,"spectator",false);
    const int side = spectator ? 0 : seatOf(current);
    const auto code = field(snapshot,"code",std::string{});
    const auto roomRun = std::make_pair(code, serial);
    if (!serial || roomRun == lastRoomRun || side < 0 || side > 1 || build.empty() || generation.empty() ||
        !validId(code,"0123456789",4,8)) return {};
    const auto& config = lobby->Settings();
    const auto roomGeneration = field(current,"runtimeGeneration",generation);
    if (roomGeneration != generation) {
        issue = "房间与本机联机版本不一致";
        return {};
    }
    lastRoomRun = roomRun;
    const auto url = config.relay + (config.relay.find('?') == std::string::npos ? "?" : "&") +
        "room=th09mp-" + code + "&run=" + std::to_string(serial) +
        "&member=" + config.member + "&runtimeGeneration=" + generation;
    const auto gameplay = url +
        (spectator ? "&spectator=" + config.client : "&player=" + std::to_string(side));
    const auto signal = gameplay + "&players=2" + (spectator ? "" : "&signal=1");
    const auto& roster = seats(current);
    Json loadouts = Json::array();
    for (unsigned index = 0; index < 2; ++index) {
        const auto loadout = index < roster.size() ? field(roster[index],"loadout",index) : index;
        loadouts.push_back({{"character",loadout}});
    }
    issue.clear();
    return Json{{"build",build},{"netplayUrl",gameplay},{"signalUrl",signal},{"run",serial},
        {"lobbyClientId",config.client},{"netplayPlayer",side},{"netplayPlayerCount",2},
        {"netplaySeed",std::stoul(code) & 0xffffu},{"netplayInGameSelection",true},{"netplayLoadouts",loadouts},
        {"netplayDifficulty",field(current,"difficulty",1u)},{"netplayAdonisMode",field(current,"adonisMode",0u)},
        {"netplayInputDelay",field(current,"inputDelay",0u)},{"netplayInputDelayAuto",field(current,"inputDelayAuto",false)},
        {"netplayPredictionReserve",field(current,"predictionReserve",2u)},
        {"netplayDisableLocalChargePriority",field(current,"disableLocalChargePriority",false)},
        {"netplayPreferTurn",false},
        {"netplaySpectator",spectator},{"netplaySpectatorTimeline",field(current,"spectatorTimeline",false)}}.dump();
}
} // namespace Th09Overlay
