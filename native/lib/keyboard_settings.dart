import 'package:flutter/services.dart';
import 'package:material_ui/material_ui.dart';

import 'catalog.dart';
import 'input_binding_row.dart';
import 'input_settings_frame.dart';
import 'keyboard_layouts.dart';
import 'l10n/strings.dart';
import 'launcher_select.dart';

class KeyboardSettings extends StatefulWidget {
  const KeyboardSettings({
    super.key,
    required this.game,
    required this.store,
    required this.onSaving,
  });

  final Game game;
  final KeyboardLayoutStore store;
  final ValueChanged<bool> onSaving;

  @override
  State<KeyboardSettings> createState() => _KeyboardSettingsState();
}

class _KeyboardSettingsState extends State<KeyboardSettings> {
  late Future<KeyboardLayouts> _initial = _load();
  KeyboardLayouts? _updated;
  bool _saving = false;
  String? _feedback;

  Future<KeyboardLayouts> _load() => widget.store.load(widget.game);

  Future<void> _save(KeyboardLayouts layouts, [String? feedback]) async {
    if (_saving) return;
    setState(() {
      _saving = true;
      _feedback = null;
    });
    widget.onSaving(true);
    try {
      await widget.store.save(widget.game, layouts);
      if (!mounted) return;
      setState(() {
        _updated = layouts;
        _feedback = feedback;
      });
    } on Exception {
      if (mounted) setState(() => _feedback = context.strings.inputSaveFailed);
    } finally {
      if (mounted) {
        setState(() => _saving = false);
        widget.onSaving(false);
      }
    }
  }

  Future<void> _capture(
    KeyboardLayouts layouts,
    String action,
    String label,
  ) async {
    final code = await showDialog<String>(
      context: context,
      animationStyle: LauncherControl.overlayMotion(context),
      builder: (_) => KeyboardCaptureDialog(label: label),
    );
    if (mounted && code != null) await _save(layouts.bind(action, code));
  }

  Future<void> _nameLayout(KeyboardLayouts layouts) async {
    final name = await showKeyboardLayoutName(
      context,
      initial: layouts.saved[layouts.selected]?.name ?? '',
    );
    if (!mounted || name == null) return;
    final next = layouts.saveAs(name);
    if (next == null) {
      setState(() => _feedback = context.strings.inputNeedLayoutName);
      return;
    }
    await _save(next, context.strings.inputLayoutSaved);
  }

  @override
  Widget build(BuildContext context) => FutureBuilder<KeyboardLayouts>(
    future: _initial,
    builder: (context, snapshot) {
      if (snapshot.hasError) {
        return OutlinedButton.icon(
          onPressed: () => setState(() => _initial = _load()),
          icon: const Icon(Icons.refresh),
          label: Text(context.strings.inputReadLayout),
        );
      }
      final layouts = _updated ?? snapshot.data;
      return layouts == null
          ? const Center(child: CircularProgressIndicator())
          : _editor(layouts);
    },
  );

  Widget _editor(KeyboardLayouts layouts) {
    final actions = inputActions(
      context.strings,
      widget.game.id,
      keyboard: true,
    );
    return Column(
      crossAxisAlignment: CrossAxisAlignment.stretch,
      spacing: 8,
      children: [
        InputSettingSelect<String>(
          label: context.strings.inputKeyboardLayout,
          value: layouts.selected,
          options: {
            'arrows': context.strings.inputArrowPreset,
            'wasd': 'WASD',
            'custom': context.strings.inputCustom,
            for (final entry in layouts.saved.entries)
              entry.key: entry.value.name,
          },
          onChanged: _saving ? null : (id) => _save(layouts.select(id)),
        ),
        Wrap(
          spacing: 8,
          children: [
            OutlinedButton(
              onPressed: _saving ? null : () => _nameLayout(layouts),
              child: Text(context.strings.inputSaveLayout),
            ),
            TextButton(
              onPressed: _saving
                  ? null
                  : () => _save(layouts.select(layouts.active.base)),
              child: Text(context.strings.inputResetPreset),
            ),
            if (layouts.saved.containsKey(layouts.selected))
              TextButton(
                onPressed: _saving
                    ? null
                    : () => _save(
                        layouts.deleteSelected(),
                        context.strings.inputLayoutDeleted,
                      ),
                child: Text(context.strings.inputDeleteLayout),
              ),
          ],
        ),
        Text(context.strings.inputKeyboardHint),
        InputBindingTable(
          entries: [
            for (final MapEntry(key: action, value: label) in actions.entries)
              InputBindingEntry(
                bindingKey: 'keyboard-$action',
                action: label,
                keys: layouts.active
                    .keysFor(layouts.profile.actions[action]!)
                    .map(keyboardKeyLabel)
                    .join(' / '),
                onBind: _saving ? null : () => _capture(layouts, action, label),
                onReset: _saving ? null : () => _save(layouts.bind(action)),
                resetLabel: context.strings.inputResetKey,
              ),
          ],
        ),
        Semantics(
          liveRegion: true,
          child: Text(
            _feedback ??
                (_saving
                    ? context.strings.inputSaving
                    : context.strings.inputAutosave),
          ),
        ),
      ],
    );
  }
}

Future<String?> showKeyboardLayoutName(
  BuildContext context, {
  String initial = '',
}) {
  var name = initial;
  return showDialog<String>(
    context: context,
    animationStyle: LauncherControl.overlayMotion(context),
    builder: (context) => StatefulBuilder(
      builder: (context, update) => AlertDialog(
        title: Text(context.strings.inputSaveLayout),
        content: TextFormField(
          autofocus: true,
          maxLength: 32,
          initialValue: initial,
          decoration: InputDecoration(
            labelText: context.strings.inputLayoutName,
          ),
          onChanged: (value) => update(() => name = value),
          onFieldSubmitted: (value) {
            if (value.trim().isNotEmpty) Navigator.of(context).pop(value);
          },
        ),
        actions: [
          TextButton(
            onPressed: () => Navigator.of(context).pop(),
            child: Text(context.strings.cancel),
          ),
          FilledButton(
            onPressed: name.trim().isEmpty
                ? null
                : () => Navigator.of(context).pop(name),
            child: Text(context.strings.inputSave),
          ),
        ],
      ),
    ),
  );
}

class KeyboardCaptureDialog extends StatefulWidget {
  const KeyboardCaptureDialog({super.key, required this.label, this.validate});

  final String label;
  final String? Function(String)? validate;

  @override
  State<KeyboardCaptureDialog> createState() => _KeyboardCaptureDialogState();
}

class _KeyboardCaptureDialogState extends State<KeyboardCaptureDialog> {
  String? _error;

  KeyEventResult _capture(FocusNode node, KeyEvent event) {
    if (event is! KeyDownEvent) return KeyEventResult.handled;
    final key = event.physicalKey;
    final hardware = HardwareKeyboard.instance;
    final code = keyboardCodes[key.usbHidUsage];
    if (key == PhysicalKeyboardKey.escape) {
      Navigator.of(context).pop();
    } else if (code == null ||
        hardware.isAltPressed ||
        hardware.isMetaPressed) {
      setState(() => _error = context.strings.inputUnsupported);
    } else if (widget.validate?.call(code) case final error?) {
      setState(() => _error = error);
    } else {
      Navigator.of(context).pop(code);
    }
    return KeyEventResult.handled;
  }

  @override
  Widget build(BuildContext context) => Focus(
    autofocus: true,
    onKeyEvent: _capture,
    child: AlertDialog(
      title: Text(widget.label),
      content: Semantics(
        liveRegion: true,
        child: Text(_error ?? context.strings.inputCaptureHint),
      ),
      actions: [
        TextButton(
          onPressed: () => Navigator.of(context).pop(),
          child: Text(context.strings.cancel),
        ),
      ],
    ),
  );
}
