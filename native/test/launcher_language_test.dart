import 'dart:io';
import 'dart:convert';

import 'package:flutter_test/flutter_test.dart';
import 'package:flutter/rendering.dart' show RenderParagraph;
import 'package:material_ui/material_ui.dart';
import 'package:shared_preferences/shared_preferences.dart';
import 'package:shared_preferences_platform_interface/in_memory_shared_preferences_async.dart';
import 'package:shared_preferences_platform_interface/shared_preferences_async_platform_interface.dart';
import 'package:touhouonline/catalog.dart';
import 'package:touhouonline/launcher_select.dart';
import 'package:touhouonline/game_detail.dart';
import 'package:touhouonline/game_preferences.dart';
import 'package:touhouonline/game_settings.dart';
import 'package:touhouonline/game_user_files.dart';
import 'package:touhouonline/l10n/strings.dart';
import 'package:touhouonline/launcher_language.dart';
import 'package:touhouonline/library.dart';
import 'package:touhouonline/site_navigation.dart';

import '../tools/capture_native_baseline_test.dart' show loadFonts, save;
import 'widget_test.dart' as fixtures;

final _selector = find.byType(LauncherSelect<LauncherLanguage>);

Future<void> _tap(WidgetTester tester, Finder finder) async {
  await tester.ensureVisible(finder);
  await tester.pumpAndSettle();
  expect(finder.hitTestable(), findsOneWidget);
  await tester.tap(finder);
  await tester.pumpAndSettle();
}

Finder _tab(String text) =>
    find.descendant(of: find.byType(NavigationBar), matching: find.text(text));

Future<void> _language(WidgetTester tester, LauncherLanguage language) async {
  await _tap(tester, _selector);
  await _tap(tester, find.text(language.label).last);
}

void _size(WidgetTester tester, Size size) {
  tester.view.devicePixelRatio = 1;
  tester.view.physicalSize = size;
  addTearDown(tester.view.resetDevicePixelRatio);
  addTearDown(tester.view.resetPhysicalSize);
}

void main() {
  TestWidgetsFlutterBinding.ensureInitialized();
  final output = Platform.environment['TOUHOU_GAME_SETTINGS_CAPTURE'];
  late Game game;
  late List<Game> catalog;
  setUpAll(() async {
    catalog = await Game.load();
    game = catalog.firstWhere((game) => game.id == 'th06');
    if (Platform.isWindows || output != null) {
      final path = Platform.environment['TOUHOU_CAPTURE_FONTS'];
      await loadFonts(path == null ? null : Directory(path));
    }
  });

  setUp(() {
    SharedPreferencesAsyncPlatform.instance =
        InMemorySharedPreferencesAsync.empty();
  });

  testWidgets(
    'Mobile language switches in place and survives restart and system locale',
    (tester) async {
      _size(tester, const Size(320, 640));
      tester.platformDispatcher.localeTestValue = const Locale('en', 'US');
      addTearDown(tester.platformDispatcher.clearLocaleTestValue);
      final storage = SharedPreferencesAsync();
      final prefs = await LauncherPreferences.load(storage);
      expect(prefs.language, LauncherLanguage.simplifiedChinese);
      final app = fixtures.fixture(preferences: prefs);
      await tester.pumpWidget(app);
      await tester.pumpAndSettle();
      final search = find.byType(SearchBar);
      await tester.enterText(search, 'th06');
      await tester.testTextInput.receiveAction(TextInputAction.done);
      await tester.pumpAndSettle();

      await _tap(tester, find.byType(DrawerButton));
      final drawer = find.byType(SiteDrawer);
      await _tap(
        tester,
        find.descendant(of: drawer, matching: find.text('界面语言')),
      );
      expect(find.byType(LauncherLanguageRow), findsOneWidget);
      await _language(tester, LauncherLanguage.english);
      expect(find.text('Interface language'), findsNWidgets(2));
      await tester.binding.handlePopRoute();
      await tester.pumpAndSettle();
      expect(find.byType(LauncherLanguageRow), findsNothing);
      expect(find.byTooltip('Close menu').hitTestable(), findsOneWidget);

      await _tap(tester, find.byTooltip('Close menu'));
      expect(tester.widget<SearchBar>(search).controller!.text, 'th06');
      expect(tester.widget<SearchBar>(search).hintText, 'Search');
      await _tap(tester, _tab('Home'));
      await _tap(tester, find.text('Launcher settings'));
      final field = tester.getRect(_selector);
      expect(field.width, lessThan(150));
      await _language(tester, LauncherLanguage.japanese);
      expect(find.text('ランチャー設定'), findsOneWidget);
      expect(_tab('ホーム'), findsOneWidget);
      await _tap(tester, _tab('マルチプレイ'));
      expect(find.byTooltip('部屋を作成'), findsOneWidget);
      await _tap(tester, _tab('シングルプレイ'));
      expect(tester.widget<SearchBar>(search).controller!.text, 'th06');
      expect(tester.widget<SearchBar>(search).hintText, '作品を検索');
      expect(tester.takeException(), isNull);

      final restarted = await LauncherPreferences.load(storage);
      expect(restarted.language, LauncherLanguage.japanese);
      await tester.pumpWidget(const SizedBox.shrink());
      await tester.pumpAndSettle();
      await tester.pumpWidget(fixtures.fixture(preferences: restarted));
      await tester.pumpAndSettle();
      expect(_tab('シングルプレイ'), findsOneWidget);
    },
  );

  testWidgets('Desktop language menu returns to the same game details', (
    tester,
  ) async {
    _size(tester, const Size(1100, 760));
    final prefs = await LauncherPreferences.load(SharedPreferencesAsync());
    await tester.pumpWidget(fixtures.fixture(preferences: prefs));
    await tester.pumpAndSettle();
    await _tap(
      tester,
      find.descendant(
        of: find.byType(LibraryPage),
        matching: find.byType(GameArtwork),
      ),
    );
    expect(find.text('开始游戏'), findsOneWidget);
    await _tap(tester, find.byTooltip('站点菜单'));
    await _tap(tester, find.text('界面语言'));
    await _language(tester, LauncherLanguage.english);

    await _tap(tester, find.widgetWithText(TextButton, 'Close'));
    expect(find.byType(GameDetail), findsOneWidget);
    expect(find.text('Start game'), findsOneWidget);
    expect(find.text('Game settings'), findsOneWidget);
    await _tap(tester, find.byType(BackButton));
    expect(find.text('All games'), findsNWidgets(2));
    expect(tester.takeException(), isNull);
  });

  for (final (device, size, language, locale) in [
    ('mobile-narrow', const Size(320, 640), LauncherLanguage.english, 'en'),
    ('mobile', const Size(390, 844), LauncherLanguage.japanese, 'ja'),
    ('desktop', const Size(1440, 734), LauncherLanguage.english, 'en'),
    (
      'mobile',
      const Size(390, 844),
      LauncherLanguage.simplifiedChinese,
      'zh-CN',
    ),
    (
      'desktop',
      const Size(1440, 734),
      LauncherLanguage.simplifiedChinese,
      'zh-CN',
    ),
  ]) {
    testWidgets('Game settings use the interface locale | $device $locale', (
      tester,
    ) async {
      _size(tester, size);
      final strings = lookupLauncherStrings(language.locale);
      final boundary = GlobalKey();
      final storage = SharedPreferencesAsync();
      final store = GamePreferenceStore(storage);
      final selectedKinds = <List<String>>[];
      await tester.pumpWidget(
        RepaintBoundary(
          key: boundary,
          child: fixtures.fixture(
            preferences: LauncherPreferences(
              storage,
              ThemeMode.dark,
              true,
              language: language,
            ),
            games: catalog,
            gamePreferences: store,
            gameUserFiles: GameUserFiles(
              picker: (extensions) async {
                selectedKinds.add(extensions);
                return null;
              },
            ),
          ),
        ),
      );
      await tester.pumpAndSettle();
      await _tap(
        tester,
        find.descendant(
          of: find.byType(LibraryPage),
          matching: find.byKey(const ValueKey('th06')),
        ),
      );
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
      final settings = find.byType(GameSettingsPage);
      expect(settings, findsOneWidget);
      for (final heading in [
        strings.gameDisplaySettings,
        strings.gameInputSettings,
        strings.gameTouchSettings,
        strings.gameVolumeSettings,
        strings.gameStorageSettings,
        strings.gameAdvancedSettings,
      ]) {
        expect(find.text(heading), findsOneWidget);
      }
      if (size.width > 780) {
        await _tap(tester, find.text(strings.gameDisplaySettings));
      }

      Future<void> capture(String scene) async {
        if (output == null) return;
        final context = tester.element(settings);
        await tester.runAsync(() async {
          for (final item in catalog) {
            if (item.artwork case final String cover) {
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
        await save(
          tester,
          boundary,
          Directory(output),
          '$device-$scene-$locale',
        );
      }

      final lang = find.byKey(const ValueKey('language'));
      expect(
        find.descendant(of: lang, matching: find.text('中文（简体）')),
        findsOneWidget,
      );
      await capture('game-settings-display');
      await _tap(tester, find.text(strings.gameHighRefresh));
      final prefs = await store.load(game, multiplayer: false);
      expect(prefs.enabled(GameToggle.frameLimit60Enabled), isFalse);
      await _tap(tester, lang);
      await _tap(tester, find.text('日本語').last);
      expect(
        (await store.load(game, multiplayer: false)).values['language'],
        'ja',
      );
      expect(find.text(strings.gameLanguage), findsOneWidget);
      await _tap(tester, lang);
      await _tap(tester, find.text('中文（简体）').last);
      await _tap(tester, find.text(strings.gameHighRefresh));

      await _tap(tester, find.text(strings.gameDisplaySettings));
      await _tap(tester, find.text(strings.gameInputSettings));
      await capture('game-settings-input');
      await _tap(tester, find.text(strings.gameInputSettings));
      await _tap(tester, find.text(strings.gameVolumeSettings));
      final music = find.text(strings.gameMusicVolume);
      await tester.ensureVisible(music);
      await tester.pumpAndSettle();
      expect(music.hitTestable(), findsOneWidget);
      await capture('game-settings-volume');
      await _tap(tester, find.text(strings.gameVolumeSettings));
      await _tap(tester, find.text(strings.gameAdvancedSettings));
      await capture('game-settings-advanced');
      await _tap(tester, find.text(strings.gameAdvancedSettings));
      await _tap(tester, find.text(strings.gameStorageSettings));
      final download = find.byKey(const ValueKey((GameUserFileKind.save, 0)));
      final upload = find.byKey(const ValueKey((GameUserFileKind.save, 1)));
      expect(tester.getCenter(download).dy, tester.getCenter(upload).dy);
      if (tester.widget(upload) is IconButton) {
        expect(
          tester.widget<IconButton>(upload).tooltip,
          '${strings.gameSave} | ${strings.gameUpload}',
        );
      } else {
        expect(
          find.descendant(of: upload, matching: find.text(strings.gameUpload)),
          findsOneWidget,
        );
      }
      final manage = find.byKey(const ValueKey((GameUserFileKind.replay, 1)));
      await tester.ensureVisible(manage);
      await tester.pumpAndSettle();
      await capture('game-files');
      await _tap(tester, upload);
      expect(selectedKinds, isEmpty);
      expect(find.text(strings.gameSaveOverwrite), findsOneWidget);
      await _tap(tester, find.text(strings.gameContinueImport));
      expect(selectedKinds.single, contains('dat'));
      expect(settings, findsOneWidget);
      await _tap(tester, find.byType(BackButton));
      expect(find.byType(GameDetail), findsOneWidget);
      expect(find.text(strings.startGame), findsOneWidget);
      expect(tester.takeException(), isNull);
    }, variant: TargetPlatformVariant.only(TargetPlatform.windows));
  }

  for (final (width, language) in [
    (320.0, LauncherLanguage.english),
    (390.0, LauncherLanguage.japanese),
    (390.0, LauncherLanguage.simplifiedChinese),
    (781.0, LauncherLanguage.english),
    (1440.0, LauncherLanguage.english),
    (1440.0, LauncherLanguage.simplifiedChinese),
  ]) {
    for (final light in [false, true]) {
      for (final scale in [1.0, 2.0]) {
        final name =
            '${width.toInt()}-${language.name}-${light ? 'light' : 'dark'}-${scale.toInt()}';
        testWidgets('Game settings layout | $name', (tester) async {
          _size(tester, Size(width, 844));
          tester.platformDispatcher.textScaleFactorTestValue = scale;
          addTearDown(tester.platformDispatcher.clearTextScaleFactorTestValue);
          final storage = SharedPreferencesAsync();
          final store = GamePreferenceStore(storage);
          final boundary = GlobalKey();
          final strings = lookupLauncherStrings(language.locale);
          await tester.pumpWidget(
            RepaintBoundary(
              key: boundary,
              child: fixtures.fixture(
                games: catalog,
                gamePreferences: store,
                accountApi: fixtures.FixtureAccountApi(),
                preferences: LauncherPreferences(
                  storage,
                  light ? ThemeMode.light : ThemeMode.dark,
                  true,
                  language: language,
                ),
              ),
            ),
          );
          await tester.pumpAndSettle();
          await _tap(
            tester,
            find.descendant(
              of: find.byType(LibraryPage),
              matching: find.byKey(const ValueKey('th06')),
            ),
          );
          final detailScroll = tester
              .state<ScrollableState>(
                find
                    .descendant(
                      of: find.byType(GameDetail),
                      matching: find.byType(Scrollable),
                    )
                    .first,
              )
              .position;
          final entry = find.widgetWithText(
            OutlinedButton,
            strings.gameSettings,
          );
          final settingsAction = entry.evaluate().isEmpty
              ? find.byTooltip(strings.gameSettings)
              : entry;
          await tester.ensureVisible(settingsAction);
          await tester.pumpAndSettle();
          final detailOffset = detailScroll.pixels;
          await _tap(tester, settingsAction);
          if (width <= 780) {
            await _tap(tester, find.text(strings.gameDisplaySettings));
          }
          final output = Platform.environment['TOUHOU_SETTINGS_LAYOUT_CAPTURE'];
          if (output != null) {
            final context = tester.element(find.byType(GameSettingsPage));
            await tester.runAsync(() async {
              for (final item in catalog) {
                if (item.artwork case final String cover) {
                  await precacheImage(
                    AssetImage('assets/book-covers/$cover'),
                    context,
                  );
                }
              }
              for (final theme in ['dark', 'light']) {
                await precacheImage(
                  AssetImage('assets/launcher-wordmark-$theme.png'),
                  context,
                );
              }
            });
            await tester.pumpAndSettle();
          }
          final issues = <String>[];
          final geometry = <String, Object>{};
          List<double> box(Finder target) {
            final r = tester.getRect(target);
            return [r.left, r.top, r.width, r.height];
          }

          Future<void> capture(String scene) async {
            if (output != null) {
              await save(tester, boundary, Directory(output), '$name-$scene');
            }
          }

          await _tap(tester, find.text(strings.gameInputSettings));
          final input = find.byKey(const PageStorageKey('input'));
          final keyboard = find.descendant(
            of: input,
            matching: find.text(strings.gameKeyboard),
          );
          final gamepad = find.descendant(
            of: input,
            matching: find.text(strings.gameGamepad),
          );
          geometry['keyboard'] = box(keyboard);
          geometry['gamepad'] = box(gamepad);
          final inline =
              (tester.getCenter(keyboard).dy - tester.getCenter(gamepad).dy)
                  .abs() <
              .5;
          if (inline != (width == 1440)) {
            issues.add(
              'input arrangement differs from the wide/narrow reference',
            );
          }
          await capture('input');
          await _tap(tester, find.text(strings.gameInputSettings));
          await _tap(tester, find.text(strings.gameVolumeSettings));
          final volume = find.byKey(const PageStorageKey('volume'));
          final amounts = find.descendant(
            of: volume,
            matching: find.text('100%'),
          );
          expect(amounts, findsNWidgets(3));
          final values = amounts
              .evaluate()
              .map((e) => find.byElementPredicate((v) => identical(v, e)))
              .toList();
          geometry['percentages'] = values.map(box).toList();
          for (final number in values) {
            final p = tester.renderObject<RenderParagraph>(number);
            final boxes = p.getBoxesForSelection(
              const TextSelection(baseOffset: 0, extentOffset: 4),
            );
            if (boxes.length != 1 || p.didExceedMaxLines) {
              issues.add('100% is split or clipped');
            }
          }
          if (width <= 390) {
            final label = tester.getRect(
              find.descendant(
                of: volume,
                matching: find.text(strings.gameMusicVolume),
              ),
            );
            final value = tester.getRect(values.first);
            if ((label.center.dy - value.center.dy).abs() < .5) {
              expect(value.left - label.right, greaterThanOrEqualTo(8));
            } else {
              expect(label.bottom, lessThanOrEqualTo(value.top));
            }
          }
          final rights = values.map((f) => tester.getRect(f).right).toList();
          expect(
            rights.every((value) => (value - rights.first).abs() < .5),
            isTrue,
          );
          await capture('volume');
          final slider = find
              .descendant(of: volume, matching: find.byType(Slider))
              .first;
          await tester.ensureVisible(slider);
          await tester.pumpAndSettle();
          final oldWidth = tester.getSize(values.first).width;
          final oldRight = tester.getRect(values.first).right;
          await tester.tap(slider);
          await tester.pumpAndSettle();
          final changed = find
              .descendant(of: volume, matching: find.textContaining('%'))
              .first;
          expect(tester.getRect(changed).right, closeTo(oldRight, .5));
          expect(tester.getSize(changed).width, oldWidth);
          final persisted = await store.load(game, multiplayer: false);
          expect(persisted.values['bgmVolume'], inInclusiveRange(40, 60));
          await tester.tap(find.byType(BackButton));
          await tester.pumpAndSettle();
          expect(find.byType(GameDetail), findsOneWidget);
          expect(detailScroll.pixels, closeTo(detailOffset, .5));
          if (output != null) {
            File('$output/$name.json').writeAsStringSync(
              jsonEncode({
                'width': width,
                'language': language.name,
                'light': light,
                'scale': scale,
                'source': 'assets/game_catalog.json',
                'geometry': geometry,
                'issues': issues,
              }),
            );
          }
          expect(tester.takeException(), isNull);
          expect(issues, isEmpty);
          await tester.pumpWidget(const SizedBox());
          await tester.pumpAndSettle();
        }, variant: TargetPlatformVariant.only(TargetPlatform.windows));
      }
    }
  }

  test('Invalid saved locale uses the default Chinese language', () async {
    final storage = SharedPreferencesAsync();
    await storage.setString('launcher.language', 'obsolete');
    expect(
      (await LauncherPreferences.load(storage)).language,
      LauncherLanguage.simplifiedChinese,
    );
  });
}
