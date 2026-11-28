import 'dart:io';

import 'package:material_ui/material_ui.dart';
import 'package:window_manager/window_manager.dart' as windows;

import 'launcher_navigation.dart';
import 'l10n/strings.dart';

windows.Window? _window;

bool get hasDesktopWindow => _window != null;

void initializeDesktopWindow() {
  if (!(Platform.isWindows || Platform.isMacOS || Platform.isLinux)) return;
  final window = windows.WindowManager.instance.getCurrent();
  if (window == null) return;
  window.minimumSize = const Size(320, 480).toNative();
  window.titleBarStyle = windows.TitleBarStyle.hidden;
  _window = window;
}

/// Uses the library's caption buttons and native maximize region in both bars.
class DesktopWindowControls extends StatefulWidget {
  const DesktopWindowControls({super.key, this.height = 44});

  final double height;

  @override
  State<DesktopWindowControls> createState() => _DesktopWindowControlsState();
}

class _DesktopWindowControlsState extends State<DesktopWindowControls> {
  windows.Window? _controlledWindow;
  windows.ListenerId? _listener;
  bool _maximized = false;

  @override
  void initState() {
    super.initState();
    final window = _controlledWindow = _window;
    if (window != null) {
      _maximized = window.isMaximized;
      _listener = window.addListener(_onWindowEvent);
    }
  }

  void _onWindowEvent(windows.WindowEvent event) {
    if (!mounted ||
        !(event is windows.WindowMaximizedEvent ||
            event is windows.WindowRestoredEvent)) {
      return;
    }
    final maximized = _controlledWindow!.isMaximized;
    if (maximized != _maximized) setState(() => _maximized = maximized);
  }

  @override
  void dispose() {
    if (_listener != null) _controlledWindow?.removeListener(_listener!);
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    final window = _controlledWindow;
    if (window == null) return const SizedBox.shrink();
    final brightness = Theme.of(context).brightness;
    final strings = context.strings;
    return SizedBox(
      width: 138,
      height: widget.height,
      child: Row(
        children: [
          Semantics(
            label: strings.windowMinimize,
            child: windows.WindowCaptionButton.minimize(
              brightness: brightness,
              onPressed: () {
                if (window.isMinimized) {
                  window.restore();
                } else {
                  window.minimize();
                }
              },
            ),
          ),
          windows.MaximizeButtonArea(
            window: window,
            child: Semantics(
              label: _maximized
                  ? strings.windowRestore
                  : strings.windowMaximize,
              child: _maximized
                  ? windows.WindowCaptionButton.unmaximize(
                      brightness: brightness,
                      onPressed: window.unmaximize,
                    )
                  : windows.WindowCaptionButton.maximize(
                      brightness: brightness,
                      onPressed: window.maximize,
                    ),
            ),
          ),
          Semantics(
            label: MaterialLocalizations.of(context).closeButtonTooltip,
            child: windows.WindowCaptionButton.close(
              brightness: brightness,
              onPressed: window.close,
            ),
          ),
        ],
      ),
    );
  }
}

class DesktopDragArea extends StatelessWidget {
  const DesktopDragArea({super.key, required this.child});

  final Widget child;

  @override
  Widget build(BuildContext context) => hasDesktopWindow
      ? windows.DragToMoveArea(window: _window, child: child)
      : child;
}

/// The outer viewport stays stable while the navigator changes pages.
class DesktopWindowFrame extends StatelessWidget {
  const DesktopWindowFrame({super.key, required this.child});

  final Widget child;

  @override
  Widget build(BuildContext context) {
    if (!hasDesktopWindow) return child;
    final compact = MediaQuery.sizeOf(context).width <= launcherCompactWidth;
    return windows.VirtualWindowFrame(
      window: _window,
      child: Column(
        children: [
          if (compact) const _WindowCaption(),
          Expanded(child: Semantics(container: true, child: child)),
        ],
      ),
    );
  }
}

/// A page outside the main masthead carries its caption inside its own route.
class DesktopWindowPage extends StatelessWidget {
  const DesktopWindowPage({super.key, required this.child});

  final Widget child;

  @override
  Widget build(BuildContext context) =>
      !hasDesktopWindow ||
          MediaQuery.sizeOf(context).width <= launcherCompactWidth
      ? child
      : Column(
          children: [
            const _WindowCaption(),
            Expanded(child: child),
          ],
        );
}

class _WindowCaption extends StatelessWidget {
  const _WindowCaption();

  @override
  Widget build(BuildContext context) {
    final theme = Theme.of(context);
    return SizedBox(
      height: windows.kWindowCaptionHeight,
      child: ColoredBox(
        color: theme.colorScheme.surfaceContainerLow,
        child: Row(
          children: [
            Expanded(
              child: DesktopDragArea(
                child: SizedBox(
                  height: double.infinity,
                  child: Row(
                    children: [
                      Container(
                        padding: const EdgeInsets.only(left: 16),
                        child: DefaultTextStyle(
                          style: TextStyle(
                            color: theme.brightness == Brightness.light
                                ? Colors.black.withValues(alpha: 0.8956)
                                : Colors.white,
                            fontSize: 14,
                          ),
                          child: const Text('TouhouOnline'),
                        ),
                      ),
                    ],
                  ),
                ),
              ),
            ),
            const DesktopWindowControls(height: windows.kWindowCaptionHeight),
          ],
        ),
      ),
    );
  }
}
