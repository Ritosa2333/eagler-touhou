import 'package:material_ui/material_ui.dart';
import 'package:prism_ui/prism_ui.dart' as prism;
import 'package:wolt_modal_sheet/wolt_modal_sheet.dart';

import 'catalog.dart';
import 'l10n/strings.dart';
import 'launcher_background.dart';
import 'launcher_navigation.dart';
import 'launcher_select.dart';
import 'launcher_wallpaper.dart';

Future<void> showBackgroundSettings(
  BuildContext context,
  LauncherPreferences preferences, {
  bool includeTheme = true,
}) async {
  final reduced = MediaQuery.disableAnimationsOf(context);
  final duration = reduced ? Duration.zero : const Duration(milliseconds: 210);
  await WoltModalSheet.show<void>(
    context: context,
    useRootNavigator: true,
    modalBarrierColor: Colors.black.withValues(alpha: .12),
    modalTypeBuilder: (context) =>
        MediaQuery.sizeOf(context).width <= launcherCompactWidth
        ? WoltBottomSheetType(
            transitionDuration: duration,
            reverseTransitionDuration: duration,
          )
        : WoltDialogType(
            transitionDuration: duration,
            reverseTransitionDuration: duration,
          ),
    pageListBuilder: (context) {
      return [
        WoltModalSheetPage(
          isTopBarLayerAlwaysVisible: true,
          navBarHeight: 54,
          surfaceTintColor: Colors.transparent,
          topBarTitle: Builder(
            builder: (context) => Text(
              includeTheme
                  ? context.strings.themeSettings
                  : context.strings.backgroundSettings,
              style: Theme.of(context).textTheme.titleMedium
                  ?.copyWith(fontWeight: FontWeight.w600),
            ),
          ),
          trailingNavBarWidget: Builder(
            builder: (context) => IconButton(
              tooltip: context.strings.close,
              onPressed: () => Navigator.of(context).pop(),
              icon: const Icon(Icons.close),
            ),
          ),
          child: _BackgroundSettings(
            preferences: preferences,
            includeTheme: includeTheme,
          ),
        ),
      ];
    },
  );
}

String _modeLabel(LauncherStrings strings, BackgroundMode mode) =>
    switch (mode) {
      BackgroundMode.plain => strings.backgroundPlain,
      BackgroundMode.dots => strings.backgroundDots,
      BackgroundMode.grid => strings.backgroundGrid,
      BackgroundMode.seigaiha => strings.backgroundWaves,
      BackgroundMode.image => strings.backgroundCustom,
    };

String _layerLabel(LauncherStrings strings, BackgroundLayer layer) =>
    switch (layer) {
      BackgroundLayer.shade => strings.backgroundShade,
      BackgroundLayer.panel => strings.backgroundPanel,
      BackgroundLayer.navigation => strings.backgroundNavigation,
      BackgroundLayer.category => strings.backgroundCategory,
      BackgroundLayer.card => strings.backgroundCard,
    };

TextStyle? _settingsLabel(BuildContext context) =>
    Theme.of(context).textTheme.bodySmall
        ?.copyWith(fontWeight: FontWeight.w600);

class LauncherThemeBaseRow extends StatelessWidget {
  const LauncherThemeBaseRow({
    super.key,
    required this.preferences,
    this.onError,
  });

  final LauncherPreferences preferences;
  final VoidCallback? onError;

  @override
  Widget build(BuildContext context) => ListenableBuilder(
    listenable: preferences,
    builder: (context, _) {
      final strings = context.strings;
      return LayoutBuilder(
        builder: (context, constraints) {
          final largeText = MediaQuery.textScalerOf(context).scale(13) > 17;
          final modes = {
            ThemeMode.system: strings.systemTheme,
            ThemeMode.light: strings.lightTheme,
            ThemeMode.dark: strings.darkTheme,
          };
          Future<void> select(ThemeMode mode) async {
            try {
              await preferences.setTheme(mode);
            } catch (_) {
              onError?.call();
            }
          }

          final label = Text(
            strings.themeBase,
            key: const ValueKey('theme base label'),
            style: _settingsLabel(context),
          );
          final options = Semantics(
            label: largeText ? null : strings.themeBase,
            child: largeText
                ? LauncherSelect<ThemeMode>(
                    key: const ValueKey('theme base segments'),
                    label: strings.themeBase,
                    value: preferences.theme,
                    labels: modes,
                    expand: true,
                    onChanged: (mode) {
                      if (mode != null) select(mode);
                    },
                  )
                : prism.SegmentedControl<ThemeMode>(
                    key: const ValueKey('theme base segments'),
                    segments: [
                      for (final mode in modes.entries) (mode.key, mode.value),
                    ],
                    value: preferences.theme,
                    onChanged: select,
                  ),
          );
          if (constraints.maxWidth < 240 || largeText) {
            return Column(
              spacing: 8,
              crossAxisAlignment: CrossAxisAlignment.stretch,
              children: [label, options],
            );
          }
          return Row(
            spacing: 10,
            children: [
              Expanded(child: label),
              SizedBox(
                width: constraints.maxWidth >= 320 ? 224 : 188,
                child: options,
              ),
            ],
          );
        },
      );
    },
  );
}

class _BackgroundSettings extends StatefulWidget {
  const _BackgroundSettings({
    required this.preferences,
    required this.includeTheme,
  });
  final LauncherPreferences preferences;
  final bool includeTheme;

  @override
  State<_BackgroundSettings> createState() => _BackgroundSettingsState();
}

class _BackgroundSettingsState extends State<_BackgroundSettings> {
  bool _failed = false;

  Future<void> _update(Future<void> Function() action) async {
    if (_failed) setState(() => _failed = false);
    try {
      await action();
    } catch (_) {
      if (mounted) setState(() => _failed = true);
    }
  }

  @override
  Widget build(BuildContext context) {
    final preferences = widget.preferences;
    final wallpaper = preferences.background;
    return ListenableBuilder(
      listenable: wallpaper,
      builder: (context, _) {
        final strings = context.strings;
        final theme = Theme.of(context);
        final hasImage = wallpaper.image != null;
        return Padding(
          padding: const EdgeInsets.fromLTRB(20, 4, 20, 22),
          child: Column(
            crossAxisAlignment: CrossAxisAlignment.stretch,
            spacing: 12,
            children: [
              if (widget.includeTheme) ...[
                LauncherThemeBaseRow(
                  preferences: preferences,
                  onError: () => setState(() => _failed = true),
                ),
                const Divider(height: 1),
              ],
              Semantics(
                header: true,
                child: Text(
                  strings.backgroundPresets,
                  style: _settingsLabel(context),
                ),
              ),
              LayoutBuilder(
                builder: (context, constraints) {
                  final columns = switch (constraints.maxWidth) {
                    >= 400 when hasImage => 5,
                    >= 270 when !hasImage => 4,
                    >= 240 => 3,
                    _ => 2,
                  };
                  final width =
                      (constraints.maxWidth - 8 * (columns - 1)) / columns;
                  return Wrap(
                    spacing: 8,
                    runSpacing: 8,
                    children: [
                      for (final mode in BackgroundMode.values)
                        if (mode != BackgroundMode.image || hasImage)
                          SizedBox(
                            width: width,
                            child: _Preset(
                              mode: mode,
                              wallpaper: wallpaper,
                              onPress: wallpaper.busy
                                  ? null
                                  : () => _update(() => wallpaper.select(mode)),
                            ),
                          ),
                    ],
                  );
                },
              ),
              Wrap(
                spacing: 8,
                runSpacing: 4,
                children: [
                  prism.FilledButton.icon(
                    onPressed: wallpaper.busy
                        ? null
                        : () => _update(wallpaper.chooseImage),
                    icon: const Icon(Icons.image_outlined),
                    label: Text(
                      hasImage
                          ? strings.backgroundReplace
                          : strings.backgroundChoose,
                    ),
                  ),
                  if (hasImage)
                    prism.TextButton(
                      onPressed: wallpaper.busy
                          ? null
                          : () => _update(wallpaper.removeImage),
                      child: Text(strings.backgroundRemove),
                    ),
                ],
              ),
              Text(
                strings.backgroundLocalNote,
                style: theme.textTheme.bodySmall?.copyWith(
                  color: theme.colorScheme.onSurfaceVariant,
                ),
              ),
              if (_failed)
                Semantics(
                  liveRegion: true,
                  child: Text(
                    strings.backgroundError,
                    style: TextStyle(color: theme.colorScheme.error),
                  ),
                ),
              if (wallpaper.mode == BackgroundMode.image)
                ListTile(
                  contentPadding: EdgeInsets.zero,
                  title: Text(strings.backgroundFit),
                  trailing: LauncherSelect<bool>(
                    label: strings.backgroundFit,
                    value: wallpaper.cover,
                    labels: {
                      true: strings.backgroundCover,
                      false: strings.backgroundContain,
                    },
                    onChanged: wallpaper.busy
                        ? null
                        : (cover) => _update(() => wallpaper.setCover(cover!)),
                  ),
                ),
              if (wallpaper.mode != BackgroundMode.plain)
                for (final layer in BackgroundLayer.values)
                  Column(
                    key: ValueKey(layer),
                    crossAxisAlignment: CrossAxisAlignment.stretch,
                    children: [
                      Row(
                        mainAxisAlignment: MainAxisAlignment.spaceBetween,
                        spacing: 8,
                        children: [
                          Flexible(child: Text(_layerLabel(strings, layer))),
                          Text('${wallpaper.opacity(layer)}%'),
                        ],
                      ),
                      Semantics(
                        label: _layerLabel(strings, layer),
                        child: prism.Slider(
                          value: wallpaper.opacity(layer).toDouble(),
                          max: 100,
                          onChanged: wallpaper.busy
                              ? null
                              : (value) =>
                                    wallpaper.setOpacity(layer, value.round()),
                          onChangeEnd: (_) => _update(wallpaper.save),
                        ),
                      ),
                    ],
                  ),
            ],
          ),
        );
      },
    );
  }
}

class _Preset extends StatelessWidget {
  const _Preset({
    required this.mode,
    required this.wallpaper,
    required this.onPress,
  });

  final BackgroundMode mode;
  final LauncherBackground wallpaper;
  final VoidCallback? onPress;

  @override
  Widget build(BuildContext context) {
    final colors = Theme.of(context).colorScheme;
    final selected = mode == wallpaper.mode;
    return prism.PressSurface(
      key: ValueKey(('background preset', mode)),
      selected: selected,
      onPressed: onPress,
      shape: prism.AppShape.sm,
      fill: selected ? colors.primaryContainer : colors.surfaceContainerLow,
      child: Padding(
        padding: const EdgeInsets.all(5),
        child: Column(
          spacing: 4,
          crossAxisAlignment: CrossAxisAlignment.stretch,
          children: [
            ClipRRect(
              borderRadius: BorderRadius.circular(5),
              child: SizedBox(
                height: 38,
                child: LauncherWallpaperSample(
                  mode: mode,
                  background: wallpaper,
                ),
              ),
            ),
            Align(
              alignment: AlignmentDirectional.centerStart,
              child: Text(
                _modeLabel(context.strings, mode),
                style: Theme.of(context).textTheme.labelMedium,
              ),
            ),
          ],
        ),
      ),
    );
  }
}
