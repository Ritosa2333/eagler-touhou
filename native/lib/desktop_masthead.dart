import 'package:material_ui/material_ui.dart';
import 'package:prism_ui/prism_ui.dart' as prism;

import 'account.dart';
import 'catalog.dart';
import 'desktop_window.dart';
import 'l10n/strings.dart';
import 'launcher_background.dart';
import 'message_highlights.dart';
import 'messages.dart';
import 'site_navigation.dart';

const _siteName = '東方 Online';
const _mastheadHeight = 44.0;
const _controlSize = 32.0;

class DesktopMasthead extends StatelessWidget implements PreferredSizeWidget {
  const DesktopMasthead({
    super.key,
    required this.preferences,
    required this.highlights,
    required this.selectedIndex,
    required this.onSelect,
    required this.onLibrary,
    this.onOpenMessageSection,
  });

  final LauncherPreferences preferences;
  final MessageHighlights highlights;
  final int selectedIndex;
  final ValueChanged<int> onSelect;
  final VoidCallback onLibrary;
  final ValueChanged<MessageSection>? onOpenMessageSection;

  @override
  Size get preferredSize => const Size.fromHeight(_mastheadHeight);

  @override
  Widget build(BuildContext context) {
    final theme = Theme.of(context);
    final colors = theme.colorScheme;
    final strings = context.strings;
    final dark = theme.brightness == Brightness.dark;
    final controlStyle = ButtonStyle(
      foregroundColor: WidgetStatePropertyAll(colors.onSurfaceVariant),
      minimumSize: WidgetStatePropertyAll(Size.square(_controlSize)),
      padding: WidgetStatePropertyAll(EdgeInsets.zero),
      iconSize: WidgetStatePropertyAll(16),
      visualDensity: VisualDensity.standard,
      tapTargetSize: MaterialTapTargetSize.shrinkWrap,
    );
    return Material(
      color: LauncherBackdrop.tint(
        context,
        BackgroundLayer.navigation,
        colors.surfaceContainerLow,
      ),
      child: DecoratedBox(
        position: DecorationPosition.foreground,
        decoration: BoxDecoration(
          border: Border(bottom: BorderSide(color: colors.outlineVariant)),
        ),
        child: SafeArea(
          bottom: false,
          child: SizedBox(
            height: _mastheadHeight,
            child: LayoutBuilder(
              builder: (context, constraints) => Stack(
                fit: StackFit.expand,
                children: [
                  const DesktopDragArea(
                    child: ColoredBox(color: Colors.transparent),
                  ),
                  IconButtonTheme(
                    data: IconButtonThemeData(style: controlStyle),
                    child: Padding(
                      padding: EdgeInsets.symmetric(
                        horizontal: constraints.maxWidth < 900 ? 16 : 28,
                      ),
                      child: Row(
                        spacing: 12,
                        children: [
                          Expanded(
                            child: Row(
                              key: const ValueKey('masthead site'),
                              children: [
                                Flexible(
                                  child: Tooltip(
                                    message: _siteName,
                                    excludeFromSemantics: true,
                                    child: prism.PressSurface(
                                      onPressed: onLibrary,
                                      fill: Colors.transparent,
                                      shape: prism.AppShape.sm,
                                      child: SizedBox(
                                        width: 98,
                                        height: _controlSize,
                                        child: Semantics(
                                          label: _siteName,
                                          child: Image.asset(
                                            dark
                                                ? 'assets/launcher-wordmark-dark.png'
                                                : 'assets/launcher-wordmark-light.png',
                                            excludeFromSemantics: true,
                                          ),
                                        ),
                                      ),
                                    ),
                                  ),
                                ),
                                Padding(
                                  padding: const EdgeInsetsDirectional.only(
                                    start: 14,
                                    end: 6,
                                  ),
                                  child: Transform.translate(
                                    offset: const Offset(0, 2),
                                    child: ColoredBox(
                                      color: colors.outlineVariant,
                                      child: const SizedBox(
                                        width: 1,
                                        height: 11,
                                      ),
                                    ),
                                  ),
                                ),
                                DesktopSiteActions(
                                  preferences: preferences,
                                  onSettings: () => onSelect(2),
                                ),
                              ],
                            ),
                          ),
                          Row(
                            key: const ValueKey('masthead navigation'),
                            spacing: 2,
                            children: [
                              _MastheadTab(
                                label: strings.singlePlayer,
                                icon: Icons.sports_esports_outlined,
                                selected: selectedIndex == 0,
                                onPressed: () => onSelect(0),
                              ),
                              _MastheadTab(
                                label: strings.multiplayer,
                                icon: Icons.people_outline,
                                selected: selectedIndex == 1,
                                onPressed: () => onSelect(1),
                              ),
                            ],
                          ),
                          Expanded(
                            child: Padding(
                              padding: EdgeInsetsDirectional.only(
                                end: hasDesktopWindow
                                    ? 150 -
                                          (constraints.maxWidth < 900 ? 16 : 28)
                                    : 0,
                              ),
                              child: Row(
                                key: const ValueKey('masthead account'),
                                mainAxisAlignment: MainAxisAlignment.end,
                                children: [
                                  Flexible(
                                    child: DecoratedBox(
                                      decoration: ShapeDecoration(
                                        color: colors.surfaceContainerHigh,
                                        shape: prism.AppShape.sm,
                                      ),
                                      child: Padding(
                                        padding: const EdgeInsets.symmetric(
                                          horizontal: 4,
                                          vertical: 2,
                                        ),
                                        child: Row(
                                          mainAxisSize: MainAxisSize.min,
                                          spacing: 6,
                                          children: [
                                            MessageActions(
                                              highlights: highlights,
                                              showPreviews: true,
                                              onOpenSection:
                                                  onOpenMessageSection,
                                            ),
                                            Flexible(
                                              child: DesktopAccountButton(
                                                session: highlights.session,
                                              ),
                                            ),
                                          ],
                                        ),
                                      ),
                                    ),
                                  ),
                                ],
                              ),
                            ),
                          ),
                        ],
                      ),
                    ),
                  ),
                  if (hasDesktopWindow)
                    const PositionedDirectional(
                      top: 0,
                      end: 0,
                      child: DesktopWindowControls(),
                    ),
                ],
              ),
            ),
          ),
        ),
      ),
    );
  }
}

class _MastheadTab extends StatelessWidget {
  const _MastheadTab({
    required this.label,
    required this.icon,
    required this.selected,
    required this.onPressed,
  });

  final String label;
  final IconData icon;
  final bool selected;
  final VoidCallback onPressed;

  @override
  Widget build(BuildContext context) {
    final colors = prism.Theme.of(context).colorScheme;
    final foreground = selected ? colors.onSurface : colors.onSurfaceVariant;
    final showLabel =
        MediaQuery.textScalerOf(context).scale(14) <= 24 &&
        (!hasDesktopWindow || MediaQuery.sizeOf(context).width >= 900);
    return Tooltip(
      message: label,
      excludeFromSemantics: true,
      child: prism.PressSurface(
        selected: selected,
        onPressed: onPressed,
        shape: prism.AppShape.sm,
        fill: selected ? colors.surfaceContainerHighest : Colors.transparent,
        showsHover: !selected,
        child: Semantics(
          label: label,
          excludeSemantics: true,
          child: ConstrainedBox(
            constraints: const BoxConstraints(minWidth: 74, maxWidth: 138),
            child: SizedBox(
              height: _controlSize,
              child: Stack(
                alignment: Alignment.center,
                children: [
                  Padding(
                    padding: const EdgeInsets.symmetric(horizontal: 10),
                    child: Row(
                      mainAxisSize: MainAxisSize.min,
                      spacing: 7,
                      children: [
                        Icon(icon, size: 17, color: foreground),
                        if (showLabel)
                          Flexible(
                            child: Text(
                              label,
                              maxLines: 1,
                              overflow: TextOverflow.ellipsis,
                              style: TextStyle(
                                fontSize: 14,
                                fontWeight: FontWeight.w600,
                                color: foreground,
                              ),
                            ),
                          ),
                      ],
                    ),
                  ),
                  if (selected)
                    Positioned(
                      bottom: 0,
                      child: DecoratedBox(
                        decoration: BoxDecoration(
                          color: colors.primary,
                          borderRadius: BorderRadius.circular(1),
                        ),
                        child: const SizedBox(width: 22, height: 2),
                      ),
                    ),
                ],
              ),
            ),
          ),
        ),
      ),
    );
  }
}
