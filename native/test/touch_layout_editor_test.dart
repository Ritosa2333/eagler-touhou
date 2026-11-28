import 'dart:io';

import 'package:flutter/foundation.dart';
import 'package:flutter/rendering.dart';
import 'package:flutter/services.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:material_ui/material_ui.dart';
import 'package:shared_preferences/shared_preferences.dart';
import 'package:shared_preferences_platform_interface/in_memory_shared_preferences_async.dart';
import 'package:shared_preferences_platform_interface/shared_preferences_async_platform_interface.dart';
import 'package:touhouonline/catalog.dart';
import 'package:touhouonline/game_preferences.dart';
import 'package:touhouonline/l10n/strings.dart';
import 'package:touhouonline/library.dart';
import 'package:touhouonline/touch_layout_editor.dart';
import 'package:touhouonline/touch_preferences.dart';

import '../tools/capture_native_baseline_test.dart' show loadFonts, save;
import 'widget_test.dart' as launcher;

Future<void> _tap(WidgetTester tester, Finder control) async {
  await tester.ensureVisible(control);
  await tester.pumpAndSettle();
  await tester.tap(control);
  await tester.pumpAndSettle();
}

Future<void> _openWorkbench(
  WidgetTester tester,
  LauncherStrings strings,
) async {
  await _tap(
    tester,
    find.descendant(
      of: find.byType(LibraryPage),
      matching: find.byType(GameArtwork),
    ),
  );
  final settings = find.widgetWithText(OutlinedButton, strings.gameSettings);
  await _tap(
    tester,
    settings.evaluate().isEmpty
        ? find.byTooltip(strings.gameSettings)
        : settings,
  );
  await _tap(tester, find.text(strings.gameTouchSettings));
  await _tap(tester, find.text(strings.gameTouchLayout));
}

void main() {
  final capturePath = Platform.environment['TOUHOU_TOUCH_EDITOR_CAPTURE'];
  late final List<Game> games;

  setUpAll(() async {
    games = await Game.load();
    if (capturePath != null) {
      Directory(capturePath).createSync(recursive: true);
      await loadFonts(Directory(Platform.environment['TOUHOU_CAPTURE_FONTS']!));
    }
  });
  setUp(
    () => SharedPreferencesAsyncPlatform.instance =
        InMemorySharedPreferencesAsync.empty(),
  );

  for (final (asset, viewport, language, gameId) in [
    (
      'desktop-touch-layout',
      const Size(1440, 734),
      LauncherLanguage.simplifiedChinese,
      'th06',
    ),
    (
      'mobile-touch-layout-fighting-ja',
      const Size(390, 844),
      LauncherLanguage.japanese,
      'th075',
    ),
    (
      'mobile-narrow-touch-layout-en',
      const Size(320, 640),
      LauncherLanguage.english,
      'th06',
    ),
  ]) {
    testWidgets('$asset uses its language through settings and back', (
      tester,
    ) async {
      tester.view.devicePixelRatio = 1;
      tester.view.physicalSize = viewport;
      tester.view.display.devicePixelRatio = 1;
      tester.view.display.size = viewport;
      addTearDown(tester.view.resetDevicePixelRatio);
      addTearDown(tester.view.resetPhysicalSize);
      addTearDown(tester.view.display.reset);
      final game = games.singleWhere((game) => game.id == gameId);
      final storage = SharedPreferencesAsync();
      final preferences = GamePreferenceStore(storage);
      final strings = lookupLauncherStrings(language.locale);
      final boundary = GlobalKey();
      await tester.pumpWidget(
        RepaintBoundary(
          key: boundary,
          child: launcher.fixture(
            gamePreferences: preferences,
            games: [game],
            preferences: LauncherPreferences(
              storage,
              ThemeMode.dark,
              true,
              language: language,
            ),
          ),
        ),
      );
      await tester.pumpAndSettle();
      await _openWorkbench(tester, strings);
      final entry = find.text(strings.gameTouchLayout);
      final editor = find.byType(TouchLayoutEditor);
      ScrollPosition editorScroll() => tester
          .state<ScrollableState>(
            find
                .descendant(
                  of: find.byKey(const PageStorageKey('touch-editor-scroll')),
                  matching: find.byType(Scrollable),
                )
                .first,
          )
          .position;
      expect(editor, findsOneWidget);
      expect(find.text(strings.touchEditorTitle), findsOneWidget);
      expect(find.byTooltip(strings.touchEditorRestore), findsOneWidget);
      expect(find.byTooltip(strings.touchEditorResetViewport), findsOneWidget);
      final fire = find.byKey(const ValueKey('touch-preview-fire'));
      expect(
        find.descendant(
          of: fire,
          matching: find.text(
            gameId == 'th075'
                ? 'A ${strings.touchEditorLight}'
                : strings.touchEditorFire,
          ),
        ),
        findsOneWidget,
      );
      if (capturePath != null) {
        final title = tester.renderObject<RenderParagraph>(
          find.text(strings.touchEditorTitle),
        );
        expect(title.didExceedMaxLines, isFalse);
        await save(tester, boundary, Directory(capturePath), asset);
      }

      final target = find.byKey(const ValueKey('touch-sensitivity-target'));
      if (gameId == 'th075') {
        expect(target, findsNothing);
      } else {
        if (viewport.width == 320) {
          await _tap(tester, find.byTooltip(strings.touchEditorCollapse));
        }
        final background = find.byKey(
          const ValueKey('touch-sensitivity-background'),
        );
        final center = tester.getCenter(background);
        final origin = center + const Offset(-16, -25);
        final drag = await tester.startGesture(origin);
        await drag.moveBy(const Offset(10, 25));
        await tester.pump();
        expect(tester.getCenter(target), center + const Offset(15, 37.5));
        if (capturePath != null) {
          await save(
            tester,
            boundary,
            Directory(capturePath),
            '$asset-sensitivity-drag',
          );
        }
        await drag.moveBy(const Offset(0, 1000));
        await tester.pump();
        expect(tester.getCenter(target).dy, tester.getRect(background).bottom);
        await drag.cancel();
        await tester.pumpAndSettle();
        expect(tester.getCenter(target), center);
        expect((await preferences.touch.load(gameId)).layouts, isEmpty);
        if (viewport.width == 320) {
          await _tap(tester, find.byTooltip(strings.touchEditorExpand));
        }

        await _tap(tester, find.text(strings.touchEditorCustom));
        final slider = find.byKey(
          ValueKey(strings.touchEditorCustomSensitivity),
        );
        await tester.ensureVisible(slider);
        await tester.pumpAndSettle();
        expect(find.text(strings.touchEditorSensitivityHint), findsOneWidget);
        final at = tester.getCenter(slider);
        final canceled = await tester.startGesture(at);
        await canceled.moveBy(const Offset(45, 0));
        await tester.pumpAndSettle();
        expect(tester.widget<Slider>(slider).value, greaterThan(150));
        expect((await preferences.touch.load(gameId)).options.sensitivity, 150);
        await canceled.cancel();
        await tester.pumpAndSettle();
        expect(tester.widget<Slider>(slider).value, 150);
        expect((await preferences.touch.load(gameId)).options.sensitivity, 150);
        await tester.tapAt(at + const Offset(40, 0));
        await tester.pumpAndSettle();
        final stored = (await preferences.touch.load(gameId))
            .options
            .sensitivity;
        expect(stored, greaterThan(150));
        expect(tester.widget<Slider>(slider).value, stored);
        final sliderFocus = find.descendant(
          of: slider,
          matching: find.byType(FocusableActionDetector),
        );
        tester
            .widget<FocusableActionDetector>(sliderFocus)
            .focusNode!
            .requestFocus();
        await tester.pumpAndSettle();
        await tester.sendKeyEvent(LogicalKeyboardKey.arrowRight);
        await tester.pumpAndSettle();
        expect(
          (await preferences.touch.load(gameId)).options.sensitivity,
          stored + 1,
        );
        if (viewport.width == 320) editorScroll().jumpTo(0);
        await tester.pumpAndSettle();
        final nextCenter = tester.getCenter(background);
        final sample = await tester.startGesture(
          nextCenter + const Offset(-16, -25),
        );
        await sample.moveBy(const Offset(26, 0));
        await tester.pump();
        expect(
          tester.getCenter(target).dx,
          closeTo(nextCenter.dx + 26 * (stored + 1) / 100, 0.01),
        );
        await sample.up();
        await tester.pumpAndSettle();
        expect(tester.getCenter(target), nextCenter);
      }

      final movement = find
          .text(
            gameId == 'th075'
                ? strings.touchEditorJoystick
                : strings.touchEditorUnlimited,
          )
          .last;
      await _tap(tester, movement);
      final freeJoystick = find.widgetWithText(
        MenuItemButton,
        strings.touchEditorFreeJoystick,
      );
      expect(freeJoystick, findsOneWidget);
      if (gameId == 'th075') {
        expect(
          find.widgetWithText(MenuItemButton, strings.touchEditorDrag),
          findsNothing,
        );
      }
      await _tap(tester, freeJoystick);
      expect(target, findsNothing);
      expect(
        (await preferences.touch.load(gameId)).options.movement,
        'joystick-free',
      );
      if (gameId != 'th075') {
        await _tap(tester, find.text(strings.touchEditorFocusHold));
        final twoFinger = find.widgetWithText(
          MenuItemButton,
          strings.touchEditorFocusTwo,
        );
        expect(tester.widget<MenuItemButton>(twoFinger).onPressed, isNull);
        await tester.sendKeyEvent(LogicalKeyboardKey.escape);
        await tester.pumpAndSettle();
        expect(editor, findsOneWidget);
        expect(twoFinger, findsNothing);
      }
      if (viewport.width == 320) {
        final scrollBefore = editorScroll().pixels;
        expect(scrollBefore, greaterThan(0));
        await _tap(tester, find.byTooltip(strings.touchEditorCollapse));
        tester.view.physicalSize = const Size(320, 296);
        await tester.pumpAndSettle();
        expect(
          find.byKey(const ValueKey('touch-preview-fire')),
          findsOneWidget,
        );
        tester.view.physicalSize = viewport;
        await tester.pumpAndSettle();
        await _tap(tester, find.byTooltip(strings.touchEditorExpand));
        expect(editorScroll().pixels, scrollBefore);
        editorScroll().jumpTo(0);
        await tester.pumpAndSettle();
        await tester.dragFrom(const Offset(20, 350), const Offset(0, -100));
        await tester.pumpAndSettle();
        expect(editorScroll().pixels, greaterThan(0));
        editorScroll().jumpTo(0);
        await tester.pumpAndSettle();
      }

      await _tap(tester, find.text(strings.touchEditorAdjust));
      expect(
        find.byKey(const ValueKey('touch-viewport-gesture')),
        findsOneWidget,
      );
      await tester.binding.handlePopRoute();
      await tester.pumpAndSettle();
      expect(editor, findsOneWidget);
      expect(
        find.byKey(const ValueKey('touch-viewport-gesture')),
        findsNothing,
      );
      await _tap(tester, find.text(strings.touchEditorLandscape));
      await tester.ensureVisible(fire);
      await tester.pumpAndSettle();
      expect(fire.hitTestable(), findsOneWidget);
      final fireBeforeDrag = tester.getCenter(fire);
      await tester.drag(fire, const Offset(24, -18));
      await tester.pumpAndSettle();
      expect(tester.getCenter(fire).dy, lessThan(fireBeforeDrag.dy));
      expect(
        tester
            .widget<FilledButton>(
              find.widgetWithText(FilledButton, strings.touchEditorSave),
            )
            .onPressed,
        isNotNull,
      );
      await tester.sendKeyEvent(LogicalKeyboardKey.escape);
      await tester.pumpAndSettle();
      expect(find.text(strings.touchEditorDiscardTitle), findsOneWidget);
      await _tap(tester, find.text(strings.touchEditorContinue));
      expect(editor, findsOneWidget);
      await tester.binding.handlePopRoute();
      await tester.pumpAndSettle();
      await _tap(tester, find.text(strings.touchEditorDiscard));
      expect(editor, findsNothing);
      expect(entry.hitTestable(), findsOneWidget);
      final result = await preferences.touch.load(gameId);
      expect(result.layouts, isEmpty);
      expect(result.options.movement, 'joystick-free');
      expect(tester.takeException(), isNull);
    });
  }

  testWidgets(
    'th06 editor rotates in place and returns orientation to Android',
    (tester) async {
      const portrait = Size(390, 844);
      const landscape = Size(844, 390);
      tester.view.devicePixelRatio = 1;
      tester.view.physicalSize = portrait;
      tester.view.display.devicePixelRatio = 1;
      tester.view.display.size = const Size(700, 1300);
      debugDefaultTargetPlatformOverride = TargetPlatform.android;
      addTearDown(() {
        tester.view.reset();
        tester.view.display.reset();
        debugDefaultTargetPlatformOverride = null;
      });
      final messenger = tester.binding.defaultBinaryMessenger;
      final requests = <List<String>>[];
      var failRotation = true;
      messenger.setMockMethodCallHandler(SystemChannels.platform, (call) async {
        if (call.method == 'SystemChrome.setPreferredOrientations') {
          final orientations = List<String>.from(call.arguments as List);
          requests.add(orientations);
          if (failRotation && orientations.isNotEmpty) {
            throw PlatformException(code: 'orientation-unavailable');
          }
        }
        return null;
      });
      addTearDown(
        () => messenger.setMockMethodCallHandler(SystemChannels.platform, null),
      );
      final game = games.singleWhere((game) => game.id == 'th06');
      final storage = SharedPreferencesAsync();
      final preferences = GamePreferenceStore(storage);
      final strings = lookupLauncherStrings(LauncherLanguage.english.locale);
      final boundary = GlobalKey();
      await tester.pumpWidget(
        RepaintBoundary(
          key: boundary,
          child: launcher.fixture(
            gamePreferences: preferences,
            games: [game],
            preferences: LauncherPreferences(
              storage,
              ThemeMode.dark,
              true,
              language: LauncherLanguage.english,
            ),
          ),
        ),
      );
      await tester.pumpAndSettle();
      await _openWorkbench(tester, strings);
      final editor = find.byType(TouchLayoutEditor);
      final rotateLandscape = find.byTooltip(
        strings.touchEditorRotateLandscape,
      );
      final rotatePortrait = find.byTooltip(strings.touchEditorRotatePortrait);
      expect(rotateLandscape, findsNothing);
      tester.view.display.size = portrait;
      await tester.pumpAndSettle();
      expect(rotateLandscape, findsOneWidget);
      await _tap(tester, rotateLandscape);
      expect(find.text(strings.touchEditorRotationFailed), findsOneWidget);
      failRotation = false;

      final settings = find.byKey(const PageStorageKey('touch-editor-fields'));
      final fire = find.byKey(const ValueKey('touch-preview-fire'));
      final screen = find.byKey(const ValueKey('touch-sensitivity-background'));
      final saveLayout = find.widgetWithText(
        FilledButton,
        strings.touchEditorSave,
      );
      final portraitStart = tester.getCenter(fire);
      Future<void> reflow(Size viewport) async {
        tester.view.display.size = viewport;
        tester.view.physicalSize = viewport;
        await tester.pumpAndSettle();
      }

      await _tap(tester, rotateLandscape);
      expect(requests.last, [
        'DeviceOrientation.landscapeLeft',
        'DeviceOrientation.landscapeRight',
      ]);
      expect(settings, findsNothing);
      await reflow(landscape);
      expect(editor, findsOneWidget);
      expect(rotatePortrait, findsOneWidget);
      expect(tester.widget<FilledButton>(saveLayout).onPressed, isNull);
      expect(
        tester.getRect(settings).left,
        greaterThan(tester.getRect(screen).right),
      );
      if (capturePath != null) {
        await save(
          tester,
          boundary,
          Directory(capturePath),
          'mobile-landscape-touch-layout-en',
        );
      }
      final landscapeStart = tester.getCenter(fire);
      await tester.drag(fire, const Offset(28, -22));
      await tester.pumpAndSettle();
      expect(tester.getCenter(fire).dx, greaterThan(landscapeStart.dx));
      await tester.drag(
        find.byKey(const ValueKey('touch-preview-resize')),
        const Offset(20, 16),
      );
      await tester.pumpAndSettle();
      final landscapeDraft = tester.getCenter(fire);

      await _tap(tester, rotatePortrait);
      expect(requests.last, ['DeviceOrientation.portraitUp']);
      await reflow(portrait);
      expect(tester.getCenter(fire), portraitStart);
      await tester.drag(fire, const Offset(25, -24));
      await tester.pumpAndSettle();
      final portraitDraft = tester.getCenter(fire);
      expect(portraitDraft.dx, greaterThan(portraitStart.dx));
      await reflow(landscape);
      expect(tester.getCenter(fire), landscapeDraft);
      await reflow(portrait);
      expect(tester.getCenter(fire), portraitDraft);
      await _tap(tester, saveLayout);
      final configuration = await preferences.touch.load(game.id);
      expect(configuration.layouts.keys, unorderedEquals(Orientation.values));
      final landscapeLayout = configuration.layouts[Orientation.landscape]!;
      final initialLandscape =
          configuration.templates[Orientation.landscape]!.defaultProfile;
      expect(
        landscapeLayout.controls[TouchControl.fire]!.scale,
        greaterThan(initialLandscape.controls[TouchControl.fire]!.scale),
      );
      await tester.binding.handlePopRoute();
      await tester.pumpAndSettle();
      expect(editor, findsNothing);
      expect(find.text(strings.gameTouchLayout).hitTestable(), findsOneWidget);
      expect(requests.last, isEmpty);
      expect(tester.takeException(), isNull);
    },
  );
}
