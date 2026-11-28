import 'dart:convert';
import 'dart:io';

import 'package:flutter_secure_storage/flutter_secure_storage.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:touhouonline/account_api.dart';
import 'package:touhouonline/profile_draft.dart';

typedef _Request = ({
  String method,
  String path,
  String? origin,
  Object? body,
  String? cookie,
});

void main() {
  test(
    'Shared avatar reads versioned profile media and retries failed lookups',
    () async {
      final server = await HttpServer.bind(InternetAddress.loopbackIPv4, 0);
      final origin = Uri.http('127.0.0.1:${server.port}');
      final api = AccountApi(origin: origin);
      var reads = 0;
      String? token = 'aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa';
      var status = HttpStatus.ok;
      server.listen((request) async {
        reads++;
        expect(request.uri.path, '/api/v1/profiles/fixture');
        expect(request.uri.queryParameters, {'media': 'urls'});
        final reply = request.response;
        reply.statusCode = status;
        reply.headers.contentType = ContentType.json;
        reply.write(
          jsonEncode({
            'username': 'fixture',
            'nickname': '头像样例',
            'media': {
              'avatar': token == null
                  ? null
                  : '/api/v1/profiles/fixture/media/avatar/$token',
            },
          }),
        );
        await reply.close();
      });
      try {
        final values = await Future.wait([
          api.avatar('fixture'),
          api.avatar('FIXTURE'),
        ]);
        expect(reads, 1);
        expect(
          values,
          everyElement(
            origin.resolve('/api/v1/profiles/fixture/media/avatar/$token'),
          ),
        );
        expect((await api.profile('fixture')).avatar, values.first);
        expect(reads, 1);
        token = 'bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb';
        api.invalidateProfile('fixture');
        expect(
          await api.avatar('fixture'),
          origin.resolve('/api/v1/profiles/fixture/media/avatar/$token'),
        );
        expect(reads, 2);
        token = null;
        api.invalidateProfile('fixture');
        expect(await api.avatar('fixture'), isNull);
        expect(await api.avatar('fixture'), isNull);
        expect(reads, 3);
        status = HttpStatus.notFound;
        api.invalidateProfile('fixture');
        expect(await api.avatar('fixture'), isNull);
        expect(reads, 4);
        status = HttpStatus.ok;
        token = 'cccccccccccccccccccccccccccccccc';
        expect(
          await api.avatar('fixture'),
          origin.resolve('/api/v1/profiles/fixture/media/avatar/$token'),
        );
        expect(reads, 5);
      } finally {
        api.close();
        await server.close(force: true);
      }
    },
  );

  test(
    'Saved session is verified, rotated and deleted across clients',
    () async {
      FlutterSecureStorage.setMockInitialValues({});
      const storage = FlutterSecureStorage();
      final server = await HttpServer.bind(InternetAddress.loopbackIPv4, 0);
      addTearDown(() => server.close(force: true));
      final origin = Uri.http('127.0.0.1:${server.port}');
      AccountApi open() {
        final api = AccountApi(origin: origin, sessionStorage: storage);
        addTearDown(api.close);
        return api;
      }

      var sessionStatus = HttpStatus.ok;
      var token = 'initial-test-session';
      var checks = 0;
      var signedOut = 0;
      void setSessionCookie(HttpResponse reply) => reply.cookies.add(
        Cookie('better-auth.session_token', token)..maxAge = 1800,
      );
      server.listen((request) async {
        final reply = request.response;
        reply.headers.contentType = ContentType.json;
        final path = request.uri.path;
        final body = await utf8.decoder.bind(request).join();
        final supplied = request.cookies.firstOrNull?.value;
        final user = {
          'user': {'username': 'fixture', 'name': 'Verified player'},
          'session': {
            'expiresAt': DateTime.now()
                .add(const Duration(hours: 1))
                .toIso8601String(),
          },
        };
        if (path == '/api/auth/sign-in/username') {
          expect(jsonDecode(body)['rememberMe'], true);
          setSessionCookie(reply);
          reply.write(jsonEncode(user));
        } else if (path == '/api/auth/get-session') {
          checks++;
          expect(supplied, token);
          reply.statusCode = sessionStatus;
          reply.write(jsonEncode(sessionStatus == HttpStatus.ok ? user : null));
        } else if (path == '/api/auth/change-password') {
          expect(supplied, token);
          token = 'rotated-test-session';
          setSessionCookie(reply);
          reply.write(jsonEncode(user));
        } else if (path == '/api/auth/sign-out') {
          expect(supplied, token);
          signedOut++;
          reply.write(jsonEncode({'success': true}));
        } else {
          reply.statusCode = HttpStatus.notFound;
        }
        await reply.close();
      });

      final first = open();
      expect(await first.restoreSession(), isNull);
      expect(checks, 0);
      await first.signIn('fixture', 'fixture-password');
      expect(checks, 1);
      await first.changePassword('fixture-password', 'fixture-new-password');
      final saved = (await storage.readAll()).values.single;
      expect(saved, contains('rotated-test-session'));
      expect(saved, isNot(contains('fixture-password')));
      expect(Cookie.fromSetCookieValue(saved).expires, isNotNull);

      final restarted = open();
      sessionStatus = HttpStatus.serviceUnavailable;
      await expectLater(
        restarted.restoreSession(),
        throwsA(isA<AccountRequestException>()),
      );
      expect((await storage.readAll()).values.single, saved);
      sessionStatus = HttpStatus.ok;
      expect((await restarted.restoreSession())?.name, 'Verified player');
      expect(checks, 3);

      final unrelated = AccountApi(
        origin: Uri.http('localhost:${server.port}'),
        sessionStorage: storage,
      );
      addTearDown(unrelated.close);
      expect(await unrelated.restoreSession(), isNull);
      expect(checks, 3);

      sessionStatus = HttpStatus.unauthorized;
      expect(await restarted.session(), isNull);
      expect(await storage.readAll(), isEmpty);
      expect(await open().restoreSession(), isNull);
      expect(checks, 4);

      sessionStatus = HttpStatus.ok;
      final signedInAgain = open();
      await signedInAgain.signIn('fixture', 'fixture-password');
      await signedInAgain.signOut();
      expect(signedOut, 1);
      expect(await storage.readAll(), isEmpty);
      expect(await open().restoreSession(), isNull);
      expect(checks, 5);
    },
  );

  test(
    'Account security mutations attach session and accept rotated cookie',
    () async {
      final server = await HttpServer.bind(InternetAddress.loopbackIPv4, 0);
      final origin = Uri.http('127.0.0.1:${server.port}');
      final api = AccountApi(origin: origin);
      final seen = <_Request>[];
      const user = {'username': 'fixture', 'name': '测试玩家'};
      server.listen((request) async {
        final raw = await utf8.decoder.bind(request).join();
        final path = request.uri.path;
        seen.add((
          method: request.method,
          path: path,
          origin: request.headers.value('Origin'),
          body: raw.isEmpty ? null : jsonDecode(raw),
          cookie: request.headers.value(HttpHeaders.cookieHeader),
        ));
        final reply = request.response;
        final Object? payload = switch (path) {
          '/api/auth/sign-in/username' ||
          '/api/auth/get-session' ||
          '/api/auth/change-password' => {'user': user},
          '/api/auth/email-otp/request-email-change' ||
          '/api/auth/email-otp/change-email' => {'success': true},
          _ => null,
        };
        final token = switch (path) {
          '/api/auth/sign-in/username' => 'before-change',
          '/api/auth/change-password' => 'after-change',
          _ => null,
        };
        if (token != null) {
          reply.cookies.add(Cookie('better-auth.session_token', token));
        }
        if (payload == null) reply.statusCode = HttpStatus.notFound;
        reply.headers.contentType = ContentType.json;
        reply.write(jsonEncode(payload));
        await reply.close();
      });

      try {
        await api.signIn('fixture', 'fixture-old-password');
        const email = 'new@example.invalid';
        await api.requestEmailChange(email, 'fixture-verification');
        await api.changeEmail(email, '123456');
        await api.changePassword(
          'fixture-old-password',
          'fixture-new-password',
        );
        expect((await api.session())?.username, 'fixture');
        expect(seen.skip(2).map((row) => row.path), [
          '/api/auth/email-otp/request-email-change',
          '/api/auth/email-otp/change-email',
          '/api/auth/change-password',
          '/api/auth/get-session',
        ]);
        expect(seen.skip(2).take(3).map((row) => row.body), [
          {'newEmail': email, 'captchaToken': 'fixture-verification'},
          {'newEmail': email, 'otp': '123456'},
          {
            'currentPassword': 'fixture-old-password',
            'newPassword': 'fixture-new-password',
            'revokeOtherSessions': true,
          },
        ]);
        for (final request in seen.skip(2).take(3)) {
          expect(request.method, 'POST');
          expect(request.cookie, contains('session_token=before-change'));
          expect(request.origin, origin.origin);
        }
        expect(seen.last.cookie, contains('session_token=after-change'));
      } finally {
        api.close();
        await server.close(force: true);
      }
    },
  );

  test(
    'Cloud saves read the authenticated current single-player revision',
    () async {
      final listener = await HttpServer.bind(InternetAddress.loopbackIPv4, 0);
      final api = AccountApi(origin: Uri.http('127.0.0.1:${listener.port}'));
      final sample = File('tools/game-progress-fixture.json')
          .readAsStringSync();
      final calls = <String?>[];
      listener.listen((request) async {
        await request.drain<void>();
        final response = request.response;
        response.headers.contentType = ContentType.json;
        if (request.uri.path == '/api/v1/me/cloud-saves') {
          final cookie = request.headers.value(HttpHeaders.cookieHeader);
          calls.add(cookie);
          if (cookie == null) response.statusCode = HttpStatus.unauthorized;
          response.write(
            cookie == null ? '{"code":"SIGN_IN_REQUIRED"}' : sample,
          );
        } else {
          if (request.uri.path == '/api/auth/sign-in/username') {
            response.cookies.add(
              Cookie('better-auth.session_token', 'profile-stat-fixture'),
            );
          }
          response.write('{"user":{"username":"fixture","name":"测试玩家"}}');
        }
        await response.close();
      });
      try {
        await api.signIn('fixture', 'fixture-password');
        final saves = await api.cloudGameSaves();
        expect(calls.single, 'better-auth.session_token=profile-stat-fixture');
        final save = saves['th07']!;
        expect(save.revision, 3);
        expect(save.statistics.playtime, 7800);
        expect(save.statistics.spells[8]?.loadouts.values.single.captures, 7);
        expect(saves, hasLength(1));
      } finally {
        api.close();
        await listener.close(force: true);
      }
    },
  );

  test(
    'Private profile reads and writes use the same account session',
    () async {
      final transport = await HttpServer.bind(InternetAddress.loopbackIPv4, 0);
      final origin = Uri.http('127.0.0.1:${transport.port}');
      final api = AccountApi(origin: origin);
      final fixture = jsonDecode(
        File('tools/profile-editor-fixture.json').readAsStringSync(),
      ) as Map<String, dynamic>;
      final seen = <_Request>[];
      transport.listen((request) async {
        final serialized = await utf8.decoder.bind(request).join();
        final path = request.uri.path;
        final method = request.method;
        seen.add((
          method: method,
          path: path,
          origin: request.headers.value('Origin'),
          body: serialized.isEmpty ? null : jsonDecode(serialized),
          cookie: request.headers.value(HttpHeaders.cookieHeader),
        ));
        final response = request.response;
        final Object? payload = switch ((path, method)) {
          ('/api/auth/sign-in/username', 'POST') ||
          ('/api/auth/get-session', 'GET') => {
            'user': {'username': 'fixture', 'name': '测试玩家'},
          },
          ('/api/v1/me/profile', 'GET') => fixture,
          ('/api/v1/me/profile', 'POST') => {
            'saved': true,
            if (seen
                    .where((row) => row.path == path && row.method == method)
                    .length ==
                1)
              'review': {'name': 'pending'},
          },
          _ => null,
        };
        if (path == '/api/auth/sign-in/username') {
          response.cookies.add(Cookie('better-auth.session_token', 'fixture'));
        }
        if (payload == null) response.statusCode = HttpStatus.notFound;
        response.headers.contentType = ContentType.json;
        response.write(jsonEncode(payload));
        await response.close();
      });

      try {
        await api.signIn('fixture', 'fixture-password');
        final draft = await api.myProfileDraft();
        const thumbnail = 'data:image/webp;base64,Zml4dHVyZQ==';
        expect(await api.saveProfile(draft, avatarThumbnail: thumbnail), {
          'name': ProfileReview.pending,
        });
        expect(await api.saveProfile(draft), isEmpty);
        final requests = seen
            .where((row) => row.path == '/api/v1/me/profile')
            .toList();
        expect(requests.map((row) => row.method), ['GET', 'POST', 'POST']);
        expect(
          requests.map((row) => row.cookie),
          everyElement(contains('session_token=fixture')),
        );
        expect(requests[1].origin, origin.origin);
        final sent = requests[1].body as Map<String, dynamic>;
        final expected = Map<String, dynamic>.of(
          fixture['draft'] as Map<String, dynamic>,
        );
        expected.remove('roomAvatarReady');
        expected['avatarThumbnail'] = thumbnail;
        expect(sent, expected);
      } finally {
        api.close();
        await transport.close(force: true);
      }
    },
  );

  test(
    'Registration and reset follow the existing account HTTP contract',
    () async {
      final transport = await HttpServer.bind(InternetAddress.loopbackIPv4, 0);
      final origin = Uri.http('127.0.0.1:${transport.port}');
      final api = AccountApi(origin: origin);
      final seen = <_Request>[];
      const email = 'sample@example.invalid';
      const password = 'fixture-password';
      const id = 'test-registration';
      const expiry = 1800000000000;
      transport.listen((request) async {
        final serialized = await utf8.decoder.bind(request).join();
        seen.add((
          method: request.method,
          path: request.uri.path,
          origin: request.headers.value('Origin'),
          body: serialized.isEmpty ? null : jsonDecode(serialized),
          cookie: request.headers.value(HttpHeaders.cookieHeader),
        ));
        final response = request.response;
        final Object? payload = switch (request.uri.path) {
          '/api/ready' => {'ready': true, 'mailCaptcha': true},
          '/api/account/register/start' => {
            'challengeId': id,
            'expiresAt': expiry,
          },
          '/api/account/register/resend' => {'expiresAt': expiry},
          '/api/account/register/verify' => {'status': true},
          '/api/auth/get-session' => {
            'user': {'username': 'sample_01', 'name': '测试玩家'},
          },
          '/api/auth/email-otp/request-password-reset' ||
          '/api/auth/email-otp/reset-password' => {'success': true},
          _ => null,
        };
        if (payload == null) response.statusCode = 404;
        if (request.uri.path == '/api/account/register/verify') {
          response.cookies.add(Cookie('better-auth.session_token', 'fixture'));
        }
        response.headers.contentType = ContentType.json;
        response.write(jsonEncode(payload));
        await response.close();
      });

      try {
        expect(await api.requiresMailCaptcha(), isTrue);
        final challenge = await api.startRegistration(
          email: email,
          username: 'sample_01',
          nickname: '测试玩家',
          password: password,
          captcha: 'fixture-start',
        );
        expect(challenge.id, id);
        expect(challenge.expiresAt.millisecondsSinceEpoch, expiry);
        expect(
          await api.resendRegistration(
            challenge: challenge,
            email: email,
            password: password,
            captcha: 'fixture-resend',
          ),
          challenge,
        );
        await api.verifyRegistration(
          challenge: challenge,
          email: email,
          password: password,
          otp: '123456',
        );
        expect((await api.session())?.username, 'sample_01');
        await api.requestPasswordReset(email, 'fixture-reset');
        await api.resetPassword(email, '234567', password);

        expect(seen.map((request) => request.path), [
          '/api/ready',
          '/api/account/register/start',
          '/api/account/register/resend',
          '/api/account/register/verify',
          '/api/auth/get-session',
          '/api/auth/email-otp/request-password-reset',
          '/api/auth/email-otp/reset-password',
        ]);
        expect(
          seen
              .where((request) => request.method == 'POST')
              .map((request) => request.origin),
          everyElement(origin.origin),
        );
        expect(seen[1].body, {
          'email': email,
          'username': 'sample_01',
          'name': '测试玩家',
          'password': password,
          'captchaToken': 'fixture-start',
        });
        expect(seen[2].body, {
          'challengeId': id,
          'email': email,
          'password': password,
          'captchaToken': 'fixture-resend',
        });
        expect(seen[3].body, {
          'challengeId': id,
          'email': email,
          'password': password,
          'otp': '123456',
        });
        expect(seen[4].cookie, 'better-auth.session_token=fixture');
        expect(seen[5].body, {'email': email, 'captchaToken': 'fixture-reset'});
        expect(seen[6].body, {
          'email': email,
          'otp': '234567',
          'password': password,
        });
      } finally {
        api.close();
        await transport.close(force: true);
      }
    },
  );
}
