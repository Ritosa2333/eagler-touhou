import 'dart:async';
import 'dart:convert';
import 'dart:developer';
import 'dart:io';
import 'dart:isolate';
import 'dart:math';

import 'resource_hash_probe.dart' show hashFile;

void heartbeat(SendPort port) {
  Timer.periodic(const Duration(milliseconds: 16), (_) {
    port.send(Timeline.now);
  });
}

Future<void> main(List<String> args) async {
  final output = File(args.single);
  final events = <({int sent, int lag})>[];
  final port = ReceivePort();
  final subscription = port.listen((value) {
    final now = Timeline.now;
    if (value is int) events.add((sent: value, lag: now - value));
  });
  final sender = await Isolate.spawn(heartbeat, port.sendPort);
  Future<Map<String, Object>> observe(Future<void> Function() action) async {
    events.clear();
    await Future<void>.delayed(const Duration(milliseconds: 40));
    final started = Timeline.now;
    await action();
    final ended = Timeline.now;
    await Future<void>.delayed(const Duration(milliseconds: 40));
    final lags = events
        .where((e) => e.sent >= started && e.sent <= ended)
        .map((e) => e.lag)
        .toList()
      ..sort();
    if (lags.isEmpty) throw StateError('No independent heartbeat samples');
    return {
      'elapsedMs': (ended - started) / 1000,
      'mailboxSamples': lags.length,
      'maxMailboxLagMs': lags.last / 1000,
      'p95MailboxLagMs': lags[(lags.length * .95).ceil() - 1] / 1000,
      'medianMailboxLagMs': lags[lags.length ~/ 2] / 1000,
    };
  }
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
    'scope': 'Dart AOT | real files with independent known hashes | producer isolate timestamps before SendPort.send, consumer timestamps on receipt | unchanged SHA-256 and async file reads | component mailbox latency, not whole UI FrameTiming or FPS | shared Windows host and warm filesystem cache',
    'cases': <Object>[],
  };
  final cases = report['cases']! as List<Object>;
  void save() => output.writeAsStringSync(
    '${const JsonEncoder.withIndent('  ').convert(report)}\n',
  );
  try {
    report['baseline'] = await observe(
      () => Future<void>.delayed(const Duration(seconds: 1)),
    );
    for (final input in inputs) {
      final before = await File(input.path).stat();
      if (before.size != input.bytes) throw StateError('Wrong input size');
      for (var round = 0; round < 3; round++) {
        for (final worker in round.isEven ? [false, true] : [true, false]) {
          late String value;
          final result = await observe(() async {
            value = worker
                ? await Isolate.run(() => hashFile(input.path))
                : await hashFile(input.path);
          });
          if (value != input.hash) throw StateError('Hash mismatch');
          final after = await File(input.path).stat();
          if (after.size != before.size || after.modified != before.modified) {
            throw StateError('Input changed during measurement');
          }
          cases.add({
            'input': input.id,
            'bytes': input.bytes,
            'sha256': value,
            'round': round,
            'worker': worker,
            ...result,
          });
          save();
        }
      }
    }
    report['status'] = 'passed';
    save();
  } finally {
    sender.kill(priority: Isolate.immediate);
    await subscription.cancel();
    port.close();
  }
  stdout.writeln(jsonEncode(report));
}
