import 'dart:async';
import 'dart:io';

import 'package:go_router/go_router.dart';
import 'package:lucide_icons_flutter/lucide_icons.dart';
import 'package:material_ui/material_ui.dart';

import 'catalog.dart';
import 'external_midi.dart';
import 'game_detail.dart';
import 'game_files_panel.dart';
import 'game_launcher.dart';
import 'game_input_dialog.dart';
import 'game_user_files.dart';
import 'game_preferences.dart';
import 'launcher_navigation.dart';
import 'launcher_select.dart';
import 'l10n/strings.dart';
import 'room_session.dart';
import 'touch_layout_editor.dart';

const _fieldPadding = EdgeInsetsDirectional.fromSTEB(48, 0, 16, 0);

class GameSettingsPage extends StatefulWidget {
  const GameSettingsPage({
    super.key,
    required this.game,
    required this.multiplayer,
    required this.store,
    required this.files,
    required this.launchService,
    required this.launcherPreferences,
    this.onBack,
    this.room,
  });

  final Game game;
  final bool multiplayer;
  final GamePreferenceStore store;
  final GameUserFiles files;
  final GameLaunchService launchService;
  final LauncherPreferences launcherPreferences;
  final VoidCallback? onBack;
  final RoomSession? room;

  @override
  State<GameSettingsPage> createState() => _GameSettingsPageState();
}

class _GameSettingsPageState extends State<GameSettingsPage> {
  late Future<GamePreferences> _initial = _load();
  GamePreferences? _updated;
  bool _saving = false;

  String get _parent =>
      gamePath(widget.game.id, multiplayer: widget.multiplayer);

  Future<GamePreferences> _load() =>
      widget.store.load(widget.game, multiplayer: widget.multiplayer);

  Future<void> _save(
    GamePreferences settings,
    String name,
    Object value,
  ) async {
    if (_saving || settings.values[name] == value) return;
    setState(() => _saving = true);
    try {
      final next = await widget.store.save(
        widget.game,
        name,
        value,
        multiplayer: widget.multiplayer,
      );
      if (name == 'music' && value != 'midi') {
        unawaited(widget.store.midi.setEnabled(false));
      }
      if (!mounted) return;
      setState(() => _updated = next);
      if (name == GameToggle.touchEnabled.name) {
        final room = widget.room;
        if (room != null) unawaited(room.refreshMovement());
      }
    } on Exception {
      if (!mounted) return;
      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(content: Text(context.strings.gameSaveSettingsFailed)),
      );
    } finally {
      if (mounted) setState(() => _saving = false);
    }
  }

  @override
  Widget build(BuildContext context) {
    final game = widget.game;
    return Column(
      children: [
        Center(
          child: ConstrainedBox(
            constraints: const BoxConstraints(maxWidth: 752),
            child: Padding(
              padding: const EdgeInsets.fromLTRB(10, 10, 16, 0),
              child: Row(
                spacing: 2,
                children: [
                  BackButton(
                    onPressed:
                        widget.onBack ??
                        () => closeLauncherPage(context, _parent),
                  ),
                  Expanded(
                    child: Semantics(
                      header: true,
                      label: context.strings.gameSettings,
                      child: GameHeading(
                        game: game,
                        coverSize: 52,
                        small: true,
                      ),
                    ),
                  ),
                ],
              ),
            ),
          ),
        ),
        Expanded(
          child: FutureBuilder<GamePreferences>(
            future: _initial,
            builder: (context, snapshot) {
              if (snapshot.hasError) {
                return Center(
                  child: OutlinedButton.icon(
                    onPressed: () => setState(() {
                      _initial = _load();
                    }),
                    icon: const Icon(Icons.refresh),
                    label: Text(context.strings.gameReadSettings),
                  ),
                );
              }
              final settings = _updated ?? snapshot.data;
              if (settings == null) {
                return const Center(child: CircularProgressIndicator());
              }
              return _groups(game, settings);
            },
          ),
        ),
      ],
    );
  }

  Future<GameLaunchStatus?> _playReplay(BuildContext dialog, File file) =>
      widget.launchService.start(
        widget.game,
        multiplayer: widget.multiplayer,
        isActive: () => dialog.mounted,
        replayFile: file,
      );

  Widget _groups(Game game, GamePreferences preferences) {
    final files = widget.files.workspace(
      game,
      multiplayer: widget.multiplayer,
      language: preferences.values['language'] as String,
    );
    final layout = game.settings;
    final strings = context.strings;
    final compact = MediaQuery.sizeOf(context).width <= launcherCompactWidth;
    final theme = Theme.of(context);
    final colors = theme.colorScheme;
    final entries = [
      (
        'display',
        strings.gameDisplaySettings,
        LucideIcons.monitor,
        [
          if (layout.highRefresh)
            _switch(
              preferences,
              GameToggle.frameLimit60Enabled,
              strings.gameHighRefresh,
              inverted: true,
              description: strings.gameHighRefreshHint,
            ),
          if (layout.renderScales.isNotEmpty)
            _choice<int>(
              preferences,
              name: 'renderScale',
              label: strings.gameRenderResolution,
              options: {
                for (final scale in layout.renderScales)
                  scale: '${640 * scale} × ${480 * scale}',
              },
            ),
          if (layout.languages.length > 1)
            _choice<String>(
              preferences,
              name: 'language',
              label: strings.gameLanguage,
              options: {
                for (final pack in layout.languages) pack.id: pack.title,
              },
              description: _languagePackSize(layout, preferences),
            ),
          _switch(
            preferences,
            GameToggle.pixelPerfectEnabled,
            strings.gamePixelPerfect,
          ),
          for (final (toggle, label) in [
            (GameToggle.focusHitboxEnabled, strings.gameFocusHitbox),
            (GameToggle.faithBarEnabled, strings.gameFaithBar),
            (GameToggle.th02RapidFire, strings.gameRapidFire),
            (GameToggle.th03RapidFire, strings.gameRapidFire),
            (GameToggle.th03FocusEnabled, strings.gameFocusMode),
            (GameToggle.th03FocusPointEnabled, strings.gameFocusHitbox),
            (GameToggle.th075NativeResolution, strings.gameLowOverhead),
          ])
            if (toggle.available(game, multiplayer: widget.multiplayer))
              _switch(preferences, toggle, label),
          if (preferences.th05Display case final display?)
            ..._th05Display(preferences, display),
        ],
      ),
      (
        'input',
        strings.gameInputSettings,
        LucideIcons.keyboard,
        [_inputEntries(game)],
      ),
      (
        'touch',
        strings.gameTouchSettings,
        LucideIcons.hand,
        [
          _switch(
            preferences,
            GameToggle.touchEnabled,
            strings.gameTouchEnabled,
          ),
          _switch(
            preferences,
            GameToggle.alwaysHitbox,
            strings.gameAlwaysHitbox,
          ),
          _switch(
            preferences,
            GameToggle.magnifierEnabled,
            strings.gameMagnifier,
          ),
          ListTile(
            title: Text(strings.gameTouchLayout),
            leading: const Icon(Icons.tune),
            trailing: const Icon(Icons.chevron_right),
            onTap: () => showDialog<void>(
              context: context,
              animationStyle: LauncherControl.overlayMotion(context),
              barrierDismissible: false,
              builder: (_) {
                final room = widget.room;
                if (room == null) {
                  return TouchLayoutEditor(
                    game: game,
                    store: widget.store.touch,
                  );
                }
                return ListenableBuilder(
                  listenable: room,
                  builder: (_, _) => TouchLayoutEditor(
                    game: game,
                    store: widget.store.touch,
                    restrictsMovement: room.restrictsLocalMovement,
                    onMovementChanged: () => unawaited(room.refreshMovement()),
                  ),
                );
              },
            ),
          ),
        ],
      ),
      (
        'volume',
        strings.gameVolumeSettings,
        LucideIcons.volume2,
        [
          _volume(preferences, 'bgmVolume', strings.gameMusicVolume),
          _volume(preferences, 'sfxVolume', strings.gameEffectsVolume),
          _VolumeField(
            label: strings.roomQuickChatVolume,
            value: widget.launcherPreferences.quickChatVolume,
            save: (value) async {
              try {
                await widget.launcherPreferences.setQuickChatVolume(value);
                if (mounted) setState(() {});
              } on Exception {
                if (mounted) {
                  ScaffoldMessenger.of(context).showSnackBar(
                    SnackBar(content: Text(strings.gameSaveSettingsFailed)),
                  );
                }
              }
            },
          ),
        ],
      ),
      if (files != null)
        (
          'files',
          strings.gameStorageSettings,
          LucideIcons.folderOpen,
          [
            GameFilesPanel(
              key: ValueKey((files.replay.namespace, files.language)),
              workspace: files,
              play: _playReplay,
              padding: _fieldPadding.add(const EdgeInsets.only(bottom: 8)),
            ),
            if (!widget.multiplayer)
              ListTile(
                leading: const Icon(Icons.cloud_outlined),
                title: Text(strings.cloudSaves),
                trailing: const Icon(Icons.chevron_right),
                onTap: () =>
                    context.push('/account/cloud-saves?game=${game.id}'),
              ),
          ],
        ),
      (
        'advanced',
        strings.gameAdvancedSettings,
        LucideIcons.slidersHorizontal,
        [
          ListTile(
            title: Text(strings.gameVersion),
            trailing: Text(strings.gameOriginal),
          ),
          if (layout.music.length > 1)
            _choice<String>(
              preferences,
              name: 'music',
              label: strings.gameMusic,
              options: {
                for (final mode in layout.music)
                  mode: switch (mode) {
                    'ogg-stream' => strings.gameMusicStream,
                    'ogg-full' => strings.gameMusicFull,
                    'ogg-akyu' => strings.gameMusicAkyu,
                    'midi' => 'MIDI',
                    _ => strings.gameMusicNone,
                  },
              },
            ),
          if (preferences.values['music'] == 'midi' &&
              layout.music.contains('midi'))
            ExternalMidiSettings(
              midi: widget.store.midi,
              selectedId: preferences.values['externalMidiDeviceId'] as String,
              onChanged: _saving
                  ? null
                  : (id) => _save(preferences, 'externalMidiDeviceId', id),
            ),
          if (GameToggle.thpracEnabled.available(
            game,
            multiplayer: widget.multiplayer,
          ))
            _switch(preferences, GameToggle.thpracEnabled, 'thprac'),
          if (widget.multiplayer)
            _switch(
              preferences,
              GameToggle.multiplayerLocalPlayerVisibility,
              strings.gameLocalVisibility,
            ),
        ],
      ),
    ];

    return ListView(
      key: PageStorageKey('settings.${game.id}.${widget.multiplayer}'),
      padding: EdgeInsets.symmetric(
        horizontal: compact ? 16 : 24,
        vertical: 16,
      ),
      children: [
        Align(
          alignment: Alignment.topCenter,
          child: ConstrainedBox(
            constraints: const BoxConstraints(maxWidth: 720),
            child: Card.outlined(
              key: const ValueKey('game settings groups'),
              margin: EdgeInsets.zero,
              elevation: 0,
              color: colors.surface,
              clipBehavior: Clip.antiAlias,
              child: ListTileTheme.merge(
                minLeadingWidth: 18,
                horizontalTitleGap: 12,
                child: Column(
                  crossAxisAlignment: CrossAxisAlignment.stretch,
                  children: [
                    for (final (index, (key, name, icon, fields))
                        in entries.indexed) ...[
                      if (index > 0) const Divider(height: 1),
                      ExpansionTile(
                        key: PageStorageKey(key),
                        splashColor: Colors.transparent,
                        initiallyExpanded: compact && index == 0,
                        tilePadding: const EdgeInsetsDirectional.fromSTEB(
                          18,
                          0,
                          16,
                          0,
                        ),
                        leading: Icon(icon, size: 18),
                        title: Text(name, style: theme.textTheme.titleSmall),
                        expansionAnimationStyle:
                            MediaQuery.disableAnimationsOf(context)
                            ? AnimationStyle.noAnimation
                            : null,
                        shape: const Border(),
                        collapsedShape: const Border(),
                        children: [
                          ListTileTheme.merge(
                            dense: true,
                            titleTextStyle: theme.textTheme.bodySmall,
                            subtitleTextStyle: theme.textTheme.bodySmall
                                ?.copyWith(color: colors.onSurfaceVariant),
                            contentPadding: _fieldPadding,
                            child: Column(
                              crossAxisAlignment: CrossAxisAlignment.stretch,
                              children: [
                                for (final (i, field) in fields.indexed) ...[
                                  if (i > 0)
                                    const Divider(
                                      height: 1,
                                      indent: 48,
                                      endIndent: 16,
                                    ),
                                  field,
                                ],
                              ],
                            ),
                          ),
                        ],
                      ),
                    ],
                  ],
                ),
              ),
            ),
          ),
        ),
      ],
    );
  }

  Widget _inputEntries(Game game) => Padding(
    padding: _fieldPadding,
    child: LayoutBuilder(
      builder: (fieldContext, constraints) {
        final strings = fieldContext.strings;
        final entries = [
          (
            GameInputDevice.keyboard,
            Icons.keyboard_outlined,
            strings.gameKeyboard,
          ),
          (
            GameInputDevice.gamepad,
            Icons.sports_esports_outlined,
            strings.gameGamepad,
          ),
        ];
        final style =
            ListTileTheme.of(fieldContext).titleTextStyle ??
            Theme.of(fieldContext).textTheme.bodySmall!;
        final halfWidth = (constraints.maxWidth - 16) / 2;
        const chromeWidth = 24.0 + 12 + 32 + 12;
        final inline =
            constraints.maxWidth > 480 &&
            entries.every(
              (item) =>
                  TextPainter.computeWidth(
                        text: TextSpan(text: item.$3, style: style),
                        textDirection: Directionality.of(fieldContext),
                        textScaler: MediaQuery.textScalerOf(fieldContext),
                        maxLines: 1,
                      ) +
                      chromeWidth <=
                  halfWidth,
            );
        Widget entry((GameInputDevice, IconData, String) item) {
          final (device, icon, label) = item;
          return ListTile(
            contentPadding: EdgeInsets.zero,
            leading: Icon(icon),
            title: Text(label),
            trailing: const Icon(Icons.chevron_right),
            onTap: () => showDialog<void>(
              context: context,
              animationStyle: LauncherControl.overlayMotion(context),
              barrierDismissible: false,
              builder: (_) => GameInputDialog(
                game: game,
                store: widget.store,
                device: device,
                allowLocalDuel: game.id == 'th09' && !widget.multiplayer,
              ),
            ),
          );
        }

        if (inline) {
          return Row(
            spacing: 16,
            children: [
              for (final item in entries) Expanded(child: entry(item)),
            ],
          );
        }
        return Column(
          children: [
            for (final (index, item) in entries.indexed) ...[
              if (index > 0) const Divider(height: 1),
              entry(item),
            ],
          ],
        );
      },
    ),
  );

  Widget _switch(
    GamePreferences preferences,
    GameToggle toggle,
    String label, {
    bool inverted = false,
    String? description,
  }) => SwitchListTile(
    key: ValueKey(toggle),
    title: Text(label),
    subtitle: description == null ? null : Text(description),
    value: preferences.enabled(toggle) != inverted,
    onChanged: _saving
        ? null
        : (value) => _save(preferences, toggle.name, value != inverted),
  );

  List<Widget> _th05Display(
    GamePreferences preferences,
    Map<String, Object> display,
  ) {
    final strings = context.strings;
    const key = GamePreferences.th05DisplayKey;
    void save(String field, Object value) =>
        _save(preferences, key, {...display, field: value});
    return [
      _select<String>(
        name: 'bossBar',
        label: strings.th05BossBar,
        value: display['bossBar']! as String,
        options: {
          'native': strings.th05BarNative,
          'eosd': strings.th05BarEosd,
          'ring': strings.th05BarRing,
        },
        onChanged: _saving ? null : (value) => save('bossBar', value),
      ),
      for (final (option, label) in [
        (Th05DisplayOption.phaseMarkers, strings.th05PhaseMarkers),
        (Th05DisplayOption.phaseStars, strings.th05PhaseStars),
        (Th05DisplayOption.phaseTimeout, strings.th05PhaseTimeout),
        (Th05DisplayOption.timeoutAsTime, strings.th05TimeoutAsTime),
        (Th05DisplayOption.numericDream, strings.th05NumericDream),
        (Th05DisplayOption.numericPower, strings.th05NumericPower),
      ])
        SwitchListTile(
          key: ValueKey(option),
          title: Text(label),
          value: display[option.name] == true,
          onChanged: _saving || !option.canChange(display)
              ? null
              : (value) => save(option.name, value),
        ),
    ];
  }

  Widget _choice<T extends Object>(
    GamePreferences preferences, {
    required String name,
    required String label,
    required Map<T, String> options,
    String? description,
  }) => _select<T>(
    name: name,
    label: label,
    value: preferences.values[name] as T,
    options: options,
    description: description,
    onChanged: _saving ? null : (value) => _save(preferences, name, value),
  );

  Widget _select<T extends Object>({
    required String name,
    required String label,
    required T value,
    required Map<T, String> options,
    required ValueChanged<T>? onChanged,
    String? description,
  }) => LauncherSettingRow(
    label: label,
    description: description,
    controlWidth: (MediaQuery.sizeOf(context).width * .42)
        .clamp(112, 152)
        .toDouble(),
    control: LauncherSelect<T>(
      key: ValueKey(name),
      value: value,
      label: label,
      labels: options,
      expand: true,
      onChanged: onChanged == null
          ? null
          : (value) {
              if (value != null) onChanged(value);
            },
    ),
  );

  Widget _volume(GamePreferences preferences, String name, String label) =>
      _VolumeField(
        key: ValueKey(name),
        label: label,
        value: preferences.values[name] as int,
        save: _saving ? null : (value) => _save(preferences, name, value),
      );

  String _languagePackSize(
    GameSettingsCatalog catalog,
    GamePreferences preferences,
  ) {
    final selected = catalog.languages.firstWhere(
      (pack) => pack.id == preferences.values['language'],
    );
    if (selected.bytes == 0) return context.strings.gameLanguageBuiltIn;
    return selected.bytes < 1024 * 1024
        ? '${(selected.bytes / 1024).toStringAsFixed(1)} KiB'
        : '${(selected.bytes / (1024 * 1024)).toStringAsFixed(2)} MiB';
  }
}

class _VolumeField extends StatefulWidget {
  const _VolumeField({
    super.key,
    required this.label,
    required this.value,
    required this.save,
  });

  final String label;
  final int value;
  final Future<void> Function(int)? save;

  @override
  State<_VolumeField> createState() => _VolumeFieldState();
}

class _VolumeFieldState extends State<_VolumeField> {
  int? _dragging;

  @override
  Widget build(BuildContext context) {
    final value = _dragging ?? widget.value;
    final slider = Semantics(
      label: widget.label,
      child: Slider(
        value: value.toDouble(),
        max: 100,
        divisions: 100,
        semanticFormatterCallback: (value) => '${value.round()}%',
        onChanged: widget.save == null
            ? null
            : (value) => setState(() => _dragging = value.round()),
        onChangeEnd: widget.save == null
            ? null
            : (value) async {
                await widget.save?.call(value.round());
                if (mounted) setState(() => _dragging = null);
              },
      ),
    );
    final amountStyle = DefaultTextStyle.of(context).style
        .copyWith(fontFeatures: const [FontFeature.tabularFigures()]);
    final amountWidth = TextPainter.computeWidth(
      text: TextSpan(text: '100%', style: amountStyle),
      textDirection: Directionality.of(context),
      textScaler: MediaQuery.textScalerOf(context),
      maxLines: 1,
    ).ceilToDouble();
    final amount = SizedBox(
      width: amountWidth < 44 ? 44 : amountWidth,
      child: Text(
        '$value%',
        textAlign: TextAlign.end,
        style: amountStyle,
        maxLines: 1,
        softWrap: false,
      ),
    );
    return Padding(
      padding: _fieldPadding.add(const EdgeInsets.symmetric(vertical: 8)),
      child: LayoutBuilder(
        builder: (_, constraints) {
          if (constraints.maxWidth >= 360) {
            return Row(
              children: [
                Expanded(child: Text(widget.label)),
                SizedBox(width: constraints.maxWidth * .48, child: slider),
                amount,
              ],
            );
          }
          final fontSize = amountStyle.fontSize ?? 14;
          if (MediaQuery.textScalerOf(context).scale(fontSize) >
              fontSize * 1.3) {
            return Column(
              crossAxisAlignment: CrossAxisAlignment.stretch,
              children: [
                Text(widget.label),
                Row(
                  spacing: 8,
                  children: [
                    Expanded(child: slider),
                    amount,
                  ],
                ),
              ],
            );
          }
          return Column(
            crossAxisAlignment: CrossAxisAlignment.stretch,
            children: [
              Row(
                spacing: 8,
                mainAxisAlignment: MainAxisAlignment.spaceBetween,
                children: [
                  Expanded(child: Text(widget.label)),
                  amount,
                ],
              ),
              slider,
            ],
          );
        },
      ),
    );
  }
}
