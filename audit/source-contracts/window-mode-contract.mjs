import fs from 'node:fs';
import path from 'node:path';
import process from 'node:process';
import { fileURLToPath } from 'node:url';

const root = path.resolve(path.dirname(fileURLToPath(import.meta.url)), '../..');
const read = (name) => fs.readFileSync(path.join(root, name), 'utf8');
const requireText = (text, needle, label) => {
  if (!text.includes(needle)) throw new Error(`missing ${label}: ${needle}`);
};

const supervisor = read('src/Supervisor.cpp');
const gameWindow = read('src/GameWindow.cpp');
const gameWindowHpp = read('src/GameWindow.hpp');
const controller = read('src/Controller.cpp');
const controllerHpp = read('src/Controller.hpp');
const main = read('src/main.cpp');

requireText(supervisor, 'g_Supervisor.cfg.windowed = 1;', 'windowed default');
requireText(gameWindow, 'SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, 640', 'desktop 640 width');
requireText(gameWindow, 'SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, 480', 'desktop 480 height');
requireText(gameWindow, 'GameWindow::ToggleFullscreen();', 'saved fullscreen post-create transition');
requireText(gameWindowHpp, 'static void ToggleFullscreen();', 'fullscreen toggle declaration');
requireText(gameWindowHpp, 'static bool IsFullscreen();', 'fullscreen state declaration');

for (const [needle, label] of [
  ['g_InBorderlessFullscreen', 'Win32 borderless state'],
  ['SetWindowLong(hwnd, GWL_STYLE', 'Win32 style transition'],
  ['WS_POPUP', 'borderless popup style'],
  ['void GameWindow::RememberWindowedState()', 'windowed geometry cache helper'],
  ['(style & WS_CAPTION) == 0', 'transient borderless cache rejection'],
  ['SDL_GetDesktopDisplayMode', 'desktop mode sizing'],
  ['SetWindowPos(hwnd, HWND_TOP', 'exact window rect restore'],
  ['SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER', 'non-client frame restoration before resize'],
  ['SDL_SyncWindow(g_GameWindow.window);', 'window transition synchronization'],
  ['SDL_SetWindowFullscreen(g_GameWindow.window, !fullscreen)', 'non-Windows SDL fallback'],
]) requireText(gameWindow, needle, label);

for (const [needle, label] of [
  ['static bool g_ToggleFullscreenRequested = false;', 'deferred toggle flag'],
  ['if (g_ToggleFullscreenRequested)', 'frame-loop deferred toggle'],
  ['SDL_SCANCODE_KP_ENTER', 'keypad Enter support'],
  ['SDL_KMOD_LALT | SDL_KMOD_RALT', 'left/right Alt detection'],
  ['Controller::SetEnterSuppressed(true);', 'Enter suppression on toggle'],
  ['Controller::SetEnterSuppressed(false);', 'Enter suppression release'],
  ['GameWindow::IsFullscreen()', 'actual fullscreen cursor state'],
  ['GameWindow::RememberWindowedState();', 'focus/create/render geometry capture'],
  ['!defined(__EMSCRIPTEN__)', 'desktop-only Alt+Enter guard'],
]) requireText(main, needle, label);

requireText(controller, 'static bool g_EnterSuppressed;', 'controller suppression state');
requireText(controller, 'if (!g_EnterSuppressed)', 'controller Enter gate');
requireText(controllerHpp, 'void SetEnterSuppressed(bool suppressed);', 'controller suppression API');

// Desktop must not ask SDL to create a fullscreen window directly. The only
// create-time fullscreen property is reserved for Android/iOS before the
// Emscripten/desktop branches.
const fullscreenCreateMatches = [...gameWindow.matchAll(/SDL_PROP_WINDOW_CREATE_FULLSCREEN_BOOLEAN/g)];
if (fullscreenCreateMatches.length !== 1) {
  throw new Error(`expected exactly one mobile create-time fullscreen property, got ${fullscreenCreateMatches.length}`);
}

console.log('th07 window mode source contract: PASS');
