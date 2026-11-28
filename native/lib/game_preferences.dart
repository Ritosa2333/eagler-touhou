import 'dart:convert';

import 'package:shared_preferences/shared_preferences.dart';

import 'catalog.dart';
import 'external_midi.dart';
import 'gamepad_preferences.dart';
import 'keyboard_layouts.dart';
import 'local_duel_preferences.dart';
import 'touch_preferences.dart';

enum GameToggle {
  frameLimit60Enabled(true),
  pixelPerfectEnabled(false),
  focusHitboxEnabled(false),
  faithBarEnabled(false),
  th02RapidFire(true),
  th03RapidFire(true),
  th03FocusEnabled(true),
  th03FocusPointEnabled(true),
  touchEnabled(false),
  alwaysHitbox(false),
  magnifierEnabled(false),
  thpracEnabled(false),
  th075NativeResolution(false),
  multiplayerLocalPlayerVisibility(false);

  const GameToggle(this.defaultValue);

  final bool defaultValue;

  bool available(Game game, {required bool multiplayer}) => switch (this) {
    frameLimit60Enabled => game.settings.highRefresh,
    focusHitboxEnabled => game.settings.features.contains('focusHitbox'),
    faithBarEnabled => game.id == 'th10',
    th02RapidFire => game.id == 'th02',
    th03RapidFire || th03FocusPointEnabled => game.id == 'th03',
    th03FocusEnabled => game.id == 'th03' && !multiplayer,
    thpracEnabled => !multiplayer && game.settings.features.contains('thprac'),
    th075NativeResolution => multiplayer && game.id == 'th075',
    multiplayerLocalPlayerVisibility => multiplayer,
    _ => true,
  };
}

enum Th05DisplayOption {
  phaseMarkers,
  phaseStars,
  phaseTimeout,
  timeoutAsTime,
  numericDream,
  numericPower;

  bool canChange(Map<String, Object> display) => switch (this) {
    phaseMarkers || phaseTimeout => display['bossBar'] == 'native',
    phaseStars => display['bossBar'] == 'eosd',
    timeoutAsTime => display['phaseTimeout'] == true,
    _ => true,
  };
}

class GamePreferences {
  GamePreferences._(this.values);

  static const th05DisplayKey = 'th05MultiplayerDisplay';
  static const th05BossBars = ['native', 'eosd', 'ring'];

  static Map<String, Object> _th05Display(Object? value) {
    final saved = value is Map ? value : const <String, Object>{};
    final bar = th05BossBars.contains(saved['bossBar'])
        ? saved['bossBar'] as String
        : th05BossBars.first;
    return Map.unmodifiable({
      'bossBar': bar,
      'phaseMarkers': bar == 'native' && saved['phaseMarkers'] == true,
      'phaseStars': bar == 'eosd' && saved['phaseStars'] == true,
      'phaseTimeout': bar != 'native' || saved['phaseTimeout'] == true,
      'timeoutAsTime': saved['timeoutAsTime'] == true,
      'numericDream': saved['numericDream'] == true,
      'numericPower': saved['numericPower'] == true,
    });
  }

  factory GamePreferences.resolve(
    Game game, {
    required bool multiplayer,
    Map<String, dynamic> stored = const {},
  }) {
    final capabilities = game.settings;
    final switches = <String, Object>{
      for (final toggle in GameToggle.values)
        if (toggle.available(game, multiplayer: multiplayer))
          toggle.name: stored[toggle.name] is bool
              ? stored[toggle.name] as bool
              : toggle == GameToggle.thpracEnabled || toggle.defaultValue,
    };
    final languages = capabilities.languages.map((entry) => entry.id);
    final defaultLanguage = languages.contains('lang_zh-hans')
        ? 'lang_zh-hans'
        : languages.firstOrNull ?? 'ja';
    final savedLanguage = stored['language'];
    final savedMusic = stored['music'];
    final scales = capabilities.renderScales;
    final storedScale = stored['renderScale'];
    return GamePreferences._(
      Map.unmodifiable({
        ...switches,
        'bgmVolume': _volume(stored['bgmVolume']),
        'sfxVolume': _volume(stored['sfxVolume']),
        'language': languages.contains(savedLanguage)
            ? savedLanguage!
            : defaultLanguage,
        'music': capabilities.music.contains(savedMusic)
            ? savedMusic!
            : capabilities.music.first,
        'gameVersion': 'original',
        if (capabilities.music.contains('midi'))
          'externalMidiDeviceId': stored['externalMidiDeviceId'] is String
              ? stored['externalMidiDeviceId'] as String
              : '',
        if (multiplayer && game.id == 'th05')
          th05DisplayKey: _th05Display(stored[th05DisplayKey]),
        if (scales.isNotEmpty)
          'renderScale': scales.contains(storedScale)
              ? storedScale!
              : scales.first,
      }),
    );
  }

  final Map<String, Object> values;

  GamePreferences forLaunch({required bool externalMidiEnabled}) =>
      values.containsKey('externalMidiDeviceId')
      ? GamePreferences._(
          Map.unmodifiable({
            ...values,
            'externalMidiEnabled':
                externalMidiEnabled && values['music'] == 'midi',
          }),
        )
      : this;

  Map<String, Object>? get th05Display =>
      values[th05DisplayKey] as Map<String, Object>?;

  bool enabled(GameToggle toggle) => values[toggle.name] as bool? ?? false;

  static int _volume(Object? value) =>
      value is num && value.isFinite ? value.round().clamp(0, 100) : 100;
}

class GamePreferenceStore {
  GamePreferenceStore(this._storage, {ExternalMidi? midi})
    : midi = midi ?? ExternalMidi(),
      keyboard = KeyboardLayoutStore(_storage),
      gamepad = GamepadPreferenceStore(_storage),
      localDuel = LocalDuelStore(_storage),
      touch = TouchPreferenceStore(_storage);
  final SharedPreferencesAsync _storage;
  final ExternalMidi midi;
  final KeyboardLayoutStore keyboard;
  final GamepadPreferenceStore gamepad;
  final LocalDuelStore localDuel;
  final TouchPreferenceStore touch;
  final _writes = <String, Future<GamePreferences>>{};

  String _key(Game game, bool multiplayer) =>
      'game-options.v1.${game.id}.${multiplayer ? 'multiplayer' : 'singleplayer'}';

  Future<GamePreferences> load(Game game, {required bool multiplayer}) async {
    final pending = _writes[_key(game, multiplayer)];
    if (pending != null) {
      try {
        return await pending;
      } on Exception {
        // A failed transaction leaves the last persisted settings available.
      }
    }
    return _read(game, multiplayer);
  }

  Future<GamePreferences> _read(Game game, bool multiplayer) async {
    final json = await _storage.getString(_key(game, multiplayer));
    var saved = <String, dynamic>{};
    if (json != null) {
      try {
        final decoded = jsonDecode(json);
        if (decoded is Map<String, dynamic>) saved = decoded;
      } on FormatException {
        // Recover settings created by an older or interrupted install.
      }
    }
    return GamePreferences.resolve(
      game,
      multiplayer: multiplayer,
      stored: saved,
    );
  }

  Future<GamePreferences> save(
    Game game,
    String name,
    Object value, {
    required bool multiplayer,
  }) async {
    final key = _key(game, multiplayer);
    final read = load(game, multiplayer: multiplayer);
    final pending = read.then((previous) async {
      final next = GamePreferences.resolve(
        game,
        multiplayer: multiplayer,
        stored: {...previous.values, name: value},
      );
      await _storage.setString(key, jsonEncode(next.values));
      return next;
    });
    _writes[key] = pending;
    try {
      return await pending;
    } finally {
      if (identical(_writes[key], pending)) _writes.remove(key);
    }
  }
}
