import 'dart:async';

import 'package:flutter/widgets.dart';
import 'package:shared_preferences/shared_preferences.dart';

import 'account.dart';
import 'account_api.dart';

/// Owns the first announcement page and unread counts shared by launcher entries.
class MessageHighlights extends ChangeNotifier {
  MessageHighlights(this.session, this._storage) {
    _username = session.identity?.username;
    _restore = _restoreSeen();
    session.addListener(_onAccountChanged);
    _lifecycle = AppLifecycleListener(onStateChange: _onLifecycleChanged);
    _foreground =
        WidgetsBinding.instance.lifecycleState == AppLifecycleState.resumed;
    _updateInboxTimer();
    _refresh();
  }

  static const _seenKey = 'launcher.announcements.seen';
  final AccountSession session;
  final conversationReadRevision = ValueNotifier(0);
  final inboxRefreshRevision = ValueNotifier(0);
  Timer? _inboxTimer;
  late bool _foreground;
  final SharedPreferencesAsync _storage;
  final _seen = <String>{};
  final _conversationReadThrough = <String, int>{};
  final _conversationReads = <String, ({int through, Future<void> request})>{};
  late final Future<void> _restore;
  late final AppLifecycleListener _lifecycle;
  late String? _username;
  int _accountGeneration = 0;
  Future<void> _write = Future<void>.value();
  Future<AnnouncementPage>? _feedRequest;
  Future<void>? _inboxRequest;
  AnnouncementPage? _feed;
  int? _inboxUnread;
  bool _inboxFailed = false;
  bool _seenLoaded = false;
  bool _closed = false;

  int get announcementUnread => !_seenLoaded
      ? 0
      : _feed?.items.where((item) => !_seen.contains('${item.id}')).length ?? 0;
  PublicAnnouncement? get latestAnnouncement => _feed?.items.firstOrNull;
  int? get inboxUnread => _inboxUnread;
  bool get inboxFailed => _inboxFailed;

  Future<void> _restoreSeen() async {
    try {
      _seen.addAll(await _storage.getStringList(_seenKey) ?? const []);
    } catch (_) {
      // The current run can still track seen entries in memory.
    }
    if (_closed) return;
    _seenLoaded = true;
    notifyListeners();
  }

  Future<void> markAnnouncementsSeen(
    Iterable<PublicAnnouncement> entries,
  ) async {
    final ids = entries.map((entry) => '${entry.id}').toList();
    await _restore;
    if (_closed) return;
    final previousSize = _seen.length;
    _seen.addAll(ids);
    if (_seen.length == previousSize) return;
    notifyListeners();
    final snapshot = _seen.toList();
    _write = _write
        .then((_) => _storage.setStringList(_seenKey, snapshot))
        .catchError((Object _) {});
    await _write;
  }

  Future<AnnouncementPage> announcements({int? before, bool refresh = false}) {
    if (before != null) return session.api.announcements(before: before);
    final existing = _feedRequest;
    if (existing != null) return existing;
    final cached = _feed;
    if (!refresh && cached != null) return Future.value(cached);

    late final Future<AnnouncementPage> request;
    request = session.api
        .announcements()
        .then((page) {
          if (identical(_feedRequest, request)) {
            _feed = page;
            notifyListeners();
          }
          return page;
        })
        .whenComplete(() {
          if (identical(_feedRequest, request)) _feedRequest = null;
        });
    _feedRequest = request;
    return request;
  }

  void _refresh() {
    unawaited(
      announcements(refresh: true).then<void>((_) {}, onError: (Object _) {}),
    );
    _refreshMessages();
  }

  void _refreshMessages() {
    if (_username == null) return;
    unawaited(refreshInbox());
    inboxRefreshRevision.value++;
  }

  void _onLifecycleChanged(AppLifecycleState state) {
    _foreground = state == AppLifecycleState.resumed;
    _updateInboxTimer();
    if (_foreground) _refresh();
  }

  void _updateInboxTimer() {
    if (_foreground && _username != null) {
      _inboxTimer ??= Timer.periodic(
        const Duration(seconds: 30),
        (_) => _refreshMessages(),
      );
    } else {
      _inboxTimer?.cancel();
      _inboxTimer = null;
    }
  }

  Future<AnnouncementPage> publishedAnnouncement() {
    _feedRequest = null;
    return announcements(refresh: true);
  }

  Future<void> refreshInbox() {
    if (_username == null) return Future.value();
    final existing = _inboxRequest;
    if (existing != null) return existing;

    late final Future<void> request;
    request = session.api
        .inbox(summary: true)
        .then<void>(
          (page) {
            if (!identical(_inboxRequest, request)) return;
            _inboxUnread = page.unread;
            _inboxFailed = false;
            notifyListeners();
          },
          onError: (Object error) {
            if (!identical(_inboxRequest, request)) return;
            session.expireIfUnauthorized(error);
            if (!identical(_inboxRequest, request)) return;
            _inboxFailed = true;
            notifyListeners();
          },
        )
        .whenComplete(() {
          if (identical(_inboxRequest, request)) _inboxRequest = null;
        });
    _inboxRequest = request;
    return request;
  }

  Future<void> markConversationRead(String username, int through) async {
    if (through <= (_conversationReadThrough[username] ?? 0)) return;
    final existing = _conversationReads[username];
    if (existing != null && through <= existing.through) {
      return existing.request;
    }
    final generation = _accountGeneration;
    late final Future<void> request;
    request = session.api
        .markRead(username, through)
        .then((_) {
          if (_closed || generation != _accountGeneration) return;
          if (through <= (_conversationReadThrough[username] ?? 0)) return;
          _conversationReadThrough[username] = through;
          _inboxRequest = null;
          conversationReadRevision.value++;
          unawaited(refreshInbox());
        })
        .whenComplete(() {
          if (identical(_conversationReads[username]?.request, request)) {
            _conversationReads.remove(username);
          }
        });
    _conversationReads[username] = (through: through, request: request);
    await request;
  }

  void _onAccountChanged() {
    final username = session.identity?.username;
    if (username == _username) return;
    _username = username;
    _accountGeneration++;
    _conversationReadThrough.clear();
    _conversationReads.clear();
    _feed = null;
    _feedRequest = null;
    _inboxUnread = null;
    _inboxFailed = false;
    _inboxRequest = null;
    notifyListeners();
    _updateInboxTimer();
    _refresh();
  }

  @override
  void dispose() {
    _closed = true;
    _feedRequest = null;
    _inboxRequest = null;
    _lifecycle.dispose();
    _inboxTimer?.cancel();
    inboxRefreshRevision.dispose();
    conversationReadRevision.dispose();
    session.removeListener(_onAccountChanged);
    super.dispose();
  }
}
