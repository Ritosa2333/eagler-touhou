import 'dart:io';
import 'dart:convert';

import 'package:flutter_test/flutter_test.dart';
import 'package:flutter/rendering.dart' show RenderParagraph;
import 'package:material_ui/material_ui.dart';
import 'package:shared_preferences/shared_preferences.dart';
import 'package:shared_preferences_platform_interface/in_memory_shared_preferences_async.dart';
import 'package:shared_preferences_platform_interface/shared_preferences_async_platform_interface.dart';
import 'package:touhouonline/catalog.dart';
import 'package:touhouonline/game_launcher.dart';
import 'package:touhouonline/game_preferences.dart';
import 'package:touhouonline/game_resources.dart';
import 'package:touhouonline/game_user_files.dart';
import 'package:touhouonline/library.dart';
import 'package:touhouonline/launcher_select.dart';

import '../tools/capture_native_baseline_test.dart' show loadFonts, save;
import 'widget_test.dart' as fixtures;

final _rail = find.byKey(const PageStorageKey('recently played games'));

Finder _card(String id) =>
    find.descendant(of: _rail, matching: find.byKey(ValueKey(id)));

void main() {
  TestWidgetsFlutterBinding.ensureInitialized();
  late final List<Game> catalog;

  final capture = Platform.environment['TOUHOU_LIBRARY_LAYOUT_CAPTURE'];
  setUpAll(() async {
    catalog = await Game.load();
    if (Platform.isWindows || capture != null) {
      final fonts = Platform.environment['TOUHOU_CAPTURE_FONTS'];
      await loadFonts(fonts == null ? null : Directory(fonts));
    }
  });
  setUp(() {
    SharedPreferencesAsyncPlatform.instance =
        InMemorySharedPreferencesAsync.empty();
  });

  for (final width in [320.0, 390.0, 1024.0, 1440.0]) {
    testWidgets('Shelf size, spacing and alignment at $width', (tester) async {
      tester.view.devicePixelRatio = 1;
      tester.view.physicalSize = Size(width, 734);
      addTearDown(tester.view.resetDevicePixelRatio);
      addTearDown(tester.view.resetPhysicalSize);
      final storage = SharedPreferencesAsync();
      final history = await GamePlayHistory.load(storage);
      for (final id in ['th08', 'th07', 'th06']) {
        await history.record(id, playedAt: DateTime.utc(2026, 11, 25));
      }
      final preferences = LauncherPreferences(
        storage,
        ThemeMode.dark,
        true,
        language: LauncherLanguage.english,
      );
      final boundary = GlobalKey();
      await tester.pumpWidget(
        RepaintBoundary(
          key: boundary,
          child: fixtures.fixture(
            games: catalog,
            history: history,
            preferences: preferences,
            accountApi: fixtures.FixtureAccountApi(),
          ),
        ),
      );
      await tester.pumpAndSettle();
      final library = find.byType(LibraryPage);
      final grid = find.descendant(
        of: library,
        matching: find.byType(SliverGrid),
      );
      final covers = find.descendant(
        of: grid,
        matching: find.byType(GameArtwork),
      );
      final recentCover = find.descendant(
        of: _card('th06'),
        matching: find.byType(GameArtwork),
      );
      if (capture != null) {
        final context = tester.element(library);
        await tester.runAsync(() async {
          for (final game in catalog) {
            if (game.artwork case final String cover) {
              await precacheImage(
                AssetImage('assets/book-covers/$cover'),
                context,
              );
            }
          }
        });
        await tester.pumpAndSettle();
        await save(
          tester,
          boundary,
          Directory(capture),
          'shelf-${width.toInt()}',
        );
      }
      expect(tester.takeException(), isNull);
      final heading = find.text('Recently played');
      expect(
        tester.getTopLeft(recentCover).dx,
        tester.getTopLeft(heading).dx + (width > 780 ? 6 : 0),
        reason:
            'Desktop recent items retain the original 6 px interactive inset',
      );
      expect(tester.getTopLeft(covers.first).dx, tester.getTopLeft(heading).dx);
      final mode = find.byKey(const ValueKey('library view control'));
      final sort = find.byType(LauncherSelect<GameOrder>);
      expect(tester.getSize(mode).height, tester.getSize(sort).height);
      expect(tester.getCenter(mode).dy, tester.getCenter(sort).dy);
      final cover = tester.getRect(covers.first);
      expect(cover.width, closeTo(cover.height, .01));
      if (width > 780) {
        expect(tester.getSize(find.byType(SearchBar)).height, 44);
        final label = find.text('Cover view');
        if (label.evaluate().isNotEmpty) {
          final paragraph = tester.renderObject<RenderParagraph>(label);
          expect(
            paragraph.getBoxesForSelection(
              const TextSelection(baseOffset: 0, extentOffset: 10),
            ),
            hasLength(1),
            reason: 'The view label stays on one line',
          );
        }
        final nextRow = covers
            .evaluate()
            .map(
              (element) => tester.getRect(
                find.byElementPredicate((value) => identical(value, element)),
              ),
            )
            .firstWhere((rect) => rect.top > cover.bottom);
        expect(
          nextRow.top - cover.bottom,
          inInclusiveRange(80, 90),
          reason: 'The two-line caption and row spacing do not grow with artwork width',
        );
        final card = find.descendant(
          of: grid,
          matching: find.byKey(const ValueKey('th05')),
        );
        expect(
          find.descendant(of: card, matching: find.text('Mystic Square')),
          findsOneWidget,
        );
        final favorite = find.descendant(
          of: card,
          matching: find.byType(FavoriteButton),
        );
        expect(
          tester.getCenter(favorite).dy,
          lessThan(cover.top + cover.height / 2),
        );
        expect(tester.getCenter(favorite).dx, greaterThan(cover.center.dx));
        await tester.tap(favorite);
        await tester.pumpAndSettle();
        expect(
          tester.widget<FavoriteButton>(favorite).book.favorites.ids,
          contains('th05'),
        );
        // The caption belongs to the same navigable card, not a detached label.
        await tester.tap(
          find.descendant(of: card, matching: find.text('Mystic Square')),
        );
        await tester.pumpAndSettle();
        expect(find.byType(BackButton), findsOneWidget);
        await tester.tap(find.byType(BackButton));
        await tester.pumpAndSettle();
        expect(find.byType(LibraryPage), findsOneWidget);
      }
    }, variant: TargetPlatformVariant.only(TargetPlatform.windows));
  }

  for (final width in [320.0, 390.0, 781.0, 1440.0]) {
    for (final language in LauncherLanguage.values) {
      for (final light in [false, true]) {
        for (final scale in [1.0, 2.0]) {
          final name =
              '${width.toInt()}-${language.name}-${light ? 'light' : 'dark'}-${scale.toInt()}';
          testWidgets('Shelf captions at actual text scale | $name', (
            tester,
          ) async {
            tester.view.devicePixelRatio = 1;
            tester.view.physicalSize = Size(width, 734);
            addTearDown(tester.view.resetDevicePixelRatio);
            addTearDown(tester.view.resetPhysicalSize);
            tester.platformDispatcher.textScaleFactorTestValue = scale;
            addTearDown(
              tester.platformDispatcher.clearTextScaleFactorTestValue,
            );
            final storage = SharedPreferencesAsync();
            final history = await GamePlayHistory.load(storage);
            for (final id in ['th08', 'th07', 'th06']) {
              await history.record(id, playedAt: DateTime.utc(2026, 11, 25));
            }
            final prefs = LauncherPreferences(
              storage,
              light ? ThemeMode.light : ThemeMode.dark,
              true,
              language: language,
            );
            final boundary = GlobalKey();
            await tester.pumpWidget(
              RepaintBoundary(
                key: boundary,
                child: fixtures.fixture(
                  games: catalog,
                  history: history,
                  preferences: prefs,
                  accountApi: fixtures.FixtureAccountApi(),
                ),
              ),
            );
            await tester.pumpAndSettle();
            final library = find.byType(LibraryPage);
            final grid = find.descendant(
              of: library,
              matching: find.byType(SliverGrid),
            );
            final output = Platform.environment['TOUHOU_SHELF_TEXT_CAPTURE'];
            if (output != null) {
              await tester.runAsync(() async {
                final context = tester.element(library);
                for (final game in catalog) {
                  if (game.artwork case final String cover) {
                    await precacheImage(
                      AssetImage('assets/book-covers/$cover'),
                      context,
                    );
                  }
                }
                for (final mode in ['dark', 'light']) {
                  await precacheImage(
                    AssetImage('assets/launcher-wordmark-$mode.png'),
                    context,
                  );
                }
              });
              await tester.pumpAndSettle();
              await save(tester, boundary, Directory(output), name);
            }
            final violations = <String>[];
            final rects = <String, Object>{};
            for (final game in catalog.take(4)) {
              final card = find.descendant(
                of: grid,
                matching: find.byKey(ValueKey(game.id)),
              );
              if (card.evaluate().isEmpty) continue;
              final bounds = tester.getRect(card);
              final art = find.descendant(
                of: card,
                matching: find.byType(GameArtwork),
              );
              final cover = tester.getRect(art);
              expect(cover.width, closeTo(cover.height, .01));
              rects[game.id] = [
                bounds.left,
                bounds.top,
                bounds.width,
                bounds.height,
              ];
              for (final element
                  in find
                      .descendant(of: card, matching: find.byType(RichText))
                      .evaluate()) {
                if (element.findAncestorWidgetOfExactType<Icon>() != null) {
                  continue;
                }
                final paragraph = element.renderObject! as RenderParagraph;
                final text = paragraph.text.toPlainText();
                final origin = paragraph.localToGlobal(Offset.zero);
                for (final box in paragraph.getBoxesForSelection(
                  TextSelection(baseOffset: 0, extentOffset: text.length),
                )) {
                  if (origin.dy + box.bottom > bounds.bottom + .5 ||
                      origin.dy + box.top < cover.bottom - .5) {
                    violations.add(
                      '${game.id} | $text | glyph outside caption',
                    );
                  }
                }
              }
            }
            for (final element
                in find
                    .descendant(of: _rail, matching: find.byType(RichText))
                    .evaluate()) {
              final paragraph = element.renderObject! as RenderParagraph;
              final text = paragraph.text.toPlainText();
              final origin = paragraph.localToGlobal(Offset.zero);
              final rail = tester.getRect(_rail);
              for (final box in paragraph.getBoxesForSelection(
                TextSelection(baseOffset: 0, extentOffset: text.length),
              )) {
                if (origin.dy + box.bottom > rail.bottom + .5 ||
                    origin.dy + box.top < rail.top - .5) {
                  violations.add('recent | $text | glyph outside row');
                }
              }
            }
            final errors = <String>[];
            for (
              var e = tester.takeException();
              e != null;
              e = tester.takeException()
            ) {
              errors.add(e.toString());
            }
            if (output != null) {
              File('$output/$name.json').writeAsStringSync(
                jsonEncode({
                  'source': 'assets/game_catalog.json',
                  'width': width,
                  'language': language.name,
                  'scale': scale,
                  'light': light,
                  'cards': rects,
                  'violations': violations,
                  'errors': errors,
                }),
              );
            }
            expect(errors, isEmpty);
            expect(violations, isEmpty);
            if (width > 780) {
              for (final id in ['th06', 'th07']) {
                final code = catalog
                    .singleWhere((game) => game.id == id)
                    .displayCode;
                final number = find.descendant(
                  of: _card(id),
                  matching: find.text(code),
                );
                expect(
                  number,
                  findsOneWidget,
                  reason: 'The recent title must leave its work number visible',
                );
                expect(
                  tester
                      .renderObject<RenderParagraph>(number)
                      .didExceedMaxLines,
                  isFalse,
                );
              }
            }
            final target = find.descendant(
              of: grid,
              matching: find.byKey(const ValueKey('th06')),
            );
            final favorite = find.descendant(
              of: target,
              matching: find.byType(FavoriteButton),
            );
            await tester.ensureVisible(favorite);
            await tester.tap(favorite);
            await tester.pumpAndSettle();
            expect(
              tester.widget<FavoriteButton>(favorite).book.favorites.ids,
              contains('th06'),
            );
            await tester.pumpWidget(const SizedBox());
            await tester.pumpAndSettle();
          }, variant: TargetPlatformVariant.only(TargetPlatform.windows));
        }
      }
    }
  }

  testWidgets(
    'Successful play records the title and date through the real button',
    (tester) async {
      tester.view.devicePixelRatio = 1;
      tester.view.physicalSize = const Size(390, 720);
      addTearDown(tester.view.resetDevicePixelRatio);
      addTearDown(tester.view.resetPhysicalSize);

      final storage = SharedPreferencesAsync();
      final history = await GamePlayHistory.load(storage);
      final options = GamePreferenceStore(storage);
      await tester.runAsync(() => options.touch.load(fixtures.game.id));
      await tester.pumpWidget(
        fixtures.fixture(
          history: history,
          gamePreferences: options,
          launcher: fixtures.RecordingGameLauncher(
            status: GameLaunchStatus.started,
          ),
        ),
      );
      await tester.pumpAndSettle();
      expect(_rail, findsNothing);
      await tester.tap(find.byType(GameArtwork));
      await tester.pumpAndSettle();
      expect(history.recentIds, isEmpty);

      await tester.tap(find.text('开始游戏'));
      await tester.pumpAndSettle();
      expect(find.text('原生游戏暂不可用'), findsNothing);
      final played = history.lastPlayed('th06')!;
      final date = MaterialLocalizations.of(
        tester.element(find.byType(BackButton)),
      ).formatShortMonthDay(played.toLocal());
      expect(find.text(date), findsOneWidget);
      expect(
        (await GamePlayHistory.load(storage))
            .lastPlayed('th06')!
            .millisecondsSinceEpoch,
        played.millisecondsSinceEpoch,
      );

      await tester.tap(find.byType(BackButton));
      await tester.pumpAndSettle();
      expect(_card('th06'), findsOneWidget);
      await tester.tap(_card('th06'));
      await tester.pumpAndSettle();
      expect(find.text(date), findsOneWidget);
    },
    variant: TargetPlatformVariant.only(TargetPlatform.windows),
  );

  for (final width in [320.0, 390.0, 1440.0]) {
    testWidgets(
      'Recently played keeps horizontal offset through detail at $width',
      (tester) async {
        tester.view.devicePixelRatio = 1;
        tester.view.physicalSize = Size(width, 700);
        addTearDown(tester.view.resetDevicePixelRatio);
        addTearDown(tester.view.resetPhysicalSize);

        final storage = SharedPreferencesAsync();
        final history = await GamePlayHistory.load(storage);
        for (final id in [
          'th15',
          'th11',
          'th10',
          'th09',
          'th08',
          'th07',
          'th06',
        ]) {
          await history.record(id, playedAt: DateTime(2026, 11, 25));
        }
        await tester.pumpWidget(
          fixtures.fixture(
            games: catalog,
            history: await GamePlayHistory.load(storage),
          ),
        );
        await tester.pumpAndSettle();

        final heading = find.text('最近游玩');
        final library = find.byType(LibraryPage);
        final all = find
            .descendant(of: library, matching: find.text('全部游戏'))
            .first;
        expect(
          tester.getCenter(heading).dy,
          lessThan(tester.getCenter(all).dy),
        );
        final first = tester.getSize(_card('th06'));
        if (width < 780) {
          expect(first.width, closeTo(first.height, .01));
          expect(
            find.descendant(
              of: _rail,
              matching: find.text(fixtures.game.subtitle),
            ),
            findsNothing,
          );
        } else {
          expect(first.width, 230);
          expect(
            find.descendant(
              of: _rail,
              matching: find.text(fixtures.game.subtitle),
            ),
            findsOneWidget,
          );
        }

        await tester.drag(_rail, const Offset(-1000, 0));
        await tester.pumpAndSettle();
        double offset() => tester
            .state<ScrollableState>(
              find.descendant(of: _rail, matching: find.byType(Scrollable)),
            )
            .position
            .pixels;
        final before = offset();
        expect(before, greaterThan(0));
        final th15 = _card('th15').hitTestable();
        expect(th15, findsOneWidget);
        await tester.tap(th15);
        await tester.pumpAndSettle();
        expect(find.text('开始游戏'), findsOneWidget);
        await tester.tap(find.byType(BackButton));
        await tester.pumpAndSettle();
        expect(offset(), closeTo(before, .01));
      },
      variant: TargetPlatformVariant.only(TargetPlatform.windows),
    );
  }

  test('History accepts only ordinary acknowledged starts', () async {
    final storage = SharedPreferencesAsync();
    final history = await GamePlayHistory.load(storage);
    GameLaunchService service(GameLauncher runtime) => GameLaunchService(
      runtime,
      GamePreferenceStore(storage),
      GameResources(),
      GameUserFiles(),
      history: history,
    );
    final live = service(
      fixtures.RecordingGameLauncher(status: GameLaunchStatus.started),
    );
    expect(
      await service(const NativeGamePlaceholder())
          .start(fixtures.game, multiplayer: false, isActive: () => true),
      GameLaunchStatus.nativeRuntimeUnavailable,
    );
    expect(
      await live.start(fixtures.game, multiplayer: true, isActive: () => true),
      GameLaunchStatus.started,
    );
    expect(
      await live.start(
        fixtures.game,
        multiplayer: false,
        isActive: () => true,
        replayFile: File('observed-replay.rpy'),
      ),
      GameLaunchStatus.started,
    );
    expect(
      await live.start(
        fixtures.game,
        multiplayer: false,
        isActive: () => false,
      ),
      isNull,
    );
    expect(history.recentIds, isEmpty);

    expect(
      await live.start(fixtures.game, multiplayer: false, isActive: () => true),
      GameLaunchStatus.started,
    );
    expect(history.recentIds, orderedEquals(['th06']));
  });

  test(
    'History persists unique most recent twenty entries across queued writes',
    () async {
      final storage = SharedPreferencesAsync();
      final history = await GamePlayHistory.load(storage);
      final sample = DateTime.utc(2026, 11, 25);
      await Future.wait([
        for (var index = 1; index <= 22; index++)
          history.record(
            'th$index',
            playedAt: sample.add(Duration(minutes: index)),
          ),
        history.record(
          'th9',
          playedAt: sample.add(const Duration(minutes: 23)),
        ),
      ]);
      final reloaded = await GamePlayHistory.load(storage);
      expect(reloaded.recentIds.length, 20);
      expect(
        reloaded.recentIds.take(3),
        orderedEquals(['th9', 'th22', 'th21']),
      );
      expect(
        reloaded.lastPlayed('th9'),
        sample.add(const Duration(minutes: 23)),
      );
      expect(reloaded.lastPlayed('th2'), isNull);
    },
  );
}
