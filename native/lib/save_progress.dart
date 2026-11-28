enum PlayDifficulty {
  easy('Easy', 'E'),
  normal('Normal', 'N'),
  hard('Hard', 'H'),
  lunatic('Lunatic', 'L'),
  extra('Extra', 'Ex'),
  phantasm('Phantasm', 'Ph'),
  overdrive('Overdrive', 'OD');

  const PlayDifficulty(this.label, this.shortLabel);
  final String label;
  final String shortLabel;
}

typedef SaveProgress = ({
  PlayDifficulty difficulty,
  int character,
  int? shot,
  String? mode,
  bool? cleared,
  bool? oneCreditClear,
  int? bestScore,
  int? playCount,
});

Iterable<Map<String, dynamic>> _rows(Object? value, int maximum) =>
    value is List
    ? value.take(maximum).whereType<Map<String, dynamic>>()
    : const [];

int? _count(Object? value, [int maximum = 9007199254740991]) =>
    value is int && value >= 0 && value <= maximum ? value : null;
num? playtimeSeconds(Object? value) =>
    value is num && value.isFinite && value >= 0 && value <= 9007199254740991
    ? value
    : null;
bool? _flag(Object? value) => value is bool ? value : null;
PlayDifficulty? _level(Object? value) =>
    PlayDifficulty.values.where((level) => level.name == value).firstOrNull;

String? _mode(Object? value) => switch (value) {
  'legacy' => 'legacy',
  'pointdevice' => 'pointdevice',
  _ => null,
};

typedef _LoadoutKey = ({int character, int? shot, String? mode});

_LoadoutKey? _loadoutKey(Map<String, dynamic> row) {
  final character = _count(row['character'], 63);
  final shot = _count(row['shot'], 63);
  if (character == null || row['shot'] != null && shot == null) return null;
  return (character: character, shot: shot, mode: _mode(row['mode']));
}

List<SaveProgress> readSaveProgress(
  Object? value, {
  Object? loadouts,
  bool records = true,
}) {
  final playCounts = <_LoadoutKey, int>{};
  if (records) {
    for (final row in _rows(loadouts, 128)) {
      final key = _loadoutKey(row);
      final count = _count(row['playCount']);
      if (key != null && count != null) {
        final earlier = playCounts[key];
        if (earlier == null || count > earlier) playCounts[key] = count;
      }
    }
  }
  final progress = <SaveProgress>[];
  for (final row in _rows(value, 1024)) {
    final key = _loadoutKey(row);
    final difficulty = _level(row['difficulty']);
    if (key == null || difficulty == null) continue;
    final cleared = _flag(row['cleared']);
    final oneCreditClear = _flag(row['oneCreditClear']);
    final playCount = records ? _count(row['playCount']) : null;
    // Modern saves can have seeded leaderboard scores before the first play.
    final neverPlayed =
        playCount == null &&
        playCounts[key] == 0 &&
        cleared == false &&
        oneCreditClear != true;
    progress.add((
      difficulty: difficulty,
      character: key.character,
      shot: key.shot,
      mode: key.mode,
      cleared: cleared,
      oneCreditClear: oneCreditClear,
      bestScore: records && !neverPlayed ? _count(row['bestScore']) : null,
      playCount: neverPlayed ? 0 : playCount,
    ));
  }
  return progress;
}

typedef SpellTally = ({int attempts, int captures});

enum SpellActivity { story, practice }

typedef SpellKey = ({
  int character,
  int? shot,
  String difficulty,
  SpellActivity? activity,
});

class SpellRecord {
  const SpellRecord({
    required this.name,
    this.total,
    this.loadouts = const {},
    this.legacy = const [],
  });

  final String? name;
  final SpellTally? total;
  final Map<SpellKey, SpellTally> loadouts;
  final List<SpellTally?> legacy;
}

SpellTally? _tally(Object? attempts, Object? captures, int maximum) {
  final count = _count(attempts, maximum);
  final captured = _count(captures, maximum);
  if (count == null || captured == null) return null;
  return (attempts: count, captures: captured.clamp(0, count));
}

String? _cardName(Object? value) =>
    value is String && value.isNotEmpty && value.length <= 49 ? value : null;

class SaveStatistics {
  const SaveStatistics({
    required this.game,
    required this.progress,
    required this.playtime,
    required this.spells,
    required this.hasSpellData,
    required this.totalSpells,
    this.playCount,
    this.warnings = const [],
  });

  factory SaveStatistics.fromJson(Map<String, dynamic> json) {
    final archive = json['spellArchive'];
    final totalSpells = archive is Map && archive['scope'] == 'total';
    final cards = json['spellCards'];
    final legacy = <int, SpellRecord>{};
    for (final row in _rows(cards, 141)) {
      final number = _count(row['number'], 141);
      final attempts = row['attempts'];
      final captures = row['captures'];
      if (number == null ||
          number == 0 ||
          attempts is! List ||
          attempts.length != 6 ||
          captures is! List ||
          captures.length != 6) {
        continue;
      }
      legacy[number] = SpellRecord(
        name: _cardName(row['name']),
        legacy: [
          for (var index = 0; index < attempts.length; index++)
            _tally(attempts[index], captures[index], 65535),
        ],
      );
    }
    if (archive is Map && (totalSpells || archive['scope'] == 'loadout')) {
      for (final row in _rows(archive['cards'], 512)) {
        final number = _count(row['number'], 512);
        if (number == null || number == 0) continue;
        final totals = row['total'];
        final loadouts = <SpellKey, SpellTally>{};
        for (final entry in _rows(row['loadouts'], 128)) {
          final character = _count(entry['character'], 63);
          final shot = _count(entry['shot'], 63);
          final difficulty = entry['difficulty'];
          final activity = switch (entry['activity']) {
            'story' => SpellActivity.story,
            'practice' => SpellActivity.practice,
            _ => null,
          };
          final tally = _tally(
            entry['attempts'],
            entry['captures'],
            2147483647,
          );
          if (character == null ||
              difficulty is! String ||
              (_level(difficulty) == null && difficulty != 'last-word') ||
              (entry['activity'] != null && activity == null) ||
              tally == null ||
              entry['shot'] != null && shot == null) {
            continue;
          }
          loadouts[(
                character: character,
                shot: shot,
                difficulty: difficulty,
                activity: activity,
              )] =
              tally;
        }
        legacy[number] = SpellRecord(
          name: _cardName(row['name']) ?? legacy[number]?.name,
          total: totals is Map
              ? _tally(totals['attempts'], totals['captures'], 2147483647)
              : null,
          loadouts: loadouts,
          legacy: legacy[number]?.legacy ?? const [],
        );
      }
    }
    return SaveStatistics(
      game: json['game'] as String,
      progress: readSaveProgress(json['progress'], loadouts: json['loadouts']),
      playtime: playtimeSeconds(json['playtimeSeconds']),
      playCount: _count(json['playCount']),
      warnings: json['warnings'] is List
          ? (json['warnings'] as List)
                .whereType<String>()
                .take(6)
                .toList(growable: false)
          : const [],
      spells: Map.unmodifiable(legacy),
      hasSpellData: cards is List || archive is Map,
      totalSpells: totalSpells,
    );
  }

  final String game;
  final List<SaveProgress> progress;
  final num? playtime;
  final int? playCount;
  final List<String> warnings;
  final Map<int, SpellRecord> spells;
  final bool hasSpellData;
  final bool totalSpells;
}

typedef CloudGameSave = ({
  int revision,
  DateTime uploadedAt,
  SaveStatistics statistics,
});
