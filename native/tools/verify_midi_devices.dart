import 'dart:convert';
import 'dart:io';

import 'package:flutter/widgets.dart';
import 'package:touhouonline/external_midi.dart';

Future<void> main() async {
  WidgetsFlutterBinding.ensureInitialized();
  final destination = Platform.environment['TOUHOU_MIDI_REPORT'];
  if (destination == null) {
    stderr.writeln('Set TOUHOU_MIDI_REPORT to an external output file');
    exit(64);
  }
  final midi = ExternalMidi();
  var failed = false;
  Object? failure;
  try {
    await midi.setEnabled(true).timeout(const Duration(seconds: 15));
    failed = midi.failed;
  } catch (error) {
    failed = true;
    failure = error;
  }
  try {
    await File(destination).writeAsString(
      const JsonEncoder.withIndent('  ').convert({
        'platform': Platform.operatingSystem,
        'failed': failed,
        if (failure != null) 'error': failure.toString(),
        'outputs': [
          for (final output in midi.outputs)
            {'id': output.id, 'name': output.name},
        ],
      }),
      flush: true,
    );
  } catch (error) {
    failed = true;
    stderr.writeln(error);
  } finally {
    midi.dispose();
  }
  exit(failed ? 1 : 0);
}
