import 'dart:io';

import 'package:crypto/crypto.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:touhouonline/catalog.dart';
import 'package:touhouonline/game_file_archive.dart';
import 'package:touhouonline/game_file_picker.dart';
import 'package:touhouonline/game_user_files.dart';

GameFileInput localFile(File file) => (
  name: file.uri.pathSegments.last,
  length: file.length,
  read: file.openRead,
);

Future<Digest> checksum(File file) => sha256.bind(file.openRead()).first;

void main() {
  TestWidgetsFlutterBinding.ensureInitialized();

  test(
    'Round trip real th09 save and replay files in isolated namespaces',
    () async {
      final scratch = Directory(
        Platform.environment['TOUHOU_CAPTURE_SCRATCH']!,
      );
      final score = File(Platform.environment['TOUHOU_CAPTURE_TH09_SCORE']!);
      final replay = File(Platform.environment['TOUHOU_CAPTURE_TH09_REPLAY']!);
      final games = await Game.load();
      final th09 = games.singleWhere((game) => game.id == 'th09');
      final th10 = games.singleWhere((game) => game.id == 'th10');
      final th15 = games.singleWhere((game) => game.id == 'th15');
      final temporary = await scratch.createTemp('file-regression-');
      final downloads = await Directory.fromUri(
        temporary.uri.resolve('downloads/'),
      ).create();
      File? exported;
      final files = GameUserFiles(
        appDirectory: () async => temporary,
        exporter: (name, file) async {
          exported = await file.copy(
            File.fromUri(downloads.uri.resolve(name)).path,
          );
          return true;
        },
      );

      try {
        final solo = files.workspace(th09, multiplayer: false, language: 'ja')!;
        final lobby = files.workspace(th09, multiplayer: true, language: 'ja')!;
        expect(await solo.exportSave(), isNull);
        expect(await solo.exportReplays(), isNull);
        expect(await solo.importFile(GameUserFileKind.save, localFile(score)), [
          'score.dat',
        ]);
        expect(await solo.exportSave(), isTrue);
        expect(await checksum(exported!), await checksum(score));
        expect(await lobby.exportSave(), isNull);

        final incomplete = (
          name: 'score.dat',
          length: () async => await score.length() + 1,
          read: score.openRead,
        );
        await expectLater(
          solo.importFile(GameUserFileKind.save, incomplete),
          throwsA(isA<GameFileError>()),
        );
        expect(await solo.exportSave(), isTrue);
        expect(await checksum(exported!), await checksum(score));

        expect(
          await solo.importFile(GameUserFileKind.replay, localFile(replay)),
          ['replay/th9_01.rpy'],
        );
        expect(
          await solo.importFile(GameUserFileKind.replay, localFile(replay)),
          ['replay/th9_ud0000.rpy'],
        );
        await solo.renameReplay('replay/th9_01.rpy', 'th9_ud0002.rpy');
        await solo.moveReplay('replay/th9_ud0002.rpy', 'replay/练习 1');
        const moved = 'replay/练习 1/th9_ud0002.rpy';
        expect(
          await solo.replayFile(moved).then(checksum),
          await checksum(replay),
        );
        expect(await solo.exportReplay(moved), isTrue);
        expect(await checksum(exported!), await checksum(replay));
        expect((await solo.replays()).map((row) => row.path), contains(moved));
        expect(await solo.exportReplays(), isTrue);
        expect(await lobby.replays(), isEmpty);
        await lobby.importFile(GameUserFileKind.replay, localFile(exported!));
        expect((await lobby.replays()).map((row) => row.path), contains(moved));
        expect(
          await lobby.replayFile(moved).then(checksum),
          await checksum(replay),
        );
        await solo.deleteReplay(moved);
        expect(
          (await solo.replays()).map((row) => row.path),
          isNot(contains(moved)),
        );
        expect(
          await lobby.replayFile(moved).then(checksum),
          await checksum(replay),
        );

        final jp = files.workspace(th10, multiplayer: false, language: 'ja')!;
        final chs = files.workspace(
          th10,
          multiplayer: false,
          language: 'lang_zh-hans',
        )!;
        final hint = await File.fromUri(temporary.uri.resolve('hint.txt'))
            .writeAsString('fixture');
        expect(await chs.importFile(GameUserFileKind.hint, localFile(hint)), [
          'hint/hint_user.txt',
        ]);
        final hintLocation = (await chs.directory()).uri.resolve(
          'hint/hint_user.txt',
        );
        expect(await File.fromUri(hintLocation).readAsString(), 'fixture');
        expect(
          await File.fromUri(
            (await jp.directory()).uri.resolve('hint/hint_user.txt'),
          ).exists(),
          isFalse,
        );
        await chs.deleteHints();
        expect(await File.fromUri(hintLocation).exists(), isFalse);

        final checkpointPackage = File.fromUri(
          temporary.uri.resolve('checkpoint-fixture.zip'),
        );
        // This fixture checks packaging and preservation, using known bytes.
        await writeGameZip(
          output: checkpointPackage.path,
          files: {
            'scoreth15.dat': score.path,
            'autosave/save0_0.dat': replay.path,
          },
        );
        final checkpoints = files.workspace(
          th15,
          multiplayer: false,
          language: 'ja',
        )!;
        expect(
          await checkpoints.importFile(
            GameUserFileKind.save,
            localFile(checkpointPackage),
          ),
          unorderedEquals(['scoreth15.dat', 'autosave/save0_0.dat']),
        );
        expect(await checkpoints.exportSave(), isTrue);
        final fresh = files.workspace(th15, multiplayer: true, language: 'ja')!;
        await fresh.importFile(GameUserFileKind.save, localFile(exported!));
        final restored = await fresh.directory();
        expect(
          await checksum(File.fromUri(restored.uri.resolve('scoreth15.dat'))),
          await checksum(score),
        );
        expect(
          await checksum(
            File.fromUri(restored.uri.resolve('autosave/save0_0.dat')),
          ),
          await checksum(replay),
        );
      } finally {
        await temporary.delete(recursive: true);
      }
    },
  );
}
