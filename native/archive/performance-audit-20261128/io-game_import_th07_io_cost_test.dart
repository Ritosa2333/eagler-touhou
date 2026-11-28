import 'dart:async';
import 'dart:convert';
import 'dart:io';

import 'package:crypto/crypto.dart';
import 'package:flutter/services.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:material_ui/material_ui.dart';
import 'package:shared_preferences/shared_preferences.dart';
import 'package:shared_preferences_platform_interface/in_memory_shared_preferences_async.dart';
import 'package:shared_preferences_platform_interface/shared_preferences_async_platform_interface.dart';
import 'package:touhouonline/catalog.dart';
import 'package:touhouonline/game_detail.dart';
import 'package:touhouonline/game_file_picker.dart';
import 'package:touhouonline/game_import_dialog.dart';
import 'package:touhouonline/game_resources.dart';
import 'package:touhouonline/game_user_files.dart';
import 'package:touhouonline/l10n/strings.dart';
import 'package:touhouonline/library.dart';

import '../tools/capture_native_baseline_test.dart' show loadFonts, save;
import 'widget_test.dart' as launcher;

class _ObservedResources extends GameResources {
  _ObservedResources({super.appDirectory, super.picker, super.reference});

  bool finished = false;
  File? _stored;

  @override
  Future<File?> installed(GameData? data) async => _stored;

  @override
  Future<File> importData(
    Game game,
    GameFileInput source, {
    required void Function(DataImportProgress) progress,
  }) async {
    try {
      final stored = await super.importData(game, source, progress: progress);
      _stored = stored;
      return stored;
    } finally {
      finished = true;
    }
  }
}

void main() {
  final archiveInput = Platform.environment['TOUHOU_CAPTURE_RELEASE_ZIP'];
  final scratch = Platform.environment['TOUHOU_CAPTURE_SCRATCH'];
  final captures = Platform.environment['TOUHOU_PACKAGE_CAPTURE'];
  late Game th07;
  final manifestPath = Platform.environment['TOUHOU_PACKAGE_MANIFEST'];
  var fallback = (
    address: Uri.https('download.example', '/touhou/release-package'),
    hint: 'fixture',
  );
  setUpAll(() async {
    th07 = (await Game.load()).singleWhere((game) => game.id == 'th07');
    if (manifestPath != null) {
      final manifest =
          jsonDecode(await File(manifestPath).readAsString()) as Map;
      final configured = manifest['shared']['gameDataFallback'] as Map;
      fallback = (
        address: Uri.parse(configured['url'] as String),
        hint: configured['hint'] as String,
      );
    }
    if (captures != null) {
      await loadFonts(Directory(Platform.environment['TOUHOU_CAPTURE_FONTS']!));
    }
  });
  setUp(() {
    SharedPreferencesAsyncPlatform.instance =
        InMemorySharedPreferencesAsync.empty();
  });
  for (final (device, size, language) in [
    ('mobile-narrow', const Size(320, 640), LauncherLanguage.english),
    ('mobile', const Size(390, 844), LauncherLanguage.japanese),
    ('desktop', const Size(1440, 734), LauncherLanguage.simplifiedChinese),
  ]) {
    testWidgets(
      '$device opens import and returns from the download reference',
      (tester) async {
        tester.view.devicePixelRatio = 1;
        tester.view.physicalSize = size;
        addTearDown(tester.view.resetPhysicalSize);
        addTearDown(tester.view.resetDevicePixelRatio);
        final strings = lookupLauncherStrings(language.locale);
        final releaseZip = archiveInput == null ? null : File(archiveInput);
        final install = releaseZip == null
            ? null
            : await tester.runAsync(
                () => Directory(scratch!).createTemp('package-'),
              );
        if (install != null) addTearDown(() => install.delete(recursive: true));
        final runtime = launcher.RecordingGameLauncher();
        var reference = Completer<GamePackageReference?>();
        var picked = 0;
        GameFileInput? replacement;
        var references = 0;
        final resources = _ObservedResources(
          appDirectory: () async => install ?? Directory(scratch ?? '.'),
          picker: () async {
            if (++picked <= 2 || releaseZip == null) return null;
            if (replacement != null) return replacement;
            return (
              name: 'th07.zip',
              length: releaseZip.length,
              read: releaseZip.openRead,
            );
          },
          reference: () {
            references++;
            return reference.future;
          },
        );
        final boundary = GlobalKey();
        await tester.pumpWidget(
          RepaintBoundary(
            key: boundary,
            child: launcher.fixture(
              games: [th07],
              launcher: runtime,
              gameResources: resources,
              gameUserFiles: install == null
                  ? null
                  : GameUserFiles(appDirectory: () async => install),
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
        if (captures != null) {
          final context = tester.element(find.byType(LibraryPage));
          await tester.runAsync(
            () => precacheImage(
              const AssetImage('assets/book-covers/th07.webp'),
              context,
            ),
          );
          await tester.pumpAndSettle();
        }
        await tester.tap(find.byType(GameArtwork).last);
        await tester.pumpAndSettle();

        final import = find.widgetWithText(
          OutlinedButton,
          strings.gameFileImport,
        );
        final importButton = import.evaluate().isEmpty
            ? find.byTooltip(strings.importingResources)
            : import;
        final importIcon = find.descendant(
          of: importButton,
          matching: find.byType(Icon),
        );
        final desktop = device == 'desktop';
        if (desktop) {
          Focus.of(tester.element(importIcon)).requestFocus();
          await tester.pump();
        }
        await tester.runAsync(() async {
          if (desktop) {
            await tester.sendKeyEvent(LogicalKeyboardKey.enter);
          } else {
            await tester.tap(importButton);
          }
          await tester.pump();
        });
        await tester.pumpAndSettle();
        final dialog = find.byType(GameDataImportDialog);
        final choose = find.widgetWithText(
          FilledButton,
          strings.importPackageChoose,
        );
        final open = find.widgetWithText(
          OutlinedButton,
          strings.importPackageOpenReference,
        );
        expect(dialog, findsOneWidget);
        final rect = tester.getRect(find.byType(AlertDialog));
        expect(rect.left, greaterThanOrEqualTo(0));
        expect(rect.right, lessThanOrEqualTo(size.width));
        final chooseRect = tester.getRect(choose);
        final linkRect = tester.getRect(open);
        expect(chooseRect.top, closeTo(linkRect.top, 1));
        expect(chooseRect.size, linkRect.size);
        if (captures != null) {
          await save(
            tester,
            boundary,
            Directory(captures),
            '$device-game-package-import',
          );
        }
        await tester.tap(choose);
        await tester.pumpAndSettle();
        expect(picked, 1);
        expect(dialog, findsOneWidget);
        expect(tester.widget<FilledButton>(choose).focusNode?.hasFocus, isTrue);
        await tester.sendKeyEvent(LogicalKeyboardKey.space);
        await tester.pumpAndSettle();
        expect(picked, 2);
        expect(tester.widget<FilledButton>(choose).focusNode?.hasFocus, isTrue);

        await tester.tap(open);
        await tester.pump();
        expect(references, 1);
        expect(find.byType(LinearProgressIndicator), findsOneWidget);
        reference.completeError(
          const SocketException('Fixture connection failed'),
        );
        await tester.pumpAndSettle();
        expect(find.text(strings.importPackageReferenceFailed), findsOneWidget);
        expect(find.text(strings.importPackageNoReference), findsNothing);
        final retry = find.widgetWithText(
          OutlinedButton,
          strings.importPackageRetryReference,
        );
        expect(retry.hitTestable(), findsOneWidget);
        if (captures != null) {
          await save(
            tester,
            boundary,
            Directory(captures),
            '$device-game-package-reference-retry',
          );
        }
        reference = Completer<GamePackageReference?>();
        await tester.tap(retry);
        await tester.pump();
        expect(references, 2);
        expect(find.text(strings.importPackageReferenceFailed), findsNothing);
        expect(find.byType(LinearProgressIndicator), findsOneWidget);
        final address = fallback.address.toString();
        reference.complete(fallback);
        await tester.pumpAndSettle();
        expect(find.text(address), findsOneWidget);
        expect(find.text(fallback.hint), findsOneWidget);
        if (captures != null) {
          await save(
            tester,
            boundary,
            Directory(captures),
            '$device-game-package-reference',
          );
        }
        Future<void> back() async {
          if (desktop) {
            await tester.sendKeyEvent(LogicalKeyboardKey.escape);
          } else {
            await tester.binding.handlePopRoute();
          }
          await tester.pumpAndSettle();
        }

        await back();
        await tester.pumpAndSettle();
        expect(dialog, findsOneWidget);
        expect(find.text(address), findsNothing);
        expect(tester.widget<OutlinedButton>(open).focusNode?.hasFocus, isTrue);
        if (desktop) {
          reference = Completer<GamePackageReference?>();
          await tester.tap(open);
          await tester.pump();
          reference.complete(null);
          await tester.pumpAndSettle();
          expect(find.text(strings.importPackageNoReference), findsOneWidget);
          expect(retry, findsNothing);
          await back();
          expect(
            tester.widget<OutlinedButton>(open).focusNode?.hasFocus,
            isTrue,
          );
        }

        if (install != null) {
          await tester.runAsync(() async {
            await tester.tap(choose);
            for (
              var attempt = 0;
              !resources.finished && attempt < 1500;
              attempt++
            ) {
              await Future<void>.delayed(const Duration(milliseconds: 10));
            }
            expect(resources.finished, isTrue);
          });
          await tester.pumpAndSettle();
          expect(dialog, findsNothing);
          expect(find.text(strings.resourcesImported), findsOneWidget);
          if (desktop) {
            final current = (await resources.installed(th07.data))!;
            final bad = (await tester.runAsync(() async {
              final copy = await current.copy(
                install.uri.resolve('bad-${th07.data!.name}').toFilePath(),
              );
              final contents = await copy.readAsBytes();
              contents[0] ^= 1;
              return copy.writeAsBytes(contents);
            }))!;
            replacement = (
              name: th07.data!.name,
              length: bad.length,
              read: bad.openRead,
            );
            resources.finished = false;
            await tester.runAsync(() => tester.tap(importButton));
            await tester.pumpAndSettle();
            await tester.runAsync(() async {
              await tester.tap(choose);
              for (
                var attempt = 0;
                !resources.finished && attempt < 1500;
                attempt++
              ) {
                await Future<void>.delayed(const Duration(milliseconds: 10));
              }
              expect(resources.finished, isTrue);
            });
            await tester.pumpAndSettle();
            expect(
              find.text(strings.importPackageDigestMismatch),
              findsOneWidget,
            );
            expect(
              tester.widget<FilledButton>(choose).focusNode?.hasFocus,
              isTrue,
            );
            await back();
          }
          await tester.runAsync(() async {
            await tester.tap(find.text(strings.startGame));
            for (
              var attempt = 0;
              runtime.data == null && attempt < 400;
              attempt++
            ) {
              await Future<void>.delayed(const Duration(milliseconds: 5));
            }
          });
          await tester.pumpAndSettle();
          expect(runtime.data, endsWith(th07.data!.name));
          final installed = File(runtime.data!);
          final checksum = await tester.runAsync(
            () async => (
              bytes: await installed.length(),
              sha256: (await sha256.bind(installed.openRead()).first)
                  .toString(),
            ),
          );
          expect(checksum, (
            bytes: th07.data!.bytes,
            sha256: th07.data!.sha256,
          ));
          await tester.binding.handlePopRoute();
          await tester.pumpAndSettle();
        } else {
          await back();
          expect(dialog, findsNothing);
          if (desktop) {
            expect(Focus.of(tester.element(importIcon)).hasFocus, isTrue);
          }
        }
        expect(find.byType(GameDetail), findsOneWidget);
        expect(tester.takeException(), isNull);
      },
    );
  }
}
