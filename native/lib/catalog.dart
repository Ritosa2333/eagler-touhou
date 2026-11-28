import 'dart:convert';

import 'package:material_ui/material_ui.dart';
import 'package:flutter/services.dart';
import 'package:shared_preferences/shared_preferences.dart';

import 'l10n/strings.dart';
import 'launcher_background.dart';
import 'room_phrases.dart';

enum BookCategory {
  all(Icons.grid_view_outlined),
  favorites(Icons.star_outline),
  windows(Icons.desktop_windows_outlined),
  versus(Icons.sports_martial_arts_outlined),
  fighting(Icons.sports_esports_outlined),
  spinOff(Icons.extension_outlined),
  pc98(Icons.album_outlined),
  doujin(Icons.category_outlined);

  const BookCategory(this.icon);
  final IconData icon;

  static const multiplayerGroups = [all, versus, windows, fighting, pc98];

  String label(LauncherStrings strings, {bool multiplayer = false}) =>
      switch (this) {
        all => multiplayer ? strings.multiplayerAll : strings.allGames,
        favorites => strings.favorites,
        windows =>
          multiplayer ? strings.multiplayerOriginal : strings.mainlineGames,
        versus => multiplayer ? strings.multiplayerVersus : strings.versusGames,
        fighting =>
          multiplayer ? strings.multiplayerFighting : strings.fightingGames,
        spinOff => strings.spinOffs,
        pc98 => strings.classicGames,
        doujin => strings.fanGames,
      };
}

enum GameOrder {
  number,
  reverse,
  name;

  String label(LauncherStrings strings) => switch (this) {
    number => strings.sortNumber,
    reverse => strings.sortReverse,
    name => strings.sortName,
  };
}

typedef GameData = ({String name, int bytes, String sha256});

typedef GameLanguage = ({String id, String title, int bytes});

class ReplayCatalog {
  const ReplayCatalog({
    required this.namespace,
    required this.prefix,
    this.slotPrefix,
    this.slots = 0,
    this.filename = false,
    this.sharedExtensionSlot = false,
    this.legacyPrefix,
    this.extensions = const ['rpy', 'rpyx'],
  });

  factory ReplayCatalog.fromJson(Map<String, dynamic> row) => ReplayCatalog(
    namespace: row['namespace'] as String,
    prefix: row['prefix'] as String,
    slotPrefix: row['slotPrefix'] as String?,
    slots: row['slots'] as int,
    filename: row['filename'] as bool,
    sharedExtensionSlot: row['sharedExtensionSlot'] as bool,
    legacyPrefix: row['legacyPrefix'] as String?,
    extensions: List<String>.from(row['extensions'] as List),
  );

  final String namespace;
  final String prefix;
  final String? slotPrefix;
  final int slots;
  final bool filename;
  final bool sharedExtensionSlot;
  final String? legacyPrefix;
  final List<String> extensions;
}

class GameFilesCatalog {
  const GameFilesCatalog({
    required this.score,
    required this.normal,
    this.multiplayer,
    this.hints = const [],
    this.saveArchive = false,
    this.languageScoped = false,
  });

  factory GameFilesCatalog.fromJson(Map<String, dynamic> row) =>
      GameFilesCatalog(
        score: row['score'] as String,
        normal: ReplayCatalog.fromJson(row['normal'] as Map<String, dynamic>),
        multiplayer: row['multiplayer'] == null
            ? null
            : ReplayCatalog.fromJson(
                row['multiplayer'] as Map<String, dynamic>,
              ),
        hints: List<String>.from(row['hints'] as List),
        saveArchive: row['saveArchive'] as bool,
        languageScoped: row['languageScoped'] as bool,
      );

  final String score;
  final ReplayCatalog normal;
  final ReplayCatalog? multiplayer;
  final List<String> hints;
  final bool saveArchive;
  final bool languageScoped;
}

class GameSettingsCatalog {
  const GameSettingsCatalog({
    this.highRefresh = false,
    this.features = const [],
    this.languages = const [],
    this.renderScales = const [],
    this.music = const ['none'],
  });

  factory GameSettingsCatalog.fromJson(Map<String, dynamic> data) =>
      GameSettingsCatalog(
        highRefresh: data['highRefresh'] as bool,
        features: List<String>.from(data['features'] as List),
        languages: [
          for (final row in data['languages'] as List)
            (
              id: row['id'] as String,
              title: row['title'] as String,
              bytes: row['bytes'] as int,
            ),
        ],
        renderScales: List<int>.from(data['renderScales'] as List),
        music: List<String>.from(data['music'] as List),
      );

  final bool highRefresh;
  final List<String> features;
  final List<GameLanguage> languages;
  final List<int> renderScales;
  final List<String> music;
}

typedef RoomTimingPolicy = ({
  bool rollback,
  bool measuredStartup,
  bool runtimeTiming,
  int manualDelayLimit,
});

class MultiplayerRules {
  const MultiplayerRules({
    required this.product,
    required this.playerCounts,
    required this.difficulties,
    this.selectInGame = false,
    this.challengeMode = false,
    this.timing,
  });

  factory MultiplayerRules.fromJson(Map<String, dynamic> row) {
    final timing = row['timing'];
    return MultiplayerRules(
      product: row['product'] as String,
      selectInGame: row['selectInGame'] == true,
      challengeMode: row['challengeMode'] == true,
      timing:
          timing is Map<String, dynamic> && timing['manualDelayLimit'] is int
          ? (
              rollback: timing['rollback'] == true,
              measuredStartup: timing['measuredStartup'] == true,
              runtimeTiming: timing['runtimeTiming'] == true,
              manualDelayLimit: timing['manualDelayLimit'] as int,
            )
          : null,
      playerCounts: List.unmodifiable(
        (row['playerCounts'] as List).cast<int>(),
      ),
      difficulties: List.unmodifiable(
        (row['difficulties'] as List).cast<String>(),
      ),
    );
  }

  final String product;
  final List<int> playerCounts;
  final List<String> difficulties;
  final bool selectInGame;
  final bool challengeMode;
  final RoomTimingPolicy? timing;
}

class Game {
  const Game({
    required this.id,
    required this.number,
    required this.title,
    required this.subtitle,
    required this.category,
    required this.nameOrder,
    required this.multiplayer,
    this.artwork,
    this.shortTitle = '',
    this.multiplayerRules,
    this.data,
    this.packageMaxBytes = 256 * 1024 * 1024,
    this.files,
    this.settings = const GameSettingsCatalog(),
  });

  factory Game.fromJson(Map<String, dynamic> row) => Game(
    id: row['id'] as String,
    number: row['number'] as String,
    title: row['title'] as String,
    subtitle: row['subtitle'] as String,
    category: row['category'] == 'spin-off'
        ? BookCategory.spinOff
        : BookCategory.values.byName(row['category'] as String),
    nameOrder: row['nameOrder'] as int,
    multiplayer: row['multiplayer'] as bool,
    shortTitle: row['shortTitle'] as String? ?? '',
    multiplayerRules: row['multiplayerRules'] is Map<String, dynamic>
        ? MultiplayerRules.fromJson(
            row['multiplayerRules'] as Map<String, dynamic>,
          )
        : null,
    artwork: row['artwork'] as String?,
    data: (
      name: row['data']['name'] as String,
      bytes: row['data']['bytes'] as int,
      sha256: row['data']['sha256'] as String,
    ),
    packageMaxBytes: row['packageMaxBytes'] as int? ?? 256 * 1024 * 1024,
    files: GameFilesCatalog.fromJson(row['files'] as Map<String, dynamic>),
    settings: GameSettingsCatalog.fromJson(
      row['settings'] as Map<String, dynamic>,
    ),
  );

  final String id;
  final String number;
  final String title;
  final String subtitle;
  final BookCategory category;
  final int nameOrder;
  final bool multiplayer;
  final String shortTitle;
  final MultiplayerRules? multiplayerRules;
  final String? artwork;

  String get multiplayerProduct => multiplayerRules?.product ?? '${id}mp';
  final GameData? data;
  final int packageMaxBytes;
  final GameFilesCatalog? files;
  final GameSettingsCatalog settings;

  String get displayCode => double.tryParse(number) == null ? id : 'th$number';

  bool matches(String query) => '$id th$number $title $subtitle'
      .toLowerCase()
      .contains(query.trim().toLowerCase());

  static Future<List<Game>> load() async {
    final catalog = await rootBundle.loadString('assets/game_catalog.json');
    return (jsonDecode(catalog) as List<dynamic>)
        .map((row) => Game.fromJson(row as Map<String, dynamic>))
        .toList(growable: false);
  }
}

enum LauncherLanguage {
  simplifiedChinese('zh-CN', '简体中文', Locale('zh')),
  english('en', 'English', Locale('en')),
  japanese('ja', '日本語', Locale('ja'));

  const LauncherLanguage(this.tag, this.label, this.locale);

  final String tag;
  final String label;
  final Locale locale;
}

class LauncherPreferences extends ChangeNotifier {
  LauncherPreferences(
    this._storage,
    this.theme,
    this.lessMotion, {
    this.language = LauncherLanguage.simplifiedChinese,
    LauncherBackground? background,
    int quickChatVolume = 100,
    List<String>? quickChatSlots,
  }) : background = background ?? LauncherBackground(_storage),
       _quickChatVolume = quickChatVolume.clamp(0, 100),
       _quickChatSlots = List.unmodifiable(
         quickChatSlots?.length == quickChatSlotCount &&
                 quickChatSlots!.every(roomPhrases.containsKey)
             ? quickChatSlots
             : roomPhrases.keys.take(quickChatSlotCount),
       );

  static const _languageKey = 'launcher.language';
  static const _volumeKey = 'launcher.quick-chat-volume';
  static const _slotsKey = 'launcher.quick-chat-slots';
  static const quickChatSlotCount = 9;
  final SharedPreferencesAsync _storage;
  final LauncherBackground background;
  ThemeMode theme;
  bool lessMotion;
  LauncherLanguage language;
  int _quickChatVolume;
  List<String> _quickChatSlots;

  int get quickChatVolume => _quickChatVolume;
  List<String> get quickChatSlots => _quickChatSlots;

  static Future<LauncherPreferences> load(
    SharedPreferencesAsync storage,
  ) async {
    final storedTheme = await storage.getString('launcher.theme');
    final storedLanguage = await storage.getString(_languageKey);
    final theme = switch (storedTheme) {
      'light' => ThemeMode.light,
      'dark' => ThemeMode.dark,
      _ => ThemeMode.system,
    };
    final background = LauncherBackground(storage);
    await background.restore();
    return LauncherPreferences(
      storage,
      theme,
      await storage.getBool('launcher.less-motion') ?? false,
      language: LauncherLanguage.values.firstWhere(
        (entry) => entry.tag == storedLanguage,
        orElse: () => LauncherLanguage.simplifiedChinese,
      ),
      background: background,
      quickChatVolume: await storage.getInt(_volumeKey) ?? 100,
      quickChatSlots: await storage.getStringList(_slotsKey),
    );
  }

  Future<void> setLanguage(LauncherLanguage value) async {
    if (language == value) return;
    await _storage.setString(_languageKey, value.tag);
    language = value;
    notifyListeners();
  }

  Future<void> setTheme(ThemeMode value) async {
    if (theme == value) return;
    theme = value;
    notifyListeners();
    await _storage.setString('launcher.theme', value.name);
  }

  Future<void> setLessMotion(bool value) async {
    if (lessMotion == value) return;
    lessMotion = value;
    notifyListeners();
    await _storage.setBool('launcher.less-motion', value);
  }

  Future<void> setQuickChatVolume(int value) async {
    value = value.clamp(0, 100);
    if (value == _quickChatVolume) return;
    await _storage.setInt(_volumeKey, value);
    _quickChatVolume = value;
    notifyListeners();
  }

  Future<void> setQuickChatSlot(int index, String phrase) async {
    if (index < 0 ||
        index >= quickChatSlotCount ||
        !roomPhrases.containsKey(phrase) ||
        _quickChatSlots[index] == phrase) {
      return;
    }
    final updated = [..._quickChatSlots]..[index] = phrase;
    await _storage.setStringList(_slotsKey, updated);
    _quickChatSlots = List.unmodifiable(updated);
    notifyListeners();
  }
}

class GameFavorites extends ChangeNotifier {
  GameFavorites(this._storage, this.ids);
  final SharedPreferencesAsync _storage;
  final Set<String> ids;

  static Future<GameFavorites> load(SharedPreferencesAsync storage) async =>
      GameFavorites(
        storage,
        (await storage.getStringList('library.favorites'))?.toSet() ?? {},
      );

  Future<void> toggle(String id) async {
    if (!ids.remove(id)) ids.add(id);
    notifyListeners();
    await _storage.setStringList('library.favorites', ids.toList());
  }
}

class GamePlayHistory extends ChangeNotifier {
  GamePlayHistory._(this._storage, this._dates);

  static const _key = 'library.play-history';
  static const _limit = 20;
  static const _maxMillis = 8640000000000000;

  final SharedPreferencesAsync _storage;
  Map<String, DateTime> _dates;
  Future<void> _saving = Future.value();

  Iterable<String> get recentIds => _dates.keys;

  DateTime? lastPlayed(String id) => _dates[id];

  static Future<GamePlayHistory> load(SharedPreferencesAsync storage) async {
    final dates = <String, DateTime>{};
    try {
      final saved = jsonDecode(await storage.getString(_key) ?? '{}');
      if (saved is Map<String, dynamic>) {
        for (final MapEntry(:key, :value) in saved.entries) {
          if (value is int && value > 0 && value <= _maxMillis) {
            dates[key] = DateTime.fromMillisecondsSinceEpoch(
              value,
              isUtc: true,
            );
            if (dates.length >= _limit) break;
          }
        }
      }
    } on Exception catch (error) {
      debugPrint('Could not read play history: $error');
    }
    return GamePlayHistory._(storage, dates);
  }

  Future<void> record(String id, {DateTime? playedAt}) {
    _dates = {
      id: playedAt ?? DateTime.now(),
      for (final entry
          in _dates.entries.where((e) => e.key != id).take(_limit - 1))
        entry.key: entry.value,
    };
    final value = jsonEncode(
      _dates.map((id, date) => MapEntry(id, date.millisecondsSinceEpoch)),
    );
    _saving = _saving.then((_) async {
      try {
        await _storage.setString(_key, value);
      } on Exception catch (error) {
        debugPrint('Could not save play history: $error');
      }
    });
    notifyListeners();
    return _saving;
  }
}

class BookState extends ChangeNotifier {
  BookState(this.games, this.favorites, {this.history}) {
    favorites.addListener(notifyListeners);
  }
  final List<Game> games;
  final GameFavorites favorites;
  final GamePlayHistory? history;
  final searchController = TextEditingController();
  final expanded = {BookCategory.windows, BookCategory.versus};
  BookCategory category = BookCategory.all;
  GameOrder order = GameOrder.number;
  bool covers = true;

  bool isFavorite(Game game) => favorites.ids.contains(game.id);

  List<Game> get recentGames {
    final byId = {for (final game in games) game.id: game};
    return [
      for (final id in history?.recentIds ?? const <String>[])
        if (byId[id] case final Game game) game,
    ];
  }

  List<Game> filter(
    BookCategory group, {
    bool applySearch = false,
    bool multiplayer = false,
  }) {
    final result = games
        .where(
          (game) =>
              (!multiplayer || game.multiplayer) &&
              (group == BookCategory.all ||
                  (group == BookCategory.favorites
                      ? isFavorite(game)
                      : game.category == group ||
                            (multiplayer &&
                                group == BookCategory.windows &&
                                game.category == BookCategory.spinOff))) &&
              (!applySearch || game.matches(searchController.text)),
        )
        .toList();
    result.sort((a, b) {
      if (group == BookCategory.all && isFavorite(a) != isFavorite(b)) {
        return isFavorite(a) ? -1 : 1;
      }
      if (order == GameOrder.name) return a.nameOrder.compareTo(b.nameOrder);
      final aNumber = double.tryParse(a.number),
          bNumber = double.tryParse(b.number);
      if (aNumber == null) return bNumber == null ? a.id.compareTo(b.id) : 1;
      if (bNumber == null) return -1;
      final comparison = aNumber.compareTo(bNumber);
      return order == GameOrder.reverse ? -comparison : comparison;
    });
    return result;
  }

  void searchChanged(String _) => notifyListeners();

  void select(BookCategory value) {
    if (category == value) return;
    category = value;
    notifyListeners();
  }

  void toggleExpanded(BookCategory value) {
    if (!expanded.remove(value)) expanded.add(value);
    notifyListeners();
  }

  void changeOrder(GameOrder value) {
    if (order == value) return;
    order = value;
    notifyListeners();
  }

  void changeView(bool value) {
    if (covers == value) return;
    covers = value;
    notifyListeners();
  }

  Future<void> toggleFavorite(Game game) => favorites.toggle(game.id);

  @override
  void dispose() {
    favorites.removeListener(notifyListeners);
    searchController.dispose();
    super.dispose();
  }
}
