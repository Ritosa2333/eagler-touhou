import 'dart:convert';
import 'dart:io';

import 'package:flutter/services.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:shared_preferences/shared_preferences.dart';
import 'package:material_ui/material_ui.dart';
import 'package:prism_ui/prism_ui.dart' as prism;
import 'package:shared_preferences_platform_interface/in_memory_shared_preferences_async.dart';
import 'package:shared_preferences_platform_interface/shared_preferences_async_platform_interface.dart';
import 'package:touhouonline/catalog.dart';
import 'package:touhouonline/game_preferences.dart';
import 'package:touhouonline/game_resources.dart';
import 'package:touhouonline/game_settings.dart';
import 'package:touhouonline/game_user_files.dart';
import 'package:touhouonline/l10n/strings.dart';
import 'package:touhouonline/launcher_navigation.dart';
import 'package:touhouonline/library.dart';
import 'package:touhouonline/lobby_directory.dart';
import 'package:touhouonline/lobby_page.dart';
import 'package:touhouonline/room_page.dart';
import 'package:touhouonline/room_phrases.dart';
import 'package:touhouonline/room_quick_picker.dart';
import 'package:touhouonline/th09_rating_chart.dart';
import 'package:touhouonline/touch_layout_editor.dart';

import '../tools/capture_native_baseline_test.dart' show loadFonts, save;
import 'lobby_page_test.dart' show RatingFixtureApi;
import 'widget_test.dart' as launcher;

const _game = Game(
  id: 'th06',
  number: '06',
  title: '東方紅魔郷',
  subtitle: 'the Embodiment of Scarlet Devil',
  category: BookCategory.windows,
  nameOrder: 0,
  multiplayer: true,
  artwork: 'th06.webp',
  multiplayerRules: MultiplayerRules(
    product: 'th06mp',
    playerCounts: [2, 3],
    difficulties: ['Easy', 'Normal', 'Hard', 'Lunatic', 'Extra'],
    challengeMode: true,
  ),
);

class _WebSocketOverride extends HttpOverrides {}

class _RoomServer {
  _RoomServer(this.server, this.fixture, {this.placeSeat = true}) {
    server.listen(_upgrade);
  }
  final HttpServer server;
  final Map<String, dynamic> fixture;
  final bool placeSeat;
  final requests = <Map<String, Object?>>[];
  final commands = <Map<String, dynamic>>[];
  final clients = <WebSocket>[];
  final movementRules = <WebSocket, void Function(bool)>{};
  String? leaveReason;

  Uri get address => Uri(scheme: 'ws', host: '127.0.0.1', port: server.port);

  Future<void> _upgrade(HttpRequest request) async {
    final socket = await WebSocketTransformer.upgrade(request);
    requests.add(request.uri.queryParameters);
    clients.add(socket);
    final state =
        jsonDecode(jsonEncode(fixture['state'])) as Map<String, dynamic>;
    final room = state['room'] as Map<String, dynamic>;
    final seats = room['seats'] as List<dynamic>;
    final spectators = room['spectators'] as List<dynamic>;
    final mine = seats[0] as Map<String, dynamic>;
    mine['clientId'] = request.uri.queryParameters['lobby'];
    if (!placeSeat) seats[0] = null;
    void publish() => socket.add(jsonEncode(state));
    movementRules[socket] = (restricted) {
      room['disableCheatMovement'] = restricted;
      publish();
    };
    void updateMovement(Map<String, dynamic> command) {
      mine['controlMode'] = command['touchEnabled'] != true
          ? 'normal'
          : switch (command['movementMode']) {
              'touch-unlimited' => 'cheat',
              'touch' || 'joystick-free' => 'touch',
              _ => 'normal',
            };
    }

    publish();
    socket.listen((raw) {
      final command = jsonDecode(raw as String) as Map<String, dynamic>;
      commands.add(command);
      switch (command['type']) {
        case 'quick-chat':
          socket.add(
            jsonEncode({
              ...command,
              'room': request.uri.queryParameters['room'],
              'seat': 0,
              'clientId': mine['clientId'],
            }),
          );
        case 'chat-report':
          socket.add(
            jsonEncode({
              'type': 'chat-result',
              'action': 'chat-report',
              'ok': true,
            }),
          );
        case 'settings':
          room['visibility'] = command['visibility'];
          room['disableCheatMovement'] = command['disableCheatMovement'];
          for (final rule in ['challengeMode', 'challengeKeepBullets']) {
            if (command.containsKey(rule)) room[rule] = command[rule];
          }
          publish();
        case 'timing-settings':
          room['adonisMode'] = command['rollbackEnabled'] == true ? 0 : 1;
          room['timingChoice'] = command['timingChoice'];
          publish();
        case 'spectate':
          seats[0] = null;
          spectators.add(mine);
          room['spectatorCount'] = spectators.length;
          publish();
        case 'take-seat':
          spectators.remove(mine);
          seats[command['seat'] as int] = mine;
          updateMovement(command);
          publish();
        case 'movement':
          updateMovement(command);
          publish();
      }
    }, onDone: () => leaveReason = socket.closeReason);
  }

  void setMovementRestriction(bool value) {
    movementRules.values.single(value);
  }

  Future<void> close() async {
    for (final socket in clients) {
      await socket.close();
    }
    await server.close(force: true);
  }
}

class _RoomDirectory extends LobbyDirectory {
  _RoomDirectory(this.address, [this.game = _game]) : super([game]);

  final Uri address;
  final Game game;
  String? occupied;
  @override
  String? get occupiedRoomCode => occupied;
  @override
  LobbyConnection get connection => LobbyConnection.live;
  @override
  bool get globalRoomCodes => true;
  @override
  void start() {}
  @override
  RoomRelay get roomRelay => (
    endpoint: address,
    origin: Uri.http('127.0.0.1:${address.port}'),
    member: 'test-room-member',
    products: {game.multiplayerProduct: game},
  );
}

Future<void> _until(WidgetTester tester, bool Function() condition) async {
  final end = DateTime.now().add(const Duration(seconds: 6));
  while (!condition()) {
    if (DateTime.now().isAfter(end)) {
      fail('Room did not reach the expected state');
    }
    await Future<void>.delayed(const Duration(milliseconds: 25));
    await tester.pump(const Duration(milliseconds: 50));
  }
}

void _viewport(WidgetTester tester, Size size) {
  tester.view.devicePixelRatio = 1;
  tester.view.physicalSize = size;
  addTearDown(tester.view.resetPhysicalSize);
  addTearDown(tester.view.resetDevicePixelRatio);
}

Future<void> _tap(WidgetTester tester, Finder control) async {
  await tester.ensureVisible(control);
  await tester.pump();
  await tester.tap(control);
  await tester.pumpAndSettle();
}

Future<void> _enter(WidgetTester tester, LauncherStrings strings) async {
  await _tap(tester, find.text(strings.multiplayer));
  final icon = find.byTooltip(strings.roomByCode);
  await _tap(
    tester,
    icon.evaluate().isEmpty ? find.text(strings.roomByCode) : icon,
  );
  await tester.enterText(find.byType(TextField).last, '4157');
  await tester.pumpAndSettle();
  await tester.tap(find.widgetWithText(FilledButton, strings.join));
  await _until(
    tester,
    () => find.text('P1 | ${strings.roomYou}').evaluate().isNotEmpty,
  );
  await tester.pumpAndSettle();
}

Finder _prismAction(String label) => find.byWidgetPredicate(
  (widget) => widget is prism.IconButton && widget.tooltip == label,
);

const _roomCodeTitle = ValueKey('room-code-title');

const _watchToken =
    'eyJnIjoidGgwNm1wIiwiciI6IjQxNTciLCJmIjp0cnVlLCJhIjoiam9pbiIsInMiOnRydWV9';
const _joinToken =
    'eyJnIjoidGgwNm1wIiwiciI6IjQxNTciLCJmIjp0cnVlLCJhIjoiam9pbiJ9';

Future<void> _deliverLink(WidgetTester tester, String address) async {
  final channel = SystemChannels.navigation;
  await tester.binding.defaultBinaryMessenger.handlePlatformMessage(
    channel.name,
    channel.codec.encodeMethodCall(
      MethodCall('pushRouteInformation', {'location': address}),
    ),
    (_) {},
  );
  await tester.pump();
  await tester.pump();
}

void main() {
  setUp(
    () => SharedPreferencesAsyncPlatform.instance =
        InMemorySharedPreferencesAsync.withData({
          'touch-options.v1.shared': '{"touchMovementMode":"joystick"}',
        }),
  );
  final outputPath = Platform.environment['TOUHOU_ROOM_CAPTURE'];
  final inviteOutput = Platform.environment['TOUHOU_ROOM_LINK_CAPTURE'];
  final settingsOutput = Platform.environment['TOUHOU_ROOM_SETTINGS_CAPTURE'];
  final policyOutput = Platform.environment['TOUHOU_ROOM_POLICY_CAPTURE'];
  late final Map<String, dynamic> fixture;
  late final Game th05;
  setUpAll(() async {
    th05 = (await Game.load()).singleWhere((game) => game.id == 'th05');
    fixture = jsonDecode(
      File('tools/room-fixture.json').readAsStringSync(),
    ) as Map<String, dynamic>;
    if (outputPath != null ||
        inviteOutput != null ||
        settingsOutput != null ||
        policyOutput != null) {
      await loadFonts(Directory(Platform.environment['TOUHOU_CAPTURE_FONTS']!));
    }
  });

  for (final (device, size, language, suffix) in [
    ('desktop', const Size(1440, 734), LauncherLanguage.simplifiedChinese, ''),
    ('mobile', const Size(390, 844), LauncherLanguage.japanese, '-ja'),
    ('mobile-narrow', const Size(320, 640), LauncherLanguage.english, '-en'),
  ]) {
    testWidgets('$device enforces the room touch movement policy', (
      tester,
    ) async {
      _viewport(tester, size);
      final strings = lookupLauncherStrings(language.locale);
      final storage = SharedPreferencesAsync();
      final settings = GamePreferenceStore(storage);
      await settings.touch.setOption(
        'th06',
        'touchMovementMode',
        'touch-unlimited',
      );
      await settings.save(
        _game,
        GameToggle.touchEnabled.name,
        true,
        multiplayer: true,
      );
      final runner = launcher.RecordingGameLauncher();
      final boundary = GlobalKey();
      final server = (await tester.runAsync(
        () async => _RoomServer(
          await HttpServer.bind(InternetAddress.loopbackIPv4, 0),
          fixture,
        ),
      ))!;
      Future<void> until(bool Function() predicate) async {
        await tester.runAsync(() => _until(tester, predicate));
        await tester.pumpAndSettle();
      }

      Iterable<Map<String, dynamic>> sent(String type) =>
          server.commands.where((command) => command['type'] == type);
      final prompt = find.text(strings.roomMovementRequired);
      final editor = find.byType(TouchLayoutEditor);
      final gameSettings = find.byType(GameSettingsPage);
      final unsupported = find.widgetWithText(
        MenuItemButton,
        strings.touchEditorUnlimited,
      );
      Future<void> openEditor() async {
        await _tap(tester, _prismAction(strings.gameSettings));
        await _tap(tester, find.text(strings.gameTouchSettings));
        await _tap(tester, find.text(strings.gameTouchLayout));
        expect(editor, findsOneWidget);
      }

      Future<void> closeEditor() async {
        for (final modal in [editor, gameSettings]) {
          await _tap(
            tester,
            find.descendant(of: modal, matching: find.byType(BackButton)),
          );
        }
      }

      try {
        await HttpOverrides.runWithHttpOverrides(() async {
          await tester.pumpWidget(
            RepaintBoundary(
              key: boundary,
              child: launcher.fixture(
                launcher: runner,
                gamePreferences: settings,
                preferences: LauncherPreferences(
                  storage,
                  ThemeMode.dark,
                  true,
                  language: language,
                ),
                accountApi: launcher.FixtureAccountApi(),
                lobbyDirectory: _RoomDirectory(server.address),
                games: const [_game],
              ),
            ),
          );
          await tester.pumpAndSettle();
          await tester.runAsync(() => _enter(tester, strings));
          await until(() => prompt.evaluate().isNotEmpty);
          final roomCode = tester.widget<Text>(find.byKey(_roomCodeTitle));
          expect(roomCode.data, endsWith('4157'));
          expect(roomCode.semanticsLabel, strings.roomNumber('4157'));
          if (policyOutput != null) {
            final output = Directory(policyOutput)..createSync(recursive: true);
            await save(
              tester,
              boundary,
              output,
              '$device-room-touch-policy$suffix',
            );
          }
          await _tap(tester, find.text(strings.cancel));
          expect(sent('movement'), isEmpty);
          expect(
            (await settings.touch.loadOptions('th06')).movement,
            'touch-unlimited',
          );

          final start = find.widgetWithText(
            prism.FilledButton,
            strings.startGame,
          );
          bool canLaunch() =>
              tester.widget<prism.FilledButton>(start).onPressed != null;
          Future<void> startAndWaitForPrompt() async {
            await until(canLaunch);
            await tester.runAsync(() async {
              await tester.tap(start);
              await _until(tester, () => prompt.evaluate().isNotEmpty);
            });
            await tester.pumpAndSettle();
          }

          await startAndWaitForPrompt();
          await _tap(tester, find.text(strings.cancel));
          expect(runner.lastTouch, isNull);
          await startAndWaitForPrompt();
          final primary = device == 'desktop';
          await _tap(
            tester,
            find.text(primary ? strings.roomUseJoystick : strings.roomUseTouch),
          );
          await until(
            () => runner.lastTouch != null && sent('movement').isNotEmpty,
          );
          final savedMode = primary ? 'joystick' : 'touch';
          expect(runner.lastTouch!.options.movement, savedMode);
          expect(sent('movement').last['movementMode'], savedMode);
          expect(sent('movement').last['touchEnabled'], isTrue);

          if (size.width == 320) {
            await openEditor();
            final movementValue = find.text(strings.touchEditorDrag).last;
            await _tap(tester, movementValue);
            expect(
              tester.widget<MenuItemButton>(unsupported).onPressed,
              isNull,
            );
            await tester.sendKeyEvent(LogicalKeyboardKey.escape);
            await tester.pumpAndSettle();
            await tester.runAsync(
              () async => server.setMovementRestriction(false),
            );
            await until(
              () => !tester.widget<TouchLayoutEditor>(editor).restrictsMovement,
            );
            await _tap(tester, movementValue);
            expect(
              tester.widget<MenuItemButton>(unsupported).onPressed,
              isNotNull,
            );
            await _tap(tester, unsupported);
            await until(
              () => sent('movement').last['movementMode'] == 'touch-unlimited',
            );
            await tester.runAsync(
              () async => server.setMovementRestriction(true),
            );
            await until(
              () => tester.widget<TouchLayoutEditor>(editor).restrictsMovement,
            );
            await _tap(tester, find.text(strings.touchEditorUnlimited).last);
            expect(
              tester.widget<MenuItemButton>(unsupported).onPressed,
              isNull,
            );
            await tester.sendKeyEvent(LogicalKeyboardKey.escape);
            await tester.pumpAndSettle();
            await closeEditor();

            await _tap(tester, _prismAction(strings.roomWatch));
            await until(
              () => find.text(strings.roomTakeHostSeat).evaluate().isNotEmpty,
            );
            final host = find.text(strings.roomTakeHostSeat);
            await _tap(tester, host);
            expect(prompt, findsOneWidget);
            await _tap(tester, find.text(strings.cancel));
            expect(sent('take-seat'), isEmpty);
            await _tap(tester, host);
            await _tap(tester, find.text(strings.roomUseTouch));
            await until(() => sent('take-seat').isNotEmpty);
            expect(sent('take-seat').single['movementMode'], 'touch');
            expect(sent('take-seat').single['touchEnabled'], isTrue);
            expect(server.requests, hasLength(1));
          }

          await tester.runAsync(() async {
            await tester.binding.handlePopRoute();
            await _until(
              tester,
              () => find.byType(RoomPage).evaluate().isEmpty,
            );
            await _until(tester, () => server.leaveReason == 'leave room');
          });
          await tester.pumpAndSettle();
          expect(find.byType(LobbyPage), findsOneWidget);
          expect(tester.takeException(), isNull);
        }, _WebSocketOverride());
      } finally {
        await tester.pumpWidget(const SizedBox.shrink());
        await tester.runAsync(server.close);
      }
    });
  }

  for (final (device, size, language) in [
    ('desktop', const Size(1440, 734), LauncherLanguage.simplifiedChinese),
    ('mobile', const Size(390, 844), LauncherLanguage.japanese),
    ('mobile-narrow', const Size(320, 640), LauncherLanguage.english),
  ]) {
    testWidgets('$device room game settings affect only local th05 launch', (
      tester,
    ) async {
      _viewport(tester, size);
      final strings = lookupLauncherStrings(language.locale);
      final game = th05;
      final server = (await tester.runAsync(
        () async => _RoomServer(
          await HttpServer.bind(InternetAddress.loopbackIPv4, 0),
          fixture,
        ),
      ))!;
      final storage = SharedPreferencesAsync();
      final preferences = GamePreferenceStore(storage);
      final appData = Directory.systemTemp.createTempSync('touhou-room-test-');
      addTearDown(() => appData.delete(recursive: true));
      final runner = launcher.RecordingGameLauncher();
      final boundary = GlobalKey();
      final trigger = _prismAction(strings.gameSettings);
      final ruleTrigger = _prismAction(strings.roomPinnedSettings);
      final roomTitle = find.byKey(_roomCodeTitle);
      final displayHeading = find.text(strings.gameDisplaySettings);
      final settings = find.byType(GameSettingsPage);
      final bossBar = find.byKey(const ValueKey('bossBar'));
      Finder option(Th05DisplayOption value) => find.byKey(ValueKey(value));
      SwitchListTile phase(Th05DisplayOption value) =>
          tester.widget<SwitchListTile>(option(value));
      Future<void> expandDisplay() async {
        if (bossBar.evaluate().isEmpty) await _tap(tester, displayHeading);
        expect(bossBar, findsOneWidget);
      }

      try {
        await HttpOverrides.runWithHttpOverrides(() async {
          await tester.pumpWidget(
            RepaintBoundary(
              key: boundary,
              child: launcher.fixture(
                launcher: runner,
                gamePreferences: preferences,
                gameUserFiles: GameUserFiles(appDirectory: () async => appData),
                gameResources: GameResources(appDirectory: () async => appData),
                preferences: LauncherPreferences(
                  storage,
                  ThemeMode.dark,
                  true,
                  language: language,
                ),
                accountApi: launcher.FixtureAccountApi(),
                lobbyDirectory: _RoomDirectory(server.address, game),
                games: [game],
              ),
            ),
          );
          await tester.pumpAndSettle();
          await tester.runAsync(() => _enter(tester, strings));
          final scrollable = find
              .descendant(
                of: find.byKey(const PageStorageKey('room content')),
                matching: find.byType(Scrollable),
              )
              .first;
          final roomScroll = tester.state<ScrollableState>(scrollable).position;
          roomScroll.jumpTo(
            roomScroll.maxScrollExtent.clamp(0.0, 80.0).toDouble(),
          );
          await tester.pumpAndSettle();
          final scrollBefore = roomScroll.pixels;
          if (size.width == 320) expect(scrollBefore, greaterThan(0));
          final titleBefore = tester.getRect(roomTitle);
          expect(ruleTrigger, findsOneWidget);
          await _tap(tester, trigger);
          expect(settings, findsOneWidget);
          final panel = tester.getRect(settings);
          expect(panel.right, size.width - (device == 'desktop' ? 16 : 0));
          expect(panel.width, device == 'desktop' ? 440 : size.width);
          await expandDisplay();
          expect(find.text('20.6 KiB'), findsOneWidget);
          expect(find.text(strings.th05BarNative), findsOneWidget);
          expect(phase(Th05DisplayOption.phaseStars).onChanged, isNull);
          expect(phase(Th05DisplayOption.timeoutAsTime).onChanged, isNull);
          if (settingsOutput != null) {
            final context = tester.element(settings);
            final selector = tester.element(bossBar);
            final scroll = Scrollable.of(selector).position;
            final offset = scroll.pixels;
            await tester.runAsync(() async {
              await precacheImage(
                AssetImage('assets/book-covers/${game.artwork}'),
                context,
              );
            });
            await Scrollable.ensureVisible(selector, alignment: .16);
            await tester.pumpAndSettle();
            await save(
              tester,
              boundary,
              Directory(settingsOutput),
              '$device-room-game-display-th05',
            );
            scroll.jumpTo(offset);
            await tester.pumpAndSettle();
          }
          await _tap(tester, option(Th05DisplayOption.phaseMarkers));
          await _tap(tester, option(Th05DisplayOption.phaseTimeout));
          expect(phase(Th05DisplayOption.timeoutAsTime).onChanged, isNotNull);
          await _tap(tester, option(Th05DisplayOption.phaseTimeout));
          await _tap(tester, bossBar);
          await _tap(tester, find.text(strings.th05BarRing));
          expect(phase(Th05DisplayOption.phaseStars).onChanged, isNull);
          expect(phase(Th05DisplayOption.phaseTimeout).value, isTrue);
          await _tap(tester, bossBar);
          await _tap(tester, find.text(strings.th05BarEosd));
          expect(phase(Th05DisplayOption.phaseMarkers).onChanged, isNull);
          expect(phase(Th05DisplayOption.phaseMarkers).value, isFalse);
          expect(phase(Th05DisplayOption.phaseTimeout).onChanged, isNull);
          expect(phase(Th05DisplayOption.phaseTimeout).value, isTrue);
          await _tap(tester, option(Th05DisplayOption.phaseStars));
          await _tap(tester, option(Th05DisplayOption.timeoutAsTime));
          await _tap(tester, option(Th05DisplayOption.numericDream));
          if (device == 'desktop') {
            await tester.sendKeyEvent(LogicalKeyboardKey.escape);
          } else {
            await tester.binding.handlePopRoute();
          }
          await tester.pumpAndSettle();
          expect(settings, findsNothing);
          expect(tester.getRect(roomTitle), titleBefore);
          expect(roomScroll.pixels, scrollBefore);
          expect(server.requests, hasLength(1));

          await _tap(tester, trigger);
          await expandDisplay();
          expect(find.text(strings.th05BarEosd), findsOneWidget);
          expect(phase(Th05DisplayOption.phaseStars).value, isTrue);
          await _tap(tester, find.byType(BackButton));
          await tester.runAsync(() async {
            await tester.tap(find.text(strings.startGame));
            await _until(
              tester,
              () => find.text(strings.runtimeUnavailable).evaluate().isNotEmpty,
            );
          });
          expect(runner.lastMultiplayer, isTrue);
          expect(runner.lastOptions?.th05Display, {
            'bossBar': 'eosd',
            'phaseMarkers': false,
            'phaseStars': true,
            'phaseTimeout': true,
            'timeoutAsTime': true,
            'numericDream': true,
            'numericPower': false,
          });
          expect(
            (await preferences.load(game, multiplayer: false)).th05Display,
            isNull,
          );

          final watch = size.width <= 370
              ? _prismAction(strings.roomWatch)
              : find.widgetWithText(prism.TextButton, strings.roomWatch);
          await tester.tap(watch);
          await tester.runAsync(
            () => _until(
              tester,
              () => find.text(strings.roomLeaveSpectator).evaluate().isNotEmpty,
            ),
          );
          expect(ruleTrigger, findsNothing);
          await _tap(tester, trigger);
          await expandDisplay();
          expect(phase(Th05DisplayOption.phaseStars).value, isTrue);
          expect(
            server.commands.where((m) => m['type'] == 'settings'),
            isEmpty,
          );
          await _tap(tester, find.byType(BackButton));
          await tester.runAsync(() async {
            await tester.binding.handlePopRoute();
            await _until(
              tester,
              () => find.byType(RoomPage).evaluate().isEmpty,
            );
            await _until(tester, () => server.leaveReason == 'leave room');
          });
          await tester.pumpAndSettle();
          expect(find.byType(LobbyPage), findsOneWidget);
          expect(tester.takeException(), isNull);
        }, _WebSocketOverride());
      } finally {
        await tester.pumpWidget(const SizedBox.shrink());
        await tester.runAsync(server.close);
      }
    });
  }

  test('Web, Android, and legacy invite URLs share room identity and mode', () {
    final origin = Uri.https('touhouonline.com');
    String? route(String url) =>
        launcherEntryLocation(Uri.parse(url), const [_game], origin);
    expect(
      route('$origin/?j=$_watchToken&lang=ja'),
      '/lobby/room/th06/4157?watch=1',
    );
    expect(
      route('touhouonline://open/?j=$_watchToken'),
      '/lobby/room/th06/4157?watch=1',
    );
    expect(
      route('$origin/?game=th06mp&mpRoom=04157&lobbyAction=join'),
      '/lobby/room/th06/04157',
    );
    expect(
      route('$origin/?game=th06mp&mpRoom=04157&lobbyAction=join&fromLobby=1'),
      '/lobby/room/th06/04157?join=1',
    );
    expect(route('$origin/?j=$_joinToken'), '/lobby/room/th06/4157?join=1');
    expect(
      route('$origin/?game=th06mp&mpRoom=4157&lobbyAction=create'),
      roomLinkUnavailablePath,
    );
    expect(
      route('$origin/?j=$_watchToken&j=$_watchToken'),
      roomLinkUnavailablePath,
    );
    expect(route('$origin/?j=%FF'), roomLinkUnavailablePath);
    expect(route('touhouonline://open/?j=%EF%BF%BD'), roomLinkUnavailablePath);
    expect(route('https://example.invalid/?j=$_watchToken'), isNull);
    expect(route('https://user@touhouonline.com/?j=$_watchToken'), isNull);
    expect(route('touhouonline://open:87/?j=$_watchToken'), isNull);
    expect(route('$origin/?game=th06'), '/game/th06');
  });

  for (final (device, size) in [
    ('desktop', const Size(1440, 734)),
    ('mobile', const Size(390, 844)),
    ('mobile-narrow', const Size(320, 640)),
  ]) {
    testWidgets('$device invite cold start, leave and reopen', (tester) async {
      _viewport(tester, size);
      const language = LauncherLanguage.english;
      final strings = lookupLauncherStrings(language.locale);
      final server = (await tester.runAsync(
        () async => _RoomServer(
          await HttpServer.bind(InternetAddress.loopbackIPv4, 0),
          fixture,
          placeSeat: false,
        ),
      ))!;
      final directory = _RoomDirectory(server.address);
      final boundary = GlobalKey();
      const web = 'https://touhouonline.com/?j=$_watchToken&lang=ja';
      const app = 'touhouonline://open/?j=$_watchToken';
      final desktop = device == 'desktop';
      if (!desktop) {
        tester.binding.platformDispatcher.defaultRouteNameTestValue = app;
        addTearDown(
          tester.binding.platformDispatcher.clearDefaultRouteNameTestValue,
        );
      }
      try {
        await HttpOverrides.runWithHttpOverrides(() async {
          await tester.pumpWidget(
            RepaintBoundary(
              key: boundary,
              child: launcher.fixture(
                accountApi: launcher.FixtureAccountApi(),
                lobbyDirectory: directory,
                games: const [_game],
                initialLocation: desktop
                    ? launcherEntryLocation(Uri.parse(web), const [
                        _game,
                      ], directory.origin)
                    : null,
                preferences: LauncherPreferences(
                  SharedPreferencesAsync(),
                  ThemeMode.dark,
                  true,
                  language: language,
                  quickChatVolume: 0,
                ),
              ),
            ),
          );
          final spectator = find.text(strings.roomLeaveSpectator);
          await tester.runAsync(
            () => _until(tester, () => spectator.evaluate().isNotEmpty),
          );
          await tester.pumpAndSettle();
          expect(find.byKey(_roomCodeTitle), findsOneWidget);
          expect(server.requests.single['room'], 'th06mp-4157');
          expect(server.requests.single['intent'], 'join');
          expect(
            server.commands.where((row) => row['type'] == 'spectate'),
            hasLength(1),
          );
          if (inviteOutput != null) {
            await tester.runAsync(
              () => precacheImage(
                const AssetImage('assets/book-covers/th06.webp'),
                tester.element(find.byType(RoomPage)),
              ),
            );
            await tester.pumpAndSettle();
            final output = Directory(inviteOutput)..createSync(recursive: true);
            await save(tester, boundary, output, '$device-room-invite-direct');
          }
          // A second delivery of the same address keeps the admitted session.
          await _deliverLink(tester, web);
          expect(spectator, findsOneWidget);
          expect(server.requests, hasLength(1));

          final back = _prismAction('Back');
          await tester.runAsync(() async {
            if (desktop) {
              await tester.tap(back);
            } else {
              await tester.binding.handlePopRoute();
            }
            await _until(
              tester,
              () => find.byType(RoomPage).evaluate().isEmpty,
            );
            await _until(tester, () => server.leaveReason == 'leave room');
          });
          await tester.pumpAndSettle();
          final lobby = find.byType(LobbyPage);
          expect(lobby.hitTestable(), findsOneWidget);
          final lobbyState = tester.state(lobby);

          directory.occupied = '4162';
          await _deliverLink(tester, app);
          expect(find.text(strings.roomConflict), findsOneWidget);
          expect(server.requests, hasLength(1));
          await tester.tap(back);
          await tester.pumpAndSettle();
          expect(tester.state(lobby), same(lobbyState));
          directory.occupied = null;

          await _deliverLink(tester, 'touhouonline://open/?j=%EF%BF%BD');
          expect(find.text(strings.roomInviteUnavailable), findsOneWidget);
          expect(server.requests, hasLength(1));
          await _tap(tester, find.byType(BackButton));
          expect(tester.state(lobby), same(lobbyState));
          final selection = tester.widget<LobbyPage>(lobby).selection;
          selection.showOnly(_game);
          await tester.pumpAndSettle();
          await _tap(
            tester,
            find.text(size.width == 320 ? strings.home : strings.singlePlayer),
          );
          expect(lobby.hitTestable(), findsNothing);
          await _deliverLink(tester, 'touhouonline://open/?j=$_joinToken');
          final seat = find.text('P1 | ${strings.roomYou}');
          await tester.runAsync(
            () => _until(tester, () => seat.evaluate().isNotEmpty),
          );
          expect(server.requests, hasLength(2));
          expect(
            server.commands
                .where((row) => row['type'] == 'take-seat')
                .single['seat'],
            0,
          );
          await tester.runAsync(() async {
            await tester.tap(back);
            await _until(
              tester,
              () => find.byType(RoomPage).evaluate().isEmpty,
            );
          });
          await tester.pumpAndSettle();
          expect(tester.state(lobby), same(lobbyState));
          expect(selection.value, {_game.id});
          expect(tester.takeException(), isNull);
          await tester.pumpWidget(const SizedBox.shrink());
        }, _WebSocketOverride());
      } finally {
        await tester.runAsync(server.close);
      }
    });
  }

  testWidgets('A pasted web invite opens the room as a spectator', (
    tester,
  ) async {
    _viewport(tester, const Size(320, 640));
    const language = LauncherLanguage.english;
    final strings = lookupLauncherStrings(language.locale);
    // Produced by the fixed web revision's room-invite.mts.
    const token = _watchToken;
    const invite = 'https://touhouonline.com/?j=$token&lang=en';
    final messenger = tester.binding.defaultBinaryMessenger;
    messenger.setMockMethodCallHandler(
      SystemChannels.platform,
      (call) async =>
          call.method == 'Clipboard.getData' ? {'text': invite} : null,
    );
    addTearDown(
      () => messenger.setMockMethodCallHandler(SystemChannels.platform, null),
    );
    final server = (await tester.runAsync(
      () async => _RoomServer(
        await HttpServer.bind(InternetAddress.loopbackIPv4, 0),
        fixture,
        placeSeat: false,
      ),
    ))!;
    final boundary = GlobalKey();
    try {
      await HttpOverrides.runWithHttpOverrides(() async {
        await tester.pumpWidget(
          RepaintBoundary(
            key: boundary,
            child: launcher.fixture(
              accountApi: launcher.FixtureAccountApi(),
              lobbyDirectory: _RoomDirectory(server.address),
              games: const [_game],
              preferences: LauncherPreferences(
                SharedPreferencesAsync(),
                ThemeMode.dark,
                true,
                language: language,
                quickChatVolume: 0,
              ),
            ),
          ),
        );
        await tester.pumpAndSettle();
        await _tap(tester, find.text(strings.multiplayer));
        await _tap(tester, find.byTooltip(strings.roomByCode));
        final input = find.descendant(
          of: find.byType(AlertDialog),
          matching: find.byType(TextField),
        );
        final join = find.widgetWithText(FilledButton, strings.join);
        for (final invalid in [
          'https://invalid.example/?j=$token',
          'https://touhouonline.com/?j=$token&j=$token',
          'https://touhouonline.com/?j=broken',
          'https://touhouonline.com/?j=eyJnIjoidGgwNm1wIiwiciI6IjQxNTciLCJmIjp0cnVlLCJhIjoiY3JlYXRlIiwicCI6Mn0',
        ]) {
          await tester.enterText(input, invalid);
          await tester.pumpAndSettle();
          expect(find.text(strings.roomCodeOrInviteInvalid), findsOneWidget);
          expect(tester.widget<FilledButton>(join).onPressed, isNull);
        }
        expect(server.requests, isEmpty);
        await tester.enterText(input, '4157');
        await tester.pumpAndSettle();
        expect(tester.widget<FilledButton>(join).onPressed, isNotNull);
        await _tap(tester, find.byTooltip('Paste'));
        expect(
          find.text('${_game.title} | ${strings.roomNumber('4157')}'),
          findsOneWidget,
        );
        expect(tester.widget<TextField>(input).controller!.text, '4157');
        final watch = find.widgetWithText(FilledButton, strings.roomWatch);
        expect(tester.widget<FilledButton>(watch).onPressed, isNotNull);
        if (outputPath != null) {
          final output = Directory(outputPath)..createSync(recursive: true);
          await save(tester, boundary, output, 'mobile-narrow-room-invite');
        }
        await tester.runAsync(() async {
          await tester.tap(watch);
          await _until(
            tester,
            () => find.text(strings.roomLeaveSpectator).evaluate().isNotEmpty,
          );
        });
        await tester.pumpAndSettle();
        expect(find.byType(RoomPage), findsOneWidget);
        expect(find.byKey(_roomCodeTitle), findsOneWidget);
        expect(server.requests.single['intent'], 'join');
        expect(server.requests.single['room'], 'th06mp-4157');
        expect(
          server.commands.any((command) => command['type'] == 'spectate'),
          isTrue,
        );
        await tester.runAsync(() async {
          await tester.binding.handlePopRoute();
          await _until(tester, () => find.byType(RoomPage).evaluate().isEmpty);
          await _until(tester, () => server.leaveReason == 'leave room');
        });
        expect(find.byType(LobbyPage), findsOneWidget);
        expect(tester.takeException(), isNull);
      }, _WebSocketOverride());
    } finally {
      await tester.runAsync(server.close);
    }
  });

  for (final (language, device, size, theme) in [
    for (final language in LauncherLanguage.values) ...[
      (language, 'mobile-narrow', const Size(320, 640), ThemeMode.dark),
      if (outputPath != null) ...[
        (language, 'desktop', const Size(1440, 734), ThemeMode.dark),
        (language, 'mobile', const Size(390, 844), ThemeMode.dark),
      ],
    ],
    if (outputPath != null) ...[
      (
        LauncherLanguage.english,
        'desktop',
        const Size(1440, 734),
        ThemeMode.light,
      ),
      (
        LauncherLanguage.english,
        'mobile',
        const Size(390, 844),
        ThemeMode.light,
      ),
    ],
  ]) {
    final localeSuffix = language == LauncherLanguage.simplifiedChinese
        ? ''
        : '-${language.tag}';
    final suffix = theme == ThemeMode.light
        ? '$localeSuffix-light'
        : localeSuffix;
    final strings = lookupLauncherStrings(language.locale);
    testWidgets(
      '$device$suffix waiting room enters from lobby and restores it',
      (tester) async {
        _viewport(tester, size);
        final boundary = GlobalKey();
        String? copied;
        tester.binding.defaultBinaryMessenger.setMockMethodCallHandler(
          SystemChannels.platform,
          (call) async {
            if (call.method == 'Clipboard.setData') {
              copied = (call.arguments as Map)['text'] as String;
            }
            return null;
          },
        );
        addTearDown(
          () => tester.binding.defaultBinaryMessenger.setMockMethodCallHandler(
            SystemChannels.platform,
            null,
          ),
        );
        final runner = launcher.RecordingGameLauncher();
        final server = (await tester.runAsync(
          () async => _RoomServer(
            await HttpServer.bind(InternetAddress.loopbackIPv4, 0),
            fixture,
          ),
        ))!;
        try {
          await HttpOverrides.runWithHttpOverrides(() async {
            await tester.pumpWidget(
              RepaintBoundary(
                key: boundary,
                child: launcher.fixture(
                  launcher: runner,
                  preferences: LauncherPreferences(
                    SharedPreferencesAsync(),
                    theme,
                    true,
                    language: language,
                    quickChatVolume: 0,
                  ),
                  accountApi: launcher.FixtureAccountApi(),
                  lobbyDirectory: _RoomDirectory(server.address),
                  games: const [_game],
                ),
              ),
            );
            await tester.pumpAndSettle();
            await tester.runAsync(() => _enter(tester, strings));
            expect(find.byType(RoomPage), findsOneWidget);
            expect(find.byKey(_roomCodeTitle), findsOneWidget);
            expect(server.requests.single['intent'], 'join');
            expect(find.text('${strings.roomSpectators} | 1'), findsOneWidget);
            final standLabel = find.widgetWithText(
              prism.TextButton,
              strings.roomLeaveSeat,
            );
            final stand = standLabel.evaluate().isEmpty
                ? _prismAction(strings.roomLeaveSeat)
                : standLabel;
            expect(stand.hitTestable(), findsOneWidget);
            final watch = size.width <= 370
                ? _prismAction(strings.roomWatch)
                : find.widgetWithText(prism.TextButton, strings.roomWatch);
            final launch = find.text(strings.startGame);
            if (outputPath != null) {
              await save(
                tester,
                boundary,
                Directory(outputPath),
                '$device-room-actions$suffix',
              );
            }
            expect(
              tester.getCenter(stand).dy,
              closeTo(tester.getCenter(watch).dy, 3),
            );
            expect(
              tester.getCenter(launch).dy,
              closeTo(tester.getCenter(watch).dy, 3),
            );
            final cards = find.byType(prism.Card);
            final cover = tester.getRect(find.byType(GameArtwork));
            final heading = tester.getRect(find.text(_game.title));
            final subtitle = tester.getRect(find.text(_game.subtitle));
            final roomTitle = tester.getRect(find.byKey(_roomCodeTitle));
            final seats = tester.getRect(cards.at(0));
            final spectators = tester.getRect(cards.at(1));
            final chat = tester.getRect(cards.at(2));
            final playButton = tester.getRect(
              find.widgetWithText(prism.FilledButton, strings.startGame),
            );
            final wide = size.width >= 820;
            expect(heading.left, closeTo(cover.right + 12, 0.5));
            expect(subtitle.left, closeTo(heading.left, 0.5));
            expect(roomTitle.left, closeTo(heading.left, 0.5));
            expect(seats.left, closeTo(cover.left, 0.5));
            expect(spectators.left, closeTo(seats.left, 0.5));
            expect(spectators.top - seats.bottom, closeTo(16, 0.5));
            if (wide) {
              expect(chat.left - seats.right, closeTo(16, 0.5));
              expect(chat.top, closeTo(seats.top, 0.5));
              expect(playButton.right, closeTo(chat.right, 0.5));
            } else {
              expect(chat.left, closeTo(seats.left, 0.5));
              expect(chat.top - spectators.bottom, closeTo(16, 0.5));
            }
            expect(tester.takeException(), isNull);
            if (outputPath != null) {
              final output = Directory(outputPath)..createSync(recursive: true);
              final stem = '$device-room-waiting$suffix';
              await save(tester, boundary, output, stem);
              List<double> edges(Rect rect) => [
                rect.left,
                rect.top,
                rect.right,
                rect.bottom,
              ];
              File('${output.path}/$stem.geometry.json').writeAsStringSync(
                const JsonEncoder.withIndent('  ').convert({
                  'viewport': [size.width, size.height],
                  'cover': edges(cover),
                  'heading': edges(heading),
                  'subtitle': edges(subtitle),
                  'roomTitle': edges(roomTitle),
                  'playerSeats': edges(seats),
                  'spectators': edges(spectators),
                  'chat': edges(chat),
                  'playButton': edges(playButton),
                }),
              );
            }
            final historyText = find.text('等下一位玩家再开始。', findRichText: true);
            await tester.runAsync(() async {
              server.clients.single.add(jsonEncode(fixture['chatHistory']));
              await _until(tester, () => historyText.evaluate().isNotEmpty);
            });
            await tester.ensureVisible(historyText.first);
            await tester.pumpAndSettle();
            if (outputPath != null) {
              final output = Directory(outputPath);
              await save(tester, boundary, output, '$device-room-chat$suffix');
              if (language == LauncherLanguage.simplifiedChinese) {
                await _tap(tester, find.byTooltip(strings.roomQuickChat));
                await save(
                  tester,
                  boundary,
                  output,
                  '$device-room-quick-picker',
                );
                await tester.binding.handlePopRoute();
                await tester.pumpAndSettle();
              }
              await _tap(tester, _prismAction(strings.roomPinnedSettings));
              await save(tester, boundary, output, '$device-room-rules$suffix');
              await _tap(tester, find.text(strings.cancel));
            }
            if (device == 'mobile-narrow') {
              await _tap(tester, _prismAction(strings.roomInviteCopy));
              final invitation = Uri.parse(copied!);
              expect(invitation.queryParameters['lang'], language.tag);
              final token = invitation.queryParameters['j']!;
              expect(
                jsonDecode(
                  utf8.decode(base64Url.decode(base64Url.normalize(token))),
                ),
                {'g': 'th06mp', 'r': '4157', 'f': true, 'a': 'join'},
              );
              await tester.runAsync(() async {
                final first = server.requests.single;
                await server.clients.single.close(
                  WebSocketStatus.goingAway,
                  'network switch',
                );
                await _until(
                  tester,
                  () =>
                      find.text(strings.roomReconnecting).evaluate().isNotEmpty,
                );
                final retry = find.widgetWithText(
                  TextButton,
                  strings.roomRetry,
                );
                await tester.ensureVisible(retry);
                await tester.pump();
                expect(retry.hitTestable(), findsOneWidget);
                await tester.tap(retry);
                await _until(
                  tester,
                  () =>
                      server.requests.length == 2 &&
                      find.text(strings.roomWaiting).evaluate().isNotEmpty,
                );
                expect(server.requests.last['member'], first['member']);
                expect(server.requests.last['lobby'], first['lobby']);
                expect(find.text('P1 | ${strings.roomYou}'), findsOneWidget);
                await _tap(tester, find.byTooltip(strings.roomQuickChat));
                final phrase = switch (language) {
                  LauncherLanguage.simplifiedChinese =>
                    '请求给条命（停止开火，然后在我旁边连点 5 次低速即可）',
                  LauncherLanguage.english => 'Life please (stop firing, then tap focus 5 times next to me)',
                  LauncherLanguage.japanese =>
                    '残機をください（ショットを止め、隣で低速を5回押してください）',
                };
                await _tap(tester, find.text(phrase));
                await _until(
                  tester,
                  () => find.text(phrase).evaluate().isNotEmpty,
                );
                expect(
                  server.commands.any(
                    (command) =>
                        command['type'] == 'quick-chat' &&
                        command['phrase'] == 'request-life',
                  ),
                  isTrue,
                );
                expect(find.text(strings.roomChatSignIn), findsOneWidget);
                if (language == LauncherLanguage.simplifiedChinese) {
                  await _tap(tester, find.byTooltip(strings.roomQuickChat));
                  final picker = find.byType(RoomQuickPicker);
                  expect(
                    find.text(strings.roomQuickChatFavorites),
                    findsOneWidget,
                  );
                  await _tap(tester, find.byTooltip(strings.roomQuickChatEdit));
                  await _tap(
                    tester,
                    find
                        .descendant(of: picker, matching: find.byType(ListTile))
                        .first,
                  );
                  expect(
                    find.text(strings.roomQuickChatChooseSlot(1)),
                    findsOneWidget,
                  );
                  final label = roomPhraseLabel(
                    roomPhrases['wait']!,
                    language.tag,
                  );
                  final phrase = find.descendant(
                    of: picker,
                    matching: find.text(label),
                  );
                  final list = find.descendant(
                    of: picker,
                    matching: find.byType(Scrollable),
                  );
                  await tester.scrollUntilVisible(
                    phrase,
                    180,
                    scrollable: list,
                  );
                  await _tap(tester, phrase);
                  final saved = await LauncherPreferences.load(
                    SharedPreferencesAsync(),
                  );
                  expect(saved.quickChatSlots.first, 'wait');
                  await tester.binding.handlePopRoute();
                  await tester.pumpAndSettle();
                  expect(
                    find.text(strings.roomQuickChatFavorites),
                    findsOneWidget,
                  );
                  expect(phrase, findsOneWidget);
                  await Future<void>.delayed(const Duration(milliseconds: 520));
                  await tester.sendKeyEvent(
                    LogicalKeyboardKey.digit1,
                    character: '1',
                  );
                  await tester.pumpAndSettle();
                  await _until(
                    tester,
                    () => server.commands.any(
                      (command) => command['phrase'] == 'wait',
                    ),
                  );
                }
                await _tap(
                  tester,
                  find.byTooltip(strings.roomChatActions).first,
                );
                await _tap(tester, find.text(strings.roomChatReportTitle));
                await _tap(
                  tester,
                  find.widgetWithText(FilledButton, strings.roomChatReport),
                );
                await _until(
                  tester,
                  () => find
                      .text(strings.roomChatSentReport)
                      .evaluate()
                      .isNotEmpty,
                );
                expect(
                  server.commands.any((command) => command['messageId'] == 1),
                  isTrue,
                );
                await _tap(tester, _prismAction(strings.roomPinnedSettings));
                await _tap(tester, find.text(strings.roomPlayerCount(2)));
                await _tap(tester, find.text(strings.roomPlayerCount(3)));
                await _tap(tester, find.text(strings.cancel));
                expect(
                  server.commands.where(
                    (command) => command['type'] == 'settings',
                  ),
                  isEmpty,
                );
                await _tap(tester, _prismAction(strings.roomPinnedSettings));
                expect(find.text(strings.roomPlayerCount(2)), findsOneWidget);
                expect(
                  find.text(strings.roomChallengeKeepBullets),
                  findsNothing,
                );
                await _tap(tester, find.text(strings.roomPrivateOption));
                await _tap(tester, find.text(strings.roomChallengeMode));
                await _tap(tester, find.text(strings.roomChallengeKeepBullets));
                await tester.tap(find.text(strings.roomSave));
                await _until(
                  tester,
                  () => find
                      .textContaining(
                        '${strings.roomPrivate} | ${strings.roomPlayerCount(2)}',
                      )
                      .evaluate()
                      .isNotEmpty,
                );
                final rules = server.commands.lastWhere(
                  (command) => command['type'] == 'settings',
                );
                expect(rules['challengeMode'], isTrue);
                expect(rules['challengeKeepBullets'], isTrue);
                await tester.pumpAndSettle();
                await tester.tap(find.text(strings.startGame));
                await _until(
                  tester,
                  () => find
                      .text(strings.runtimeUnavailable)
                      .evaluate()
                      .isNotEmpty,
                );
                expect(runner.lastMultiplayer, isTrue);
                expect(find.byType(RoomPage), findsOneWidget);
                expect(
                  server.commands.any((message) => message['type'] == 'start'),
                  isFalse,
                );
                await tester.tap(watch);
                await _until(
                  tester,
                  () => find
                      .text('${strings.roomSpectators} | 2')
                      .evaluate()
                      .isNotEmpty,
                );
                expect(find.text(strings.roomLeaveSpectator), findsOneWidget);
                expect(_prismAction(strings.roomPinnedSettings), findsNothing);
                await _tap(tester, find.text(strings.roomTakeHostSeat));
                await _until(
                  tester,
                  () =>
                      _prismAction(strings.roomPinnedSettings)
                          .evaluate()
                          .isNotEmpty,
                );
                expect(find.text('P1 | ${strings.roomYou}'), findsOneWidget);
              });
            }
            await tester.runAsync(() async {
              await tester.binding.handlePopRoute();
              await _until(
                tester,
                () => find.byType(RoomPage).evaluate().isEmpty,
              );
              await _until(tester, () => server.leaveReason == 'leave room');
            });
            expect(find.byType(LobbyPage), findsOneWidget);
            expect(tester.takeException(), isNull);
          }, _WebSocketOverride());
        } finally {
          await tester.runAsync(server.close);
        }
      },
      variant: TargetPlatformVariant.only(TargetPlatform.windows),
    );
  }

  for (final (device, size) in [
    ('mobile-narrow', const Size(320, 640)),
    if (outputPath != null) ...[
      ('mobile', const Size(390, 844)),
      ('desktop', const Size(1440, 734)),
    ],
  ]) {
    testWidgets(
      '$device room keeps the seat after rating card and profile navigation',
      (tester) async {
        _viewport(tester, size);
        final strings = lookupLauncherStrings(
          LauncherLanguage.simplifiedChinese.locale,
        );
        final boundary = GlobalKey();
        final ratings = jsonDecode(
          File('tools/lobby-ratings-fixture.json').readAsStringSync(),
        ) as Map<String, dynamic>;
        final state = fixture['state'] as Map<String, dynamic>;
        final room = state['room'] as Map<String, dynamic>;
        final rated = (ratings['rooms'] as List).cast<Map<String, dynamic>>();
        Object? profile(String code) =>
            (rated.singleWhere((row) => row['code'] == code)['seats'] as List)
                .first['profile'];
        final peers = (room['seats'] as List).cast<Map<String, dynamic>>();
        final audience = (room['spectators'] as List)
            .cast<Map<String, dynamic>>();
        final game =
            (jsonDecode(File('assets/game_catalog.json').readAsStringSync())
                    as List)
                .cast<Map<String, dynamic>>()
                .map(Game.fromJson)
                .singleWhere((game) => game.id == 'th09');
        final server = (await tester.runAsync(
          () async => _RoomServer(
            await HttpServer.bind(InternetAddress.loopbackIPv4, 0),
            {
              'state': {
                ...state,
                'room': {
                  ...room,
                  'seats': [
                    {...peers[0], 'profile': profile('4201')},
                    {...peers[1], 'profile': profile('4202')},
                  ],
                  'spectators': [
                    {
                      ...audience.single,
                      'name': '妖梦',
                      'profile': profile('4203'),
                    },
                  ],
                },
              },
            },
          ),
        ))!;
        final api = RatingFixtureApi(ratings);
        try {
          await HttpOverrides.runWithHttpOverrides(() async {
            await tester.pumpWidget(
              RepaintBoundary(
                key: boundary,
                child: launcher.fixture(
                  preferences: LauncherPreferences(
                    SharedPreferencesAsync(),
                    ThemeMode.dark,
                    true,
                  ),
                  lobbyDirectory: _RoomDirectory(server.address, game),
                  accountApi: api,
                  games: [game],
                ),
              ),
            );
            await tester.pumpAndSettle();
            await tester.runAsync(() => _enter(tester, strings));
            await tester.pumpAndSettle();
            expect(api.requests.first.usernames, ['other', 'rated', 'unrated']);
            expect(
              find.textContaining('局数 26', findRichText: true),
              findsOneWidget,
            );
            final button = find.byKey(const ValueKey('th09-card-rated'));
            final before = tester.getRect(button);
            await tester.tap(button);
            await tester.pumpAndSettle();
            final card = find.byKey(const ValueKey('th09-player-card'));
            final portraits = find.descendant(
              of: card,
              matching: find.byType(Th09CharacterPortrait),
            );
            expect(portraits, findsNWidgets(3));
            if (outputPath != null) {
              final context = tester.element(card);
              await tester.runAsync(
                () => Future.wait([
                  for (final id in [0, 2, 4])
                    precacheImage(
                      AssetImage('assets/th09-portraits/$id.webp'),
                      context,
                    ),
                ]),
              );
              await tester.pump();
              await save(
                tester,
                boundary,
                Directory(outputPath),
                '$device-room-rating-card',
              );
            }
            if (device == 'desktop') {
              await tester.sendKeyEvent(LogicalKeyboardKey.escape);
            } else if (device == 'mobile') {
              final link = await launcher.openAccountAndReturn(
                tester,
                find.descendant(
                  of: card,
                  matching: find.text(strings.viewProfile),
                ),
              );
              expect(link.path, startsWith('/u/'));
            } else {
              await tester.binding.handlePopRoute();
            }
            await tester.pumpAndSettle();
            expect(card, findsNothing);
            expect(tester.getRect(button), before);
            expect(find.text('P1 | ${strings.roomYou}'), findsOneWidget);
            expect(server.leaveReason, isNull);
            if (device == 'mobile') {
              await _tap(tester, find.text('${strings.roomSpectators} | 1'));
              await _tap(
                tester,
                find.byKey(const ValueKey('th09-card-unrated')),
              );
              expect(
                find.descendant(of: card, matching: find.text('@unrated')),
                findsOneWidget,
              );
              expect(portraits, findsNothing);
              await tester.binding.handlePopRoute();
              await tester.pumpAndSettle();
            }
            expect(server.commands, isEmpty);
            await tester.pumpWidget(const SizedBox.shrink());
            await tester.pumpAndSettle();
            final count = api.requests.length;
            await tester.pump(const Duration(seconds: 13));
            expect(api.requests, hasLength(count));
            expect(tester.takeException(), isNull);
          }, _WebSocketOverride());
        } finally {
          await tester.runAsync(server.close);
        }
      },
    );

    testWidgets('$device edits th08 timing through the room toolbar', (
      tester,
    ) async {
      _viewport(tester, size);
      final boundary = GlobalKey();
      final strings = lookupLauncherStrings(
        LauncherLanguage.simplifiedChinese.locale,
      );
      final game =
          (jsonDecode(File('assets/game_catalog.json').readAsStringSync())
                  as List)
              .cast<Map<String, dynamic>>()
              .map(Game.fromJson)
              .singleWhere((game) => game.id == 'th08');
      final timing = Map<String, dynamic>.of(
        fixture['timing'] as Map<String, dynamic>,
      );
      expect(timing.remove('product'), game.multiplayerProduct);
      final state = fixture['state'] as Map<String, dynamic>;
      final timedFixture = {
        'state': {
          ...state,
          'room': {...state['room'] as Map<String, dynamic>, ...timing},
        },
      };
      final server = (await tester.runAsync(
        () async => _RoomServer(
          await HttpServer.bind(InternetAddress.loopbackIPv4, 0),
          timedFixture,
        ),
      ))!;
      try {
        await HttpOverrides.runWithHttpOverrides(() async {
          await tester.pumpWidget(
            RepaintBoundary(
              key: boundary,
              child: launcher.fixture(
                preferences: LauncherPreferences(
                  SharedPreferencesAsync(),
                  ThemeMode.dark,
                  true,
                ),
                lobbyDirectory: _RoomDirectory(server.address, game),
                accountApi: launcher.FixtureAccountApi(),
                games: [game],
              ),
            ),
          );
          await tester.pumpAndSettle();
          await tester.runAsync(() => _enter(tester, strings));
          await _tap(tester, _prismAction(strings.roomPinnedSettings));
          expect(find.text('2f'), findsOneWidget);
          final title = find.text(strings.roomPinnedSettings);
          final titleTop = tester.getTopLeft(title).dy;
          await tester.ensureVisible(
            find.text(strings.roomInputDelayAutoToggle),
          );
          await tester.pumpAndSettle();
          expect(tester.getTopLeft(title).dy, closeTo(titleTop, 0.5));
          if (outputPath != null) {
            await save(
              tester,
              boundary,
              Directory(outputPath),
              '$device-room-timing',
            );
          }
          await _tap(tester, find.byTooltip(strings.roomIncreaseDelay));
          expect(find.text('3f'), findsOneWidget);
          await _tap(tester, find.text(strings.roomRollback));
          expect(find.byTooltip(strings.roomIncreaseDelay), findsNothing);
          await _tap(tester, find.text(strings.roomRollback));
          expect(find.text('3f'), findsOneWidget);
          await _tap(tester, find.text(strings.cancel));
          expect(server.commands, isEmpty);

          await _tap(tester, _prismAction(strings.roomPinnedSettings));
          expect(find.text('2f'), findsOneWidget);
          await _tap(tester, find.text(strings.roomInputDelayAutoToggle));
          final increase = _prismAction(strings.roomIncreaseDelay);
          expect(tester.widget<prism.IconButton>(increase).onPressed, isNull);
          await tester.runAsync(() async {
            await _tap(tester, find.text(strings.roomSave));
            await _until(tester, () => server.commands.isNotEmpty);
            expect(server.commands.single, {
              'type': 'timing-settings',
              'rollbackEnabled': false,
              'timingChoice': 'auto',
            });
            await _until(
              tester,
              () => find
                  .textContaining(strings.roomInputDelayAutoToggle)
                  .evaluate()
                  .isNotEmpty,
            );
            final back = _prismAction(
              MaterialLocalizations.of(tester.element(find.byType(RoomPage)))
                  .backButtonTooltip,
            );
            await _tap(tester, back);
            await _until(
              tester,
              () => find.byType(RoomPage).evaluate().isEmpty,
            );
            await _until(tester, () => server.leaveReason == 'leave room');
          });
          expect(find.byType(LobbyPage), findsOneWidget);
          expect(tester.takeException(), isNull);
        }, _WebSocketOverride());
      } finally {
        await tester.runAsync(server.close);
      }
    });
  }
}
