import 'dart:convert';
import 'dart:io';

import 'package:flutter/services.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:go_router/go_router.dart';
import 'package:material_ui/material_ui.dart';
import 'package:shared_preferences/shared_preferences.dart';
import 'package:shared_preferences_platform_interface/in_memory_shared_preferences_async.dart';
import 'package:shared_preferences_platform_interface/shared_preferences_async_platform_interface.dart';
import 'package:touhouonline/catalog.dart';
import 'package:touhouonline/launcher_select.dart';
import 'package:touhouonline/profile_page.dart';
import 'package:touhouonline/th09_board_page.dart';
import 'package:touhouonline/th09_rating_chart.dart';

import '../tools/capture_native_baseline_test.dart' show loadFonts, save;
import 'lobby_page_test.dart' show RatingFixtureApi;
import 'widget_test.dart' as launcher;

void main() {
  TestWidgetsFlutterBinding.ensureInitialized();
  final capture = Platform.environment['TOUHOU_ROOM_CAPTURE'];
  late Map<String, dynamic> data;
  setUpAll(() async {
    data = jsonDecode(
      File('tools/lobby-ratings-fixture.json').readAsStringSync(),
    ) as Map<String, dynamic>;
    if (capture != null) {
      await loadFonts(Directory(Platform.environment['TOUHOU_CAPTURE_FONTS']!));
    }
  });
  setUp(() {
    SharedPreferencesAsyncPlatform.instance =
        InMemorySharedPreferencesAsync.empty();
  });

  for (final (device, size) in [
    ('mobile-narrow', const Size(320, 640)),
    ('desktop', const Size(1440, 734)),
  ]) {
    testWidgets('$device finds a player deep in the full leaderboard', (
      tester,
    ) async {
      tester.view.devicePixelRatio = 1;
      tester.view.physicalSize = size;
      addTearDown(tester.view.resetDevicePixelRatio);
      addTearDown(tester.view.resetPhysicalSize);
      final original = data['board'] as Map<String, dynamic>;
      final entries = (original['board'] as List).cast<Map<String, dynamic>>();
      final fixture = {
        ...data,
        'board': {
          ...original,
          'board': [
            for (var index = 0; index < 64; index++)
              {
                ...entries.first,
                'player': 'fixture-$index',
                'rating': 1700 - index,
                'profile': null,
              },
            entries.last,
          ],
        },
      };
      final boundary = GlobalKey();
      final api = RatingFixtureApi(fixture);
      await tester.pumpWidget(
        RepaintBoundary(
          key: boundary,
          child: launcher.fixture(
            accountApi: api,
            preferences: LauncherPreferences(
              SharedPreferencesAsync(),
              ThemeMode.dark,
              true,
            ),
          ),
        ),
      );
      await tester.pumpAndSettle();
      await tester.tap(find.text('联机'));
      await tester.pumpAndSettle();
      await tester.tap(find.text('排行榜'));
      await tester.pumpAndSettle();
      final board = find.byType(Th09BoardPage);
      final search = find
          .descendant(of: board, matching: find.byType(TextField))
          .first;
      final locate = find.byWidgetPredicate(
        (widget) => widget is IconButton && widget.tooltip == '在全榜定位',
      );
      bool canLocate() => tester.widget<IconButton>(locate).onPressed != null;
      await tester.enterText(search, '@missing');
      await tester.pumpAndSettle();
      expect(find.text('没有符合条件的玩家。'), findsOneWidget);
      expect(canLocate(), isFalse);
      await tester.enterText(search, '@rated');
      await tester.pumpAndSettle();
      final rank = device == 'desktop' ? '65' : '#65';
      expect(find.text(rank), findsOneWidget);
      expect(find.text('fixture-63'), findsNothing);
      expect(canLocate(), isTrue);
      await tester.tap(locate);
      await tester.pumpAndSettle();
      final player = find.byKey(const ValueKey('rated'));
      expect(player.hitTestable(), findsOneWidget);
      expect(find.text(rank).hitTestable(), findsOneWidget);
      expect(find.text('fixture-63').hitTestable(), findsOneWidget);
      final tile = find.descendant(
        of: player,
        matching: find.byType(ExpansionTile),
      );
      expect(Focus.of(tester.element(tile)).hasPrimaryFocus, isTrue);
      if (capture != null) {
        await save(
          tester,
          boundary,
          Directory(capture),
          '$device-board-locate',
        );
      }
      final located = tester.getRect(player);
      api.updatedRating = 1450;
      await tester.pump(const Duration(seconds: 13));
      await tester.pumpAndSettle();
      expect(tester.getRect(player), located);
      final updated = device == 'desktop' ? '1,450' : '1,450  ±120';
      expect(find.text(updated).hitTestable(), findsOneWidget);
      expect(Focus.of(tester.element(tile)).hasPrimaryFocus, isTrue);
      if (device == 'desktop') {
        tester.view.physicalSize = const Size(800, 640);
        await tester.pumpAndSettle();
        expect(player.hitTestable(), findsOneWidget);
        expect(find.text('#65').hitTestable(), findsOneWidget);
        expect(Focus.of(tester.element(tile)).hasPrimaryFocus, isTrue);
        tester.view.physicalSize = size;
        await tester.pumpAndSettle();
        expect(player.hitTestable(), findsOneWidget);
        expect(find.text('65').hitTestable(), findsOneWidget);
        expect(Focus.of(tester.element(tile)).hasPrimaryFocus, isTrue);
      }
      final list = find.descendant(
        of: board,
        matching: find.byType(CustomScrollView),
      );
      tester.widget<CustomScrollView>(list).controller!.jumpTo(0);
      await tester.pumpAndSettle();
      expect(canLocate(), isFalse);
      await tester.tap(find.byTooltip('清空搜索'));
      await tester.pumpAndSettle();
      expect(find.text('fixture-0').hitTestable(), findsOneWidget);
      expect(tester.takeException(), isNull);
    });
  }

  testWidgets('desktop columns align and retain board state across widths', (
    tester,
  ) async {
    tester.view.devicePixelRatio = 1;
    tester.view.physicalSize = const Size(1440, 734);
    addTearDown(tester.view.resetDevicePixelRatio);
    addTearDown(tester.view.resetPhysicalSize);
    final boundary = GlobalKey();
    final api = RatingFixtureApi(data);
    final preferences = LauncherPreferences(
      SharedPreferencesAsync(),
      ThemeMode.dark,
      true,
    );
    await tester.pumpWidget(
      RepaintBoundary(
        key: boundary,
        child: launcher.fixture(accountApi: api, preferences: preferences),
      ),
    );
    await tester.pumpAndSettle();
    await tester.tap(find.text('联机'));
    await tester.pumpAndSettle();
    await tester.tap(find.text('排行榜'));
    await tester.pumpAndSettle();

    final header = find.byKey(const ValueKey('board-column-header'));
    final rated = find.byKey(const ValueKey('rated'));
    final other = find.byKey(const ValueKey('other'));
    Finder cell(Finder row, String name) =>
        find.descendant(of: row, matching: find.byKey(ValueKey('board-$name')));
    void assertCells(Finder row, Map<String, String> values) {
      for (final MapEntry(:key, :value) in values.entries) {
        final heading = tester.getRect(cell(header, key));
        final body = tester.getRect(cell(row, key));
        expect(body.left, closeTo(heading.left, .01));
        expect(body.right, closeTo(heading.right, .01));
        expect(
          find.descendant(of: cell(row, key), matching: find.text(value)),
          findsOneWidget,
        );
      }
    }

    final material = MaterialLocalizations.of(tester.element(header));
    final date = material.formatCompactDate(
      DateTime.fromMillisecondsSinceEpoch(1795449600000),
    );
    assertCells(rated, {
      'position': '2',
      'name': '灵梦',
      'score': '1,438',
      'interval': '± 120',
      'games': '26',
      'winRate': '50.0%',
      'group': '组 1',
      'lastPlayed': date,
      'status': '正式',
    });
    final winRate = find.descendant(
      of: cell(header, 'winRate'),
      matching: find.byType(TextButton),
    );
    await tester.tap(winRate);
    await tester.pumpAndSettle();
    expect(tester.getTopLeft(rated).dy, lessThan(tester.getTopLeft(other).dy));
    expect(
      find.descendant(of: winRate, matching: find.byIcon(Icons.arrow_downward)),
      findsOneWidget,
    );
    assertCells(rated, {'position': '2'});
    if (capture != null) {
      await save(tester, boundary, Directory(capture), 'desktop-board-columns');
    }
    await preferences.setTheme(ThemeMode.light);
    await tester.pumpAndSettle();
    assertCells(rated, {'winRate': '50.0%', 'score': '1,438'});
    if (capture != null) {
      await save(
        tester,
        boundary,
        Directory(capture),
        'desktop-board-columns-light',
      );
    }
    await preferences.setTheme(ThemeMode.dark);
    await tester.pumpAndSettle();

    final scope = find.byType(LauncherSelect<int?>);
    await tester.tap(scope);
    await tester.pumpAndSettle();
    await tester.tap(find.widgetWithText(MenuItemButton, '灵梦'));
    await tester.pumpAndSettle();
    assertCells(rated, {
      'position': '1',
      'score': '1,410',
      'overall': '1,438',
      'games': '13',
    });
    tester.view.physicalSize = const Size(801, 640);
    await tester.pumpAndSettle();
    assertCells(rated, {'score': '1,410', 'overall': '1,438', 'games': '13'});
    if (capture != null) {
      await save(
        tester,
        boundary,
        Directory(capture),
        'desktop-narrow-board-character-columns',
      );
    }
    tester.view.physicalSize = const Size(800, 640);
    await tester.pumpAndSettle();
    expect(header, findsNothing);
    expect(find.text('1,410  ±90').hitTestable(), findsOneWidget);
    tester.view.physicalSize = const Size(1440, 734);
    await tester.pumpAndSettle();
    assertCells(rated, {'score': '1,410', 'overall': '1,438'});
    final card = find.byKey(const ValueKey('th09-player-card'));
    await tester.tap(find.byKey(const ValueKey('th09-card-rated')));
    await tester.pumpAndSettle();
    expect(card, findsOneWidget);
    await tester.sendKeyEvent(LogicalKeyboardKey.escape);
    await tester.pumpAndSettle();
    expect(card, findsNothing);
    await tester.tap(
      find.descendant(of: cell(rated, 'name'), matching: find.text('灵梦')),
    );
    await tester.pumpAndSettle();
    final profile = find.byType(ProfilePage);
    expect(profile, findsOneWidget);
    expect(
      GoRouterState.of(tester.element(profile))
          .uri
          .queryParameters['character'],
      '0',
    );
    await tester.binding.handlePopRoute();
    await tester.pumpAndSettle();
    assertCells(rated, {'score': '1,410', 'overall': '1,438'});
    expect(tester.takeException(), isNull);
  });

  for (final (device, size) in [
    ('mobile-narrow', const Size(320, 640)),
    ('mobile', const Size(390, 844)),
    ('desktop', const Size(1440, 734)),
  ]) {
    testWidgets(
      '$device leaderboard opens public cards and refreshes in place',
      (tester) async {
        tester.view.devicePixelRatio = 1;
        tester.view.physicalSize = size;
        addTearDown(tester.view.resetDevicePixelRatio);
        addTearDown(tester.view.resetPhysicalSize);
        final api = RatingFixtureApi(data);
        final boundary = GlobalKey();
        await tester.pumpWidget(
          RepaintBoundary(
            key: boundary,
            child: launcher.fixture(
              accountApi: api,
              preferences: LauncherPreferences(
                SharedPreferencesAsync(),
                ThemeMode.dark,
                true,
              ),
            ),
          ),
        );
        await tester.pumpAndSettle();
        await tester.tap(find.text('联机'));
        await tester.pumpAndSettle();
        await tester.tap(find.text('排行榜'));
        await tester.pumpAndSettle();
        final board = find.byType(Th09BoardPage);
        expect(board, findsOneWidget);
        expect(api.boardRequests, greaterThanOrEqualTo(1));
        final list = find.descendant(
          of: board,
          matching: find.byType(CustomScrollView),
        );
        final trigger = find.byKey(const ValueKey('th09-card-rated'));
        final card = find.byKey(const ValueKey('th09-player-card'));
        await tester.scrollUntilVisible(
          trigger,
          120,
          scrollable: find
              .descendant(of: list, matching: find.byType(Scrollable))
              .first,
        );
        await tester.pumpAndSettle();
        final before = tester.getRect(trigger);
        await tester.tap(trigger);
        await tester.pumpAndSettle();
        final portraits = find.descendant(
          of: card,
          matching: find.byType(Th09CharacterPortrait),
        );
        expect(portraits, findsNWidgets(3));
        expect(
          find.descendant(of: card, matching: find.text('1,438')),
          findsOneWidget,
        );
        if (capture != null) {
          final context = tester.element(card);
          await tester.runAsync(
            () => Future.wait([
              for (final id in [0, 2, 4])
                precacheImage(
                  AssetImage('assets/th09-portraits/$id.webp'),
                  context,
                ),
            ]),
          );
          await tester.pump();
          await save(
            tester,
            boundary,
            Directory(capture),
            '$device-board-rating-card',
          );
        }
        if (device == 'desktop') {
          await tester.sendKeyEvent(LogicalKeyboardKey.escape);
        } else {
          await tester.binding.handlePopRoute();
        }
        await tester.pumpAndSettle();
        expect(card, findsNothing);
        expect(tester.getRect(trigger), before);

        final reads = api.boardRequests;
        api.failBoard = true;
        await tester.pump(const Duration(seconds: 13));
        await tester.pumpAndSettle();
        expect(api.boardRequests, greaterThan(reads));
        expect(tester.getRect(trigger), before);
        expect(find.text('榜单读取失败。'), findsNothing);
        expect(api.requests, isEmpty);

        api.failBoard = false;
        api.updatedRating = 1500;
        await tester.pump(const Duration(seconds: 13));
        await tester.pumpAndSettle();
        final updated = device == 'desktop' ? '1,500' : '1,500  ±120';
        expect(find.text(updated), findsOneWidget);
        expect(tester.getRect(trigger), before);

        final router = GoRouter.of(tester.element(board));
        router.push('/ratings.html?character=0');
        await tester.pumpAndSettle();
        await tester.scrollUntilVisible(
          trigger,
          120,
          scrollable: find
              .descendant(of: list, matching: find.byType(Scrollable))
              .first,
        );
        await tester.pumpAndSettle();
        await tester.tap(trigger);
        await tester.pumpAndSettle();
        await tester.tap(
          find.descendant(of: card, matching: find.text('查看个人主页')),
        );
        await tester.pumpAndSettle();
        final profile = find.byType(ProfilePage);
        expect(profile, findsOneWidget);
        final address = GoRouterState.of(tester.element(profile)).uri;
        expect(address.path, '/u/rated');
        expect(address.queryParameters['character'], '0');
        final lastBoardRead = api.boardRequests;
        await tester.pump(const Duration(seconds: 13));
        expect(api.boardRequests, lastBoardRead);
        expect(tester.takeException(), isNull);
      },
    );
  }

  for (final scenario in [
    (
      language: LauncherLanguage.simplifiedChinese,
      device: 'desktop',
      size: const Size(1440, 734),
      lobby: '联机',
      rankings: '排行榜',
      title: '花映塚排行榜',
      failure: '榜单读取失败。',
      retry: '重试',
      filter: '筛选',
      minimum: '最少局数',
      status: '定级状态',
      group: '交手组',
      clear: '清除筛选',
      empty: '没有符合条件的玩家。',
      character: '灵梦',
      active: '正式',
      distribution: '评分分布',
      sort: '排序',
      help: '评分与定级说明',
    ),
    (
      language: LauncherLanguage.english,
      device: 'mobile-narrow',
      size: const Size(320, 640),
      lobby: 'Multiplayer',
      rankings: 'Rankings',
      title: 'Flower View rankings',
      failure: 'Could not load rankings.',
      retry: 'Retry',
      filter: 'Filter',
      minimum: 'Minimum games',
      status: 'Rating status',
      group: 'Opponent group',
      clear: 'Clear filters',
      empty: 'No players match these filters.',
      character: 'Reimu',
      active: 'Active',
      distribution: 'Rating distribution',
      sort: 'Sort',
      help: 'About ratings',
    ),
    (
      language: LauncherLanguage.japanese,
      device: 'mobile',
      size: const Size(390, 844),
      lobby: 'マルチプレイ',
      rankings: 'ランキング',
      title: '花映塚ランキング',
      failure: 'ランキングを読み込めませんでした。',
      retry: '再試行',
      filter: '絞り込み',
      minimum: '最小対戦数',
      status: 'レートの状態',
      group: '対戦グループ',
      clear: '絞り込みを解除',
      empty: '条件に一致するプレイヤーはいません。',
      character: '霊夢',
      active: '確定',
      distribution: 'レート分布',
      sort: '並べ替え',
      help: 'レートについて',
    ),
  ]) {
    testWidgets(
      '${scenario.device} leaderboard uses ${scenario.language.tag}',
      (tester) async {
        tester.view.devicePixelRatio = 1;
        tester.view.physicalSize = scenario.size;
        addTearDown(tester.view.resetDevicePixelRatio);
        addTearDown(tester.view.resetPhysicalSize);
        final api = RatingFixtureApi(data)..failBoard = true;
        final boundary = GlobalKey();
        await tester.pumpWidget(
          RepaintBoundary(
            key: boundary,
            child: launcher.fixture(
              accountApi: api,
              preferences: LauncherPreferences(
                SharedPreferencesAsync(),
                ThemeMode.dark,
                true,
                language: scenario.language,
              ),
            ),
          ),
        );
        await tester.pumpAndSettle();
        await tester.tap(find.text(scenario.lobby));
        await tester.pumpAndSettle();
        final rankings = find.text(scenario.rankings);
        await tester.ensureVisible(rankings);
        await tester.pumpAndSettle();
        await tester.tap(rankings.hitTestable());
        await tester.pumpAndSettle();
        expect(find.text(scenario.title), findsOneWidget);
        expect(find.text(scenario.failure), findsOneWidget);
        api.failBoard = false;
        await tester.tap(find.text(scenario.retry));
        await tester.pumpAndSettle();
        expect(find.text(scenario.failure), findsNothing);
        final desktop = scenario.device == 'desktop';
        final rated = desktop ? '1,438' : '1,438  ±120';
        final other = desktop ? '1,710' : '1,710  ±95';
        expect(find.text(rated).hitTestable(), findsOneWidget);
        final filter = find.widgetWithText(OutlinedButton, scenario.filter);
        final scope = find.byType(LauncherSelect<int?>);
        final sort = desktop
            ? find.widgetWithText(TextButton, 'WHR')
            : find.byTooltip(scenario.sort);
        final search = find.byType(TextField);
        expect(tester.getRect(filter).height, tester.getRect(scope).height);
        expect(
          tester.getRect(filter).center.dy,
          tester.getRect(scope).center.dy,
        );
        if (!desktop) {
          expect(tester.getRect(search).height, tester.getRect(sort).height);
          expect(
            tester.getRect(search).center.dy,
            tester.getRect(sort).center.dy,
          );
        }
        await tester.tap(find.byTooltip(scenario.help));
        await tester.pumpAndSettle();
        expect(find.widgetWithText(AlertDialog, scenario.help), findsOneWidget);
        await tester.binding.handlePopRoute();
        await tester.pumpAndSettle();
        final dialog = find.byType(AlertDialog);
        expect(dialog, findsNothing);
        await tester.tap(filter);
        await tester.pumpAndSettle();
        expect(
          find.descendant(of: dialog, matching: find.text(scenario.group)),
          findsOneWidget,
        );
        expect(
          find.descendant(of: dialog, matching: find.text(scenario.status)),
          findsOneWidget,
        );
        expect(find.text(scenario.active).hitTestable(), findsOneWidget);
        if (capture != null) {
          await save(
            tester,
            boundary,
            Directory(capture),
            '${scenario.device}-board-filters-${scenario.language.tag}',
          );
        }
        final minimum = find.widgetWithText(TextField, scenario.minimum);
        expect(tester.getRect(minimum).height, greaterThanOrEqualTo(44));
        await tester.enterText(minimum, '100');
        await tester.pumpAndSettle();
        await tester.binding.handlePopRoute();
        await tester.pumpAndSettle();
        expect(find.byType(Th09BoardPage), findsOneWidget);
        expect(find.text(scenario.empty).hitTestable(), findsOneWidget);
        await tester.tap(find.text(scenario.clear));
        await tester.pumpAndSettle();
        expect(find.text(rated).hitTestable(), findsOneWidget);
        await tester.tap(sort);
        await tester.pumpAndSettle();
        if (!desktop) {
          await tester.tap(find.widgetWithText(MenuItemButton, 'WHR'));
          await tester.pumpAndSettle();
        }
        expect(
          tester.getTopLeft(find.text(rated)).dy,
          lessThan(tester.getTopLeft(find.text(other)).dy),
        );
        await tester.tap(scope);
        await tester.pumpAndSettle();
        await tester.tap(
          find.widgetWithText(MenuItemButton, scenario.character),
        );
        await tester.pumpAndSettle();
        final score = desktop ? '1,410' : '1,410  ±90';
        expect(find.text(score).hitTestable(), findsOneWidget);
        await tester.tap(filter);
        await tester.pumpAndSettle();
        expect(
          find.descendant(of: dialog, matching: find.text(scenario.group)),
          findsNothing,
        );
        await tester.sendKeyEvent(LogicalKeyboardKey.escape);
        await tester.pumpAndSettle();
        expect(dialog, findsNothing);
        await tester.tap(find.text(scenario.distribution));
        await tester.pumpAndSettle();
        expect(find.bySemanticsLabel(RegExp(r'1,400')), findsOneWidget);
        expect(tester.takeException(), isNull);
      },
    );
  }
}
