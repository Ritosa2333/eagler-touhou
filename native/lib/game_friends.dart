import 'package:lucide_icons_flutter/lucide_icons.dart';
import 'package:material_ui/material_ui.dart';

import 'launcher_prism.dart';

import 'account.dart';
import 'l10n/strings.dart';
import 'site_documents.dart';
import 'launcher_navigation.dart';
import 'launcher_select.dart';
import 'profile_data.dart';
import 'profile_identity.dart';

typedef _Activity = ({List<ProfilePerson> players, bool failed});

/// Friends with publicly visible activity for a single work.
class GameFriends extends StatefulWidget {
  const GameFriends({
    super.key,
    required this.session,
    required this.owner,
    required this.game,
  });

  final AccountSession session;
  final String owner;
  final String game;

  @override
  State<GameFriends> createState() => _GameFriendsState();
}

class _GameFriendsState extends State<GameFriends> {
  static const _limit = 3;
  int _generation = 0;
  late Future<_Activity> _activity;

  @override
  void initState() {
    super.initState();
    _activity = _load();
    widget.session.socialRevision.addListener(_reload);
  }

  @override
  void dispose() {
    widget.session.socialRevision.removeListener(_reload);
    super.dispose();
  }

  void _reload() {
    final read = _load();
    read.ignore();
    setState(() {
      _activity = read;
    });
  }

  Future<_Activity> _load() async {
    final generation = ++_generation;
    final api = widget.session.api;
    final social = await api.social(
      widget.owner,
      signedIn: true,
      list: ProfileList.friends,
    );
    final players = <ProfilePerson>[];
    var failed = false;
    final friends = social.visible ? social.people : const <ProfilePerson>[];
    for (
      var index = 0;
      index < friends.length && players.length < _limit;
      index += _limit
    ) {
      if (!mounted || generation != _generation) break;
      final matches = await Future.wait(
        friends.skip(index).take(_limit).map((friend) async {
          try {
            final statistics = await api.profileStatistics(
              friend.username,
              owner: false,
            );
            return _played(statistics.games[widget.game]) ? friend : null;
          } catch (_) {
            failed = true;
            return null;
          }
        }),
      );
      players.addAll(matches.whereType<ProfilePerson>());
    }
    return (players: players.take(_limit).toList(), failed: failed);
  }

  bool _played(SaveSummary? save) =>
      save != null &&
      ((save.playtime ?? 0) > 0 ||
          (save.playCount ?? 0) > 0 ||
          save.progress.any(
            (record) =>
                record.cleared == true ||
                record.oneCreditClear == true ||
                (record.playCount ?? 0) > 0 ||
                (record.bestScore ?? 0) > 0,
          ));

  @override
  Widget build(BuildContext context) {
    final strings = context.strings;
    final theme = Theme.of(context);
    final touch = MediaQuery.sizeOf(context).width <= launcherCompactWidth;
    final rowStyle = TextButton.styleFrom(
      textStyle: LauncherControl.label(context),
      minimumSize: Size(0, touch ? LauncherControl.height : 32),
      tapTargetSize: MaterialTapTargetSize.shrinkWrap,
      alignment: AlignmentDirectional.centerStart,
      padding: const EdgeInsets.symmetric(horizontal: 8),
      foregroundColor: theme.colorScheme.onSurfaceVariant,
    );
    return Column(
      crossAxisAlignment: CrossAxisAlignment.start,
      spacing: 6,
      children: [
        Semantics(
          header: true,
          child: Text(strings.friends, style: launcherSectionTitle(context)),
        ),
        FutureBuilder<_Activity>(
          future: _activity,
          builder: (context, snapshot) {
            if (snapshot.connectionState != ConnectionState.done) {
              return const Padding(
                padding: EdgeInsets.symmetric(vertical: 12),
                child: LinearProgressIndicator(),
              );
            }
            final result = snapshot.data;
            if (result != null && result.players.isNotEmpty) {
              return Column(
                crossAxisAlignment: CrossAxisAlignment.stretch,
                children: [
                  for (final player in result.players)
                    TextButton(
                      style: rowStyle,
                      onPressed: () =>
                          openSiteLink(context, profilePath(player.username)),
                      child: Text(player.nickname),
                    ),
                ],
              );
            }
            final failed = snapshot.hasError || result?.failed == true;
            return Column(
              crossAxisAlignment: CrossAxisAlignment.start,
              children: [
                Semantics(
                  liveRegion: true,
                  child: Text(
                    failed
                        ? strings.socialLoadFailed
                        : strings.noFriendActivity,
                    style: theme.textTheme.bodySmall?.copyWith(
                      color: theme.colorScheme.onSurfaceVariant,
                    ),
                  ),
                ),
                if (failed)
                  TextButton(
                    style: rowStyle,
                    onPressed: _reload,
                    child: Text(strings.roomRetry),
                  ),
              ],
            );
          },
        ),
        TextButton.icon(
          style: rowStyle,
          icon: const Icon(LucideIcons.usersRound, size: 16),
          onPressed: () => openSiteLink(
            context,
            profileSocialPath(widget.owner, ProfileList.friends),
          ),
          label: Text(strings.viewFriends),
        ),
      ],
    );
  }
}
