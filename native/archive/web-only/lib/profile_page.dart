import 'dart:async';

import 'package:go_router/go_router.dart';
import 'package:flutter/services.dart';
import 'package:intl/intl.dart';
import 'package:material_ui/material_ui.dart';
import 'package:skeletonizer/skeletonizer.dart';
import 'package:url_launcher/url_launcher.dart';

import 'account.dart';
import 'account_api.dart';
import 'catalog.dart';
import 'game_launcher.dart';
import 'l10n/strings.dart';
import 'launcher_background.dart';
import 'launcher_select.dart';
import 'profile_data.dart';
import 'profile_games.dart';
import 'profile_identity.dart';
import 'profile_social.dart';

class ProfilePage extends StatefulWidget {
  const ProfilePage({
    super.key,
    required this.session,
    required this.username,
    required this.games,
    required this.launcher,
    this.initialCharacter,
  });

  final AccountSession session;
  final String username;
  final List<Game> games;
  final GameLaunchService launcher;
  final int? initialCharacter;

  @override
  State<ProfilePage> createState() => _ProfilePageState();
}

class _ProfilePageState extends State<ProfilePage> {
  final _gamesKey = GlobalKey();
  late Future<PublicProfile> _profile = widget.session.api.profile(
    widget.username,
  );

  StreamSubscription<ProfileChange>? _profileUpdates;

  @override
  void initState() {
    super.initState();
    _profileUpdates = widget.session.api.profileChanges
        .where((change) => change.username == widget.username.toLowerCase())
        .listen((_) {
          final read = widget.session.api.profile(widget.username);
          read.ignore();
          if (mounted) {
            setState(() {
              _profile = read;
            });
          }
        });
  }

  @override
  void dispose() {
    _profileUpdates?.cancel();
    super.dispose();
  }

  Future<void> _refresh() async {
    final future = widget.session.api.refreshPublishedProfile(widget.username);
    setState(() {
      _profile = future;
    });
    try {
      await future;
      if (mounted) widget.session.socialRevision.value++;
    } catch (_) {
      // FutureBuilder displays the request result.
    }
  }

  Future<void> _open(Uri address) async {
    try {
      if (await launchUrl(address, mode: LaunchMode.externalApplication)) {
        return;
      }
    } catch (_) {
      // Browser launch failure is shown below.
    }
    if (mounted) {
      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(content: Text(context.strings.profileLinkFailed)),
      );
    }
  }

  @override
  Widget build(BuildContext context) {
    return AnnotatedRegion<SystemUiOverlayStyle>(
      value: Theme.of(context).brightness == Brightness.dark
          ? SystemUiOverlayStyle.light
          : SystemUiOverlayStyle.dark,
      child: Semantics(
        namesRoute: true,
        container: true,
        explicitChildNodes: true,
        label: context.strings.profile,
        child: Scaffold(
          backgroundColor: LauncherBackdrop.page(context),
          body: SafeArea(
            child: FutureBuilder<PublicProfile>(
              future: _profile,
              builder: (context, snapshot) {
                if (snapshot.hasError) {
                  final error = snapshot.error;
                  final missing =
                      error is AccountRequestException && error.status == 404;
                  final message = Center(
                    child: missing
                        ? Text(context.strings.profileMissing)
                        : OutlinedButton.icon(
                            onPressed: _refresh,
                            icon: const Icon(Icons.refresh),
                            label: Text(context.strings.profileRetry),
                          ),
                  );
                  return Column(
                    children: [
                      SizedBox(height: 44, child: _profileNavigation(false)),
                      Expanded(child: message),
                    ],
                  );
                }
                final profile = snapshot.data;
                if (profile == null) {
                  return _profileSkeleton();
                }
                return ListenableBuilder(
                  listenable: widget.session,
                  builder: (context, _) => _overview(profile),
                );
              },
            ),
          ),
        ),
      ),
    );
  }

  Widget _overview(PublicProfile profile) {
    final session = widget.session;
    final owner =
        session.identity?.username.toLowerCase() ==
        profile.username.toLowerCase();
    final theme = Theme.of(context);
    final details = _about(profile);
    final showGames =
        profile.showCareer || profile.showRecords || profile.showVersus;
    final games = showGames
        ? CardTheme(
            data: LauncherBackdrop.active(context)
                ? theme.cardTheme.copyWith(
                    color: LauncherBackdrop.tint(
                      context,
                      BackgroundLayer.panel,
                      theme.colorScheme.surfaceContainerLow,
                    ),
                  )
                : theme.cardTheme,
            child: ProfileGameData(
              key: ValueKey((
                profile.username,
                owner,
                profile.showCareer,
                profile.showRecords,
                profile.showVersus,
                profile.defaultView,
              )),
              api: session.api,
              owner: owner,
              cloudRevision: owner ? session.cloudSaveRevision : null,
              profile: profile,
              games: widget.games,
              viewer: session.identity?.username,
              launcher: widget.launcher,
              initialCharacter: widget.initialCharacter,
            ),
          )
        : null;

    final gameSection = games == null
        ? null
        : KeyedSubtree(key: _gamesKey, child: games);
    return LayoutBuilder(
      builder: (context, constraints) {
        final wide = constraints.maxWidth > 780;
        return ProfileRelationships(
          key: ValueKey(profile.username),
          session: session,
          username: profile.username,
          layoutBuilder: (actions, counts) => RefreshIndicator(
            onRefresh: _refresh,
            child: CustomScrollView(
              key: PageStorageKey('profile:${profile.username}'),
              physics: const AlwaysScrollableScrollPhysics(),
              slivers: [
                SliverPadding(
                  padding: EdgeInsets.fromLTRB(
                    wide ? 24 : 0,
                    0,
                    wide ? 24 : 0,
                    24,
                  ),
                  sliver: SliverToBoxAdapter(
                    child: Align(
                      alignment: Alignment.topCenter,
                      child: ConstrainedBox(
                        constraints: const BoxConstraints(maxWidth: 1016),
                        child: Column(
                          crossAxisAlignment: CrossAxisAlignment.stretch,
                          spacing: wide ? 21 : 16,
                          children: [
                            _header(
                              profile,
                              wide,
                              actions: owner ? _ownerActions(wide) : actions,
                            ),
                            if (owner && profile.reviewPending)
                              Semantics(
                                liveRegion: true,
                                child: Padding(
                                  padding: const EdgeInsets.symmetric(
                                    horizontal: 16,
                                  ),
                                  child: Text(
                                    context.strings.profileReviewPending,
                                  ),
                                ),
                              ),
                            if (wide)
                              Row(
                                crossAxisAlignment: CrossAxisAlignment.start,
                                spacing: 32,
                                children: [
                                  SizedBox(
                                    width: 200,
                                    child: Column(
                                      spacing: 12,
                                      crossAxisAlignment:
                                          CrossAxisAlignment.stretch,
                                      children: [counts, details],
                                    ),
                                  ),
                                  if (gameSection != null)
                                    Expanded(child: gameSection),
                                ],
                              )
                            else
                              Padding(
                                padding: const EdgeInsets.symmetric(
                                  horizontal: 16,
                                ),
                                child: Column(
                                  crossAxisAlignment:
                                      CrossAxisAlignment.stretch,
                                  spacing: 16,
                                  children: [counts, details, ?gameSection],
                                ),
                              ),
                          ],
                        ),
                      ),
                    ),
                  ),
                ),
              ],
            ),
          ),
        );
      },
    );
  }

  Widget _profileSkeleton() => LayoutBuilder(
    builder: (context, constraints) {
      final wide = constraints.maxWidth > 780;
      final side = wide ? 24.0 : 0.0;
      return Semantics(
        liveRegion: true,
        label: context.strings.profileLoading,
        child: Padding(
          padding: EdgeInsets.symmetric(horizontal: side),
          child: Align(
            alignment: Alignment.topCenter,
            child: ConstrainedBox(
              constraints: const BoxConstraints(maxWidth: 1016),
              child: Stack(
                children: [
                  ExcludeSemantics(
                    child: Skeletonizer.zone(
                      effect: SolidColorEffect(
                        color: Theme.of(context)
                            .colorScheme
                            .surfaceContainerHighest,
                      ),
                      child: _header(null, wide, navigation: false),
                    ),
                  ),
                  PositionedDirectional(
                    top: 0,
                    start: 8,
                    end: 8,
                    child: _profileNavigation(true),
                  ),
                ],
              ),
            ),
          ),
        ),
      );
    },
  );

  Widget _ownerActions(bool wide) => Wrap(
    spacing: 8,
    runSpacing: 8,
    crossAxisAlignment: WrapCrossAlignment.center,
    children: [
      for (final (label, icon, address) in [
        (
          context.strings.cloudSaves,
          Icons.cloud_outlined,
          '/account/cloud-saves',
        ),
        (
          context.strings.editProfile,
          Icons.edit_outlined,
          profileEditPath(widget.username),
        ),
      ])
        if (wide)
          OutlinedButton.icon(
            style: LauncherControl.style(context),
            onPressed: () => context.push(address),
            icon: Icon(icon, size: 16),
            label: Text(label),
          )
        else
          IconButton.outlined(
            style: LauncherControl.style(context),
            tooltip: label,
            onPressed: () => context.push(address),
            icon: Icon(icon, size: 16),
          ),
    ],
  );

  Widget _profileNavigation(bool overPhoto) {
    final style = overPhoto
        ? IconButton.styleFrom(
            backgroundColor: Colors.black.withValues(alpha: .45),
            foregroundColor: Colors.white,
          )
        : null;
    return Row(
      mainAxisAlignment: MainAxisAlignment.spaceBetween,
      children: [
        BackButton(style: style, onPressed: () => profileBack(context, '/')),
        IconButton(
          style: style,
          tooltip: context.strings.profileRefresh,
          onPressed: _refresh,
          icon: const Icon(Icons.refresh),
        ),
      ],
    );
  }

  Widget _header(
    PublicProfile? profile,
    bool wide, {
    Widget? actions,
    bool navigation = true,
  }) {
    final theme = Theme.of(context);
    final height = wide ? 160.0 : 150.0;
    final avatarSize = wide ? 96.0 : 64.0;
    final stackActions =
        !wide &&
        MediaQuery.sizeOf(context).width /
                (MediaQuery.textScalerOf(context).scale(22) / 22) <=
            365;
    final title = theme.textTheme.headlineSmall?.copyWith(
      fontWeight: FontWeight.w600,
      fontSize: wide ? 28 : 22,
      height: 1.25,
    );
    final cover = profile?.cover;
    final placeholder = ColoredBox(
      color: theme.colorScheme.surfaceContainerHighest,
    );
    return Stack(
      children: [
        SizedBox(
          key: const ValueKey('profile cover'),
          width: double.infinity,
          height: height,
          child: ClipRRect(
            borderRadius: wide ? BorderRadius.circular(12) : BorderRadius.zero,
            child: profile == null
                ? Bone(height: height)
                : cover == null
                ? placeholder
                : Image.network(
                    cover.toString(),
                    fit: BoxFit.cover,
                    excludeFromSemantics: true,
                    frameBuilder: (_, child, frame, _) =>
                        frame == null ? placeholder : child,
                    errorBuilder: (_, _, _) => placeholder,
                  ),
          ),
        ),
        Padding(
          padding: EdgeInsets.only(
            top: height - (wide ? 26 : 24),
            left: wide ? 19 : 16,
            right: wide ? 19 : 16,
          ),
          child: Column(
            crossAxisAlignment: CrossAxisAlignment.stretch,
            children: [
              Row(
                crossAxisAlignment: CrossAxisAlignment.start,
                spacing: wide ? 14 : 12,
                children: [
                  SizedBox.square(
                    key: const ValueKey('profile avatar'),
                    dimension: avatarSize,
                    child: profile == null
                        ? DecoratedBox(
                            decoration: BoxDecoration(
                              color: theme.scaffoldBackgroundColor,
                              shape: BoxShape.circle,
                            ),
                            child: Padding(
                              padding: const EdgeInsets.all(2),
                              child: Bone.circle(size: avatarSize - 4),
                            ),
                          )
                        : AccountAvatar(
                            api: widget.session.api,
                            username: profile.username,
                            address: profile.avatar,
                            size: avatarSize,
                          ),
                  ),
                  Expanded(
                    child: Padding(
                      padding: EdgeInsets.only(top: wide ? 40 : 24),
                      child: Column(
                        crossAxisAlignment: CrossAxisAlignment.start,
                        spacing: 4,
                        children: [
                          if (profile == null) ...[
                            Bone.text(width: wide ? 136 : 112, style: title),
                            Bone.text(
                              words: 1,
                              style: theme.textTheme.bodySmall,
                            ),
                          ] else ...[
                            Text(profile.nickname, style: title),
                            IdentityDetails(
                              username: profile.username,
                              region: profile.country,
                            ),
                          ],
                        ],
                      ),
                    ),
                  ),
                  if (actions != null && !stackActions)
                    Padding(
                      padding: EdgeInsets.only(top: wide ? 40 : 24),
                      child: ConstrainedBox(
                        constraints: BoxConstraints(maxWidth: wide ? 360 : 144),
                        child: actions,
                      ),
                    ),
                ],
              ),
              if (stackActions)
                Padding(
                  padding: const EdgeInsets.only(top: 8),
                  child: actions == null
                      ? const SizedBox(height: 44)
                      : Align(
                          alignment: AlignmentDirectional.centerStart,
                          child: actions,
                        ),
                ),
            ],
          ),
        ),
        if (navigation)
          PositionedDirectional(
            top: 0,
            start: 8,
            end: 8,
            child: _profileNavigation(true),
          ),
      ],
    );
  }

  Widget _about(PublicProfile profile) {
    final theme = Theme.of(context);
    final strings = context.strings;
    final joined = profile.joinedAt;
    return Column(
      crossAxisAlignment: CrossAxisAlignment.start,
      spacing: 8,
      children: [
        Text(profile.bio.trim().isEmpty ? strings.profileNoBio : profile.bio),
        if (joined != null)
          Row(
            spacing: 6,
            children: [
              const Icon(Icons.calendar_month_outlined, size: 16),
              Flexible(
                child: Text(
                  strings.profileJoined(
                    DateFormat.yMMMM(strings.localeName).format(joined),
                  ),
                  style: theme.textTheme.bodySmall,
                ),
              ),
            ],
          ),
        for (final link in profile.links)
          TextButton.icon(
            onPressed: () => _open(link.url),
            icon: const Icon(Icons.open_in_new, size: 16),
            label: Text(
              link.label,
              maxLines: 2,
              overflow: TextOverflow.ellipsis,
            ),
          ),
      ],
    );
  }
}
