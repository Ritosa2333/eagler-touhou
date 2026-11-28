const th09CharacterNames = [
  '灵梦',
  '魔理沙',
  '咲夜',
  '妖梦',
  '铃仙',
  '琪露诺',
  '莉莉卡',
  '米斯蒂娅',
  '帝',
  '幽香',
  '文',
  '梅蒂欣',
  '小町',
  '四季映姬',
  '梅露兰',
  '露娜萨',
];

const _englishCharacterNames = [
  'Reimu',
  'Marisa',
  'Sakuya',
  'Youmu',
  'Reisen',
  'Cirno',
  'Lyrica',
  'Mystia',
  'Tewi',
  'Yuuka',
  'Aya',
  'Medicine',
  'Komachi',
  'Shikieiki',
  'Merlin',
  'Lunasa',
];
const _japaneseCharacterNames = [
  '霊夢',
  '魔理沙',
  '咲夜',
  '妖夢',
  '鈴仙',
  'チルノ',
  'リリカ',
  'ミスティア',
  'てゐ',
  '幽香',
  '文',
  'メディスン',
  '小町',
  '四季映姫',
  'メルラン',
  'ルナサ',
];

String th09CharacterLabel(int id, String language) => switch (language) {
  'en' => _englishCharacterNames[id],
  'ja' => _japaneseCharacterNames[id],
  _ => th09CharacterNames[id],
};

int? th09CharacterParameter(String? value) {
  final id = int.tryParse(value ?? '');
  return id != null && id >= 0 && id < th09CharacterNames.length ? id : null;
}

typedef Th09Point = ({int at, int value});
typedef Th09CharacterResult = ({int character, int games, int wins});

typedef Th09LobbyRating = ({int score, double interval, int games, int group});
const th09RatingsRefreshPeriod = Duration(seconds: 12);
typedef Th09CharacterScore = ({int character, int score});
typedef Th09PlayerScoreCard = ({
  int? score,
  List<Th09CharacterScore> characters,
});

class Th09LobbyRatings {
  const Th09LobbyRatings({this.players = const {}, this.cards = const {}});

  factory Th09LobbyRatings.fromJson(Map<String, dynamic> json) {
    final players = <String, Th09LobbyRating>{};
    final cards = <String, Th09PlayerScoreCard>{};
    if (json['players'] case final Map<String, dynamic> rows) {
      for (final MapEntry(:key, :value) in rows.entries) {
        if (value
            case {
              'rating': final int score,
              'interval': final num interval,
              'games': final int games,
              'group': final int group,
            }
            when interval >= 0 &&
                interval.isFinite &&
                games >= 0 &&
                group > 0) {
          players[key.toLowerCase()] = (
            score: score,
            interval: interval.toDouble(),
            games: games,
            group: group,
          );
        }
      }
    }
    if (json['cards'] case final Map<String, dynamic> rows) {
      for (final MapEntry(:key, :value) in rows.entries) {
        if (value case {'characters': final List<dynamic> characters}) {
          final score = value['rating'];
          cards[key.toLowerCase()] = (
            score: score is int ? score : null,
            characters: List.unmodifiable([
              for (final row in characters.take(3))
                if (row case [final int id, final int points]
                    when id >= 0 && id < th09CharacterNames.length)
                  (character: id, score: points),
            ]),
          );
        }
      }
    }
    return Th09LobbyRatings(
      players: Map.unmodifiable(players),
      cards: Map.unmodifiable(cards),
    );
  }

  final Map<String, Th09LobbyRating> players;
  final Map<String, Th09PlayerScoreCard> cards;
}

class Th09CharacterRating {
  const Th09CharacterRating({
    required this.character,
    required this.score,
    required this.interval,
    required this.games,
    this.history = const [],
  });

  factory Th09CharacterRating.fromJson(Map<String, dynamic> json) {
    final id = json['character'] as int;
    if (id < 0 || id >= th09CharacterNames.length) {
      throw const FormatException('Invalid th09 character');
    }
    return Th09CharacterRating(
      character: id,
      score: json['rating'] as int,
      interval: json['interval'] as int,
      games: json['games'] as int,
      history: th09Points(json['history'], 'rating'),
    );
  }

  final int character;
  final int score;
  final int interval;
  final int games;
  final List<Th09Point> history;
}

typedef Th09DuelReplay = ({
  String id,
  String? name,
  int? bytes,
  int? frames,
  List<int> characters,
});

List<Th09DuelReplay> th09DuelReplaysFromJson(Object? value) => [
  for (final row in _rows(value))
    if (row case {
      'roundId': final String id,
      'characters': final List<dynamic> characters,
    })
      if (RegExp(r'^[0-9]{1,16}:[0-9]{1,16}$').hasMatch(id) &&
          characters.length == 2 &&
          characters.every(
            (id) => id is int && id >= 0 && id < th09CharacterNames.length,
          ))
        (
          id: id,
          name: switch (row['name']) {
            final String name
                when name.length <= 160 &&
                    name.trim().isNotEmpty &&
                    name == name.trim() &&
                    !RegExp(r'[<>:"/\\|?*\x00-\x1f\x7f]').hasMatch(name) =>
              name,
            _ => null,
          },
          bytes: row['size'] as int?,
          frames: row['frames'] as int?,
          characters: characters.cast<int>(),
        ),
];

List<Th09Point> th09Points(Object? value, String key) => [
  for (final row in _rows(value))
    if (row['at'] case final int at)
      if (row[key] case final int score)
        if (at >= 0 && at <= 8640000000000000) (at: at, value: score),
]..sort((a, b) => a.at.compareTo(b.at));

List<Th09CharacterRating> th09CharacterRatings(Object? value) => [
  for (final row in _rows(value)) Th09CharacterRating.fromJson(row),
];

class Th09Board {
  const Th09Board(this.updatedAt, this.minimumGames, this.players);

  factory Th09Board.fromJson(Map<String, dynamic> json) => Th09Board(
    DateTime.fromMillisecondsSinceEpoch(json['as_of'] as int),
    (json['default_min_games'] as int? ?? 11).clamp(1, 1000000),
    _rows(json['board']).map(Th09BoardPlayer.fromJson).toList(),
  );

  final DateTime updatedAt;
  final int minimumGames;
  final List<Th09BoardPlayer> players;
}

enum Th09RatingStatus { active, provisional, inactive }

class Th09BoardPlayer {
  const Th09BoardPlayer({
    required this.id,
    required this.nickname,
    required this.username,
    required this.rating,
    required this.interval,
    required this.games,
    required this.wins,
    required this.opponents,
    required this.group,
    required this.lastPlayed,
    required this.status,
    required this.topCharacters,
    required this.characterRatings,
    this.hasPublishedCharacterRatings = false,
    this.avatar,
  });

  factory Th09BoardPlayer.fromJson(Map<String, dynamic> json) {
    final profile = json['profile'] as Map<String, dynamic>?;
    final id = json['player'] as String;
    return Th09BoardPlayer(
      id: id,
      nickname: profile?['nickname'] as String? ?? id,
      username: profile?['username'] as String?,
      rating: (json['rating'] as num).round(),
      interval: (json['interval_halfwidth'] as num).round(),
      games: json['games'] as int,
      wins: json['wins'] as int,
      opponents: json['opponents'] as int,
      group: json['connected_group'] as int,
      lastPlayed: DateTime.fromMillisecondsSinceEpoch(json['last_at'] as int),
      status: Th09RatingStatus.values.byName(json['status'] as String),
      topCharacters: [
        for (final row in _rows(json['top_characters']))
          if (row['character'] case final int id)
            if (id >= 0 && id < th09CharacterNames.length) id,
      ],
      characterRatings: th09CharacterRatings(json['character_ratings']),
      hasPublishedCharacterRatings: json['character_ratings'] is List,
      avatar: profile?['avatar'] is String
          ? profile!['avatar'] as String
          : null,
    );
  }

  final String id;
  final String nickname;
  final String? username;
  final int rating;
  final int interval;
  final int games;
  final int wins;
  final int opponents;
  final int group;
  final DateTime lastPlayed;
  final Th09RatingStatus status;
  final List<int> topCharacters;
  final List<Th09CharacterRating> characterRatings;
  final bool hasPublishedCharacterRatings;
  final String? avatar;

  Th09PlayerScoreCard? get scoreCard {
    if (!hasPublishedCharacterRatings) return null;
    final byCharacter = {
      for (final character in characterRatings) character.character: character,
    };
    final byGames = [...characterRatings]
      ..sort((a, b) {
        final games = b.games.compareTo(a.games);
        return games == 0 ? a.character.compareTo(b.character) : games;
      });
    final favorites = {
      ...topCharacters.where(byCharacter.containsKey),
      ...byGames.map((row) => row.character),
    };
    return (
      score: rating,
      characters: [
        for (final id in favorites.take(3))
          (character: id, score: byCharacter[id]!.score),
      ],
    );
  }
}

Iterable<Map<String, dynamic>> _rows(Object? value) =>
    value is List ? value.whereType<Map<String, dynamic>>() : const [];
