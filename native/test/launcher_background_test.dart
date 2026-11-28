import 'dart:io';
import 'dart:math' as math;

import 'package:flutter/rendering.dart' show RenderParagraph;
import 'package:flutter/services.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:lucide_icons_flutter/lucide_icons.dart';
import 'package:material_ui/material_ui.dart';
import 'package:prism_ui/prism_ui.dart' as prism;
import 'package:shared_preferences/shared_preferences.dart';
import 'package:shared_preferences_platform_interface/in_memory_shared_preferences_async.dart';
import 'package:shared_preferences_platform_interface/shared_preferences_async_platform_interface.dart';
import 'package:touhouonline/catalog.dart';
import 'package:touhouonline/l10n/strings.dart';
import 'package:touhouonline/launcher_background.dart';
import 'package:touhouonline/library.dart';

import '../tools/capture_native_baseline_test.dart' show loadFonts, save;
import 'widget_test.dart' as launcher;

Finder _preset(BackgroundMode mode) =>
    find.byKey(ValueKey(('background preset', mode)));

Future<void> _tap(WidgetTester tester, Finder target) async {
  expect(target.hitTestable(), findsOneWidget);
  await tester.tap(target);
  await tester.pumpAndSettle();
}

void main() {
  final capture = Platform.environment['TOUHOU_APPEARANCE_CAPTURE'];
  late List<Game> games;

  setUpAll(() async {
    games = await Game.load();
    if (capture != null) {
      await Directory(capture).create(recursive: true);
      await loadFonts(Directory(Platform.environment['TOUHOU_CAPTURE_FONTS']!));
    }
  });
  setUp(() {
    SharedPreferencesAsyncPlatform.instance =
        InMemorySharedPreferencesAsync.empty();
  });

  for (final (device, size, language) in [
    ('desktop', const Size(1440, 734), LauncherLanguage.simplifiedChinese),
    ('mobile', const Size(390, 844), LauncherLanguage.simplifiedChinese),
    ('mobile-narrow', const Size(320, 640), LauncherLanguage.simplifiedChinese),
    ('mobile-narrow-en', const Size(320, 640), LauncherLanguage.english),
  ]) {
    final strings = lookupLauncherStrings(language.locale);
    testWidgets(
      'Background opens from the active shell and stays local | $device',
      (tester) async {
        tester.view.devicePixelRatio = 1;
        tester.view.physicalSize = size;
        addTearDown(tester.view.resetDevicePixelRatio);
        addTearDown(tester.view.resetPhysicalSize);
        final boundary = GlobalKey();
        final storage = SharedPreferencesAsync();
        final preferences = LauncherPreferences(
          storage,
          ThemeMode.dark,
          true,
          language: language,
        );
        await tester.pumpWidget(
          RepaintBoundary(
            key: boundary,
            child: launcher.fixture(
              accountApi: launcher.FixtureAccountApi(),
              preferences: preferences,
              games: games,
            ),
          ),
        );
        await tester.pumpAndSettle();
        final library = find.byType(LibraryPage);
        final before = tester.getRect(library);
        if (capture != null) {
          final context = tester.element(library);
          await tester.runAsync(() async {
            for (final game in games) {
              if (game.artwork case final String art) {
                await precacheImage(
                  AssetImage('assets/book-covers/$art'),
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
        final view = find.byKey(const ValueKey('library view control'));
        final sort = find.byKey(const ValueKey('library sort control'));
        final viewBounds = tester.getRect(view);
        final sortBounds = tester.getRect(sort);
        expect(viewBounds.height, 44);
        expect(sortBounds.height, viewBounds.height);
        expect(sortBounds.center.dy, closeTo(viewBounds.center.dy, .5));

        final sortLabel = find.descendant(
          of: sort,
          matching: find.text(GameOrder.number.label(strings)),
        );
        final listLabel = find.descendant(
          of: view,
          matching: find.text(strings.listView),
        );
        double baseline(Finder label) {
          final paragraph = tester.renderObject<RenderParagraph>(label);
          final painter = TextPainter(
            text: paragraph.text,
            textDirection: paragraph.textDirection,
            textScaler: paragraph.textScaler,
            maxLines: 1,
          )..layout(maxWidth: paragraph.size.width);
          final value = painter.computeDistanceToActualBaseline(
            TextBaseline.alphabetic,
          );
          painter.dispose();
          return tester.getTopLeft(label).dy + value;
        }

        final sortStyle = tester
            .renderObject<RenderParagraph>(sortLabel)
            .text
            .style!;
        expect(sortStyle.fontSize, 12);
        expect(sortStyle.fontWeight, FontWeight.w500);
        for (final (scope, symbol) in [
          (view, LucideIcons.layoutGrid),
          (view, LucideIcons.list),
          (sort, LucideIcons.chevronDown),
        ]) {
          final icon = find.descendant(
            of: scope,
            matching: find.byIcon(symbol),
          );
          expect(tester.getSize(icon), const Size.square(16));
          expect(tester.getCenter(icon).dy, closeTo(sortBounds.center.dy, .5));
        }
        if (listLabel.evaluate().isNotEmpty) {
          final listStyle = tester
              .renderObject<RenderParagraph>(listLabel)
              .text
              .style!;
          expect(listStyle.fontSize, sortStyle.fontSize);
          expect(listStyle.fontWeight, sortStyle.fontWeight);
          expect(baseline(listLabel), closeTo(baseline(sortLabel), .5));
        } else {
          expect(find.byTooltip(strings.listView), findsOneWidget);
        }

        if (device != 'desktop') {
          final bar = find.byKey(const ValueKey('mobile library toolbar'));
          final search = find.byKey(const ValueKey('mobile search fill'));
          final barBounds = tester.getRect(bar);
          expect(barBounds.height, 48);
          expect(tester.getRect(search).height, 34);
          expect(tester.getRect(search).left, 60);
          final firstCard = find.byKey(const ValueKey('th05'));
          final title = find.descendant(
            of: firstCard,
            matching: find.text('東方怪綺談'),
          );
          final code = find.descendant(
            of: firstCard,
            matching: find.text('th05'),
          );
          expect(title.hitTestable(), findsOneWidget);
          expect(code.hitTestable(), findsOneWidget);
          for (final icon in [
            LucideIcons.menu,
            LucideIcons.search,
            LucideIcons.megaphone,
            LucideIcons.mail,
          ]) {
            final glyph = find.descendant(of: bar, matching: find.byIcon(icon));
            expect(
              tester.getCenter(glyph).dy,
              closeTo(barBounds.center.dy, .5),
            );
          }
        }
        if (capture != null) {
          await save(
            tester,
            boundary,
            Directory(capture),
            '$device-polish-library',
          );
        }
        if (device == 'desktop') {
          await _tap(tester, find.byTooltip(strings.themeSettings));
        } else {
          await _tap(tester, find.byType(DrawerButton));
          final theme = find.widgetWithText(ListTile, strings.themeSettings);
          await tester.ensureVisible(theme);
          await _tap(tester, theme);
        }
        final baseLabel = find.byKey(const ValueKey('theme base label'));
        final options = find.byKey(const ValueKey('theme base segments'));
        expect(
          tester.getCenter(baseLabel).dy,
          closeTo(tester.getCenter(options).dy, 1),
        );
        expect(
          tester.getTopLeft(baseLabel).dx,
          tester.getTopLeft(find.text(strings.backgroundPresets)).dx,
        );
        expect(_preset(BackgroundMode.plain), findsOneWidget);
        final dialogBounds = tester.getRect(options);
        for (final (mode, label, brightness) in [
          (ThemeMode.light, strings.lightTheme, Brightness.light),
          (ThemeMode.dark, strings.darkTheme, Brightness.dark),
        ]) {
          await _tap(
            tester,
            find.descendant(of: options, matching: find.text(label)),
          );
          expect(preferences.theme, mode);
          final theme = Theme.of(tester.element(baseLabel));
          expect(theme.brightness, brightness);
          final sheet = tester.widget<Material>(
            find.ancestor(of: baseLabel, matching: find.byType(Material)).first,
          );
          final title = find.text(strings.themeSettings).hitTestable().last;
          final titleInk = tester
              .renderObject<RenderParagraph>(title)
              .text
              .style!
              .color!;
          final background = sheet.color!.computeLuminance();
          expect(
            background,
            brightness == Brightness.light ? greaterThan(.5) : lessThan(.1),
          );
          expect(
            (math.max(titleInk.computeLuminance(), background) + .05) /
                (math.min(titleInk.computeLuminance(), background) + .05),
            greaterThanOrEqualTo(4.5),
          );
          expect(tester.getRect(options), dialogBounds);
          if (capture != null) {
            await save(
              tester,
              boundary,
              Directory(capture),
              '$device-theme-switch-${mode.name}',
            );
          }
        }
        expect(find.byKey(const ValueKey(BackgroundLayer.shade)), findsNothing);
        await _tap(tester, _preset(BackgroundMode.grid));
        expect(
          tester
              .widget<prism.PressSurface>(_preset(BackgroundMode.grid))
              .selected,
          isTrue,
        );
        expect(find.text(strings.backgroundShade), findsOneWidget);
        expect(find.text(strings.backgroundNavigation), findsOneWidget);
        if (capture != null) {
          await save(
            tester,
            boundary,
            Directory(capture),
            '$device-background-settings',
          );
        }

        final layer = find.byKey(const ValueKey(BackgroundLayer.navigation));
        final slider = find.descendant(
          of: layer,
          matching: find.byType(prism.Slider),
        );
        await tester.ensureVisible(slider);
        await tester.pumpAndSettle();
        await _tap(tester, slider);
        final amount = preferences.background.opacity(
          BackgroundLayer.navigation,
        );
        expect(amount, inInclusiveRange(45, 55));
        if (device == 'desktop') {
          await tester.sendKeyEvent(LogicalKeyboardKey.escape);
          await tester.pumpAndSettle();
        } else {
          await _tap(tester, find.byTooltip(strings.close).hitTestable());
        }
        expect(tester.getRect(library), before);
        final surface = Theme.of(tester.element(library)).colorScheme.surface;
        expect(
          LauncherBackdrop.tint(
            tester.element(library),
            BackgroundLayer.navigation,
            surface,
          ).a,
          closeTo(amount / 100, .01),
        );
        final restored = await LauncherPreferences.load(storage);
        expect(restored.background.mode, BackgroundMode.grid);
        expect(restored.background.opacity(BackgroundLayer.navigation), amount);
        expect(tester.takeException(), isNull);
        if (device != 'desktop') {
          await _tap(tester, find.text(strings.home).hitTestable());
          await _tap(tester, find.text(strings.themeSettings));
          final group = find.text(strings.themeSettings);
          final background = find.text(strings.backgroundSettings);
          final base = find.byKey(const ValueKey('theme base label'));
          expect(tester.getTopLeft(group).dx, tester.getTopLeft(background).dx);
          expect(tester.getTopLeft(base).dx, tester.getTopLeft(background).dx);
          if (capture != null) {
            await save(
              tester,
              boundary,
              Directory(capture),
              '$device-polish-home',
            );
          }
        }
        if (device == 'mobile-narrow') {
          await _tap(tester, find.text(strings.backgroundSettings));
          expect(_preset(BackgroundMode.grid), findsOneWidget);
          expect(
            find.byKey(const ValueKey('theme base segments')).hitTestable(),
            findsNothing,
          );
          await tester.binding.handlePopRoute();
          await tester.pumpAndSettle();
          expect(
            find.text(strings.backgroundSettings).hitTestable(),
            findsOneWidget,
          );
        }
      },
      variant: TargetPlatformVariant.only(TargetPlatform.windows),
    );
  }

  test('Real image is copied, reopened, replaced safely and removed', () async {
    TestWidgetsFlutterBinding.ensureInitialized();
    final directory = await Directory.systemTemp.createTemp(
      'touhou-wallpaper-',
    );
    addTearDown(() => directory.delete(recursive: true));
    final storage = SharedPreferencesAsync();
    Future<Directory> resources() async => directory;
    final image = await File('assets/book-covers/th06.webp').readAsBytes();
    var data = image;
    final state = LauncherBackground(
      storage,
      appDirectory: resources,
      pickImage: () async => (
        name: 'fixture.webp',
        length: () async => data.length,
        read: () => Stream.value(data),
      ),
    );
    await state.chooseImage();
    expect(state.mode, BackgroundMode.image);
    expect(state.image, image);
    await state.setCover(false);
    state.setOpacity(BackgroundLayer.card, 37);
    await state.save();
    final reopened = LauncherBackground(storage, appDirectory: resources);
    await reopened.restore();
    expect(reopened.image, image);
    expect(reopened.cover, isFalse);
    expect(reopened.opacity(BackgroundLayer.card), 37);
    final original = await directory.list().single;
    data = Uint8List.fromList([1, 2, 3]);
    await expectLater(state.chooseImage(), throwsA(isNotNull));
    expect(state.image, image);
    expect(
      await directory.list().single.then((file) => file.path),
      original.path,
    );
    data = image;
    await state.chooseImage();
    expect(
      await directory.list().single.then((file) => file.path),
      isNot(original.path),
    );
    await state.select(BackgroundMode.dots);
    await state.removeImage();
    expect(await directory.list().isEmpty, isTrue);
    final cleared = LauncherBackground(storage, appDirectory: resources);
    await cleared.restore();
    expect(cleared.image, isNull);
    expect(cleared.mode, BackgroundMode.dots);
  });
}
