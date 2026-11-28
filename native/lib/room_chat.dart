import 'dart:async';

import 'package:flutter/foundation.dart';

import 'l10n/strings.dart';
import 'room_phrases.dart';

typedef RoomMessage = ({
  String client,
  String name,
  String text,
  String? phrase,
  bool spectator,
  String? author,
  int? id,
});

enum RoomChatResult {
  timeout,
  disconnected,
  reportSent,
  signIn,
  sendRateLimited,
  reportRateLimited,
  banned,
  alreadyReported,
  failed;

  String label(LauncherStrings strings) => switch (this) {
    timeout => strings.roomRequestTimeout,
    disconnected => strings.roomChatInterrupted,
    reportSent => strings.roomChatSentReport,
    signIn => strings.roomChatSignIn,
    sendRateLimited => strings.roomChatSendSlow,
    reportRateLimited => strings.roomChatReportSlow,
    banned => strings.roomChatBan,
    alreadyReported => strings.roomChatAlreadyReported,
    failed => strings.roomChatFailed,
  };
}

class RoomChat extends ChangeNotifier {
  RoomChat(this._send, {this.onPhrase});

  final bool Function(Map<String, Object>) _send;
  final void Function(String)? onPhrase;
  final _rows = <RoomMessage>[];
  final _retracted = <String, int>{};
  final _muted = <String>{};
  String? _author;
  int _historyThrough = -1;
  int _bannedUntil = 0;
  Timer? _pendingTimeout;
  Timer? _banExpiry;
  DateTime? _lastPhrase;
  String? _pendingAction;
  int _resultRevision = 0;
  RoomChatResult? _result;

  bool get registered => _author != null;
  bool get banned =>
      _bannedUntil < 0 || _bannedUntil > DateTime.now().millisecondsSinceEpoch;
  bool get busy => _pendingAction != null;
  int get resultRevision => _resultRevision;
  RoomChatResult? get result => _result;
  List<RoomMessage> get messages => [
    for (final row in _rows)
      if (!_muted.contains(row.client)) row,
  ];

  void mute(String client) {
    _muted.add(client);
    notifyListeners();
  }

  bool phrase(String id, int serial) {
    final now = DateTime.now();
    final previous = _lastPhrase;
    if (!roomPhrases.containsKey(id) ||
        previous != null &&
            now.difference(previous) < const Duration(milliseconds: 500)) {
      return false;
    }
    if (!_send({'type': 'quick-chat', 'phrase': id, 'serial': serial})) {
      return false;
    }
    _lastPhrase = now;
    onPhrase?.call(id);
    return true;
  }

  bool sendText(String draft, int serial) {
    final text = draft.trim();
    if (text.isEmpty || text.length > 500 || !registered || banned) {
      return false;
    }
    return _submit({'type': 'free-chat', 'text': text, 'serial': serial});
  }

  bool canReport(RoomMessage row) => row.id != null && row.author != _author;

  bool report(RoomMessage row, String draft) {
    if (!canReport(row)) return false;
    final reason = draft.trim();
    if (reason.length > 500) return false;
    return _submit({
      'type': 'chat-report',
      'messageId': row.id!,
      if (reason.isNotEmpty) 'reason': reason,
    });
  }

  bool _submit(Map<String, Object> message) {
    if (busy || !_send(message)) return false;
    _pendingAction = message['type']! as String;
    _pendingTimeout = Timer(
      Duration(seconds: _pendingAction == 'chat-report' ? 23 : 13),
      () => _complete(RoomChatResult.timeout),
    );
    notifyListeners();
    return true;
  }

  void connectionLost() {
    _author = null;
    _bannedUntil = 0;
    _banExpiry?.cancel();
    if (busy) {
      _complete(RoomChatResult.disconnected);
    } else {
      notifyListeners();
    }
  }

  void receive(
    Map<String, dynamic> event, {
    required String room,
    required int serial,
    required String? Function(int seat, String client) participantName,
    String? localClient,
    bool seated = false,
  }) {
    final eventRoom = event['room'];
    if (eventRoom != null && eventRoom != room) return;
    switch (event['type']) {
      case 'chat-status':
        final author = event['authorId'];
        _author =
            event['registered'] == true && author is String && author.isNotEmpty
            ? author
            : null;
        _setBan(event['bannedUntil']);
      case 'chat-retract':
        final author = event['authorId'];
        final through = event['throughId'];
        if (author is! String || through is! int || through < 1) return;
        if (through <= (_retracted[author] ?? 0)) return;
        _retracted[author] = through;
        if (_retracted.length > 256) _retracted.remove(_retracted.keys.first);
        _rows.removeWhere((row) => row.author == author && row.id! <= through);
        if (author == _author) {
          _setBan(event['bannedUntil']);
        } else {
          notifyListeners();
        }
      case 'chat-result':
        final action = event['action'];
        if (_pendingAction == null || _pendingAction != action) return;
        if (event['ok'] == true) {
          _complete(action == 'chat-report' ? RoomChatResult.reportSent : null);
        } else {
          if (event['code'] == 'sign-in-required') _author = null;
          _complete(switch (event['code']) {
            'sign-in-required' => RoomChatResult.signIn,
            'send-rate-limited' => RoomChatResult.sendRateLimited,
            'report-rate-limited' => RoomChatResult.reportRateLimited,
            'chat-banned' => RoomChatResult.banned,
            'already-reported' => RoomChatResult.alreadyReported,
            _ => RoomChatResult.failed,
          });
        }
      case 'chat-history':
        final through = event['throughId'];
        final history = event['entries'];
        if (through is! int || through < _historyThrough || history is! List) {
          return;
        }
        _historyThrough = through;
        _rows.removeWhere((row) => row.id != null && row.id! <= through);
        final parsed = <int, RoomMessage>{};
        for (final record in history.reversed.take(50)) {
          final row = _textMessage(record, room);
          if (row != null && row.id! <= through) parsed[row.id!] = row;
        }
        final ids = parsed.keys.toList()..sort();
        _rows.insertAll(0, ids.map((id) => parsed[id]!));
        _publishRows();
      case 'free-chat':
        if (event['serial'] != serial) return;
        final row = _textMessage(event, room);
        if (row == null || _rows.any((entry) => entry.id == row.id)) return;
        _rows.add(row);
        _publishRows();
      case 'quick-chat':
        final who = event['clientId'];
        final seat = event['seat'];
        final phrase = event['phrase'];
        if (eventRoom != room ||
            event['serial'] != serial ||
            who is! String ||
            seat is! int ||
            phrase is! String ||
            !roomPhrases.containsKey(phrase)) {
          return;
        }
        final name = participantName(seat, who);
        if (name == null) return;
        _rows.add((
          client: who,
          name: name,
          text: '',
          phrase: phrase,
          spectator: seat == -1,
          author: null,
          id: null,
        ));
        if (who != localClient &&
            !_muted.contains(who) &&
            (!seated || seat != -1)) {
          onPhrase?.call(phrase);
        }
        _publishRows();
    }
  }

  RoomMessage? _textMessage(Object? raw, String room) {
    if (raw is! Map || raw['room'] != room) return null;
    final who = raw['clientId'];
    final name = raw['name'];
    final text = raw['text'];
    final id = raw['messageId'];
    final author = raw['authorId'];
    final seat = raw['seat'];
    if (who is! String ||
        who.isEmpty ||
        who.length > 64 ||
        name is! String ||
        name.length > 32 ||
        text is! String ||
        text.length > 500 ||
        text.trim().isEmpty ||
        id is! int ||
        author is! String ||
        author.isEmpty ||
        author.length > 128 ||
        id <= (_retracted[author] ?? 0) ||
        seat is! int ||
        seat < -1 ||
        seat > 2 ||
        (seat == -1) != (raw['spectator'] == true)) {
      return null;
    }
    return (
      client: who,
      name: name,
      text: text,
      phrase: null,
      spectator: seat == -1,
      author: author,
      id: id,
    );
  }

  void _setBan(Object? until) {
    _banExpiry?.cancel();
    _bannedUntil = until is int ? until : 0;
    if (_bannedUntil > DateTime.now().millisecondsSinceEpoch) {
      _banExpiry = Timer(
        Duration(
          milliseconds: _bannedUntil - DateTime.now().millisecondsSinceEpoch,
        ),
        notifyListeners,
      );
    }
    notifyListeners();
  }

  void _publishRows() {
    if (_rows.length > 50) _rows.removeRange(0, _rows.length - 50);
    notifyListeners();
  }

  void _complete(RoomChatResult? message) {
    _pendingTimeout?.cancel();
    _pendingAction = null;
    _result = message;
    _resultRevision++;
    notifyListeners();
  }

  @override
  void dispose() {
    _pendingTimeout?.cancel();
    _banExpiry?.cancel();
    super.dispose();
  }
}
