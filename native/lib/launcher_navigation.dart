import 'package:cupertino_ui/cupertino_ui.dart'
    show CupertinoPageTransition, CupertinoPageTransitionsBuilder;
import 'package:go_router/go_router.dart';
import 'package:material_ui/material_ui.dart';
import 'package:prism_ui/prism_ui.dart' as prism;

import 'catalog.dart';
import 'launcher_background.dart';
import 'launcher_wallpaper.dart';
import 'room_session.dart';

const launcherCompactWidth = 780.0;

/// Connects each mounted form to GoRouter's per-page exit callback.
class LauncherExitGuards {
  final _checks = <LocalKey, Future<bool> Function()>{};

  LauncherExitGuard page(LocalKey key) => LauncherExitGuard._(this, key);

  Future<bool> allowExit(LocalKey key) async {
    final check = _checks[key];
    return check == null || await check();
  }
}

class LauncherExitGuard {
  LauncherExitGuard._(this._registry, this._key);

  final LauncherExitGuards _registry;
  final LocalKey _key;

  VoidCallback bind(Future<bool> Function() check) {
    _registry._checks[_key] = check;
    return () {
      if (identical(_registry._checks[_key], check)) {
        _registry._checks.remove(_key);
      }
    };
  }
}

const launcherBottomBarHeight = 66.0;
const roomLinkUnavailablePath = '/lobby/invite-unavailable';

String gamePath(String id, {bool multiplayer = false}) =>
    '${multiplayer ? '/lobby' : ''}/game/${Uri.encodeComponent(id)}';

String? launcherGameLink(Uri address) {
  if ((address.path.isNotEmpty && address.path != '/') ||
      address.userInfo.isNotEmpty) {
    return null;
  }
  final allowed = switch (address.scheme) {
    '' => !address.hasAuthority,
    'https' => address.host == 'touhouonline.com' && address.port == 443,
    'touhouonline' => address.host == 'open' && !address.hasPort,
    _ => false,
  };
  if (!allowed) return null;
  try {
    final values = address.queryParametersAll['game'];
    if (values == null) return null;
    final id = values.length == 1 ? values.single : '';
    if (RegExp(r'^th[0-9]{2,3}$').hasMatch(id)) return gamePath(id);
  } on FormatException {
    // The platform can deliver a percent-encoded query with invalid UTF-8.
  }
  return gamePath('unavailable');
}

String? launcherEntryLocation(
  Uri address,
  Iterable<Game> games,
  Uri roomOrigin,
) {
  final root =
      (address.path.isEmpty || address.path == '/') && address.userInfo.isEmpty;
  final allowed = switch (address.scheme) {
    '' => !address.hasAuthority,
    'touhouonline' => address.authority == 'open',
    _ =>
      address.scheme == roomOrigin.scheme &&
          address.host == roomOrigin.host &&
          address.port == roomOrigin.port,
  };
  if (root && allowed) {
    try {
      final all = address.queryParametersAll;
      if (all.containsKey('j') || all.containsKey('mpRoom')) {
        return RoomRequest.fromInviteQuery(all, games)?.location ??
            roomLinkUnavailablePath;
      }
    } on FormatException {
      if (RegExp(r'(^|&)(j|mpRoom)(=|&|$)').hasMatch(address.query)) {
        return roomLinkUnavailablePath;
      }
    }
  }
  return launcherGameLink(address);
}

void openLibraryGame(BuildContext context, String id) {
  final next = gamePath(id);
  final current = GoRouterState.of(context).uri.path;
  if (current == next) return;
  if (current.endsWith('/settings') && current.startsWith('/game/')) {
    context.go(next);
  } else if (current.startsWith('/game/')) {
    context.pushReplacement(next);
  } else {
    context.push(next);
  }
}

void closeLauncherPage(BuildContext context, String fallback) {
  if (ModalRoute.of(context)?.isCurrent == false) return;
  if (context.canPop()) {
    context.pop();
  } else {
    context.go(fallback);
  }
}

Page<void> launcherGamePage(GoRouterState state, Widget child) =>
    MaterialPage<void>(
      key: state.pageKey,
      name: state.uri.path,
      arguments: {...state.pathParameters, ...state.uri.queryParameters},
      restorationId: state.pageKey.value,
      child: child,
    );

class LauncherPageTransitions extends PageTransitionsTheme {
  const LauncherPageTransitions({required this.reduced});

  final bool reduced;
  static final _normal =
      Map<TargetPlatform, PageTransitionsBuilder>.unmodifiable({
        ...const PageTransitionsTheme().builders,
        TargetPlatform.iOS: const _LauncherCupertinoTransitions(),
        for (final platform in [
          TargetPlatform.windows,
          TargetPlatform.macOS,
          TargetPlatform.linux,
        ])
          platform: const _DesktopFade(),
      });
  static final _still =
      Map<TargetPlatform, PageTransitionsBuilder>.unmodifiable({
        for (final platform in TargetPlatform.values)
          if (platform != TargetPlatform.iOS) platform: const _NoPageMotion(),
        TargetPlatform.iOS: const _LauncherCupertinoTransitions(reduced: true),
      });

  @override
  Map<TargetPlatform, PageTransitionsBuilder> get builders =>
      reduced ? _still : _normal;

  @override
  Widget buildTransitions<T>(
    PageRoute<T> route,
    BuildContext context,
    Animation<double> animation,
    Animation<double> secondaryAnimation,
    Widget child,
  ) {
    child = LauncherPageSurface(child: child);
    final ios = Theme.of(context).platform == TargetPlatform.iOS;
    if (MediaQuery.disableAnimationsOf(context) && !ios) return child;
    return super.buildTransitions(
      route,
      context,
      animation,
      secondaryAnimation,
      child,
    );
  }
}

class _LauncherCupertinoTransitions extends CupertinoPageTransitionsBuilder {
  const _LauncherCupertinoTransitions({this.reduced = false});

  final bool reduced;

  @override
  Duration get transitionDuration =>
      reduced ? Duration.zero : super.transitionDuration;

  // A shared delegate keeps covered routes in the same subtree as motion changes.
  static Widget? _covered(
    BuildContext context,
    Animation<double> animation,
    Animation<double> secondaryAnimation,
    bool allowSnapshotting,
    Widget? child,
  ) => MediaQuery.disableAnimationsOf(context)
      ? child
      : CupertinoPageTransition.delegatedTransition(
          context,
          animation,
          secondaryAnimation,
          allowSnapshotting,
          child,
        );

  @override
  DelegatedTransitionBuilder get delegatedTransition => _covered;

  @override
  Widget buildTransitions<T>(
    PageRoute<T> route,
    BuildContext context,
    Animation<double> animation,
    Animation<double> secondaryAnimation,
    Widget child,
  ) {
    final still = MediaQuery.disableAnimationsOf(context);
    return super.buildTransitions<T>(
      route,
      context,
      still ? const AlwaysStoppedAnimation(1.0) : animation,
      still ? const AlwaysStoppedAnimation(0.0) : secondaryAnimation,
      FadeTransition(
        opacity: still ? animation : const AlwaysStoppedAnimation(1.0),
        child: child,
      ),
    );
  }
}

// Prism uses a continuous desktop fade. The covered page stays opaque so
// navigation never exposes an empty surface between the two pages.
class _DesktopFade extends PageTransitionsBuilder {
  const _DesktopFade();

  @override
  Duration get transitionDuration => const Duration(milliseconds: 200);

  @override
  Widget buildTransitions<T>(
    PageRoute<T> route,
    BuildContext context,
    Animation<double> animation,
    Animation<double> secondaryAnimation,
    Widget child,
  ) => FadeTransition(opacity: animation, child: child);
}

class _NoPageMotion extends PageTransitionsBuilder {
  const _NoPageMotion();

  @override
  Duration get transitionDuration => Duration.zero;

  @override
  Widget buildTransitions<T>(
    PageRoute<T> route,
    BuildContext context,
    Animation<double> animation,
    Animation<double> secondaryAnimation,
    Widget child,
  ) => child;
}

class LauncherBranches extends StatefulWidget {
  const LauncherBranches({
    super.key,
    required this.index,
    required this.children,
  });

  final int index;
  final List<Widget> children;

  @override
  State<LauncherBranches> createState() => _LauncherBranchesState();
}

class _LauncherBranchesState extends State<LauncherBranches>
    with SingleTickerProviderStateMixin {
  late final _controller = AnimationController(
    vsync: this,
    duration: prism.fadeTabDuration,
    value: 1,
  );
  late final Animation<double> _progress = _controller.drive(
    CurveTween(curve: prism.fadeTabCurve),
  );
  late List<double> _from = List.filled(widget.children.length, 0.0);
  bool _reducedMotion = false;

  List<double> _weights(int target) {
    final progress = _progress.value;
    return [
      for (var index = 0; index < widget.children.length; index++)
        _from[index] * (1 - progress) + (index == target ? progress : 0),
    ];
  }

  @override
  void didChangeDependencies() {
    super.didChangeDependencies();
    _reducedMotion = MediaQuery.disableAnimationsOf(context);
    if (_reducedMotion) _controller.value = 1;
  }

  @override
  void didUpdateWidget(covariant LauncherBranches oldWidget) {
    super.didUpdateWidget(oldWidget);
    if (widget.index == oldWidget.index) return;
    if (_reducedMotion) {
      _controller.value = 1;
    } else {
      _from = _weights(oldWidget.index);
      _controller.forward(from: 0);
    }
  }

  @override
  void dispose() {
    _controller.dispose();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) => AnimatedBuilder(
    animation: _controller,
    builder: (context, _) {
      final current = widget.index;
      final weights = _weights(current);
      final backdrop = LauncherBackdrop.page(context);
      final background = backdrop ?? Theme.of(context).colorScheme.surface;
      final layers = <Widget>[];
      var covered = 0.0;
      for (final (index, child) in widget.children.indexed) {
        final weight = weights[index];
        covered += weight;
        // Source-over blending needs an opaque base on plain surfaces.
        // With wallpaper, fade each branch into the shared background.
        final opacity = backdrop == null && covered > 0
            ? weight / covered
            : weight;
        layers.add(
          Offstage(
            key: ValueKey(index),
            offstage: index != current && weight == 0,
            child: ExcludeFocus(
              excluding: index != current,
              child: ExcludeSemantics(
                excluding: index != current,
                child: IgnorePointer(
                  ignoring: index != current,
                  child: TickerMode(
                    enabled: index == current,
                    child: FadeTransition(
                      opacity: AlwaysStoppedAnimation(opacity),
                      alwaysIncludeSemantics: index == current,
                      child: Material(color: background, child: child),
                    ),
                  ),
                ),
              ),
            ),
          ),
        );
      }
      return Stack(fit: StackFit.expand, children: layers);
    },
  );
}
