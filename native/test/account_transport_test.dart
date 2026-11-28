import 'dart:convert';
import 'dart:io';

import 'package:flutter_secure_storage/flutter_secure_storage.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:touhouonline/account_api.dart';

void main() {
  test(
    'Large production response preserves rows and transport failures',
    () async {
      final bytes = gzip.decode(
        File('archive/performance-audit-20261128/payload-board.json.gz')
            .readAsBytesSync(),
      );
      final reference = jsonDecode(utf8.decode(bytes)) as Map<String, dynamic>;
      final players = reference['board'] as List;
      expect(players, hasLength(265));
      final server = await HttpServer.bind(InternetAddress.loopbackIPv4, 0);
      final api = AccountApi(origin: Uri.http('127.0.0.1:${server.port}'));
      addTearDown(api.close);
      addTearDown(() => server.close(force: true));
      var mode = 'board';
      server.listen((request) async {
        expect(request.uri.path, '/api/v1/th09/ratings/board');
        final response = request.response;
        response.headers.contentType = ContentType.json;
        switch (mode) {
          case 'board':
            response.add(bytes);
          case 'malformed':
            response.add(bytes.sublist(0, bytes.length - 1));
          case 'error':
            response.statusCode = HttpStatus.serviceUnavailable;
            response.write(
              jsonEncode({
                'error': {'code': 'BUSY'},
                'details': players,
              }),
            );
        }
        await response.close();
      });
      final board = await api.th09Board();
      expect(
        board.players.map((player) => player.id),
        players.map((row) => row['player']),
      );
      mode = 'malformed';
      await expectLater(api.th09Board(), throwsFormatException);
      mode = 'error';
      await expectLater(
        api.th09Board(),
        throwsA(
          isA<AccountRequestException>()
              .having(
                (error) => error.status,
                'status',
                HttpStatus.serviceUnavailable,
              )
              .having((error) => error.code, 'code', 'BUSY'),
        ),
      );
    },
  );

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
        } else if (path == '/api/v1/announcements') {
          expect(supplied, token);
          token = 'rotated-test-session';
          setSessionCookie(reply);
          reply.write(
            jsonEncode({'items': [], 'nextBefore': null, 'canPublish': false}),
          );
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
      await first.announcements();
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
}
