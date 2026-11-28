import 'dart:async';
import 'dart:convert';
import 'dart:io';

import 'package:flutter_test/flutter_test.dart';
import 'package:touhouonline/catalog.dart';
import 'package:touhouonline/lobby_directory.dart';

import 'widget_test.dart' as launcher;

Future<void> _until(LobbyDirectory directory, bool Function() test) async {
  if (test()) return;
  final done = Completer<void>();
  void listen() {
    if (test() && !done.isCompleted) done.complete();
  }

  directory.addListener(listen);
  try {
    await done.future.timeout(const Duration(seconds: 20));
  } finally {
    directory.removeListener(listen);
  }
}

void main() {
  test(
    'directory reads published configuration and handles live updates',
    () async {
      final loopback = await HttpServer.bind(InternetAddress.loopbackIPv4, 0);
      final origin = Uri.http('127.0.0.1:${loopback.port}');
      final server = origin.replace(scheme: 'ws', path: '/relay');
      final connected = <Uri>[];
      WebSocket? socket;
      final refreshed = Completer<Map<String, dynamic>>();
      final fixture = jsonDecode(
        File('tools/lobby-directory-fixture.json').readAsStringSync(),
      ) as Map<String, dynamic>;
      final rows = fixture['rooms'] as List;
      loopback.listen((request) async {
        if (request.uri.path == '/site-manifest.json') {
          request.response.headers.contentType = ContentType.json;
          request.response.write(
            jsonEncode({
              'schema': 'eagler-touhou/site-manifest/1',
              'games': {
                'th06': {'multiplayerRuntime': 'runtime.json'},
              },
              'shared': {'netplayRelay': server.toString()},
            }),
          );
          await request.response.close();
        } else if (request.uri.path == '/relay') {
          connected.add(request.uri);
          final peer = await WebSocketTransformer.upgrade(request);
          socket = peer;
          peer.add(jsonEncode(fixture));
          peer.listen((event) {
            refreshed.complete(
              jsonDecode(event as String) as Map<String, dynamic>,
            );
            peer.add(
              jsonEncode({
                ...fixture,
                'total': 1,
                'rooms': [rows[1], {}],
              }),
            );
          });
        } else {
          request.response.statusCode = HttpStatus.notFound;
          await request.response.close();
        }
      });

      final directory = LobbyDirectory(const [launcher.game], origin: origin);
      try {
        directory.start();
        await _until(
          directory,
          () => directory.connection == LobbyConnection.live,
        );
        expect(connected.single.queryParameters['directory'], '1');
        expect(connected.single.queryParameters['member'], isNotEmpty);
        expect(connected.single.queryParameters.containsKey('room'), isFalse);
        expect(directory.availableGameIds.value, {'th06'});
        expect(directory.globalRoomCodes, isTrue);
        expect(directory.rooms.single.code, '5601');
        expect(directory.rooms.single.seats[1]?.name, '咲夜');
        expect(directory.rooms.single.liveSpectator, isTrue);
        expect(directory.rooms.single.rules, [LobbyRule.limitMovement]);
        directory.refresh();
        expect(directory.refreshing, isTrue);
        expect(directory.rooms.single.code, '5601');
        expect(await refreshed.future.timeout(const Duration(seconds: 4)), {
          'type': 'refresh',
          'product': '',
        });
        await _until(directory, () => !directory.refreshing);
        expect(directory.total, 1);
        expect(directory.rooms.single.code, '5601');
        socket!.close();
        await _until(
          directory,
          () => directory.connection == LobbyConnection.offline,
        );
        expect(directory.rooms.single.code, '5601');
      } finally {
        directory.dispose();
        await loopback.close(force: true);
      }
    },
  );

  test('release follows Relay confirmation when membership changes', () async {
    final loopback = await HttpServer.bind(InternetAddress.loopbackIPv4, 0);
    final origin = Uri.http('127.0.0.1:${loopback.port}');
    final relay = origin.replace(scheme: 'ws', path: '/relay');
    final incoming = StreamController<Object?>();
    final requests = StreamIterator(incoming.stream);
    final connected = Completer<WebSocket>();
    Map<String, Object?> snapshot(String token, {bool supported = true}) => {
      'type': 'directory',
      'version': 1,
      'rooms': [],
      'total': 0,
      'membershipRecovery': supported,
      'mine': {'product': 'th06mp', 'code': '4162', 'recoveryToken': token},
    };
    loopback.listen((request) async {
      if (request.uri.path == '/relay') {
        final socket = await WebSocketTransformer.upgrade(request);
        connected.complete(socket);
        socket.listen((message) => incoming.add(jsonDecode(message as String)));
        socket.add(jsonEncode(snapshot('fixture-old-token', supported: false)));
      } else {
        request.response.headers.contentType = ContentType.json;
        request.response.write(
          jsonEncode({
            'schema': 'eagler-touhou/site-manifest/1',
            'games': {
              'th06': {'multiplayerRuntime': 'runtime.json'},
            },
            'shared': {'netplayRelay': relay.toString()},
          }),
        );
        await request.response.close();
      }
    });
    final directory = LobbyDirectory(const [launcher.game], origin: origin);
    Future<void> expectRelease(String token) async {
      expect(
        await requests.moveNext().timeout(const Duration(seconds: 5)),
        isTrue,
      );
      expect(requests.current, {
        'type': 'release-membership',
        'recoveryToken': token,
      });
    }

    try {
      directory.start();
      final socket = await connected.future.timeout(const Duration(seconds: 5));
      await _until(directory, () => directory.occupiedRoomCode == '4162');
      expect(directory.canReleaseMembership, isFalse);
      socket.add(jsonEncode(snapshot('fixture-old-token')));
      await _until(directory, () => directory.canReleaseMembership);

      final stale = directory.releaseMembership();
      await expectRelease('fixture-old-token');
      expect(directory.releasingMembership, isTrue);
      expect(directory.occupiedRoomCode, '4162');
      socket.add(jsonEncode(snapshot('fixture-new-token')));
      expect(await stale, LobbyReleaseResult.changed);
      expect(directory.occupiedRoomCode, '4162');

      final uncertain = directory.releaseMembership();
      await expectRelease('fixture-new-token');
      await socket.close();
      expect(await uncertain, LobbyReleaseResult.unconfirmed);
      expect(directory.occupiedRoomCode, '4162');
      expect(directory.canReleaseMembership, isFalse);
    } finally {
      directory.dispose();
      await requests.cancel();
      await incoming.close();
      await loopback.close(force: true);
    }
  });

  test('capped all-games directory requests and follows the active title', () {
    TestWidgetsFlutterBinding.ensureInitialized();
    return HttpOverrides.runWithHttpOverrides(() async {
      final games = await Game.load();
      final gameIds = {'th06', 'th09'};
      final loopback = await HttpServer.bind(InternetAddress.loopbackIPv4, 0);
      final origin = Uri.http('127.0.0.1:${loopback.port}');
      final endpoint = origin.replace(scheme: 'ws', path: '/relay');
      final connections = <WebSocket>[];
      final requests = StreamController<String>();
      final products = StreamIterator(requests.stream);
      final fixture = jsonDecode(
        File('tools/lobby-directory-fixture.json').readAsStringSync(),
      ) as Map<String, dynamic>;
      final rows = (fixture['rooms'] as List).cast<Map<String, dynamic>>();
      final th06 = rows.singleWhere((row) => row['product'] == 'th06mp');
      final th09 = rows.singleWhere((row) => row['product'] == 'th09mp');
      final bulk = [
        for (var i = 2000; i < 2200; i++) {...th06, 'code': '$i'},
      ];
      final global = {...fixture, 'total': 201, 'rooms': bulk};
      final rare = {
        ...fixture,
        'product': 'th09mp',
        'total': 1,
        'rooms': [th09],
      };
      var initial = global;
      loopback.listen((request) async {
        if (request.uri.path == '/site-manifest.json') {
          request.response.headers.contentType = ContentType.json;
          request.response.write(
            jsonEncode({
              'schema': 'eagler-touhou/site-manifest/1',
              'games': {
                for (final id in gameIds)
                  id: {'multiplayerRuntime': 'runtime.json'},
              },
              'shared': {'netplayRelay': endpoint.toString()},
            }),
          );
          await request.response.close();
        } else if (request.uri.path == '/relay') {
          final socket = await WebSocketTransformer.upgrade(request);
          connections.add(socket);
          socket.add(jsonEncode(initial));
          socket.listen((message) {
            final packet =
                jsonDecode(message as String) as Map<String, dynamic>;
            expect(packet['type'], 'refresh');
            requests.add(packet['product'] as String);
          });
        } else {
          request.response.statusCode = HttpStatus.notFound;
          await request.response.close();
        }
      });
      final directory = LobbyDirectory(
        games.where((game) => gameIds.contains(game.id)),
        origin: origin,
      );
      Future<void> requested(String product) async {
        expect(
          await products.moveNext().timeout(const Duration(seconds: 5)),
          isTrue,
        );
        expect(products.current, product);
      }

      try {
        directory.start();
        await _until(
          directory,
          () => directory.connection == LobbyConnection.live,
        );
        expect(directory.rooms, hasLength(200));
        expect(directory.total, 201);
        expect(directory.rooms.every((room) => room.game.id == 'th06'), isTrue);
        final socket = connections.single;

        directory.filterGame('th09');
        expect(directory.refreshing, isTrue);
        expect(directory.rooms, hasLength(200));
        await requested('th09mp');
        // An all-games broadcast already on the wire cannot answer this request.
        socket.add(jsonEncode(global));
        socket.add(jsonEncode(rare));
        await _until(directory, () => !directory.refreshing);
        expect(directory.publishedProduct, 'th09mp');
        expect(directory.rooms.single.code, '1048');
        expect(directory.total, 1);

        directory.refresh();
        await requested('th09mp');
        socket.add(
          jsonEncode({
            ...rare,
            'rooms': [
              {...th09, 'code': '1049'},
            ],
          }),
        );
        await _until(directory, () => !directory.refreshing);
        expect(directory.rooms.single.code, '1049');

        directory.filterGame('th06');
        await requested('th06mp');
        directory.filterGame('th09');
        socket.add(jsonEncode({...global, 'product': 'th06mp', 'total': 200}));
        await requested('th09mp');
        expect(directory.rooms.single.code, '1049');
        socket.add(jsonEncode(rare));
        await _until(directory, () => !directory.refreshing);
        expect(directory.rooms.single.code, '1048');

        directory.filterGame(null);
        await requested('');
        socket.add(jsonEncode(global));
        await _until(directory, () => !directory.refreshing);
        expect(directory.rooms, hasLength(200));
        expect(directory.total, 201);

        // Live all-games updates can make the sampled list complete again.
        initial = {
          ...fixture,
          'total': 2,
          'rooms': [th06, th09],
        };
        socket.add(jsonEncode(initial));
        await _until(directory, () => directory.total == 2);
        directory.filterGame('th09');
        expect(directory.refreshing, isFalse);
        expect(directory.rooms, hasLength(2));
        expect(connections, hasLength(1));

        initial = global;
        await socket.close();
        await _until(
          directory,
          () => directory.connection == LobbyConnection.offline,
        );
        expect(directory.rooms, hasLength(2));
        await requested('th09mp');
        expect(connections, hasLength(2));
        connections.last.add(jsonEncode(rare));
        await _until(directory, () => !directory.refreshing);
        expect(directory.rooms.single.code, '1048');
      } finally {
        directory.dispose();
        await products.cancel();
        await requests.close();
        await loopback.close(force: true);
      }
    }, _RealNetwork());
  });

  test(
    'limits room models to published capacity, safe code and occupied rows',
    () {
      final games = {launcher.game.multiplayerProduct: launcher.game};
      final valid = {
        'product': 'th06mp',
        'code': '0101',
        'capacity': 2,
        'joinable': true,
        'phase': 'lobby',
        'seats': [
          {'name': 'private label', 'initial': 'A', 'controlMode': 'touch'},
          null,
        ],
      };
      final room = LobbyRoom.read(valid, games)!;
      expect(room.canJoin, isTrue);
      expect(room.seats.first?.name, 'A');
      expect(LobbyRoomFilter.open.includes(room), isTrue);
      expect(LobbyRoomFilter.spectate.includes(room), isFalse);
      expect(LobbyRoom.read({...valid, 'code': 'bad'}, games), isNull);
      expect(LobbyRoom.read({...valid, 'capacity': 4}, games), isNull);
      expect(
        LobbyRoom.read({
          ...valid,
          'seats': [null, null],
        }, games),
        isNull,
      );
    },
  );

  test(
    'published website responds with a public room directory',
    () async {
      TestWidgetsFlutterBinding.ensureInitialized();
      final games = await Game.load();
      await HttpOverrides.runWithHttpOverrides(() async {
        final directory = LobbyDirectory(games);
        try {
          directory.start();
          await _until(
            directory,
            () => directory.connection == LobbyConnection.live,
          );
          expect(directory.availableGameIds.value, contains('th09'));
          final count = directory.rooms.length;
          directory.refresh();
          await _until(directory, () => !directory.refreshing);
          expect(directory.connection, LobbyConnection.live);
          stdout.writeln(
            'Initial public directory: $count rooms, refresh: ${directory.rooms.length}',
          );
        } finally {
          directory.dispose();
        }
      }, _RealNetwork());
    },
    skip: Platform.environment['TOUHOU_VERIFY_LIVE_LOBBY'] != '1'
        ? 'Requires TOUHOU_VERIFY_LIVE_LOBBY=1'
        : false,
  );
}

class _RealNetwork extends HttpOverrides {}
