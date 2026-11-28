import 'dart:convert';
import 'dart:io';

import 'package:archive/archive.dart';

import 'package:crypto/crypto.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:touhouonline/catalog.dart';
import 'package:touhouonline/game_file_picker.dart';
import 'package:touhouonline/game_resources.dart';

GameFileInput localData(File file) => (
  name: file.uri.pathSegments.last,
  length: file.length,
  read: file.openRead,
);

class _RealNetwork extends HttpOverrides {}

void main() {
  TestWidgetsFlutterBinding.ensureInitialized();

  test(
    'Import release DATA, reject corrupted replacement, reopen the original',
    () async {
      final scratch = Directory(
        Platform.environment['TOUHOU_CAPTURE_SCRATCH']!,
      );
      final source = File(Platform.environment['TOUHOU_CAPTURE_TH06_DATA']!);
      final games = await Game.load();
      final game = games.singleWhere((entry) => entry.id == 'th06');
      final data = game.data!;
      final temporary = await scratch.createTemp('data-regression-');
      try {
        final resources = GameResources(appDirectory: () async => temporary);
        expect(await resources.installed(data), isNull);
        final progress = <DataImportProgress>[];
        final installed = await resources.importData(
          game,
          localData(source),
          progress: progress.add,
        );
        expect(progress.last.phase, DataImportPhase.verifying);
        expect(progress.any((event) => event.percent == 100), isTrue);
        expect(
          (await sha256.bind(installed.openRead()).first).toString(),
          data.sha256,
        );

        final bad = await temporary.createTemp('input-');
        final altered = await File(source.path)
            .copy('${bad.path}/${data.name}');
        final writer = await altered.open(mode: FileMode.append);
        try {
          final originalByte = await installed.openRead(0, 1).first;
          await writer.setPosition(0);
          await writer.writeByte(originalByte.single ^ 1);
        } finally {
          await writer.close();
        }
        expect(await altered.length(), data.bytes);
        await expectLater(
          resources.importData(game, localData(altered), progress: (_) {}),
          throwsA(
            isA<GameDataImportException>().having(
              (error) => error.reason,
              'reason',
              DataImportError.checksum,
            ),
          ),
        );
        final reopened = await GameResources(
          appDirectory: () async => temporary,
        ).installed(data);
        expect(reopened?.path, installed.path);
        expect(
          (await sha256.bind(reopened!.openRead()).first).toString(),
          data.sha256,
        );
        final children = await installed.parent.list().toList();
        expect(children, hasLength(1));
        expect(
          await FileSystemEntity.identical(
            children.single.path,
            installed.path,
          ),
          isTrue,
        );
      } finally {
        await temporary.delete(recursive: true);
      }
    },
  );

  final zipPath = Platform.environment['TOUHOU_CAPTURE_RELEASE_ZIP'];
  test(
    'Extract release ZIP and compatible legacy ZIP into the same verified DATA',
    () async {
      final scratch = Directory(
        Platform.environment['TOUHOU_CAPTURE_SCRATCH']!,
      );
      final temporary = await scratch.createTemp('zip-regression-');
      addTearDown(() => temporary.delete(recursive: true));
      final game = (await Game.load()).singleWhere(
        (entry) => entry.id == 'th06',
      );
      final data = game.data!;
      final resources = GameResources(appDirectory: () async => temporary);
      final progress = <DataImportProgress>[];
      final installed = await resources.importData(
        game,
        localData(File(zipPath!)),
        progress: progress.add,
      );
      expect(
        progress.map((event) => event.phase),
        contains(DataImportPhase.extracting),
      );
      expect(
        (await sha256.bind(installed.openRead()).first).toString(),
        data.sha256,
      );

      final bytes = await installed.readAsBytes();
      final manifest = {
        'schema': 'eagler-touhou/game-data-pack/1',
        'game': game.id,
        'data': {'path': data.name, 'bytes': data.bytes, 'sha256': data.sha256},
      };
      File archive(
        Map<String, Object> manifest,
        List<int> bytes, {
        String? extra,
      }) {
        final zip = Archive()
          ..addFile(ArchiveFile.string('manifest.json', jsonEncode(manifest)))
          ..addFile(ArchiveFile(data.name, bytes.length, bytes));
        if (extra != null) zip.addFile(ArchiveFile.string(extra, 'ignored'));
        return File.fromUri(temporary.uri.resolve('sample.zip'))
          ..writeAsBytesSync(ZipEncoder().encode(zip, level: 0));
      }

      await expectLater(
        resources.importData(
          game,
          localData(archive({...manifest, 'game': 'th07'}, bytes)),
          progress: (_) {},
        ),
        throwsA(
          isA<GameDataImportException>().having(
            (e) => e.reason,
            'reason',
            DataImportError.wrongGame,
          ),
        ),
      );
      await expectLater(
        resources.importData(
          game,
          localData(archive(manifest, bytes, extra: '../sample.txt')),
          progress: (_) {},
        ),
        throwsA(
          isA<GameDataImportException>().having(
            (e) => e.reason,
            'reason',
            DataImportError.archive,
          ),
        ),
      );
      bytes[0] ^= 1;
      await expectLater(
        resources.importData(
          game,
          localData(archive(manifest, bytes)),
          progress: (_) {},
        ),
        throwsA(
          isA<GameDataImportException>().having(
            (e) => e.reason,
            'reason',
            DataImportError.checksum,
          ),
        ),
      );
      expect(
        (await sha256.bind(installed.openRead()).first).toString(),
        data.sha256,
      );
      bytes[0] ^= 1;
      final legacy = await resources.importData(
        game,
        localData(archive(manifest, bytes)),
        progress: (_) {},
      );
      expect(legacy.path, installed.path);
      expect(
        (await installed.parent.list().toList()).single.path,
        installed.path,
      );
    },
    skip: zipPath == null,
  );

  final siteSnapshot = Platform.environment['TOUHOU_PACKAGE_MANIFEST'];
  test('Read the published download reference over HTTP', () async {
    final raw = await File(siteSnapshot!).readAsString();
    final manifest = jsonDecode(raw) as Map;
    final expected = manifest['shared']['gameDataFallback'] as Map;
    final server = await HttpServer.bind(InternetAddress.loopbackIPv4, 0);
    addTearDown(() => server.close(force: true));
    server.listen((request) {
      expect(request.uri.path, '/site-manifest.json');
      request.response
        ..headers.contentType = ContentType.json
        ..write(raw)
        ..close();
    });
    final origin = Uri.http('127.0.0.1:${server.port}');
    final reference = await HttpOverrides.runWithHttpOverrides(
      () => GameResources(origin: origin).reference(),
      _RealNetwork(),
    );
    expect(reference?.address.toString(), expected['url']);
    expect(reference?.hint, expected['hint']);
  }, skip: siteSnapshot == null);
}
