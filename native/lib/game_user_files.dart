import 'dart:io';
import 'dart:typed_data';

import 'package:path_provider/path_provider.dart';

import 'catalog.dart';
import 'cloud_saves.dart';
import 'game_file_archive.dart';
import 'game_file_picker.dart';
import 'th09_replay_match.dart';

enum GameUserFileKind { save, replay, hint }

typedef ReplayEntry = ({String path, int size, Th09ReplayMatch? match});

String gameFileName(String path) => path.split('/').last;
bool isTh09MatchArchive(String path) => path.toLowerCase().endsWith('.th09r');

class GameUserFiles {
  GameUserFiles({
    Future<Directory> Function()? appDirectory,
    Future<GameFileInput?> Function(List<String>)? picker,
    Future<List<GameFileInput>> Function(List<String>)? multiPicker,
    Future<bool> Function(String, File)? exporter,
  }) : _appDirectory = appDirectory ?? getApplicationSupportDirectory,
       pick = picker ?? pickGameFile,
       pickMany = multiPicker ?? pickGameFiles,
       _exporter = exporter ?? exportGameFile;

  final Future<Directory> Function() _appDirectory;
  final Future<GameFileInput?> Function(List<String>) pick;
  final Future<List<GameFileInput>> Function(List<String>) pickMany;
  final Future<bool> Function(String, File) _exporter;
  Future<void>? _tail;

  GameFileWorkspace? workspace(
    Game game, {
    required bool multiplayer,
    required String language,
  }) {
    final catalog = game.files;
    if (catalog == null) return null;
    final replay = multiplayer ? catalog.multiplayer : catalog.normal;
    if (replay == null) return null;
    return GameFileWorkspace._(
      this,
      game.id,
      catalog,
      replay,
      language,
      multiplayer,
    );
  }

  Future<T> _run<T>(Future<T> Function() action) {
    final next = (_tail ?? Future<void>.value()).then((_) => action());
    final finished = next.then<void>(
      (_) {},
      onError: (Object _, StackTrace _) {},
    );
    _tail = finished;
    finished.then((_) {
      if (identical(_tail, finished)) _tail = null;
    });
    return next;
  }
}

class GameFileWorkspace {
  const GameFileWorkspace._(
    this._owner,
    this.game,
    this.catalog,
    this.replay,
    this.language,
    this.multiplayer,
  );

  final GameUserFiles _owner;
  final String game;
  final GameFilesCatalog catalog;
  final ReplayCatalog replay;
  final String language;
  final bool multiplayer;

  bool get supportsFolders => game == 'th09';
  bool get supportsMatchArchives => game == 'th09' && multiplayer;

  List<String> extensions(GameUserFileKind kind) => switch (kind) {
    GameUserFileKind.save => [
      catalog.score.split('.').last.toLowerCase(),
      if (catalog.saveArchive) 'zip',
    ],
    GameUserFileKind.replay => [...replay.extensions, 'zip'],
    GameUserFileKind.hint => const ['txt'],
  };

  Future<GameFileInput?> pick(GameUserFileKind kind) =>
      _owner.pick(extensions(kind));

  Future<GameFileInput?> pickReplayForViewing() => _owner.pick([
    ...extensions(GameUserFileKind.replay),
    if (supportsMatchArchives) 'th09r',
  ]);

  Future<Directory> directory() async {
    final app = await _owner._appDirectory();
    final suffix = catalog.languageScoped
        ? (language == 'lang_zh-hans' ? '/chs' : '/jp')
        : '';
    final location = await _checkedLocation(
      app,
      'game-user-data/${replay.namespace}$suffix',
      directory: true,
    );
    return Directory.fromUri(location);
  }

  Future<T> _stage<T>(Future<T> Function(Directory) action) async {
    final app = await _owner._appDirectory();
    final uri = await _checkedLocation(
      app,
      'game-file-staging',
      directory: true,
    );
    final parent = Directory.fromUri(uri);
    await parent.create(recursive: true);
    final staging = await parent.createTemp('transfer-');
    var keepBackup = false;
    try {
      return await action(staging);
    } on _RollbackFailed {
      // A prior version may still be available in this transaction directory.
      keepBackup = true;
      throw const GameFileError(GameFileIssue.rollbackFailed);
    } finally {
      if (!keepBackup) await staging.delete(recursive: true);
    }
  }

  Future<T> withArchivedReplay<T>(
    String name,
    Uint8List bytes,
    Future<T> Function(File file, Future<bool> Function() export) use,
  ) => _stage((staging) async {
    final extension = name.split('.').last.toLowerCase();
    if (!isGameFilePath(name) ||
        name.contains('/') ||
        !(supportsMatchArchives && isTh09MatchArchive(name) ||
            replay.extensions.contains(extension))) {
      throw const GameFileError(GameFileIssue.cannotReadReplayName);
    }
    if (bytes.isEmpty || bytes.length > th09ReplayLimit) {
      throw const GameFileError(GameFileIssue.importEmptyOrLarge);
    }
    final replayFile = File.fromUri(
      staging.uri.resolveUri(Uri(pathSegments: [name])),
    );
    await replayFile.writeAsBytes(bytes, flush: true);
    return use(replayFile, () => _owner._exporter(name, replayFile));
  });

  Future<T> withImportedMatchReplay<T>(
    GameFileInput source,
    Future<T> Function(File file) use,
  ) => _stage((staging) async {
    final name = source.name;
    if (!supportsMatchArchives ||
        !isTh09MatchArchive(name) ||
        !isGameFilePath(name) ||
        name.contains('/')) {
      throw const GameFileError(GameFileIssue.cannotReadReplayName);
    }
    final file = File.fromUri(
      staging.uri.resolveUri(Uri(pathSegments: [name])),
    );
    await _copyInput(source, file, th09ReplayLimit);
    return use(file);
  });

  Future<List<String>> importFile(
    GameUserFileKind kind,
    GameFileInput source,
  ) => _owner._run(
    () => _stage((staging) async {
      if (kind == GameUserFileKind.hint && catalog.hints.isEmpty) {
        {
          throw const GameFileError(GameFileIssue.noRouteHint);
        }
      }
      final dot = source.name.lastIndexOf('.');
      final ext = dot > 0 ? source.name.substring(dot + 1).toLowerCase() : '';
      if (!extensions(kind).contains(ext)) {
        {
          throw const GameFileError(GameFileIssue.wrongGameFormat);
        }
      }
      final archive = ext == 'zip';
      final sourceFile = File.fromUri(staging.uri.resolve('input'));
      await _copyInput(
        source,
        sourceFile,
        archive
            ? replayArchiveLimit
            : kind == GameUserFileKind.hint
            ? 16 * 1024 * 1024
            : storedFileLimit,
      );

      final root = await directory();
      final matchMetadata =
          archive && supportsMatchArchives && kind == GameUserFileKind.replay;
      final Map<String, String> files;
      if (archive) {
        files = await extractGameZip(
          source: sourceFile.path,
          staging: staging.path,
          replayExtensions: replay.extensions,
          score: kind == GameUserFileKind.save ? catalog.score : null,
          replayMatchMetadata: matchMetadata,
        );
      } else {
        files = {source.name: sourceFile.path};
      }

      final occupied = kind == GameUserFileKind.replay
          ? (await _replays(
              root,
              includeMatches: false,
            )).map((entry) => _slotKey(entry.path)).toSet()
          : <String>{};
      final pending = <String, File>{};
      final installed = <String>[];
      for (final entry in files.entries) {
        if (matchMetadata && entry.key.toLowerCase().endsWith('.json')) {
          continue;
        }
        final target = switch (kind) {
          GameUserFileKind.save =>
            archive
                ? entry.key.toLowerCase() == catalog.score.toLowerCase()
                      ? catalog.score
                      : entry.key.toLowerCase()
                : catalog.score,
          GameUserFileKind.hint => catalog.hints.first,
          GameUserFileKind.replay => _allocateReplay(entry.key, occupied),
        };
        pending[target] = File(entry.value);
        installed.add(target);
        final metadataPath = matchMetadata
            ? files[th09ReplayMetadataPath(entry.key)]
            : null;
        if (metadataPath != null) {
          final metadata = File(metadataPath);
          if (await readTh09ReplayMatch(metadata) != null) {
            final copy = File.fromUri(
              staging.uri.resolve('match-${installed.length}'),
            );
            pending[th09ReplayMetadataPath(target)] = await metadata.copy(
              copy.path,
            );
          }
        }
      }
      await _publish(
        root,
        staging,
        pending,
        replace: kind != GameUserFileKind.replay,
      );
      return installed;
    }),
  );

  Future<List<ReplayEntry>> replays() =>
      _owner._run(() async => _replays(await directory()));

  Future<List<ReplayEntry>> _replays(
    Directory root, {
    bool includeMatches = true,
  }) async {
    final uri = await _checkedLocation(root, 'replay', directory: true);
    final folder = Directory.fromUri(uri);
    if (!await folder.exists()) return [];
    final rows = <ReplayEntry>[];
    Future<void> collect(
      Directory parent,
      String relative, {
      required bool folders,
    }) async {
      await for (final node in parent.list(followLinks: false)) {
        final name = node is Directory
            ? node.uri.pathSegments.reversed.skip(1).first
            : node.uri.pathSegments.last;
        final path = '$relative/$name';
        if (!isGameFilePath(path)) continue;
        if (node is Directory && folders && name.length <= 128) {
          await collect(node, path, folders: false);
        } else if (node is File && _validReplayName(name)) {
          final metadata = includeMatches
              ? await _matchSidecar(root, path)
              : null;
          rows.add((
            path: path,
            size: await node.length(),
            match: metadata == null
                ? null
                : await readTh09ReplayMatch(metadata),
          ));
        }
      }
    }

    await collect(folder, 'replay', folders: supportsFolders);
    return rows..sort((a, b) => a.path.compareTo(b.path));
  }

  Future<File?> _matchSidecar(Directory root, String path) async {
    if (!supportsMatchArchives) return null;
    final file = File.fromUri(
      await _checkedLocation(root, th09ReplayMetadataPath(path)),
    );
    return await file.exists() ? file : null;
  }

  String? _localCloudName(String cloudName) {
    if (game == 'th10' &&
        const {'scoreth10.dat', 'scoreth10c.dat'}.contains(cloudName)) {
      final score = language == 'lang_zh-hans'
          ? 'scoreth10c.dat'
          : 'scoreth10.dat';
      return cloudName == score ? catalog.score : null;
    }
    return cloudName;
  }

  bool canRestoreCloudSave(CloudSaveProduct product, Iterable<String> files) =>
      product.id == game &&
      product.hasRequired(files.where((name) => _localCloudName(name) != null));

  Future<List<CloudFilePayload>> cloudLocalFiles(
    CloudSaveProduct product,
    CloudSaveLimits limits,
  ) => _owner._run(() async {
    final root = await directory();
    final files = <CloudFilePayload>[];
    var remaining = limits.totalBytes;
    for (final name in product.files) {
      final path = _localCloudName(name);
      if (path == null) continue;
      final file = File.fromUri(await _checkedLocation(root, path));
      if (!await file.exists()) continue;
      if (files.length >= limits.files) {
        throw const GameFileError(GameFileIssue.tooManyLocalSaves);
      }
      final bytes = await _readFileInput((
        name: name,
        length: file.length,
        read: file.openRead,
      ), remaining.clamp(0, limits.fileBytes));
      remaining -= bytes.length;
      files.add((
        name: name,
        bytes: bytes,
        modifiedAt: await file.lastModified(),
      ));
    }
    return files;
  });

  Future<List<CloudFilePayload>> pickCloudFiles(
    CloudSaveProduct product,
    CloudSaveLimits limits,
  ) async {
    final extensions = {
      for (final file in product.files) file.split('.').last.toLowerCase(),
    }.toList();
    final selected = await _owner.pickMany(extensions);
    if (selected.length > limits.files) {
      throw const GameFileError(GameFileIssue.tooManySelectedSaves);
    }
    final names = <String>{};
    final files = <CloudFilePayload>[];
    var remaining = limits.totalBytes;
    for (final item in selected) {
      final name = product.resolveName(item.name);
      if (name == null || !names.add(name)) {
        {
          throw const GameFileError(GameFileIssue.saveNamesInvalid);
        }
      }
      final bytes = await _readFileInput(
        item,
        remaining.clamp(0, limits.fileBytes),
      );
      remaining -= bytes.length;
      files.add((name: name, bytes: bytes, modifiedAt: null));
    }
    return files;
  }

  Future<bool> exportCloudArchive(CloudSaveSnapshot snapshot, Uint8List zip) =>
      _stage((stage) async {
        await _verifiedCloudFiles(snapshot, zip);
        final file = File.fromUri(stage.uri.resolve('cloud.zip'));
        await file.writeAsBytes(zip, flush: true);
        return _owner._exporter(
          '${snapshot.product}-save-r${snapshot.record.revision}.zip',
          file,
        );
      });

  Future<void> restoreCloudArchive(
    CloudSaveProduct product,
    CloudSaveSnapshot snapshot,
    Uint8List zip, {
    required bool Function() isActive,
  }) => _owner._run(
    () => _stage((stage) async {
      final unpacked = await _verifiedCloudFiles(snapshot, zip);
      if (!canRestoreCloudSave(product, unpacked.keys)) {
        throw const GameFileError(GameFileIssue.cloudMainSaveMissing);
      }
      final targets = <String, File>{};
      for (final entry in unpacked.entries) {
        final path = _localCloudName(entry.key);
        if (path == null) continue;
        final file = File.fromUri(
          stage.uri.resolve('restore-${targets.length}'),
        );
        await file.writeAsBytes(entry.value, flush: true);
        targets[path] = file;
      }
      await _publish(
        await directory(),
        stage,
        targets,
        replace: true,
        isActive: isActive,
        remove: product.files
            .map(_localCloudName)
            .whereType<String>()
            .where((name) => !targets.containsKey(name)),
      );
    }),
  );

  Future<Map<String, Uint8List>> _verifiedCloudFiles(
    CloudSaveSnapshot snapshot,
    Uint8List zip,
  ) {
    if (snapshot.product != game || zip.length > cloudArchiveLimit) {
      {
        throw const GameFileError(GameFileIssue.cloudWrongGameOrLimit);
      }
    }
    return readVerifiedSaveZip(zip, {
      for (final file in snapshot.files)
        file.name: (size: file.size, sha: file.sha),
    });
  }

  Future<bool?> exportSave() => _owner._run(() async {
    final root = await directory();
    final score = File.fromUri(await _checkedLocation(root, catalog.score));
    if (!catalog.saveArchive) {
      return await score.exists()
          ? _owner._exporter(catalog.score, score)
          : null;
    }
    final entries = <String, String>{
      if (await score.exists()) catalog.score: score.path,
    };
    final autosave = Directory.fromUri(
      await _checkedLocation(root, 'autosave', directory: true),
    );
    if (await autosave.exists()) {
      await for (final file in autosave.list(followLinks: false)) {
        if (file is! File) continue;
        final key = 'autosave/${file.uri.pathSegments.last}';
        if (isCheckpointPath(key)) entries[key] = file.path;
      }
    }
    if (entries.isEmpty) return null;
    return _exportZip(
      entries,
      '${catalog.score.split('.').first}.zip',
      saveArchiveLimit,
    );
  });

  Future<bool?> exportReplays({String? folder}) => _owner._run(() async {
    final root = await directory();
    final rows = (await _replays(root))
        .where((entry) => folder == null || entry.path.startsWith('$folder/'));
    if (rows.isEmpty) return null;
    final date = DateTime.now().toUtc().toIso8601String().substring(0, 10);
    final entries = <String, String>{};
    for (final entry in rows) {
      entries[entry.path] = File.fromUri(
        await _checkedLocation(root, entry.path),
      ).path;
      if (entry.match == null) continue;
      if (await _matchSidecar(root, entry.path) case final sidecar?) {
        entries[th09ReplayMetadataPath(entry.path)] = sidecar.path;
      }
    }
    return _exportZip(entries, '$game-replay-$date.zip', replayArchiveLimit);
  });

  Future<bool> _exportZip(
    Map<String, String> entries,
    String name,
    int limit,
  ) => _stage((staging) async {
    var total = 0;
    for (final source in entries.entries) {
      final length = await File(source.value).length();
      total += length;
      if (length >
              (isCheckpointPath(source.key)
                  ? checkpointLimit
                  : storedFileLimit) ||
          total > limit) {
        throw const GameFileError(GameFileIssue.exportTooLarge);
      }
    }
    final path = File.fromUri(staging.uri.resolve('export.zip'));
    await writeGameZip(output: path.path, files: entries);
    return _owner._exporter(name, path);
  });

  Future<bool> exportReplay(String path) => _owner._run(() async {
    final file = await _existingReplay(path);
    return _owner._exporter(gameFileName(path), file);
  });

  Future<void> deleteReplay(String path) => _owner._run(() async {
    final file = await _existingReplay(path);
    final root = await directory();
    if (await _matchSidecar(root, path) == null) {
      await file.delete();
    } else {
      await _stage(
        (staging) => _publish(
          root,
          staging,
          {},
          replace: true,
          remove: [path, th09ReplayMetadataPath(path)],
        ),
      );
    }
  });

  Future<void> deleteHints() => _owner._run(() async {
    final root = await directory();
    for (final path in catalog.hints) {
      final file = File.fromUri(await _checkedLocation(root, path));
      if (await file.exists()) await file.delete();
    }
  });

  Future<void> renameReplay(String path, String name) async {
    _validateReplayPath(path);
    if (name.contains('/')) {
      throw const GameFileError(GameFileIssue.replayNameRequired);
    }
    await _moveReplay(
      path,
      '${path.substring(0, path.lastIndexOf('/'))}/${name.trim()}',
    );
  }

  Future<void> moveReplay(String path, String folder) =>
      _moveReplay(path, '$folder/${gameFileName(path)}');

  Future<void> _moveReplay(String from, String to) => _owner._run(() async {
    if (from == to) return;
    _validateReplayPath(to);
    if (gameFileName(from).split('.').last.toLowerCase() !=
        gameFileName(to).split('.').last.toLowerCase()) {
      throw const GameFileError(GameFileIssue.keepReplayExtension);
    }
    final root = await directory();
    final source = await _existingReplay(from);
    final occupied = (await _replays(
      root,
      includeMatches: false,
    )).map((entry) => _slotKey(entry.path));
    if (occupied.contains(_slotKey(to))) {
      {
        throw const GameFileError(GameFileIssue.nameTaken);
      }
    }
    final file = File.fromUri(await _checkedLocation(root, to));
    if (await _matchSidecar(root, to) != null) {
      throw const GameFileError(GameFileIssue.nameTaken);
    }
    final metadata = await _matchSidecar(root, from);
    if (metadata != null) {
      final target = th09ReplayMetadataPath(to);
      await _stage((staging) async {
        final replayCopy = await source.copy(
          File.fromUri(staging.uri.resolve('replay')).path,
        );
        final metadataCopy = await metadata.copy(
          File.fromUri(staging.uri.resolve('match')).path,
        );
        await _publish(
          root,
          staging,
          {to: replayCopy, target: metadataCopy},
          replace: true,
          remove: [from, th09ReplayMetadataPath(from)],
        );
      });
      return;
    }
    await file.parent.create(recursive: true);
    await source.rename(file.path);
  });

  Future<File> replayFile(String path) =>
      _owner._run(() => _existingReplay(path));

  Future<File> _existingReplay(String path) async {
    _validateReplayPath(path);
    final file = File.fromUri(await _checkedLocation(await directory(), path));
    if (!await file.exists()) {
      throw const GameFileError(GameFileIssue.replayMissing);
    }
    return file;
  }

  void _validateReplayPath(String path) {
    final parts = path.split('/');
    if (!isGameFilePath(path) ||
        parts.first != 'replay' ||
        (parts.length != 2 &&
            !(supportsFolders &&
                parts.length == 3 &&
                parts[1].length <= 128)) ||
        !_validReplayName(parts.last)) {
      throw const GameFileError(GameFileIssue.wrongReplayName);
    }
  }

  bool _validReplayName(String name) {
    if (!isGameFilePath(name) || name.contains('/')) return false;
    final ext = name.split('.').last.toLowerCase();
    if (!replay.extensions.contains(ext)) return false;
    if (replay.filename) {
      return name.length <= 200 &&
          RegExp(r'^[a-zA-Z0-9_(). -]+$').hasMatch(name) &&
          !name.contains('..');
    }
    final slot = replay.slotPrefix;
    if (slot != null) {
      return RegExp(
        '^${RegExp.escape(slot)}'
        r'[0-9]{2}\.rpy$',
        caseSensitive: false,
      ).hasMatch(name);
    }
    if (replay.prefix == 'th12_mp') {
      return RegExp(
        r'^th12_mp_[a-z0-9_-]+\.rpy$',
        caseSensitive: false,
      ).hasMatch(name);
    }
    return [replay.prefix, ?replay.legacyPrefix].any(
      (prefix) => RegExp(
        '^${RegExp.escape(prefix)}_'
        r'(?:[0-9]{2}|ud[0-9a-f]{4})\.'
        '$ext'
        r'$',
        caseSensitive: false,
      ).hasMatch(name),
    );
  }

  String _slotKey(String path) {
    final lower = path.toLowerCase();
    return replay.sharedExtensionSlot && lower.endsWith('.rpyx')
        ? lower.substring(0, lower.length - 1)
        : lower;
  }

  String _allocateReplay(String from, Set<String> occupied) {
    final parts = from.split('/');
    final original = parts.last.replaceAll(RegExp(r'[^\w.()-]'), '_');
    final extension = original.split('.').last.toLowerCase();
    final folder =
        supportsFolders &&
            parts.length == 3 &&
            parts.first == 'replay' &&
            isGameFilePath(parts[1]) &&
            parts[1].length <= 128
        ? 'replay/${parts[1]}'
        : 'replay';
    final previous = '$folder/$original';
    if (_validReplayName(original) && occupied.add(_slotKey(previous))) {
      return previous;
    }
    final slot = replay.slotPrefix;
    final limit = slot == null ? 65536 : replay.slots;
    for (var i = 0; i < limit; i++) {
      final name = slot == null
          ? '${replay.prefix}_ud${i.toRadixString(16).padLeft(4, '0')}.$extension'
          : '$slot${i.toString().padLeft(2, '0')}.rpy';
      final path = '$folder/$name';
      if (occupied.add(_slotKey(path))) return path;
    }
    throw const GameFileError(GameFileIssue.replaySlotsFull);
  }
}

Future<Uri> _checkedLocation(
  Directory root,
  String path, {
  bool directory = false,
}) async {
  if (!isGameFilePath(path)) {
    throw const GameFileError(GameFileIssue.invalidPath);
  }
  final parts = path.split('/');
  var uri = root.uri;
  for (final (index, part) in parts.indexed) {
    final isDirectory = directory || index < parts.length - 1;
    final entity = uri.resolveUri(Uri(pathSegments: [part]));
    final type = await FileSystemEntity.type(
      File.fromUri(entity).path,
      followLinks: false,
    );
    if (type != FileSystemEntityType.notFound &&
        type !=
            (isDirectory
                ? FileSystemEntityType.directory
                : FileSystemEntityType.file)) {
      throw const GameFileError(GameFileIssue.cannotReadLocal);
    }
    uri = isDirectory ? uri.resolveUri(Uri(pathSegments: [part, ''])) : entity;
  }
  return uri;
}

Future<Uint8List> _readFileInput(GameFileInput source, int maximum) async {
  final length = await source.length();
  if (length != null && (length <= 0 || length > maximum)) {
    {
      throw const GameFileError(GameFileIssue.cloudSaveEmptyOrLarge);
    }
  }
  final result = BytesBuilder(copy: false);
  await for (final chunk in source.read()) {
    if (result.length + chunk.length > maximum) {
      {
        throw const GameFileError(GameFileIssue.cloudSaveTooLarge);
      }
    }
    result.add(chunk);
  }
  if (result.isEmpty || length != null && length != result.length) {
    {
      throw const GameFileError(GameFileIssue.incompleteSave);
    }
  }
  return result.takeBytes();
}

Future<void> _copyInput(GameFileInput source, File target, int limit) async {
  final length = await source.length();
  if (length != null && (length > limit || length <= 0)) {
    {
      throw const GameFileError(GameFileIssue.importEmptyOrLarge);
    }
  }
  final output = await target.open(mode: FileMode.writeOnly);
  var count = 0;
  try {
    await for (final chunk in source.read()) {
      count += chunk.length;
      if (count > limit) {
        throw const GameFileError(GameFileIssue.importTooLarge);
      }
      await output.writeFrom(chunk);
    }
    if (count == 0 || length != null && length != count) {
      {
        throw const GameFileError(GameFileIssue.incompleteFile);
      }
    }
    await output.flush();
  } finally {
    await output.close();
  }
}

class _RollbackFailed implements Exception {}

Future<void> _publish(
  Directory root,
  Directory staging,
  Map<String, File> files, {
  required bool replace,
  Iterable<String> remove = const [],
  bool Function()? isActive,
}) async {
  void ensureActive() {
    if (isActive?.call() == false) {
      throw const GameFileError(GameFileIssue.restoreCancelled);
    }
  }

  final previous = <({File target, File? backup})>[];
  try {
    for (final path in {...files.keys, ...remove}) {
      ensureActive();
      final target = File.fromUri(await _checkedLocation(root, path));
      await target.parent.create(recursive: true);
      final exists = await target.exists();
      if (exists && !replace) {
        throw const GameFileError(GameFileIssue.importNameConflict);
      }
      final backup = exists
          ? await target.copy(
              File.fromUri(staging.uri.resolve('backup-${previous.length}'))
                  .path,
            )
          : null;
      previous.add((target: target, backup: backup));
      if (files[path] case final File source) {
        await source.rename(target.path);
      } else if (exists) {
        await target.delete();
      }
    }
    ensureActive();
  } on Exception {
    try {
      for (final row in previous.reversed) {
        if (row.backup case final backup?) {
          await backup.rename(row.target.path);
        } else if (await row.target.exists()) {
          await row.target.delete();
        }
      }
    } on Exception {
      throw _RollbackFailed();
    }
    rethrow;
  }
}
