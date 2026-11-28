import 'dart:convert';
import 'dart:io';

import 'package:archive/archive.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:touhouonline/catalog.dart';
import 'package:touhouonline/game_user_files.dart';

final matchSidecarFixture = utf8.encode(
  jsonEncode({
    'schema': 'eagler-touhou/th09-match-replay/1',
    'names': ['灵梦', 'Marisa'],
    'startedAt': 1767225600000,
    'frames': 3600,
    'winner': 1,
    'roundStarts': [0, 1900],
  }),
);

List<int> matchReplayZip(List<int> replay, {int extraFolders = 0}) {
  final archive = Archive()
    ..addFile(ArchiveFile.bytes('replay/对局/th9_01.rpy', replay))
    ..addFile(ArchiveFile.bytes('replay/对局/th9_01.json', matchSidecarFixture))
    ..addFile(ArchiveFile.bytes('replay/对局/th9_02.rpy', replay))
    ..addFile(
      ArchiveFile.bytes('replay/对局/th9_02.json', utf8.encode('invalid')),
    );
  for (var index = 0; index < extraFolders; index++) {
    final folder = 'folder ${index.toString().padLeft(2, '0')}';
    archive.addFile(ArchiveFile.bytes('replay/$folder/th9_01.rpy', replay));
  }
  return ZipEncoder().encodeBytes(archive);
}

void main() {
  TestWidgetsFlutterBinding.ensureInitialized();

  test(
    'match sidecar follows a multiplayer replay through the file workspace',
    () async {
      final game = (await Game.load()).singleWhere((game) => game.id == 'th09');
      final root = await Directory.systemTemp.createTemp('th09-match-zip-');
      addTearDown(() => root.delete(recursive: true));
      final sample = Platform.environment['TOUHOU_PUBLIC_MATCH_REPLAY'];
      final replay = sample == null
          ? [1, 2, 3]
          : await File(sample).readAsBytes();
      final input = matchReplayZip(replay);
      final downloads = <Archive>[];
      final files = GameUserFiles(
        appDirectory: () async => root,
        exporter: (_, file) async {
          downloads.add(ZipDecoder().decodeBytes(await file.readAsBytes()));
          return true;
        },
      );
      final multiplayer = files.workspace(
        game,
        multiplayer: true,
        language: 'jp',
      )!;
      final single = files.workspace(game, multiplayer: false, language: 'jp')!;
      final pick = (
        name: 'matches.zip',
        length: () async => input.length,
        read: () => Stream<List<int>>.value(input),
      );
      final imported = await multiplayer.importFile(
        GameUserFileKind.replay,
        pick,
      );
      expect(imported, ['replay/对局/th9_01.rpy', 'replay/对局/th9_02.rpy']);
      final rows = await multiplayer.replays();
      expect(rows.first.match?.names, ['灵梦', 'Marisa']);
      expect(rows.first.match?.winner, 1);
      expect(rows.first.match?.frames, 3600);
      expect(rows.last.match, isNull);
      expect(await single.replays(), isEmpty);
      await single.importFile(GameUserFileKind.replay, pick);
      expect(
        (await single.replays()).every((entry) => entry.match == null),
        isTrue,
      );

      await multiplayer.renameReplay(imported.first, 'th9_ud0004.rpy');
      await multiplayer.moveReplay('replay/对局/th9_ud0004.rpy', 'replay/新对局');
      final moved = (await multiplayer.replays()).singleWhere(
        (entry) => entry.path == 'replay/新对局/th9_ud0004.rpy',
      );
      expect(moved.match?.names, rows.first.match?.names);
      expect(await multiplayer.exportReplays(folder: 'replay/新对局'), isTrue);
      final download = downloads.single;
      expect(download.findFile(moved.path)!.content, replay);
      expect(
        download.findFile('replay/新对局/th9_ud0004.json')!.content,
        matchSidecarFixture,
      );
      await multiplayer.deleteReplay(moved.path);
      final folder = await multiplayer.directory();
      expect(
        File.fromUri(folder.uri.resolve('replay/新对局/th9_ud0004.json'))
            .existsSync(),
        isFalse,
      );
      expect((await multiplayer.replays()).single.match, isNull);
    },
  );
}
