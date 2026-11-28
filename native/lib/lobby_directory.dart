import 'dart:async';
import 'dart:convert';
import 'dart:io';

import 'package:flutter/foundation.dart';
import 'package:shared_preferences/shared_preferences.dart';
import 'package:uuid/uuid.dart';

import 'catalog.dart';
import 'site_manifest.dart';
import 'l10n/strings.dart';
import 'network_diagnostics.dart';

enum LobbyConnection { loading, live, offline, unconfigured }

enum LobbyFailure {
  loadFailed,
  unavailable,
  disconnected,
  timeout;

  String label(LauncherStrings strings) => switch (this) {
    loadFailed => strings.lobbyLoadFailed,
    unavailable => strings.lobbyOffline,
    disconnected => strings.lobbyDisconnected,
    timeout => strings.roomRequestTimeout,
  };
}

enum LobbyRoomFilter {
  all,
  open,
  waiting,
  spectate;

  String label(LauncherStrings strings) => switch (this) {
    all => strings.allRooms,
    open => strings.openRooms,
    waiting => strings.waitingRooms,
    spectate => strings.spectateRooms,
  };

  bool includes(LobbyRoom room) => switch (this) {
    all => true,
    open => room.canJoin,
    waiting => !room.playing,
    spectate => room.watchEntry,
  };
}

bool validRoomCode(String code) => RegExp(r'^[0-9]{4,8}$').hasMatch(code);

enum RoomMovement {
  normal,
  touch,
  cheat;

  static RoomMovement? parse(Object? value) => switch (value) {
    'normal' => normal,
    'touch' => touch,
    'cheat' => cheat,
    _ => null,
  };

  String label(LauncherStrings strings) => switch (this) {
    normal => strings.roomMovementNormal,
    touch => strings.roomMovementTouch,
    cheat => strings.roomMovementCheat,
  };
}

enum LobbyRule {
  limitMovement,
  challenge,
  noFocus,
  unrated;

  String label(LauncherStrings strings) => switch (this) {
    limitMovement => strings.roomLimitMovement,
    challenge => strings.roomChallenge,
    noFocus => strings.roomDisableFocus,
    unrated => strings.roomUnrated,
  };
}

typedef LobbySeat = ({
  String name,
  String? account,
  bool online,
  bool ready,
  RoomMovement? control,
  int wins,
  int? ratingGames,
});

class LobbyRoom {
  const LobbyRoom({
    required this.game,
    required this.code,
    required this.capacity,
    required this.difficulty,
    required this.playing,
    required this.joinable,
    required this.liveSpectator,
    required this.spectators,
    required this.seats,
    required this.rules,
    this.ballCount,
    this.createdAt,
    this.startedAt,
    this.track,
  });

  final Game game;
  final String code;
  final int capacity;
  final String? difficulty;
  final bool playing;
  final bool joinable;
  final bool liveSpectator;
  final int spectators;
  final List<LobbySeat?> seats;
  final List<LobbyRule> rules;
  final int? ballCount;
  final DateTime? createdAt;
  final DateTime? startedAt;
  final String? track;

  String get id => '${game.id}-$code';
  int get occupied => seats.whereType<LobbySeat>().length;
  bool get watchEntry => playing || occupied >= capacity;
  bool get canJoin => !watchEntry && joinable;

  static LobbyRoom? read(Object? value, Map<String, Game> games) {
    if (value is! Map<String, dynamic>) return null;
    final game = games[value['product']];
    final code = value['code'];
    final capacity = value['capacity'];
    if (game == null ||
        code is! String ||
        !validRoomCode(code) ||
        capacity is! int ||
        !(game.multiplayerRules?.playerCounts ?? const [2, 3]).contains(
          capacity,
        )) {
      return null;
    }
    final rawSeats = value['seats'] ?? value['initials'];
    final seats = rawSeats is List ? rawSeats : const [];
    final allowedNames = value['publicNames'] == true;
    final parsedSeats = List<LobbySeat?>.generate(capacity, (index) {
      final entry = index < seats.length ? seats[index] : null;
      if (entry == null) return null;
      final seat = entry is Map<String, dynamic> ? entry : null;
      final name = allowedNames ? _clean(seat?['name'], 12) : '';
      final initial = _clean(seat?['initial'] ?? entry, 1);
      final profile = seat?['profile'];
      final username = profile is Map<String, dynamic>
          ? profile['username']
          : null;
      final stats = seat?['stats'];
      final duelProfile = profile is Map ? profile['th09'] : null;
      return (
        name: name.isNotEmpty
            ? name
            : initial.isEmpty
            ? '?'
            : initial,
        account: username is String && _username.hasMatch(username)
            ? username
            : null,
        online: seat?['online'] != false,
        ready: seat?['ready'] == true,
        control: RoomMovement.parse(seat?['controlMode']),
        wins: stats is Map ? _bounded(stats['wins'], 999999) : 0,
        ratingGames: switch (duelProfile) {
          {'games': final int count} when count >= 0 => count,
          _ => null,
        },
      );
    }, growable: false);
    final spectators = _bounded(value['spectators'], 999);
    if (spectators == 0 && parsedSeats.every((seat) => seat == null)) {
      return null;
    }
    final difficulties =
        game.multiplayerRules?.difficulties ??
        const ['Easy', 'Normal', 'Hard', 'Lunatic', 'Extra'];
    final track = value['listeningTrack'];
    final title = track is Map ? _clean(track['name'], 64) : '';
    final artist = track is Map ? _clean(track['artist'], 64) : '';
    return LobbyRoom(
      game: game,
      code: code,
      capacity: capacity,
      difficulty:
          difficulties.isEmpty ||
              game.multiplayerRules?.selectInGame == true && game.id != 'th03'
          ? null
          : difficulties[_bounded(
              value['difficulty'],
              difficulties.length - 1,
            )],
      playing: value['phase'] != 'lobby',
      joinable: value['joinable'] == true,
      liveSpectator: value['spectatable'] == true,
      spectators: spectators,
      seats: List.unmodifiable(parsedSeats),
      rules: [
        if (value['disableCheatMovement'] == true) LobbyRule.limitMovement,
        if (value['challengeMode'] == true) LobbyRule.challenge,
        if (game.id == 'th02' && value['disableFocus'] == true)
          LobbyRule.noFocus,
        if (game.id == 'th09' && value['unrated'] == true) LobbyRule.unrated,
      ],
      ballCount: game.id == 'th01' ? _bounded(value['ballCount'], 9) : null,
      createdAt: _time(value['createdAt']),
      startedAt: _time(value['startedAt']),
      track: title.isEmpty
          ? null
          : (artist.isEmpty ? title : '$title | $artist'),
    );
  }
}

final _username = RegExp(r'^[A-Za-z0-9_.]{3,12}$');
final _invisible = RegExp(
  r'[\u0000-\u001f\u007f-\u009f\u061c\u200b-\u200f\u202a-\u202e\u2060-\u206f\ufeff]',
);
String _clean(Object? text, int length) => text is String
    ? String.fromCharCodes(
        text.replaceAll(_invisible, '').trim().runes.take(length),
      )
    : '';
int _bounded(Object? value, int max) => value is int ? value.clamp(0, max) : 0;
DateTime? _time(Object? value) =>
    value is int && value > 0 && value < 4133980800000
    ? DateTime.fromMillisecondsSinceEpoch(value)
    : null;

class LobbySelection extends ValueNotifier<Set<String>> {
  LobbySelection() : super(const {});

  bool includes(Game game) => value.isEmpty || value.contains(game.id);

  void showOnly(Game game) => _set({game.id});
  void clear() => _set({});

  void toggle(Game game) {
    final next = {...value};
    if (!next.remove(game.id)) next.add(game.id);
    _set(next);
  }

  void _set(Set<String> games) {
    if (!setEquals(value, games)) value = Set.unmodifiable(games);
  }
}

typedef RoomRelay = ({
  Uri endpoint,
  Uri origin,
  String member,
  Map<String, Game> products,
});

enum LobbyReleaseResult { released, changed, unconfirmed }

typedef _LobbyMembership = ({String product, String code, String? token});

final _memberIdFormat = RegExp(r'^[A-Za-z0-9_-]{8,64}$');
final _multiplayerProductFormat = RegExp(r'^th[0-9]{2,4}mp$');

class LobbyDirectory extends ChangeNotifier {
  LobbyDirectory(Iterable<Game> games, {Uri? origin, this.preferences})
    : origin = origin ?? Uri.https('touhouonline.com'),
      _catalog = {
        for (final game in games)
          if (game.multiplayer) game.multiplayerProduct: game,
      };

  final Uri origin;
  final SharedPreferencesAsync? preferences;
  final Map<String, Game> _catalog;
  late final Future<String> _storedMember = _readMember();
  String? _member;
  late final _http = HttpClient()
    ..connectionTimeout = const Duration(seconds: 8);
  static const _readLimit = Duration(seconds: 12);
  // The Relay accepts one directory refresh per 150 ms on a socket.
  static const _refreshDelay = Duration(milliseconds: 180);
  static const _pageLimit = 200;
  WebSocket? _socket;
  StreamSubscription<dynamic>? _messages;
  Timer? _deadline;
  Timer? _retry;
  Timer? _scheduledRefresh;
  bool _started = false;
  bool _disposed = false;
  int _epoch = 0;
  int _failures = 0;
  int _relayIndex = 0;
  List<Uri> _relays = const [];
  late Map<String, Game> _games = _catalog;
  LobbyConnection _connection = LobbyConnection.loading;
  List<LobbyRoom> _rooms = const [];
  bool _refreshing = false;
  bool _allGamesCapped = false;
  String? _filteredGame;
  String _loadedProduct = '';
  String? _pendingProduct;
  String _publishedProduct = '';
  int _total = 0;
  bool _globalRoomCodes = false;
  LobbyFailure? _failure;
  _LobbyMembership? _membership;
  bool _supportsMembershipRelease = false;
  _LobbyMembership? _releasingMembership;
  Completer<LobbyReleaseResult>? _releaseReply;
  Timer? _releaseDeadline;

  LobbyConnection get connection => _connection;
  List<LobbyRoom> get rooms => _rooms;
  bool get refreshing => _refreshing;
  bool get globalRoomCodes => _globalRoomCodes;
  int get total => _total;
  String get publishedProduct => _publishedProduct;
  String? get occupiedRoomCode => _membership?.code;
  bool get canReleaseMembership =>
      _connection == LobbyConnection.live &&
      supportsMembershipRelease &&
      !releasingMembership;
  bool get supportsMembershipRelease =>
      _supportsMembershipRelease && _membership?.token != null;
  bool get releasingMembership => _releaseReply != null;
  late final availableGameIds = ValueNotifier<Set<String>>(
    Set.unmodifiable(_catalog.values.map((game) => game.id)),
  );
  LobbyFailure? get failure => _failure;

  NetworkDiagnostics diagnostics() => NetworkDiagnostics(
    endpoint: _relays.isEmpty
        ? roomRelay?.endpoint
        : _relays[_relayIndex % _relays.length],
    origin: origin,
  );

  RoomRelay? get roomRelay =>
      _connection == LobbyConnection.live && _relays.isNotEmpty
      ? (
          endpoint: _relays[_relayIndex % _relays.length],
          origin: origin,
          member: _member!,
          products: _games,
        )
      : null;

  void start() {
    if (_started || _disposed) return;
    _started = true;
    unawaited(_initialize());
  }

  Future<String> _readMember() async {
    final storage = preferences;
    final created = const Uuid().v4();
    if (storage == null) return created;
    final key = 'lobby.memberId.${origin.origin}';
    try {
      final saved = await storage.getString(key);
      if (saved != null && _memberIdFormat.hasMatch(saved)) return saved;
      await storage.setString(key, created);
    } on Exception {
      // The current launch can still use its in-memory identity.
    }
    return created;
  }

  Future<LobbyReleaseResult> releaseMembership() async {
    final socket = _socket;
    final membership = _membership;
    if (!canReleaseMembership || socket == null || membership == null) {
      return LobbyReleaseResult.unconfirmed;
    }
    final reply = _releaseReply = Completer<LobbyReleaseResult>();
    _releasingMembership = membership;
    _releaseDeadline = Timer(const Duration(seconds: 8), () {
      _finishRelease(LobbyReleaseResult.unconfirmed);
      notifyListeners();
    });
    try {
      socket.add(
        jsonEncode({
          'type': 'release-membership',
          'recoveryToken': membership.token,
        }),
      );
    } on Object {
      _finishRelease(LobbyReleaseResult.unconfirmed);
    }
    notifyListeners();
    return reply.future;
  }

  void _finishRelease(LobbyReleaseResult result) {
    _releaseDeadline?.cancel();
    _releaseDeadline = null;
    _releasingMembership = null;
    final reply = _releaseReply;
    _releaseReply = null;
    reply?.complete(result);
  }

  bool _receiveMembership(Map<String, dynamic> directory) {
    final previous = (_membership, _supportsMembershipRelease);
    final active = directory['mine'];
    _membership = switch (active) {
      {'product': final String product, 'code': final String code}
          when _multiplayerProductFormat.hasMatch(product) &&
              validRoomCode(code) =>
        (
          product: product,
          code: code,
          token: switch (active['recoveryToken']) {
            final String token when _memberIdFormat.hasMatch(token) => token,
            _ => null,
          },
        ),
      _ => null,
    };
    _supportsMembershipRelease = directory['membershipRecovery'] == true;
    final requested = _releasingMembership;
    if (requested != null && _membership != requested) {
      _finishRelease(
        _membership == null
            ? LobbyReleaseResult.released
            : LobbyReleaseResult.changed,
      );
    }
    return previous != (_membership, _supportsMembershipRelease);
  }

  String get _requestedProduct => _allGamesCapped
      ? _games.entries
                .where((entry) => entry.value.id == _filteredGame)
                .firstOrNull
                ?.key ??
            ''
      : '';

  void filterGame(String? gameId) {
    if (_disposed || _filteredGame == gameId) return;
    _filteredGame = gameId;
    if (_connection == LobbyConnection.live &&
        !_refreshing &&
        _requestedProduct != _loadedProduct) {
      _queueRefresh();
      notifyListeners();
    }
  }

  void refresh() {
    if (_disposed || _refreshing) return;
    if (_connection != LobbyConnection.live || _socket == null) {
      if (_connection != LobbyConnection.loading) unawaited(_initialize());
      return;
    }
    _queueRefresh();
    notifyListeners();
  }

  void _queueRefresh() {
    _refreshing = true;
    final epoch = _epoch;
    _scheduledRefresh = Timer(_refreshDelay, () {
      _scheduledRefresh = null;
      final socket = _socket;
      if (!_current(epoch) || socket == null) return;
      final product = _pendingProduct = _requestedProduct;
      _awaitDirectory(epoch);
      try {
        socket.add(jsonEncode({'type': 'refresh', 'product': product}));
      } on Object {
        _interrupted(epoch, LobbyFailure.loadFailed);
      }
    });
  }

  Future<void> _initialize() async {
    _disconnect();
    final epoch = ++_epoch;
    _connection = LobbyConnection.loading;
    _failure = null;
    notifyListeners();
    try {
      final data = await readSiteManifest(origin, _http, _readLimit);
      if (!_current(epoch)) return;
      final published = data['games'] as Map<String, dynamic>;
      final shared = data['shared'] as Map<String, dynamic>;
      _games = Map.unmodifiable({
        for (final entry in _catalog.entries)
          if (published[entry.value.id]
              case {'multiplayerRuntime': final String path}
              when path.isNotEmpty)
            entry.key: entry.value,
      });
      final available = _games.values.map((game) => game.id).toSet();
      if (!setEquals(availableGameIds.value, available)) {
        availableGameIds.value = Set.unmodifiable(available);
      }
      final spare = shared['netplayRelayFallbacks'];
      _relays = {
        for (final value in [
          shared['netplayRelay'],
          if (spare is List) ...spare.take(3),
        ])
          if (_relay(value) case final Uri relay) relay,
      }.toList();
      _relayIndex = 0;
      if (_relays.isEmpty || _games.isEmpty) {
        _rooms = const [];
        _total = 0;
        _publishedProduct = '';
        _membership = null;
        _connection = LobbyConnection.unconfigured;
        notifyListeners();
      } else {
        await _connect(epoch);
      }
    } on Object {
      _interrupted(epoch, LobbyFailure.loadFailed);
    }
  }

  Uri? _relay(Object? raw) {
    final address = raw is String ? Uri.tryParse(raw) : null;
    if (address == null ||
        address.host.isEmpty ||
        address.userInfo.isNotEmpty) {
      return null;
    }
    if (address.scheme == 'wss' ||
        address.scheme == 'ws' &&
            _loopback(address.host) &&
            _loopback(origin.host)) {
      return address;
    }
    return null;
  }

  bool _loopback(String host) =>
      const ['localhost', '127.0.0.1', '::1'].contains(host);

  bool _current(int epoch) => !_disposed && epoch == _epoch;

  Future<void> _connect(int epoch) async {
    if (!_current(epoch)) return;
    final member = _member ??= await _storedMember;
    if (!_current(epoch)) return;
    _allGamesCapped = false;
    _loadedProduct = '';
    final relay = _relays[_relayIndex % _relays.length];
    final url = relay.replace(
      queryParameters:
          {...relay.queryParameters, 'directory': '1', 'member': member}
            ..removeWhere(
              (key, _) => const [
                'room',
                'run',
                'lobby',
                'player',
                'signal',
                'spectator',
                'diagnostic',
              ].contains(key),
            ),
      fragment: '',
    );
    final pending = WebSocket.connect(
      url.toString(),
      headers: {'Origin': origin.origin},
      customClient: _http,
    );
    try {
      final socket = await pending.timeout(_readLimit);
      if (!_current(epoch)) {
        await socket.close();
        return;
      }
      _socket = socket;
      _awaitDirectory(epoch);
      _messages = socket.listen(
        (event) => _receive(epoch, event),
        onError: (Object _) => _interrupted(epoch, LobbyFailure.disconnected),
        onDone: () => _interrupted(epoch, LobbyFailure.disconnected),
        cancelOnError: true,
      );
    } on TimeoutException {
      pending.then((socket) => socket.close()).ignore();
      _interrupted(epoch, LobbyFailure.timeout);
    } on Object {
      _interrupted(epoch, LobbyFailure.unavailable);
    }
  }

  void _awaitDirectory(int epoch) {
    _deadline?.cancel();
    _deadline = Timer(
      _readLimit,
      () => _interrupted(epoch, LobbyFailure.timeout),
    );
  }

  void _receive(int epoch, Object? event) {
    if (!_current(epoch) ||
        event is! String ||
        event.length > 2 * 1024 * 1024) {
      return;
    }
    final Object? decoded;
    try {
      decoded = jsonDecode(event);
    } on FormatException {
      return;
    }
    if (decoded is! Map<String, dynamic> ||
        decoded['type'] != 'directory' ||
        decoded['version'] != 1 ||
        decoded['rooms'] is! List) {
      return;
    }
    final pending = _pendingProduct;
    final product = decoded['product'] is String
        ? decoded['product'] as String
        : pending ?? _loadedProduct;
    final membershipChanged = _receiveMembership(decoded);
    if (pending != null ? product != pending : product != _loadedProduct) {
      if (membershipChanged) notifyListeners();
      return;
    }
    if (pending != null) _pendingProduct = null;
    final rows = decoded['rooms'] as List;
    final reportedTotal = _bounded(decoded['total'], 100000);
    final capped = reportedTotal > rows.length || rows.length > _pageLimit;
    if (product.isEmpty) _allGamesCapped = capped;
    _loadedProduct = product;
    // Keep the last applicable rows while a new filter is in flight. A
    // late result for the previous filter never replaces the current list.
    if (product == _requestedProduct ||
        (product.isEmpty && _publishedProduct != _requestedProduct)) {
      _publishRooms(rows, product, capped ? reportedTotal : 0);
    }
    _globalRoomCodes = decoded['globalRoomCodes'] == true;
    _connection = LobbyConnection.live;
    _failure = null;
    _deadline?.cancel();
    _failures = 0;
    _refreshing = _scheduledRefresh != null;
    if (!_refreshing && product != _requestedProduct) _queueRefresh();
    notifyListeners();
  }

  void _publishRooms(List<dynamic> rows, String product, int reportedTotal) {
    final entries = <String, LobbyRoom>{};
    for (final row in rows.take(_pageLimit)) {
      if (LobbyRoom.read(row, _games) case final LobbyRoom room
          when product.isEmpty || room.game.multiplayerProduct == product) {
        entries[room.id] = room;
      }
    }
    _rooms = List.unmodifiable(entries.values);
    _publishedProduct = product;
    _total = reportedTotal > _rooms.length ? reportedTotal : _rooms.length;
  }

  void _interrupted(int epoch, LobbyFailure problem) {
    if (!_current(epoch)) return;
    _failure = problem;
    _connection = LobbyConnection.offline;
    _refreshing = false;
    final current = ++_epoch;
    _disconnect();
    notifyListeners();
    _retry = Timer(Duration(seconds: (2 << _failures.clamp(0, 3))), () {
      if (!_current(current)) return;
      if (_relays.isEmpty) {
        unawaited(_initialize());
      } else {
        _relayIndex++;
        ++_epoch;
        unawaited(_connect(_epoch));
      }
    });
    _failures++;
  }

  void _disconnect() {
    _deadline?.cancel();
    _retry?.cancel();
    _finishRelease(LobbyReleaseResult.unconfirmed);
    _scheduledRefresh?.cancel();
    _scheduledRefresh = null;
    _pendingProduct = null;
    _refreshing = false;
    _messages?.cancel().ignore();
    _messages = null;
    _socket?.close(WebSocketStatus.normalClosure).ignore();
    _socket = null;
  }

  @override
  void dispose() {
    _disposed = true;
    _disconnect();
    if (_started) _http.close(force: true);
    availableGameIds.dispose();
    super.dispose();
  }
}
