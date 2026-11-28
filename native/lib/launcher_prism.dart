import 'package:material_ui/material_ui.dart';
import 'package:prism_ui/prism_ui.dart' as prism;

import 'launcher_background.dart';
import 'launcher_navigation.dart';
import 'l10n/strings.dart' show LauncherStrings;

const launcherAccentSeed = Color(0xFFC9748B);
final _unfoldCurve = prism.SpringCurve(
  prism.UnfoldMotion.spring,
  seconds: .367,
);

({String family, List<String> fallback}) launcherFonts(
  TargetPlatform platform,
  Iterable<Locale> locales,
) => (
  family: prism.systemFontOf(platform).family,
  fallback: [
    ...{
      ...prism.appFontFamilyFallback(platform: platform, locales: locales),
      // Player names can use any supported script, independently of UI language.
      for (final locale in LauncherStrings.supportedLocales)
        ...prism.appFontFamilyFallback(platform: platform, locales: [locale]),
    },
  ],
);

TextStyle launcherSectionTitle(BuildContext context) =>
    Theme.of(context).textTheme.titleMedium!.copyWith(
      fontSize: 19,
      fontWeight: FontWeight.w700,
      letterSpacing: 0,
      height: 1.4,
      leadingDistribution: TextLeadingDistribution.even,
    );

final _palettes = <Brightness, prism.ColorScheme>{};

prism.ColorScheme _palette(Brightness brightness) =>
    _palettes.putIfAbsent(brightness, () {
      final dark = brightness == Brightness.dark;
      return prism
          .seededColorScheme(
            seedColor: launcherAccentSeed,
            brightness: brightness,
            style: prism.ColorStyle.soft,
          )
          .copyWith(
            surface: Color(dark ? 0xFF17181B : 0xFFF7F7F8),
            surfaceContainerLow: Color(dark ? 0xFF1B1C20 : 0xFFF2F2F4),
            surfaceContainer: Color(dark ? 0xFF212227 : 0xFFF7F7F8),
            surfaceContainerHigh: Color(dark ? 0xFF292A30 : 0xFFEEEEF1),
            surfaceContainerHighest: Color(dark ? 0xFF303138 : 0xFFEAEAF0),
            surfaceBright: Color(dark ? 0xFF303138 : 0xFFFFFFFF),
            onSurface: Color(dark ? 0xFFECECEF : 0xFF27272D),
            onSurfaceVariant: Color(dark ? 0xFFB0B0B9 : 0xFF66666F),
            outline: Color(dark ? 0xFFA8A9B4 : 0xFF8B8B9C),
            outlineVariant: Color(dark ? 0xFF37383F : 0xFFE1E1E6),
            primary: Color(dark ? 0xFFECAAB2 : 0xFFA93648),
            onPrimary: Color(dark ? 0xFF232428 : 0xFFFFFFFF),
          );
    });

ColorScheme launcherMaterialColors(Brightness brightness) {
  final colors = _palette(brightness);
  return ColorScheme.fromSeed(
    seedColor: launcherAccentSeed,
    brightness: brightness,
    surface: prism.SurfaceLevel.page.colorOf(colors),
    surfaceContainerLow: colors.surfaceContainerLow,
    surfaceContainer: colors.surfaceContainer,
    surfaceContainerHigh: colors.surfaceContainerHigh,
    surfaceContainerHighest: colors.surfaceContainerHighest,
    onSurface: colors.onSurface,
    onSurfaceVariant: colors.onSurfaceVariant,
    outline: colors.outline,
    outlineVariant: colors.outlineVariant,
    primary: colors.primary,
    onPrimary: colors.onPrimary,
  );
}

/// Applies the configured wallpaper panel opacity to a Prism workspace.
class LauncherPrismPanel extends StatelessWidget {
  const LauncherPrismPanel({super.key, required this.child});
  final Widget child;

  @override
  Widget build(BuildContext context) {
    if (!LauncherBackdrop.active(context)) return child;
    final theme = prism.Theme.of(context);
    final colors = theme.colorScheme;
    Color panel(Color fill) =>
        LauncherBackdrop.tint(context, BackgroundLayer.panel, fill);
    return prism.Theme(
      data: theme.copyWith(
        colorScheme: colors.copyWith(
          surfaceContainerLow: panel(colors.surfaceContainerLow),
          surfaceContainer: panel(colors.surfaceContainer),
          surfaceContainerHigh: panel(colors.surfaceContainerHigh),
          surfaceContainerHighest: panel(colors.surfaceContainerHighest),
        ),
      ),
      child: child,
    );
  }
}

/// Provides Prism components with the launcher palette and viewport metrics.
class LauncherPrismScope extends StatelessWidget {
  const LauncherPrismScope({super.key, required this.child});

  final Widget child;

  @override
  Widget build(BuildContext context) {
    final theme = Theme.of(context);
    final size = MediaQuery.sizeOf(context);
    final mobile = size.width <= launcherCompactWidth;
    final fonts = launcherFonts(theme.platform, [
      Localizations.localeOf(context),
      ...WidgetsBinding.instance.platformDispatcher.locales,
    ]);
    final inheritedIcons = IconTheme.of(context);
    return ExpansionTileTheme(
      data: ExpansionTileTheme.of(context).copyWith(
        expansionAnimationStyle: MediaQuery.disableAnimationsOf(context)
            ? AnimationStyle.noAnimation
            : AnimationStyle(
                duration: const Duration(milliseconds: 367),
                curve: _unfoldCurve,
                reverseCurve: FlippedCurve(_unfoldCurve),
              ),
      ),
      child: prism.Theme(
        data: prism.ThemeData(
          colorScheme: _palette(theme.brightness),
          platform: theme.platform,
          fontFamily: fonts.family,
          fontFamilyFallback: fonts.fallback,
          tapTargetSize: mobile ? prism.TapTargetSize.padded : null,
        ),
        child: IconTheme(
          data: inheritedIcons,
          child: prism.ViewScope(
            size: size,
            isMobile: mobile,
            child: Builder(
              builder: (context) => prism.MeasureScope(
                measure: prism.Measure.of(
                  context,
                  MediaQuery.textScalerOf(context).scale(1),
                ),
                child: child,
              ),
            ),
          ),
        ),
      ),
    );
  }
}
