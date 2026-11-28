import 'dart:async';
import 'dart:convert';
import 'dart:io';

import 'package:flutter_secure_storage/flutter_secure_storage.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:touhouonline/account.dart';
import 'package:touhouonline/account_api.dart';

class _DelayedSessions extends AccountApi {
  final replies = <Completer<AccountIdentity?>>[];
  @override
  Future<AccountIdentity?> acceptWebSession(Uri source, Cookie? cookie) {
    final reply = Completer<AccountIdentity?>();
    replies.add(reply);
    return reply.future;
  }
}

void main() {
  test(
    'A delayed browser response cannot replace a newer verified account',
    () async {
      final api = _DelayedSessions();
      final session = AccountSession(api);
      addTearDown(session.dispose);
      final origin = Uri.https('touhouonline.com');
      final old = session.syncWebSession(origin, null);
      final latest = session.syncWebSession(origin, null);
      api.replies[1].complete((username: 'new', name: '新会话', expiresAt: null));
      await latest;
      api.replies[0].complete((username: 'old', name: '旧会话', expiresAt: null));
      await old;
      expect(session.identity?.username, 'new');
      expect(session.restoreStatus, AccountRestoreStatus.idle);
    },
  );

  test(
    'Web session is verified, rotated, restored and cleared across clients',
    () async {
      FlutterSecureStorage.setMockInitialValues({});
      const storage = FlutterSecureStorage();
      final server = await HttpServer.bind(InternetAddress.loopbackIPv4, 0);
      final origin = Uri.http('127.0.0.1:${server.port}');
      final api = AccountApi(origin: origin, sessionStorage: storage);
      final session = AccountSession(api);
      var requests = 0;
      var fail = false;
      server.listen((request) async {
        requests++;
        expect(request.uri.path, '/api/auth/get-session');
        expect(request.cookies.single.name, 'better-auth.session_token');
        expect(request.cookies.single.value, anyOf('web-a', 'web-b'));
        request.response.headers.contentType = ContentType.json;
        if (fail) {
          request.response.statusCode = 503;
          request.response.write('{"code":"SERVICE_UNAVAILABLE"}');
        } else {
          request.response.cookies.add(
            Cookie('better-auth.session_token', 'web-b')..httpOnly = true,
          );
          request.response.write(
            jsonEncode({
              'user': {'username': 'fixture', 'name': '网页玩家'},
              'session': {
                'expiresAt': DateTime.now()
                    .add(const Duration(days: 1))
                    .toIso8601String(),
              },
            }),
          );
        }
        await request.response.close();
      });
      addTearDown(() async {
        session.dispose();
        await server.close(force: true);
      });
      final cookie = Cookie('better-auth.session_token', 'web-a')
        ..httpOnly = true;
      await session.syncWebSession(origin, cookie);
      expect(requests, 1);
      expect(session.identity?.username, 'fixture');
      expect(session.cloudSaveRevision.value, 1);
      expect(api.webSessionCookie?.value, 'web-b');
      final copy = api.webSessionCookie!;
      copy.value = 'changed-copy';
      expect(api.webSessionCookie?.value, 'web-b');

      final restored = AccountApi(origin: origin, sessionStorage: storage);
      expect((await restored.restoreSession())?.username, 'fixture');
      restored.close();
      expect(requests, 2);

      // Returning from the same account may follow web changes to friends or
      // cloud saves. Refresh subscribers without dropping the verified user.
      final sameAccount = session.syncWebSession(origin, api.webSessionCookie);
      expect(session.identity?.username, 'fixture');
      await sameAccount;
      expect(requests, 3);
      expect(session.socialRevision.value, 2);
      expect(session.cloudSaveRevision.value, 2);

      fail = true;
      await expectLater(
        session.syncWebSession(origin, cookie),
        throwsA(isA<AccountRequestException>()),
      );
      expect(session.identity, isNull);
      expect(session.restoreStatus, AccountRestoreStatus.failed);
      fail = false;
      await session.restore();
      expect(session.identity?.username, 'fixture');

      final count = requests;
      await session.syncWebSession(origin, null);
      expect(session.identity, isNull);
      expect(session.restoreStatus, AccountRestoreStatus.idle);
      expect(api.webSessionCookie, isNull);
      expect(requests, count);
      final afterLogout = AccountApi(origin: origin, sessionStorage: storage);
      expect(await afterLogout.restoreSession(), isNull);
      afterLogout.close();
      expect(requests, count);
    },
  );

  test('Web credentials reject another origin, cookie name, domain and insecure transport', () async {
    final api = AccountApi();
    addTearDown(api.close);
    final production = Uri.https('touhouonline.com');
    Cookie valid() =>
        Cookie('__Secure-better-auth.session_token', 'test-only')
          ..secure = true;
    for (final source in [
      Uri.https('test.touhouonline.com'),
      Uri.https('other.example'),
      Uri.http('touhouonline.com'),
      Uri.parse('https://touhouonline.com:444'),
    ]) {
      await expectLater(
        api.acceptWebSession(source, valid()),
        throwsFormatException,
      );
    }
    await expectLater(
      api.acceptWebSession(
        production,
        Cookie('different', 'test-only')..secure = true,
      ),
      throwsFormatException,
    );
    await expectLater(
      api.acceptWebSession(production, valid()..domain = 'other.example'),
      throwsFormatException,
    );
    await expectLater(
      api.acceptWebSession(production, valid()..secure = false),
      throwsFormatException,
    );
    await expectLater(
      api.acceptWebSession(production, valid()..maxAge = 0),
      throwsFormatException,
    );
    expect(api.webSessionCookie, isNull);
  });
}
