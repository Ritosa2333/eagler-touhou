import 'dart:io';

import 'package:flutter_test/flutter_test.dart';
import 'package:material_ui/material_ui.dart';
import 'package:shared_preferences/shared_preferences.dart';
import 'package:shared_preferences_platform_interface/in_memory_shared_preferences_async.dart';
import 'package:shared_preferences_platform_interface/shared_preferences_async_platform_interface.dart';
import 'package:touhouonline/catalog.dart';
import 'package:touhouonline/launcher_select.dart';
import 'package:touhouonline/l10n/strings.dart';
import 'package:touhouonline/library.dart';
import 'package:touhouonline/site_navigation.dart';

import '../test/widget_test.dart' as fixtures;
import 'capture_native_baseline_test.dart' show loadFonts, save;

void main() {
  TestWidgetsFlutterBinding.ensureInitialized();
  final location = Platform.environment['TOUHOU_LANGUAGE_CAPTURE'];
  if (location == null) return;
  final output = Directory(location);

  setUpAll(() async {
    await output.create(recursive: true);
    await loadFonts(Directory(Platform.environment['TOUHOU_CAPTURE_FONTS']!));
  });

  for (final (name, width, height) in [
    ('desktop', 1440.0, 734.0),
    ('mobile', 390.0, 844.0),
    ('mobile-narrow', 320.0, 640.0),
  ]) {
    testWidgets('Compare selected language at $name', (tester) async {
      tester.view.devicePixelRatio = 1;
      tester.view.physicalSize = Size(width, height);
      addTearDown(tester.view.resetDevicePixelRatio);
      addTearDown(tester.view.resetPhysicalSize);
      SharedPreferencesAsyncPlatform.instance =
          InMemorySharedPreferencesAsync.empty();

      final games = (await tester.runAsync(Game.load))!;
      final preferences = LauncherPreferences(
        SharedPreferencesAsync(),
        ThemeMode.dark,
        true,
      );
      final painting = GlobalKey();
      await tester.pumpWidget(
        RepaintBoundary(
          key: painting,
          child: fixtures.fixture(
            games: games,
            preferences: preferences,
            accountApi: fixtures.FixtureAccountApi(),
          ),
        ),
      );
      await tester.pumpAndSettle();
      final library = tester.element(find.byType(LibraryPage));
      await tester.runAsync(() async {
        for (final game in games) {
          if (game.artwork case final String cover) {
            await precacheImage(
              AssetImage('assets/book-covers/$cover'),
              library,
            );
          }
        }
      });
      await tester.pumpAndSettle();

      Future<void> openLanguages() async {
        final strings = library.strings;
        final desktop = name == 'desktop';
        await tester.tap(
          desktop
              ? find.byTooltip(strings.siteMenu)
              : find.byType(DrawerButton),
        );
        await tester.pumpAndSettle();
        final entry = desktop
            ? find.text(strings.language)
            : find.descendant(
                of: find.byType(SiteDrawer),
                matching: find.text(strings.language),
              );
        await tester.ensureVisible(entry);
        await tester.pumpAndSettle();
        expect(entry.hitTestable(), findsOneWidget);
        await tester.tap(entry);
        await tester.pumpAndSettle();
      }

      Future<void> choose(LauncherLanguage language) async {
        await tester.tap(find.byType(LauncherSelect<LauncherLanguage>));
        await tester.pumpAndSettle();
        await tester.tap(find.text(language.label).last);
        await tester.pumpAndSettle();
        if (name == 'desktop') {
          await tester.tap(
            find.widgetWithText(TextButton, library.strings.close),
          );
        } else {
          await tester.tap(find.byTooltip(library.strings.closeMenu));
        }
        await tester.pumpAndSettle();
        expect(preferences.language, language);
        expect(tester.takeException(), isNull);
      }

      if (name != 'desktop') {
        await tester.tap(find.text(library.strings.home));
        await tester.pumpAndSettle();
        await save(tester, painting, output, '$name-home-guest');
        await tester.tap(find.text(library.strings.launcherSettings));
        await tester.pumpAndSettle();
        expect(find.byType(LauncherSelect<LauncherLanguage>), findsOneWidget);
        await save(tester, painting, output, '$name-home-language');
        await tester.tap(find.text(library.strings.singlePlayer));
        await tester.pumpAndSettle();
      }
      await openLanguages();
      await save(tester, painting, output, '$name-interface-language');
      await choose(LauncherLanguage.english);
      await save(tester, painting, output, '$name-library-en');
      await openLanguages();
      await choose(LauncherLanguage.japanese);
      await save(tester, painting, output, '$name-library-ja');
    }, variant: TargetPlatformVariant.only(TargetPlatform.windows));
  }
}
