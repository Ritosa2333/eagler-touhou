import 'dart:convert';
import 'dart:io';
import 'dart:isolate';
import 'dart:typed_data';

import 'file:///C:/Users/Ritosa/AppData/Local/Packages/OpenAI.Codex_2p2nqsd0c76g0/LocalCache/Local/eagler-account-api-routing/native/lib/th09_ratings.dart';

Map<String, dynamic> decode(Uint8List bytes) =>
    jsonDecode(utf8.decode(bytes)) as Map<String, dynamic>;

Future<Map<String, dynamic>> worker(Uint8List bytes) =>
    Isolate.run(() => decode(bytes));

int model(String name, Map<String, dynamic> value) => switch (name) {
  'profile' => (value['username'] as String).length,
  'statistics' => (value['games'] as List).length,
  'board' => Th09Board.fromJson(value).players.length,
  'ratings' => Th09LobbyRatings.fromJson(value).players.length,
  'manifest' => (value['games'] as Map).length,
  _ => throw StateError(name),
};

Map<String, Object> distribution(List<int> values) {
  final sorted = [...values]..sort();
  return {
    'samples': values.length,
    'medianMs': sorted[sorted.length ~/ 2] / 1000,
    'p95Ms': sorted[(sorted.length * .95).ceil() - 1] / 1000,
    'maxMs': sorted.last / 1000,
  };
}

Future<void> main(List<String> args) async {
  final directory = Directory(args[0]);
  final expected = jsonDecode(await File('${directory.path}/expected.json').readAsString()) as Map;
  final rows = <Object>[];
  for (final name in ['profile', 'statistics', 'board', 'ratings', 'manifest']) {
    final bytes = await File('${directory.path}/$name.json').readAsBytes();
    final jsonTimes = <int>[], modelTimes = <int>[], totalTimes = <int>[];
    final asyncWall = <int>[];
    final first = Stopwatch()..start();
    final actual = model(name, decode(bytes));
    first.stop();
    if (actual != expected[name]) throw StateError('Reference mismatch: $name $actual');
    for (var i = 0; i < 20; i++) {
      if (model(name, decode(bytes)) != actual) throw StateError(name);
    }
    for (var i = 0; i < 100; i++) {
      final clock = Stopwatch()..start();
      final value = decode(bytes);
      final decoded = clock.elapsedMicroseconds;
      if (model(name, value) != actual) throw StateError(name);
      clock.stop();
      jsonTimes.add(decoded);
      modelTimes.add(clock.elapsedMicroseconds - decoded);
      totalTimes.add(clock.elapsedMicroseconds);
    }
    for (var i = 0; i < 20; i++) {
      final clock = Stopwatch()..start();
      final value = await worker(bytes);
      if (model(name, value) != actual) throw StateError(name);
      clock.stop();
      asyncWall.add(clock.elapsedMicroseconds);
    }
    rows.add({
      'input': name,
      'bytes': bytes.length,
      'modelScope': ['board', 'ratings'].contains(name)
          ? 'Existing production constructor'
          : 'Reference count only; application model construction excluded',
      'verifiedModelCount': actual,
      'firstUseDecodeAndModelMs': first.elapsedMicroseconds / 1000,
      'currentDecode': distribution(jsonTimes),
      'currentModel': distribution(modelTimes),
      'currentCombined': distribution(totalTimes),
      'candidateDecodeWorkerAndModelWall': distribution(asyncWall),
    });
  }
  final report = {
    'scope': 'Pinned Dart SDK native AOT component measurement. Five actual unauthenticated production responses. Exact current UTF-8/JSON expression; existing production model constructors measured for board and ratings only. Other cases perform independent reference-count checks, excluding application model construction and site-manifest structural validation. File/network I/O excluded. Results checked against independently parsed JSON counts. Worker experiment offloads only decode, returns real Map, then runs the same constructor or reference check. Worker wall time is not UI-thread blocking or presented FPS. No product-source changes.',
    'cases': rows,
  };
  await File(args[1]).writeAsString('${const JsonEncoder.withIndent('  ').convert(report)}\n');
  stdout.writeln(jsonEncode(report));
}
