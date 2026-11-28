import 'dart:async';
import 'dart:io';

import 'package:intl/intl.dart';

import 'package:go_router/go_router.dart';
import 'package:material_ui/material_ui.dart';

import 'account_api.dart';
import 'l10n/strings.dart';
import 'site_documents.dart';
import 'profile_data.dart';
import 'profile_identity.dart';

enum AccountRestoreStatus { idle, checking, failed }

class AccountSession extends ChangeNotifier {
  AccountSession(this.api);

  final AccountApi api;
  final socialRevision = ValueNotifier(0);
  final cloudSaveRevision = ValueNotifier(0);
  AccountIdentity? _identity;
  AccountRestoreStatus _restoreStatus = AccountRestoreStatus.idle;
  Future<void>? _restoring;
  bool _disposed = false;
  int _webSessionGeneration = 0;
  Timer? _expiry;
  AccountIdentity? get identity => _identity;
  AccountRestoreStatus get restoreStatus => _restoreStatus;

  Future<void> restore() {
    if (!api.canRestoreSession || _identity != null || _disposed) {
      return Future<void>.value();
    }
    return _restoring ??= _readSavedSession();
  }

  Future<void> _readSavedSession() async {
    final generation = _webSessionGeneration;
    _restoreStatus = AccountRestoreStatus.checking;
    notifyListeners();
    try {
      final user = await api.restoreSession();
      if (_disposed || generation != _webSessionGeneration) return;
      _restoreStatus = AccountRestoreStatus.idle;
      if (user != null) {
        _adopt(user);
      } else {
        notifyListeners();
      }
    } catch (_) {
      if (!_disposed && generation == _webSessionGeneration) {
        _restoreStatus = AccountRestoreStatus.failed;
        notifyListeners();
      }
    } finally {
      _restoring = null;
    }
  }

  Future<void> refreshIdentity() async {
    final previous = _identity;
    if (previous == null) return;
    final updated = await api.session();
    if (!identical(_identity, previous)) return;
    if (updated == null || updated.username != previous.username) {
      expireIfUnauthorized(
        const AccountRequestException(401, 'SIGN_IN_REQUIRED'),
      );
    } else {
      _adopt(updated);
    }
  }

  Future<void> syncWebSession(Uri source, Cookie? cookie) async {
    if (_disposed) return;
    final generation = ++_webSessionGeneration;
    final previous = _identity;
    final known = api.webSessionCookie;
    final sameSession =
        known?.name == cookie?.name && known?.value == cookie?.value;
    _expiry?.cancel();
    if (!sameSession) _identity = null;
    _restoreStatus = AccountRestoreStatus.checking;
    notifyListeners();
    try {
      final user = await api.acceptWebSession(source, cookie);
      if (_disposed || generation != _webSessionGeneration) return;
      if (user != null) {
        _adopt(user);
        api.invalidateProfile(user.username);
      } else {
        _identity = null;
        _restoreStatus = AccountRestoreStatus.idle;
        notifyListeners();
      }
      if (user != null || previous?.username != _identity?.username) {
        socialRevision.value++;
        cloudSaveRevision.value++;
      }
    } catch (_) {
      if (!_disposed && generation == _webSessionGeneration) {
        _identity = null;
        _restoreStatus = AccountRestoreStatus.failed;
        notifyListeners();
      }
      rethrow;
    }
  }

  Future<void> signIn(String identity, String password) async {
    await _restoring;
    if (_disposed) return;
    final user = await api.signIn(identity, password);
    if (!_disposed) _adopt(user);
  }

  void _adopt(AccountIdentity identity) {
    _restoreStatus = AccountRestoreStatus.idle;
    _identity = identity;
    _expiry?.cancel();
    if (_identity?.expiresAt case final DateTime expires) {
      _expiry = Timer(expires.difference(DateTime.now()), () {
        _identity = null;
        notifyListeners();
      });
    }
    notifyListeners();
  }

  void expireIfUnauthorized(Object error) {
    if (error is AccountRequestException &&
        error.status == 401 &&
        _identity != null) {
      _expiry?.cancel();
      _identity = null;
      notifyListeners();
    }
  }

  Future<void> signOut() async {
    _webSessionGeneration++;
    await _restoring;
    if (_disposed) return;
    await api.signOut();
    if (_disposed) return;
    _expiry?.cancel();
    _restoreStatus = AccountRestoreStatus.idle;
    _identity = null;
    notifyListeners();
  }

  @override
  void dispose() {
    _disposed = true;
    _identity = null;
    _expiry?.cancel();
    socialRevision.dispose();
    cloudSaveRevision.dispose();
    api.close();
    super.dispose();
  }
}

class AccountAvatar extends StatelessWidget {
  const AccountAvatar({
    super.key,
    required this.api,
    required this.username,
    this.address,
    this.size = 40,
  });

  final AccountApi api;
  final String? username;
  final Uri? address;
  final double size;

  @override
  Widget build(BuildContext context) {
    final fallback = CircleAvatar(
      radius: size / 2,
      backgroundColor: Theme.of(context).colorScheme.surfaceContainerHighest,
      foregroundColor: Theme.of(context).colorScheme.onSurfaceVariant,
      child: Icon(Icons.person_outline, size: size * .55),
    );
    Widget photo(Uri url) => ClipOval(
      child: Image(
        image: switch (url.data) {
          final UriData embedded => MemoryImage(embedded.contentAsBytes()),
          _ => NetworkImage(url.toString()),
        },
        width: size,
        height: size,
        fit: BoxFit.cover,
        frameBuilder: (_, child, frame, _) => frame == null ? fallback : child,
        errorBuilder: (_, _, _) => fallback,
      ),
    );
    if (address case final Uri url) return photo(url);
    final name = username;
    if (name == null) return fallback;
    return StreamBuilder<ProfileChange>(
      stream: api.profileChanges.where(
        (change) => change.username == name.toLowerCase(),
      ),
      builder: (_, _) => FutureBuilder<Uri?>(
        key: ValueKey((api, name.toLowerCase())),
        future: api.avatar(name),
        builder: (context, snapshot) => switch (snapshot.data) {
          final Uri url => photo(url),
          null => fallback,
        },
      ),
    );
  }
}

class AccountSummary extends StatelessWidget {
  const AccountSummary({
    super.key,
    required this.session,
    this.onProfile,
    this.beforeOpen,
  });

  final AccountSession session;
  final VoidCallback? onProfile;
  final VoidCallback? beforeOpen;

  void _open(BuildContext context, String path) {
    beforeOpen?.call();
    unawaited(openSiteLink(context, path));
  }

  @override
  Widget build(BuildContext context) => ListenableBuilder(
    listenable: session,
    builder: (context, _) {
      final member = session.identity;
      if (member == null) {
        final strings = context.strings;
        return Row(
          spacing: 8,
          children: [
            AccountAvatar(api: session.api, username: null, size: 54),
            Expanded(
              child: session.restoreStatus == AccountRestoreStatus.checking
                  ? Text(strings.accountChecking)
                  : Wrap(
                      crossAxisAlignment: WrapCrossAlignment.center,
                      children: [
                        TextButton(
                          onPressed: () => _open(context, signInPath),
                          child: Text(strings.signIn),
                        ),
                        const ExcludeSemantics(child: Text('/')),
                        TextButton(
                          onPressed: () => _open(context, registerPath),
                          child: Text(strings.register),
                        ),
                        if (session.restoreStatus ==
                            AccountRestoreStatus.failed)
                          IconButton(
                            tooltip: strings.accountRestoreRetry,
                            icon: const Icon(Icons.refresh, size: 18),
                            onPressed: session.restore,
                          ),
                      ],
                    ),
            ),
          ],
        );
      }
      return StreamBuilder<ProfileChange>(
        stream: session.api.profileChanges.where(
          (change) => change.username == member.username.toLowerCase(),
        ),
        builder: (_, _) => FutureBuilder<PublicProfile>(
          key: ValueKey(member.username),
          future: session.api.profile(member.username),
          builder: (context, snapshot) {
            final profile = snapshot.data;
            return ListTile(
              contentPadding: EdgeInsets.zero,
              leading: AccountAvatar(
                api: session.api,
                username: member.username,
                address: profile?.avatar,
                size: 54,
              ),
              title: Text(
                profile?.nickname ?? member.name,
                maxLines: 1,
                overflow: TextOverflow.ellipsis,
              ),
              subtitle: IdentityDetails(
                username: member.username,
                region: profile?.region ?? profile?.country,
              ),
              trailing: Icon(
                Icons.chevron_right,
                semanticLabel: context.strings.viewProfile,
              ),
              onTap:
                  onProfile ??
                  () => _open(context, profilePath(member.username)),
            );
          },
        ),
      );
    },
  );
}

Future<void> signOutAccount(
  BuildContext context,
  AccountSession session,
) async {
  try {
    await session.signOut();
  } catch (_) {
    if (!context.mounted) return;
    ScaffoldMessenger.of(context)
        .showSnackBar(SnackBar(content: Text(context.strings.signOutFailed)));
  }
}

class DesktopAccountButton extends StatelessWidget {
  const DesktopAccountButton({super.key, required this.session});

  final AccountSession session;

  @override
  Widget build(BuildContext context) => ListenableBuilder(
    listenable: session,
    builder: (context, _) {
      final member = session.identity;
      final menuController = MenuController();
      return MenuAnchor(
        animated: !MediaQuery.disableAnimationsOf(context),
        controller: menuController,
        menuChildren: [
          if (session.restoreStatus == AccountRestoreStatus.failed) ...[
            MenuItemButton(
              leadingIcon: const Icon(Icons.refresh),
              onPressed: session.restore,
              child: Text(context.strings.accountRestoreRetry),
            ),
            MenuItemButton(
              leadingIcon: const Icon(Icons.login),
              onPressed: () => openSiteLink(context, signInPath),
              child: Text(context.strings.signIn),
            ),
          ],
          if (member != null) ...[
            SizedBox(
              width: 272,
              child: Padding(
                padding: const EdgeInsets.symmetric(horizontal: 12),
                child: AccountSummary(
                  session: session,
                  onProfile: () {
                    menuController.close();
                    openSiteLink(context, profilePath(member.username));
                  },
                ),
              ),
            ),
            MenuItemButton(
              leadingIcon: const Icon(Icons.home_outlined),
              onPressed: () =>
                  openSiteLink(context, profilePath(member.username)),
              child: Text(context.strings.profile),
            ),
            MenuItemButton(
              leadingIcon: const Icon(Icons.edit_outlined),
              onPressed: () =>
                  openSiteLink(context, profileEditPath(member.username)),
              child: Text(context.strings.editProfile),
            ),
            MenuItemButton(
              leadingIcon: const Icon(Icons.shield_outlined),
              onPressed: () => openSiteLink(
                context,
                '${profilePath(member.username)}#security',
              ),
              child: Text(context.strings.accountSettings),
            ),
            MenuItemButton(
              leadingIcon: const Icon(Icons.cloud_outlined),
              onPressed: () => context.push('/account/cloud-saves'),
              child: Text(context.strings.cloudSaves),
            ),
            const Divider(),
            MenuItemButton(
              leadingIcon: const Icon(Icons.logout),
              onPressed: () => signOutAccount(context, session),
              child: Text(context.strings.signOut),
            ),
          ],
        ],
        builder: (context, menu, _) {
          final avatar = AccountAvatar(
            api: session.api,
            username: member?.username,
            size: 28,
          );
          if (member == null) {
            if (session.restoreStatus == AccountRestoreStatus.checking) {
              return IconButton(
                tooltip: context.strings.accountChecking,
                onPressed: null,
                icon: const SizedBox.square(
                  dimension: 20,
                  child: CircularProgressIndicator(strokeWidth: 2),
                ),
              );
            }
            if (session.restoreStatus == AccountRestoreStatus.failed) {
              return IconButton(
                tooltip: context.strings.accountRestoreRetry,
                onPressed: () => menu.isOpen ? menu.close() : menu.open(),
                icon: avatar,
              );
            }
            return IconButton(
              tooltip: context.strings.signIn,
              icon: const Icon(Icons.person_outline, size: 20),
              onPressed: () => openSiteLink(context, signInPath),
            );
          }
          return Tooltip(
            message: context.strings.account,
            child: ConstrainedBox(
              constraints: const BoxConstraints(maxWidth: 144),
              child: TextButton(
                style: TextButton.styleFrom(
                  minimumSize: const Size(0, 32),
                  padding: const EdgeInsets.symmetric(horizontal: 4),
                  tapTargetSize: MaterialTapTargetSize.shrinkWrap,
                  visualDensity: VisualDensity.standard,
                  foregroundColor: Theme.of(context).colorScheme.onSurface,
                ),
                onPressed: () => menu.isOpen ? menu.close() : menu.open(),
                child: Row(
                  mainAxisSize: MainAxisSize.min,
                  spacing: 6,
                  children: [
                    avatar,
                    Flexible(
                      child: Text(
                        member.name.isEmpty ? member.username : member.name,
                        overflow: TextOverflow.ellipsis,
                        maxLines: 1,
                      ),
                    ),
                    const Icon(Icons.expand_more, size: 16),
                  ],
                ),
              ),
            ),
          );
        },
      );
    },
  );
}

/// Read-only launcher shortcuts. The corresponding lists are maintained on web.
class AccountRelationships extends StatefulWidget {
  const AccountRelationships({
    super.key,
    required this.session,
    required this.username,
  });
  final AccountSession session;
  final String username;
  @override
  State<AccountRelationships> createState() => _AccountRelationshipsState();
}

class _AccountRelationshipsState extends State<AccountRelationships> {
  late Future<ProfileSocial> _read;
  String? _viewer;
  void _refresh() {
    _viewer = widget.session.identity?.username.toLowerCase();
    final read = widget.session.api.social(
      widget.username,
      signedIn: _viewer != null,
    );
    read.ignore();
    setState(() {
      _read = read;
    });
  }

  void _sessionChanged() {
    if (_viewer != widget.session.identity?.username.toLowerCase()) _refresh();
  }

  @override
  void initState() {
    super.initState();
    widget.session.addListener(_sessionChanged);
    widget.session.socialRevision.addListener(_refresh);
    _refresh();
  }

  @override
  void dispose() {
    widget.session.removeListener(_sessionChanged);
    widget.session.socialRevision.removeListener(_refresh);
    super.dispose();
  }

  @override
  Widget build(BuildContext context) => FutureBuilder(
    future: _read,
    builder: (context, snapshot) {
      final result = snapshot.data;
      if (result == null && snapshot.connectionState != ConnectionState.done) {
        return const SizedBox(height: 64);
      }
      final strings = context.strings;
      final theme = Theme.of(context);
      final numbers = NumberFormat.decimalPattern(strings.localeName);
      final counts = [
        result?.followingCount,
        result?.followerCount,
        result?.friendCount,
      ];
      final visible =
          result != null &&
          result.visible &&
          result.followingCount != null &&
          result.followerCount != null;
      return Column(
        crossAxisAlignment: CrossAxisAlignment.stretch,
        spacing: 8,
        children: [
          if (snapshot.hasError)
            Align(
              alignment: AlignmentDirectional.centerStart,
              child: TextButton.icon(
                onPressed: _refresh,
                icon: const Icon(Icons.refresh),
                label: Text(strings.profileSocialRetry),
              ),
            ),
          if (visible)
            Row(
              children: [
                for (final (index, item) in ProfileList.values.indexed)
                  Expanded(
                    child: TextButton(
                      onPressed: () => openSiteLink(
                        context,
                        profileSocialPath(widget.username, item),
                      ),
                      child: Column(
                        spacing: 3,
                        children: [
                          Text(
                            counts[index] == null
                                ? '—'
                                : numbers.format(counts[index]),
                            style: theme.textTheme.titleMedium,
                          ),
                          Text(
                            switch (item) {
                              ProfileList.following => strings.profileFollowing,
                              ProfileList.followers => strings.profileFollowers,
                              ProfileList.friends => strings.friends,
                            },
                            textAlign: TextAlign.center,
                            style: theme.textTheme.bodySmall,
                          ),
                        ],
                      ),
                    ),
                  ),
              ],
            ),
        ],
      );
    },
  );
}
