import 'dart:async';
import 'dart:convert';
import 'dart:io';
import 'dart:typed_data';

import 'package:flutter_secure_storage/flutter_secure_storage.dart';
import 'package:http/http.dart' show MultipartFile, MultipartRequest;

import 'cloud_saves.dart';
import 'game_file_archive.dart' show isGameFilePath;

import 'profile_data.dart';
import 'th09_ratings.dart';
import 'th09_replay_match.dart' show th09ReplayLimit;
import 'save_progress.dart';
import 'profile_draft.dart';

class PublicAnnouncement {
  const PublicAnnouncement({
    required this.id,
    required this.author,
    required this.authorUsername,
    required this.body,
    required this.publishedAt,
  });

  factory PublicAnnouncement.fromJson(Map<String, dynamic> json) =>
      PublicAnnouncement(
        id: json['id'] as int,
        author: json['author'] as String,
        authorUsername: json['authorUsername'] as String?,
        body: json['body'] as String,
        publishedAt: DateTime.fromMillisecondsSinceEpoch(
          json['publishedAt'] as int,
          isUtc: true,
        ),
      );

  final int id;
  final String author;
  final String? authorUsername;
  final String body;
  final DateTime publishedAt;
}

typedef AnnouncementPage = ({
  List<PublicAnnouncement> items,
  int? nextBefore,
  bool canPublish,
});

class DirectMessage {
  const DirectMessage({
    required this.id,
    required this.body,
    required this.outgoing,
    required this.sentAt,
  });

  factory DirectMessage.fromJson(Map<String, dynamic> json) => DirectMessage(
    id: json['id'] as int,
    body: json['body'] as String,
    outgoing: json['outgoing'] as bool,
    sentAt: DateTime.fromMillisecondsSinceEpoch(json['sentAt'] as int),
  );

  final int id;
  final String body;
  final bool outgoing;
  final DateTime sentAt;
}

typedef MessagePeer = ({String username, String nickname});
typedef MessageThread = ({
  MessagePeer peer,
  DirectMessage lastMessage,
  int unread,
});
typedef InboxPage = ({
  List<MessageThread> threads,
  int unread,
  int? nextBefore,
});
typedef ConversationPage = ({
  MessagePeer peer,
  List<DirectMessage> messages,
  int? nextBefore,
});

MessagePeer _peer(Map<String, dynamic> value) => (
  username: value['username'] as String,
  nickname: value['nickname'] as String,
);

class AccountRequestException implements Exception {
  const AccountRequestException(this.status, this.code);

  final int status;
  final String code;
}

typedef AccountIdentity = ({String username, String name, DateTime? expiresAt});
typedef RegistrationChallenge = ({String id, DateTime expiresAt});

typedef ProfileChange = ({String username, bool reviewFinished});

class AccountApi {
  AccountApi({Uri? origin, this._sessionStorage})
    : _origin = origin ?? Uri.https('touhouonline.com'),
      _client = HttpClient()..connectionTimeout = _timeout;

  static const _timeout = Duration(seconds: 10);
  final Uri _origin;
  final FlutterSecureStorage? _sessionStorage;
  final HttpClient _client;
  final _profiles =
      <String, ({DateTime requestedAt, Future<PublicProfile> result})>{};
  Cookie? _sessionCookie;
  Future<void> _sessionWrite = Future<void>.value();
  final _profileChanges = StreamController<ProfileChange>.broadcast();
  Stream<ProfileChange> get profileChanges => _profileChanges.stream;
  bool get canRestoreSession => _sessionStorage != null;

  String get _sessionKey =>
      'account-session-${base64Url.encode(utf8.encode(_origin.origin))}';

  Future<void> _writeSessionCookie(Cookie? cookie) {
    _sessionCookie = cookie;
    final storage = _sessionStorage;
    if (storage == null) return Future<void>.value();
    return _sessionWrite = _sessionWrite.then((_) async {
      try {
        await storage.write(key: _sessionKey, value: cookie?.toString());
      } on Exception {
        // The confirmed session remains usable for this run.
      }
    });
  }

  bool _validCookie(Cookie cookie) =>
      cookie.name == _sessionCookieName &&
      cookie.value.isNotEmpty &&
      (cookie.maxAge ?? 1) > 0 &&
      (cookie.expires?.isAfter(DateTime.now()) ?? true);

  String get sessionCookieName => _sessionCookieName;

  Cookie? get webSessionCookie {
    final cookie = _sessionCookie;
    return cookie != null && _validCookie(cookie)
        ? Cookie.fromSetCookieValue(cookie.toString())
        : null;
  }

  /// Accepts the native WebView cookie store, then verifies identity with the service.
  Future<AccountIdentity?> acceptWebSession(Uri source, Cookie? cookie) async {
    if (source.origin != _origin.origin || source.userInfo.isNotEmpty) {
      throw const FormatException('Account WebView origin mismatch');
    }
    if (cookie != null) {
      final domain = cookie.domain?.toLowerCase().replaceFirst(
        RegExp(r'^\.'),
        '',
      );
      if (!_validCookie(cookie) ||
          domain != null && domain != _origin.host ||
          _origin.scheme == 'https' && !cookie.secure) {
        throw const FormatException('Invalid account WebView session cookie');
      }
    }
    await _writeSessionCookie(cookie);
    _profiles.clear();
    return session();
  }

  Future<AccountIdentity?> restoreSession() async {
    final storage = _sessionStorage;
    if (storage == null || _sessionCookie != null) return session();
    await _sessionWrite;
    final saved = await storage.read(key: _sessionKey);
    if (saved == null) return null;
    Cookie? cookie;
    try {
      cookie = Cookie.fromSetCookieValue(saved);
    } on FormatException {
      // Invalid stored credentials are discarded using the same path as expiry.
    }
    if (cookie == null || !_validCookie(cookie)) {
      await _writeSessionCookie(null);
      return null;
    }
    _sessionCookie = cookie;
    return session();
  }

  void invalidateProfile(String username) {
    if (_profileChanges.isClosed) return;
    final key = username.toLowerCase();
    _profiles.remove(key);
    _profileChanges.add((username: key, reviewFinished: false));
  }

  Future<PublicProfile> refreshPublishedProfile(String username) async {
    final result = await profile(username, fresh: true);
    if (!result.reviewPending && !_profileChanges.isClosed) {
      _profileChanges.add((
        username: result.username.toLowerCase(),
        reviewFinished: true,
      ));
    }
    return result;
  }

  Future<ProfileDraft> myProfileDraft() async {
    final result = await _request(
      _origin.resolve('/api/v1/me/profile'),
      authenticated: true,
    );
    if (result == null) throw const FormatException('Profile draft missing');
    return ProfileDraft.fromJson(result);
  }

  Future<Map<String, ProfileReview>> saveProfile(
    ProfileDraft draft, {
    String? avatarThumbnail,
  }) async {
    final result = await _request(
      _origin.resolve('/api/v1/me/profile'),
      data: draft.updatePayload(thumbnail: avatarThumbnail),
      authenticated: true,
    );
    if (result?['saved'] != true) {
      throw const FormatException('Profile update was not confirmed');
    }
    return readProfileReview(result?['review'] ?? const <String, dynamic>{});
  }

  String get _sessionCookieName {
    final prefix = _origin.host == 'test.touhouonline.com'
        ? 'touhou-test'
        : 'better-auth';
    return '${_origin.scheme == 'https' ? '__Secure-' : ''}$prefix.session_token';
  }

  Future<Map<String, dynamic>?> _request(
    Uri address, {
    Map<String, Object>? data,
    MultipartRequest? form,
    bool authenticated = false,
    Duration timeout = _timeout,
  }) async {
    final payload = await _exchange(
      address,
      data: data,
      form: form,
      authenticated: authenticated,
      timeout: timeout,
      accept: 'application/json',
      maxBytes: 32 * 1024 * 1024,
    );
    return payload.isEmpty
        ? null
        : jsonDecode(utf8.decode(payload)) as Map<String, dynamic>?;
  }

  Future<Uint8List> _exchange(
    Uri address, {
    Map<String, Object>? data,
    MultipartRequest? form,
    required bool authenticated,
    required Duration timeout,
    required String accept,
    required int maxBytes,
    void Function(ContentType?)? onContentType,
  }) async {
    if (address.origin != _origin.origin || !address.path.startsWith('/api/')) {
      throw const FormatException('Invalid account endpoint');
    }
    if (_origin.scheme != 'https' &&
        !['127.0.0.1', 'localhost', '::1'].contains(_origin.host)) {
      throw const FormatException('Account endpoint requires HTTPS');
    }
    final changed = data != null || form != null;
    final cookie = _sessionCookie;
    final request = await _client
        .openUrl(changed ? 'POST' : 'GET', address)
        .timeout(_timeout);
    request.followRedirects = false;
    request.headers.set(HttpHeaders.acceptHeader, accept);
    if (authenticated && cookie != null && _validCookie(cookie)) {
      request.cookies.add(cookie);
    }
    if (changed) request.headers.set('Origin', _origin.origin);

    Future<Uint8List> transfer() async {
      if (form != null) {
        final stream = form.finalize();
        request.contentLength = form.contentLength;
        request.headers.set(
          HttpHeaders.contentTypeHeader,
          form.headers['content-type']!,
        );
        await request.addStream(stream);
      } else if (data != null) {
        request.headers.contentType = ContentType.json;
        request.write(jsonEncode(data));
      }
      final response = await request.close();
      final buffer = BytesBuilder(copy: false);
      final ok = response.statusCode >= 200 && response.statusCode < 300;
      if (response.statusCode == HttpStatus.unauthorized &&
          authenticated &&
          cookie != null &&
          identical(_sessionCookie, cookie)) {
        await _writeSessionCookie(null);
      }
      final limit = ok ? maxBytes : 1024 * 1024;
      if (response.contentLength > limit) {
        throw const FormatException('Account response too large');
      }
      await for (final chunk in response) {
        if (buffer.length + chunk.length > limit) {
          throw const FormatException('Account response too large');
        }
        buffer.add(chunk);
      }
      final bytes = buffer.takeBytes();
      if (!ok) {
        String? code;
        try {
          final json = jsonDecode(utf8.decode(bytes));
          if (json is Map<String, dynamic>) {
            final error = json['error'];
            final value =
                json['code'] ??
                (error is Map<String, dynamic> ? error['code'] : null);
            if (value is String) code = value;
          }
        } on FormatException {
          // Account gateways may return a plain error body.
        }
        throw AccountRequestException(
          response.statusCode,
          code ?? 'ACCOUNT_REQUEST_FAILED',
        );
      }
      if ((authenticated || changed) && identical(_sessionCookie, cookie)) {
        for (final updated in response.cookies) {
          if (updated.name == _sessionCookieName) {
            final maxAge = updated.maxAge;
            if (maxAge != null && maxAge > 0) {
              updated.expires = DateTime.now().add(Duration(seconds: maxAge));
            }
            final next = _validCookie(updated) ? updated : null;
            if (next?.toString() != cookie?.toString()) {
              await _writeSessionCookie(next);
            }
            break;
          }
        }
      }
      onContentType?.call(response.headers.contentType);
      return bytes;
    }

    try {
      return await transfer().timeout(timeout);
    } catch (_) {
      request.abort();
      rethrow;
    }
  }

  Future<bool> requiresMailCaptcha() async {
    final ready = await _request(_origin.resolve('/api/ready'));
    if (ready?['ready'] != true) {
      throw const AccountRequestException(503, 'ACCOUNT_API_UNAVAILABLE');
    }
    return ready?['mailCaptcha'] == true;
  }

  Future<Map<String, dynamic>> mailCaptchaChallenge() async {
    final result = await _request(
      _origin.resolve('/api/account/mail-captcha/challenge'),
      data: const {},
    );
    if (result?['code'] != 200 || result?['data'] is! Map<String, dynamic>) {
      throw const AccountRequestException(503, 'CAPTCHA_UNAVAILABLE');
    }
    return result!['data'] as Map<String, dynamic>;
  }

  Future<String?> checkMailCaptcha(
    String id,
    Map<String, Object> gesture,
  ) async {
    final result = await _request(
      _origin.resolve('/api/account/mail-captcha/check'),
      data: {'id': id, 'data': gesture},
    );
    if (result?['code'] != 200) return null;
    if (result?['data'] case {'id': final String ticket}
        when ticket.isNotEmpty) {
      return ticket;
    }
    throw const AccountRequestException(503, 'CAPTCHA_UNAVAILABLE');
  }

  Future<RegistrationChallenge> startRegistration({
    required String email,
    required String username,
    required String nickname,
    required String password,
    required String? captcha,
  }) async {
    final result = await _request(
      _origin.resolve('/api/account/register/start'),
      data: {
        'email': email,
        'username': username,
        'name': nickname,
        'password': password,
        'captchaToken': ?captcha,
      },
    );
    if (result?['challengeId'] case final String id when id.isNotEmpty) {
      return (id: id, expiresAt: _challengeExpiry(result));
    }
    throw const AccountRequestException(502, 'ACCOUNT_RESPONSE_INVALID');
  }

  DateTime _challengeExpiry(Map<String, dynamic>? result) {
    if (result?['expiresAt'] case final int millis) {
      return DateTime.fromMillisecondsSinceEpoch(millis);
    }
    throw const AccountRequestException(502, 'ACCOUNT_RESPONSE_INVALID');
  }

  Future<RegistrationChallenge> resendRegistration({
    required RegistrationChallenge challenge,
    required String email,
    required String password,
    required String? captcha,
  }) async {
    final result = await _request(
      _origin.resolve('/api/account/register/resend'),
      data: {
        'challengeId': challenge.id,
        'email': email,
        'password': password,
        'captchaToken': ?captcha,
      },
    );
    return (id: challenge.id, expiresAt: _challengeExpiry(result));
  }

  Future<void> verifyRegistration({
    required RegistrationChallenge challenge,
    required String email,
    required String password,
    required String otp,
  }) async {
    await _request(
      _origin.resolve('/api/account/register/verify'),
      data: {
        'challengeId': challenge.id,
        'email': email,
        'password': password,
        'otp': otp,
      },
    );
  }

  Future<void> requestPasswordReset(String email, String? captcha) async {
    final result = await _request(
      _origin.resolve('/api/auth/email-otp/request-password-reset'),
      data: {'email': email, 'captchaToken': ?captcha},
    );
    if (result?['success'] != true) {
      throw const AccountRequestException(502, 'ACCOUNT_RESPONSE_INVALID');
    }
  }

  Future<void> resetPassword(String email, String otp, String password) async {
    final result = await _request(
      _origin.resolve('/api/auth/email-otp/reset-password'),
      data: {'email': email, 'otp': otp, 'password': password},
    );
    if (result?['success'] != true) {
      throw const AccountRequestException(502, 'ACCOUNT_RESPONSE_INVALID');
    }
  }

  Future<String?> roomIdentityTicket(String challenge) async {
    final result = await _request(
      _origin.resolve('/api/v1/me/room-nickname'),
      data: {'challenge': challenge},
      authenticated: true,
    );
    final ticket = result?['ticket'];
    return ticket is String && ticket.isNotEmpty && ticket.length <= 4096
        ? ticket
        : null;
  }

  Future<void> requestEmailChange(String email, String? captcha) async {
    final result = await _request(
      _origin.resolve('/api/auth/email-otp/request-email-change'),
      data: {'newEmail': email, 'captchaToken': ?captcha},
      authenticated: true,
    );
    if (result?['success'] != true) {
      throw const AccountRequestException(502, 'ACCOUNT_RESPONSE_INVALID');
    }
  }

  Future<void> changeEmail(String email, String otp) async {
    final result = await _request(
      _origin.resolve('/api/auth/email-otp/change-email'),
      data: {'newEmail': email, 'otp': otp},
      authenticated: true,
    );
    if (result?['success'] != true) {
      throw const AccountRequestException(502, 'ACCOUNT_RESPONSE_INVALID');
    }
  }

  Future<void> changePassword(String current, String password) async {
    final result = await _request(
      _origin.resolve('/api/auth/change-password'),
      data: {
        'currentPassword': current,
        'newPassword': password,
        'revokeOtherSessions': true,
      },
      authenticated: true,
    );
    if (result?['user'] is! Map<String, dynamic>) {
      throw const AccountRequestException(502, 'ACCOUNT_RESPONSE_INVALID');
    }
  }

  Future<AccountIdentity> signIn(String identity, String password) async {
    final kind = identity.contains('@') ? 'email' : 'username';
    final previous = _sessionCookie;
    try {
      await _request(
        _origin.resolve('/api/auth/sign-in/$kind'),
        data: {
          kind: identity,
          'password': password,
          'rememberMe': canRestoreSession,
        },
      );
      if (await session() case final AccountIdentity user) return user;
      throw const AccountRequestException(401, 'SIGN_IN_REQUIRED');
    } catch (_) {
      if (!identical(_sessionCookie, previous) && _sessionCookie != null) {
        await _writeSessionCookie(null);
      }
      rethrow;
    }
  }

  Future<AccountIdentity?> session() async {
    final cookie = _sessionCookie;
    if (cookie == null) return null;
    if (!_validCookie(cookie)) {
      await _writeSessionCookie(null);
      return null;
    }
    Map<String, dynamic>? result;
    try {
      result = await _request(
        _origin.resolve('/api/auth/get-session'),
        authenticated: true,
      );
    } on AccountRequestException catch (error) {
      if (error.status != HttpStatus.unauthorized) rethrow;
      return null;
    }
    if (result?['user'] case {
      'username': final String username,
      'name': final String name,
    }) {
      final sessionData = result?['session'];
      final expiry = sessionData is Map<String, dynamic>
          ? sessionData['expiresAt']
          : null;
      final expiresAt = expiry is String ? DateTime.tryParse(expiry) : null;
      if (expiresAt == null || expiresAt.isAfter(DateTime.now())) {
        return (username: username, name: name, expiresAt: expiresAt);
      }
    }
    if (identical(_sessionCookie, cookie)) await _writeSessionCookie(null);
    return null;
  }

  Future<void> signOut() async {
    if (_sessionCookie != null) {
      try {
        await _request(
          _origin.resolve('/api/auth/sign-out'),
          data: const {},
          authenticated: true,
        );
      } on AccountRequestException catch (error) {
        if (error.status != HttpStatus.unauthorized) rethrow;
      }
    }
    await _writeSessionCookie(null);
  }

  Future<AnnouncementPage> announcements({int? before}) async {
    var address = _origin.resolve('/api/v1/announcements');
    if (before != null) {
      address = address.replace(queryParameters: {'before': '$before'});
    }
    final page = (await _request(
      address,
      authenticated: _sessionCookie != null,
    ))!;
    return (
      items: (page['items'] as List<dynamic>)
          .cast<Map<String, dynamic>>()
          .map(PublicAnnouncement.fromJson)
          .toList(growable: false),
      nextBefore: page['nextBefore'] as int?,
      canPublish: page['canPublish'] as bool,
    );
  }

  Future<void> publishAnnouncement(String body, String requestId) async {
    await _request(
      _origin.resolve('/api/v1/announcements'),
      data: {'body': body, 'requestId': requestId},
      authenticated: true,
    );
  }

  static bool isMessageUsername(String value) =>
      value != '.' &&
      value != '..' &&
      RegExp(r'^[\w.-]{1,32}$').hasMatch(value);

  Uri _threadAddress(String username, {int? before, bool read = false}) {
    if (!isMessageUsername(username)) {
      throw const FormatException('Invalid conversation username');
    }
    final postfix = read ? '/read' : '';
    return _cursor(
      '/api/v1/me/messages/${Uri.encodeComponent(username)}$postfix',
      before,
    );
  }

  Uri _cursor(String path, int? before) {
    final address = _origin.resolve(path);
    return before == null
        ? address
        : address.replace(queryParameters: {'before': '$before'});
  }

  Future<InboxPage> inbox({int? before, bool summary = false}) async {
    final address = _cursor('/api/v1/me/messages', before);
    final page = (await _request(
      summary ? address.replace(queryParameters: {'summary': '1'}) : address,
      authenticated: true,
    ))!;
    return (
      threads: (page['threads'] as List<dynamic>)
          .cast<Map<String, dynamic>>()
          .map(
            (row) => (
              peer: _peer(row),
              lastMessage: DirectMessage.fromJson(
                row['lastMessage'] as Map<String, dynamic>,
              ),
              unread: row['unread'] as int,
            ),
          )
          .toList(growable: false),
      unread: page['unread'] as int,
      nextBefore: page['nextBefore'] as int?,
    );
  }

  Future<ConversationPage> conversation(String username, {int? before}) async {
    final page = (await _request(
      _threadAddress(username, before: before),
      authenticated: true,
    ))!;
    return (
      peer: _peer(page['peer'] as Map<String, dynamic>),
      messages: (page['messages'] as List<dynamic>)
          .cast<Map<String, dynamic>>()
          .map(DirectMessage.fromJson)
          .toList(growable: false),
      nextBefore: page['nextBefore'] as int?,
    );
  }

  Future<void> sendMessage(
    String username,
    String body,
    String requestId,
  ) async {
    await _request(
      _threadAddress(username),
      data: {'body': body, 'requestId': requestId},
      authenticated: true,
    );
  }

  Future<void> markRead(String username, int through) async {
    await _request(
      _threadAddress(username, read: true),
      data: {'through': through},
      authenticated: true,
    );
  }

  Uri _profileRoute(
    String username, {
    String? child,
    Map<String, String>? query,
  }) {
    final name = username.toLowerCase();
    if (!isPublicUsername(name)) {
      throw const AccountRequestException(404, 'PROFILE_NOT_FOUND');
    }
    return _origin.replace(
      pathSegments: ['api', 'v1', 'profiles', name, ?child],
      queryParameters: query,
    );
  }

  Future<PublicProfile> profile(String username, {bool fresh = false}) async {
    final key = username.toLowerCase();
    final now = DateTime.now();
    final cached = _profiles[key];
    if (!fresh &&
        cached != null &&
        now.difference(cached.requestedAt) < const Duration(seconds: 45)) {
      return cached.result;
    }
    final address = _profileRoute(key, query: {'media': 'urls'});
    final read = _request(address).then((json) {
      if (json == null || json['username'] != key) {
        throw const FormatException('Profile identity does not match');
      }
      return PublicProfile.fromJson(json, (kind, value) {
        if (value is! String || value.isEmpty) return null;
        final media = _origin.resolve(value);
        final segments = media.pathSegments;
        return media.origin == _origin.origin &&
                media.userInfo.isEmpty &&
                segments.length == 7 &&
                media.path.startsWith('/api/v1/profiles/$key/media/$kind/') &&
                RegExp(r'^[a-f0-9]{32}$').hasMatch(segments.last)
            ? media
            : null;
      });
    });
    _profiles[key] = (requestedAt: now, result: read);
    try {
      return await read;
    } catch (_) {
      if (identical(_profiles[key]?.result, read)) _profiles.remove(key);
      rethrow;
    }
  }

  Future<Uri?> avatar(String username) async {
    try {
      return (await profile(username)).avatar;
    } catch (_) {
      return null;
    }
  }

  Future<ProfileSocial> social(
    String username, {
    required bool signedIn,
    ProfileList? list,
  }) async {
    final result = await _request(
      _profileRoute(
        username,
        child: 'social',
        query: list == null ? {'summary': '1'} : {'list': list.name},
      ),
      authenticated: signedIn,
    );
    return ProfileSocial.fromJson(result!, list);
  }

  Future<void> setFollowing(String username, bool follow) async {
    if (!isPublicUsername(username)) {
      throw const AccountRequestException(404, 'PROFILE_NOT_FOUND');
    }
    final response = await _request(
      _origin.resolve('/api/v1/me/following'),
      data: {'username': username, 'follow': follow},
      authenticated: true,
    );
    if (response?['following'] != follow) {
      throw const FormatException('Follow state does not match');
    }
  }

  Future<Map<String, CloudGameSave>> cloudGameSaves() async =>
      (await cloudSaveDirectory()).statistics;

  Uri _cloudRoute([String? product, String operation = '']) {
    if (product == null) return _origin.resolve('/api/v1/me/cloud-saves');
    if (!cloudProductId(product)) {
      throw const FormatException('Invalid save product');
    }
    return _origin.resolve('/api/v1/me/cloud-saves/$product$operation');
  }

  Future<CloudSaveDirectory> cloudSaveDirectory() async {
    final result = await _request(_cloudRoute(), authenticated: true);
    if (result == null) throw const FormatException('Missing cloud saves');
    return CloudSaveDirectory.fromJson(result);
  }

  MultipartRequest _saveForm(
    Uri address,
    List<CloudFilePayload> files, {
    int? revision,
  }) {
    const CloudSaveLimits().validate(files);
    return MultipartRequest('POST', address)
      ..fields.addAll({if (revision != null) 'baseRevision': '$revision'})
      ..files.addAll([
        for (final file in files)
          MultipartFile.fromBytes('files', file.bytes, filename: file.name),
      ]);
  }

  Future<CloudSaveSnapshot> uploadCloudSave(
    CloudSaveProduct product,
    List<CloudFilePayload> files,
  ) async {
    final address = _cloudRoute(product.id);
    final json = await _request(
      address,
      form: _saveForm(address, files, revision: product.revision),
      authenticated: true,
      timeout: const Duration(seconds: 90),
    );
    if (json == null) throw const FormatException('Missing uploaded save');
    final snapshot = CloudSaveSnapshot.fromJson(json, product);
    if (snapshot.record.revision <= product.revision) {
      throw const FormatException('Cloud revision did not advance');
    }
    return snapshot;
  }

  Future<SaveStatistics> previewCloudSave(
    String product,
    List<CloudFilePayload> files,
  ) async {
    final address = _cloudRoute(product, '/preview');
    final json = await _request(
      address,
      form: _saveForm(address, files),
      authenticated: true,
      timeout: const Duration(milliseconds: 2500),
    );
    final stats = json?['statistics'];
    if (json?['product'] != product ||
        stats is! Map<String, dynamic> ||
        stats['game'] != product ||
        stats['parserVersion'] != 1) {
      throw const FormatException('Missing save preview');
    }
    return SaveStatistics.fromJson(stats);
  }

  Future<int> deleteCloudSave(CloudSaveProduct product) async {
    final json = await _request(
      _cloudRoute(product.id, '/delete'),
      data: {'baseRevision': product.revision},
      authenticated: true,
    );
    final revision = json?['revision'];
    if (revision is! int || revision <= product.revision) {
      throw const FormatException('Cloud revision did not advance');
    }
    return revision;
  }

  Future<Uint8List> downloadCloudSave(CloudSaveSnapshot snapshot) => _exchange(
    _cloudRoute(snapshot.product, '/${snapshot.record.revision}/download'),
    authenticated: true,
    timeout: const Duration(seconds: 90),
    accept: 'application/zip',
    maxBytes: cloudArchiveLimit,
  );

  Future<ProfileStatistics> profileStatistics(
    String username, {
    required bool owner,
  }) async {
    final result = await _request(
      _profileRoute(username, child: 'statistics'),
      authenticated: owner,
    );
    return ProfileStatistics.fromJson(result!);
  }

  Uri _th09ReplayRoute(String duel, [String? round]) {
    if (!RegExp(
          r'^[0-9a-f]{8}(?:-[0-9a-f]{4}){3}-[0-9a-f]{12}$',
          caseSensitive: false,
        ).hasMatch(duel) ||
        round != null &&
            !RegExp(r'^[0-9]{1,16}:[0-9]{1,16}$').hasMatch(round)) {
      throw const FormatException('Invalid th09 replay identifier');
    }
    return _origin.replace(
      pathSegments: [
        'api',
        'v1',
        'th09',
        'duels',
        duel,
        'replays',
        ?round?.replaceFirst(':', '-'),
      ],
    );
  }

  Future<List<Th09DuelReplay>> th09DuelReplays(String duel) async {
    final result = await _request(_th09ReplayRoute(duel), authenticated: true);
    return th09DuelReplaysFromJson(result!['files']);
  }

  Future<({Uint8List bytes, String filename})> downloadTh09Replay(
    String duel,
    Th09DuelReplay round,
  ) async {
    ContentType? type;
    final bytes = await _exchange(
      _th09ReplayRoute(duel, round.id),
      authenticated: true,
      timeout: _timeout,
      accept: 'application/octet-stream, application/x-th09-rpy, application/x-th09-rpyx',
      maxBytes: th09ReplayLimit,
      onContentType: (contentType) => type = contentType,
    );
    if (bytes.isEmpty || round.bytes != null && bytes.length != round.bytes) {
      throw const FormatException('Incomplete th09 replay');
    }
    final extension = switch (type?.mimeType) {
      'application/x-th09-rpy' => 'rpy',
      'application/x-th09-rpyx' => 'rpyx',
      'application/octet-stream' || 'application/x-th09-replay' => 'th09r',
      _ => throw const FormatException('Unexpected th09 replay response'),
    };
    final fallback = 'th09-$duel-${round.id.replaceFirst(':', '-')}.$extension';
    final preferred = round.name == null
        ? fallback
        : '${round.name}.$extension';
    return (
      bytes: bytes,
      filename: !preferred.contains('/') && isGameFilePath(preferred)
          ? preferred
          : fallback,
    );
  }

  Future<Th09Board> th09Board() async {
    final result = await _request(
      _origin.resolve('/api/v1/th09/ratings/board'),
    );
    return Th09Board.fromJson(result!);
  }

  Uri? th09BoardAvatar(String username, String? source) {
    if (source == null ||
        source.length > 11000 ||
        !isPublicUsername(username)) {
      return null;
    }
    if (RegExp(r'^data:image/(?:webp|jpeg);base64,[A-Za-z0-9+/]+={0,2}$')
        .hasMatch(source)) {
      final parsed = Uri.tryParse(source);
      try {
        return parsed?.data?.contentAsBytes() == null ? null : parsed;
      } on FormatException {
        return null;
      }
    }
    final prefix = '/api/v1/profiles/${Uri.encodeComponent(username)}/avatar/';
    if (source.startsWith(prefix) &&
        RegExp(r'^[a-f0-9]{16}$').hasMatch(source.substring(prefix.length))) {
      return _origin.resolve(source);
    }
    return null;
  }

  Future<Th09LobbyRatings> th09LobbyRatings(
    Iterable<String> usernames, {
    required bool authenticated,
  }) async {
    final roster =
        usernames
            .map((name) => name.toLowerCase())
            .where(isPublicUsername)
            .toSet()
            .toList()
          ..sort();
    final url = _origin
        .resolve('/api/v1/th09/ratings')
        .replace(
          queryParameters: roster.isEmpty
              ? null
              : {'cards': roster.take(400).join(',')},
        );
    final result = await _request(url, authenticated: authenticated);
    return Th09LobbyRatings.fromJson(result ?? const {});
  }

  Future<DuelHistory> duels(
    String username, {
    required bool owner,
    String? cursor,
  }) async {
    final result = await _request(
      _profileRoute(
        username,
        child: 'th09-duels',
        query: cursor == null ? null : {'cursor': cursor},
      ),
      authenticated: owner,
    );
    return DuelHistory.fromJson(result!);
  }

  void close() {
    _profileChanges.close();
    _client.close(force: true);
  }
}
