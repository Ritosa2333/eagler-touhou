import 'dart:async';
import 'dart:convert';
import 'dart:io';
import 'dart:math';

import 'package:flutter/foundation.dart';
import 'package:uuid/uuid.dart';

import 'catalog.dart';
import 'l10n/strings.dart';
import 'lobby_directory.dart';
import 'room_chat.dart';

typedef RoomOptions = ({
  int players,
  int difficulty,
  bool private,
  bool limitMovement,
  bool unrated,
});

typedef RoomTiming = ({bool rollback, int? frames});
typedef RoomInput = ({String movementMode, bool touchEnabled});
typedef RoomInputLoader = Future<RoomInput?> Function(
  Game game, {
  required bool restrictMovement,
});

enum RoomRule {
  challengeMode,
  challengeKeepBullets,
  disableLocalChargePriority,
  showHitboxes,
  disableFocus,
  ballCount,
  initialSpares,
  initialBombs,
  chapterThreshold;

  List<int>? choices(Game game) => switch (this) {
    ballCount => const [1, 2, 3, 4, 6, 8, 9],
    initialSpares => const [0, 1, 2, 3, 4, 5],
    initialBombs => game.id == 'th05' ? const [0, 1, 2, 3] : const [0, 1, 2],
    chapterThreshold => List.generate(19, (index) => 500000 + index * 250000),
    _ => null,
  };

  Object? parse(Object? value, Game game) {
    if (this == challengeMode && game.multiplayerRules?.challengeMode != true) {
      return null;
    }
    final allowed = choices(game);
    if (allowed == null) return value is bool ? value : null;
    return value is int && allowed.contains(value) ? value : null;
  }
}

class RoomRequest {
  const RoomRequest({
    required this.game,
    required this.code,
    this.create,
    this.watch = false,
    this.autoSeat = false,
  });

  final Game game;
  final String code;
  final RoomOptions? create;
  final bool watch;
  final bool autoSeat;

  String get location => Uri(
    pathSegments: ['', 'lobby', 'room', game.id, code],
    queryParameters: watch
        ? {'watch': '1'}
        : autoSeat
        ? {'join': '1'}
        : null,
  ).toString();

  Uri inviteLink(Uri origin, String language) {
    final payload = jsonEncode({
      'g': game.multiplayerProduct,
      'r': code,
      'f': true,
      'a': 'join',
      if (watch) 's': true,
    });
    final token = base64Url.encode(utf8.encode(payload)).replaceAll('=', '');
    return origin
        .resolve('/')
        .replace(queryParameters: {'j': token, 'lang': language});
  }

  static RoomRequest? fromInviteLink(
    String input,
    Uri lobbyOrigin,
    Iterable<Game> games,
  ) {
    if (input.length > 2048) return null;
    try {
      final address = Uri.tryParse(input);
      if (address == null ||
          !address.hasAuthority ||
          address.scheme != lobbyOrigin.scheme ||
          address.userInfo.isNotEmpty ||
          address.origin != lobbyOrigin.origin ||
          (address.path.isNotEmpty && address.path != '/')) {
        return null;
      }
      return fromInviteQuery(address.queryParametersAll, games);
    } on FormatException {
      return null;
    }
  }

  static RoomRequest? fromInviteQuery(
    Map<String, List<String>> query,
    Iterable<Game> games,
  ) {
    String? single(String key) {
      final values = query[key];
      return values?.length == 1 ? values!.single : null;
    }

    final Object? data;
    if (query.containsKey('j')) {
      final token = single('j');
      if (token == null ||
          token.length > 2048 ||
          !RegExp(r'^[a-zA-Z0-9_-]+$').hasMatch(token)) {
        return null;
      }
      try {
        data = jsonDecode(
          utf8.decode(base64Url.decode(base64Url.normalize(token))),
        );
      } on FormatException {
        return null;
      }
    } else {
      if ((query['lobbyAction']?.length ?? 0) > 1 ||
          (query['fromLobby']?.length ?? 0) > 1) {
        return null;
      }
      data = {
        'g': single('game'),
        'r': single('mpRoom'),
        'a': single('lobbyAction'),
        'f': single('fromLobby') == '1',
      };
    }

    if (data case {'g': final String product, 'r': final String code}
        when validRoomCode(code)) {
      final intent = data['a'];
      if (intent != null && intent != 'join') return null;
      final game = games
          .where(
            (game) => game.multiplayer && game.multiplayerProduct == product,
          )
          .firstOrNull;
      if (game != null) {
        final watch = intent == 'join' && data['s'] == true;
        return RoomRequest(
          game: game,
          code: code,
          watch: watch,
          autoSeat: !watch && intent == 'join' && data['f'] == true,
        );
      }
    }
    return null;
  }

  static String newCode(Iterable<String> reserved) {
    final random = Random.secure();
    final unavailable = reserved.toSet();
    var code = '';
    do {
      code = (1000 + random.nextInt(9000)).toString();
    } while (unavailable.contains(code));
    return code;
  }
}

enum RoomConnection { connecting, connected, reconnecting, closed }

enum RoomProblem {
  timeout,
  disconnected,
  unavailableGame,
  movementRequired,
  rejected,
  expired,
  unavailable,
  replaced,
  conflictingMembership,
  removed,
  roomLimit,
  connectionLimit;

  String label(LauncherStrings strings) => switch (this) {
    timeout => strings.roomRequestTimeout,
    disconnected => strings.roomConnectionError,
    unavailableGame => strings.roomGameMissing,
    movementRequired => strings.roomMovementRequired,
    rejected => strings.roomRejected,
    expired => strings.roomExpired,
    unavailable => strings.roomGone,
    replaced => strings.roomReplaced,
    conflictingMembership => strings.roomConflict,
    removed => strings.roomRemoved,
    roomLimit => strings.roomRoomLimit,
    connectionLimit => strings.roomConnectionLimit,
  };

  static RoomProblem? terminal(int? code, String? reason) => switch (code) {
    1008 || 4007 => unavailable,
    4004 => expired,
    4008 => switch (reason) {
      'room creation limit' => roomLimit,
      'connection limit' => connectionLimit,
      _ => replaced,
    },
    4009 => conflictingMembership,
    4010 => removed,
    _ => null,
  };
}

typedef RoomParticipant = ({
  String id,
  String name,
  String? account,
  bool offline,
  bool ready,
  int loadout,
  RoomMovement? movement,
  int? ratingGames,
});

class RoomSnapshot {
  const RoomSnapshot({
    required this.options,
    required this.seats,
    required this.spectators,
    required this.phase,
    this.rules = const {},
    this.timing,
    this.serial = 0,
    this.title,
  });

  final RoomOptions options;
  final List<RoomParticipant?> seats;
  final List<RoomParticipant> spectators;
  final String phase;
  final Map<RoomRule, Object> rules;
  final RoomTiming? timing;
  final int serial;
  final String? title;
  bool get waiting => phase == 'lobby';

  static RoomSnapshot? parse(Object? json, Game game) {
    if (json is! Map<String, dynamic>) return null;
    final count = json['playerCount'];
    final seats = json['seats'];
    final phase = json['phase'];
    if (count is! int ||
        !(game.multiplayerRules?.playerCounts ?? const [2, 3]).contains(
          count,
        ) ||
        seats is! List ||
        !const ['lobby', 'starting', 'running'].contains(phase)) {
      return null;
    }
    final observers = json['spectators'];
    final title = _label(json['roomName'], 32);
    return RoomSnapshot(
      options: (
        players: count,
        difficulty: (json['difficulty'] is int ? json['difficulty'] as int : 1)
            .clamp(0, (game.multiplayerRules?.difficulties.length ?? 5) - 1),
        private: json['visibility'] == 'private',
        limitMovement: json['disableCheatMovement'] == true,
        unrated: json['unrated'] == true,
      ),
      seats: List.unmodifiable(
        List<RoomParticipant?>.generate(
          count,
          (i) => i < seats.length ? _participant(seats[i]) : null,
        ),
      ),
      spectators: List.unmodifiable(
        observers is List
            ? observers.take(64).map(_participant).whereType<RoomParticipant>()
            : const <RoomParticipant>[],
      ),
      phase: phase as String,
      timing: _parseTiming(json, game.multiplayerRules?.timing),
      rules: Map.unmodifiable({
        for (final rule in RoomRule.values)
          if (rule.parse(json[rule.name], game) case final Object value)
            rule: value,
      }),
      serial: json['startSerial'] is int ? json['startSerial'] as int : 0,
      title: title.isEmpty ? null : title,
    );
  }

  static RoomTiming? _parseTiming(
    Map<String, dynamic> json,
    RoomTimingPolicy? policy,
  ) {
    final mode = json['adonisMode'];
    final choice = json['timingChoice'];
    if (policy == null || !const [0, 1, 2].contains(mode)) return null;
    if (choice != 'auto' &&
        (choice is! int || choice < 0 || choice > policy.manualDelayLimit)) {
      return null;
    }
    return (
      rollback: policy.rollback && mode != 1,
      frames: choice == 'auto' ? null : choice as int,
    );
  }
}

final _participantId = RegExp(r'^[A-Za-z0-9_-]{8,64}$');
final _accountId = RegExp(r'^[A-Za-z0-9_.]{3,12}$');
final _hidden = RegExp(
  r'[\u0000-\u001f\u007f-\u009f\u061c\u200b-\u200f\u202a-\u202e\u2060-\u206f\ufeff]',
);

String _label(Object? value, int limit) => value is String
    ? String.fromCharCodes(
        value.replaceAll(_hidden, '').trim().runes.take(limit),
      )
    : '';

RoomParticipant? _participant(Object? value) {
  if (value is! Map<String, dynamic> ||
      value['clientId'] is! String ||
      !_participantId.hasMatch(value['clientId'] as String)) {
    return null;
  }
  final nickname = _label(value['name'], 12);
  final profile = value['profile'];
  final username = profile is Map ? profile['username'] : null;
  final stats = profile is Map ? profile['th09'] : null;
  return (
    id: value['clientId'] as String,
    name: nickname,
    account: username is String && _accountId.hasMatch(username)
        ? username
        : null,
    offline: value['offline'] == true,
    ready: value['ready'] == true,
    loadout: value['loadout'] is int ? value['loadout'] as int : 0,
    movement: RoomMovement.parse(value['controlMode']),
    ratingGames: switch (stats) {
      {'games': final int games} when games >= 0 => games,
      _ => null,
    },
  );
}

class RoomSession extends ChangeNotifier {
  RoomSession({
    required this._relay,
    required RoomRequest request,
    this.identityTicket,
    this.onPhrase,
    this.onMovementRestricted,
    required this.readInput,
  }) : _request = request,
       game = request.game,
       _wantWatch = request.watch;

  final RoomRelay _relay;
  final RoomRequest _request;
  final Future<String?> Function(String challenge)? identityTicket;
  final void Function(String)? onPhrase;
  final VoidCallback? onMovementRestricted;
  final RoomInputLoader readInput;
  final _clientId = const Uuid().v4();
  final _http = HttpClient()..connectionTimeout = const Duration(seconds: 8);
  final _visitedProducts = <String>{};
  WebSocket? _socket;
  StreamSubscription<dynamic>? _events;
  Timer? _deadline;
  Timer? _retry;
  bool _admitted = false;
  bool _connecting = false;
  bool _wantWatch;
  bool _closed = false;
  bool _disposed = false;
  int _attempt = 0;
  int _epoch = 0;
  int? _seatOnAdmission;
  bool _takingSeat = false;
  int? _unrestrictedSeatAttempt;
  int _movementRevision = 0;

  late final chat = RoomChat(
    (message) => (localSeat != null || watching) && _send(message),
    onPhrase: onPhrase,
  );
  Game game;
  RoomSnapshot? snapshot;
  RoomConnection connection = RoomConnection.connecting;
  RoomProblem? notice;
  String? _serverDetail;

  String? statusText(LauncherStrings strings, String language) =>
      notice == RoomProblem.rejected &&
          language == 'zh-CN' &&
          _serverDetail != null
      ? _serverDetail
      : notice?.label(strings);
  bool watchReady = false;

  String get code => _request.code;
  String get _roomId => '${game.multiplayerProduct}-$code';
  bool get connected => connection == RoomConnection.connected;
  int? get localSeat {
    final seats = snapshot?.seats;
    if (seats == null) return null;
    final index = seats.indexWhere((seat) => seat?.id == _clientId);
    return index < 0 ? null : index;
  }

  bool get watching =>
      snapshot?.spectators.any((entry) => entry.id == _clientId) == true;

  bool get ownsRoom => connected && localSeat == 0;
  bool get restrictsMovement => snapshot?.options.limitMovement == true;
  bool get restrictsLocalMovement => restrictsMovement && localSeat != null;

  void start() {
    if (_epoch > 0 || _closed) return;
    _visitedProducts.add(game.multiplayerProduct);
    _seatOnAdmission = _request.create == null ? null : 0;
    unawaited(_connect());
  }

  void reconnect() {
    if (_closed || connected || _connecting) return;
    _retry?.cancel();
    _attempt = 0;
    unawaited(_connect());
  }

  void refreshIdentity() {
    if (!_closed && connected) unawaited(_connect());
  }

  RoomParticipant? participant(String client) {
    final state = snapshot;
    if (state == null) return null;
    for (final entry in [...state.seats, ...state.spectators]) {
      if (entry?.id == client) return entry;
    }
    return null;
  }

  Uri _address() {
    final options = !_admitted ? _request.create : null;
    final query = {..._relay.endpoint.queryParameters}
      ..removeWhere(
        (key, _) => const [
          'room',
          'lobby',
          'member',
          'intent',
          'directory',
          'diagnostic',
          'run',
          'player',
          'signal',
          'spectator',
          'players',
          'difficulty',
        ].contains(key),
      );
    return _relay.endpoint.replace(
      fragment: '',
      queryParameters: {
        ...query,
        'room': _roomId,
        'lobby': _clientId,
        'member': _relay.member,
        'intent': options == null ? 'join' : 'create',
        if (options != null) ...{
          'players': options.players.toString(),
          'difficulty': options.difficulty.toString(),
          'visibility': options.private ? 'private' : 'public',
          'disableCheatMovement': options.limitMovement ? '1' : '0',
          if (game.id == 'th09') ...{
            'disableLocalChargePriority': '1',
            'unrated': options.unrated ? '1' : '0',
          },
        },
      },
    );
  }

  Future<void> _connect() async {
    if (_closed) return;
    final epoch = ++_epoch;
    _unrestrictedSeatAttempt = null;
    _connecting = true;
    _deadline?.cancel();
    await _retireSocket();
    if (!_current(epoch)) return;
    chat.connectionLost();
    connection = snapshot == null
        ? RoomConnection.connecting
        : RoomConnection.reconnecting;
    notice = null;
    _serverDetail = null;
    notifyListeners();
    final pending = WebSocket.connect(
      _address().toString(),
      headers: {'Origin': _relay.origin.origin},
      customClient: _http,
    );
    try {
      final socket = await pending.timeout(const Duration(seconds: 10));
      if (!_current(epoch)) {
        await socket.close(WebSocketStatus.normalClosure, 'leave room');
        return;
      }
      _socket = socket;
      _deadline = Timer(
        const Duration(seconds: 10),
        () => _interrupted(epoch, RoomProblem.timeout),
      );
      _events = socket.listen(
        (event) => _receive(epoch, event),
        onError: (Object _) => _interrupted(epoch, RoomProblem.disconnected),
        onDone: () => _interrupted(
          epoch,
          RoomProblem.disconnected,
          terminal: RoomProblem.terminal(socket.closeCode, socket.closeReason),
        ),
        cancelOnError: true,
      );
    } on TimeoutException {
      pending
          .then(
            (socket) =>
                socket.close(WebSocketStatus.normalClosure, 'leave room'),
          )
          .ignore();
      _interrupted(epoch, RoomProblem.timeout);
    } on Object {
      _interrupted(epoch, RoomProblem.disconnected);
    }
  }

  bool _current(int epoch) => !_closed && epoch == _epoch;

  void _receive(int epoch, Object? event) {
    if (!_current(epoch) || event is! String || event.length > 262144) return;
    final Object? raw;
    try {
      raw = jsonDecode(event);
    } on FormatException {
      return;
    }
    if (raw is! Map<String, dynamic>) return;

    switch (raw['type']) {
      case 'room-product':
        final product = raw['product'];
        final next = _relay.products[product];
        if (raw['code'] != code ||
            next == null ||
            _visitedProducts.length >= 4 ||
            !_visitedProducts.add(next.multiplayerProduct)) {
          _interrupted(
            epoch,
            RoomProblem.unavailableGame,
            terminal: RoomProblem.unavailableGame,
          );
          return;
        }
        game = next;
        _admitted = true;
        _wantWatch = _wantWatch || raw['spectator'] == true;
        unawaited(_connect());
      case 'chat-status' ||
          'chat-history' ||
          'chat-retract' ||
          'chat-result' ||
          'free-chat' ||
          'quick-chat':
        final current = snapshot;
        if (!connected || current == null) return;
        final isSpectator = raw['spectator'] == true;
        chat.receive(
          raw,
          room: _roomId,
          serial: current.serial,
          localClient: _clientId,
          seated: localSeat != null,
          participantName: (seat, client) {
            if (seat == -1 && isSpectator) {
              for (final person in current.spectators) {
                if (person.id == client) {
                  return person.name;
                }
              }
            } else if (seat >= 0 && seat < current.seats.length) {
              final player = current.seats[seat];
              if (player?.id == client) {
                return player!.name.isEmpty ? 'P${seat + 1}' : player.name;
              }
            }
            return null;
          },
        );
      case 'error':
        if (raw['code'] == 'movement-policy') {
          final seat = _unrestrictedSeatAttempt;
          _unrestrictedSeatAttempt = null;
          if (seat != null && raw['seat'] == seat && _seatAvailable(seat)) {
            unawaited(_takeSeat(seat, relayRequiresCompatibleMode: true));
          } else {
            notice = RoomProblem.movementRequired;
            notifyListeners();
          }
          return;
        }
        final message = _label(raw['error'], 120);
        _serverDetail = message.isEmpty ? null : message;
        notice = RoomProblem.rejected;
        notifyListeners();
      case 'state' || 'start' || 'spectator-start' || 'spectator-waiting':
        final updated = RoomSnapshot.parse(raw['room'], game);
        if (updated == null) return;
        final first = !connected;
        final firstAdmission = !_admitted;
        final wasRestricted = restrictsMovement;
        snapshot = updated;
        if (raw['type'] == 'spectator-start') watchReady = true;
        if (raw['type'] == 'spectator-waiting' || updated.waiting) {
          watchReady = false;
        }
        connection = RoomConnection.connected;
        if (_unrestrictedSeatAttempt case final seat?
            when !_seatAvailable(seat)) {
          _unrestrictedSeatAttempt = null;
        }
        _connecting = false;
        _admitted = true;
        _attempt = 0;
        _deadline?.cancel();

        if (first) {
          if (raw['accountChallenge'] case final String challenge
              when challenge.length <= 128 && identityTicket != null) {
            unawaited(_registerIdentity(epoch, challenge));
          }
          if (_seatOnAdmission case final int seat) {
            _seatOnAdmission = null;
            if (localSeat == null) takeSeat(seat);
          } else if (_wantWatch && !watching && localSeat == null) {
            spectate();
          } else if (firstAdmission &&
              _request.autoSeat &&
              !watching &&
              localSeat == null) {
            final empty = updated.seats.indexWhere((seat) => seat == null);
            if (updated.waiting && empty >= 0) {
              takeSeat(empty);
            } else if (game.id != 'th09') {
              spectate();
            }
          }
        } else {
          _wantWatch = watching;
        }
        notifyListeners();
        if (!wasRestricted && restrictsLocalMovement && updated.waiting) {
          onMovementRestricted?.call();
        }
    }
  }

  Future<void> _registerIdentity(int epoch, String challenge) async {
    try {
      final ticket = await identityTicket!(challenge);
      if (_current(epoch) && ticket != null) {
        _send({'type': 'account-nickname', 'ticket': ticket});
      }
    } on Exception {
      // The room keeps the Relay's guest identity if account binding fails.
    }
  }

  bool _send(Map<String, Object> value) {
    final socket = _socket;
    if (!connected || socket == null || socket.readyState != WebSocket.open) {
      return false;
    }
    notice = null;
    _serverDetail = null;
    try {
      socket.add(jsonEncode(value));
      return true;
    } on Object {
      _interrupted(_epoch, RoomProblem.rejected);
      return false;
    }
  }

  bool _seatAvailable(int index) {
    final state = snapshot;
    return connected &&
        state != null &&
        state.waiting &&
        index >= 0 &&
        index < state.seats.length &&
        state.seats[index] == null;
  }

  Future<void> takeSeat(int index) => _takeSeat(index);

  Future<void> _takeSeat(
    int index, {
    bool relayRequiresCompatibleMode = false,
  }) async {
    if (_takingSeat || !_seatAvailable(index)) return;
    final epoch = _epoch;
    _takingSeat = true;
    try {
      var input = await readInput(
        game,
        restrictMovement: restrictsMovement || relayRequiresCompatibleMode,
      );
      if (!_current(epoch) || !_seatAvailable(index) || input == null) return;
      if (restrictsMovement && input.movementMode == 'touch-unlimited') {
        input = await readInput(game, restrictMovement: true);
      }
      if (!_current(epoch) || !_seatAvailable(index) || input == null) return;
      if ((restrictsMovement || relayRequiresCompatibleMode) &&
          input.movementMode == 'touch-unlimited') {
        return;
      }
      final sent = _send({
        'type': 'take-seat',
        'seat': index,
        'loadout': 0,
        'movementMode': input.movementMode,
        'touchEnabled': input.touchEnabled,
      });
      _unrestrictedSeatAttempt = sent && input.movementMode == 'touch-unlimited'
          ? index
          : null;
    } on Exception {
      if (_current(epoch)) {
        notice = RoomProblem.rejected;
        notifyListeners();
      }
    } finally {
      _takingSeat = false;
    }
  }

  Future<void> refreshMovement() async {
    final seat = localSeat;
    if (!connected || seat == null || snapshot?.waiting != true) return;
    final epoch = _epoch;
    final revision = ++_movementRevision;
    try {
      final input = await readInput(game, restrictMovement: false);
      if (!_current(epoch) ||
          revision != _movementRevision ||
          localSeat != seat ||
          snapshot?.waiting != true ||
          input == null ||
          restrictsMovement && input.movementMode == 'touch-unlimited') {
        return;
      }
      _send({
        'type': 'movement',
        'movementMode': input.movementMode,
        'touchEnabled': input.touchEnabled,
      });
    } on Exception {
      if (_current(epoch) && revision == _movementRevision) {
        notice = RoomProblem.rejected;
        notifyListeners();
      }
    }
  }

  void standUp() {
    if (snapshot?.waiting == true && localSeat != null) {
      _send({'type': 'stand-up'});
    }
  }

  void spectate() {
    if (watching || localSeat != null && snapshot?.waiting != true) return;
    _send({'type': 'spectate'});
  }

  void stopSpectating() {
    if (watching) _send({'type': 'leave-spectator'});
  }

  void changeOptions(
    RoomOptions options, {
    Map<RoomRule, Object> rules = const {},
  }) {
    final current = snapshot;
    if (!ownsRoom || current == null || !current.waiting) return;
    final ruleChanges = {
      for (final entry in rules.entries)
        if (current.rules.containsKey(entry.key) &&
            entry.key.parse(entry.value, game) != null &&
            current.rules[entry.key] != entry.value)
          entry.key.name: entry.value,
    };
    if (options == current.options && ruleChanges.isEmpty) return;
    _send({
      'type': 'settings',
      'playerCount': options.players,
      'difficulty': options.difficulty,
      'visibility': options.private ? 'private' : 'public',
      'disableCheatMovement': options.limitMovement,
      if (game.id == 'th09') 'unrated': options.unrated,
      ...ruleChanges,
    });
  }

  void changeTiming(RoomTiming timing) {
    final current = snapshot;
    final policy = game.multiplayerRules?.timing;
    final frames = timing.frames;
    if (!ownsRoom ||
        current == null ||
        !current.waiting ||
        current.timing == null ||
        current.timing == timing ||
        policy == null ||
        (timing.rollback && !policy.rollback) ||
        (frames != null && (frames < 0 || frames > policy.manualDelayLimit))) {
      return;
    }
    _send({
      'type': 'timing-settings',
      'rollbackEnabled': timing.rollback,
      'timingChoice': frames ?? 'auto',
    });
  }

  void manageSeat(int seat, String id, {required bool remove}) {
    final state = snapshot;
    if (!ownsRoom ||
        state == null ||
        !state.waiting ||
        seat < 1 ||
        seat >= state.seats.length ||
        state.seats[seat]?.id != id) {
      return;
    }
    _send({
      'type': remove ? 'remove-player' : 'move-player-to-spectator',
      'seat': seat,
      'clientId': id,
    });
  }

  void removeSpectator(String id) {
    if (!ownsRoom ||
        snapshot?.waiting != true ||
        snapshot?.spectators.any((entry) => entry.id == id) != true) {
      return;
    }
    _send({'type': 'remove-spectator', 'clientId': id});
  }

  void _interrupted(int epoch, RoomProblem problem, {RoomProblem? terminal}) {
    if (!_current(epoch)) return;
    ++_epoch;
    _connecting = false;
    _deadline?.cancel();
    _retry?.cancel();
    unawaited(_retireSocket());
    connection = terminal == null
        ? RoomConnection.reconnecting
        : RoomConnection.closed;
    chat.connectionLost();
    notice = terminal ?? notice ?? problem;
    notifyListeners();
    if (terminal == null) {
      _retry = Timer(Duration(seconds: 1 << _attempt.clamp(0, 3)), () {
        if (!_closed) unawaited(_connect());
      });
      _attempt++;
    }
  }

  Future<void> _retireSocket({bool leave = false}) async {
    final socket = _socket;
    _socket = null;
    final events = _events;
    _events = null;
    try {
      await socket
          ?.close(
            WebSocketStatus.normalClosure,
            leave ? 'leave room' : 'reconnect room',
          )
          .timeout(const Duration(seconds: 2));
    } on Exception {
      // Socket closure is also observed by Relay's disconnect timer.
    } finally {
      await events?.cancel();
    }
  }

  Future<void> leave() async {
    if (_closed) return;
    _closed = true;
    ++_epoch;
    _deadline?.cancel();
    _retry?.cancel();
    await _retireSocket(leave: true);
    _http.close(force: true);
  }

  @override
  void dispose() {
    if (_disposed) return;
    _disposed = true;
    unawaited(leave());
    chat.dispose();
    super.dispose();
  }
}
