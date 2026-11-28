import 'dart:async';

import 'package:material_ui/material_ui.dart';

import 'l10n/strings.dart';
import 'network_diagnostics.dart';

class NetworkCheckDialog extends StatefulWidget {
  const NetworkCheckDialog({super.key, required this.createDiagnostics});

  final NetworkDiagnostics Function() createDiagnostics;

  @override
  State<NetworkCheckDialog> createState() => _NetworkCheckDialogState();
}

class _NetworkCheckDialogState extends State<NetworkCheckDialog> {
  final _readings = <NetworkCheck, NetworkReading>{};
  NetworkDiagnostics? _active;
  bool _running = true;

  @override
  void initState() {
    super.initState();
    WidgetsBinding.instance.addPostFrameCallback((_) {
      if (mounted) unawaited(_run());
    });
  }

  Future<void> _run() async {
    final task = widget.createDiagnostics();
    setState(() {
      _active = task;
      _running = true;
      _readings.clear();
    });
    try {
      await task.run((check, reading) {
        if (mounted && identical(_active, task)) {
          setState(() => _readings[check] = reading);
        }
      });
    } finally {
      if (mounted && identical(_active, task)) setState(() => _running = false);
    }
  }

  @override
  void dispose() {
    _active?.close();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    final strings = context.strings;
    return AlertDialog(
      insetPadding: const EdgeInsets.symmetric(horizontal: 16, vertical: 24),
      title: Text(strings.networkCheckTitle),
      content: SizedBox(
        width: 360,
        child: SingleChildScrollView(
          child: Column(
            mainAxisSize: MainAxisSize.min,
            crossAxisAlignment: CrossAxisAlignment.stretch,
            spacing: 16,
            children: [
              _group(strings.networkCheckServer, [
                _row(NetworkCheck.websocket, 'WebSocket'),
                _row(NetworkCheck.turn, 'TURN'),
              ]),
              _group(strings.networkCheckDirect, [
                _row(NetworkCheck.nat, 'NAT'),
                _row(NetworkCheck.ipv6, 'IPv6'),
              ]),
            ],
          ),
        ),
      ),
      actions: [
        TextButton(
          onPressed: _running ? null : _run,
          child: Text(strings.checkAgain),
        ),
        FilledButton(
          onPressed: () => Navigator.of(context).pop(),
          child: Text(strings.close),
        ),
      ],
    );
  }

  Widget _group(String title, List<Widget> rows) => Column(
    crossAxisAlignment: CrossAxisAlignment.stretch,
    spacing: 6,
    children: [
      Semantics(
        header: true,
        child: Text(title, style: Theme.of(context).textTheme.titleSmall),
      ),
      Card.outlined(
        margin: EdgeInsets.zero,
        child: Column(
          children: [rows.first, const Divider(height: 1), rows.last],
        ),
      ),
    ],
  );

  Widget _row(NetworkCheck check, String label) {
    final strings = context.strings;
    final colors = Theme.of(context).colorScheme;
    final reading = _readings[check] ?? networkPending;
    final (message, color, icon) = switch (reading.state) {
      NetworkCheckState.pending => (
        strings.networkCheckChecking,
        colors.onSurfaceVariant,
        Icons.more_horiz,
      ),
      NetworkCheckState.good => (
        strings.networkCheckAvailable,
        colors.primary,
        Icons.check_circle_outline,
      ),
      NetworkCheckState.warning => (
        strings.networkCheckAttention,
        colors.error,
        Icons.error_outline,
      ),
      NetworkCheckState.failed => (
        strings.networkCheckFailed,
        colors.error,
        Icons.error_outline,
      ),
      NetworkCheckState.unavailable => (
        strings.networkCheckUnavailable,
        colors.onSurfaceVariant,
        Icons.remove_circle_outline,
      ),
    };
    final value = reading.milliseconds != null
        ? strings.networkCheckLatency(reading.milliseconds!)
        : reading.nat ?? message;
    return Padding(
      padding: const EdgeInsets.symmetric(horizontal: 12, vertical: 9),
      child: Semantics(
        label: '$label | $value',
        value: reading.state == NetworkCheckState.warning ? message : null,
        liveRegion: true,
        excludeSemantics: true,
        child: Row(
          spacing: 8,
          children: [
            Expanded(child: Text(label)),
            Expanded(
              child: Text(
                value,
                textAlign: TextAlign.end,
                style: TextStyle(color: color),
              ),
            ),
            Icon(icon, size: 16, color: color),
          ],
        ),
      ),
    );
  }
}
