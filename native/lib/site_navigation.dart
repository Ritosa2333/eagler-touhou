import 'dart:math' as math;

import 'package:material_ui/material_ui.dart';

import 'account.dart';
import 'catalog.dart';
import 'l10n/strings.dart';
import 'launcher_appearance.dart';
import 'launcher_language.dart';
import 'launcher_select.dart';
import 'site_documents.dart';

enum _SiteAction {
  firstUse(Icons.tips_and_updates_outlined, firstUsePath),
  faq(Icons.help_outline, faqPath),
  multiplayer(Icons.menu_book_outlined, multiplayerGuidePath),
  support(Icons.favorite_border),
  status(Icons.monitor_heart_outlined, uptimePath),
  theme(Icons.contrast_outlined),
  language(Icons.language),
  about(Icons.info_outline, aboutPath),
  github(Icons.code),
  terms(Icons.description_outlined, termsPath),
  privacy(Icons.verified_user_outlined, privacyPath);

  const _SiteAction(this.icon, [this.path]);

  String label(LauncherStrings strings) => switch (this) {
    firstUse => strings.firstUse,
    faq => strings.faq,
    multiplayer => strings.multiplayerGuide,
    support => strings.supportUs,
    status => strings.serviceStatus,
    theme => strings.themeSettings,
    language => strings.language,
    about => strings.about,
    github => 'GitHub',
    terms => strings.terms,
    privacy => strings.privacy,
  };
  final IconData icon;
  final String? path;
}

List<({String title, List<_SiteAction> entries})> _groups(
  LauncherStrings strings,
) => [
  (
    title: strings.help,
    entries: [_SiteAction.faq, _SiteAction.multiplayer, _SiteAction.firstUse],
  ),
  (title: strings.site, entries: [_SiteAction.support, _SiteAction.status]),
  (
    title: strings.settings,
    entries: [
      _SiteAction.theme,
      _SiteAction.language,
      _SiteAction.about,
      _SiteAction.terms,
      _SiteAction.privacy,
    ],
  ),
];

void _activate(
  BuildContext context,
  _SiteAction action,
  LauncherPreferences preferences, {
  VoidCallback? close,
}) {
  if (close != null) {
    FocusManager.instance.primaryFocus?.unfocus();
    close();
  }
  switch (action) {
    case _SiteAction.support:
      openSiteLink(context, '/assets/donation.webp');
    case _SiteAction.theme:
      showBackgroundSettings(context, preferences);
    case _SiteAction.language:
      showLauncherLanguage(context, preferences);
    case _SiteAction.github:
      openSiteLink(context, 'https://github.com/YomotsuHisami/eagler-touhou');
    default:
      openSiteLink(context, action.path!);
  }
}

class SiteDrawer extends StatefulWidget {
  const SiteDrawer({
    super.key,
    required this.session,
    required this.preferences,
  });

  final AccountSession session;
  final LauncherPreferences preferences;

  @override
  State<SiteDrawer> createState() => _SiteDrawerState();
}

class _SiteDrawerState extends State<SiteDrawer> {
  LocalHistoryEntry? _panelEntry;
  _SiteAction? _panel;

  void _openPanel(_SiteAction panel) {
    if (_panelEntry != null) return;
    final route = ModalRoute.of(context)!;
    final entry = LocalHistoryEntry(
      onRemove: () {
        if (!mounted || _panelEntry == null) return;
        setState(() {
          _panelEntry = null;
          _panel = null;
        });
      },
    );
    route.addLocalHistoryEntry(entry);
    setState(() {
      _panelEntry = entry;
      _panel = panel;
    });
  }

  @override
  void dispose() {
    final entry = _panelEntry;
    _panelEntry = null;
    entry?.remove();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    final theme = Theme.of(context);
    final strings = context.strings;
    void close() {
      _panelEntry?.remove();
      Scaffold.of(context).closeDrawer();
    }

    final panel = _panel;
    final background = theme.colorScheme.surfaceContainer;
    Widget groupFrame(Widget child) => Card.outlined(
      margin: EdgeInsets.zero,
      color: background,
      shape: RoundedRectangleBorder(
        borderRadius: BorderRadius.circular(10),
        side: BorderSide(color: theme.colorScheme.outlineVariant),
      ),
      clipBehavior: Clip.antiAlias,
      child: child,
    );
    Widget entry(_SiteAction action, {String? label}) => ListTile(
      minTileHeight: 53,
      minLeadingWidth: 16,
      horizontalTitleGap: 16,
      contentPadding: const EdgeInsets.symmetric(horizontal: 16),
      leading: Icon(action.icon, size: 16),
      title: Text(
        label ?? action.label(strings),
        style: theme.textTheme.bodyMedium?.copyWith(
          fontSize: 15,
          fontWeight: FontWeight.w500,
        ),
      ),
      trailing: const Icon(Icons.chevron_right, size: 14),
      onTap:
          panel == null &&
              (action == _SiteAction.language || action == _SiteAction.about)
          ? () => _openPanel(action)
          : () => _activate(context, action, widget.preferences, close: close),
    );
    Widget groupEntries(List<_SiteAction> actions) => groupFrame(
      Column(
        children: [
          for (final (index, action) in actions.indexed) ...[
            if (index > 0) const Divider(height: 1),
            entry(
              action,
              label: panel == _SiteAction.about && action == _SiteAction.about
                  ? strings.siteInformation
                  : null,
            ),
          ],
        ],
      ),
    );

    return Drawer(
      width: math.min(292, MediaQuery.sizeOf(context).width - 32),
      backgroundColor: background,
      shape: BorderDirectional(
        end: BorderSide(color: theme.colorScheme.outlineVariant),
      ),
      child: SafeArea(
        child: Column(
          children: [
            Padding(
              padding: EdgeInsetsDirectional.only(
                start: panel == null ? 16 : 8,
                end: 10,
              ),
              child: ConstrainedBox(
                constraints: const BoxConstraints(minHeight: 68),
                child: Row(
                  children: [
                    if (panel != null)
                      BackButton(onPressed: _panelEntry!.remove),
                    Expanded(
                      child: panel != null
                          ? Text(
                              panel.label(strings),
                              style: theme.textTheme.titleMedium?.copyWith(
                                fontSize: 16,
                                fontWeight: FontWeight.w500,
                              ),
                            )
                          : Align(
                              alignment: AlignmentDirectional.centerStart,
                              child: Image.asset(
                                theme.brightness == Brightness.dark
                                    ? 'assets/launcher-wordmark-dark.png'
                                    : 'assets/launcher-wordmark-light.png',
                                width: 112,
                                height: 32,
                                fit: BoxFit.contain,
                                semanticLabel: 'TouhouOnline',
                              ),
                            ),
                    ),
                    IconButton(
                      tooltip: strings.closeMenu,
                      icon: const Icon(Icons.close),
                      onPressed: close,
                    ),
                  ],
                ),
              ),
            ),
            Expanded(
              child: ListView(
                key: PageStorageKey(panel?.name ?? 'site drawer'),
                padding: const EdgeInsets.fromLTRB(16, 12, 16, 16),
                children: [
                  if (panel == _SiteAction.language)
                    groupFrame(
                      LauncherLanguageRow(preferences: widget.preferences),
                    )
                  else if (panel == _SiteAction.about)
                    groupEntries([
                      _SiteAction.about,
                      _SiteAction.github,
                      _SiteAction.terms,
                      _SiteAction.privacy,
                      _SiteAction.firstUse,
                    ])
                  else ...[
                    Padding(
                      padding: const EdgeInsets.symmetric(horizontal: 3),
                      child: ConstrainedBox(
                        constraints: const BoxConstraints(minHeight: 70),
                        child: Theme(
                          data: theme.copyWith(
                            listTileTheme: theme.listTileTheme.copyWith(
                              horizontalTitleGap: 10,
                              titleTextStyle: theme.textTheme.titleLarge
                                  ?.copyWith(
                                    fontSize: 20,
                                    fontWeight: FontWeight.w400,
                                  ),
                            ),
                            textButtonTheme: TextButtonThemeData(
                              style: theme.textButtonTheme.style?.copyWith(
                                padding: const WidgetStatePropertyAll(
                                  EdgeInsets.symmetric(horizontal: 4),
                                ),
                                minimumSize: const WidgetStatePropertyAll(
                                  Size(0, 44),
                                ),
                                textStyle: WidgetStatePropertyAll(
                                  theme.textTheme.bodyMedium?.copyWith(
                                    fontSize: 14,
                                  ),
                                ),
                              ),
                            ),
                          ),
                          child: AccountSummary(
                            session: widget.session,
                            beforeOpen: close,
                          ),
                        ),
                      ),
                    ),
                    for (final group in _groups(strings)) ...[
                      Padding(
                        padding: const EdgeInsets.only(top: 18, bottom: 6),
                        child: Text(
                          group.title,
                          style: theme.textTheme.labelMedium?.copyWith(
                            fontSize: 12,
                            fontWeight: FontWeight.w500,
                            color: theme.colorScheme.onSurfaceVariant,
                          ),
                        ),
                      ),
                      groupEntries(
                        group.entries
                            .where(
                              (action) =>
                                  action != _SiteAction.firstUse &&
                                  action != _SiteAction.terms &&
                                  action != _SiteAction.privacy,
                            )
                            .toList(),
                      ),
                    ],
                  ],
                ],
              ),
            ),
          ],
        ),
      ),
    );
  }
}

class DesktopSiteActions extends StatelessWidget {
  const DesktopSiteActions({
    super.key,
    required this.preferences,
    required this.onSettings,
  });

  final LauncherPreferences preferences;
  final VoidCallback onSettings;

  @override
  Widget build(BuildContext context) {
    final strings = context.strings;
    final theme = Theme.of(context);
    final colors = theme.colorScheme;
    final dark = theme.brightness == Brightness.dark;

    PopupMenuItem<VoidCallback> menuEntry(
      IconData icon,
      String label,
      VoidCallback activate,
    ) => PopupMenuItem(
      value: activate,
      height: 36,
      padding: const EdgeInsets.symmetric(horizontal: 12),
      child: Row(
        spacing: 10,
        children: [
          Icon(icon, size: 16, color: colors.onSurfaceVariant),
          Flexible(child: Text(label)),
        ],
      ),
    );

    return Row(
      mainAxisSize: MainAxisSize.min,
      children: [
        for (final (action, icon) in [
          (_SiteAction.faq, _SiteAction.faq.icon),
          (_SiteAction.support, _SiteAction.support.icon),
          (
            _SiteAction.theme,
            dark ? Icons.light_mode_outlined : Icons.dark_mode_outlined,
          ),
        ])
          IconButton(
            tooltip: action.label(strings),
            onPressed: () => _activate(context, action, preferences),
            icon: Transform.translate(
              offset: const Offset(0, 2),
              child: Icon(icon, size: 14),
            ),
          ),
        SizedBox.square(
          dimension: 32,
          child: PopupMenuButton<VoidCallback>(
            popUpAnimationStyle: LauncherControl.overlayMotion(context),
            tooltip: strings.siteMenu,
            padding: EdgeInsets.zero,
            icon: Transform.translate(
              offset: const Offset(0, 2),
              child: const Icon(Icons.menu, size: 14),
            ),
            position: PopupMenuPosition.under,
            offset: const Offset(0, 8),
            constraints: const BoxConstraints(minWidth: 220, maxWidth: 288),
            menuPadding: const EdgeInsets.symmetric(vertical: 4),
            color: colors.surfaceContainer,
            surfaceTintColor: Colors.transparent,
            elevation: 3,
            shape: RoundedRectangleBorder(
              borderRadius: BorderRadius.circular(8),
              side: BorderSide(color: colors.outlineVariant),
            ),
            onSelected: (activate) => activate(),
            itemBuilder: (_) => [
              menuEntry(
                Icons.settings_outlined,
                strings.launcherSettings,
                onSettings,
              ),
              const PopupMenuDivider(height: 9),
              for (final (index, group) in _groups(strings).indexed) ...[
                if (index > 0) const PopupMenuDivider(height: 9),
                for (final action in group.entries)
                  if (action != _SiteAction.theme)
                    menuEntry(
                      action.icon,
                      action.label(strings),
                      () => _activate(context, action, preferences),
                    ),
              ],
            ],
          ),
        ),
      ],
    );
  }
}
