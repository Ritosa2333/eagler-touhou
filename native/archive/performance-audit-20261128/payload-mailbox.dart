import 'dart:async';
import 'dart:convert';
import 'dart:developer';
import 'dart:io';
import 'dart:isolate';
import 'dart:typed_data';

import 'current-decoder.dart';
import 'file:///C:/Users/Ritosa/AppData/Local/Packages/OpenAI.Codex_2p2nqsd0c76g0/LocalCache/Local/eagler-account-api-routing/native/lib/th09_ratings.dart';

void heartbeat(SendPort receiver) {
  Timer.periodic(const Duration(milliseconds: 16), (_) {
    receiver.send(Timeline.now);
  });
}

Map<String, Object?> distribution(List<int> values) {
  final sorted = [...values]..sort();
  return {
    'samples': values.length,
    'medianMs': sorted.isEmpty ? null : sorted[sorted.length ~/ 2] / 1000,
    'p95Ms': sorted.isEmpty ? null : sorted[(sorted.length * .95).ceil() - 1] / 1000,
    'maxMs': sorted.isEmpty ? null : sorted.last / 1000,
  };
}

Future<void> main(List<String> args) async {
  final directory = args[0];
  final messages = <({int sent, int lag})>[];
  final port = ReceivePort();
  final subscription = port.listen((Object? raw) {
    if (raw is int) messages.add((sent: raw, lag: Timeline.now - raw));
  });
  final sender = await Isolate.spawn(heartbeat, port.sendPort);
  final rows = <Object>[];
  try {
    await Future<void>.delayed(const Duration(seconds: 1));
    final baseline = distribution(messages.map((m) => m.lag).toList());
    for (final name in ['board', 'ratings']) {
      final bytes = await File('$directory/$name.json').readAsBytes();
      final order = int.parse(args[2]).isEven ? [true, false] : [false, true];
      for (final background in order) {
        messages.clear();
        final intervals = <({int start, int end})>[];
        final elapsed = <int>[];
        for (var i = 0; i < 100; i++) {
          final start = Timeline.now;
          final value = background
              ? await decodeAccountJson(bytes)
              : jsonDecode(utf8.decode(bytes));
          final json = value as Map<String, dynamic>;
          final count = name == 'board'
              ? Th09Board.fromJson(json).players.length
              : Th09LobbyRatings.fromJson(json).players.length;
          if (count != 265) throw StateError('Model reference mismatch');
          final end = Timeline.now;
          elapsed.add(end - start);
          intervals.add((start: start, end: end));
          await Future<void>.delayed(Duration(milliseconds: 17 + i % 7));
        }
        await Future<void>.delayed(const Duration(milliseconds: 50));
        final active = messages.where((m) => intervals.any(
            (range) => m.sent >= range.start && m.sent <= range.end));
        final idle = messages.where((m) => !intervals.any(
            (range) => m.sent >= range.start && m.sent <= range.end));
        rows.add({
          'input': name,
          'bytes': bytes.length,
          'strategy': background ? 'Current size-gated decoder' : 'Previous inline decoder',
          'wallTime': distribution(elapsed),
          'processingMailboxLag': distribution(active.map((m) => m.lag).toList()),
          'idleMailboxLag': distribution(idle.map((m) => m.lag).toList()),
          'totalHeartbeatSamples': messages.length,
        });
      }
    }
    final result = {
      'scope': 'Native Dart AOT; actual production board/ratings, exact extracted current size-gated decoder, existing production model constructors and independent 265-row reference. Separate producer isolate timestamps each message using Timeline.now. Receiver measures arrival lag during one request-sized parse; 17-23 ms idle gaps avoid a synthetic synchronous bulk loop. One second baseline. Shared Windows desktop, not Flutter frame timing, UI presentation, 32 MiB maximum or device acceptance.',
      'baseline': baseline,
      'cases': rows,
    };
    await File(args[1]).writeAsString('${const JsonEncoder.withIndent('  ').convert(result)}\n');
    stdout.writeln(jsonEncode(result));
  } finally {
    sender.kill(priority: Isolate.immediate);
    await subscription.cancel();
    port.close();
  }
}
