import 'dart:convert';

import 'package:flutter/foundation.dart';
import 'package:flutter/services.dart';
import 'package:flutter/widgets.dart' show Orientation;
import 'package:shared_preferences/shared_preferences.dart';

const _touchMovements = {
  'touch',
  'touch-unlimited',
  'joystick',
  'joystick-free',
};
const _touchFocusModes = {'hold-button', 'toggle-button', 'two-finger'};

class TouchOptions {
  TouchOptions.fromJson(Map<String, dynamic> json)
    : movement = _touchMovements.contains(json['touchMovementMode'])
          ? json['touchMovementMode'] as String
          : 'touch-unlimited',
      focus = _touchFocusModes.contains(json['touchFocusMode'])
          ? json['touchFocusMode'] as String
          : 'hold-button',
      sensitivity = _numeric(json['touchSensitivity'], 150, 100, 300),
      opacity = _numeric(json['touchControlOpacity'], 100, 20, 100, step: 5),
      doubleTapBomb = json['doubleTapBombEnabled'] == true,
      restart = json['restartButtonEnabled'] == true,
      thprac = json['thpracTouchControlsEnabled'] == true;

  final String movement;
  final String focus;
  final int sensitivity;
  final int opacity;
  final bool doubleTapBomb;
  final bool restart;
  final bool thprac;

  bool get joystick => movement.startsWith('joystick');

  TouchOptions forGame(String gameId) => TouchOptions.fromJson({
    ...toJson(),
    if (gameId == 'th075' && !joystick) 'touchMovementMode': 'joystick',
    if (gameId == 'th075' ||
        gameId == 'th01' ||
        joystick && focus == 'two-finger')
      'touchFocusMode': 'hold-button',
  });

  Map<String, Object> toJson() => {
    'touchMovementMode': movement,
    'touchFocusMode': focus,
    'touchSensitivity': sensitivity,
    'touchControlOpacity': opacity,
    'doubleTapBombEnabled': doubleTapBomb,
    'restartButtonEnabled': restart,
    'thpracTouchControlsEnabled': thprac,
  };

  static int _numeric(
    Object? value,
    int fallback,
    int min,
    int max, {
    int step = 1,
  }) => value is num && value.isFinite
      ? ((value / step).round() * step).clamp(min, max)
      : fallback;
}

enum TouchControl {
  focus,
  fire,
  bomb,
  joystick,
  escape,
  restart,
  function,
  thpracTab,
  thpracMenu,
}

typedef TouchPlacement = ({Offset position, double scale, double priority});

class TouchProfile {
  const TouchProfile(this.controls, {this.viewportX = 0});

  final Map<TouchControl, TouchPlacement> controls;
  final double viewportX;

  TouchProfile update(TouchControl control, TouchPlacement placement) =>
      TouchProfile({...controls, control: placement}, viewportX: viewportX);

  TouchProfile moveViewport(double x) =>
      TouchProfile(controls, viewportX: x.clamp(-.5, .5));

  bool matches(TouchProfile? other) =>
      other != null &&
      other.viewportX == viewportX &&
      mapEquals(other.controls, controls);

  Map<String, Object> toJson() => {
    'controls': controls.map(
      (control, placement) => MapEntry(control.name, {
        'x': placement.position.dx,
        'y': placement.position.dy,
        'scale': placement.scale,
        'priority': placement.priority,
      }),
    ),
    'viewport': {'x': viewportX},
  };
}

class TouchTemplate {
  const TouchTemplate(
    this.referenceSize,
    this.defaultProfile,
    this.controlSizes,
  );

  final Size referenceSize;
  final TouchProfile defaultProfile;
  final Map<TouchControl, Size> controlSizes;

  factory TouchTemplate.fromJson(Map<String, dynamic> json) {
    final controls = <TouchControl, TouchPlacement>{};
    final sizes = <TouchControl, Size>{};
    final records = json['controls'] as Map<String, dynamic>;
    for (final control in TouchControl.values) {
      final rect = records[control.name] as Map<String, dynamic>;
      controls[control] = (
        position: Offset(
          (rect['x'] as num).toDouble(),
          (rect['y'] as num).toDouble(),
        ),
        scale: (rect['scale'] as num).toDouble(),
        priority: (rect['priority'] as num).toDouble(),
      );
      sizes[control] = Size(
        (rect['width'] as num).toDouble(),
        (rect['height'] as num).toDouble(),
      );
    }
    return TouchTemplate(
      Size(
        (json['width'] as num).toDouble(),
        (json['height'] as num).toDouble(),
      ),
      TouchProfile(controls),
      sizes,
    );
  }
}

class TouchCatalog {
  TouchCatalog.fromJson(Map<String, dynamic> json)
    : minScale = (json['minScale'] as num).toDouble(),
      maxScale = (json['maxScale'] as num).toDouble(),
      functionGames = Set<String>.from(json['functionGames'] as List),
      _profiles = json['profiles'] as Map<String, dynamic>;

  final double minScale;
  final double maxScale;
  final Set<String> functionGames;
  final Map<String, dynamic> _profiles;

  Map<Orientation, TouchTemplate> templates(String gameId) {
    final scope = _profiles[gameId] ?? _profiles['shared'];
    return {
      for (final orientation in Orientation.values)
        orientation: TouchTemplate.fromJson(
          scope[orientation.name] as Map<String, dynamic>,
        ),
    };
  }
}

class TouchConfiguration {
  const TouchConfiguration({
    required this.catalog,
    required this.templates,
    required this.options,
    required this.layouts,
  });

  final TouchCatalog catalog;
  final Map<Orientation, TouchTemplate> templates;
  final TouchOptions options;
  final Map<Orientation, TouchProfile> layouts;

  TouchConfiguration withOptions(TouchOptions next) => TouchConfiguration(
    catalog: catalog,
    templates: templates,
    options: next,
    layouts: layouts,
  );
}

class TouchPreferenceStore {
  TouchPreferenceStore(this._storage);

  final SharedPreferencesAsync _storage;
  TouchCatalog? _catalog;
  final _optionWrites = <String, Future<TouchOptions>>{};

  String _layoutKey(String gameId) =>
      'touch-layout.v1.${const {'th01', 'th075'}.contains(gameId) ? gameId : 'shared'}';

  String _optionsKey(String gameId) =>
      'touch-options.v1.${gameId == 'th01' ? gameId : 'shared'}';

  Future<TouchConfiguration> load(String gameId) async {
    final catalog = _catalog ??= await _loadCatalog();
    final templates = catalog.templates(gameId);
    final json = await _read(_layoutKey(gameId));
    return TouchConfiguration(
      catalog: catalog,
      templates: templates,
      options: await loadOptions(gameId),
      layouts: {
        for (final orientation in Orientation.values)
          if (json[orientation.name] case final Map<String, dynamic> profile)
            orientation: _readProfile(
              templates[orientation]!,
              catalog,
              profile,
            ),
      },
    );
  }

  Future<TouchOptions> loadOptions(String gameId) async {
    final key = _optionsKey(gameId);
    if (_optionWrites[key] case final pending?) {
      try {
        return (await pending).forGame(gameId);
      } on Exception {
        // A failed write leaves the last saved options intact.
      }
    }
    return TouchOptions.fromJson(await _read(key)).forGame(gameId);
  }

  Future<TouchCatalog> _loadCatalog() async {
    final raw = await rootBundle.loadString(
      'assets/touch_layout_catalog.json',
      cache: false,
    );
    return TouchCatalog.fromJson(jsonDecode(raw) as Map<String, dynamic>);
  }

  TouchProfile _readProfile(
    TouchTemplate template,
    TouchCatalog catalog,
    Map<String, dynamic> json,
  ) {
    double number(Object? value, double fallback, double min, double max) =>
        value is num && value.isFinite
        ? value.toDouble().clamp(min, max)
        : fallback;
    final raw = json['controls'];
    final controls = {...template.defaultProfile.controls};
    if (raw is Map<String, dynamic>) {
      for (final control in TouchControl.values) {
        final entry = raw[control.name];
        if (entry is! Map<String, dynamic>) continue;
        final previous = controls[control]!;
        controls[control] = (
          position: Offset(
            number(entry['x'], previous.position.dx, 0, 1),
            number(entry['y'], previous.position.dy, 0, 1),
          ),
          scale: number(
            entry['scale'],
            previous.scale,
            catalog.minScale,
            catalog.maxScale,
          ),
          priority: number(entry['priority'], previous.priority, 0, 100),
        );
      }
    }
    final viewport = json['viewport'];
    return TouchProfile(
      controls,
      viewportX: viewport is Map<String, dynamic>
          ? number(viewport['x'], 0, -.5, .5)
          : 0,
    );
  }

  Future<TouchOptions> setOption(
    String gameId,
    String name,
    Object value,
  ) async {
    final key = _optionsKey(gameId);
    final pending = _optionWrites[key];
    final write = _writeOption(key, name, value, pending);
    _optionWrites[key] = write;
    try {
      return (await write).forGame(gameId);
    } finally {
      if (identical(_optionWrites[key], write)) _optionWrites.remove(key);
    }
  }

  Future<TouchOptions> _writeOption(
    String key,
    String name,
    Object value,
    Future<TouchOptions>? pending,
  ) async {
    if (pending != null) {
      try {
        await pending;
      } on Exception {
        // The remaining write can still use the saved value.
      }
    }
    final next = {...await _read(key), name: value};
    if (name == 'touchMovementMode' &&
        (value as String).startsWith('joystick') &&
        next['touchFocusMode'] == 'two-finger') {
      next['touchFocusMode'] = 'hold-button';
    }
    final normalized = TouchOptions.fromJson(next);
    await _storage.setString(key, jsonEncode(normalized.toJson()));
    return normalized;
  }

  Future<void> saveLayouts(
    String gameId,
    Map<Orientation, TouchProfile> layouts,
  ) => _storage.setString(
    _layoutKey(gameId),
    jsonEncode(
      layouts.map(
        (orientation, profile) => MapEntry(orientation.name, profile.toJson()),
      ),
    ),
  );

  Future<Map<String, dynamic>> _read(String key) async {
    final stored = await _storage.getString(key);
    if (stored != null) {
      try {
        final json = jsonDecode(stored);
        if (json is Map<String, dynamic>) return json;
      } on FormatException {
        // A malformed record falls back to the authored defaults.
      }
    }
    return {};
  }
}
