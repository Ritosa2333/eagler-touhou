import 'dart:async';
import 'dart:io';

import 'package:archive/archive.dart';
import 'package:flutter/services.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:material_ui/material_ui.dart';
import 'package:shared_preferences/shared_preferences.dart';
import 'package:shared_preferences_platform_interface/in_memory_shared_preferences_async.dart';
import 'package:shared_preferences_platform_interface/shared_preferences_async_platform_interface.dart';
import 'package:touhouonline/catalog.dart';
import 'package:touhouonline/game_file_picker.dart';
import 'package:touhouonline/game_user_files.dart';
import 'package:touhouonline/l10n/strings.dart';
import 'package:touhouonline/library.dart';

import '../tools/capture_native_baseline_test.dart' show loadFonts, save;
import 'widget_test.dart' as launcher;

Future<void> _tap(WidgetTester tester, Finder finder) async {
  await tester.ensureVisible(finder);
  await tester.pumpAndSettle();
  await tester.tap(finder);
  await tester.pumpAndSettle();
}

Future<void> _waitForFileUI(WidgetTester tester, Finder finder) async {
  final until = DateTime.now().add(const Duration(seconds: 5));
  while (finder.evaluate().isEmpty && DateTime.now().isBefore(until)) {
    await Future<void>.delayed(const Duration(milliseconds: 15));
    await tester.pump(const Duration(milliseconds: 25));
  }
  expect(finder, findsOneWidget);
  await tester.pump(const Duration(milliseconds: 300));
}

void main() {
  final capture = Platform.environment['TOUHOU_SAVE_FLOW_CAPTURE'];
  late Game th15;
  setUpAll(() async {
    th15 = (await Game.load()).singleWhere((game) => game.id == 'th15');
    if (capture != null) {
      Directory(capture).createSync(recursive: true);
      await loadFonts(Directory(Platform.environment['TOUHOU_CAPTURE_FONTS']!));
    }
  });
  setUp(() {
    SharedPreferencesAsyncPlatform.instance =
        InMemorySharedPreferencesAsync.empty();
  });

  for (final (device, size, language) in [
    ('desktop', const Size(1440, 734), LauncherLanguage.english),
    ('mobile-narrow', const Size(320, 640), LauncherLanguage.english),
  ]) {
    testWidgets('$device th15 save picker cancels and round-trips ZIP', (
      tester,
    ) async {
      tester.view.devicePixelRatio = 1;
      tester.view.physicalSize = size;
      addTearDown(tester.view.reset);
      final root = (await tester.runAsync(() {
        final base = Platform.environment['TOUHOU_CAPTURE_SCRATCH'];
        return (base == null ? Directory.systemTemp : Directory(base))
            .createTemp('th15-save-flow-');
      }))!;
      addTearDown(() => root.delete(recursive: true));
      final strings = lookupLauncherStrings(language.locale);
      final boundary = GlobalKey();
      final entries = <String, List<int>>{
        'scoreth15.dat': [0, 1, 3, 7, 11],
        'autosave/save0_0.dat': [53, 72, 89, 113],
      };
      final archive = ZipEncoder().encode(
        Archive()
          ..addFile(ArchiveFile.bytes('readme.txt', [1]))
          ..addFile(
            ArchiveFile.bytes('scoreth15.dat', entries['scoreth15.dat']!),
          )
          ..addFile(
            ArchiveFile.bytes(
              'autosave/save0_0.dat',
              entries['autosave/save0_0.dat']!,
            ),
          ),
      );
      final input = (
        name: 'progress.zip',
        length: () async => archive.length,
        read: () => Stream.value(archive),
      );
      var picked = 0;
      final pendingPick = Completer<GameFileInput?>();
      final exported = File.fromUri(root.uri.resolve('scoreth15.zip'));
      final files = GameUserFiles(
        appDirectory: () async => root,
        picker: (extensions) async {
          expect(extensions, ['dat', 'zip']);
          picked++;
          return picked == 1 ? pendingPick.future : input;
        },
        exporter: (name, file) async {
          expect(name, 'scoreth15.zip');
          await file.copy(exported.path);
          return true;
        },
      );
      await tester.pumpWidget(
        RepaintBoundary(
          key: boundary,
          child: launcher.fixture(
            games: [th15],
            gameUserFiles: files,
            preferences: LauncherPreferences(
              SharedPreferencesAsync(),
              ThemeMode.dark,
              true,
              language: language,
            ),
          ),
        ),
      );
      await tester.pumpAndSettle();
      if (capture != null) {
        final context = tester.element(find.byType(LibraryPage));
        await tester.runAsync(() async {
          await precacheImage(
            AssetImage('assets/book-covers/${th15.artwork}'),
            context,
          );
          await precacheImage(
            const AssetImage('assets/launcher-wordmark-dark.png'),
            context,
          );
        });
        await tester.pumpAndSettle();
      }
      await _tap(
        tester,
        find.descendant(
          of: find.byType(LibraryPage),
          matching: find.byType(GameArtwork),
        ),
      );
      final settings = find.widgetWithText(
        OutlinedButton,
        strings.gameSettings,
      );
      await _tap(
        tester,
        settings.evaluate().isEmpty
            ? find.byTooltip(strings.gameSettings)
            : settings,
      );
      await _tap(tester, find.text(strings.gameStorageSettings));
      final download = find.byKey(const ValueKey((GameUserFileKind.save, 0)));
      final upload = find.byKey(const ValueKey((GameUserFileKind.save, 1)));
      await tester.ensureVisible(download);
      await tester.pumpAndSettle();
      for (final (index, save) in [download, upload].indexed) {
        final replay = find.byKey(ValueKey((GameUserFileKind.replay, index)));
        expect(tester.getRect(save).left, tester.getRect(replay).left);
        expect(tester.getSize(save).width, tester.getSize(replay).width);
      }
      expect(tester.getSize(download).width, tester.getSize(upload).width);
      if (capture != null) {
        await save(
          tester,
          boundary,
          Directory(capture),
          '$device-th15-save-tools-en',
        );
      }
      final uploadIcon = find.descendant(
        of: upload,
        matching: find.byIcon(Icons.upload_outlined),
      );
      final uploadFocus = Focus.of(tester.element(uploadIcon));
      uploadFocus.requestFocus();
      await tester.pump();

      Future<void> openConfirmation() async {
        await tester.sendKeyEvent(LogicalKeyboardKey.enter);
        await tester.pump();
        await tester.pump(const Duration(milliseconds: 300));
      }

      await openConfirmation();
      final confirmation = find.text(strings.gameImportSaveTitle);
      expect(confirmation, findsOneWidget);
      expect(picked, 0);
      if (capture != null) {
        final cancel = find.widgetWithText(TextButton, strings.cancel);
        final proceed = find.widgetWithText(
          FilledButton,
          strings.gameContinueImport,
        );
        expect(tester.getCenter(cancel).dy, tester.getCenter(proceed).dy);
        await save(
          tester,
          boundary,
          Directory(capture),
          '$device-th15-confirm-save-en',
        );
      }
      await tester.sendKeyEvent(LogicalKeyboardKey.escape);
      await tester.pumpAndSettle();
      expect(confirmation, findsNothing);
      expect(picked, 0);
      expect(uploadFocus.hasPrimaryFocus, isTrue);
      await openConfirmation();
      await tester.tap(find.text(strings.gameContinueImport));
      await tester.pump();
      expect(picked, 1);
      pendingPick.complete(null);
      await tester.pumpAndSettle();
      expect(confirmation, findsNothing);
      expect(uploadFocus.hasPrimaryFocus, isTrue);
      final workspace = files.workspace(
        th15,
        multiplayer: false,
        language: 'ja',
      )!;
      expect(await tester.runAsync(workspace.exportSave), isNull);

      await tester.runAsync(() async {
        await tester.tap(download);
        await tester.pump();
        await _waitForFileUI(tester, find.text(strings.gameNoSave));
        await tester.tap(find.text(strings.gameSelectFile));
        await tester.pump();
        await _waitForFileUI(tester, find.text(strings.gameFilesImported(2)));
      });
      expect(picked, 2);
      expect(confirmation, findsNothing);
      await tester.runAsync(() async {
        await tester.tap(download);
        await tester.pump();
        await _waitForFileUI(tester, find.text(strings.gameFileSaved));
        final decoded = ZipDecoder().decodeBytes(
          await exported.readAsBytes(),
          verify: true,
        );
        expect(
          decoded.files.map((file) => file.name),
          unorderedEquals(entries.keys),
        );
        for (final file in decoded.files) {
          expect(file.content, entries[file.name]);
        }
      });
      expect(tester.takeException(), isNull);
    });
  }
}
