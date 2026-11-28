import 'dart:io';
import 'dart:ui' show SemanticsAction, Tristate;

import 'package:flutter/services.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:material_ui/material_ui.dart';
import 'package:go_router/go_router.dart';
import 'package:prism_ui/prism_ui.dart' as prism;
import 'package:shared_preferences/shared_preferences.dart';
import 'package:shared_preferences_platform_interface/in_memory_shared_preferences_async.dart';
import 'package:shared_preferences_platform_interface/shared_preferences_async_platform_interface.dart';
import 'package:touhouonline/catalog.dart';
import 'package:touhouonline/game_detail.dart';
import 'package:touhouonline/library.dart';

import '../tools/capture_native_baseline_test.dart' show loadFonts;
import 'widget_test.dart' as fixtures;

void main() {
  late Game game;
  setUpAll(() async {
    game = (await Game.load()).singleWhere((game) => game.id == 'th06');
    if (Platform.isWindows) {
      final fonts = Platform.environment['TOUHOU_CAPTURE_FONTS'];
      await loadFonts(fonts == null ? null : Directory(fonts));
    }
  });
  setUp(() {
    SharedPreferencesAsyncPlatform.instance =
        InMemorySharedPreferencesAsync.empty();
  });

  testWidgets('Desktop brand is a named library button', (tester) async {
    tester.view.devicePixelRatio = 1;
    tester.view.physicalSize = const Size(1440, 844);
    addTearDown(tester.view.resetDevicePixelRatio);
    addTearDown(tester.view.resetPhysicalSize);
    final semantics = tester.ensureSemantics();
    try {
      final app = fixtures.fixture(
        games: [game],
        accountApi: fixtures.FixtureAccountApi(),
      );
      await tester.pumpWidget(app);
      await tester.pumpAndSettle();
      Finder brand() => find
          .descendant(
            of: find.byKey(const ValueKey('masthead site')),
            matching: find.byType(prism.PressSurface),
          )
          .first;
      final node = tester.getSemantics(brand());
      final data = node.getSemanticsData();
      expect(data.flagsCollection.isButton, isTrue);
      expect(data.label, '東方 Online');
      expect(data.flagsCollection.isImage, isFalse);
      expect(data.hasAction(SemanticsAction.tap), isTrue);
      GoRouter.of(tester.element(find.byType(LibraryPage))).push('/game/th06');
      await tester.pumpAndSettle();
      expect(find.byType(GameDetail), findsOneWidget);
      final active = tester.getSemantics(brand());
      active.owner!.performAction(active.id, SemanticsAction.tap);
      await tester.pumpAndSettle();
      expect(find.byType(LibraryPage).hitTestable(), findsOneWidget);
      GoRouter.of(tester.element(find.byType(LibraryPage))).push('/game/th06');
      await tester.pumpAndSettle();
      await tester.tap(brand());
      await tester.pumpAndSettle();
      expect(find.byType(LibraryPage).hitTestable(), findsOneWidget);
      GoRouter.of(tester.element(find.byType(LibraryPage))).push('/game/th06');
      await tester.pumpAndSettle();
      Focus.of(
        tester.element(
          find.descendant(of: brand(), matching: find.byType(Image)),
        ),
      ).requestFocus();
      await tester.pump();
      await tester.sendKeyEvent(LogicalKeyboardKey.enter);
      await tester.pumpAndSettle();
      expect(find.byType(LibraryPage).hitTestable(), findsOneWidget);
      expect(tester.takeException(), isNull);
    } finally {
      semantics.dispose();
    }
  }, variant: TargetPlatformVariant.only(TargetPlatform.windows));

  for (final scale in [1.0, 2.0]) {
    testWidgets('Shared buttons expose names and selection | $scale', (
      tester,
    ) async {
      tester.view.devicePixelRatio = 1;
      tester.view.physicalSize = const Size(1440, 844);
      tester.platformDispatcher.textScaleFactorTestValue = scale;
      addTearDown(tester.view.resetDevicePixelRatio);
      addTearDown(tester.view.resetPhysicalSize);
      addTearDown(tester.platformDispatcher.clearTextScaleFactorTestValue);
      final semantics = tester.ensureSemantics();
      try {
        final app = fixtures.fixture(
          games: [game],
          accountApi: fixtures.FixtureAccountApi(),
        );
        await tester.pumpWidget(app);
        await tester.pumpAndSettle();
        Finder tabs() => find.descendant(
          of: find.byKey(const ValueKey('masthead navigation')),
          matching: find.byType(prism.PressSurface),
        );
        void checkTab(int index, String label, bool selected) {
          final data = tester.getSemantics(tabs().at(index)).getSemanticsData();
          expect(data.label, label);
          expect(data.flagsCollection.isButton, isTrue);
          expect(data.flagsCollection.isSelected == Tristate.isTrue, selected);
        }

        checkTab(0, '单机', true);
        checkTab(1, '联机', false);
        await tester.tap(tabs().at(1));
        await tester.pumpAndSettle();
        checkTab(0, '单机', false);
        checkTab(1, '联机', true);
        await tester.tap(tabs().first);
        await tester.pumpAndSettle();
        final side = find.byKey(const PageStorageKey(('game sidebar', false)));
        final all = find
            .descendant(of: side, matching: find.byType(prism.PressSurface))
            .first;
        expect(
          tester
                  .getSemantics(all)
                  .getSemanticsData()
                  .flagsCollection
                  .isSelected ==
              Tristate.isTrue,
          isTrue,
        );
        final row = find
            .descendant(of: side, matching: find.byType(GameRow))
            .first;
        final rowButton = find.descendant(
          of: row,
          matching: find.byType(prism.PressSurface),
        );
        await tester.ensureVisible(rowButton);
        await tester.tap(rowButton);
        await tester.pumpAndSettle();
        expect(
          tester
                  .getSemantics(rowButton)
                  .getSemanticsData()
                  .flagsCollection
                  .isSelected ==
              Tristate.isTrue,
          isTrue,
        );
        final router = GoRouter.of(tester.element(find.byType(GameDetail)));
        router.push('/announcements');
        await tester.pumpAndSettle();
        Finder message(String path) => find.byKey(ValueKey((path, 'sidebar')));
        expect(
          tester
                  .getSemantics(message('/announcements'))
                  .getSemanticsData()
                  .flagsCollection
                  .isSelected ==
              Tristate.isTrue,
          isTrue,
        );
        await tester.tap(message('/messages'));
        await tester.pumpAndSettle();
        final inbox = tester
            .getSemantics(message('/messages'))
            .getSemanticsData();
        expect(inbox.label, contains('私信'));
        expect(inbox.flagsCollection.isSelected == Tristate.isTrue, isTrue);
        expect(tester.takeException(), isNull);
      } finally {
        semantics.dispose();
      }
    }, variant: TargetPlatformVariant.only(TargetPlatform.windows));
  }

  for (final width in [320.0, 390.0, 1440.0]) {
    for (final recent in [false, true]) {
      testWidgets('Game cover is an actionable button | $width recent=$recent', (
        tester,
      ) async {
        tester.view.devicePixelRatio = 1;
        tester.view.physicalSize = Size(width, 844);
        addTearDown(tester.view.resetDevicePixelRatio);
        addTearDown(tester.view.resetPhysicalSize);
        final semantics = tester.ensureSemantics();
        try {
          final history = await GamePlayHistory.load(SharedPreferencesAsync());
          if (recent) await history.record(game.id);
          final app = fixtures.fixture(
            games: [game],
            history: history,
            accountApi: fixtures.FixtureAccountApi(),
          );
          await tester.pumpWidget(app);
          await tester.pumpAndSettle();
          final library = find.byType(LibraryPage);
          await tester.runAsync(
            () => precacheImage(
              AssetImage('assets/book-covers/${game.artwork}'),
              tester.element(library),
            ),
          );
          await tester.pumpAndSettle();
          Finder entry() => find.descendant(
            of: find.descendant(
              of: find.byType(LibraryPage),
              matching: recent
                  ? find.byKey(const PageStorageKey('recently played games'))
                  : find.byType(SliverGrid),
            ),
            matching: find.byKey(ValueKey(game.id)),
          );
          Finder ink() => find
              .descendant(of: entry(), matching: find.byType(InkWell))
              .first;
          if (!recent) {
            final favorite = tester.getSemantics(
              find.descendant(
                of: entry(),
                matching: find.byType(FavoriteButton),
              ),
            );
            expect(
              favorite.getSemanticsData().flagsCollection.isButton,
              isTrue,
            );
            favorite.owner!.performAction(favorite.id, SemanticsAction.tap);
            await tester.pumpAndSettle();
            expect(app.book.isFavorite(game), isTrue);
            expect(find.byType(GameDetail), findsNothing);
          }
          final node = tester.getSemantics(ink());
          final data = node.getSemanticsData();
          expect(data.label, contains(game.title));
          expect(data.label, contains(game.displayCode));
          expect(
            data.flagsCollection.isButton,
            isTrue,
            reason:
                'A game entry opens details and must be announced as a button',
          );
          expect(
            data.flagsCollection.isImage,
            isFalse,
            reason: 'The artwork is decoration for the labelled entry',
          );
          expect(data.hasAction(SemanticsAction.tap), isTrue);
          node.owner!.performAction(node.id, SemanticsAction.tap);
          await tester.pumpAndSettle();
          expect(find.byType(GameDetail), findsOneWidget);
          expect(
            tester.widget<GameDetail>(find.byType(GameDetail)).game.id,
            game.id,
          );
          await tester.binding.handlePopRoute();
          await tester.pumpAndSettle();
          expect(find.byType(LibraryPage), findsOneWidget);

          Focus.of(
            tester.element(
              find
                  .descendant(of: ink(), matching: find.byType(GameArtwork))
                  .first,
            ),
          ).requestFocus();
          await tester.pump();
          await tester.sendKeyEvent(LogicalKeyboardKey.enter);
          await tester.pumpAndSettle();
          expect(find.byType(GameDetail), findsOneWidget);
          expect(tester.takeException(), isNull);
        } finally {
          semantics.dispose();
        }
      }, variant: TargetPlatformVariant.only(TargetPlatform.windows));
    }
  }
}
