import 'dart:async';
import 'dart:convert';
import 'dart:io';

import 'package:crypto/crypto.dart';
import 'package:flutter/foundation.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:shared_preferences/shared_preferences.dart';
import 'package:shared_preferences_platform_interface/in_memory_shared_preferences_async.dart';
import 'package:shared_preferences_platform_interface/shared_preferences_async_platform_interface.dart';
import 'package:touhouonline/catalog.dart';
import 'package:touhouonline/lobby_directory.dart';
import 'package:touhouonline/room_chat.dart';
import 'package:touhouonline/room_session.dart';
import 'package:uuid/uuid.dart';

const _game = Game(
  id: 'th06',
  number: '06',
  title: '東方紅魔郷',
  subtitle: 'the Embodiment of Scarlet Devil',
  category: BookCategory.windows,
  nameOrder: 0,
  multiplayer: true,
  multiplayerRules: MultiplayerRules(
    product: 'th06mp',
    playerCounts: [2, 3],
    difficulties: ['Easy', 'Normal', 'Hard', 'Lunatic', 'Extra'],
    challengeMode: true,
  ),
);
const _other = Game(
  id: 'th07',
  number: '07',
  title: '東方妖々夢',
  subtitle: 'Perfect Cherry Blossom',
  category: BookCategory.windows,
  nameOrder: 1,
  multiplayer: true,
);

Future<void> _until(
  Listenable service,
  bool Function() check,
  String stage,
) async {
  if (check()) return;
  final done = Completer<void>();
  void listener() {
    if (!done.isCompleted && check()) done.complete();
  }

  service.addListener(listener);
  try {
    await done.future.timeout(
      const Duration(seconds: 8),
      onTimeout: () => throw StateError('未观察到 $stage'),
    );
  } finally {
    service.removeListener(listener);
  }
}

const _ticketKey = 'isolated-room-test-credential-fixture';

String _ticket(String challenge) {
  final body = base64Url
      .encode(
        utf8.encode(
          jsonEncode([
            challenge,
            '灵梦',
            DateTime.now()
                .add(const Duration(seconds: 30))
                .millisecondsSinceEpoch,
            'fixture-user',
            'fixture-session',
            {'username': 'fixture', 'avatar': '4e65c008de4b549d'},
          ]),
        ),
      )
      .replaceAll('=', '');
  final signature = Hmac(
    sha256,
    utf8.encode(_ticketKey),
  ).convert(utf8.encode('room-nickname-v1:$body'));
  return '$body.${base64Url.encode(signature.bytes).replaceAll('=', '')}';
}

Future<Map<String, Object?>> _readJson(HttpRequest request) async =>
    (jsonDecode(await utf8.decoder.bind(request).join()) as Map)
        .cast<String, Object?>();

void main() {
  test('room snapshot validates capacity and preserves observer data', () {
    final data = {
      'playerCount': 2,
      'difficulty': 99,
      'phase': 'lobby',
      'visibility': 'private',
      'seats': [
        {'clientId': 'fixture-host', 'name': '房主\u202e', 'loadout': 0},
      ],
      'spectators': [
        {'clientId': 'fixture-watcher', 'name': '若鹭姬'},
      ],
    };
    final room = RoomSnapshot.parse(data, _game)!;
    expect(room.options.private, isTrue);
    expect(room.options.difficulty, 4);
    expect(room.seats, hasLength(2));
    expect(room.seats[0]!.name, '房主');
    expect(room.seats[1], isNull);
    expect(room.spectators.single.name, '若鹭姬');
    expect(RoomSnapshot.parse({...data, 'playerCount': 5}, _game), isNull);
    expect(RoomSnapshot.parse({...data, 'phase': 'unknown'}, _game), isNull);
    expect(
      RoomRequest(game: _game, code: '0104').location,
      '/lobby/room/th06/0104',
    );
    expect(RoomRequest.newCode(['1000']), isNot('1000'));
  });

  test(
    'native rooms round-trip against the pinned real Relay',
    () async {
      final relayScript = File('../services/relay/src/netplay-relay.mjs');
      final accountBridge = await HttpServer.bind(
        InternetAddress.loopbackIPv4,
        0,
      );
      addTearDown(() => accountBridge.close(force: true));
      final history = <Map<String, Object?>>[];
      final reports = <Map<String, Object?>>[];
      accountBridge.listen((request) async {
        final response = request.response;
        response.headers.contentType = ContentType.json;
        Object body;
        switch (request.uri.pathSegments.last) {
          case 'events':
            await Future<void>.delayed(const Duration(milliseconds: 150));
            body = [];
          case 'history':
            body = {'throughId': history.length, 'entries': history};
          case 'status':
            body = {'bannedUntil': 0};
          case 'messages':
            final value = await _readJson(request);
            body = {
              ...value,
              'type': 'free-chat',
              'messageId': history.length + 1,
              'name': '灵梦',
              'authorId': value['userId'],
            };
            history.add(body as Map<String, Object?>);
          case 'reports':
            reports.add(await _readJson(request));
            body = {'ok': true};
          default:
            response.statusCode = HttpStatus.notFound;
            body = {'code': 'not-found'};
        }
        response.write(jsonEncode(body));
        await response.close();
      });
      final node = await Process.start(
        'node',
        [relayScript.absolute.path],
        environment: {
          for (final name in ['PATH', 'SystemRoot', 'TEMP', 'TMP'])
            if (Platform.environment[name] case final String value) name: value,
          'EAGLER_NETPLAY_RELAY_HOST': '127.0.0.1',
          'EAGLER_NETPLAY_RELAY_PORT': '0',
          'EAGLER_NETPLAY_STUN_URLS': '',
          'ACCOUNT_RELAY_SECRET': _ticketKey,
          'ACCOUNT_API_INTERNAL_URL': 'http://127.0.0.1:${accountBridge.port}',
        },
        includeParentEnvironment: false,
      );
      final port = Completer<int>();
      final stdout = node.stdout
          .transform(utf8.decoder)
          .transform(const LineSplitter())
          .listen((line) {
            final match = RegExp(r'listening ws://127\.0\.0\.1:(\d+)')
                .firstMatch(line);
            if (match != null && !port.isCompleted) {
              port.complete(int.parse(match.group(1)!));
            }
          });
      final log = StringBuffer();
      final stderr = node.stderr.transform(utf8.decoder).listen(log.write);
      final published =
          (jsonDecode(File('assets/game_catalog.json').readAsStringSync())
                  as List)
              .cast<Map<String, dynamic>>()
              .map(Game.fromJson)
              .where(
                (game) =>
                    const ['th05', 'th08', 'th09', 'th15'].contains(game.id),
              );
      final relayGames = [_game, _other, ...published];
      HttpServer? manifest;
      LobbyDirectory? directory;
      LobbyDirectory? recoveredDirectory;
      final sessions = <RoomSession>[];

      try {
        final relayPort = await port.future.timeout(
          const Duration(seconds: 8),
          onTimeout: () => throw StateError('Relay startup failed: $log'),
        );
        final endpoint = Uri(scheme: 'ws', host: '127.0.0.1', port: relayPort);
        manifest = await HttpServer.bind(InternetAddress.loopbackIPv4, 0);
        final origin = Uri.http('127.0.0.1:${manifest.port}');
        manifest.listen((request) async {
          request.response.headers.contentType = ContentType.json;
          request.response.write(
            jsonEncode({
              'schema': 'eagler-touhou/site-manifest/1',
              'shared': {'netplayRelay': endpoint.toString()},
              'games': {
                for (final game in relayGames)
                  game.id: {'multiplayerRuntime': 'runtime.json'},
              },
            }),
          );
          await request.response.close();
        });
        SharedPreferencesAsyncPlatform.instance =
            InMemorySharedPreferencesAsync.empty();
        final live = LobbyDirectory(
          relayGames,
          origin: origin,
          preferences: SharedPreferencesAsync(),
        );
        directory = live;
        live.start();
        await _until(live, () => live.roomRelay != null, '大厅目录');
        final access = live.roomRelay!;
        const code = '4157';
        final voice = <RoomSession, List<String>>{};
        RoomSession connect(
          RoomRequest request, {
          bool host = false,
          RoomInputLoader? readInput,
        }) {
          final spoken = <String>[];
          final client = RoomSession(
            relay: host
                ? access
                : (
                    endpoint: access.endpoint,
                    origin: origin,
                    member: const Uuid().v4(),
                    products: access.products,
                  ),
            request: request,
            readInput:
                readInput ??
                (_, {required restrictMovement}) async =>
                    (movementMode: 'joystick', touchEnabled: false),
            onPhrase: spoken.add,
            identityTicket: host
                ? (challenge) async => _ticket(challenge)
                : null,
          );
          sessions.add(client);
          voice[client] = spoken;
          client.start();
          return client;
        }

        final host = connect(
          const RoomRequest(
            game: _game,
            code: code,
            create: (
              players: 2,
              difficulty: 1,
              private: false,
              limitMovement: true,
              unrated: false,
            ),
          ),
          host: true,
        );
        await _until(host, () => host.ownsRoom, '创建房间并入座 P1');
        await _until(
          live,
          () => live.rooms.any((room) => room.code == code),
          '大厅公开房间',
        );
        expect(host.snapshot!.options.limitMovement, isTrue);

        var guestMovement = 'touch-unlimited';
        var movementDecisions = 0;
        final guest = connect(
          const RoomRequest(game: _other, code: code),
          readInput: (_, {required restrictMovement}) async {
            if (restrictMovement && guestMovement == 'touch-unlimited') {
              movementDecisions++;
              return null;
            }
            return (movementMode: guestMovement, touchEnabled: true);
          },
        );
        await _until(guest, () => guest.connected, '房号按作品重定向');
        expect(guest.game.id, _game.id);
        expect(guest.localSeat, isNull);
        await guest.takeSeat(1);
        expect(movementDecisions, 1);
        expect(guest.localSeat, isNull);
        expect(host.snapshot!.seats[1], isNull);
        guestMovement = 'touch';
        await guest.takeSeat(1);
        await _until(guest, () => guest.localSeat == 1, '加入 P2');
        await _until(host, () => host.snapshot!.seats[1] != null, 'P1 看到 P2');
        expect(host.snapshot!.seats[1]!.movement, RoomMovement.touch);
        guestMovement = 'joystick';
        await guest.refreshMovement();
        await _until(
          host,
          () => host.snapshot!.seats[1]!.movement == RoomMovement.normal,
          'P1 收到 P2 更新的移动方式',
        );

        final viewer = connect(
          const RoomRequest(game: _game, code: code, watch: true),
        );
        await _until(viewer, () => viewer.watching, '新会话进旁观席');
        await _until(
          host,
          () => host.snapshot!.spectators.length == 1,
          '房主看到旁观者',
        );
        expect(viewer.localSeat, isNull);
        expect(guest.chat.phrase('wait', guest.snapshot!.serial), isTrue);
        expect(guest.chat.phrase('thanks', guest.snapshot!.serial), isFalse);
        await _until(host.chat, () => host.chat.messages.isNotEmpty, '玩家快捷发言');
        expect(host.chat.messages.single.phrase, 'wait');
        expect(viewer.chat.phrase('thanks', viewer.snapshot!.serial), isTrue);
        await _until(
          guest.chat,
          () => guest.chat.messages.length == 2,
          '旁观者快捷发言',
        );
        expect(guest.chat.messages.last.phrase, 'thanks');
        expect(guest.chat.messages.last.spectator, isTrue);
        expect(voice[guest], ['wait']);
        expect(voice[host], ['wait']);
        await _until(
          viewer.chat,
          () => viewer.chat.messages.length == 2,
          '旁观者收到双方发言',
        );
        expect(voice[viewer], unorderedEquals(['wait', 'thanks']));
        expect(guest.chat.sendText('游客文字', guest.snapshot!.serial), isFalse);
        await _until(host.chat, () => host.chat.registered, '账号签发和发言资格');
        expect(host.chat.sendText('等待下一位玩家', host.snapshot!.serial), isTrue);
        await _until(host.chat, () => !host.chat.busy, '发言回执');
        await _until(
          guest.chat,
          () => guest.chat.messages.lastOrNull?.id == 1,
          '游客接收文字',
        );
        expect(guest.chat.messages.last.text, '等待下一位玩家');
        expect(history.single['userId'], 'fixture-user');
        expect(history.single['sessionId'], 'fixture-session');
        expect(guest.chat.report(guest.chat.messages.last, '隔离测试'), isTrue);
        await _until(guest.chat, () => !guest.chat.busy, '举报回执');
        expect(guest.chat.result, RoomChatResult.reportSent);
        expect(reports.single['messageId'], 1);
        expect(reports.single['reason'], '隔离测试');
        final second = host.snapshot!.seats[1]!;
        host.manageSeat(1, second.id, remove: false);
        await _until(
          guest,
          () => guest.watching && guest.localSeat == null,
          '移到旁观席',
        );

        host.changeOptions(
          (
            players: 3,
            difficulty: 2,
            private: true,
            limitMovement: true,
            unrated: false,
          ),
          rules: {
            RoomRule.challengeMode: true,
            RoomRule.challengeKeepBullets: true,
          },
        );
        await _until(
          viewer,
          () => viewer.snapshot?.options.players == 3,
          '房主选项同步',
        );
        expect(viewer.snapshot!.options.difficulty, 2);
        expect(viewer.snapshot!.rules[RoomRule.challengeMode], isTrue);
        expect(viewer.snapshot!.rules[RoomRule.challengeKeepBullets], isTrue);
        await _until(
          live,
          () => !live.rooms.any((room) => room.code == code),
          '私人房间退出公开目录',
        );
        final removed = host.snapshot!.spectators.firstWhere(
          (entry) => entry.id == second.id,
        );
        host.removeSpectator(removed.id);
        await _until(
          guest,
          () => guest.connection == RoomConnection.closed,
          '房主移出旁观者',
        );
        expect(guest.notice, RoomProblem.removed);

        await viewer.leave();
        await _until(host, () => host.snapshot!.spectators.isEmpty, '旁观者主动离房');
        host.changeOptions((
          players: 2,
          difficulty: 1,
          private: false,
          limitMovement: true,
          unrated: false,
        ));
        await _until(
          live,
          () => live.rooms.any((room) => room.code == code),
          '房间重新公开',
        );
        final successor = connect(RoomRequest(game: _game, code: code));
        await _until(successor, () => successor.connected, '接任者进入房间');
        successor.takeSeat(1);
        await _until(successor, () => successor.localSeat == 1, '接任者入座 P2');
        successor.changeOptions(
          successor.snapshot!.options,
          rules: {RoomRule.challengeMode: false},
        );
        expect(successor.ownsRoom, isFalse);
        await host.leave();
        await _until(
          successor,
          () => successor.snapshot!.seats.first == null,
          '原房主离开后 P1 空出',
        );
        expect(successor.localSeat, 1);
        expect(successor.snapshot!.rules[RoomRule.challengeMode], isTrue);
        successor.takeSeat(0);
        await _until(successor, () => successor.ownsRoom, '主动接任 P1');
        expect(successor.snapshot!.seats[1], isNull);
        successor.changeOptions(
          successor.snapshot!.options,
          rules: {RoomRule.challengeMode: false},
        );
        await _until(
          successor,
          () => successor.snapshot!.rules[RoomRule.challengeMode] == false,
          '新房主关闭挑战模式',
        );
        expect(
          successor.snapshot!.rules[RoomRule.challengeKeepBullets],
          isFalse,
        );
        await successor.leave();
        await _until(
          live,
          () => !live.rooms.any((room) => room.code == code),
          '房主主动离房并释放席位',
        );
        for (final (title, rules) in [
          ('th05', {RoomRule.initialSpares: 4, RoomRule.initialBombs: 1}),
          ('th09', {RoomRule.disableLocalChargePriority: false}),
          ('th15', {RoomRule.chapterThreshold: 2500000}),
        ]) {
          final game = relayGames.singleWhere((game) => game.id == title);
          final session = connect(
            RoomRequest(
              game: game,
              code: '50${game.number}',
              create: (
                players: 2,
                difficulty: 1,
                private: true,
                limitMovement: true,
                unrated: false,
              ),
            ),
          );
          await _until(session, () => session.ownsRoom, '创建 $title 私人房间');
          final options = session.snapshot!.options;
          expect(
            rules.entries.any(
              (entry) => session.snapshot!.rules[entry.key] != entry.value,
            ),
            isTrue,
            reason: '$title defaults ${session.snapshot!.rules}',
          );
          session.changeOptions(options, rules: rules);
          await _until(
            session,
            () => rules.entries.every(
              (entry) => session.snapshot!.rules[entry.key] == entry.value,
            ),
            '$title 作品规则写入并回读',
          );
          await session.leave();
        }
        final timedGame = relayGames.singleWhere((game) => game.id == 'th08');
        final timeOwner = connect(
          RoomRequest(
            game: timedGame,
            code: '5080',
            create: (
              players: 2,
              difficulty: 1,
              private: true,
              limitMovement: true,
              unrated: false,
            ),
          ),
        );
        await _until(timeOwner, () => timeOwner.ownsRoom, 'th08 房主入座');
        final timeGuest = connect(RoomRequest(game: timedGame, code: '5080'));
        await _until(timeGuest, () => timeGuest.connected, 'th08 客户端加入');
        expect(timeGuest.snapshot!.timing, (rollback: true, frames: null));
        timeOwner.changeTiming((rollback: false, frames: 9));
        await _until(
          timeGuest,
          () => timeGuest.snapshot?.timing == (rollback: false, frames: 9),
          '访客回读 th08 的 9 帧输入延迟',
        );
        await _until(
          timeOwner,
          () => timeOwner.snapshot?.timing == (rollback: false, frames: 9),
          '房主收到时序回执',
        );
        timeGuest.changeTiming((rollback: true, frames: null));
        timeOwner.changeTiming((rollback: false, frames: 10));
        timeOwner.changeTiming((rollback: false, frames: null));
        await _until(
          timeGuest,
          () => timeGuest.snapshot?.timing == (rollback: false, frames: null),
          '访客回读 th08 的自动实测设置',
        );
        expect(timeGuest.notice, isNull);
        expect(timeOwner.notice, isNull);

        await _until(live, () => live.occupiedRoomCode == null, '前一房间已清理');
        final leftOpen = connect(
          const RoomRequest(
            game: _game,
            code: '4162',
            create: (
              players: 2,
              difficulty: 1,
              private: true,
              limitMovement: false,
              unrated: false,
            ),
          ),
          host: true,
        );
        await _until(leftOpen, () => leftOpen.ownsRoom, '创建待恢复的私人房间');
        final recovered = recoveredDirectory = LobbyDirectory(
          relayGames,
          origin: origin,
          preferences: SharedPreferencesAsync(),
        );
        recovered.start();
        await _until(
          recovered,
          () => recovered.occupiedRoomCode == '4162',
          '新大厅读取原房间',
        );
        expect(recovered.rooms.any((room) => room.code == '4162'), isFalse);
        expect(recovered.canReleaseMembership, isTrue);
        expect(
          await recovered.releaseMembership(),
          LobbyReleaseResult.released,
        );
        expect(recovered.occupiedRoomCode, isNull);
        await _until(
          leftOpen,
          () => leftOpen.connection == RoomConnection.closed,
          '原房间连接已断开',
        );
        expect(timeGuest.connected, isTrue);
      } finally {
        for (final session in sessions.reversed) {
          await session.leave();
          session.dispose();
        }
        directory?.dispose();
        recoveredDirectory?.dispose();
        await manifest?.close(force: true);
        node.kill();
        await node.exitCode.timeout(const Duration(seconds: 5));
        await stdout.cancel();
        await stderr.cancel();
      }
    },
    timeout: const Timeout(Duration(minutes: 1)),
    skip: Platform.environment['TOUHOU_VERIFY_RELAY_ROOMS'] != '1',
  );
}
