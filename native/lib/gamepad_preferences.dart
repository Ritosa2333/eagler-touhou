import 'dart:convert';

import 'package:flutter/services.dart';
import 'package:just_gamepads/just_gamepads.dart';
import 'package:shared_preferences/shared_preferences.dart';

import 'keyboard_layouts.dart';

class PadControl {
  const PadControl.button(this.index) : direction = 0;
  const PadControl.axis(this.index, this.direction);

  final int index;

  /// A standard button has direction 0. Stick axes use -1 or +1.
  final int direction;

  static PadControl? fromJson(Object? json) {
    if (json is! Map<String, dynamic> || json['index'] is! int) return null;
    final index = json['index'] as int;
    if (json['kind'] == 'button' && GamepadButton.values.contains(index)) {
      return PadControl.button(index);
    }
    final value = json['value'];
    if (json['kind'] == 'axis' &&
        index >= 0 &&
        index < 4 &&
        (value == 1 || value == -1)) {
      return PadControl.axis(index, value == 1 ? 1 : -1);
    }
    return null;
  }

  Map<String, Object> toJson() => {
    'index': index,
    'kind': direction == 0 ? 'button' : 'axis',
    if (direction != 0) ...{'rest': 0, 'value': direction},
  };

  bool pressed(GamepadState state, double threshold) {
    if (direction != 0) {
      return (state.axes[GamepadAxis.values[index]] ?? 0) * direction >=
          threshold;
    }
    final trigger = switch (index) {
      GamepadButton.leftTriggerButton => state.axes[GamepadAxis.leftTrigger],
      GamepadButton.rightTriggerButton => state.axes[GamepadAxis.rightTrigger],
      _ => null,
    };
    return trigger == null
        ? state.buttonsDown.contains(index)
        : trigger >= threshold;
  }
}

class PadProfile {
  const PadProfile(this.bindings, this.threshold);

  final Map<String, List<PadControl>> bindings;
  final double threshold;

  factory PadProfile.fromJson(Map<String, dynamic> json) {
    final controls = json['bindings'];
    final threshold = json['threshold'];
    return PadProfile(
      {
        for (final action in inputActionIds)
          action: [
            if (controls is Map<String, dynamic> && controls[action] is List)
              for (final value in (controls[action] as List).take(4))
                ?PadControl.fromJson(value),
          ],
      },
      threshold is num && threshold.isFinite
          ? threshold.toDouble().clamp(.1, .9)
          : .5,
    );
  }

  PadProfile bind(String action, PadControl? control) => PadProfile({
    ...bindings,
    action: [?control],
  }, threshold);

  Set<String> pressedActions(GamepadState state) => {
    for (final MapEntry(:key, :value) in bindings.entries)
      if (value.any((control) => control.pressed(state, threshold))) key,
  };

  Map<String, Object> toJson() => {
    'threshold': threshold,
    'bindings': bindings.map(
      (action, controls) => MapEntry(
        action,
        controls.map((control) => control.toJson()).toList(),
      ),
    ),
  };
}

class PadMappings {
  const PadMappings({
    required this.defaults,
    this.enabled = false,
    this.selected = '',
    this.profiles = const {},
  });

  final PadProfile defaults;
  final bool enabled;
  final String selected;
  final Map<String, PadProfile> profiles;

  PadProfile get profile => profiles[selected] ?? defaults;

  PadMappings change({bool? enabled, String? selected, PadProfile? profile}) =>
      PadMappings(
        defaults: defaults,
        enabled: enabled ?? this.enabled,
        selected: selected ?? this.selected,
        profiles: {...profiles, this.selected: ?profile},
      );

  factory PadMappings.fromJson(PadProfile defaults, Map<String, dynamic> json) {
    final saved = json['profiles'];
    final selected = json['selected'];
    return PadMappings(
      defaults: defaults,
      enabled: json['enabled'] == true,
      selected: selected is String ? selected : '',
      profiles: {
        if (saved is Map<String, dynamic>)
          for (final MapEntry(:key, :value) in saved.entries)
            if (value is Map<String, dynamic>) key: PadProfile.fromJson(value),
      },
    );
  }

  Map<String, Object> toJson() => {
    'enabled': enabled,
    'selected': selected,
    'profiles': profiles.map((id, profile) => MapEntry(id, profile.toJson())),
  };
}

class GamepadPreferenceStore {
  GamepadPreferenceStore(this._storage);

  final SharedPreferencesAsync _storage;
  PadProfile? _defaults;

  String _key(String gameId) =>
      'gamepad-mapping.v1.${gameId == 'th19' ? 'th19' : 'general'}';

  Future<PadProfile> _loadDefaults() async {
    final asset = await rootBundle.loadString(
      'assets/gamepad_defaults.json',
      cache: false,
    );
    return PadProfile.fromJson(jsonDecode(asset) as Map<String, dynamic>);
  }

  Future<PadMappings> load(String gameId) async {
    final defaults = _defaults ??= await _loadDefaults();
    final saved = await _storage.getString(_key(gameId));
    try {
      final json = saved == null ? null : jsonDecode(saved);
      if (json is Map<String, dynamic>) {
        return PadMappings.fromJson(defaults, json);
      }
    } on FormatException {
      // Discard a malformed profile and offer the standard layout.
    }
    return PadMappings(defaults: defaults);
  }

  Future<void> save(String gameId, PadMappings mappings) =>
      _storage.setString(_key(gameId), jsonEncode(mappings.toJson()));
}
