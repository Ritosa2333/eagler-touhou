import 'dart:convert';

import 'package:flutter/services.dart';
import 'package:shared_preferences/shared_preferences.dart';
import 'package:uuid/uuid.dart';

import 'catalog.dart';

class KeyboardProfile {
  const KeyboardProfile(this.actions, this.wasd);

  final Map<String, String> actions;
  final Map<String, String> wasd;

  KeyboardBindings preset(String id) =>
      KeyboardBindings(id, id == 'wasd' ? wasd : const {});
}

class KeyboardBindings {
  const KeyboardBindings(this.base, this.aliases);

  final String base;
  final Map<String, String> aliases;

  List<String> keysFor(String target) => {
    for (final entry in aliases.entries)
      if (entry.value == target) entry.key,
    if ((aliases[target] ?? target) == target) target,
  }.toList();

  Map<String, Object> toJson() => {'base': base, 'aliases': aliases};
}

class SavedKeyboardLayout {
  const SavedKeyboardLayout(this.name, this.bindings);

  final String name;
  final KeyboardBindings bindings;

  Map<String, Object> toJson() => {'name': name, 'bindings': bindings.toJson()};
}

class KeyboardLayouts {
  const KeyboardLayouts({
    required this.profile,
    this.selected = 'arrows',
    this.custom = const KeyboardBindings('arrows', {}),
    this.saved = const {},
  });

  final KeyboardProfile profile;
  final String selected;
  final KeyboardBindings custom;
  final Map<String, SavedKeyboardLayout> saved;

  KeyboardBindings get active =>
      saved[selected]?.bindings ??
      (selected == 'custom' ? custom : profile.preset(selected));

  KeyboardLayouts select(String id) => KeyboardLayouts(
    profile: profile,
    selected: id,
    custom: custom,
    saved: saved,
  );

  KeyboardLayouts bind(String action, [String? code]) {
    final target = profile.actions[action]!;
    final aliases = {
      for (final entry in active.aliases.entries)
        if (entry.value != target) entry.key: entry.value,
      ?code: target,
    };
    return KeyboardLayouts(
      profile: profile,
      selected: 'custom',
      custom: KeyboardBindings(active.base, aliases),
      saved: saved,
    );
  }

  KeyboardLayouts? saveAs(String input) {
    final name = input.trim();
    if (name.isEmpty || name.length > 32) return null;
    final existing = saved.entries.where((item) => item.value.name == name);
    if (existing.isEmpty && saved.length >= 32) return null;
    final id = existing.firstOrNull?.key ?? 'saved-${const Uuid().v4()}';
    return KeyboardLayouts(
      profile: profile,
      selected: id,
      custom: custom,
      saved: {...saved, id: SavedKeyboardLayout(name, active)},
    );
  }

  KeyboardLayouts deleteSelected() => KeyboardLayouts(
    profile: profile,
    selected: 'custom',
    custom: active,
    saved: {...saved}..remove(selected),
  );

  Map<String, Object> toJson() => {
    'selected': selected,
    'custom': custom.toJson(),
    'saved': saved.map((key, value) => MapEntry(key, value.toJson())),
  };

  factory KeyboardLayouts.fromJson(KeyboardProfile profile, Object? json) {
    if (json is! Map<String, dynamic>) return KeyboardLayouts(profile: profile);
    KeyboardBindings readBindings(Object? json) {
      if (json is! Map<String, dynamic>) return profile.preset('arrows');
      final aliases = json['aliases'];
      final valid = {
        if (aliases is Map<String, dynamic>)
          for (final entry in aliases.entries)
            if (keyboardCodes.containsValue(entry.key) &&
                profile.actions.containsValue(entry.value))
              entry.key: entry.value as String,
      };
      return KeyboardBindings(
        json['base'] == 'wasd' ? 'wasd' : 'arrows',
        valid,
      );
    }

    final entries = json['saved'];
    final saved = <String, SavedKeyboardLayout>{};
    if (entries is Map<String, dynamic>) {
      for (final entry in entries.entries.take(32)) {
        final item = entry.value;
        if (!entry.key.startsWith('saved-') || item is! Map<String, dynamic>) {
          continue;
        }
        final name = item['name'];
        if (name is! String || name.trim().isEmpty || name.length > 32) {
          continue;
        }
        saved[entry.key] = SavedKeyboardLayout(
          name,
          readBindings(item['bindings']),
        );
      }
    }

    final requested = json['selected'];
    final selected =
        requested is String &&
            (saved.containsKey(requested) ||
                const ['arrows', 'wasd', 'custom'].contains(requested))
        ? requested
        : 'arrows';
    return KeyboardLayouts(
      profile: profile,
      selected: selected,
      custom: readBindings(json['custom']),
      saved: saved,
    );
  }
}

class KeyboardLayoutStore {
  KeyboardLayoutStore(this._storage);

  final SharedPreferencesAsync _storage;
  Map<String, dynamic>? _profiles;

  String _scope(Game game) =>
      const {'th075', 'th19'}.contains(game.id) ? game.id : 'general';

  String _key(Game game) => 'keyboard-layouts.v1.${_scope(game)}';

  Future<KeyboardLayouts> load(Game game) async {
    final profiles = _profiles ??= jsonDecode(
      await rootBundle.loadString('assets/keyboard_layouts.json', cache: false),
    ) as Map<String, dynamic>;
    final profile = profiles[_scope(game)] as Map<String, dynamic>;
    final definition = KeyboardProfile(
      Map<String, String>.from(profile['actions'] as Map),
      Map<String, String>.from(profile['wasd'] as Map),
    );
    final stored = await _storage.getString(_key(game));
    try {
      return KeyboardLayouts.fromJson(
        definition,
        stored == null ? null : jsonDecode(stored),
      );
    } on FormatException {
      return KeyboardLayouts(profile: definition);
    }
  }

  Future<void> save(Game game, KeyboardLayouts layouts) =>
      _storage.setString(_key(game), jsonEncode(layouts.toJson()));
}

// Flutter's keys are USB HID usages. Only consecutive ranges use arithmetic.
final keyboardCodes = <int, String>{
  for (var i = 0; i < 26; i++)
    PhysicalKeyboardKey.keyA.usbHidUsage + i:
        'Key${String.fromCharCode(65 + i)}',
  for (var i = 0; i < 10; i++) ...{
    PhysicalKeyboardKey.digit1.usbHidUsage + i: 'Digit${(i + 1) % 10}',
    PhysicalKeyboardKey.numpad1.usbHidUsage + i: 'Numpad${(i + 1) % 10}',
  },
  for (var i = 0; i < 12; i++)
    PhysicalKeyboardKey.f1.usbHidUsage + i: 'F${i + 1}',
  for (final entry in {
    PhysicalKeyboardKey.space: 'Space',
    PhysicalKeyboardKey.enter: 'Enter',
    PhysicalKeyboardKey.numpadEnter: 'NumpadEnter',
    PhysicalKeyboardKey.escape: 'Escape',
    PhysicalKeyboardKey.tab: 'Tab',
    PhysicalKeyboardKey.backspace: 'Backspace',
    PhysicalKeyboardKey.home: 'Home',
    PhysicalKeyboardKey.arrowUp: 'ArrowUp',
    PhysicalKeyboardKey.arrowDown: 'ArrowDown',
    PhysicalKeyboardKey.arrowLeft: 'ArrowLeft',
    PhysicalKeyboardKey.arrowRight: 'ArrowRight',
    PhysicalKeyboardKey.shiftLeft: 'ShiftLeft',
    PhysicalKeyboardKey.shiftRight: 'ShiftRight',
    PhysicalKeyboardKey.controlLeft: 'ControlLeft',
    PhysicalKeyboardKey.controlRight: 'ControlRight',
    PhysicalKeyboardKey.numpadAdd: 'NumpadAdd',
    PhysicalKeyboardKey.numpadSubtract: 'NumpadSubtract',
    PhysicalKeyboardKey.numpadMultiply: 'NumpadMultiply',
    PhysicalKeyboardKey.numpadDivide: 'NumpadDivide',
    PhysicalKeyboardKey.numpadDecimal: 'NumpadDecimal',
  }.entries)
    entry.key.usbHidUsage: entry.value,
};

const inputActionIds = [
  'up',
  'down',
  'left',
  'right',
  'shoot',
  'bomb',
  'focus',
  'action',
  'pause',
  'confirm',
  'skip',
  'restart',
];

String keyboardKeyLabel(String code) =>
    const {
      'ArrowUp': '↑',
      'ArrowDown': '↓',
      'ArrowLeft': '←',
      'ArrowRight': '→',
      'ShiftLeft': 'Shift',
      'ShiftRight': 'R Shift',
      'ControlLeft': 'Ctrl',
      'ControlRight': 'R Ctrl',
    }[code] ??
    code
        .replaceFirst(RegExp(r'^(Key|Digit)'), '')
        .replaceFirst('Numpad', 'Num ');
