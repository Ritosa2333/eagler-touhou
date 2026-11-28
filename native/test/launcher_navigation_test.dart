import 'dart:convert';
import 'dart:io';
import 'dart:typed_data';
import 'dart:ui' show ImageByteFormat, PointerDeviceKind;

import 'package:crypto/crypto.dart';

import 'package:flutter/foundation.dart'
    show debugDefaultTargetPlatformOverride, listEquals;
import 'package:flutter/rendering.dart' show RenderRepaintBoundary;
import 'package:flutter/services.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:go_router/go_router.dart';
import 'package:material_ui/material_ui.dart';
import 'package:shared_preferences_platform_interface/in_memory_shared_preferences_async.dart';
import 'package:shared_preferences_platform_interface/shared_preferences_async_platform_interface.dart';
import 'package:touhouonline/catalog.dart';
import 'package:touhouonline/desktop_masthead.dart';
import 'package:touhouonline/game_detail.dart';
import 'package:touhouonline/game_settings.dart';
import 'package:touhouonline/launcher_background.dart';
import 'package:touhouonline/launcher_navigation.dart';
import 'package:touhouonline/launcher_wallpaper.dart';
import 'package:touhouonline/library.dart';
import 'package:touhouonline/site_documents.dart';
import 'package:touhouonline/lobby_directory.dart';
import 'package:touhouonline/lobby_page.dart';
import 'package:touhouonline/main.dart' show LauncherApp;
import 'package:touhouonline/site_navigation.dart';

import '../tools/capture_native_baseline_test.dart' show loadFonts, save;
import 'widget_test.dart' as fixtures;

class IdleLobby extends LobbyDirectory {
  IdleLobby(super.games);

  @override
  LobbyConnection get connection => LobbyConnection.live;

  @override
  void start() {}
}

void _size(WidgetTester tester, Size size) {
  tester.view.devicePixelRatio = 1;
  tester.view.physicalSize = size;
  addTearDown(tester.view.resetPhysicalSize);
  addTearDown(tester.view.resetDevicePixelRatio);
}

Finder _descend(Finder surface, Finder element) =>
    find.descendant(of: surface, matching: element);

Finder _side(bool multiplayer, {bool offstage = false}) => find.byKey(
  PageStorageKey(('game sidebar', multiplayer)),
  skipOffstage: !offstage,
);

ScrollPosition _scrollIn(WidgetTester tester, Finder container) => tester
    .state<ScrollableState>(_descend(container, find.byType(Scrollable)).first)
    .position;

List<double> _box(Rect rect) => [rect.left, rect.top, rect.width, rect.height];

Future<void> _tap(WidgetTester tester, Finder target) async {
  expect(target.hitTestable(), findsOneWidget);
  await tester.tap(target);
  await tester.pumpAndSettle();
}

Future<void> _trace(
  WidgetTester tester,
  String name,
  List<Map<String, Object?>> frames,
) async {
  final location = Platform.environment['TOUHOU_NAV_TRACE'];
  if (location == null) return;
  await tester.runAsync(() async {
    final target = Directory(location);
    await target.create(recursive: true);
    final text = const JsonEncoder.withIndent('  ').convert(frames);
    await File('${target.path}/$name.json').writeAsString('$text\n');
  });
}

Future<GoRouter> _show(
  WidgetTester tester,
  List<Game> games, {
  required bool lessMotion,
  ThemeMode theme = ThemeMode.dark,
  GlobalKey? boundary,
  BackgroundMode background = BackgroundMode.plain,
}) async {
  final app = fixtures.fixture(
    games: games,
    accountApi: fixtures.FixtureAccountApi(),
    lobbyDirectory: IdleLobby(games),
  );
  await app.preferences.setLessMotion(lessMotion);
  await app.preferences.setTheme(theme);
  await app.preferences.background.select(background);
  await tester.pumpWidget(
    boundary == null ? app : RepaintBoundary(key: boundary, child: app),
  );
  await tester.pumpAndSettle();
  return GoRouter.of(tester.element(find.byType(LibraryPage)));
}

Finder _branchLabel(String label) =>
    _descend(find.byType(NavigationBar), find.text(label));

Future<void> _warmBookImages(WidgetTester tester, List<Game> games) async {
  final context = tester.element(find.byType(Scaffold).first);
  await tester.runAsync(() async {
    for (final asset in [
      'assets/launcher-wordmark-dark.png',
      'assets/launcher-wordmark-light.png',
      for (final game in games)
        if (game.artwork case final String cover) 'assets/book-covers/$cover',
    ]) {
      await precacheImage(AssetImage(asset), context);
    }
  });
  await tester.pumpAndSettle();
}

Future<List<int>> _pixels(
  WidgetTester tester,
  RenderRepaintBoundary boundary,
  Iterable<Offset> points,
) async => (await tester.runAsync(() async {
  final image = await boundary.toImage();
  try {
    final data = (await image.toByteData(format: ImageByteFormat.rawRgba))!;
    return [
      for (final point in points)
        ...data.buffer.asUint8List(
          data.offsetInBytes +
              (point.dy.floor() * image.width + point.dx.floor()) * 4,
          4,
        ),
    ];
  } finally {
    image.dispose();
  }
}))!;

void main() {
  TestWidgetsFlutterBinding.ensureInitialized();
  late List<Game> games;
  setUpAll(() async => games = await Game.load());
  final sidebarCapture = Platform.environment['TOUHOU_SIDEBAR_CAPTURE'];
  final unfoldCapture = Platform.environment['TOUHOU_UNFOLD_CAPTURE'];
  final iosCapture = Platform.environment['TOUHOU_IOS_BACK_CAPTURE'];
  if (iosCapture != null) {
    setUpAll(() async {
      await loadFonts();
      // Host-only motion evidence. These stand-ins do not verify Apple fonts.
      final fonts = Directory('${Platform.environment['WINDIR']}/Fonts');
      for (final family in ['CupertinoSystemText', 'CupertinoSystemDisplay']) {
        final loader = FontLoader(family);
        for (final name in ['msyh.ttc', 'msyhbd.ttc']) {
          loader.addFont(
            File('${fonts.path}/$name')
                .readAsBytes()
                .then(ByteData.sublistView),
          );
        }
        await loader.load();
      }
      await Directory(iosCapture).create(recursive: true);
    });
  }
  if (sidebarCapture != null || unfoldCapture != null) {
    setUpAll(() async {
      await loadFonts(Directory(Platform.environment['TOUHOU_CAPTURE_FONTS']!));
      if (unfoldCapture != null) {
        await Directory(unfoldCapture).create(recursive: true);
      }
    });
  }
  setUp(
    () => SharedPreferencesAsyncPlatform.instance =
        InMemorySharedPreferencesAsync.empty(),
  );

  test('Game links use the catalog route and validate the host and query', () {
    expect(launcherGameLink(Uri.parse('/?game=th075')), '/game/th075');
    expect(
      launcherGameLink(Uri.parse('https://touhouonline.com/?game=th07')),
      '/game/th07',
    );
    expect(
      launcherGameLink(Uri.parse('touhouonline://open/?game=th07')),
      '/game/th07',
    );
    expect(
      launcherGameLink(Uri.parse('/?game=th06&game=th07')),
      '/game/unavailable',
    );
    expect(
      launcherGameLink(Uri.parse('/?game=%2Faccount%2Fsecurity')),
      '/game/unavailable',
    );
    expect(
      launcherGameLink(Uri.parse('touhouonline://open/?game=%FF')),
      '/game/unavailable',
    );
    for (final address in [
      'http://touhouonline.com/?game=th07',
      'https://example.invalid/?game=th07',
      'https://touhouonline.com:8443/?game=th07',
      'https://someone@touhouonline.com/?game=th07',
      'touhouonline://other/?game=th07',
      'touhouonline://open/account/security?game=th07',
      '/game/th07/settings',
    ]) {
      expect(launcherGameLink(Uri.parse(address)), isNull, reason: address);
    }
  });

  final linkCapture = Platform.environment['TOUHOU_LINK_CAPTURE'];
  if (linkCapture != null) {
    setUpAll(() async {
      Directory(linkCapture).createSync(recursive: true);
      await loadFonts(Directory(Platform.environment['TOUHOU_CAPTURE_FONTS']!));
    });
  }

  for (final (device, size) in [
    ('mobile', const Size(320, 640)),
    ('desktop', const Size(1440, 734)),
  ]) {
    testWidgets('$device opens a cold game link and returns to the library', (
      tester,
    ) async {
      _size(tester, size);
      final compact = device == 'mobile';
      const nativeLink = 'touhouonline://open/?game=th07';
      const publishedLink = 'https://touhouonline.com/?game=th07';
      if (compact) {
        tester.binding.platformDispatcher.defaultRouteNameTestValue =
            nativeLink;
        addTearDown(
          tester.binding.platformDispatcher.clearDefaultRouteNameTestValue,
        );
      }
      final app = fixtures.fixture(
        games: games,
        accountApi: fixtures.FixtureAccountApi(),
        lobbyDirectory: IdleLobby(games),
        initialLocation: compact
            ? null
            : launcherGameLink(Uri.parse(publishedLink)),
      );
      final boundary = GlobalKey();
      await tester.pumpWidget(RepaintBoundary(key: boundary, child: app));
      await tester.pumpAndSettle();
      final detail = find.byType(GameDetail);
      expect(tester.widget<GameDetail>(detail).game.id, 'th07');
      final sidebarBounds = compact ? null : tester.getRect(_side(false));
      if (linkCapture != null) {
        final context = tester.element(detail);
        await tester.runAsync(
          () => precacheImage(
            const AssetImage('assets/book-covers/th07.webp'),
            context,
          ),
        );
        await tester.pumpAndSettle();
        await save(
          tester,
          boundary,
          Directory(linkCapture),
          '$device-game-link',
        );
      }
      final settings = compact ? find.byTooltip('游戏设置') : find.text('游戏设置');
      await _tap(tester, settings);
      expect(find.byType(GameSettingsPage), findsOneWidget);
      await tester.binding.handlePopRoute();
      await tester.pumpAndSettle();
      expect(tester.widget<GameDetail>(detail).game.id, 'th07');
      await _tap(tester, find.byType(BackButton));
      expect(find.byType(LibraryPage).hitTestable(), findsOneWidget);
      if (sidebarBounds != null) {
        expect(tester.getRect(_side(false)), sidebarBounds);
      }

      Future<void> deliver(String address) async {
        final channel = SystemChannels.navigation;
        await tester.binding.defaultBinaryMessenger.handlePlatformMessage(
          channel.name,
          channel.codec.encodeMethodCall(
            MethodCall('pushRouteInformation', {'location': address}),
          ),
          (_) {},
        );
        await tester.pumpAndSettle();
      }

      await _tap(tester, find.text('联机').hitTestable());
      expect(find.byType(LobbyPage).hitTestable(), findsOneWidget);
      await deliver('touhouonline://open/?game=th075');
      expect(tester.widget<GameDetail>(detail).game.id, 'th075');
      await deliver('touhouonline://open/?game=th89');
      expect(detail, findsNothing);
      expect(find.text('找不到该作品'), findsOneWidget);
      // Android re-encodes invalid octets before forwarding the route.
      await deliver('touhouonline://open/?game=%EF%BF%BD');
      expect(find.text('找不到该作品'), findsOneWidget);
      if (linkCapture != null) {
        await save(
          tester,
          boundary,
          Directory(linkCapture),
          '$device-game-link-unavailable',
        );
      }
      await tester.binding.handlePopRoute();
      await tester.pumpAndSettle();
      expect(find.byType(LibraryPage).hitTestable(), findsOneWidget);
      expect(tester.takeException(), isNull);
    });
  }

  for (final (name, appReduced, systemReduced) in [
    ('normal', false, false),
    ('app-reduced', true, false),
    ('system-reduced', false, true),
  ]) {
    testWidgets(
      'Mobile tab and game transitions keep viewports stable | $name',
      (tester) async {
        _size(tester, Size(appReduced ? 320 : 390, 720));
        final safeBottom = systemReduced ? 16.0 : 0.0;
        if (systemReduced) {
          tester.view.padding = FakeViewPadding(top: 24, bottom: safeBottom);
          addTearDown(tester.view.resetPadding);
        }
        if (systemReduced) {
          tester.platformDispatcher.accessibilityFeaturesTestValue =
              const FakeAccessibilityFeatures(disableAnimations: true);
          addTearDown(
            tester.platformDispatcher.clearAccessibilityFeaturesTestValue,
          );
        }
        final router = await _show(tester, games, lessMotion: appReduced);
        expect(find.byType(BookSidebar, skipOffstage: false), findsNothing);
        final library = find.byType(LibraryPage);
        final libraryBefore = tester.getRect(library);
        final navigation = find.byType(NavigationBar);
        final navBefore = tester.getRect(navigation);
        expect(navBefore.height, launcherBottomBarHeight + safeBottom);
        final trace = <Map<String, Object?>>[];
        void record(String action, int milliseconds) {
          final branches = _descend(
            find.byKey(const ValueKey('launcher branches')),
            find.byWidgetPredicate(
              (widget) =>
                  widget is FadeTransition &&
                  widget.child is Material &&
                  widget.opacity is AlwaysStoppedAnimation<double>,
            ),
          );
          final opacity = tester
              .widgetList<FadeTransition>(branches)
              .map((transition) => transition.opacity.value)
              .toList();
          if (!action.startsWith('open ')) {
            expect(opacity, contains(1.0));
            if (appReduced || systemReduced) expect(opacity, [1.0]);
          }
          trace.add({
            'branchOpacity': opacity,
            'timeMs': milliseconds,
            'action': action,
            'bottomNavigation': navigation.evaluate().isEmpty
                ? null
                : _box(tester.getRect(navigation)),
            'bookViewport': _box(
              tester.getRect(find.byType(LibraryPage, skipOffstage: false)),
            ),
          });
          expect(tester.takeException(), isNull);
        }

        final rootScroll = _scrollIn(tester, library);
        final scroll = find.descendant(
          of: find.byKey(const PageStorageKey('library category rail')),
          matching: find.byType(Scrollable),
        );
        final rail = tester.state<ScrollableState>(scroll).position;
        expect(rail.maxScrollExtent, greaterThan(0));
        rail.jumpTo(rail.maxScrollExtent * .6);
        final railOffset = rail.pixels;
        final cover = _descend(library, find.byKey(const ValueKey('th08')));
        await tester.scrollUntilVisible(
          cover,
          160,
          scrollable: _descend(library, find.byType(Scrollable)).first,
        );
        await tester.pumpAndSettle();
        final offset = rootScroll.pixels;
        expect(offset, greaterThan(0));

        for (final label in ['联机', '主页', '单机']) {
          await tester.tap(_branchLabel(label));
          await tester.pump();
          record(label, 0);
          for (final time in [
            16,
            32,
            48,
            64,
            80,
            96,
            112,
            128,
            144,
            160,
            176,
            192,
            208,
            224,
          ]) {
            await tester.pump(const Duration(milliseconds: 16));
            record(label, time);
            expect(tester.getRect(navigation), navBefore);
          }
          await tester.pumpAndSettle();
          expect(tester.getRect(navigation), navBefore);
          if (label != '单机') {
            expect(find.byType(LibrarySearch).hitTestable(), findsNothing);
          }
        }
        expect(
          _scrollIn(tester, find.byType(LibraryPage)).pixels,
          closeTo(offset, .5),
        );

        final artwork = _descend(cover, find.byType(GameArtwork));
        await tester.tap(artwork);
        await tester.pump();
        record('open th08', 0);
        await tester.pump(const Duration(milliseconds: 16));
        record('open th08', 16);
        final during = tester.getRect(
          find.byType(LibraryPage, skipOffstage: false),
        );
        expect(during.size, libraryBefore.size);
        expect(during.top, libraryBefore.top);
        if (appReduced || systemReduced) expect(during, libraryBefore);
        await tester.pumpAndSettle();
        expect(find.byType(GameDetail).hitTestable(), findsOneWidget);
        expect(navigation.hitTestable(), findsNothing);
        expect(find.byType(LibrarySearch).hitTestable(), findsNothing);

        await tester.binding.handlePopRoute();
        await tester.pumpAndSettle();
        expect(tester.getRect(navigation), navBefore);
        expect(_scrollIn(tester, library).pixels, closeTo(offset, .5));

        rootScroll.jumpTo(0);
        await tester.pumpAndSettle();
        final restoredRail = tester.state<ScrollableState>(scroll).position;
        expect(restoredRail.pixels, closeTo(railOffset, .5));

        router.go('/?game=th07');
        await tester.pumpAndSettle();
        final game = tester.widget<GameDetail>(find.byType(GameDetail));
        expect(game.game.id, 'th07');
        await _tap(tester, find.byType(BackButton));
        expect(library.hitTestable(), findsOneWidget);
        expect(tester.takeException(), isNull);
        await _trace(tester, 'mobile-$name', trace);
      },
    );
  }

  for (final (name, width, theme, background) in [
    ('dark', 390.0, ThemeMode.dark, BackgroundMode.plain),
    ('light', 390.0, ThemeMode.light, BackgroundMode.dots),
    ('desktop', 1100.0, ThemeMode.dark, BackgroundMode.plain),
  ]) {
    testWidgets(
      'Rapid tab changes continue from the last visible frame | $name',
      (tester) async {
        debugDefaultTargetPlatformOverride = TargetPlatform.windows;
        addTearDown(() => debugDefaultTargetPlatformOverride = null);
        _size(tester, Size(width, 720));
        final compact = width <= launcherCompactWidth;
        final boundary = GlobalKey();
        final capture = Platform.environment['TOUHOU_TAB_CAPTURE'];
        if (capture != null) {
          await tester.runAsync(
            () => loadFonts(
              Directory(Platform.environment['TOUHOU_CAPTURE_FONTS']!),
            ),
          );
        }
        await _show(
          tester,
          games,
          lessMotion: false,
          theme: theme,
          boundary: boundary,
          background: background,
        );
        final branch = find.byKey(const ValueKey('launcher branches'));
        final nav = compact
            ? find.byType(NavigationBar)
            : find.byType(DesktopMasthead);
        final bounds = tester.getRect(branch);
        final navBounds = tester.getRect(nav);
        // The page fills the window width. Sample its rows without the navigation bar.
        expect(
          bounds.width,
          compact
              ? width
              : width - bookSidebarWidth(tester.element(branch)) - 1,
        );
        final contentTop = bounds.top.round();
        final contentBottom = (compact ? navBounds.top : bounds.bottom).round();
        final library = find.byType(LibraryPage);
        final lobby = find.byType(LobbyPage);
        final librarySearch = tester
            .widget<EditableText>(
              _descend(find.byType(LibrarySearch), find.byType(EditableText)),
            )
            .focusNode;
        final libraryContext = tester.element(library);
        await tester.runAsync(() async {
          if (capture != null && !compact) {
            await precacheImage(
              const AssetImage('assets/launcher-wordmark-dark.png'),
              libraryContext,
            );
          }
          for (final game in games) {
            if (game.artwork case final String cover) {
              await precacheImage(
                AssetImage('assets/book-covers/$cover'),
                libraryContext,
              );
            }
          }
        });
        final rootScroll = _scrollIn(tester, library);
        rootScroll.jumpTo(110);
        await tester.pumpAndSettle();
        expect(rootScroll.pixels, 110);

        Future<Uint8List> pixels() async {
          final surface =
              boundary.currentContext!.findRenderObject()!
                  as RenderRepaintBoundary;
          return (await tester.runAsync(() async {
            final image = await surface.toImage();
            try {
              final bytes = (await image.toByteData(
                format: ImageByteFormat.rawRgba,
              ))!;
              return bytes.buffer.asUint8List(
                bytes.offsetInBytes + (contentTop * image.width * 4),
                (contentBottom - contentTop) * image.width * 4,
              );
            } finally {
              image.dispose();
            }
          }))!;
        }

        Future<void> select(String label) async {
          await tester.tap(_descend(nav, find.text(label)));
          await tester.pump();
          expect(tester.getRect(branch), bounds);
          expect(tester.getRect(nav), navBounds);
        }

        await select('联机');
        expect(librarySearch.canRequestFocus, isFalse);
        await tester.pump(const Duration(milliseconds: 64));
        final guide = find.semantics.byLabel('联机玩法介绍');
        expect(guide, findsOneWidget);
        final first = await pixels();
        if (capture != null) {
          await save(
            tester,
            boundary,
            Directory(capture),
            'rapid-$name-before',
          );
        }
        await select(compact ? '主页' : '单机');
        if (capture != null) {
          await save(tester, boundary, Directory(capture), 'rapid-$name-after');
        }
        expect(
          listEquals(await pixels(), first),
          isTrue,
          reason: 'A new selection starts on the displayed frame',
        );
        expect(lobby.hitTestable(), findsNothing);
        expect(guide, findsNothing);
        expect(library.hitTestable(), compact ? findsNothing : findsOneWidget);
        await tester.pump(const Duration(milliseconds: 32));
        final second = await pixels();
        expect(listEquals(second, first), isFalse);
        await select(compact ? '单机' : '联机');
        expect(
          listEquals(await pixels(), second),
          isTrue,
          reason: 'Another interruption keeps the existing blend',
        );
        await tester.pumpAndSettle();
        if (!compact) await _tap(tester, _descend(nav, find.text('单机')));
        expect(library.hitTestable(), findsOneWidget);
        expect(librarySearch.canRequestFocus, isTrue);
        expect(_scrollIn(tester, library).pixels, 110);
        expect(tester.getRect(nav), navBounds);
        expect(tester.takeException(), isNull);
      },
    );
  }

  for (final theme in [ThemeMode.dark, ThemeMode.light]) {
    testWidgets('Shared sidebar columns coincide | ${theme.name}', (
      tester,
    ) async {
      _size(tester, const Size(1440, 734));
      final boundary = GlobalKey();
      await _show(
        tester,
        games,
        lessMotion: false,
        theme: theme,
        boundary: boundary,
      );
      final game = games.singleWhere((game) => game.id == 'th06');
      final frames = <Map<String, Object?>>[];
      Map<String, Rect> coordinates(bool multiplayer) {
        final side = _side(multiplayer);
        final all = _descend(
          side,
          find.byKey(const ValueKey(BookCategory.all)),
        );
        final group = _descend(
          side,
          find.byKey(const ValueKey(BookCategory.windows)),
        );
        final work = _descend(
          side,
          find.byKey(const ValueKey((BookCategory.windows, 'th06'))),
        );
        final title = _descend(
          work,
          find.byWidgetPredicate(
            (widget) =>
                widget is Text &&
                (widget.data ?? widget.textSpan?.toPlainText() ?? '')
                    .startsWith(game.title),
          ),
        );
        final subtitle = _descend(work, find.text(game.subtitle));
        final count = find.byKey(const ValueKey('category count'));
        return {
          'side': tester.getRect(side),
          'search': tester.getRect(_descend(side, find.byType(SearchBar))),
          'all': tester.getRect(all),
          'allText': tester.getRect(
            _descend(all, find.text(multiplayer ? '全部联机' : '全部游戏')),
          ),
          'groupText': tester.getRect(
            _descend(group, find.text(multiplayer ? '原作' : '正作')),
          ),
          'allCount': tester.getRect(_descend(all, count)),
          'groupCount': tester.getRect(_descend(group, count)),
          'work': tester.getRect(work),
          'cover': tester.getRect(_descend(work, find.byType(GameArtwork))),
          'mark': multiplayer
              ? tester.getRect(_descend(work, find.byType(GameSelectionMarker)))
              : Rect.zero,
          'title': tester.getRect(title),
          'code': tester.getRect(_descend(work, find.text(game.displayCode))),
          'subtitle': tester.getRect(subtitle),
          'favorite': tester.getRect(
            _descend(work, find.byType(FavoriteButton)),
          ),
        };
      }

      Future<void> record(String route, Map<String, Rect> boxes) async {
        final favorite = boxes['favorite']!;
        final cover = boxes['cover']!;
        final title = boxes['title']!;
        final subtitle = boxes['subtitle']!;
        expect(boxes['side']!.width, 296);
        expect(boxes['search']!.left, boxes['all']!.left);
        expect(boxes['search']!.right, boxes['all']!.right);
        expect(boxes['all']!.height, 38);
        expect(boxes['allText']!.left, boxes['groupText']!.left);
        expect(boxes['allCount']!.right, boxes['groupCount']!.right);
        expect(title.left - cover.right, 10);
        expect(subtitle.left, title.left);
        expect(title.right, lessThanOrEqualTo(boxes['code']!.left));
        expect(boxes['code']!.right, lessThanOrEqualTo(favorite.left));
        expect(subtitle.right, greaterThan(favorite.left));
        expect(favorite.top, boxes['work']!.top);
        final mark = boxes['mark']!;
        if (!mark.isEmpty) {
          expect(mark.center.dx, closeTo(cover.left + 5, 1));
          expect(mark.center.dy, closeTo(cover.top + 5, 3));
        }
        frames.add({
          'route': route,
          for (final MapEntry(key: name, value: rect) in boxes.entries)
            name: _box(rect),
        });
        if (sidebarCapture != null) {
          await save(
            tester,
            boundary,
            Directory(sidebarCapture),
            'desktop-side-$route-${theme.name}',
          );
        }
      }

      final single = coordinates(false);
      await record('library', single);
      await _tap(tester, find.text('联机'));
      final multi = coordinates(true);
      await record('lobby', multi);
      for (final name in [
        'side',
        'search',
        'all',
        'allText',
        'groupText',
        'allCount',
        'groupCount',
        'work',
        'cover',
        'title',
        'subtitle',
        'favorite',
      ]) {
        expect(multi[name]!.left, single[name]!.left, reason: name);
        if (!name.endsWith('Count') && !name.endsWith('Text')) {
          expect(multi[name]!.width, single[name]!.width, reason: name);
        }
      }
      final row = _descend(
        _side(true),
        find.byKey(const ValueKey((BookCategory.windows, 'th06'))),
      );
      await _tap(tester, _descend(row, find.byType(FavoriteButton)));
      expect(tester.widget<GameRow>(row).book.isFavorite(game), isTrue);
      expect(tester.widget<GameRow>(row).selected, isFalse);
      await _tap(tester, _descend(row, find.byType(GameSelectionMarker)));
      expect(tester.widget<GameRow>(row).selected, isTrue);
      await _tap(tester, find.text('单机'));
      final singleBook = _descend(
        _side(false),
        find.byKey(const ValueKey((BookCategory.windows, 'th06'))),
      );
      expect(tester.widget<GameRow>(singleBook).book.isFavorite(game), isTrue);
      expect(tester.takeException(), isNull);
      await _trace(tester, 'desktop-sidebar-alignment-${theme.name}', frames);
    });
  }

  testWidgets(
    'Crossing the navigation breakpoint retains the open game and library scroll',
    (tester) async {
      _size(tester, const Size(781, 550));
      final boundary = GlobalKey();
      final capture = Platform.environment['TOUHOU_RESIZE_CAPTURE'];
      if (capture != null) {
        await tester.runAsync(
          () => loadFonts(
            Directory(Platform.environment['TOUHOU_CAPTURE_FONTS']!),
          ),
        );
      }
      await _show(tester, games, lessMotion: false, boundary: boundary);
      final library = find.byType(LibraryPage);
      final libraryElement = tester.element(library);
      if (capture != null) {
        await tester.runAsync(() async {
          await precacheImage(
            const AssetImage('assets/launcher-wordmark-dark.png'),
            libraryElement,
          );
          for (final game in games) {
            if (game.artwork case final String cover) {
              await precacheImage(
                AssetImage('assets/book-covers/$cover'),
                libraryElement,
              );
            }
          }
        });
        await tester.pumpAndSettle();
      }
      final viewport = find.byType(LibraryPage, skipOffstage: false);
      final list = find
          .descendant(
            of: viewport,
            matching: find.byType(Scrollable, skipOffstage: false),
            skipOffstage: false,
          )
          .first;
      ScrollPosition position() => tester.state<ScrollableState>(list).position;
      final search = find.descendant(
        of: find.byType(LibrarySearch),
        matching: find.byType(EditableText),
      );
      await tester.enterText(search, 'th0');
      await tester.pumpAndSettle();
      position().jumpTo(140);
      await tester.pumpAndSettle();
      expect(position().pixels, 140);

      final frames = <Map<String, Object?>>[];
      Future<void> resize(double width, String step) async {
        final offset = position().pixels;
        tester.view.physicalSize = Size(width, 550);
        await tester.pumpAndSettle();
        expect(tester.takeException(), isNull);
        expect(tester.element(viewport), same(libraryElement));
        final page = tester.widget<LibraryPage>(viewport);
        expect(page.book.searchController.text, 'th0');
        expect(position().pixels, offset);
        final detail = tester
            .widgetList<GameDetail>(find.byType(GameDetail))
            .singleOrNull;
        frames.add({
          'step': step,
          'game': detail?.game.id,
          'drawerVisible': find
              .byType(SiteDrawer)
              .hitTestable()
              .evaluate()
              .isNotEmpty,
          'width': width,
          'scrollOffset': position().pixels,
        });
      }

      await resize(780, 'book to compact');
      expect(find.byType(NavigationBar).hitTestable(), findsOneWidget);
      final cover = _descend(library, find.byKey(const ValueKey('th06')));
      await _tap(tester, _descend(cover, find.byType(GameArtwork)));
      expect(
        tester.widget<GameDetail>(find.byType(GameDetail)).game.id,
        'th06',
      );
      await resize(781, 'detail to wide');
      expect(
        tester.widget<GameDetail>(find.byType(GameDetail)).game.id,
        'th06',
      );
      expect(_side(false).hitTestable(), findsOneWidget);
      expect(find.byType(NavigationBar), findsNothing);
      if (capture != null) {
        await save(tester, boundary, Directory(capture), 'resize-detail-wide');
      }
      await resize(780, 'detail to compact');
      expect(
        tester.widget<GameDetail>(find.byType(GameDetail)).game.id,
        'th06',
      );
      expect(find.byType(NavigationBar).hitTestable(), findsNothing);
      if (capture != null) {
        await save(
          tester,
          boundary,
          Directory(capture),
          'resize-detail-compact',
        );
      }
      await _tap(tester, find.byType(BackButton));
      expect(find.byType(NavigationBar).hitTestable(), findsOneWidget);
      await resize(781, 'returned book to wide');
      expect(library.hitTestable(), findsOneWidget);
      expect(_side(false).hitTestable(), findsOneWidget);
      await resize(780, 'book back to compact');
      await _tap(tester, find.byType(DrawerButton));
      expect(find.byType(SiteDrawer).hitTestable(), findsOneWidget);
      await resize(781, 'menu to wide');
      expect(find.byType(SiteDrawer).hitTestable(), findsNothing);
      await resize(780, 'menu back to compact');
      expect(find.byType(SiteDrawer).hitTestable(), findsNothing);
      expect(find.byType(NavigationBar).hitTestable(), findsOneWidget);
      await _trace(tester, 'navigation-breakpoint', frames);
    },
  );

  testWidgets('Desktop hover animates the star inside the same row', (
    tester,
  ) async {
    _size(tester, const Size(1440, 734));
    await _show(tester, games, lessMotion: false);
    final row = _descend(
      _side(false),
      find.byKey(const ValueKey((BookCategory.windows, 'th06'))),
    );
    final favorite = _descend(row, find.byType(FavoriteButton));
    final star = _descend(favorite, find.byIcon(Icons.star_outline_rounded));
    final rowBounds = tester.getRect(row);
    final buttonBounds = tester.getRect(favorite);
    final baseline = tester.getRect(star);
    final mouse = await tester.createGesture(kind: PointerDeviceKind.mouse);
    await mouse.addPointer(location: const Offset(550, 55));
    await mouse.moveTo(rowBounds.center);
    await tester.pump();
    await tester.pump(const Duration(milliseconds: 64));
    final during = tester.getRect(star);
    expect(during.width, greaterThan(baseline.width));
    expect(during.center, baseline.center);
    await tester.pumpAndSettle();
    expect(tester.getRect(star).width, greaterThan(during.width));
    expect(tester.getRect(favorite), buttonBounds);
    expect(tester.getRect(row), rowBounds);
    await mouse.moveTo(const Offset(550, 55));
    await tester.pumpAndSettle();
    expect(tester.getRect(star), baseline);
    await mouse.removePointer();
  });

  testWidgets('Desktop masthead aligns across widths and routes', (
    tester,
  ) async {
    _size(tester, const Size(1440, 734));
    final boundary = GlobalKey();
    final router = await _show(
      tester,
      games,
      lessMotion: false,
      boundary: boundary,
    );
    final site = find.byKey(const ValueKey('masthead site'));
    final navigation = find.byKey(const ValueKey('masthead navigation'));
    final account = find.byKey(const ValueKey('masthead account'));
    final frames = <Map<String, Object?>>[];
    final capture = Platform.environment['TOUHOU_MASTHEAD_CAPTURE'];
    if (capture != null) {
      final context = tester.element(find.byType(LibraryPage));
      await tester.runAsync(() async {
        for (final asset in [
          'assets/launcher-wordmark-dark.png',
          'assets/launcher-wordmark-light.png',
          for (final game in games)
            if (game.artwork case final String cover)
              'assets/book-covers/$cover',
        ]) {
          await precacheImage(AssetImage(asset), context);
        }
      });
      await tester.pumpAndSettle();
    }
    Future<void> record(String state) async {
      final width = tester.view.physicalSize.width;
      final navigationBounds = tester.getRect(navigation);
      final siteBounds = tester.getRect(site);
      final accountBounds = tester.getRect(account);
      expect(navigationBounds.center.dx, closeTo(width / 2, .01));
      expect(navigationBounds.top, 6);
      expect(navigationBounds.height, 32);
      expect(siteBounds.left, width >= 900 ? 28 : 16);
      expect(siteBounds.right, closeTo(navigationBounds.left - 12, .01));
      expect(accountBounds.left, closeTo(navigationBounds.right + 12, .01));
      expect(accountBounds.right, width - siteBounds.left);
      frames.add({
        'state': state,
        'site': _box(siteBounds),
        'navigation': _box(navigationBounds),
        'account': _box(accountBounds),
      });
      if (capture != null &&
          state != 'desktop-library' &&
          state != 'desktop-lobby-live') {
        await save(tester, boundary, Directory(capture), state);
      }
    }

    expect(tester.getTopLeft(_side(false)).dy, 44);
    await record('desktop-library');
    final tabs = tester.getRect(navigation);
    await _tap(tester, find.text('联机'));
    expect(tester.getRect(navigation), tabs);
    expect(tester.getTopLeft(_side(true)).dy, 44);
    await record('desktop-lobby-live');
    await _tap(tester, find.byTooltip('站点菜单'));
    final settingsItem = find.widgetWithText(
      PopupMenuItem<VoidCallback>,
      '启动器设置',
    );
    expect(tester.getSize(settingsItem).height, 36);
    expect(tester.getSize(settingsItem).width, 220);
    expect(tester.getTopLeft(settingsItem).dy, greaterThanOrEqualTo(44));
    await record('desktop-site-menu');
    await _tap(tester, find.text('启动器设置').hitTestable());
    expect(find.text('启动器设置').hitTestable(), findsOneWidget);
    expect(
      tester.getTopLeft(_descend(navigation, find.text('单机'))).dy,
      greaterThan(6),
    );
    await _tap(
      tester,
      find.descendant(of: site, matching: find.byType(Image)).first,
    );
    expect(router.routeInformationProvider.value.uri.path, '/');

    tester.view.physicalSize = const Size(781, 640);
    await tester.pumpAndSettle();
    await fixtures.signInFixture(tester);
    expect(_descend(account, find.text('测试玩家')), findsOneWidget);
    await _tap(tester, find.byTooltip('主题设置'));
    await _tap(tester, find.text('亮色').hitTestable());
    await _tap(tester, find.byTooltip('关闭'));
    await record('desktop-narrow-account-light');
    await _tap(tester, find.byTooltip('账户'));
    expect(find.text('个人主页').hitTestable(), findsWidgets);
    await tester.binding.handlePopRoute();
    await tester.pumpAndSettle();
    await _tap(tester, find.byTooltip('私信'));
    await _tap(tester, find.text('查看全部'));
    expect(find.text('消息中心'), findsOneWidget);
    await tester.binding.handlePopRoute();
    await tester.pumpAndSettle();
    await record('desktop-narrow-inbox-return');

    final preferences = tester
        .widget<LauncherApp>(find.byType(LauncherApp))
        .preferences;
    for (final (language, label) in [
      (LauncherLanguage.english, 'Single-player'),
      (LauncherLanguage.japanese, 'シングルプレイ'),
    ]) {
      await preferences.setLanguage(language);
      await tester.pumpAndSettle();
      expect(
        _descend(navigation, find.text(label)).hitTestable(),
        findsOneWidget,
      );
      await record('desktop-narrow-${language.tag}');
    }
    expect(tester.takeException(), isNull);
    await _trace(tester, 'desktop-masthead', frames);
  });

  testWidgets(
    'Desktop sidebars stay in their viewport and outside page scrolling',
    (tester) async {
      _size(tester, const Size(1100, 550));
      debugDefaultTargetPlatformOverride = TargetPlatform.windows;
      addTearDown(() => debugDefaultTargetPlatformOverride = null);
      final boundary = GlobalKey();
      final capture = Platform.environment['TOUHOU_MOTION_CAPTURE'];
      if (capture != null) {
        await tester.runAsync(
          () => loadFonts(
            Directory(Platform.environment['TOUHOU_CAPTURE_FONTS']!),
          ),
        );
      }
      await _show(tester, games, lessMotion: false, boundary: boundary);
      if (capture != null) {
        final context = tester.element(find.byType(LibraryPage));
        await tester.runAsync(() async {
          await precacheImage(
            const AssetImage('assets/launcher-wordmark-dark.png'),
            context,
          );
          for (final game in games) {
            if (game.artwork case final String cover) {
              await precacheImage(
                AssetImage('assets/book-covers/$cover'),
                context,
              );
            }
          }
        });
        await tester.pumpAndSettle();
      }
      final single = _side(false);
      final sidebarBounds = tester.getRect(single);
      final sidebar = _scrollIn(tester, single);
      final book = find.byType(LibraryPage);
      final content = _scrollIn(tester, book);
      sidebar.jumpTo(140.clamp(0, sidebar.maxScrollExtent).toDouble());
      final sidebarOffset = sidebar.pixels;
      content.jumpTo(content.maxScrollExtent * .6);
      final contentOffset = content.pixels;
      expect(sidebarOffset, greaterThan(0));
      expect(contentOffset, greaterThan(0));
      await tester.pumpAndSettle();
      expect(sidebar.pixels, sidebarOffset);

      final frames = <Map<String, Object?>>[];
      Future<void> sample(String action, Finder target) async {
        await tester.tap(target);
        await tester.pump();
        for (var i = 0; i < 12; i++) {
          final sidebar = tester.getRect(_side(false, offstage: true));
          frames.add({
            'timeMs': i * 16,
            'action': action,
            'sidebar': _box(sidebar),
          });
          expect(sidebar, sidebarBounds);
          expect(tester.takeException(), isNull);
          if (capture != null &&
              i == 5 &&
              (action == 'open game' ||
                  action == 'open settings' ||
                  action == 'back to library')) {
            await save(
              tester,
              boundary,
              Directory(capture),
              action.replaceAll(' ', '-'),
            );
          }
          await tester.pump(const Duration(milliseconds: 16));
        }
        await tester.pumpAndSettle();
      }

      await sample('lobby', find.text('联机'));
      expect(_side(true).hitTestable(), findsOneWidget);
      expect(tester.getRect(_side(true)), sidebarBounds);
      final webLink = await fixtures.visitAccountWeb(tester, () {
        openSiteLink(tester.element(find.byType(LobbyPage)), '/u/fixture');
      });
      expect(webLink.path, '/u/fixture');
      expect(find.byType(LobbyPage).hitTestable(), findsOneWidget);
      expect(tester.getRect(_side(true)), sidebarBounds);

      await sample('book', find.text('单机'));
      expect(_scrollIn(tester, book).pixels, closeTo(contentOffset, .5));
      expect(sidebar.pixels, closeTo(sidebarOffset, .5));

      final firstRow = _descend(
        single,
        find.byKey(const ValueKey((BookCategory.windows, 'th06'))),
      );
      final secondRow = _descend(
        single,
        find.byKey(const ValueKey((BookCategory.windows, 'th07'))),
      );
      for (final row in [firstRow, secondRow]) {
        await tester.ensureVisible(row);
        await tester.pumpAndSettle();
        final cover = _descend(row, find.byType(GameArtwork));
        if (row == firstRow) {
          await sample('open game', cover);
        } else {
          await _tap(tester, cover);
        }
        expect(tester.getRect(single), sidebarBounds);
      }
      expect(
        tester.widget<GameDetail>(find.byType(GameDetail)).game.id,
        'th07',
      );
      final selected = tester.widget<GameRow>(secondRow);
      expect(selected.selected, isTrue);
      sidebar.jumpTo(0);
      await tester.pumpAndSettle();
      expect(tester.widget<GameRow>(firstRow).selected, isFalse);
      final settings = find.text('游戏设置');
      await sample('open settings', settings);
      final settingsPage = find.byType(GameSettingsPage);
      expect(settingsPage, findsOneWidget);
      await tester.binding.handlePopRoute();
      await tester.pumpAndSettle();
      expect(
        tester.widget<GameDetail>(find.byType(GameDetail)).game.id,
        'th07',
      );
      await sample('back to library', find.byType(BackButton));
      expect(book.hitTestable(), findsOneWidget);
      expect(_scrollIn(tester, book).pixels, closeTo(contentOffset, .5));
      expect(tester.getRect(single), sidebarBounds);
      await _tap(tester, find.byTooltip('站点菜单'));
      await sample('home settings', find.text('启动器设置').hitTestable());
      expect(find.text('启动器设置').hitTestable(), findsOneWidget);
      expect(single.hitTestable(), findsNothing);
      await sample('book from settings', find.text('单机'));
      expect(_scrollIn(tester, book).pixels, closeTo(contentOffset, .5));
      await _trace(tester, 'desktop-normal', frames);
    },
  );

  for (final width in [781.0, 1440.0]) {
    testWidgets(
      'Desktop detail exit stays visible through the transition | $width',
      (tester) async {
        _size(tester, Size(width, 734));
        debugDefaultTargetPlatformOverride = TargetPlatform.windows;
        addTearDown(() => debugDefaultTargetPlatformOverride = null);
        final capture = Platform.environment['TOUHOU_DETAIL_FADE_CAPTURE'];
        final boundary = GlobalKey();
        if (capture != null) await tester.runAsync(() => loadFonts());
        await _show(tester, games, lessMotion: false, boundary: boundary);
        final library = find.byType(LibraryPage);
        final sidebar = tester.getRect(_side(false));
        final artwork = _descend(
          _descend(library, find.byKey(const ValueKey('th07'))),
          find.byType(GameArtwork),
        );
        await tester.ensureVisible(artwork);
        await tester.pumpAndSettle();
        final scroll = _scrollIn(tester, library).pixels;
        if (capture != null) {
          final context = tester.element(library);
          await tester.runAsync(() async {
            await precacheImage(
              const AssetImage('assets/launcher-wordmark-dark.png'),
              context,
            );
            for (final game in games) {
              if (game.artwork case final String cover) {
                await precacheImage(
                  AssetImage('assets/book-covers/$cover'),
                  context,
                );
              }
            }
          });
          await tester.pumpAndSettle();
        }
        final detail = find.byType(GameDetail, skipOffstage: false);
        double opacityOf(Finder finder) {
          if (finder.evaluate().isEmpty) return 0;
          var result = 1.0;
          tester.element(finder).visitAncestorElements((element) {
            final widget = element.widget;
            if (widget is FadeTransition) result *= widget.opacity.value;
            if (widget is Offstage && widget.offstage) result = 0;
            return true;
          });
          return result;
        }

        final frames = <Map<String, Object?>>[];
        Future<void> sample(String action, Finder trigger) async {
          await tester.tap(trigger);
          await tester.pump();
          for (var time = 0; time <= 224; time += 16) {
            if (time > 0) await tester.pump(const Duration(milliseconds: 16));
            final opacity = opacityOf(detail);
            final bookOpacity = opacityOf(
              find.byType(LibraryPage, skipOffstage: false),
            );
            final box = tester.getRect(_side(false, offstage: true));
            expect(box, sidebar);
            expect(tester.takeException(), isNull);
            frames.add({
              'action': action,
              'timeMs': time,
              'opacity': opacity,
              'bookOpacity': bookOpacity,
              'coverage': opacity + bookOpacity * (1 - opacity),
              'sidebar': _box(box),
            });
            if (capture != null &&
                const [0, 32, 64, 80, 112, 160, 208].contains(time)) {
              await save(
                tester,
                boundary,
                Directory(capture),
                '${width.toInt()}-$action-$time',
              );
            }
          }
          await tester.pumpAndSettle();
        }

        await sample('enter', artwork);
        expect(detail.hitTestable(), findsOneWidget);
        await sample('leave', find.byType(BackButton));
        expect(library.hitTestable(), findsOneWidget);
        expect(_scrollIn(tester, library).pixels, closeTo(scroll, .5));
        if (capture != null) {
          File('$capture/${width.toInt()}-frames.json').writeAsStringSync(
            const JsonEncoder.withIndent('  ').convert(frames),
          );
        }
        final middle = frames.singleWhere(
          (frame) => frame['action'] == 'leave' && frame['timeMs'] == 80,
        );
        expect(
          middle['opacity'] as double,
          greaterThan(.2),
          reason: 'The departing page must remain legible during the middle of a routine return',
        );
        expect(
          frames
              .map((frame) => frame['coverage'] as double)
              .reduce((a, b) => a < b ? a : b),
          greaterThan(.9),
          reason: 'A page must cover the content viewport throughout a plain-background transition',
        );
      },
    );
  }

  for (final width in [390.0, 834.0]) {
    for (final mode in ['normal', 'app-reduced', 'system-reduced']) {
      for (final background in [BackgroundMode.plain, BackgroundMode.dots]) {
        testWidgets(
          'iOS edge-back preserves routes and scroll | $width $mode ${background.name}',
          (tester) async {
            _size(tester, Size(width, 844));
            if (mode == 'system-reduced') {
              tester.platformDispatcher.accessibilityFeaturesTestValue =
                  const FakeAccessibilityFeatures(disableAnimations: true);
              addTearDown(
                tester.platformDispatcher.clearAccessibilityFeaturesTestValue,
              );
            }
            final boundary = GlobalKey();
            final router = await _show(
              tester,
              games,
              lessMotion: mode == 'app-reduced',
              boundary: boundary,
              background: background,
            );
            if (iosCapture != null) await _warmBookImages(tester, games);
            final library = find.byType(LibraryPage);
            final navigation = width <= launcherCompactWidth
                ? find.byType(NavigationBar)
                : find.byType(DesktopMasthead);
            final navigationBounds = tester.getRect(navigation);
            final scroll = _scrollIn(tester, library);
            scroll.jumpTo(140);
            await tester.pumpAndSettle();
            final cover = _descend(library, find.byKey(const ValueKey('th08')));
            await tester.ensureVisible(cover);
            await tester.pumpAndSettle();
            final offset = scroll.pixels;
            expect(offset, greaterThan(0));
            await _tap(tester, _descend(cover, find.byType(GameArtwork)));

            final captureName =
                'ios-${width.toInt()}-$mode${background == BackgroundMode.plain ? '' : '-${background.name}'}';
            final surface =
                boundary.currentContext!.findRenderObject()!
                    as RenderRepaintBoundary;
            final frames = <Map<String, Object?>>[];
            var sequence = 0;
            Future<void> swipe(Finder page, {required bool cancel}) async {
              final route =
                  ModalRoute.of(tester.element(page))! as PageRoute<dynamic>;
              expect(route.popGestureEnabled, isTrue);
              final navigator = route.navigator!;
              final bounds = tester.getRect(page);
              final pageType = page
                  .evaluate()
                  .single
                  .widget
                  .runtimeType
                  .toString();
              final started = tester.binding.clock.now();
              final animation = route.animation!;
              final samples = [
                for (var y = 280.0; y < bounds.height - 20; y += 16)
                  for (final x in [12.0, 20.0, 28.0])
                    bounds.topLeft + Offset(x, y),
              ];
              final solidPixel =
                  mode == 'normal' &&
                      background == BackgroundMode.plain &&
                      page.evaluate().single.widget is GameDetail
                  ? await _pixels(tester, surface, samples)
                  : null;
              final name =
                  '$captureName-${sequence++}-${cancel ? 'cancel' : 'commit'}';
              Future<void> record(String phase) async {
                if (iosCapture == null) return;
                final file = '$name-$phase';
                frames.add({
                  'file': '$file.png',
                  'sequence': name,
                  'phase': phase,
                  'ms': tester.binding.clock
                      .now()
                      .difference(started)
                      .inMilliseconds,
                  'page': pageType,
                  'route': router.state.uri.toString(),
                  'background': background.name,
                  'progress': animation.value,
                  'gesture': navigator.userGestureInProgress,
                  'pageBounds': page.evaluate().isEmpty
                      ? null
                      : _box(tester.getRect(page)),
                  'navigation': navigation.evaluate().isEmpty
                      ? null
                      : _box(tester.getRect(navigation)),
                  'sidebar': _side(false).evaluate().isEmpty
                      ? null
                      : _box(tester.getRect(_side(false))),
                  'libraryScroll': scroll.pixels,
                });
                await save(tester, boundary, Directory(iosCapture), file);
                await _trace(tester, captureName, frames);
              }

              final sidebar =
                  width > launcherCompactWidth &&
                      (page.evaluate().single.widget is GameDetail ||
                          page.evaluate().single.widget is GameSettingsPage)
                  ? tester.getRect(_side(false))
                  : null;
              await record('before');
              final gesture = await tester.startGesture(
                Offset(bounds.left + 2, bounds.top + 150),
              );
              await gesture.moveBy(const Offset(20, 0));
              await tester.pump(const Duration(milliseconds: 16));
              await record('first');
              await gesture.moveBy(const Offset(90, 0));
              await tester.pump(const Duration(milliseconds: 16));
              await record('middle');
              expect(
                navigator.userGestureInProgress,
                isTrue,
                reason: 'Reducing visual motion must preserve the system back gesture',
              );
              expect(route.animation!.value, lessThan(1));
              if (sidebar != null) {
                expect(tester.getRect(_side(false)), sidebar);
              }
              if (mode == 'normal') {
                expect(tester.getRect(page).left, greaterThan(bounds.left));
                if (solidPixel != null) {
                  final shift = tester.getRect(page).left - bounds.left;
                  final moved = await _pixels(
                    tester,
                    surface,
                    samples.map((point) => point + Offset(shift, 0)),
                  );
                  final delta = [
                    for (var i = 0; i < moved.length; i++)
                      (moved[i] - solidPixel[i]).abs(),
                  ].reduce((a, b) => a > b ? a : b);
                  expect(
                    delta,
                    lessThanOrEqualTo(2),
                    reason: 'A moving page must not expose the bookshelf through its gaps',
                  );
                }
                if (background != BackgroundMode.plain &&
                    page.evaluate().single.widget is GameDetail) {
                  final wallpaper = find
                      .byType(LauncherWallpaper)
                      .evaluate()
                      .singleWhere(
                        (element) => element.widget.key is GlobalKey,
                      );
                  final wallpaperBoundary = tester
                      .renderObject<RenderRepaintBoundary>(
                        find
                            .descendant(
                              of: find.byWidget(wallpaper.widget),
                              matching: find.byType(RepaintBoundary),
                            )
                            .first,
                      );
                  final point = tester.getTopLeft(page) + const Offset(12, 600);
                  final expected = await _pixels(tester, wallpaperBoundary, [
                    point,
                  ]);
                  expect(
                    await _pixels(tester, surface, [point]),
                    expected,
                    reason: 'A moving page must show the wallpaper at its window coordinates',
                  );
                }
              } else {
                expect(tester.getRect(page), bounds);
              }
              if (cancel) {
                await gesture.moveBy(const Offset(-75, 0));
              } else {
                await gesture.moveBy(Offset(bounds.width * .8, 0));
              }
              await tester.pump(const Duration(milliseconds: 200));
              await record('release');
              if (width <= launcherCompactWidth && pageType == 'GameDetail') {
                final bar = tester.getRect(navigation);
                expect(bar.top, navigationBounds.top);
                expect(bar.size, navigationBounds.size);
              }
              await gesture.up();
              await tester.pump(const Duration(milliseconds: 16));
              await record('settling');
              await tester.pumpAndSettle();
              await record('settled');
              expect(navigator.userGestureInProgress, isFalse);
              if (cancel) {
                expect(route.isCurrent, isTrue);
                expect(route.animation!.value, 1);
                expect(tester.getRect(page), bounds);
              }
              expect(tester.takeException(), isNull);
            }

            final detail = find.byType(GameDetail);
            await swipe(detail, cancel: true);
            if (background != BackgroundMode.plain) {
              final detailState = tester.state(detail);
              final resized = Size(width + 18, 844);
              tester.view.physicalSize = resized;
              await tester.pumpAndSettle();
              for (final wallpaper
                  in find.byType(LauncherWallpaper).evaluate()) {
                expect(
                  (wallpaper.findRenderObject()! as RenderBox).size,
                  resized,
                );
              }
              expect(tester.state(detail), same(detailState));
              tester.view.physicalSize = Size(width, 844);
              await tester.pumpAndSettle();
              final wallpaper = LauncherBackdrop.of(tester.element(detail))!;
              await wallpaper.select(BackgroundMode.grid);
              await tester.pumpAndSettle();
              expect(tester.state(detail), same(detailState));
              await wallpaper.select(BackgroundMode.dots);
              await tester.pumpAndSettle();
              expect(tester.state(detail), same(detailState));
            }
            final settingsControl = find.byTooltip('游戏设置').evaluate().isNotEmpty
                ? find.byTooltip('游戏设置')
                : find.text('游戏设置');
            await tester.ensureVisible(settingsControl);
            await tester.pumpAndSettle();
            await _tap(tester, settingsControl);
            final settings = find.byType(GameSettingsPage);
            await swipe(settings, cancel: true);
            await swipe(settings, cancel: false);
            expect(detail.hitTestable(), findsOneWidget);
            await swipe(detail, cancel: false);
            expect(library.hitTestable(), findsOneWidget);
            expect(scroll.pixels, closeTo(offset, .5));
            expect(tester.getRect(navigation), navigationBounds);
            await _tap(tester, _descend(navigation, find.text('联机')));
            expect(find.byType(LobbyPage).hitTestable(), findsOneWidget);
            expect(tester.getRect(navigation), navigationBounds);
          },
          variant: TargetPlatformVariant.only(TargetPlatform.iOS),
        );
      }
    }
  }

  for (final multiplayer in [false, true]) {
    for (final mode in ['normal', 'app-reduced', 'system-reduced']) {
      testWidgets('Sidebar fold preserves continuity | $multiplayer $mode', (
        tester,
      ) async {
        _size(tester, const Size(1440, 1000));
        if (mode == 'system-reduced') {
          tester.platformDispatcher.accessibilityFeaturesTestValue =
              const FakeAccessibilityFeatures(disableAnimations: true);
          addTearDown(
            tester.platformDispatcher.clearAccessibilityFeaturesTestValue,
          );
        }
        final boundary = GlobalKey();
        final router = await _show(
          tester,
          games,
          lessMotion: mode == 'app-reduced',
          boundary: boundary,
        );
        if (multiplayer) {
          router.go('/lobby');
          await tester.pumpAndSettle();
        }
        if (unfoldCapture != null) await _warmBookImages(tester, games);
        final side = _side(multiplayer);
        final order = multiplayer
            ? BookCategory.multiplayerGroups.toList()
            : BookCategory.values;
        final next = order[order.indexOf(BookCategory.windows) + 1];
        Finder group(BookCategory value) =>
            _descend(side, find.byKey(ValueKey(value)));
        final toggle = _descend(
          group(BookCategory.windows),
          find.byType(IconButton),
        );
        final headerBefore = tester.getRect(group(BookCategory.windows));
        final nextBefore = tester.getTopLeft(group(next)).dy;
        final sideBefore = tester.getRect(side);
        final scrollBefore = _scrollIn(tester, side).pixels;
        String? contentDigest;
        final frames = <Map<String, Object?>>[];
        Future<double> record(String phase, int milliseconds) async {
          final y = tester.getTopLeft(group(next)).dy;
          expect(tester.getRect(side), sideBefore);
          expect(tester.getRect(group(BookCategory.windows)), headerBefore);
          expect(_scrollIn(tester, side).pixels, scrollBefore);
          frames.add({
            'phase': phase,
            'ms': milliseconds,
            'nextY': y,
            'sidebar': [
              sideBefore.left,
              sideBefore.top,
              sideBefore.width,
              sideBefore.height,
            ],
            'headerTop': headerBefore.top,
            'scrollY': _scrollIn(tester, side).pixels,
          });
          if (unfoldCapture != null) {
            await save(
              tester,
              boundary,
              Directory(unfoldCapture),
              '${multiplayer ? 'lobby' : 'library'}-$mode-$phase-$milliseconds',
            );
            final digest = await tester.runAsync(() async {
              final render =
                  boundary.currentContext!.findRenderObject()!
                      as RenderRepaintBoundary;
              final image = await render.toImage();
              try {
                final data = (await image.toByteData(
                  format: ImageByteFormat.rawRgba,
                ))!;
                final pixels = data.buffer.asUint8List(
                  data.offsetInBytes,
                  data.lengthInBytes,
                );
                final content = BytesBuilder(copy: false);
                for (
                  var row = sideBefore.top.ceil();
                  row < image.height;
                  row++
                ) {
                  content.add(
                    Uint8List.sublistView(
                      pixels,
                      (row * image.width + sideBefore.right.ceil() + 2) * 4,
                      (row + 1) * image.width * 4,
                    ),
                  );
                }
                return sha256.convert(content.takeBytes()).toString();
              } finally {
                image.dispose();
              }
            });
            contentDigest ??= digest;
            expect(
              digest,
              contentDigest,
              reason: 'The right content keeps the same painted frame',
            );
            frames.last['contentSha256'] = digest;
          }
          return y;
        }

        await record('before', 0);
        await tester.tap(toggle);
        await tester.pump();
        final closeStart = await record('close', 0);
        for (final milliseconds in [32, 64, 96, 160, 240, 400]) {
          final previous = (frames.last['ms'] as int);
          await tester.pump(Duration(milliseconds: milliseconds - previous));
          await record('close', milliseconds);
        }
        await tester.pumpAndSettle();
        final closed = tester.getTopLeft(group(next)).dy;
        expect(closed, lessThan(nextBefore - 100));
        expect(tester.takeException(), isNull);
        await _trace(tester, 'fold-$multiplayer-$mode', frames);
        if (mode == 'normal') {
          expect(
            closeStart,
            closeTo(nextBefore, .01),
            reason: 'A collapse starts at the currently visible layout',
          );
          expect(
            frames.where((frame) => frame['phase'] == 'close').any((frame) {
              final y = frame['nextY'] as double;
              return y > closed + 1 && y < nextBefore - 1;
            }),
            isTrue,
          );
        } else {
          expect(closeStart, closeTo(closed, .01));
        }

        await tester.tap(toggle);
        await tester.pump();
        await record('open', 0);
        await tester.pump(const Duration(milliseconds: 64));
        final opening = await record('open', 64);
        await tester.tap(toggle);
        await tester.pump();
        final reversed = await record('reverse', 0);
        expect(
          reversed,
          closeTo(mode == 'normal' ? opening : closed, .01),
          reason: 'A second click continues from the current frame',
        );
        for (final milliseconds in [32, 96, 240, 400]) {
          final previous = frames.last['ms'] as int;
          await tester.pump(Duration(milliseconds: milliseconds - previous));
          await record('reverse', milliseconds);
        }
        await tester.pumpAndSettle();
        expect(tester.getTopLeft(group(next)).dy, closeTo(closed, .01));
        await _trace(tester, 'fold-$multiplayer-$mode', frames);
      }, variant: TargetPlatformVariant.only(TargetPlatform.windows));
    }
  }

  for (final width in [320.0, 1440.0]) {
    for (final mode in ['normal', 'app-reduced', 'system-reduced']) {
      testWidgets('Settings fold shares motion and reduction | $width $mode', (
        tester,
      ) async {
        _size(tester, Size(width, 1000));
        if (mode == 'system-reduced') {
          tester.platformDispatcher.accessibilityFeaturesTestValue =
              const FakeAccessibilityFeatures(disableAnimations: true);
          addTearDown(
            tester.platformDispatcher.clearAccessibilityFeaturesTestValue,
          );
        }
        final boundary = GlobalKey();
        final router = await _show(
          tester,
          games,
          lessMotion: mode == 'app-reduced',
          boundary: boundary,
        );
        router.go('/home');
        await tester.pumpAndSettle();
        if (unfoldCapture != null) await _warmBookImages(tester, games);
        final first = find.byKey(const PageStorageKey('launcher-settings'));
        final next = find.byKey(const PageStorageKey('theme-settings'));
        final before = tester.getTopLeft(next).dy;
        final header = tester.getTopLeft(first);
        final frames = <Map<String, Object?>>[];
        Future<double> record(String phase, int milliseconds) async {
          final y = tester.getTopLeft(next).dy;
          expect(tester.getTopLeft(first), header);
          frames.add({'phase': phase, 'ms': milliseconds, 'nextY': y});
          if (unfoldCapture != null) {
            await save(
              tester,
              boundary,
              Directory(unfoldCapture),
              'home-${width.toInt()}-$mode-$phase-$milliseconds',
            );
          }
          return y;
        }

        final title = _descend(first, find.text('启动器设置'));
        await record('before', 0);
        await tester.tap(title);
        await tester.pump();
        final start = await record('open', 0);
        for (final ms in [32, 64, 128, 240, 400]) {
          await tester.pump(
            Duration(milliseconds: ms - (frames.last['ms'] as int)),
          );
          await record('open', ms);
        }
        await tester.pumpAndSettle();
        final expanded = tester.getTopLeft(next).dy;
        expect(expanded, greaterThan(before + 50));
        expect(start, closeTo(mode == 'normal' ? before : expanded, .01));
        if (mode == 'normal') {
          expect(
            frames.any((frame) {
              final y = frame['nextY'] as double;
              return y > before + 1 && y < expanded - 1;
            }),
            isTrue,
          );
        }
        await tester.tap(title);
        await tester.pump();
        await tester.pump(const Duration(milliseconds: 64));
        final closing = await record('close', 64);
        await tester.tap(title);
        await tester.pump();
        final reversed = await record('reverse', 0);
        expect(reversed, closeTo(mode == 'normal' ? closing : expanded, .01));
        for (final ms in [32, 128, 400]) {
          await tester.pump(
            Duration(milliseconds: ms - (frames.last['ms'] as int)),
          );
          await record('reverse', ms);
        }
        await tester.pumpAndSettle();
        expect(tester.getTopLeft(next).dy, closeTo(expanded, .01));
        expect(tester.takeException(), isNull);
        await _trace(tester, 'settings-fold-${width.toInt()}-$mode', frames);
      }, variant: TargetPlatformVariant.only(TargetPlatform.windows));
    }
  }
}
