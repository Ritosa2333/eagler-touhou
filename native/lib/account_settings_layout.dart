import 'package:go_router/go_router.dart';
import 'package:lucide_icons_flutter/lucide_icons.dart';
import 'package:material_ui/material_ui.dart';

import 'l10n/strings.dart';
import 'site_documents.dart';
import 'launcher_navigation.dart';
import 'profile_identity.dart';

enum AccountSettingsSection {
  security,
  profile,
  games;

  String label(BuildContext context) => switch (this) {
    security => context.strings.accountSecurity,
    profile => context.strings.personalProfile,
    games => context.strings.gameSettings,
  };

  String path(String username) => switch (this) {
    security => '${profilePath(username)}#security',
    profile => profileEditPath(username),
    games => '/account/cloud-saves',
  };
}

class AccountSettingsLayout extends StatelessWidget {
  const AccountSettingsLayout({
    super.key,
    required this.section,
    required this.username,
    required this.title,
    required this.onBack,
    required this.child,
    this.backKey,
    this.onNavigate,
    this.busy = false,
    this.progress = false,
    this.mobileActions = const [],
    this.footer,
  });

  final AccountSettingsSection section;
  final String? username;
  final String title;
  final VoidCallback? onBack;
  final Key? backKey;
  final ValueChanged<String>? onNavigate;
  final bool busy;
  final bool progress;
  final List<Widget> mobileActions;
  final Widget child;
  final Widget? footer;

  static bool wide(BuildContext context) {
    final size = MediaQuery.sizeOf(context);
    return size.width > launcherCompactWidth &&
        (size.width > 900 || size.width >= size.height);
  }

  @override
  Widget build(BuildContext context) {
    final desktop = MediaQuery.sizeOf(context).width > launcherCompactWidth;
    final columns = wide(context);
    return Scaffold(
      appBar: desktop
          ? null
          : AppBar(
              toolbarHeight: 52,
              title: Text(title),
              leading: BackButton(
                key: backKey,
                onPressed: onBack ?? () {},
                color: onBack == null ? Theme.of(context).disabledColor : null,
              ),
              actions: mobileActions,
              bottom: PreferredSize(
                preferredSize: const Size.fromHeight(2),
                child: _progress(),
              ),
            ),
      body: SafeArea(
        bottom: footer == null || desktop,
        child: SingleChildScrollView(
          padding: EdgeInsets.fromLTRB(
            columns ? 32 : 20,
            columns ? 36 : 20,
            columns ? 32 : 20,
            48,
          ),
          child: Center(
            child: ConstrainedBox(
              constraints: const BoxConstraints(maxWidth: 1080),
              child: Column(
                crossAxisAlignment: CrossAxisAlignment.stretch,
                spacing: 12,
                children: [
                  if (desktop)
                    Align(
                      alignment: AlignmentDirectional.centerStart,
                      child: TextButton.icon(
                        key: backKey,
                        onPressed: onBack,
                        icon: const Icon(LucideIcons.arrowLeft, size: 16),
                        label: Text(
                          MaterialLocalizations.of(context).backButtonTooltip,
                        ),
                      ),
                    ),
                  Row(
                    crossAxisAlignment: CrossAxisAlignment.start,
                    children: [
                      Offstage(
                        offstage: !columns || username == null,
                        child: SizedBox(
                          width: 176,
                          child: _navigation(context),
                        ),
                      ),
                      SizedBox(width: columns && username != null ? 48 : 0),
                      Expanded(
                        child: Align(
                          alignment: AlignmentDirectional.topStart,
                          child: ConstrainedBox(
                            constraints: const BoxConstraints(maxWidth: 800),
                            child: Column(
                              crossAxisAlignment: CrossAxisAlignment.stretch,
                              children: [
                                if (desktop) ...[
                                  Semantics(
                                    header: true,
                                    namesRoute: true,
                                    child: Text(
                                      title,
                                      style: Theme.of(context)
                                          .textTheme
                                          .titleLarge
                                          ?.copyWith(
                                            fontSize: 20,
                                            height: 1.2,
                                            fontWeight: FontWeight.w600,
                                          ),
                                    ),
                                  ),
                                  const SizedBox(height: 20),
                                  const Divider(height: 1),
                                  _progress(),
                                ],
                                SizedBox(height: desktop ? 24 : 0),
                                child,
                                if (desktop && footer != null) ...[
                                  const SizedBox(height: 24),
                                  footer!,
                                ],
                              ],
                            ),
                          ),
                        ),
                      ),
                    ],
                  ),
                ],
              ),
            ),
          ),
        ),
      ),
      bottomNavigationBar: desktop ? null : footer,
    );
  }

  Widget _progress() => SizedBox(
    height: 2,
    child: progress ? const LinearProgressIndicator(minHeight: 2) : null,
  );

  Widget _navigation(BuildContext context) => Column(
    crossAxisAlignment: CrossAxisAlignment.stretch,
    spacing: 4,
    children: [
      Padding(
        padding: const EdgeInsets.only(left: 10, bottom: 12),
        child: Semantics(
          header: true,
          child: Text(
            context.strings.accountSettings,
            style: Theme.of(context).textTheme.titleMedium
                ?.copyWith(fontSize: 18, fontWeight: FontWeight.w600),
          ),
        ),
      ),
      for (final destination in AccountSettingsSection.values)
        ListTile(
          key: ValueKey('account-settings-${destination.name}'),
          dense: true,
          minTileHeight: 44,
          minLeadingWidth: 16,
          horizontalTitleGap: 10,
          contentPadding: const EdgeInsets.symmetric(horizontal: 10),
          shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(6)),
          selected: destination == section,
          selectedColor: Theme.of(context).colorScheme.onSurface,
          selectedTileColor: Theme.of(context)
              .colorScheme
              .surfaceContainerHighest,
          iconColor: Theme.of(context).colorScheme.onSurfaceVariant,
          leading: Icon(switch (destination) {
            AccountSettingsSection.security => LucideIcons.shieldCheck,
            AccountSettingsSection.profile => LucideIcons.userRound,
            AccountSettingsSection.games => LucideIcons.gamepad2,
          }, size: 16),
          title: Text(
            destination.label(context),
            style: const TextStyle(fontSize: 13),
          ),
          onTap: busy || destination == section || username == null
              ? null
              : () {
                  final path = destination.path(username!);
                  if (onNavigate case final navigate?) {
                    navigate(path);
                  } else {
                    if (destination == AccountSettingsSection.games) {
                      context.pushReplacement(path);
                    } else {
                      openSiteLink(context, path);
                    }
                  }
                },
        ),
    ],
  );
}
