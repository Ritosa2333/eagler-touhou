import 'package:material_ui/material_ui.dart';

import 'gamepad_preferences.dart';
import 'gamepad_settings.dart';
import 'input_binding_row.dart';
import 'input_settings_frame.dart';
import 'keyboard_layouts.dart';
import 'keyboard_settings.dart';
import 'local_duel_preferences.dart';
import 'l10n/strings.dart';
import 'launcher_select.dart';

class LocalDuelSettings extends StatefulWidget {
  const LocalDuelSettings({
    super.key,
    required this.store,
    required this.gamepads,
    required this.onSaving,
  });

  final LocalDuelStore store;
  final GamepadPreferenceStore gamepads;
  final ValueChanged<bool> onSaving;

  @override
  State<LocalDuelSettings> createState() => _LocalDuelSettingsState();
}

class _LocalDuelSettingsState extends State<LocalDuelSettings> {
  late Future<LocalDuelPreferences> _initial = widget.store.load();
  LocalDuelPreferences? _state;
  int _seat = 0;
  bool _saving = false;
  String? _feedback;

  Future<String?> _persist(LocalDuelPreferences next) async {
    final strings = context.strings;
    if (_saving) return strings.inputSaving;
    setState(() {
      _saving = true;
      _feedback = null;
    });
    widget.onSaving(true);
    try {
      await widget.store.save(next);
      if (mounted) setState(() => _state = next);
      return null;
    } on Exception {
      return strings.inputSaveFailed;
    } finally {
      if (mounted) {
        setState(() => _saving = false);
        widget.onSaving(false);
      }
    }
  }

  String? _conflictLabel(int seat, DuelPlayer candidate) {
    final conflict = _state!.conflict(seat, candidate);
    if (conflict == null) return null;
    final strings = context.strings;
    return switch (conflict.key) {
      final String key => strings.inputKeyConflict(keyboardKeyLabel(key)),
      null => strings.inputControllerConflict,
    };
  }

  Future<String?> _replace(int seat, DuelPlayer candidate) async {
    final state = _state!;
    final conflict = _conflictLabel(seat, candidate);
    if (conflict != null) return conflict;
    return _persist(state.replace(seat, candidate));
  }

  Future<void> _apply(DuelPlayer player, {String? success}) async {
    final error = await _replace(_seat, player);
    if (mounted) setState(() => _feedback = error ?? success);
  }

  Future<void> _toggle(bool enabled) async {
    final error = await _persist(_state!.toggle(enabled));
    if (mounted) setState(() => _feedback = error);
  }

  Future<void> _capture(DuelPlayer player, String action, String label) async {
    final seat = _seat;
    final code = await showDialog<String>(
      context: context,
      animationStyle: LauncherControl.overlayMotion(context),
      builder: (_) => KeyboardCaptureDialog(
        label: label,
        validate: (key) => _conflictLabel(seat, player.bind(action, key)),
      ),
    );
    if (mounted && code != null) await _apply(player.bind(action, code));
  }

  Future<void> _saveLayout(DuelPlayer player) async {
    final name = await showKeyboardLayoutName(
      context,
      initial: player.saved[player.layout]?.name ?? '',
    );
    if (!mounted || name == null) return;
    final saved = player.saveLayout(name);
    if (saved == null) {
      setState(() => _feedback = context.strings.inputNeedLayoutName);
    } else {
      await _apply(saved, success: context.strings.inputLayoutSaved);
    }
  }

  void _selectLayout(LocalDuelPreferences state, DuelPlayer player, String id) {
    final bindings = player.saved[id]?.bindings ?? state.presets[id]?[_seat];
    if (bindings != null) _apply(player.change(layout: id, bindings: bindings));
  }

  Future<void> _assignController(DuelPlayer player) async {
    final seat = _seat;
    var saving = false;
    await showDialog<void>(
      context: context,
      animationStyle: LauncherControl.overlayMotion(context),
      barrierDismissible: false,
      builder: (_) => StatefulBuilder(
        builder: (dialog, update) => InputSettingsFrame(
          title: dialog.strings.inputConfigureController,
          saving: saving,
          body: GamepadSettings(
            gameId: 'th09',
            store: widget.gamepads,
            onSaving: (value) => update(() => saving = value),
            assignment: (
              controller: player.controller,
              change: (id) =>
                  _replace(seat, _state!.players[seat].change(controller: id)),
            ),
          ),
        ),
      ),
    );
  }

  @override
  Widget build(BuildContext context) => FutureBuilder<LocalDuelPreferences>(
    future: _initial,
    builder: (context, snapshot) {
      if (snapshot.hasError) {
        return OutlinedButton.icon(
          onPressed: () => setState(() => _initial = widget.store.load()),
          icon: const Icon(Icons.refresh),
          label: Text(context.strings.inputReadDuel),
        );
      }
      if (snapshot.data case final state?) {
        return _editor(_state ??= state);
      }
      return const Center(child: CircularProgressIndicator());
    },
  );

  Widget _editor(LocalDuelPreferences state) {
    final player = state.players[_seat];
    return Column(
      spacing: 8,
      crossAxisAlignment: CrossAxisAlignment.stretch,
      children: [
        SwitchListTile(
          contentPadding: EdgeInsets.zero,
          title: Text(
            context.strings.inputDuelEnabled,
            style: Theme.of(context).textTheme.bodyMedium,
          ),
          value: state.enabled,
          onChanged: _saving ? null : _toggle,
        ),
        Text(context.strings.inputDuelNote),
        LauncherSegments<int>(
          segments: [
            for (var seat = 0; seat < 2; seat++)
              ButtonSegment(
                value: seat,
                label: Text(context.strings.inputPlayer(seat + 1)),
              ),
          ],
          value: _seat,
          onChanged: _saving
              ? null
              : (value) => setState(() {
                  _seat = value;
                  _feedback = null;
                }),
        ),
        InputSettingSelect<bool>(
          label: context.strings.inputDevice,
          value: player.gamepad,
          options: {
            false: context.strings.inputKeyboardDevice,
            true: context.strings.inputGamepadDevice,
          },
          onChanged: _saving
              ? null
              : (value) => _apply(player.change(gamepad: value)),
        ),
        if (_feedback case final message?)
          Semantics(liveRegion: true, child: Text(message)),
        if (player.gamepad)
          OutlinedButton.icon(
            icon: const Icon(Icons.sports_esports_outlined),
            label: Text(context.strings.inputConfigureController),
            onPressed: _saving ? null : () => _assignController(player),
          )
        else ...[
          InputSettingSelect<String>(
            label: context.strings.inputDuelLayout,
            value: player.layout,
            options: {
              'original': _seat == 0
                  ? context.strings.inputOriginalLeft
                  : context.strings.inputOriginalRight,
              'wasd': 'WASD',
              'arrows': context.strings.inputArrowPreset,
              if (player.layout == 'custom')
                'custom': context.strings.inputCustom,
              for (final entry in player.saved.entries)
                entry.key: entry.value.name,
            },
            onChanged: _saving
                ? null
                : (id) => _selectLayout(state, player, id),
          ),
          Wrap(
            spacing: 8,
            children: [
              OutlinedButton(
                onPressed: _saving ? null : () => _saveLayout(player),
                child: Text(context.strings.inputSaveLayout),
              ),
              TextButton(
                onPressed: _saving
                    ? null
                    : () => _selectLayout(state, player, 'original'),
                child: Text(context.strings.inputRestoreOriginal),
              ),
              if (player.saved.containsKey(player.layout))
                TextButton(
                  onPressed: _saving
                      ? null
                      : () => _apply(player.deleteLayout()),
                  child: Text(context.strings.inputDeleteLayout),
                ),
            ],
          ),
          InputBindingTable(
            showHeading: true,
            entries: [
              for (final MapEntry(key: action, value: label)
                  in localDuelActions(context.strings).entries)
                InputBindingEntry(
                  bindingKey: 'duel-$action',
                  action: label,
                  keys: player.bindings[action]!
                      .map(keyboardKeyLabel)
                      .join(' / '),
                  onBind: _saving
                      ? null
                      : () => _capture(player, action, label),
                  onReset: _saving
                      ? null
                      : () => _apply(player.bind(action, null)),
                  resetLabel: context.strings.inputClearKey,
                ),
            ],
          ),
        ],
      ],
    );
  }
}
