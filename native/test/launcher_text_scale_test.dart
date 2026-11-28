import 'dart:convert';
import 'dart:io';

import 'package:flutter/rendering.dart';
import 'package:flutter/services.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:material_ui/material_ui.dart';
import 'package:shared_preferences/shared_preferences.dart';
import 'package:shared_preferences_platform_interface/in_memory_shared_preferences_async.dart';
import 'package:shared_preferences_platform_interface/shared_preferences_async_platform_interface.dart';
import 'package:touhouonline/catalog.dart';
import 'package:touhouonline/game_detail.dart';
import 'package:touhouonline/game_settings.dart';
import 'package:touhouonline/l10n/strings.dart';
import 'package:touhouonline/launcher_select.dart';
import 'package:touhouonline/library.dart';

import '../tools/capture_native_baseline_test.dart' show loadFonts, save;
import 'widget_test.dart' as fixtures;

Future<void> _tap(WidgetTester tester, Finder target) async {
  await tester.ensureVisible(target);
  await tester.pumpAndSettle();
  expect(target.hitTestable(), findsOneWidget);
  await tester.tap(target);
  await tester.pumpAndSettle();
}

void _wholeWord(WidgetTester tester, String label, String word) {
  final paragraph = tester.renderObject<RenderParagraph>(find.text(label));
  final start = label.indexOf(word);
  expect(start, greaterThanOrEqualTo(0));
  expect(
    paragraph.getBoxesForSelection(
      TextSelection(baseOffset: start, extentOffset: start + word.length),
    ),
    hasLength(1),
    reason: '$word must fit on one line',
  );
}

void main() {
  TestWidgetsFlutterBinding.ensureInitialized();
  final output = Platform.environment['TOUHOU_TEXT_SCALE_CAPTURE'];
  late List<Game> games;
  setUpAll(() async {
    games = await Game.load();
    if (output != null) {
      await loadFonts(Directory(Platform.environment['TOUHOU_CAPTURE_FONTS']!));
    } else {
      // Word wrapping needs real glyph widths, not the square Ahem test font.
      final config = File('.dart_tool/package_config.json');
      final packages =
          jsonDecode(config.readAsStringSync())['packages'] as List;
      final flutter = packages.singleWhere(
        (dynamic item) => item['name'] == 'flutter',
      );
      final sdk = Directory.fromUri(
        config.absolute.uri.resolve(flutter['rootUri'] as String),
      ).parent.parent;
      final fonts = Directory('${sdk.path}/bin/cache/artifacts/material_fonts');
      if (Platform.isWindows) {
        await loadFonts(fonts);
        return;
      }
      final roboto = FontLoader('Roboto');
      for (final name in ['regular', 'medium', 'bold']) {
        roboto.addFont(
          File('${fonts.path}/roboto-$name.ttf')
              .readAsBytes()
              .then(ByteData.sublistView),
        );
      }
      await roboto.load();
    }
  });
  setUp(() {
    SharedPreferencesAsyncPlatform.instance =
        InMemorySharedPreferencesAsync.empty();
  });

  for (final (name, size, language) in [
    ('narrow-en', const Size(320, 640), LauncherLanguage.english),
    ('mobile-zh', const Size(390, 844), LauncherLanguage.simplifiedChinese),
    ('mobile-ja', const Size(390, 844), LauncherLanguage.japanese),
    ('desktop-en', const Size(1440, 734), LauncherLanguage.english),
  ]) {
    for (final scale in [1.5, 2.0]) {
      testWidgets('Core navigation at large text | $name $scale', (
        tester,
      ) async {
        tester.view.devicePixelRatio = 1;
        tester.view.physicalSize = size;
        tester.platformDispatcher.textScaleFactorTestValue = scale;
        addTearDown(tester.view.resetDevicePixelRatio);
        addTearDown(tester.view.resetPhysicalSize);
        addTearDown(tester.platformDispatcher.clearTextScaleFactorTestValue);
        final preferences = LauncherPreferences(
          SharedPreferencesAsync(),
          ThemeMode.dark,
          true,
          language: language,
        );
        final strings = lookupLauncherStrings(language.locale);
        final boundary = GlobalKey();
        await tester.pumpWidget(
          RepaintBoundary(
            key: boundary,
            child: fixtures.fixture(
              games: games,
              preferences: preferences,
              accountApi: fixtures.FixtureAccountApi(),
            ),
          ),
        );
        await tester.pumpAndSettle();
        Future<void> capture(String scene) async {
          if (output == null) return;
          final context = tester.element(find.byType(Scaffold).last);
          await tester.runAsync(() async {
            for (final game in games) {
              if (game.artwork != null) {
                await precacheImage(
                  AssetImage('assets/book-covers/${game.artwork}'),
                  context,
                );
              }
            }
          });
          await tester.pumpAndSettle();
          await save(
            tester,
            boundary,
            Directory(output),
            '$name-$scale-$scene',
          );
        }

        await capture('library');
        expect(tester.takeException(), isNull);
        await _tap(tester, find.byType(LauncherSelect<GameOrder>));
        await _tap(tester, find.text(GameOrder.reverse.label(strings)).last);
        await tester.enterText(find.byType(SearchBar), 'th06');
        await tester.testTextInput.receiveAction(TextInputAction.done);
        await tester.pumpAndSettle();
        await _tap(
          tester,
          find.descendant(
            of: find.byType(LibraryPage),
            matching: find.byType(GameArtwork),
          ),
        );
        expect(find.byType(GameDetail), findsOneWidget);
        await capture('detail');
        expect(tester.takeException(), isNull);
        final settingsLabel = find.widgetWithText(
          OutlinedButton,
          strings.gameSettings,
        );
        await _tap(
          tester,
          settingsLabel.evaluate().isEmpty
              ? find.byTooltip(strings.gameSettings)
              : settingsLabel,
        );
        expect(find.byType(GameSettingsPage), findsOneWidget);
        if (size.width > 780) {
          await _tap(tester, find.text(strings.gameDisplaySettings));
        }
        await capture('settings');
        expect(tester.takeException(), isNull);
        await tester.ensureVisible(find.text(strings.gameLanguage));
        await tester.pumpAndSettle();
        await capture('settings-language');
        await _tap(tester, find.byKey(const ValueKey('language')));
        await _tap(tester, find.text('日本語').last);
        await _tap(tester, find.byType(BackButton));
        expect(find.byType(GameDetail), findsOneWidget);
        await _tap(tester, find.byType(BackButton));
        expect(find.byType(GameDetail), findsNothing);
        expect(find.byType(LibraryPage), findsOneWidget);
        expect(tester.takeException(), isNull);
        if (size.width <= 780) {
          Finder tab(String label) => find.descendant(
            of: find.byType(NavigationBar),
            matching: find.text(label),
          );
          await _tap(tester, tab(strings.home));
          await _tap(tester, find.text(strings.launcherSettings));
          await capture('home');
          expect(tester.takeException(), isNull);
          if (language == LauncherLanguage.english) {
            _wholeWord(tester, strings.language, 'Interface');
            _wholeWord(tester, strings.language, 'language');
          }
          await _tap(tester, find.byType(LauncherSelect<LauncherLanguage>));
          await _tap(tester, find.text(language.label).last);
          await _tap(tester, find.text(strings.launcherSettings));
          await _tap(tester, find.text(strings.themeSettings));
          await tester.ensureVisible(
            find.byKey(const ValueKey('theme base segments')),
          );
          await tester.pumpAndSettle();
          final themeSelector = find.byType(LauncherSelect<ThemeMode>);
          await _tap(tester, themeSelector);
          for (final label in [
            strings.systemTheme,
            strings.lightTheme,
            strings.darkTheme,
          ]) {
            final paragraph = tester.renderObject<RenderParagraph>(
              find.text(label).last,
            );
            expect(paragraph.didExceedMaxLines, isFalse);
          }
          await capture('theme-options');
          await _tap(tester, find.text(strings.systemTheme).last);
          expect(preferences.theme, ThemeMode.system);
          await _tap(tester, themeSelector);
          await _tap(tester, find.text(strings.lightTheme).last);
          expect(preferences.theme, ThemeMode.light);
          await _tap(tester, themeSelector);
          await capture('theme-menu-near-bottom');
          await _tap(tester, find.text(strings.darkTheme).last);
          expect(preferences.theme, ThemeMode.dark);
          await capture('theme');
          expect(tester.takeException(), isNull);
          await _tap(tester, tab(strings.multiplayer));
          await capture('lobby');
          expect(tester.takeException(), isNull);
          if (language == LauncherLanguage.english) {
            _wholeWord(tester, strings.lobbyOffline, 'unavailable');
          }
          await _tap(tester, tab(strings.singlePlayer));
          expect(find.byType(LibraryPage), findsOneWidget);
        }
        await tester.pumpWidget(const SizedBox.shrink());
        await tester.pumpAndSettle();
      }, variant: TargetPlatformVariant.only(TargetPlatform.windows));
    }
  }
}
