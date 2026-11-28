import 'th09_ratings.dart';
import 'profile_display.dart';
import 'save_progress.dart';

export 'profile_display.dart';
export 'save_progress.dart' show PlayDifficulty, SaveProgress;

bool isPublicUsername(String name) =>
    name != '.' &&
    name != '..' &&
    RegExp(r'^[A-Za-z0-9_.-]{1,32}$').hasMatch(name);

enum ProfileList {
  following,
  followers,
  friends;

  static ProfileList? parse(String? value) =>
      values.where((tab) => tab.name == value).firstOrNull;
}

class PublicProfile {
  const PublicProfile({
    required this.username,
    required this.nickname,
    this.bio = '',
    this.joinedAt,
    this.country,
    this.region,
    this.avatar,
    this.cover,
    this.links = const [],
    this.showCareer = true,
    this.showRecords = true,
    this.showVersus = true,
    this.reviewPending = false,
    this.defaultView = ProfileGames.th09,
  });

  final String username;
  final String nickname;
  final String bio;
  final DateTime? joinedAt;
  final String? country;
  final String? region;
  final Uri? avatar;
  final Uri? cover;
  final List<({String label, Uri url})> links;
  final bool showCareer;
  final bool showRecords;
  final bool showVersus;
  final bool reviewPending;
  final ProfileGames defaultView;

  factory PublicProfile.fromJson(
    Map<String, dynamic> json,
    Uri? Function(String kind, Object? value) mediaUrl,
  ) {
    final details = _object(json['details']);
    final media = _object(json['media']);
    final display = ProfileDisplay.fromJson(details['display']);
    final links = <Uri, ({String label, Uri url})>{};
    for (final link in _rows(details['links'])) {
      if (link case {
        'label': final String label,
        'url': final String address,
      }) {
        final url = Uri.tryParse(address);
        if (url != null &&
            (url.scheme == 'http' || url.scheme == 'https') &&
            url.host.isNotEmpty &&
            url.userInfo.isEmpty) {
          links.putIfAbsent(url, () => (label: label, url: url));
        }
      }
    }
    return PublicProfile(
      username: json['username'] as String,
      nickname: json['nickname'] as String,
      bio: details['bio'] as String? ?? '',
      joinedAt: DateTime.tryParse(json['joinedAt'] as String? ?? '')?.toUtc(),
      country: json['countryCode'] as String?,
      region: json['regionCode'] as String?,
      avatar: mediaUrl('avatar', media['avatar']),
      cover: mediaUrl('cover', media['cover']),
      links: links.values.toList(growable: false),
      showCareer: display.career,
      showRecords: display.records,
      showVersus: display.versus,
      reviewPending: json['reviewPending'] == true,
      defaultView: display.selected ?? ProfileGames.th09,
    );
  }
}

typedef ProfilePerson = ({String username, String nickname, String? region});

class ProfileSocial {
  const ProfileSocial({
    required this.visible,
    this.following,
    this.followingCount,
    this.followerCount,
    this.friendCount,
    this.people = const [],
  });

  final bool visible;
  final bool? following;
  final int? followingCount;
  final int? followerCount;
  final int? friendCount;
  final List<ProfilePerson> people;

  factory ProfileSocial.fromJson(Map<String, dynamic> json, ProfileList? list) {
    final visible = json['visible'] as bool;
    final key = list == ProfileList.following ? 'followingUsers' : list?.name;
    return ProfileSocial(
      visible: visible,
      following: json['following'] as bool?,
      followerCount: visible ? json['followerCount'] as int? : null,
      followingCount: visible ? json['followingCount'] as int? : null,
      friendCount: visible ? json['friendCount'] as int? : null,
      people: visible && key != null
          ? [
              for (final row in _rows(json[key]))
                if (row
                    case {
                      'username': final String username,
                      'nickname': final String nickname,
                    }
                    when isPublicUsername(username))
                  (
                    username: username,
                    nickname: nickname,
                    region:
                        (row['regionCode'] ?? row['countryCode']) as String?,
                  ),
            ]
          : const [],
    );
  }
}

typedef SaveSummary = ({
  String game,
  num? playtime,
  int? playCount,
  List<SaveProgress> progress,
});

class ProfileStatistics {
  const ProfileStatistics({
    required this.career,
    required this.records,
    this.playtime,
    this.games = const {},
  });

  factory ProfileStatistics.fromJson(Map<String, dynamic> json) {
    final career = json['career'] as bool;
    final records = json['records'] as bool;
    final games = <String, SaveSummary>{};
    for (final entry in _rows(json['games'])) {
      final stats = _object(entry['statistics']);
      final id = stats['game'];
      if (id is! String) continue;
      final playCount = stats['playCount'];
      games[id] = (
        game: id,
        playtime: records ? playtimeSeconds(stats['playtimeSeconds']) : null,
        playCount: records && playCount is int && playCount >= 0
            ? playCount
            : null,
        progress: career
            ? readSaveProgress(
                stats['progress'],
                loadouts: stats['loadouts'],
                records: records,
              )
            : const [],
      );
    }
    return ProfileStatistics(
      career: career,
      records: records,
      playtime: records ? json['playtimeSeconds'] as num? : null,
      games: games,
    );
  }

  final bool career;
  final bool records;
  final num? playtime;
  final Map<String, SaveSummary> games;
}

typedef DuelRecord = ({
  int? durationFrames,
  int replayCount,
  String id,
  int wins,
  int losses,
  DateTime playedAt,
  String? opponent,
  String? name,
});

class DuelHistory {
  const DuelHistory({
    required this.games,
    required this.wins,
    required this.rounds,
    required this.nextCursor,
    this.durationFrames,
    this.characters = const [],
    this.characterRatings = const [],
    this.ratingHistory = const [],
    this.rankHistory = const [],
    this.ratingPreview,
  });

  factory DuelHistory.fromJson(Map<String, dynamic> json) {
    final summary = _object(json['summary']);
    return DuelHistory(
      games: summary['games'] as int,
      wins: summary['wins'] as int,
      durationFrames: json['durationFrames'] as int?,
      ratingPreview: _object(json['characterRatingPreview'])['rating'] as int?,
      ratingHistory: th09Points(json['ratingHistory'], 'rating'),
      rankHistory: th09Points(json['rankHistory'], 'rank'),
      characterRatings: th09CharacterRatings(json['characterRatings']),
      characters: [
        for (final row in _rows(json['characters']))
          if (row case {
            'character': final int id,
            'games': final int games,
            'wins': final int wins,
          })
            if (id >= 0 &&
                id < th09CharacterNames.length &&
                games > 0 &&
                wins >= 0 &&
                wins <= games)
              (character: id, games: games, wins: wins),
      ],
      rounds: [
        for (final row in _rows(json['duels']))
          if (row case {
            'duelId': final String id,
            'wins': final int wins,
            'losses': final int losses,
            'playedAt': final int playedAt,
          })
            (
              id: id,
              wins: wins,
              losses: losses,
              durationFrames: row['durationFrames'] as int?,
              replayCount: row['replayCount'] as int? ?? 0,
              playedAt: DateTime.fromMillisecondsSinceEpoch(playedAt),
              opponent: _object(row['opponent'])['username'] as String?,
              name: _object(row['opponent'])['displayName'] as String?,
            ),
      ],
      nextCursor: json['nextCursor'] as String?,
    );
  }

  final int games;
  final int wins;
  final List<DuelRecord> rounds;
  final String? nextCursor;
  final int? durationFrames;
  final List<Th09CharacterResult> characters;
  final List<Th09CharacterRating> characterRatings;
  final List<Th09Point> ratingHistory;
  final List<Th09Point> rankHistory;
  final int? ratingPreview;

  DuelHistory append(DuelHistory page) {
    final joined = {
      for (final duel in [...rounds, ...page.rounds]) duel.id: duel,
    };
    return DuelHistory(
      games: page.games,
      wins: page.wins,
      rounds: joined.values.toList(growable: false),
      nextCursor: page.nextCursor,
      durationFrames: page.durationFrames ?? durationFrames,
      characters: page.characters.isEmpty ? characters : page.characters,
      characterRatings: page.characterRatings.isEmpty
          ? characterRatings
          : page.characterRatings,
      ratingHistory: page.ratingHistory.isEmpty
          ? ratingHistory
          : page.ratingHistory,
      rankHistory: page.rankHistory.isEmpty ? rankHistory : page.rankHistory,
      ratingPreview: page.ratingPreview ?? ratingPreview,
    );
  }
}

Map<String, dynamic> _object(Object? json) =>
    json is Map<String, dynamic> ? json : const {};
Iterable<Map<String, dynamic>> _rows(Object? json) =>
    json is List<dynamic> ? json.whereType<Map<String, dynamic>>() : const [];
