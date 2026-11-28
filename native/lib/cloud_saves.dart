import 'dart:isolate';
import 'dart:typed_data';

import 'package:collection/collection.dart';
import 'package:crypto/crypto.dart';

import 'game_file_archive.dart';
import 'save_progress.dart';

const cloudArchiveLimit = 9 * 1024 * 1024;
const _fileLimit = 4 * 1024 * 1024;
const _totalLimit = 8 * 1024 * 1024;
const _countLimit = 6;

bool cloudProductId(String value) =>
    RegExp(r'^[a-z][a-z0-9]{1,19}$').hasMatch(value);

bool _validCloudPath(String name) => name.length <= 128 && isGameFilePath(name);

Iterable<Map<String, dynamic>> _objects(Object? input, int max) => input is List
    ? input.take(max).whereType<Map<String, dynamic>>()
    : const [];

List<String> _names(Object? input) {
  if (input is! List || input.length > 12) {
    throw const FormatException('Invalid save file list');
  }
  final names = <String>[];
  final unique = <String>{};
  for (final value in input) {
    if (value is! String ||
        !_validCloudPath(value) ||
        !unique.add(value.toLowerCase())) {
      throw const FormatException('Invalid save file name');
    }
    names.add(value);
  }
  return List.unmodifiable(names);
}

typedef CloudFileInfo = ({String name, int size, String sha});
typedef CloudFilePayload = ({
  String name,
  Uint8List bytes,
  DateTime? modifiedAt,
});

Future<({bool matchesCloud, bool unchanged})> compareCloudFiles(
  List<CloudFilePayload> files,
  List<CloudFileInfo>? cloud,
  List<CloudFilePayload> previous,
) => Isolate.run(() {
  final cloudByName = {
    for (final file in cloud ?? const <CloudFileInfo>[]) file.name: file,
  };
  final previousByName = {for (final file in previous) file.name: file};
  final matches =
      cloud != null &&
      files.isNotEmpty &&
      files.length == cloud.length &&
      files.every((file) {
        final entry = cloudByName[file.name];
        return entry != null &&
            entry.size == file.bytes.length &&
            sha256.convert(file.bytes).toString() == entry.sha;
      });
  final unchanged =
      files.length == previous.length &&
      files.every((file) {
        final entry = previousByName[file.name];
        return entry != null &&
            const ListEquality<int>().equals(entry.bytes, file.bytes);
      });
  return (matchesCloud: matches, unchanged: unchanged);
});

class CloudSaveLimits {
  const CloudSaveLimits({
    this.files = _countLimit,
    this.fileBytes = _fileLimit,
    this.totalBytes = _totalLimit,
  });

  factory CloudSaveLimits.fromJson(Object? json) {
    int limit(String name, int max) {
      final value = json is Map ? json[name] : null;
      return value is int && value > 0 ? value.clamp(1, max) : max;
    }

    return CloudSaveLimits(
      files: limit('files', _countLimit),
      fileBytes: limit('fileBytes', _fileLimit),
      totalBytes: limit('totalBytes', _totalLimit),
    );
  }

  final int files, fileBytes, totalBytes;

  void validate(List<CloudFilePayload> input) {
    if (input.length > files ||
        input.fold(0, (bytes, file) => bytes + file.bytes.length) >
            totalBytes ||
        input.any(
          (file) => file.bytes.isEmpty || file.bytes.length > fileBytes,
        )) {
      throw const GameFileError(GameFileIssue.cloudFileEmptyOrLarge);
    }
  }
}

class CloudSaveProduct {
  const CloudSaveProduct({
    required this.id,
    required this.revision,
    required this.files,
    required this.requiredFiles,
  });

  final String id;
  final int revision;
  final List<String> files, requiredFiles;

  bool get canUpload => id != 'th17' && requiredFiles.isNotEmpty;

  String? resolveName(String source) {
    final name = source.toLowerCase();
    final matches = files.where(
      (path) =>
          path.toLowerCase() == name ||
          path.split('/').last.toLowerCase() == name,
    );
    return matches.length == 1 ? matches.single : null;
  }

  bool hasRequired(Iterable<String> names) => requiredFiles.any(names.contains);

  CloudSaveProduct withRevision(int next) => CloudSaveProduct(
    id: id,
    revision: next,
    files: files,
    requiredFiles: requiredFiles,
  );
}

class CloudSaveSnapshot {
  const CloudSaveSnapshot({
    required this.product,
    required this.record,
    required this.files,
  });

  factory CloudSaveSnapshot.fromJson(
    Map<String, dynamic> row,
    CloudSaveProduct product,
  ) {
    final revision = row['revision'];
    final uploaded = row['uploadedAt'];
    final timestamp = uploaded is String ? DateTime.tryParse(uploaded) : null;
    final statistics = row['statistics'];
    if (row['product'] != product.id ||
        revision is! int ||
        revision <= 0 ||
        timestamp == null ||
        statistics is! Map<String, dynamic> ||
        statistics['game'] != product.id ||
        statistics['parserVersion'] != 1) {
      throw const FormatException('Invalid cloud save revision');
    }
    final input = row['files'];
    if (input is! List || input.length > _countLimit) {
      throw const FormatException('Invalid cloud save manifest');
    }
    final names = <String>{};
    var total = 0;
    final files = input
        .map((file) {
          if (file
              case {
                'name': final String name,
                'bytes': final int size,
                'sha256': final String sha,
              }
              when product.files.contains(name) &&
                  names.add(name) &&
                  size > 0 &&
                  size <= _fileLimit &&
                  RegExp(r'^[a-f0-9]{64}$').hasMatch(sha)) {
            total += size;
            return (name: name, size: size, sha: sha);
          }
          throw const FormatException('Invalid cloud save file');
        })
        .toList(growable: false);
    if (total > _totalLimit) {
      throw const FormatException('Cloud save too large');
    }
    return CloudSaveSnapshot(
      product: product.id,
      record: (
        revision: revision,
        uploadedAt: timestamp,
        statistics: SaveStatistics.fromJson(statistics),
      ),
      files: List.unmodifiable(files),
    );
  }

  final String product;
  final CloudGameSave record;
  final List<CloudFileInfo> files;
}

class CloudSaveDirectory {
  const CloudSaveDirectory(this.products, this.saves, this.limits);

  factory CloudSaveDirectory.fromJson(Map<String, dynamic> json) {
    if (json['products'] is! List || json['saves'] is! List) {
      throw const FormatException('Invalid cloud save listing');
    }
    final products = <String, CloudSaveProduct>{};
    for (final row in _objects(json['products'], 32)) {
      if (row
          case {
            'id': final String id,
            'game': final String game,
            'multiplayer': false,
            'revision': final int revision,
          }
          when id == game && cloudProductId(id) && revision >= 0) {
        final files = _names(row['files']);
        final requiredFiles = _names(row['requiredFiles']);
        if (!requiredFiles.every(files.contains)) {
          throw const FormatException('Invalid required save files');
        }
        products[id] = CloudSaveProduct(
          id: id,
          revision: revision,
          files: files,
          requiredFiles: requiredFiles,
        );
      }
    }
    final saves = <String, CloudSaveSnapshot>{};
    for (final row in _objects(json['saves'], 64)) {
      final product = products[row['product']];
      if (product == null || row['revision'] != product.revision) continue;
      saves[product.id] = CloudSaveSnapshot.fromJson(row, product);
    }
    return CloudSaveDirectory(
      Map.unmodifiable(products),
      Map.unmodifiable(saves),
      CloudSaveLimits.fromJson(json['limits']),
    );
  }

  final Map<String, CloudSaveProduct> products;
  final Map<String, CloudSaveSnapshot> saves;
  final CloudSaveLimits limits;

  CloudSaveDirectory updated(
    CloudSaveProduct product,
    CloudSaveSnapshot? snapshot,
  ) {
    final next = {...saves};
    if (snapshot == null) {
      next.remove(product.id);
    } else {
      next[product.id] = snapshot;
    }
    return CloudSaveDirectory(
      Map.unmodifiable({...products, product.id: product}),
      Map.unmodifiable(next),
      limits,
    );
  }

  Map<String, CloudGameSave> get statistics =>
      Map.unmodifiable(saves.map((key, value) => MapEntry(key, value.record)));
}
