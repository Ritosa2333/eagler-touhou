import 'dart:io';
import 'dart:ui' show PointerDeviceKind;

import 'package:flutter/services.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:material_ui/material_ui.dart';
import 'package:shared_preferences_platform_interface/in_memory_shared_preferences_async.dart';
import 'package:shared_preferences_platform_interface/shared_preferences_async_platform_interface.dart';
import 'package:touhouonline/catalog.dart';
import 'package:touhouonline/launcher_select.dart';
import 'package:touhouonline/library.dart';
import 'package:touhouonline/l10n/strings.dart';

import '../tools/capture_native_baseline_test.dart' show loadFonts, save;
import 'widget_test.dart' as fixtures;

Finder _in(Finder area, Finder widget) =>
    find.descendant(of: area, matching: widget);

Finder _favorite(Finder area, String id) => _in(
  area,
  find.byWidgetPredicate((w) => w is FavoriteButton && w.game.id == id),
);

Future<void> _reverse(WidgetTester tester) async {
  await tester.tap(find.byType(LauncherSelect<GameOrder>));
  await tester.pumpAndSettle();
  final strings = tester.element(find.byType(LibraryPage)).strings;
  await tester.tap(find.text(GameOrder.reverse.label(strings)).last);
  await tester.pump();
  // MenuItemButton runs its action after restoring focus at the end of the frame.
  await tester.pump();
}

void main() {
  TestWidgetsFlutterBinding.ensureInitialized();
  late List<Game> catalog;
  late List<Game> games;
  final output = Platform.environment['TOUHOU_KEYBOARD_CAPTURE'];

  setUpAll(() async {
    catalog = await Game.load();
    games = catalog
        .where((game) => {'th06', 'th07', 'th08', 'th10'}.contains(game.id))
        .toList();
    expect(games, hasLength(4));
    if (output != null) {
      await loadFonts(Directory(Platform.environment['TOUHOU_CAPTURE_FONTS']!));
    }
  });

  setUp(() {
    SharedPreferencesAsyncPlatform.instance =
        InMemorySharedPreferencesAsync.empty();
  });

  for (final reduced in [false, true]) {
    testWidgets(
      'Sort menu handles arrow keys, Enter and Escape | reduced=$reduced',
      (tester) async {
        tester.view.devicePixelRatio = 1;
        tester.view.physicalSize = const Size(1440, 734);
        addTearDown(tester.view.resetDevicePixelRatio);
        addTearDown(tester.view.resetPhysicalSize);
        final app = fixtures.fixture(games: catalog);
        final boundary = GlobalKey();
        await app.preferences.setLessMotion(reduced);
        await tester.pumpWidget(RepaintBoundary(key: boundary, child: app));
        await tester.pumpAndSettle();
        if (output != null && !reduced) {
          final context = tester.element(find.byType(LibraryPage));
          await tester.runAsync(() async {
            for (final game in catalog) {
              if (game.artwork case final String cover) {
                await precacheImage(
                  AssetImage('assets/book-covers/$cover'),
                  context,
                );
              }
            }
            await precacheImage(
              const AssetImage('assets/launcher-wordmark-dark.png'),
              context,
            );
          });
          await tester.pumpAndSettle();
        }

        final select = find.byType(LauncherSelect<GameOrder>);
        final button = find.descendant(
          of: select,
          matching: find.byType(TextButton),
        );
        final focus = tester.widget<TextButton>(button).focusNode!;
        focus.requestFocus();
        await tester.pump();
        final strings = tester.element(select).strings;

        Future<void> press(LogicalKeyboardKey key) async {
          await tester.sendKeyEvent(key);
          await tester.pumpAndSettle();
        }

        Finder item(GameOrder order) =>
            find.widgetWithText(MenuItemButton, order.label(strings));
        bool focused(GameOrder order) {
          final label = find.descendant(
            of: item(order),
            matching: find.text(order.label(strings)),
          );
          return Focus.of(tester.element(label)).hasFocus;
        }

        await press(LogicalKeyboardKey.enter);
        expect(item(GameOrder.number), findsOneWidget);
        expect(focused(GameOrder.number), isTrue);
        await press(LogicalKeyboardKey.arrowDown);
        expect(focused(GameOrder.reverse), isTrue);
        await press(LogicalKeyboardKey.enter);
        expect(app.book.order, GameOrder.reverse);
        expect(item(GameOrder.reverse), findsNothing);
        expect(focus.hasPrimaryFocus, isTrue);

        await press(LogicalKeyboardKey.arrowDown);
        expect(item(GameOrder.reverse), findsOneWidget);
        expect(focused(GameOrder.reverse), isTrue);
        if (output != null && !reduced) {
          await save(
            tester,
            boundary,
            Directory(output)..createSync(recursive: true),
            'desktop-library-sort-keyboard',
          );
        }
        await press(LogicalKeyboardKey.escape);
        expect(app.book.order, GameOrder.reverse);
        expect(item(GameOrder.reverse), findsNothing);
        expect(focus.hasPrimaryFocus, isTrue);
      },
    );
  }

  for (final (view, reduced) in [
    ('desktop-list', false),
    ('desktop-cover', false),
    ('mobile-cover', false),
    ('app-reduced', true),
    ('system-reduced', true),
  ]) {
    testWidgets(
      'Work and favorite move together during reverse order | $view',
      (tester) async {
        final desktop = view.startsWith('desktop');
        tester.view.devicePixelRatio = 1;
        tester.view.physicalSize = desktop
            ? const Size(1100, 820)
            : Size(view == 'mobile-cover' ? 390 : 320, 720);
        addTearDown(tester.view.resetDevicePixelRatio);
        addTearDown(tester.view.resetPhysicalSize);
        if (view == 'system-reduced') {
          tester.platformDispatcher.accessibilityFeaturesTestValue =
              const FakeAccessibilityFeatures(disableAnimations: true);
          addTearDown(
            tester.platformDispatcher.clearAccessibilityFeaturesTestValue,
          );
        }

        final app = fixtures.fixture(games: games);
        await app.preferences.setLessMotion(view == 'app-reduced');
        await tester.pumpWidget(app);
        await tester.pumpAndSettle();
        if (view == 'desktop-list' || reduced) {
          await tester.tap(find.text('列表'));
          await tester.pumpAndSettle();
        }

        final library = find.byType(LibraryPage);
        final side = find.byType(BookSidebar);
        final areas = [library, if (desktop) side];
        final start = [
          for (final area in areas) tester.getCenter(_favorite(area, 'th07')),
        ];
        final otherStart = [
          for (final area in areas) tester.getCenter(_favorite(area, 'th08')),
        ];

        if (desktop) {
          final row = _in(
            side,
            find.byKey(const ValueKey((BookCategory.windows, 'th07'))),
          );
          final hoveredStar = _favorite(side, 'th07');
          final pointer = await tester.createGesture(
            kind: PointerDeviceKind.mouse,
          );
          await pointer.addPointer(location: const Offset(1000, 600));
          await pointer.moveTo(tester.getCenter(row));
          await tester.pump();
          expect(
            tester.widget<FavoriteButton>(hoveredStar).tileHovered,
            isTrue,
          );
          await pointer.moveTo(tester.getCenter(hoveredStar));
          await tester.pump();
          expect(
            tester.widget<FavoriteButton>(hoveredStar).tileHovered,
            isTrue,
          );
          await pointer.removePointer();
          await tester.pumpAndSettle();
        }

        await _reverse(tester);
        await tester.pump(const Duration(milliseconds: 40));

        for (final (index, area) in areas.indexed) {
          final star = _favorite(area, 'th07');
          final distance = (tester.getCenter(star) - start[index]).distance;
          final total = (otherStart[index] - start[index]).distance;
          expect(total, greaterThan(1));
          if (reduced) {
            expect(distance, closeTo(total, .01));
          } else {
            expect(distance, greaterThan(1));
            expect(distance, lessThan(total));
            if (area == side || view == 'desktop-list') {
              final row = find.ancestor(
                of: star,
                matching: find.byType(GameRow),
              );
              expect(
                tester.widget<GameRow>(row).movement.value,
                isNot(Offset.zero),
              );
            }
          }
        }

        await tester.pumpAndSettle();
        for (final (index, area) in areas.indexed) {
          expect(tester.getCenter(_favorite(area, 'th07')), otherStart[index]);
          expect(tester.getCenter(_favorite(area, 'th08')), start[index]);
        }

        await tester.ensureVisible(_favorite(library, 'th07'));
        await tester.pumpAndSettle();
        expect(_favorite(library, 'th07').hitTestable(), findsOneWidget);
        await tester.tap(_favorite(library, 'th07'));
        await tester.pumpAndSettle();
        expect(
          tester
              .widget<FavoriteButton>(_favorite(library, 'th07'))
              .book
              .favorites
              .ids,
          {'th07'},
        );
        expect(
          tester.getCenter(_favorite(library, 'th07')).dy,
          lessThanOrEqualTo(tester.getCenter(_favorite(library, 'th08')).dy),
        );
      },
    );
  }
}
