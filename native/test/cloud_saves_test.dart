import 'dart:convert';
import 'dart:io';
import 'dart:typed_data';

import 'package:archive/archive.dart';
import 'package:crypto/crypto.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:touhouonline/account_api.dart';
import 'package:touhouonline/catalog.dart';
import 'package:touhouonline/cloud_saves.dart';
import 'package:touhouonline/game_file_archive.dart';
import 'package:touhouonline/game_user_files.dart';

const testCloudGame = Game(
  id: 'th07',
  number: '07',
  title: '東方妖々夢',
  subtitle: 'Perfect Cherry Blossom',
  category: BookCategory.windows,
  nameOrder: 0,
  multiplayer: true,
  artwork: 'th07.webp',
  files: GameFilesCatalog(
    score: 'score.dat',
    normal: ReplayCatalog(namespace: 'savesth07', prefix: 'th7'),
  ),
);

Map<String, dynamic> cloudFixture() =>
    jsonDecode(File('tools/game-progress-fixture.json').readAsStringSync())
        as Map<String, dynamic>;

Uint8List saveZip(List<int> contents) => ZipEncoder().encodeBytes(
  Archive()..addFile(ArchiveFile.bytes('score.dat', contents)),
);

void main() {
  test(
    'cloud comparison detects changed bytes independently of file order',
    () async {
      final files = <CloudFilePayload>[
        (
          name: 'score.dat',
          bytes: Uint8List.fromList([1, 2, 3]),
          modifiedAt: null,
        ),
        (
          name: 'autosave/save0_0.dat',
          bytes: Uint8List.fromList([4, 5]),
          modifiedAt: null,
        ),
      ];
      final cloud = <CloudFileInfo>[
        for (final file in files)
          (
            name: file.name,
            size: file.bytes.length,
            sha: sha256.convert(file.bytes).toString(),
          ),
      ];
      expect(await compareCloudFiles(files, cloud, files.reversed.toList()), (
        matchesCloud: true,
        unchanged: true,
      ));
      final changed = <CloudFilePayload>[
        (
          name: 'score.dat',
          bytes: Uint8List.fromList([1, 2, 9]),
          modifiedAt: null,
        ),
        files.last,
      ];
      expect(await compareCloudFiles(changed, cloud, files), (
        matchesCloud: false,
        unchanged: false,
      ));
      expect(await compareCloudFiles(files.take(1).toList(), cloud, files), (
        matchesCloud: false,
        unchanged: false,
      ));
      expect(await compareCloudFiles(files, null, files), (
        matchesCloud: false,
        unchanged: true,
      ));
      expect(await compareCloudFiles(const [], const [], const []), (
        matchesCloud: false,
        unchanged: true,
      ));
    },
  );

  test('cloud transport uses the same authenticated session and revision on all operations', () async {
    final fixture = cloudFixture();
    final valid = utf8.encode('cloud-th07-score');
    final latest = (fixture['saves'] as List).last as Map<String, dynamic>;
    final listener = await HttpServer.bind(InternetAddress.loopbackIPv4, 0);
    addTearDown(() => listener.close(force: true));
    final origin = Uri.http('127.0.0.1:${listener.port}');
    final api = AccountApi(origin: origin);
    addTearDown(api.close);
    final received =
        <
          ({
            String method,
            String path,
            String? origin,
            String? cookie,
            String? type,
            String body,
          })
        >[];
    var stale = true;
    final z = saveZip(valid);
    listener.listen((request) async {
      final bytes = await request.fold<List<int>>(
        [],
        (all, next) => all..addAll(next),
      );
      final reply = request.response;
      final path = request.uri.path;
      received.add((
        method: request.method,
        path: path,
        origin: request.headers.value('Origin'),
        cookie: request.headers.value(HttpHeaders.cookieHeader),
        type: request.headers.contentType?.mimeType,
        body: latin1.decode(bytes),
      ));
      const route = '/api/v1/me/cloud-saves';
      reply.headers.contentType = ContentType.json;
      Object payload;
      switch (path) {
        case '/api/auth/sign-in/username':
          reply.cookies.add(
            Cookie('better-auth.session_token', 'cloud-session'),
          );
          payload = <String, Object>{};
        case '/api/auth/get-session':
          payload = {
            'user': {'username': 'fixture', 'name': '玩家'},
          };
        case route:
          payload = fixture;
        case '$route/th07/preview':
          payload = {'product': 'th07', 'statistics': latest['statistics']};
        case '$route/th07':
          if (stale) {
            stale = false;
            reply.statusCode = HttpStatus.conflict;
            payload = {'code': 'SAVE_CONFLICT'};
          } else {
            payload = {...latest, 'revision': 4};
          }
        case '$route/th07/4/download':
          reply.headers.contentType = ContentType('application', 'zip');
          reply.add(z);
          await reply.close();
          return;
        case '$route/th07/delete':
          payload = {'revision': 5};
        default:
          reply.statusCode = HttpStatus.notFound;
          payload = {};
      }
      reply.write(jsonEncode(payload));
      await reply.close();
    });

    await api.signIn('fixture', 'a-valid-fixture-value');
    final directory = await api.cloudSaveDirectory();
    final product = directory.products['th07']!;
    final file = (
      name: 'score.dat',
      bytes: Uint8List.fromList(valid),
      modifiedAt: null as DateTime?,
    );
    expect(product.revision, 3);
    expect(directory.saves['th07']!.files.single.size, valid.length);
    expect((await api.previewCloudSave(product.id, [file])).playtime, 7800);
    await expectLater(
      api.uploadCloudSave(product, [file]),
      throwsA(
        isA<AccountRequestException>().having(
          (e) => e.code,
          'code',
          'SAVE_CONFLICT',
        ),
      ),
    );
    final saved = await api.uploadCloudSave(product, [file]);
    expect(saved.record.revision, 4);
    expect(await api.downloadCloudSave(saved), z);
    expect(await api.deleteCloudSave(product.withRevision(4)), 5);
    expect(received.skip(2).map((row) => row.cookie).toSet(), {
      'better-auth.session_token=cloud-session',
    });
    expect(
      received
          .where(
            (row) => row.path.contains('cloud-saves') && row.method == 'POST',
          )
          .every((row) => row.origin == origin.origin),
      isTrue,
    );
    final preview = received.firstWhere((row) => row.path.endsWith('/preview'));
    expect(preview.type, 'multipart/form-data');
    expect(preview.body, contains('filename="score.dat"'));
    expect(preview.body, contains('cloud-th07-score'));
    expect(preview.body, isNot(contains('baseRevision')));
    final posted = received.where((row) => row.path.endsWith('/th07'));
    expect(posted.length, 2);
    for (final row in posted) {
      expect(row.body, contains('name="baseRevision"\r\n\r\n3\r\n'));
      expect(row.body, contains('cloud-th07-score'));
    }
    expect(jsonDecode(received.last.body), {'baseRevision': 4});
  });

  test('verified restore preserves original bytes on failure and clears only cloud-managed files', () async {
    final temporary = await Directory.systemTemp.createTemp(
      'native-cloud-restore-',
    );
    addTearDown(() => temporary.delete(recursive: true));
    final files = GameUserFiles(appDirectory: () async => temporary);
    final workspace = files.workspace(
      testCloudGame,
      multiplayer: false,
      language: 'ja',
    )!;
    final root = await (await workspace.directory()).create(recursive: true);
    final score = File.fromUri(root.uri.resolve('score.dat'));
    final config = File.fromUri(root.uri.resolve('th07.cfg'));
    final replay = File.fromUri(root.uri.resolve('replay/th7_01.rpy'));
    await score.writeAsString('original bytes');
    await config.writeAsString('old configuration');
    await replay.parent.create(recursive: true);
    await replay.writeAsString('separate replay data');
    final directory = CloudSaveDirectory.fromJson(cloudFixture());
    final product = directory.products['th07']!;
    final snapshot = directory.saves['th07']!;
    final z = saveZip(utf8.encode('cloud-th07-score'));
    final wrong = saveZip(utf8.encode('cloud-th07-wrong'));
    await expectLater(
      workspace.restoreCloudArchive(
        product,
        snapshot,
        wrong,
        isActive: () => true,
      ),
      throwsA(isA<GameFileError>()),
    );
    expect(await score.readAsString(), 'original bytes');
    expect(await config.exists(), isTrue);
    var checks = 0;
    await expectLater(
      workspace.restoreCloudArchive(
        product,
        snapshot,
        z,
        isActive: () => ++checks < 2,
      ),
      throwsA(isA<GameFileError>()),
    );
    expect(await score.readAsString(), 'original bytes');
    expect(await config.exists(), isTrue);
    await workspace.restoreCloudArchive(
      product,
      snapshot,
      z,
      isActive: () => true,
    );
    expect(await score.readAsString(), 'cloud-th07-score');
    expect(await config.exists(), isFalse);
    expect(await replay.readAsString(), 'separate replay data');
    final local = await workspace.cloudLocalFiles(product, directory.limits);
    expect(local.single.name, 'score.dat');
    expect(
      sha256.convert(local.single.bytes).toString(),
      snapshot.files.single.sha,
    );
  });

  final th09 = Platform.environment['TOUHOU_CAPTURE_TH09_SCORE'];
  test('verified cloud ZIP round-trips a real th09 save', () async {
    final temporary = await Directory.systemTemp.createTemp(
      'native-cloud-th09-',
    );
    addTearDown(() => temporary.delete(recursive: true));
    final catalog = jsonDecode(
      await File('assets/game_catalog.json').readAsString(),
    ) as List;
    final games = catalog.cast<Map<String, dynamic>>().map(Game.fromJson);
    final game = games.singleWhere((game) => game.id == 'th09');
    final files = GameUserFiles(appDirectory: () async => temporary);
    final workspace = files.workspace(
      game,
      multiplayer: false,
      language: 'ja',
    )!;
    final input = await File(th09!).readAsBytes();
    final directory = cloudFixture();
    directory['products'] = [
      {
        'id': 'th09',
        'game': 'th09',
        'multiplayer': false,
        'revision': 1,
        'files': ['score.dat', 'th09.cfg'],
        'requiredFiles': ['score.dat'],
      },
    ];
    directory['saves'] = [
      {
        'product': 'th09',
        'revision': 1,
        'uploadedAt': '2026-11-25T08:00:00Z',
        'statistics': {'game': 'th09', 'parserVersion': 1},
        'files': [
          {
            'name': 'score.dat',
            'bytes': input.length,
            'sha256': sha256.convert(input).toString(),
          },
        ],
      },
    ];
    final result = CloudSaveDirectory.fromJson(directory);
    await workspace.restoreCloudArchive(
      result.products['th09']!,
      result.saves['th09']!,
      saveZip(input),
      isActive: () => true,
    );
    final payload = await workspace.cloudLocalFiles(
      result.products['th09']!,
      result.limits,
    );
    expect(payload.single.bytes, input);
  }, skip: th09 == null);
}
