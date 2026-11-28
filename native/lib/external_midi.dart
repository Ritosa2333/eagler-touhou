import 'dart:async';
import 'dart:io';

import 'package:flutter_midi_command/flutter_midi_command.dart';
import 'package:material_ui/material_ui.dart';

import 'l10n/strings.dart';
import 'launcher_select.dart';

typedef MidiOutput = ({String id, String name});

class ExternalMidi extends ChangeNotifier {
  ExternalMidi({
    Future<List<MidiOutput>> Function()? read,
    Stream<void> Function()? changes,
  }) : _read = read ?? _nativeOutputs,
       _changes = changes ?? _nativeChanges;

  final Future<List<MidiOutput>> Function() _read;
  final Stream<void> Function() _changes;
  StreamSubscription<void>? _subscription;
  bool enabled = false;
  bool loading = false;
  bool failed = false;
  List<MidiOutput> outputs = const [];
  int _request = 0;
  bool _disposed = false;

  static Future<List<MidiOutput>> _nativeOutputs() async => [
    for (final device in await MidiCommand().devices ?? <MidiDevice>[])
      // WinMM describes host outputs; Android describes device inputs.
      if ((Platform.isWindows ? device.outputPorts : device.inputPorts)
          .isNotEmpty)
        (id: device.id, name: device.name),
  ];

  static Stream<void> _nativeChanges() =>
      MidiCommand().onMidiSetupChanged?.map((_) {}) ?? const Stream.empty();

  Future<void> setEnabled(bool value) async {
    if (_disposed || value == enabled) return;
    enabled = value;
    _request++;
    if (!value) {
      unawaited(_subscription?.cancel());
      _subscription = null;
      loading = failed = false;
      outputs = const [];
      notifyListeners();
      return;
    }
    try {
      _subscription = _changes().listen(
        (_) => unawaited(refresh()),
        onError: (Object _, StackTrace _) {
          if (!_disposed && enabled) {
            failed = true;
            loading = false;
            notifyListeners();
          }
        },
      );
      await refresh();
    } on Exception {
      if (!_disposed && enabled) {
        loading = false;
        failed = true;
        notifyListeners();
      }
    }
  }

  Future<void> refresh() async {
    if (_disposed || !enabled) return;
    final request = ++_request;
    loading = true;
    failed = false;
    notifyListeners();
    try {
      final next = await _read();
      if (_disposed || request != _request) return;
      outputs = List.unmodifiable(next);
    } on Exception {
      if (_disposed || request != _request) return;
      failed = true;
    } finally {
      if (!_disposed && request == _request) {
        loading = false;
        notifyListeners();
      }
    }
  }

  @override
  void dispose() {
    _disposed = true;
    _request++;
    unawaited(_subscription?.cancel());
    super.dispose();
  }
}

class ExternalMidiSettings extends StatelessWidget {
  const ExternalMidiSettings({
    super.key,
    required this.midi,
    required this.selectedId,
    required this.onChanged,
  });

  final ExternalMidi midi;
  final String selectedId;
  final ValueChanged<String>? onChanged;

  @override
  Widget build(BuildContext context) => ListenableBuilder(
    listenable: midi,
    builder: (context, _) {
      final strings = context.strings;
      final available = midi.outputs;
      final missing =
          selectedId.isNotEmpty &&
          !available.any((output) => output.id == selectedId);
      return Column(
        children: [
          SwitchListTile(
            key: const ValueKey('external MIDI enabled'),
            title: Text(strings.externalMidi),
            value: midi.enabled,
            onChanged: (value) => unawaited(midi.setEnabled(value)),
          ),
          if (midi.enabled) ...[
            const Divider(height: 1, indent: 48, endIndent: 16),
            LauncherSettingRow(
              label: strings.externalMidiDevice,
              description: midi.failed
                  ? strings.externalMidiReadFailed
                  : available.isEmpty && !midi.loading
                  ? strings.externalMidiNoDevice
                  : null,
              controlWidth: midi.loading
                  ? 24
                  : midi.failed || available.isEmpty
                  ? 48
                  : (MediaQuery.sizeOf(context).width * .42)
                        .clamp(112, 152)
                        .toDouble(),
              control: midi.loading
                  ? const SizedBox.square(
                      dimension: 24,
                      child: CircularProgressIndicator(strokeWidth: 2),
                    )
                  : midi.failed || available.isEmpty
                  ? IconButton(
                      tooltip: strings.externalMidiRefresh,
                      onPressed: () => unawaited(midi.refresh()),
                      icon: const Icon(Icons.refresh),
                    )
                  : LauncherSelect<String>(
                      key: const ValueKey('external MIDI device'),
                      label: strings.externalMidiDevice,
                      value: selectedId.isEmpty
                          ? available.first.id
                          : selectedId,
                      labels: {
                        if (missing)
                          selectedId: strings.externalMidiDisconnected,
                        for (final output in available) output.id: output.name,
                      },
                      expand: true,
                      onChanged: onChanged == null
                          ? null
                          : (id) {
                              if (id != null) onChanged!(id);
                            },
                    ),
            ),
          ],
        ],
      );
    },
  );
}
