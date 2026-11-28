import 'dart:async';
import 'dart:convert';

import 'package:flutter/services.dart';
import 'package:just_gamepads/just_gamepads.dart';
import 'package:material_ui/material_ui.dart';

import 'gamepad_preferences.dart';
import 'input_binding_row.dart';
import 'input_settings_frame.dart';
import 'l10n/strings.dart';

class GamepadSettings extends StatefulWidget {
  const GamepadSettings({
    super.key,
    required this.gameId,
    required this.store,
    required this.onSaving,
    this.assignment,
  });

  final String gameId;
  final GamepadPreferenceStore store;
  final ValueChanged<bool> onSaving;
  final ({String controller, Future<String?> Function(String) change})?
  assignment;

  @override
  State<GamepadSettings> createState() => _GamepadSettingsState();
}

class _GamepadSettingsState extends State<GamepadSettings> {
  final _manager = GamepadManager();
  final _inputFocus = FocusNode(skipTraversal: true);
  final _keys = <String, String>{};
  late final StreamSubscription<GamepadConnectionEvent> _connections;
  late final StreamSubscription<GamepadStateEvent> _states;
  late final AppLifecycleListener _lifecycle;
  late Future<PadMappings> _initial = _load();
  PadMappings? _updated;
  bool _saving = false;
  bool _armed = false;
  String _generalSelection = '';
  String? _learning;
  String? _feedback;
  Set<String> _active = {};

  Future<PadMappings> _load() async {
    final mappings = await widget.store.load(widget.gameId);
    _generalSelection = mappings.selected;
    return mappings.change(selected: widget.assignment?.controller);
  }

  @override
  void initState() {
    super.initState();
    _connections = _manager.connections.listen((event) => _refreshDevices());
    _states = _manager.stateChanges.listen(_observe);
    _lifecycle = AppLifecycleListener(onInactive: _cancelLearning);
    unawaited(_manager.start());
  }

  @override
  void dispose() {
    _inputFocus.dispose();
    _lifecycle.dispose();
    unawaited(_connections.cancel());
    unawaited(_states.cancel());
    _manager.dispose();
    super.dispose();
  }

  void _refreshDevices() {
    final devices = _manager.connected;
    final current = devices.map((device) => device.id).toSet();
    setState(() {
      _keys.removeWhere((id, _) => !current.contains(id));
      for (final device in devices) {
        _keys.putIfAbsent(device.id, () {
          final identity = jsonEncode([
            device.vendorId,
            device.productId,
            device.name,
          ]);
          var ordinal = 0;
          while (_keys.containsValue('$identity#$ordinal')) {
            ordinal++;
          }
          return '$identity#$ordinal';
        });
      }
      if (_updated?.selected case final selected?
          when !_keys.containsValue(selected)) {
        _learning = null;
        _active = {};
      }
    });
  }

  Future<void> _save(PadMappings next, {bool changeAssignment = false}) async {
    if (_saving) return;
    setState(() {
      _saving = true;
      _learning = null;
      _feedback = null;
    });
    widget.onSaving(true);
    try {
      final assignment = widget.assignment;
      if (assignment != null && changeAssignment) {
        final conflict = await assignment.change(next.selected);
        if (conflict != null) {
          if (mounted) setState(() => _feedback = conflict);
          return;
        }
      } else {
        await widget.store.save(
          widget.gameId,
          next.change(selected: assignment == null ? null : _generalSelection),
        );
      }
      if (mounted) {
        setState(() {
          _updated = next;
          _active = {};
        });
      }
    } on Exception {
      if (mounted) setState(() => _feedback = context.strings.inputSaveFailed);
    } finally {
      if (mounted) {
        setState(() => _saving = false);
        widget.onSaving(false);
      }
    }
  }

  void _cancelLearning() {
    if (mounted && _learning != null) setState(() => _learning = null);
  }

  void _observe(GamepadStateEvent event) {
    final mappings = _updated;
    if (!mounted ||
        mappings == null ||
        _keys[event.gamepadId] != mappings.selected ||
        ModalRoute.of(context)?.isCurrent != true) {
      return;
    }
    final state = event.state;
    final action = _learning;
    if (action != null) {
      if (!_armed) {
        _armed = _isNeutral(state);
      } else if (_learnedControl(state) case final control?) {
        unawaited(
          _save(
            mappings.change(profile: mappings.profile.bind(action, control)),
          ),
        );
        return;
      }
    }
    final active = mappings.profile.pressedActions(state);
    if (_active.length != active.length || !_active.containsAll(active)) {
      setState(() => _active = active);
    }
  }

  bool _isNeutral(GamepadState state) =>
      state.buttonsDown.isEmpty &&
      state.axes.values.every((value) => value.abs() < .25);

  PadControl? _learnedControl(GamepadState state) {
    for (final index in GamepadButton.values) {
      if (PadControl.button(index).pressed(state, .5)) {
        return PadControl.button(index);
      }
    }
    for (var index = 0; index < 4; index++) {
      final value = state.axes[GamepadAxis.values[index]] ?? 0;
      if (value.abs() >= .5) return PadControl.axis(index, value.sign.toInt());
    }
    return null;
  }

  void _beginLearning(String action, GamepadInfo device) {
    if (_learning == action) {
      _cancelLearning();
      return;
    }
    final latest = _manager.latestState(device.id);
    setState(() {
      _learning = action;
      _feedback = null;
      _armed = latest == null || _isNeutral(latest);
    });
    _inputFocus.requestFocus();
  }

  @override
  Widget build(BuildContext context) => Focus(
    focusNode: _inputFocus,
    skipTraversal: true,
    onKeyEvent: (_, event) {
      if (_learning == null || event.logicalKey != LogicalKeyboardKey.escape) {
        return KeyEventResult.ignored;
      }
      if (event is KeyDownEvent) _cancelLearning();
      return KeyEventResult.handled;
    },
    child: PopScope(
      canPop: _learning == null,
      onPopInvokedWithResult: (didPop, _) {
        if (!didPop) _cancelLearning();
      },
      child: FutureBuilder<PadMappings>(
        future: _initial,
        builder: (context, snapshot) {
          if (snapshot.hasError) {
            return OutlinedButton.icon(
              onPressed: () => setState(() => _initial = _load()),
              icon: const Icon(Icons.refresh),
              label: Text(context.strings.inputReadController),
            );
          }
          if (snapshot.data case final mappings?) {
            return _editor(_updated ??= mappings);
          }
          return const Center(child: CircularProgressIndicator());
        },
      ),
    ),
  );

  Widget _editor(PadMappings mappings) {
    final selected = mappings.selected;
    final devices = _manager.connected.where(
      (device) => _keys.containsKey(device.id),
    );
    final device = devices
        .where((device) => _keys[device.id] == selected)
        .firstOrNull;
    final profile = mappings.profile;
    final waiting = _learning != null;
    final actions = widget.assignment == null
        ? inputActions(context.strings, widget.gameId)
        : localDuelActions(context.strings);
    return Column(
      spacing: 8,
      crossAxisAlignment: CrossAxisAlignment.stretch,
      children: [
        if (widget.assignment == null)
          SwitchListTile(
            contentPadding: EdgeInsets.zero,
            title: Text(
              context.strings.inputMappingEnabled,
              style: Theme.of(context).textTheme.bodyMedium,
            ),
            value: mappings.enabled,
            onChanged: _saving
                ? null
                : (enabled) => _save(mappings.change(enabled: enabled)),
          ),
        InputSettingSelect<String>(
          label: context.strings.inputController,
          value: selected,
          options: {
            '': context.strings.inputSelectController,
            if (selected.isNotEmpty && device == null)
              selected: context.strings.inputControllerDisconnected,
            for (final (index, info) in devices.indexed)
              _keys[info.id]!: '${info.name} (#${index + 1})',
          },
          onChanged: _saving
              ? null
              : (key) => _save(
                  mappings.change(selected: key),
                  changeAssignment: true,
                ),
        ),
        if (_feedback case final message?)
          Semantics(liveRegion: true, child: Text(message)),
        if (device == null)
          Text(context.strings.inputConnectController)
        else ...[
          Text(context.strings.inputChangeControllerBinding),
          InputBindingTable(
            entries: [
              for (final MapEntry(key: action, value: label) in actions.entries)
                InputBindingEntry(
                  bindingKey: 'gamepad-$action',
                  action: label,
                  keys: _learning == action
                      ? context.strings.inputWaiting
                      : profile.bindings[action]!
                            .map(
                              (control) => control.direction == 0
                                  ? context.strings.inputButtonNumber(
                                      control.index + 1,
                                    )
                                  : context.strings.inputAxisNumber(
                                      control.index + 1,
                                      control.direction < 0 ? '−' : '+',
                                    ),
                            )
                            .join(' / '),
                  onBind: _saving ? null : () => _beginLearning(action, device),
                  onReset: _saving
                      ? null
                      : () => _save(
                          mappings.change(profile: profile.bind(action, null)),
                        ),
                  resetLabel: context.strings.inputClearBinding,
                  active: _active.contains(action),
                ),
            ],
          ),
          _PadThreshold(
            key: ValueKey(('threshold', selected)),
            threshold: profile.threshold,
            onSave: _saving
                ? null
                : (threshold) => _save(
                    mappings.change(
                      profile: PadProfile(profile.bindings, threshold),
                    ),
                  ),
          ),
          Wrap(
            spacing: 8,
            children: [
              OutlinedButton(
                onPressed: _saving
                    ? null
                    : () => _save(mappings.change(profile: mappings.defaults)),
                child: Text(context.strings.inputControllerDefaults),
              ),
              if (waiting)
                TextButton(
                  onPressed: _cancelLearning,
                  child: Text(context.strings.inputCancelCapture),
                ),
            ],
          ),
        ],
        if (_saving || device != null)
          Semantics(
            liveRegion: true,
            child: Text(
              _saving
                  ? context.strings.inputSaving
                  : waiting
                  ? context.strings.inputReleaseControls
                  : context.strings.inputControllerAutosave,
            ),
          ),
      ],
    );
  }
}

class _PadThreshold extends StatefulWidget {
  const _PadThreshold({
    super.key,
    required this.threshold,
    required this.onSave,
  });

  final double threshold;
  final ValueChanged<double>? onSave;

  @override
  State<_PadThreshold> createState() => _PadThresholdState();
}

class _PadThresholdState extends State<_PadThreshold> {
  double? _preview;

  @override
  Widget build(BuildContext context) {
    final value = _preview ?? widget.threshold;
    return Column(
      crossAxisAlignment: CrossAxisAlignment.stretch,
      children: [
        Row(
          children: [
            Expanded(child: Text(context.strings.inputStickThreshold)),
            Text('${(value * 100).round()}%'),
          ],
        ),
        Semantics(
          label: context.strings.inputStickThreshold,
          child: Slider(
            value: value,
            min: .1,
            max: .9,
            divisions: 80,
            label: '${(value * 100).round()}%',
            semanticFormatterCallback: (value) => '${(value * 100).round()}%',
            onChanged: widget.onSave == null
                ? null
                : (value) => setState(() => _preview = value),
            onChangeEnd: widget.onSave == null
                ? null
                : (value) {
                    setState(() => _preview = null);
                    widget.onSave!(value);
                  },
          ),
        ),
      ],
    );
  }
}
