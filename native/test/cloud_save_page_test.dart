import 'dart:async';
import 'dart:convert';
import 'dart:io';

import 'package:archive/archive.dart';
import 'package:crypto/crypto.dart';
import 'package:flutter/services.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:material_ui/material_ui.dart';
import 'package:shared_preferences/shared_preferences.dart';
import 'package:shared_preferences_platform_interface/in_memory_shared_preferences_async.dart';
import 'package:shared_preferences_platform_interface/shared_preferences_async_platform_interface.dart';
import 'package:touhouonline/account_api.dart';
import 'package:touhouonline/catalog.dart';
import 'package:touhouonline/cloud_save_comparison.dart';
import 'package:touhouonline/cloud_save_page.dart';
import 'package:touhouonline/cloud_saves.dart';
import 'package:touhouonline/desktop_masthead.dart';
import 'package:touhouonline/game_preferences.dart';
import 'package:touhouonline/game_settings.dart';
import 'package:touhouonline/game_user_files.dart';
import 'package:touhouonline/l10n/strings.dart';
import 'package:touhouonline/library.dart';
import 'package:touhouonline/save_progress.dart'
    show CloudGameSave, SaveStatistics;

import '../tools/capture_native_baseline_test.dart' show loadFonts, save;
import 'cloud_saves_test.dart' show cloudFixture, saveZip, testCloudGame;
import 'widget_test.dart' as launcher;

class _CloudApi extends launcher.FixtureAccountApi {
  _CloudApi({CloudSaveDirectory? directory, this.archive})
    : current = directory ?? CloudSaveDirectory.fromJson(cloudFixture());

  CloudSaveDirectory current;
  final Uint8List? archive;
  final requests = <int>[];
  final deletions = <int>[];
  final downloads = <int>[];
  bool rejectRead = false;
  int previewRequests = 0;

  @override
  Future<Map<String, CloudGameSave>> cloudGameSaves() async =>
      current.statistics;

  @override
  Future<CloudSaveDirectory> cloudSaveDirectory() async {
    if (!signedIn) throw StateError('Guest account used private route');
    if (rejectRead) {
      rejectRead = false;
      throw const AccountRequestException(503, 'SERVICE_UNAVAILABLE');
    }
    return current;
  }

  @override
  Future<SaveStatistics> previewCloudSave(
    String product,
    List<CloudFilePayload> files,
  ) async {
    expect(product, 'th07');
    expect(files.any((file) => file.name == 'score.dat'), isTrue);
    previewRequests++;
    return SaveStatistics.fromJson({'game': product, 'playtimeSeconds': 2580});
  }

  @override
  Future<CloudSaveSnapshot> uploadCloudSave(
    CloudSaveProduct product,
    List<CloudFilePayload> files,
  ) async {
    requests.add(product.revision);
    if (files.single.name != 'score.dat' ||
        utf8.decode(files.single.bytes) != 'cloud-th07-score') {
      throw StateError('Unexpected file upload');
    }
    final revision = product.revision + 1;
    final snapshot = current.saves[product.id]!;
    final next = CloudSaveSnapshot(
      product: product.id,
      record: (
        revision: revision,
        statistics: snapshot.record.statistics,
        uploadedAt: snapshot.record.uploadedAt,
      ),
      files: snapshot.files,
    );
    current = current.updated(product.withRevision(revision), next);
    if (requests.length == 1) {
      throw const AccountRequestException(409, 'SAVE_CONFLICT');
    }
    return next;
  }

  @override
  Future<Uint8List> downloadCloudSave(CloudSaveSnapshot snapshot) async {
    downloads.add(snapshot.record.revision);
    return archive ?? saveZip(utf8.encode('cloud-th07-score'));
  }

  @override
  Future<int> deleteCloudSave(CloudSaveProduct product) async {
    deletions.add(product.revision);
    final next = product.revision + 1;
    current = current.updated(product.withRevision(next), null);
    return next;
  }
}

Future<void> _until(WidgetTester tester, bool Function() predicate) async {
  final expires = DateTime.now().add(const Duration(seconds: 6));
  await tester.runAsync(() async {
    while (!predicate()) {
      if (DateTime.now().isAfter(expires)) fail('Cloud page did not load');
      await Future<void>.delayed(const Duration(milliseconds: 20));
      await tester.pump(const Duration(milliseconds: 40));
    }
  });
}

Future<void> _tap(WidgetTester tester, Finder control) async {
  await tester.ensureVisible(control);
  await tester.pump(const Duration(milliseconds: 240));
  expect(control.hitTestable(), findsOneWidget);
  await tester.tap(control);
  await tester.pump();
  await tester.pump(const Duration(milliseconds: 380));
}

void main() {
  final outputPath = Platform.environment['TOUHOU_CLOUD_CAPTURE'];
  setUpAll(() async {
    if (outputPath != null) {
      await loadFonts(Directory(Platform.environment['TOUHOU_CAPTURE_FONTS']!));
    }
  });
  setUp(
    () => SharedPreferencesAsyncPlatform.instance =
        InMemorySharedPreferencesAsync.empty(),
  );

  testWidgets('cloud comparison timestamp follows the device clock format', (
    tester,
  ) async {
    await tester.pumpWidget(
      MaterialApp(
        home: Column(
          children: [
            for (final flag in [false, true])
              MediaQuery(
                data: MediaQueryData(alwaysUse24HourFormat: flag),
                child: Builder(
                  builder: (context) => Text(
                    cloudSaveDateTime(context, DateTime(2026, 11, 25, 8, 20)),
                  ),
                ),
              ),
          ],
        ),
      ),
    );
    expect(find.text('11/25/2026 8:20 AM'), findsOneWidget);
    expect(find.text('11/25/2026 08:20'), findsOneWidget);
  });

  for (final (name, size, language, mutations) in [
    (
      'mobile-narrow',
      const Size(320, 640),
      LauncherLanguage.simplifiedChinese,
      true,
    ),
    ('mobile-narrow', const Size(320, 640), LauncherLanguage.english, true),
    ('mobile', const Size(390, 844), LauncherLanguage.japanese, true),
    ('desktop', const Size(1440, 734), LauncherLanguage.english, false),
    if (outputPath != null) ...[
      (
        'mobile',
        const Size(390, 844),
        LauncherLanguage.simplifiedChinese,
        false,
      ),
      (
        'desktop',
        const Size(1440, 734),
        LauncherLanguage.simplifiedChinese,
        false,
      ),
    ],
  ]) {
    testWidgets(
      '$name cloud page keeps the interface language | ${language.name}',
      (tester) async {
        tester.view.devicePixelRatio = 1;
        tester.view.physicalSize = size;
        addTearDown(tester.view.resetDevicePixelRatio);
        addTearDown(tester.view.resetPhysicalSize);
        final root = (await tester.runAsync(
          () => Directory.systemTemp.createTemp('native-cloud-page-'),
        ))!;
        addTearDown(() => root.delete(recursive: true));
        final strings = lookupLauncherStrings(language.locale);
        final locale = language.locale.languageCode;
        final suffix = locale == 'zh' ? '' : '-$locale';
        final api = _CloudApi()..rejectRead = locale == 'en' && mutations;
        final boundary = GlobalKey();
        final exported = <String>[];
        Completer<Directory>? pendingLocalRead;
        var readingLocal = false;
        final files = GameUserFiles(
          appDirectory: () async {
            if (pendingLocalRead case final pending?) {
              readingLocal = true;
              return pending.future;
            }
            return root;
          },
          multiPicker: (extensions) async => [
            (
              name: 'SCORE.DAT',
              length: () async => 'cloud-th07-score'.length,
              read: () => Stream.value(utf8.encode('cloud-th07-score')),
            ),
          ],
          exporter: (name, file) async {
            exported.add(name);
            return file.exists();
          },
        );
        await tester.pumpWidget(
          RepaintBoundary(
            key: boundary,
            child: launcher.fixture(
              accountApi: api,
              preferences: LauncherPreferences(
                SharedPreferencesAsync(),
                ThemeMode.dark,
                true,
                language: language,
              ),
              games: const [testCloudGame],
              gameUserFiles: files,
            ),
          ),
        );
        await tester.pumpAndSettle();
        await _tap(tester, find.byType(GameArtwork).last);
        await tester.pumpAndSettle();
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
        await tester.pumpAndSettle();
        await _tap(tester, find.text(strings.gameStorageSettings));
        await _tap(tester, find.widgetWithText(ListTile, strings.cloudSaves));
        expect(find.byType(CloudSavePage), findsOneWidget);
        await launcher.signInFixture(tester, settle: false);
        if (locale == 'en' && mutations) {
          final retry = find.text(strings.cloudReadFailed);
          await _until(tester, () => retry.evaluate().isNotEmpty);
          await _tap(tester, retry);
        }
        await _until(
          tester,
          () =>
              find.text(strings.cloudAccountSave).evaluate().isNotEmpty &&
              find.text(strings.cloudReadingLocal).evaluate().isEmpty,
        );
        await tester.pumpAndSettle();
        final duration = switch (locale) {
          'en' => '2 hr 10 min',
          'ja' => '2 時間 10 分',
          _ => '2 小时 10 分',
        };
        expect(find.text(duration), findsOneWidget);
        expect(find.text(strings.cloudUploadedAt), findsOneWidget);
        expect(
          tester.getSize(find.byKey(const ValueKey('cloud-row-th07'))).height,
          greaterThanOrEqualTo(62),
        );
        expect(
          find.ancestor(
            of: find.byType(SaveVersionCards),
            matching: find.byType(Card),
          ),
          findsNothing,
        );
        final cloudFiles = find.text('${strings.cloudFileList} (1)');
        expect(find.text('score.dat'), findsNothing);
        await _tap(tester, cloudFiles);
        expect(find.text('score.dat'), findsOneWidget);
        if (size.width == 1440) {
          final cards = find.descendant(
            of: find.byType(SaveVersionCards),
            matching: find.byType(Card),
          );
          final local = tester.getRect(cards.first);
          final cloud = tester.getRect(cards.last);
          expect(local.size, cloud.size);
          expect(local.top, cloud.top);
        }
        await _tap(tester, cloudFiles);
        expect(find.text('score.dat'), findsNothing);
        if (outputPath != null && size.width == 320 && locale == 'en') {
          final download = tester.getRect(
            find.widgetWithText(OutlinedButton, strings.gameDownload),
          );
          final restore = tester.getRect(
            find.widgetWithText(OutlinedButton, strings.cloudRestore),
          );
          final local = tester.getRect(
            find.widgetWithText(OutlinedButton, strings.cloudChooseLocal),
          );
          final files = tester.getRect(
            find.widgetWithText(OutlinedButton, strings.cloudChooseFiles),
          );
          expect(download.height, 44);
          expect(restore.height, download.height);
          expect(restore.center.dy, download.center.dy);
          expect(local.height, files.height);
          expect(local.center.dy, files.center.dy);
        }
        final versionCards = find.descendant(
          of: find.byType(SaveVersionCards),
          matching: find.byType(Card),
        );
        if (size.width <= 780) {
          expect(
            tester.getTopLeft(versionCards.last).dy -
                tester.getBottomLeft(versionCards.first).dy,
            12,
          );
        }
        if (outputPath != null) {
          final output = Directory(outputPath)..createSync(recursive: true);
          final position = Scrollable.of(tester.element(cloudFiles)).position;
          position.jumpTo(position.minScrollExtent);
          await tester.pumpAndSettle();
          List<double> rect(Finder item) {
            final bounds = tester.getRect(item);
            return [bounds.left, bounds.top, bounds.width, bounds.height];
          }

          final cards = find.descendant(
            of: find.byType(SaveVersionCards),
            matching: find.byType(Card),
          );
          final geometry = {
            'viewport': [size.width, size.height],
            'row': rect(find.byKey(const ValueKey('cloud-row-th07'))),
            'local': rect(cards.first),
            'cloud': rect(cards.last),
          };
          File('${output.path}/$name-cloud-saves$suffix.geometry.json')
              .writeAsStringSync('${jsonEncode(geometry)}\n');
          await save(tester, boundary, output, '$name-cloud-saves$suffix');
          await _tap(tester, cloudFiles);
          await save(tester, boundary, output, '$name-cloud-files$suffix');
          await _tap(tester, cloudFiles);
        }
        if (locale == 'en' && size.width == 320) {
          tester.platformDispatcher.textScaleFactorTestValue = 2;
          addTearDown(tester.platformDispatcher.clearTextScaleFactorTestValue);
          await tester.pumpAndSettle();
          expect(tester.takeException(), isNull);
          tester.platformDispatcher.clearTextScaleFactorTestValue();
          await tester.pumpAndSettle();
        }
        if (locale == 'en') {
          final workspace = files.workspace(
            testCloudGame,
            multiplayer: false,
            language: 'jp',
          )!;
          final directory = (await tester.runAsync(() async {
            final directory = await workspace.directory();
            return directory.create(recursive: true);
          }))!;
          final refresh = find.byTooltip(strings.cloudRefresh);
          if (mutations) {
            await tester.runAsync(() async {
              await File.fromUri(directory.uri.resolve('th07.cfg'))
                  .writeAsString('config-only');
            });
            await _tap(tester, refresh);
            await _until(
              tester,
              () => find.text(strings.cloudLocalModified).evaluate().isNotEmpty,
            );
            await _tap(tester, find.text(strings.cloudChooseLocal));
            expect(find.text(strings.cloudMissingLocalSave), findsOneWidget);
            expect(find.text(strings.cloudClearChoice), findsNothing);
            expect(api.requests, isEmpty);
          }
          final score = File.fromUri(directory.uri.resolve('score.dat'));
          await tester.runAsync(
            () => score.writeAsString('local-th07-original'),
          );
          await _tap(tester, refresh);
          final preview = find.text(strings.cloudPreviewLocal);
          await _until(tester, () => preview.evaluate().isNotEmpty);
          await _tap(tester, preview);
          final parsedTime = find.text(strings.formatPlaytime(2580));
          await _until(tester, () => parsedTime.evaluate().isNotEmpty);
          final localCard = find
              .descendant(
                of: find.byType(SaveVersionCards),
                matching: find.byType(Card),
              )
              .first;
          final localFiles = find.descendant(
            of: localCard,
            matching: find.byType(ExpansionTile),
          );
          await _tap(tester, localFiles);
          final localName = find.descendant(
            of: localCard,
            matching: find.text('score.dat'),
          );
          expect(localName, findsOneWidget);
          final scroll = Scrollable.of(tester.element(localName)).position;
          if (size.width == 320) {
            scroll.jumpTo(64);
            await tester.pumpAndSettle();
          }
          final before = scroll.pixels;
          final row = tester.getRect(localName);
          final progress = find.byType(LinearProgressIndicator);
          final refreshButton = find.descendant(
            of: refresh,
            matching: find.byIcon(Icons.refresh),
          );
          final refreshFocus = Focus.of(tester.element(refreshButton));
          refreshFocus.requestFocus();
          await tester.pump();
          final pending = pendingLocalRead = Completer<Directory>();
          await tester.sendKeyEvent(LogicalKeyboardKey.enter);
          await tester.pump();
          await _until(tester, () => readingLocal);
          expect(progress, findsOneWidget);
          expect(parsedTime, findsOneWidget);
          expect(tester.getRect(localName), row);
          expect(scroll.pixels, before);
          if (outputPath != null) {
            await save(
              tester,
              boundary,
              Directory(outputPath),
              '$name-cloud-refresh-en',
            );
          }
          pending.complete(root);
          pendingLocalRead = null;
          await _until(tester, () => progress.evaluate().isEmpty);
          await tester.pumpAndSettle();
          expect(parsedTime, findsOneWidget);
          expect(tester.getRect(localName), row);
          expect(scroll.pixels, before);
          expect(refreshFocus.hasPrimaryFocus, isTrue);
          expect(api.previewRequests, 1);
          if (outputPath != null) {
            await save(
              tester,
              boundary,
              Directory(outputPath),
              '$name-cloud-refreshed-en',
            );
          }
          await tester.runAsync(
            () => score.writeAsString('local-th07-replaced'),
          );
          await _tap(tester, refresh);
          await _until(tester, () => progress.evaluate().isEmpty);
          expect(parsedTime, findsNothing);
          expect(preview, findsOneWidget);
          expect(localName, findsOneWidget);
        }
        if (mutations) {
          await _tap(tester, find.text(strings.cloudChooseFiles));
          await _until(
            tester,
            () => find.text(strings.cloudUploadParse).evaluate().isNotEmpty,
          );
          await _tap(
            tester,
            find.widgetWithText(FilledButton, strings.cloudUploadParse),
          );
          await _tap(
            tester,
            find.descendant(
              of: find.byType(AlertDialog),
              matching: find.text(strings.cloudUploadParse),
            ),
          );
          await _until(
            tester,
            () =>
                find.text(strings.cloudConflict).evaluate().isNotEmpty &&
                find.byType(LinearProgressIndicator).evaluate().isEmpty,
          );
          expect(api.requests, [3]);
          expect(find.text(strings.cloudClearChoice), findsOneWidget);
          final submit = find.widgetWithText(
            FilledButton,
            strings.cloudUploadParse,
          );
          await _until(
            tester,
            () => tester.widget<FilledButton>(submit).onPressed != null,
          );
          if (outputPath != null && locale != 'zh') {
            await tester.ensureVisible(find.text(strings.cloudConflict));
            await tester.pumpAndSettle();
            await save(
              tester,
              boundary,
              Directory(outputPath),
              '$name-cloud-conflict$suffix',
            );
          }
          await tester.binding.handlePopRoute();
          await tester.pump();
          await tester.pump(const Duration(milliseconds: 380));
          expect(find.text(strings.cloudDiscardTitle), findsOneWidget);
          await _tap(tester, find.text(strings.cancel));
          expect(find.text(strings.cloudClearChoice), findsOneWidget);
          await _tap(tester, submit);
          await _tap(
            tester,
            find.descendant(
              of: find.byType(AlertDialog),
              matching: find.text(strings.cloudUploadParse),
            ),
          );
          await _until(
            tester,
            () =>
                find.text(strings.cloudUploaded).evaluate().isNotEmpty &&
                find.byType(LinearProgressIndicator).evaluate().isEmpty,
          );
          expect(api.requests, [3, 4]);
          expect(find.text(strings.cloudClearChoice), findsNothing);
          final download = find.widgetWithText(
            OutlinedButton,
            strings.gameDownload,
          );
          await _until(
            tester,
            () => tester.widget<OutlinedButton>(download).onPressed != null,
          );
          await _tap(tester, download);
          await _until(
            tester,
            () => find.text(strings.cloudExported).evaluate().isNotEmpty,
          );
          expect(exported.single, 'th07-save-r5.zip');
          await _tap(tester, find.text(strings.cloudRestore));
          await _tap(tester, find.text(strings.cloudRestoreAction));
          await _until(
            tester,
            () => find.text(strings.cloudIdentical).evaluate().isNotEmpty,
          );
          final delete = find.widgetWithText(TextButton, strings.cloudDelete);
          await _until(
            tester,
            () => tester.widget<TextButton>(delete).onPressed != null,
          );
          await _tap(tester, delete);
          await _tap(tester, find.text(strings.cancel));
          expect(api.deletions, isEmpty);
          await _tap(tester, find.text(strings.cloudDelete));
          await _tap(
            tester,
            find.descendant(
              of: find.byType(AlertDialog),
              matching: find.text(strings.gameFileDelete),
            ),
          );
          await _until(
            tester,
            () => find.text(strings.cloudDeleted).evaluate().isNotEmpty,
          );
          expect(api.deletions, [5]);
        }
        if (!mutations && locale == 'en') {
          await _tap(tester, find.text(strings.cloudChooseFiles));
          await _until(
            tester,
            () =>
                find.text(strings.cloudClearChoice).evaluate().isNotEmpty &&
                find.byType(LinearProgressIndicator).evaluate().isEmpty,
          );
          final library = find.descendant(
            of: find.byType(DesktopMasthead),
            matching: find.text(strings.singlePlayer),
          );
          for (var attempt = 0; attempt < 2; attempt++) {
            await _tap(tester, library);
            expect(find.text(strings.cloudDiscardTitle), findsOneWidget);
            await _tap(tester, find.text(strings.cancel));
            expect(find.text(strings.cloudClearChoice), findsOneWidget);
          }
          await _tap(tester, library);
          await _tap(
            tester,
            find.descendant(
              of: find.byType(AlertDialog),
              matching: find.text(strings.cloudDiscard),
            ),
          );
          expect(find.byType(CloudSavePage), findsNothing);
          expect(find.byType(GameSettingsPage), findsNothing);
          expect(find.byType(SearchBar), findsOneWidget);
          expect(tester.takeException(), isNull);
          return;
        }
        final refresh = find.widgetWithIcon(IconButton, Icons.refresh);
        await _until(
          tester,
          () => tester.widget<IconButton>(refresh).onPressed != null,
        );
        await tester.binding.handlePopRoute();
        await tester.pumpAndSettle();
        expect(find.byType(CloudSavePage), findsNothing);
        expect(find.byType(GameSettingsPage), findsOneWidget);
        expect(tester.takeException(), isNull);
      },
      variant: TargetPlatformVariant.only(TargetPlatform.windows),
    );
  }

  for (final (width, uiLanguage, cloudLanguage, cloudName) in [
    (320.0, LauncherLanguage.english, 'ja', 'scoreth10.dat'),
    (
      1440.0,
      LauncherLanguage.simplifiedChinese,
      'lang_zh-hans',
      'scoreth10c.dat',
    ),
  ]) {
    testWidgets('th10 restore follows the game language at $width px', (
      tester,
    ) async {
      tester.view.devicePixelRatio = 1;
      tester.view.physicalSize = Size(width, width == 320 ? 640 : 734);
      addTearDown(tester.view.resetDevicePixelRatio);
      addTearDown(tester.view.resetPhysicalSize);
      final root = (await tester.runAsync(
        () => Directory.systemTemp.createTemp('native-cloud-th10-'),
      ))!;
      addTearDown(() => root.delete(recursive: true));
      final games = (await tester.runAsync(Game.load))!;
      final game = games.singleWhere((game) => game.id == 'th10');
      final strings = lookupLauncherStrings(uiLanguage.locale);
      final storage = SharedPreferencesAsync();
      final preferences = GamePreferenceStore(storage);
      final otherLanguage = cloudLanguage == 'ja' ? 'lang_zh-hans' : 'ja';
      await preferences.save(
        game,
        'language',
        otherLanguage,
        multiplayer: false,
      );
      final bytes = utf8.encode('cloud sample for $cloudLanguage');
      final product = CloudSaveProduct(
        id: game.id,
        revision: 1,
        files: const ['scoreth10.dat', 'scoreth10c.dat'],
        requiredFiles: const ['scoreth10.dat', 'scoreth10c.dat'],
      );
      final snapshot = CloudSaveSnapshot.fromJson({
        'product': game.id,
        'revision': 1,
        'uploadedAt': '2026-11-24T10:00:00Z',
        'files': [
          {
            'name': cloudName,
            'bytes': bytes.length,
            'sha256': sha256.convert(bytes).toString(),
          },
        ],
        'statistics': {'game': game.id, 'parserVersion': 1},
      }, product);
      final api = _CloudApi(
        directory: CloudSaveDirectory(
          {game.id: product},
          {game.id: snapshot},
          const CloudSaveLimits(),
        ),
        archive: ZipEncoder().encodeBytes(
          Archive()..addFile(ArchiveFile.bytes(cloudName, bytes)),
        ),
      );
      final exported = <String>[];
      final files = GameUserFiles(
        appDirectory: () async => root,
        exporter: (name, file) async {
          expect(await file.readAsBytes(), api.archive);
          exported.add(name);
          return true;
        },
      );
      final (matchingFile, otherFile) = (await tester.runAsync(() async {
        Future<File> seed(String language) async {
          final workspace = files.workspace(
            game,
            multiplayer: false,
            language: language,
          )!;
          final directory = await workspace.directory();
          await directory.create(recursive: true);
          return File.fromUri(directory.uri.resolve(game.files!.score))
              .writeAsString('local sample for $language');
        }

        return (await seed(cloudLanguage), await seed(otherLanguage));
      }))!;
      final boundary = GlobalKey();
      await tester.pumpWidget(
        RepaintBoundary(
          key: boundary,
          child: launcher.fixture(
            initialLocation: '/game/th10/settings',
            accountApi: api,
            gamePreferences: preferences,
            preferences: LauncherPreferences(
              storage,
              ThemeMode.dark,
              true,
              language: uiLanguage,
            ),
            games: [game],
            gameUserFiles: files,
          ),
        ),
      );
      await tester.pumpAndSettle();
      Future<void> openCloud() async {
        final cloud = find.widgetWithText(ListTile, strings.cloudSaves);
        if (cloud.evaluate().isEmpty) {
          await _tap(tester, find.text(strings.gameStorageSettings));
        }
        await _tap(tester, cloud);
      }

      await openCloud();
      await launcher.signInFixture(tester, settle: false);
      final notice = find.text(strings.fileIssueCloudMainSaveMissing);
      final refresh = find.widgetWithIcon(IconButton, Icons.refresh);
      await _until(
        tester,
        () =>
            notice.evaluate().isNotEmpty &&
            tester.widget<IconButton>(refresh).onPressed != null,
      );
      final restore = find.widgetWithText(OutlinedButton, strings.cloudRestore);
      expect(tester.widget<OutlinedButton>(restore).onPressed, isNull);
      expect(api.downloads, isEmpty);
      final download = find.widgetWithText(
        OutlinedButton,
        width == 320 ? strings.gameDownload : strings.cloudDownload,
      );
      if (outputPath != null) {
        await tester.ensureVisible(restore);
        await tester.pumpAndSettle();
        await save(
          tester,
          boundary,
          Directory(outputPath),
          'th10-cloud-language-${width.toInt()}',
        );
      }
      await _tap(tester, download);
      await _until(tester, () => exported.isNotEmpty);
      expect(exported.single, 'th10-save-r1.zip');
      expect(api.downloads, [1]);
      expect(
        await tester.runAsync(otherFile.readAsString),
        'local sample for $otherLanguage',
      );

      // Export notification precedes temporary-file cleanup and releasing Back.
      await _until(
        tester,
        () => find.byType(LinearProgressIndicator).evaluate().isEmpty,
      );
      await tester.binding.handlePopRoute();
      await tester.pumpAndSettle();
      expect(find.byType(GameSettingsPage), findsOneWidget);
      final languagePicker = find.byKey(const ValueKey('language'));
      if (languagePicker.evaluate().isEmpty) {
        await _tap(tester, find.text(strings.gameDisplaySettings));
      }
      await _tap(tester, languagePicker);
      final pack = game.settings.languages.singleWhere(
        (pack) => pack.id == cloudLanguage,
      );
      await _tap(tester, find.text(pack.title).last);
      await openCloud();
      await _until(
        tester,
        () =>
            restore.evaluate().isNotEmpty &&
            tester.widget<OutlinedButton>(restore).onPressed != null,
      );
      expect(notice, findsNothing);
      await _tap(tester, restore);
      expect(find.text(strings.cloudRestoreDetail), findsOneWidget);
      await _tap(tester, find.text(strings.cloudRestoreAction));
      await _until(
        tester,
        () => find.text(strings.cloudRestored).evaluate().isNotEmpty,
      );
      expect(api.downloads, [1, 1]);
      expect(await tester.runAsync(matchingFile.readAsBytes), bytes);
      expect(
        await tester.runAsync(otherFile.readAsString),
        'local sample for $otherLanguage',
      );
      expect(tester.takeException(), isNull);
    });
  }
}
