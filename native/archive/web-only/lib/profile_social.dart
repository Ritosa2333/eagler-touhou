import 'package:go_router/go_router.dart';
import 'package:intl/intl.dart';
import 'package:material_ui/material_ui.dart';

import 'account.dart';
import 'inbox.dart' show messageThreadPath;
import 'l10n/strings.dart';
import 'launcher_select.dart';
import 'profile_data.dart';
import 'profile_identity.dart';
import 'profile_loading.dart';
import 'launcher_navigation.dart';

String profileSocialPath(String username, ProfileList tab) =>
    '${profilePath(username)}?social=${tab.name}';

ProfileList? profileSocialTab(Uri uri) =>
    ProfileList.parse(uri.queryParameters['social']) ??
    (uri.fragment == 'profile-friends' ? ProfileList.friends : null);

extension _ProfileListStrings on ProfileList {
  String label(LauncherStrings strings) => switch (this) {
    ProfileList.following => strings.profileFollowing,
    ProfileList.followers => strings.profileFollowers,
    ProfileList.friends => strings.friends,
  };

  String empty(LauncherStrings strings) => switch (this) {
    ProfileList.following => strings.profileFollowingEmpty,
    ProfileList.followers => strings.profileFollowersEmpty,
    ProfileList.friends => strings.profileFriendsEmpty,
  };
}

void profileBack(BuildContext context, String fallback) =>
    closeLauncherPage(context, fallback);

class ProfileRelationships extends StatefulWidget {
  const ProfileRelationships({
    super.key,
    required this.session,
    required this.username,
    this.showActions = true,
    this.layoutBuilder,
  });

  final AccountSession session;
  final String username;
  final bool showActions;
  final Widget Function(Widget actions, Widget counts)? layoutBuilder;

  @override
  State<ProfileRelationships> createState() => _ProfileRelationshipsState();
}

class _ProfileRelationshipsState extends State<ProfileRelationships> {
  late Future<ProfileSocial> _read;
  bool _saving = false;
  bool _followFailed = false;
  String? _viewer;
  int _viewerGeneration = 0;

  void _refresh() {
    final viewer = widget.session.identity?.username.toLowerCase();
    final read = widget.session.api.social(
      widget.username,
      signedIn: viewer != null,
    );
    read.ignore();
    setState(() {
      if (_viewer != viewer) {
        _viewer = viewer;
        _viewerGeneration++;
        _saving = false;
        _followFailed = false;
      }
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

  Future<void> _follow(bool following) async {
    if (_saving) return;
    final generation = _viewerGeneration;
    setState(() {
      _saving = true;
      _followFailed = false;
    });
    try {
      await widget.session.api.setFollowing(widget.username, following);
      if (mounted && generation == _viewerGeneration) {
        widget.session.socialRevision.value++;
      }
    } catch (error) {
      if (!mounted || generation != _viewerGeneration) return;
      widget.session.expireIfUnauthorized(error);
      if (generation == _viewerGeneration) {
        setState(() => _followFailed = true);
      }
    } finally {
      if (mounted && generation == _viewerGeneration) {
        setState(() => _saving = false);
      }
    }
  }

  @override
  Widget build(BuildContext context) => FutureBuilder(
    key: ValueKey(_viewerGeneration),
    future: _read,
    builder: (context, snapshot) {
      final result = snapshot.data;
      final theme = Theme.of(context);
      final strings = context.strings;
      final numbers = NumberFormat.decimalPattern(strings.localeName);
      final actions =
          widget.showActions && _viewer != widget.username.toLowerCase();
      final loading = snapshot.connectionState != ConnectionState.done;
      final following = result?.following;
      if (widget.layoutBuilder == null &&
          !actions &&
          result == null &&
          loading) {
        return const SizedBox(height: 64);
      }
      final counts = [
        result?.followingCount,
        result?.followerCount,
        result?.friendCount,
      ];
      final actionWidgets = <Widget>[
        if (actions)
          Wrap(
            spacing: 8,
            runSpacing: 8,
            children: [
              if (_viewer != null)
                if (widget.layoutBuilder != null &&
                    MediaQuery.sizeOf(context).width <= 780)
                  IconButton.outlined(
                    tooltip: strings.privateMessages,
                    onPressed: () =>
                        context.push(messageThreadPath(widget.username)),
                    icon: const Icon(Icons.mail_outline, size: 16),
                  )
                else
                  OutlinedButton.icon(
                    onPressed: () =>
                        context.push(messageThreadPath(widget.username)),
                    icon: const Icon(Icons.mail_outline),
                    label: Text(strings.privateMessages),
                  ),
              FilledButton.tonal(
                onPressed: _viewer == null
                    ? () => context.push('/login')
                    : _saving || loading || following == null
                    ? null
                    : () => _follow(!following),
                child: Text(
                  _viewer == null
                      ? strings.profileFollowSignIn
                      : following == true
                      ? strings.profileUnfollow
                      : strings.profileFollow,
                ),
              ),
            ],
          ),
        if (snapshot.hasError)
          Align(
            alignment: AlignmentDirectional.centerStart,
            child: TextButton.icon(
              onPressed: _refresh,
              icon: const Icon(Icons.refresh),
              label: Text(strings.profileSocialRetry),
            ),
          ),
        if (_followFailed)
          Semantics(
            liveRegion: true,
            child: Text(
              strings.profileFollowFailed,
              style: TextStyle(color: theme.colorScheme.error),
            ),
          ),
      ];
      final showCounts =
          result != null &&
          result.visible &&
          result.followerCount != null &&
          result.followingCount != null;
      final inlineCounts = widget.layoutBuilder != null;
      final countStyle = inlineCounts
          ? theme.textTheme.titleMedium!.copyWith(
              fontSize: 15,
              fontWeight: FontWeight.w400,
            )
          : theme.textTheme.titleMedium;
      final countStrut = inlineCounts
          ? StrutStyle.fromTextStyle(countStyle!, forceStrutHeight: true)
          : null;
      Widget countContents(int index, ProfileList item) {
        final contents = [
          Text(
            counts[index] == null ? '—' : numbers.format(counts[index]),
            style: countStyle,
            strutStyle: countStrut,
          ),
          Text(
            item.label(strings),
            textAlign: TextAlign.center,
            style: theme.textTheme.bodySmall,
            strutStyle: countStrut,
          ),
        ];
        return inlineCounts
            ? Wrap(
                spacing: 5,
                runSpacing: 3,
                crossAxisAlignment: WrapCrossAlignment.end,
                children: contents,
              )
            : Column(spacing: 3, children: contents);
      }

      final countButtons = [
        for (final (index, item) in ProfileList.values.indexed)
          TextButton(
            style: inlineCounts
                ? TextButton.styleFrom(
                    padding: const EdgeInsets.symmetric(
                      horizontal: 4,
                      vertical: 7,
                    ),
                  )
                : null,
            onPressed: !showCounts
                ? null
                : () => context.push(profileSocialPath(widget.username, item)),
            child: countContents(index, item),
          ),
      ];
      final countRow = showCounts || widget.layoutBuilder != null && loading
          ? inlineCounts
                ? Wrap(
                    spacing: 12,
                    runSpacing: 4,
                    crossAxisAlignment: WrapCrossAlignment.center,
                    children: countButtons,
                  )
                : Row(
                    children: [
                      for (final button in countButtons)
                        Expanded(child: button),
                    ],
                  )
          : const SizedBox.shrink();
      return widget.layoutBuilder?.call(
            Column(
              mainAxisSize: MainAxisSize.min,
              crossAxisAlignment: CrossAxisAlignment.start,
              spacing: 8,
              children: actionWidgets,
            ),
            countRow,
          ) ??
          Column(
            crossAxisAlignment: CrossAxisAlignment.stretch,
            spacing: 8,
            children: [...actionWidgets, if (showCounts) countRow],
          );
    },
  );
}

class ProfileSocialPage extends StatefulWidget {
  const ProfileSocialPage({
    super.key,
    required this.session,
    required this.username,
    required this.startingTab,
  });

  final AccountSession session;
  final String username;
  final ProfileList startingTab;

  @override
  State<ProfileSocialPage> createState() => _ProfileSocialPageState();
}

class _ProfileSocialPageState extends State<ProfileSocialPage> {
  ProfileList get _tab => widget.startingTab;
  late String? _viewer = widget.session.identity?.username.toLowerCase();
  late Future<ProfileSocial> _data = _load();

  @override
  void initState() {
    super.initState();
    widget.session.addListener(_sessionChanged);
    widget.session.socialRevision.addListener(_reload);
  }

  @override
  void dispose() {
    widget.session.removeListener(_sessionChanged);
    widget.session.socialRevision.removeListener(_reload);
    super.dispose();
  }

  @override
  void didUpdateWidget(covariant ProfileSocialPage oldWidget) {
    super.didUpdateWidget(oldWidget);
    if (oldWidget.startingTab != _tab) _data = _load();
  }

  void _sessionChanged() {
    final viewer = widget.session.identity?.username.toLowerCase();
    if (viewer == _viewer) return;
    _viewer = viewer;
    _reload();
  }

  void _reload() {
    setState(() {
      _data = _load();
    });
  }

  Future<ProfileSocial> _load() {
    final read = widget.session.api.social(
      widget.username,
      signedIn: _viewer != null,
      list: _tab,
    );
    read.ignore();
    return read;
  }

  void _select(ProfileList next) {
    if (next == _tab) return;
    final uri = GoRouterState.of(context).uri;
    context.replace(
      uri
          .replace(
            queryParameters: {...uri.queryParameters, 'social': next.name},
            fragment: '',
          )
          .toString(),
    );
  }

  @override
  Widget build(BuildContext context) => Scaffold(
    appBar: AppBar(
      title: Text(context.strings.profileSocialTitle),
      leading: BackButton(
        onPressed: () => profileBack(context, profilePath(widget.username)),
      ),
    ),
    body: SafeArea(
      child: Center(
        child: ConstrainedBox(
          constraints: const BoxConstraints(maxWidth: 640),
          child: Column(
            children: [
              Padding(
                padding: const EdgeInsets.all(12),
                child: LauncherSegments<ProfileList>(
                  segments: [
                    for (final entry in ProfileList.values)
                      ButtonSegment(
                        value: entry,
                        label: Text(entry.label(context.strings)),
                      ),
                  ],
                  value: _tab,
                  onChanged: _select,
                ),
              ),
              Expanded(
                child: FutureBuilder(
                  future: _data,
                  builder: (context, snapshot) {
                    if (snapshot.connectionState != ConnectionState.done) {
                      return Align(
                        alignment: AlignmentDirectional.topStart,
                        child: Padding(
                          padding: const EdgeInsets.symmetric(horizontal: 16),
                          child: ProfilePanelLoading(
                            label: context.strings.profileSocialLoading,
                          ),
                        ),
                      );
                    }
                    if (snapshot.hasError) {
                      return Center(
                        child: Padding(
                          padding: const EdgeInsets.symmetric(horizontal: 16),
                          child: OutlinedButton.icon(
                            onPressed: _reload,
                            icon: const Icon(Icons.refresh),
                            label: Text(context.strings.profileSocialRetry),
                          ),
                        ),
                      );
                    }
                    final social = snapshot.data!;
                    if (!social.visible) {
                      return Center(
                        child: Text(context.strings.profileSocialPrivate),
                      );
                    }
                    if (social.people.isEmpty) {
                      return Center(child: Text(_tab.empty(context.strings)));
                    }
                    return ListView.builder(
                      key: PageStorageKey((widget.username, _tab, _viewer)),
                      padding: const EdgeInsets.symmetric(horizontal: 16),
                      itemCount: social.people.length,
                      itemBuilder: (_, index) {
                        final person = social.people[index];
                        return ListTile(
                          leading: AccountAvatar(
                            api: widget.session.api,
                            username: person.username,
                          ),
                          title: Text(
                            person.nickname,
                            maxLines: 1,
                            overflow: TextOverflow.ellipsis,
                          ),
                          subtitle: IdentityDetails(
                            username: person.username,
                            region: person.region,
                          ),
                          trailing: const Icon(Icons.chevron_right),
                          onTap: () =>
                              context.push(profilePath(person.username)),
                        );
                      },
                    );
                  },
                ),
              ),
            ],
          ),
        ),
      ),
    ),
  );
}
