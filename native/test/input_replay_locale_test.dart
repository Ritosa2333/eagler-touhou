import 'dart:async';
import 'dart:io';

import 'package:flutter/services.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:material_ui/material_ui.dart';
import 'package:shared_preferences/shared_preferences.dart';
import 'package:shared_preferences_platform_interface/in_memory_shared_preferences_async.dart';
import 'package:shared_preferences_platform_interface/shared_preferences_async_platform_interface.dart';
import 'package:touhouonline/catalog.dart';
import 'package:touhouonline/game_input_dialog.dart';
import 'package:touhouonline/game_preferences.dart';
import 'package:touhouonline/game_user_files.dart';
import 'package:touhouonline/gamepad_settings.dart';
import 'package:touhouonline/keyboard_settings.dart';
import 'package:touhouonline/l10n/strings.dart';
import 'package:touhouonline/launcher_select.dart';
import 'package:touhouonline/library.dart';
import 'package:touhouonline/local_duel_settings.dart';
import 'package:touhouonline/replay_manager.dart';

import '../tools/capture_native_baseline_test.dart' show loadFonts, save;
import 'widget_test.dart' as fixtures;

final class _DelayedKeyboardPreferences extends InMemorySharedPreferencesAsync {
  _DelayedKeyboardPreferences() : super.empty();

  Completer<void>? keyboardWrite;

  @override
  Future<bool> setString(
    String key,
    String value,
    SharedPreferencesOptions options,
  ) async {
    if (key == 'keyboard-layouts.v1.general') await keyboardWrite?.future;
    return super.setString(key, value, options);
  }
}

Future<void> _tap(WidgetTester tester, Finder control) async {
  await tester.ensureVisible(control);
  await tester.pumpAndSettle();
  expect(
    control.hitTestable(),
    findsOneWidget,
    reason: 'Control bounds: ${tester.getRect(control)}',
  );
  await tester.tap(control);
  await tester.pumpAndSettle();
}

Finder _inside(Type widget, String label) =>
    find.descendant(of: find.byType(widget), matching: find.text(label));

Future<void> _tapAndWaitForFileIO(
  WidgetTester tester,
  Finder control,
  Finder result,
) async {
  await tester.ensureVisible(control);
  await tester.pumpAndSettle();
  await tester.runAsync(() async {
    await tester.tap(control);
    final deadline = DateTime.now().add(const Duration(seconds: 4));
    while (result.evaluate().isEmpty && DateTime.now().isBefore(deadline)) {
      await tester.pump(const Duration(milliseconds: 20));
      await Future<void>.delayed(const Duration(milliseconds: 20));
    }
    expect(result, findsOneWidget);
  });
  await tester.pumpAndSettle();
}

void main() {
  TestWidgetsFlutterBinding.ensureInitialized();
  final output = Platform.environment['TOUHOU_INPUT_REPLAY_CAPTURE'];
  final replaySample = Platform.environment['TOUHOU_CAPTURE_TH09_REPLAY'];
  late Game game;
  late _DelayedKeyboardPreferences preferences;

  setUpAll(() async {
    game = (await Game.load()).singleWhere((game) => game.id == 'th09');
    if (output != null) {
      Directory(output).createSync(recursive: true);
      await loadFonts(Directory(Platform.environment['TOUHOU_CAPTURE_FONTS']!));
    }
  });

  setUp(() {
    preferences = _DelayedKeyboardPreferences();
    SharedPreferencesAsyncPlatform.instance = preferences;
  });

  for (final (device, size, language, locale) in [
    ('mobile-narrow', const Size(320, 640), LauncherLanguage.english, 'en'),
    ('mobile', const Size(390, 844), LauncherLanguage.japanese, 'ja'),
    if (output != null)
      ('desktop', const Size(1440, 734), LauncherLanguage.english, 'en'),
  ]) {
    testWidgets(
      'Input and replay panels keep the interface language | $device',
      (tester) async {
        tester.view.devicePixelRatio = 1;
        tester.view.physicalSize = size;
        addTearDown(tester.view.resetDevicePixelRatio);
        addTearDown(tester.view.resetPhysicalSize);
        final workspace = await tester.runAsync(
          () => Directory.systemTemp.createTemp('touhou-replay-locale-'),
        );
        addTearDown(() => workspace!.delete(recursive: true));

        const events = MethodChannel('just_gamepads/events');
        final messenger = tester.binding.defaultBinaryMessenger;
        messenger.setMockMethodCallHandler(events, (_) async => null);
        addTearDown(() => messenger.setMockMethodCallHandler(events, null));
        final strings = lookupLauncherStrings(language.locale);
        final storage = SharedPreferencesAsync();
        final options = GamePreferenceStore(storage);
        final boundary = GlobalKey();
        var useSample = false;
        final files = GameUserFiles(
          appDirectory: () async => workspace!,
          picker: (_) async {
            if (useSample && replaySample != null) {
              final sample = File(replaySample);
              return (
                name: sample.uri.pathSegments.last,
                length: sample.length,
                read: sample.openRead,
              );
            }
            return (
              name: 'th9_01.rpy',
              length: () async => 0,
              read: () => const Stream<List<int>>.empty(),
            );
          },
        );
        await tester.pumpWidget(
          RepaintBoundary(
            key: boundary,
            child: fixtures.fixture(
              games: [game],
              preferences: LauncherPreferences(
                storage,
                ThemeMode.dark,
                true,
                language: language,
              ),
              gameUserFiles: files,
              gamePreferences: options,
            ),
          ),
        );
        await tester.pumpAndSettle();
        if (output != null) {
          final context = tester.element(find.byType(LibraryPage));
          await tester.runAsync(() async {
            await precacheImage(
              AssetImage('assets/book-covers/${game.artwork}'),
              context,
            );
            await precacheImage(
              const AssetImage('assets/launcher-wordmark-dark.png'),
              context,
            );
          });
          await tester.pumpAndSettle();
        }
        Future<void> capture(String state) async {
          if (output != null) {
            await save(
              tester,
              boundary,
              Directory(output),
              '$device-$state-$locale',
            );
          }
        }

        await _tap(
          tester,
          find.descendant(
            of: find.byType(LibraryPage),
            matching: find.byType(GameArtwork),
          ),
        );
        final settingsText = find.widgetWithText(
          OutlinedButton,
          strings.gameSettings,
        );
        await _tap(
          tester,
          settingsText.evaluate().isEmpty
              ? find.byTooltip(strings.gameSettings)
              : settingsText,
        );
        await _tap(tester, find.text(strings.gameInputSettings));
        final entry = find.text(strings.gameKeyboard);
        final trigger = Focus.of(tester.element(entry));
        trigger.requestFocus();
        await tester.pump();
        await _tap(tester, entry);
        final input = find.byType(GameInputDialog);
        final surface = find.descendant(
          of: input,
          matching: find.byWidgetPredicate(
            (widget) => widget is Material && widget.type == MaterialType.card,
          ),
        );
        final bounds = tester.getRect(surface);
        expect(bounds.width, (size.width - 24).clamp(0, 560));
        expect(bounds.height, 608);
        expect(
          _inside(KeyboardSettings, strings.inputKeyboardHint),
          findsOneWidget,
        );
        await capture('game-keyboard');
        final binding = find.byKey(const ValueKey('keyboard-up'));
        await _tap(tester, binding);
        expect(find.text(strings.inputCaptureHint), findsOneWidget);
        await tester.sendKeyEvent(LogicalKeyboardKey.escape);
        await tester.pumpAndSettle();
        expect(find.byType(KeyboardCaptureDialog), findsNothing);
        await _tap(tester, binding);
        final write = Completer<void>();
        preferences.keyboardWrite = write;
        addTearDown(() {
          if (!write.isCompleted) write.complete();
        });
        await tester.sendKeyEvent(LogicalKeyboardKey.keyH);
        await tester.pumpAndSettle();
        expect(
          tester
              .widget<LauncherSegments<GameInputDevice>>(
                find.byType(LauncherSegments<GameInputDevice>),
              )
              .onChanged,
          isNull,
        );
        final done = find.widgetWithText(TextButton, strings.inputDone);
        expect(tester.widget<TextButton>(done).onPressed, isNull);
        await tester.binding.handlePopRoute();
        await tester.pumpAndSettle();
        expect(input, findsOneWidget);
        write.complete();
        await tester.pumpAndSettle();
        final layout = await options.keyboard.load(game);
        expect(layout.active.keysFor('ArrowUp'), contains('KeyH'));
        await tester.ensureVisible(
          find
              .descendant(
                of: find.byType(KeyboardSettings),
                matching: find.byWidgetPredicate(
                  (widget) =>
                      widget is TextButton &&
                      widget.key is ValueKey<String> &&
                      (widget.key! as ValueKey<String>).value.startsWith(
                        'keyboard-',
                      ),
                ),
              )
              .last,
        );
        await tester.pumpAndSettle();
        final layoutLabel = _inside(
          KeyboardSettings,
          strings.inputKeyboardLayout,
        );
        expect(layoutLabel.hitTestable(), findsNothing);
        final controller = find.byType(GamepadSettings);
        await _tap(
          tester,
          _inside(GameInputDialog, strings.inputGamepadDevice),
        );
        expect(
          _inside(GamepadSettings, strings.inputSelectController),
          findsOneWidget,
        );
        expect(tester.getRect(surface), bounds);
        await capture('game-gamepad');
        await _tap(
          tester,
          _inside(GameInputDialog, strings.inputKeyboardDevice),
        );
        expect(controller, findsNothing);
        expect(layoutLabel.hitTestable(), findsOneWidget);
        expect(
          find.descendant(of: binding, matching: find.textContaining('H')),
          findsOneWidget,
        );

        await _tap(tester, _inside(GameInputDialog, strings.inputGeneralScope));
        await _tap(tester, find.text(strings.gameLocalDuel));
        expect(find.byType(LocalDuelSettings), findsOneWidget);
        expect(tester.getRect(surface), bounds);
        await capture('th09-local-duel');
        await _tap(tester, find.text(strings.inputPlayer(2)));
        await _tap(tester, find.byKey(const ValueKey('duel-shoot')));
        await tester.sendKeyEvent(LogicalKeyboardKey.keyZ);
        await tester.pumpAndSettle();
        expect(find.text(strings.inputKeyConflict('Z')), findsOneWidget);
        await tester.sendKeyEvent(LogicalKeyboardKey.escape);
        await tester.pumpAndSettle();
        if (device == 'desktop') {
          await tester.sendKeyEvent(LogicalKeyboardKey.escape);
        } else {
          await tester.binding.handlePopRoute();
        }
        await tester.pumpAndSettle();
        expect(input, findsNothing);
        expect(entry.hitTestable(), findsOneWidget);
        expect(trigger.hasFocus, isTrue);

        await _tap(tester, find.text(strings.gameStorageSettings));
        final manage = find.byKey(const ValueKey((GameUserFileKind.replay, 1)));
        final empty = _inside(ReplayManagerDialog, strings.gameNoReplay);
        await _tapAndWaitForFileIO(tester, manage, empty);
        expect(
          _inside(ReplayManagerDialog, strings.replayManager),
          findsOneWidget,
        );
        await capture('replay-manager-empty');
        final import = _inside(ReplayManagerDialog, strings.replayImport);
        final invalid = _inside(
          ReplayManagerDialog,
          strings.fileIssueImportEmptyOrLarge,
        );
        await _tapAndWaitForFileIO(tester, import, invalid);
        await capture('replay-manager-invalid');

        if (replaySample != null) {
          useSample = true;
          await _tapAndWaitForFileIO(tester, import, find.text('th9_01.rpy'));
          await _tap(tester, find.byTooltip(strings.replayActions));
          expect(find.text(strings.replayRename), findsOneWidget);
          await capture('replay-manager-actions');
          await tester.binding.handlePopRoute();
          await tester.pumpAndSettle();
        }
        final manager = find.byType(ReplayManagerDialog);
        await _tap(
          tester,
          find.descendant(of: manager, matching: find.byTooltip(strings.close)),
        );
        expect(manager, findsNothing);
        expect(manage.hitTestable(), findsOneWidget);
        expect(tester.takeException(), isNull);
      },
    );
  }
}
