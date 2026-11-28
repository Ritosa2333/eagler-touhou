import 'dart:async';
import 'dart:convert';
import 'dart:io';
import 'dart:isolate';
import 'dart:math';

import 'package:crypto/crypto.dart';

Future<String> hashFile(String path) async =>
    (await sha256.bind(File(path).openRead()).first).toString();

Future<Map<String, Object>> observe(Future<void> Function() action) async {
  final clock = Stopwatch()..start();
  var last = 0;
  final gaps = <int>[];
  final timer = Timer.periodic(const Duration(milliseconds: 1), (_) {
    final now = clock.elapsedMicroseconds;
    gaps.add(now - last);
    last = now;
  });
  await Future<void>.delayed(const Duration(milliseconds: 40));
  final started = clock.elapsedMicroseconds;
  final watch = Stopwatch()..start();
  await action();
  watch.stop();
  final ended = clock.elapsedMicroseconds;
  await Future<void>.delayed(const Duration(milliseconds: 40));
  timer.cancel();
  gaps.sort();
  return {
    'elapsedMs': watch.elapsedMicroseconds / 1000,
    'actionStartUs': started,
    'actionEndUs': ended,
    'maxEventLoopGapMs': gaps.last / 1000,
    'p95EventLoopGapMs': gaps[(gaps.length * .95).ceil() - 1] / 1000,
    'ticks': gaps.length,
    'rssBytes': ProcessInfo.currentRss,
  };
}

Future<void> main(List<String> args) async {
  final output = File(args.single);
  final inputs = [
    (
      id: 'th07',
      path: 'D:/workspace/eagler/artifacts/launcher-ui-pencil-20261122/return-rail-validation/release-cbf99745/hosted-rehearsal/games/th07/th07.data',
      bytes: 23829135,
      hash: '5fe8d772191505bf208f966da8070ee0a9e0d0ee8567a8827ef3a362bcb0c287',
    ),
    (
      id: 'th14',
      path: 'D:/workspace/eagler/artifacts/th14-windows-native-20261128/辉针城 资源/th14.data',
      bytes: 90776747,
      hash: '57d84a5cbe82460931cca26f090a0fde7aef180889ce5bfe8315851fc97b0233',
    ),
  ];
  final report = <String, Object>{
    'scope': 'Dart AOT | actual local resource files independently matched against published th07 catalog and th14 provenance | identical async file stream and SHA-256, execution isolate is the variable | timer delivery gap, not UI FrameTiming | shared Windows host and warm filesystem cache',
    'cases': <Object>[],
  };
  final cases = report['cases']! as List<Object>;
  report['baseline'] = await observe(
    () => Future<void>.delayed(const Duration(milliseconds: 300)),
  );
  for (final input in inputs) {
    final before = await File(input.path).stat();
    if (before.size != input.bytes) throw StateError('Wrong input size');
    for (var round = 0; round < 3; round++) {
      for (final worker in round.isEven ? [false, true] : [true, false]) {
        late String actual;
        final measured = await observe(() async {
          actual = worker
              ? await Isolate.run(() => hashFile(input.path))
              : await hashFile(input.path);
        });
        if (actual != input.hash) throw StateError('Hash mismatch');
        final after = await File(input.path).stat();
        if (before.size != after.size || before.modified != after.modified) {
          throw StateError('Input changed during measurement');
        }
        cases.add({
          'input': input.id,
          'bytes': input.bytes,
          'sha256': actual,
          'round': round,
          'worker': worker,
          ...measured,
        });
        await output.writeAsString(
          '${const JsonEncoder.withIndent('  ').convert(report)}\n',
        );
      }
    }
  }
  report['status'] = 'passed';
  await output.writeAsString(
    '${const JsonEncoder.withIndent('  ').convert(report)}\n',
  );
  stdout.writeln(jsonEncode(report));
}
