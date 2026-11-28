import 'dart:convert';

import 'package:flutter/services.dart';
import 'package:shared_preferences/shared_preferences.dart';
import 'package:uuid/uuid.dart';

import 'keyboard_layouts.dart';

typedef DuelBindings = Map<String, List<String>>;
typedef DuelLayout = ({String name, DuelBindings bindings});
typedef DuelPresets = Map<String, List<DuelBindings>>;

class DuelPlayer {
  const DuelPlayer({
    required this.bindings,
    this.gamepad = false,
    this.controller = '',
    this.layout = 'original',
    this.saved = const {},
  });

  final DuelBindings bindings;
  final bool gamepad;
  final String controller;
  final String layout;
  final Map<String, DuelLayout> saved;

  DuelPlayer change({
    DuelBindings? bindings,
    bool? gamepad,
    String? controller,
    String? layout,
    Map<String, DuelLayout>? saved,
  }) => DuelPlayer(
    bindings: bindings ?? this.bindings,
    gamepad: gamepad ?? this.gamepad,
    controller: controller ?? this.controller,
    layout: layout ?? this.layout,
    saved: saved ?? this.saved,
  );

  DuelPlayer bind(String action, String? key) => change(
    layout: 'custom',
    bindings: {
      for (final entry in bindings.entries)
        entry.key: entry.key == action
            ? [?key]
            : entry.value.where((existing) => existing != key).toList(),
    },
  );

  DuelPlayer? saveLayout(String input) {
    final name = input.trim();
    final duplicate = saved.entries
        .where((entry) => entry.value.name == name)
        .firstOrNull;
    if (name.isEmpty ||
        name.length > 32 ||
        (duplicate == null && saved.length >= 32)) {
      return null;
    }
    final id = duplicate?.key ?? 'saved-${const Uuid().v4()}';
    return change(
      layout: id,
      saved: {...saved, id: (name: name, bindings: bindings)},
    );
  }

  DuelPlayer deleteLayout() =>
      change(layout: 'custom', saved: {...saved}..remove(layout));

  Map<String, Object> toJson() => {
    'gamepad': gamepad,
    'controller': controller,
    'layout': layout,
    'bindings': bindings,
    'saved': saved.map(
      (id, entry) =>
          MapEntry(id, {'name': entry.name, 'bindings': entry.bindings}),
    ),
  };
}

class LocalDuelPreferences {
  const LocalDuelPreferences(
    this.presets,
    this.players, {
    this.enabled = false,
  });

  factory LocalDuelPreferences.defaults(DuelPresets presets) =>
      LocalDuelPreferences(presets, [
        for (final bindings in presets['original']!)
          DuelPlayer(bindings: bindings),
      ]);

  final DuelPresets presets;
  final List<DuelPlayer> players;
  final bool enabled;

  LocalDuelPreferences toggle(bool enabled) =>
      LocalDuelPreferences(presets, players, enabled: enabled);

  LocalDuelPreferences replace(int seat, DuelPlayer player) =>
      LocalDuelPreferences(presets, [
        for (var index = 0; index < players.length; index++)
          index == seat ? player : players[index],
      ], enabled: enabled);

  /// Shared menu actions may have the same physical key on both seats.
  ({String? key})? conflict(int seat, DuelPlayer candidate) {
    final other = players[1 - seat];
    if (candidate.gamepad != other.gamepad) return null;
    if (candidate.gamepad) {
      return candidate.controller.isNotEmpty &&
              candidate.controller == other.controller
          ? (key: null)
          : null;
    }
    const shared = {'pause', 'confirm', 'skip', 'restart'};
    for (final entry in candidate.bindings.entries) {
      for (final peer in other.bindings.entries) {
        if (entry.key == peer.key && shared.contains(entry.key)) continue;
        for (final key in entry.value) {
          if (peer.value.contains(key)) {
            return (key: key);
          }
        }
      }
    }
    return null;
  }

  Map<String, Object> toJson() => {
    'enabled': enabled,
    'players': players.map((player) => player.toJson()).toList(),
  };
}

class LocalDuelStore {
  LocalDuelStore(this._storage);

  static const _key = 'th09-local-input.v1';
  final SharedPreferencesAsync _storage;
  DuelPresets? _presets;

  Future<DuelPresets> _loadPresets() async {
    final asset = await rootBundle.loadString(
      'assets/th09_local_inputs.json',
      cache: false,
    );
    final json = jsonDecode(asset) as Map<String, dynamic>;
    return json.map(
      (name, value) => MapEntry(name, [
        for (final item in value as List)
          (item as Map<String, dynamic>).map(
            (action, keys) => MapEntry(action, List<String>.from(keys as List)),
          ),
      ]),
    );
  }

  Future<LocalDuelPreferences> load() async {
    final presets = _presets ??= await _loadPresets();
    final defaults = LocalDuelPreferences.defaults(presets);
    final saved = await _storage.getString(_key);
    if (saved == null) return defaults;
    try {
      final json = jsonDecode(saved);
      if (json is! Map<String, dynamic>) return defaults;
      final players = json['players'];
      if (players is! List || players.length != 2) return defaults;
      final decoded = LocalDuelPreferences(presets, [
        for (var seat = 0; seat < 2; seat++)
          _readPlayer(players[seat], presets, seat),
      ], enabled: json['enabled'] == true);
      return decoded.conflict(0, decoded.players.first) == null
          ? decoded
          : defaults;
    } on FormatException {
      return defaults;
    }
  }

  DuelPlayer _readPlayer(Object? json, DuelPresets presets, int seat) {
    final original = presets['original']![seat];
    if (json is! Map<String, dynamic>) return DuelPlayer(bindings: original);
    DuelBindings? readBindings(Object? raw) {
      if (raw is! Map<String, dynamic>) return null;
      final result = <String, List<String>>{};
      for (final action in original.keys) {
        final keys = raw[action];
        if (keys is! List ||
            keys.length > 8 ||
            !keys.every(
              (key) => key is String && keyboardCodes.containsValue(key),
            )) {
          return null;
        }
        result[action] = keys.cast<String>();
      }
      return result;
    }

    final layouts = json['saved'];
    final saved = <String, DuelLayout>{};
    if (layouts is Map<String, dynamic>) {
      for (final entry in layouts.entries.take(32)) {
        final item = entry.value;
        if (!entry.key.startsWith('saved-') || item is! Map<String, dynamic>) {
          continue;
        }
        final name = item['name'];
        final bindings = readBindings(item['bindings']);
        if (name is String &&
            name.trim().isNotEmpty &&
            name.length <= 32 &&
            bindings != null) {
          saved[entry.key] = (name: name, bindings: bindings);
        }
      }
    }
    final requested = json['layout'];
    final controller = json['controller'];
    final custom = readBindings(json['bindings']);
    final layout =
        requested is String &&
            (presets.containsKey(requested) ||
                saved.containsKey(requested) ||
                (requested == 'custom' && custom != null))
        ? requested
        : 'original';
    return DuelPlayer(
      gamepad: json['gamepad'] == true,
      controller: controller is String && controller.length <= 1024
          ? controller
          : '',
      bindings:
          saved[layout]?.bindings ??
          presets[layout]?[seat] ??
          custom ??
          original,
      layout: layout,
      saved: saved,
    );
  }

  Future<void> save(LocalDuelPreferences state) =>
      _storage.setString(_key, jsonEncode(state.toJson()));
}
