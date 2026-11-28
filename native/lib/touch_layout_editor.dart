import 'dart:async';

import 'package:flutter/foundation.dart';
import 'package:flutter/gestures.dart';
import 'package:flutter/services.dart';
import 'package:material_ui/material_ui.dart';

import 'catalog.dart';
import 'l10n/strings.dart';
import 'launcher_select.dart';
import 'touch_layout_preview.dart';
import 'touch_preferences.dart';

class TouchLayoutEditor extends StatefulWidget {
  const TouchLayoutEditor({
    super.key,
    required this.game,
    required this.store,
    this.restrictsMovement = false,
    this.onMovementChanged,
  });

  final Game game;
  final TouchPreferenceStore store;
  final bool restrictsMovement;
  final VoidCallback? onMovementChanged;

  @override
  State<TouchLayoutEditor> createState() => _TouchLayoutEditorState();
}

class _TouchLayoutEditorState extends State<TouchLayoutEditor>
    with WidgetsBindingObserver {
  late Future<TouchConfiguration> _initial = widget.store.load(widget.game.id);
  late Orientation _orientation;
  Orientation? _screenOrientation;
  TouchConfiguration? _configuration;
  TouchOptions? _editedOptions;
  Map<Orientation, TouchProfile> _saved = {};
  final _draft = <Orientation, TouchProfile>{};
  TouchControl _selected = TouchControl.escape;
  bool _previewOnly = false;
  bool _adjustingViewport = false;
  bool _customSensitivity = false;
  double? _sensitivityDraft;
  bool _saving = false;
  bool _confirming = false;
  bool _requestingRotation = false;
  bool _ownsRotation = false;
  String? _feedback;

  bool get _fighting => widget.game.id == 'th075';
  bool get _dirty =>
      _draft.length != _saved.length ||
      _draft.entries.any((entry) => !entry.value.matches(_saved[entry.key]));

  TouchTemplate get _template => _configuration!.templates[_orientation]!;
  TouchProfile get _profile => _draft[_orientation] ?? _template.defaultProfile;

  @override
  void initState() {
    super.initState();
    WidgetsBinding.instance.addObserver(this);
  }

  @override
  void didChangeMetrics() => setState(() {});

  @override
  void didChangeDependencies() {
    super.didChangeDependencies();
    final screen = MediaQuery.orientationOf(context);
    if (_screenOrientation == screen) return;
    _screenOrientation = screen;
    _showOrientation(screen);
  }

  void _showOrientation(Orientation value) {
    _orientation = value;
    _adjustingViewport = false;
    _sensitivityDraft = null;
    _feedback = null;
  }

  Future<void> _rotateScreen(Orientation target) async {
    setState(() {
      _requestingRotation = true;
      _feedback = null;
    });
    _ownsRotation = true;
    try {
      await SystemChrome.setPreferredOrientations(
        target == Orientation.landscape
            ? const [
                DeviceOrientation.landscapeLeft,
                DeviceOrientation.landscapeRight,
              ]
            : const [DeviceOrientation.portraitUp],
      );
    } on Exception {
      if (mounted) {
        setState(() => _feedback = context.strings.touchEditorRotationFailed);
      }
    } finally {
      if (mounted) setState(() => _requestingRotation = false);
    }
  }

  @override
  void dispose() {
    WidgetsBinding.instance.removeObserver(this);
    if (_ownsRotation) {
      unawaited(
        SystemChrome.setPreferredOrientations([]).catchError(
          (Object error) => FlutterError.reportError(
            FlutterErrorDetails(exception: error, library: 'touch layout'),
          ),
        ),
      );
    }
    super.dispose();
  }

  void _edit(TouchProfile next) {
    if (_saving) return;
    setState(() {
      if (next.matches(_template.defaultProfile)) {
        _draft.remove(_orientation);
      } else {
        _draft[_orientation] = next;
      }
      _feedback = null;
    });
  }

  Future<void> _save() async {
    if (_saving || !_dirty) return;
    setState(() => _saving = true);
    try {
      await widget.store.saveLayouts(widget.game.id, _draft);
      if (!mounted) return;
      setState(() {
        _saved = {..._draft};
        _feedback = context.strings.touchEditorSaved;
      });
    } on Exception {
      if (mounted) {
        setState(() => _feedback = context.strings.touchEditorSaveFailed);
      }
    } finally {
      if (mounted) setState(() => _saving = false);
    }
  }

  Future<void> _changeOption(String name, Object value) async {
    if (_saving ||
        widget.restrictsMovement &&
            name == 'touchMovementMode' &&
            value == 'touch-unlimited') {
      return;
    }
    setState(() {
      _saving = true;
      _feedback = null;
    });
    try {
      final options = await widget.store.setOption(widget.game.id, name, value);
      if (!mounted) return;
      setState(() => _editedOptions = options);
      if (name == 'touchMovementMode') widget.onMovementChanged?.call();
    } on Exception {
      if (mounted) {
        setState(() => _feedback = context.strings.touchEditorOptionsFailed);
      }
    } finally {
      if (mounted) setState(() => _saving = false);
    }
  }

  Future<void> _exit() async {
    if (_saving || _confirming || ModalRoute.of(context)?.isCurrent == false) {
      return;
    }
    if (_adjustingViewport) {
      setState(() => _adjustingViewport = false);
      return;
    }
    if (_dirty) {
      _confirming = true;
      final discard = await showDialog<bool>(
        context: context,
        animationStyle: LauncherControl.overlayMotion(context),
        builder: (context) {
          final strings = context.strings;
          return AlertDialog(
            scrollable: true,
            title: Text(strings.touchEditorDiscardTitle),
            content: Text(strings.touchEditorDiscardBody),
            actions: [
              TextButton(
                onPressed: () => Navigator.of(context).pop(false),
                child: Text(strings.touchEditorContinue),
              ),
              FilledButton(
                onPressed: () => Navigator.of(context).pop(true),
                child: Text(strings.touchEditorDiscard),
              ),
            ],
          );
        },
      );
      _confirming = false;
      if (!mounted || discard != true) return;
    }
    if (mounted) Navigator.of(context).pop();
  }

  List<TouchControl> _visible(TouchOptions options) => [
    TouchControl.escape,
    if (_fighting || options.focus != 'two-finger') TouchControl.focus,
    TouchControl.fire,
    TouchControl.bomb,
    if (options.joystick) TouchControl.joystick,
    if (options.restart) TouchControl.restart,
    if (_fighting ||
        _configuration!.catalog.functionGames.contains(widget.game.id))
      TouchControl.function,
    if (options.thprac) TouchControl.thpracMenu,
  ];

  void _select(TouchControl control) {
    if (_saving) return;
    setState(() => _selected = control);
    final controls = _profile.controls;
    final ordered = controls.keys.toList()
      ..sort((a, b) => controls[a]!.priority.compareTo(controls[b]!.priority));
    if (ordered.last == control) return;
    ordered.remove(control);
    ordered.add(control);
    _edit(
      TouchProfile({
        for (final (position, key) in ordered.indexed)
          key: (
            position: controls[key]!.position,
            scale: controls[key]!.scale,
            priority: position.toDouble(),
          ),
      }, viewportX: _profile.viewportX),
    );
  }

  void _place(TouchControl control, {Offset? position, double? scale}) {
    final previous = _profile.controls[control]!;
    final point = position ?? previous.position;
    _edit(
      _profile.update(control, (
        position: Offset(point.dx.clamp(0, 1), point.dy.clamp(0, 1)),
        scale:
            scale?.clamp(
              _configuration!.catalog.minScale,
              _configuration!.catalog.maxScale,
            ) ??
            previous.scale,
        priority: previous.priority,
      )),
    );
  }

  @override
  Widget build(BuildContext context) {
    final strings = context.strings;
    final display = View.of(context).display;
    final displayWidth = display.size.shortestSide / display.devicePixelRatio;
    final canRotate =
        defaultTargetPlatform == TargetPlatform.android &&
        displayWidth > 0 &&
        displayWidth < 600;
    final compactTitle = canRotate && MediaQuery.sizeOf(context).width <= 360;
    final rotation = _screenOrientation == Orientation.portrait
        ? Orientation.landscape
        : Orientation.portrait;
    return PopScope(
      canPop: !_dirty && !_saving && !_adjustingViewport,
      onPopInvokedWithResult: (didPop, _) {
        if (!didPop) _exit();
      },
      child: Dialog.fullscreen(
        child: CallbackShortcuts(
          bindings: {const SingleActivator(LogicalKeyboardKey.escape): _exit},
          child: Focus(
            autofocus: true,
            child: Scaffold(
              appBar: AppBar(
                leading: BackButton(onPressed: _exit),
                titleSpacing: compactTitle ? 4 : null,
                title: Text(
                  strings.touchEditorTitle,
                  style: compactTitle ? const TextStyle(fontSize: 18) : null,
                  maxLines: 1,
                  overflow: TextOverflow.ellipsis,
                ),
                actions: [
                  if (canRotate)
                    IconButton(
                      tooltip: rotation == Orientation.landscape
                          ? strings.touchEditorRotateLandscape
                          : strings.touchEditorRotatePortrait,
                      onPressed: _saving || _requestingRotation
                          ? null
                          : () => _rotateScreen(rotation),
                      icon: const Icon(Icons.screen_rotation_outlined),
                    ),
                  IconButton(
                    tooltip: _previewOnly
                        ? strings.touchEditorExpand
                        : strings.touchEditorCollapse,
                    onPressed: _saving
                        ? null
                        : () => setState(() => _previewOnly = !_previewOnly),
                    icon: Icon(_previewOnly ? Icons.tune : Icons.fullscreen),
                  ),
                ],
              ),
              body: FutureBuilder<TouchConfiguration>(
                future: _initial,
                builder: (context, snapshot) {
                  if (snapshot.hasError) {
                    return Center(
                      child: OutlinedButton.icon(
                        onPressed: () => setState(
                          () => _initial = widget.store.load(widget.game.id),
                        ),
                        icon: const Icon(Icons.refresh),
                        label: Text(strings.touchEditorRead),
                      ),
                    );
                  }
                  if (snapshot.data case final configuration?) {
                    if (_configuration == null) {
                      _configuration = configuration;
                      _saved = configuration.layouts;
                      _draft.addAll(_saved);
                    }
                    return _editor(_editedOptions ?? configuration.options);
                  }
                  return const Center(child: CircularProgressIndicator());
                },
              ),
              bottomNavigationBar: SafeArea(
                minimum: const EdgeInsets.fromLTRB(12, 6, 12, 8),
                child: Column(
                  mainAxisSize: MainAxisSize.min,
                  crossAxisAlignment: CrossAxisAlignment.stretch,
                  children: [
                    if (_feedback case final status?)
                      Semantics(
                        liveRegion: true,
                        child: Text(status, textAlign: TextAlign.center),
                      ),
                    OverflowBar(
                      alignment: MainAxisAlignment.end,
                      overflowAlignment: OverflowBarAlignment.end,
                      spacing: 8,
                      children: [
                        TextButton(
                          onPressed: _saving ? null : _exit,
                          child: Text(
                            _adjustingViewport
                                ? strings.touchEditorFinishAdjust
                                : strings.touchEditorExit,
                          ),
                        ),
                        FilledButton(
                          onPressed: _saving || !_dirty ? null : _save,
                          child: Text(strings.touchEditorSave),
                        ),
                      ],
                    ),
                  ],
                ),
              ),
            ),
          ),
        ),
      ),
    );
  }

  Widget _editor(TouchOptions options) {
    final visible = _visible(options);
    final selected = visible.contains(_selected) ? _selected : visible.first;
    return SafeArea(
      bottom: false,
      child: LayoutBuilder(
        builder: (context, constraints) {
          final sideBySide =
              constraints.maxWidth >= 620 &&
              constraints.maxWidth > constraints.maxHeight &&
              constraints.maxHeight >= 205;
          final preview = _preview(
            options,
            visible,
            selected,
            inlineToolbar: sideBySide && constraints.maxHeight < 400,
          );
          const padding = EdgeInsets.symmetric(horizontal: 16, vertical: 8);
          if (_previewOnly) {
            if (constraints.maxHeight < 205) {
              return SingleChildScrollView(
                padding: padding,
                child: SizedBox(height: 190, child: preview),
              );
            }
            return Padding(padding: padding, child: preview);
          }
          final fields = _fields(options, visible, selected);
          if (sideBySide) {
            return Padding(
              padding: padding,
              child: Row(
                spacing: 24,
                children: [
                  Expanded(flex: 3, child: preview),
                  Expanded(
                    flex: 2,
                    child: SingleChildScrollView(
                      key: const PageStorageKey('touch-editor-fields'),
                      child: fields,
                    ),
                  ),
                ],
              ),
            );
          }
          return ListView(
            key: const PageStorageKey('touch-editor-scroll'),
            padding: padding,
            children: [
              SizedBox(
                height: (constraints.maxHeight * .76).clamp(185, 520),
                child: preview,
              ),
              const SizedBox(height: 16),
              fields,
            ],
          );
        },
      ),
    );
  }

  Widget _preview(
    TouchOptions options,
    List<TouchControl> visible,
    TouchControl selected, {
    required bool inlineToolbar,
  }) {
    final strings = context.strings;
    final adjust = _saving
        ? null
        : () => setState(() => _adjustingViewport = !_adjustingViewport);
    final resetViewport = IconButton.outlined(
      style: LauncherControl.style(context),
      tooltip: strings.touchEditorResetViewport,
      onPressed: _saving ? null : () => _edit(_profile.moveViewport(0)),
      icon: const Icon(Icons.center_focus_strong),
    );
    return Column(
      crossAxisAlignment: CrossAxisAlignment.stretch,
      children: [
        Row(
          spacing: 8,
          children: [
            Expanded(
              child: LauncherSegments<Orientation>(
                segments: [
                  ButtonSegment(
                    value: Orientation.portrait,
                    label: Text(
                      strings.touchEditorPortrait,
                      maxLines: 1,
                      overflow: TextOverflow.ellipsis,
                    ),
                  ),
                  ButtonSegment(
                    value: Orientation.landscape,
                    label: Text(
                      strings.touchEditorLandscape,
                      maxLines: 1,
                      overflow: TextOverflow.ellipsis,
                    ),
                  ),
                ],
                value: _orientation,
                onChanged: _saving
                    ? null
                    : (value) => setState(() => _showOrientation(value)),
              ),
            ),
            IconButton.outlined(
              style: LauncherControl.style(context),
              tooltip: strings.touchEditorRestore,
              onPressed: _saving ? null : () => _edit(_template.defaultProfile),
              icon: const Icon(Icons.restart_alt),
            ),
            if (inlineToolbar) ...[
              IconButton.outlined(
                style: LauncherControl.style(context),
                tooltip: _adjustingViewport
                    ? strings.touchEditorFinishAdjust
                    : strings.touchEditorAdjust,
                isSelected: _adjustingViewport,
                onPressed: adjust,
                icon: const Icon(Icons.open_with),
                selectedIcon: const Icon(Icons.check),
              ),
              resetViewport,
            ],
          ],
        ),
        if (!inlineToolbar) ...[
          const SizedBox(height: 6),
          Row(
            spacing: 8,
            children: [
              Expanded(
                child: TextButton(
                  onPressed: adjust,
                  child: Text(strings.touchEditorAdjust),
                ),
              ),
              resetViewport,
            ],
          ),
        ],
        const SizedBox(height: 8),
        Expanded(
          child: TouchLayoutPreview(
            key: ValueKey(_orientation),
            game: widget.game,
            template: _template,
            profile: _profile,
            controls: visible,
            selected: selected,
            opacity: options.opacity / 100,
            adjustingViewport: _adjustingViewport,
            sensitivity: options.joystick || _adjustingViewport
                ? null
                : (_sensitivityDraft ?? options.sensitivity) / 100,
            onSelect: _select,
            onMove: (control, delta) => _place(
              control,
              position: _profile.controls[control]!.position + delta,
            ),
            onResize: (control, delta) => _place(
              control,
              scale: _profile.controls[control]!.scale + delta,
            ),
            onViewport: (delta) =>
                _edit(_profile.moveViewport(_profile.viewportX + delta)),
          ),
        ),
      ],
    );
  }

  Widget _fields(
    TouchOptions options,
    List<TouchControl> visible,
    TouchControl selected,
  ) {
    final strings = context.strings;
    final placement = _profile.controls[selected]!;
    final sensitivity = options.sensitivity;
    final custom =
        _customSensitivity || !const [100, 150, 200].contains(sensitivity);
    final colors = Theme.of(context).colorScheme;
    final joystick = options.joystick;
    return Column(
      spacing: 10,
      crossAxisAlignment: CrossAxisAlignment.stretch,
      children: [
        _selectField(strings.touchEditorChoose, selected.name, {
          for (final control in visible)
            control.name: control.labelFor(widget.game.id, strings),
        }, (value) => _select(TouchControl.values.byName(value))),
        _TouchSlider(
          label: strings.touchEditorControlSize,
          value: placement.scale * 100,
          min: _configuration!.catalog.minScale * 100,
          max: _configuration!.catalog.maxScale * 100,
          divisions: 48,
          live: true,
          onChanged: _saving
              ? null
              : (value) => _place(selected, scale: value / 100),
        ),
        ExpansionTile(
          key: const PageStorageKey('touch-control-position'),
          title: Text(strings.touchEditorControlPosition),
          tilePadding: EdgeInsets.zero,
          children: [
            for (final (label, horizontal) in [
              (strings.touchEditorHorizontal, true),
              (strings.touchEditorVertical, false),
            ])
              _TouchSlider(
                label: label,
                value:
                    (horizontal
                        ? placement.position.dx
                        : placement.position.dy) *
                    100,
                min: 0,
                max: 100,
                divisions: 100,
                live: true,
                onChanged: _saving
                    ? null
                    : (value) => _place(
                        selected,
                        position: horizontal
                            ? Offset(value / 100, placement.position.dy)
                            : Offset(placement.position.dx, value / 100),
                      ),
              ),
          ],
        ),
        if (_adjustingViewport)
          _TouchSlider(
            label: strings.touchEditorViewportOffset,
            value: _profile.viewportX * 100,
            min: -50,
            max: 50,
            divisions: 100,
            live: true,
            onChanged: _saving
                ? null
                : (value) => _edit(_profile.moveViewport(value / 100)),
          ),
        const Divider(),
        _selectField(
          strings.touchEditorMove,
          options.movement,
          {
            if (!_fighting) ...{
              'touch': strings.touchEditorDrag,
              'touch-unlimited': strings.touchEditorUnlimited,
            },
            'joystick': strings.touchEditorJoystick,
            'joystick-free': strings.touchEditorFreeJoystick,
          },
          (value) => _changeOption('touchMovementMode', value),
          disabled: {if (widget.restrictsMovement) 'touch-unlimited'},
        ),
        if (widget.restrictsMovement && !_fighting)
          Text(
            strings.roomLimitMovement,
            style: Theme.of(context).textTheme.bodySmall
                ?.copyWith(color: colors.onSurfaceVariant),
          ),
        if (!_fighting && widget.game.id != 'th01')
          _selectField(
            strings.touchEditorFocusMethod,
            options.focus,
            {
              'hold-button': strings.touchEditorFocusHold,
              'toggle-button': strings.touchEditorFocusToggle,
              'two-finger': strings.touchEditorFocusTwo,
            },
            (value) => _changeOption('touchFocusMode', value),
            disabled: {if (joystick) 'two-finger'},
          ),
        if (!_fighting) ...[
          Text(
            strings.touchEditorSensitivity,
            style: TextStyle(
              color: joystick ? colors.onSurfaceVariant : colors.onSurface,
            ),
          ),
          Wrap(
            spacing: 8,
            runSpacing: 4,
            children: [
              for (final value in const [100, 150, 200])
                ChoiceChip(
                  label: Text('$value%'),
                  selected: !custom && sensitivity == value,
                  onSelected: joystick || _saving
                      ? null
                      : (_) {
                          setState(() => _customSensitivity = false);
                          _changeOption('touchSensitivity', value);
                        },
                ),
              ChoiceChip(
                label: Text(strings.touchEditorCustom),
                selected: custom,
                onSelected: joystick || _saving
                    ? null
                    : (_) => setState(() => _customSensitivity = true),
              ),
            ],
          ),
          if (custom)
            _TouchSlider(
              label: strings.touchEditorCustomSensitivity,
              value: sensitivity.toDouble(),
              min: 100,
              max: 300,
              divisions: 200,
              onPreview: (value) => setState(() => _sensitivityDraft = value),
              onChanged: joystick || _saving
                  ? null
                  : (value) => _changeOption('touchSensitivity', value.round()),
            ),
          if (!joystick)
            Text(
              strings.touchEditorSensitivityHint,
              style: Theme.of(context).textTheme.bodySmall
                  ?.copyWith(color: colors.onSurfaceVariant),
            ),
          _toggle(
            strings.touchEditorDoubleBomb,
            options.doubleTapBomb,
            'doubleTapBombEnabled',
          ),
        ],
        _toggle(
          'R',
          options.restart,
          'restartButtonEnabled',
          hint: strings.touchEditorRestartHint,
        ),
        _toggle(
          strings.touchEditorThprac,
          options.thprac,
          'thpracTouchControlsEnabled',
        ),
        _TouchSlider(
          label: strings.touchEditorOpacity,
          value: options.opacity.toDouble(),
          min: 20,
          max: 100,
          divisions: 16,
          onChanged: _saving
              ? null
              : (value) => _changeOption('touchControlOpacity', value.round()),
        ),
      ],
    );
  }

  Widget _selectField(
    String label,
    String value,
    Map<String, String> choices,
    ValueChanged<String> apply, {
    Set<String> disabled = const {},
  }) => Column(
    crossAxisAlignment: CrossAxisAlignment.stretch,
    spacing: 6,
    children: [
      Text(label, style: Theme.of(context).textTheme.bodySmall),
      LauncherSelect<String>(
        label: label,
        value: value,
        labels: choices,
        expand: true,
        disabledValues: disabled,
        onChanged: _saving
            ? null
            : (next) {
                if (next != null) apply(next);
              },
      ),
    ],
  );

  Widget _toggle(String label, bool selected, String name, {String? hint}) =>
      SwitchListTile(
        contentPadding: EdgeInsets.zero,
        title: Text(label),
        subtitle: hint == null ? null : Text(hint),
        value: selected,
        onChanged: _saving ? null : (value) => _changeOption(name, value),
      );
}

class _TouchSlider extends StatefulWidget {
  const _TouchSlider({
    required this.label,
    required this.value,
    required this.min,
    required this.max,
    required this.divisions,
    required this.onChanged,
    this.onPreview,
    this.live = false,
  });

  final String label;
  final double value;
  final double min;
  final double max;
  final int divisions;
  final ValueChanged<double>? onChanged;
  final ValueChanged<double?>? onPreview;
  final bool live;

  @override
  State<_TouchSlider> createState() => _TouchSliderState();
}

class _TouchSliderState extends State<_TouchSlider> {
  double? _preview;
  int? _pointer;
  bool _canceled = false;

  void _previewValue(double? value) {
    setState(() => _preview = value);
    widget.onPreview?.call(value);
  }

  @override
  Widget build(BuildContext context) {
    final value = _preview ?? widget.value;
    return Column(
      crossAxisAlignment: CrossAxisAlignment.stretch,
      children: [
        Row(
          children: [
            Expanded(child: Text(widget.label)),
            Text('${value.round()}%'),
          ],
        ),
        Semantics(
          label: widget.label,
          child: Listener(
            onPointerDown: (event) {
              if (_pointer != null || event.buttons != kPrimaryButton) return;
              _pointer = event.pointer;
              _canceled = false;
            },
            onPointerUp: (event) {
              if (_pointer == event.pointer) _pointer = null;
            },
            onPointerCancel: (event) {
              if (_pointer != event.pointer) return;
              _pointer = null;
              _canceled = true;
              _previewValue(null);
            },
            child: Slider(
              key: ValueKey(widget.label),
              value: value,
              min: widget.min,
              max: widget.max,
              divisions: widget.divisions,
              semanticFormatterCallback: (value) => '${value.round()}%',
              onChangeStart: widget.onChanged == null || widget.live
                  ? null
                  : (_) => _canceled = false,
              onChanged: widget.onChanged == null
                  ? null
                  : (next) {
                      if (widget.live) {
                        widget.onChanged!(next);
                      } else {
                        _previewValue(next);
                      }
                    },
              onChangeEnd: widget.onChanged == null || widget.live
                  ? null
                  : (next) {
                      // Material also ends the interaction when a scroll wins.
                      final canceled = _canceled || _pointer != null;
                      _canceled = false;
                      _previewValue(null);
                      if (!canceled && next != widget.value) {
                        widget.onChanged!(next);
                      }
                    },
            ),
          ),
        ),
      ],
    );
  }
}
