import 'dart:async';
import 'dart:convert';
import 'dart:io';
import 'dart:ui' show PointerDeviceKind;

import 'package:flutter_test/flutter_test.dart';
import 'package:flutter/services.dart';
import 'package:material_ui/material_ui.dart';
import 'package:prism_ui/prism_ui.dart' as prism;
import 'package:shared_preferences/shared_preferences.dart';
import 'package:shared_preferences_platform_interface/in_memory_shared_preferences_async.dart';
import 'package:shared_preferences_platform_interface/shared_preferences_async_platform_interface.dart';
import 'package:touhouonline/catalog.dart';
import 'package:touhouonline/game_file_picker.dart';
import 'package:touhouonline/game_resources.dart';
import 'package:touhouonline/game_user_files.dart';
import 'package:touhouonline/l10n/strings.dart';
import 'package:touhouonline/library.dart';
import 'package:touhouonline/lobby_directory.dart';
import 'package:touhouonline/lobby_page.dart';
import 'package:touhouonline/lobby_room_card.dart';
import 'package:touhouonline/network_check_dialog.dart';
import 'package:touhouonline/network_diagnostics.dart';
import 'package:touhouonline/profile_data.dart';
import 'package:touhouonline/replay_manager.dart';
import 'package:touhouonline/th09_rating_chart.dart';
import 'package:touhouonline/th09_ratings.dart';

import '../tools/capture_native_baseline_test.dart' show loadFonts, save;
import 'replay_match_test.dart' show matchReplayZip;
import 'widget_test.dart' as launcher;

class _LobbyFixture extends LobbyDirectory {
  _LobbyFixture(super.games, this.entries);
  final List<LobbyRoom> entries;
  int calls = 0;
  final checks = <_DiagnosticsFixture>[];
  LobbyConnection status = LobbyConnection.live;

  @override
  LobbyConnection get connection => status;
  @override
  List<LobbyRoom> get rooms => entries;
  @override
  int get total => entries.length;
  @override
  bool get globalRoomCodes => true;

  @override
  void start() {}

  @override
  void refresh() => calls++;

  @override
  NetworkDiagnostics diagnostics() {
    final check = _DiagnosticsFixture();
    checks.add(check);
    return check;
  }

  void setConnection(LobbyConnection value) {
    status = value;
    notifyListeners();
  }

  void disconnect() => setConnection(LobbyConnection.offline);

  void clearRooms() {
    entries.clear();
    setConnection(LobbyConnection.live);
  }
}

class _DiagnosticsFixture extends NetworkDiagnostics {
  _DiagnosticsFixture() : super(endpoint: null, origin: Uri.http('127.0.0.1'));

  late void Function(NetworkCheck, NetworkReading) publish;
  final _done = Completer<void>();
  var closed = false;

  @override
  Future<void> run(void Function(NetworkCheck, NetworkReading) update) {
    publish = update;
    return _done.future;
  }

  void finish() {
    publish(NetworkCheck.websocket, (
      state: NetworkCheckState.good,
      milliseconds: 38,
      nat: null,
    ));
    publish(NetworkCheck.turn, networkUnavailable);
    publish(NetworkCheck.nat, (
      state: NetworkCheckState.good,
      milliseconds: null,
      nat: 'NAT3',
    ));
    publish(NetworkCheck.ipv6, networkUnavailable);
    _done.complete();
  }

  @override
  void close() {
    closed = true;
    if (!_done.isCompleted) _done.complete();
    super.close();
  }
}

class _CappedLobbyFixture extends _LobbyFixture {
  _CappedLobbyFixture(super.games, super.entries, this.target);

  final LobbyRoom target;
  String? requestedGame;
  bool _waiting = false;
  bool _scoped = false;

  @override
  bool get refreshing => _waiting;
  @override
  List<LobbyRoom> get rooms => _scoped ? [target] : super.rooms;
  @override
  int get total => _scoped ? 1 : 201;
  @override
  String get publishedProduct => _scoped ? target.game.multiplayerProduct : '';

  @override
  void filterGame(String? gameId) {
    requestedGame = gameId;
    _waiting = true;
    notifyListeners();
  }

  void reply() {
    _scoped = requestedGame == target.game.id;
    _waiting = false;
    notifyListeners();
  }
}

class _OccupiedLobbyFixture extends _LobbyFixture {
  _OccupiedLobbyFixture(super.games, super.entries);

  @override
  String? occupiedRoomCode = '4162';
  @override
  bool supportsMembershipRelease = true;
  Completer<LobbyReleaseResult>? _reply;

  @override
  bool get releasingMembership => _reply != null;
  @override
  bool get canReleaseMembership => supportsMembershipRelease && _reply == null;

  @override
  Future<LobbyReleaseResult> releaseMembership() {
    final reply = _reply = Completer<LobbyReleaseResult>();
    notifyListeners();
    return reply.future;
  }

  void reply(LobbyReleaseResult result) {
    if (result == LobbyReleaseResult.released) occupiedRoomCode = null;
    _reply!.complete(result);
    _reply = null;
    notifyListeners();
  }

  void setRecoverySupport(bool supported) {
    supportsMembershipRelease = supported;
    notifyListeners();
  }
}

class RatingFixtureApi extends launcher.FixtureAccountApi {
  RatingFixtureApi(this.fixture);
  final Map<String, dynamic> fixture;
  final requests = <({List<String> usernames, bool authenticated})>[];
  int boardRequests = 0;
  bool failBoard = false;
  int? updatedRating;

  @override
  Future<Th09Board> th09Board() async {
    boardRequests++;
    if (failBoard) throw StateError('Isolated board failure');
    final board = fixture['board'] as Map<String, dynamic>;
    if (updatedRating case final int rating) {
      return Th09Board.fromJson({
        ...board,
        'board': [
          for (final row
              in (board['board'] as List).cast<Map<String, dynamic>>())
            row['player'] == 'rated' ? {...row, 'rating': rating} : row,
        ],
      });
    }
    return Th09Board.fromJson(board);
  }

  @override
  Future<Th09LobbyRatings> th09LobbyRatings(
    Iterable<String> usernames, {
    required bool authenticated,
  }) async {
    requests.add((usernames: usernames.toList(), authenticated: authenticated));
    return Th09LobbyRatings.fromJson(
      fixture['ratings'] as Map<String, dynamic>,
    );
  }

  @override
  Future<PublicProfile> profile(String username, {bool fresh = false}) async {
    final value = fixture['profiles'][username];
    return value is Map<String, dynamic> && username != 'other'
        ? PublicProfile.fromJson(value, (_, _) => null)
        : PublicProfile(
            username: username,
            nickname: value?['nickname'] ?? username,
            showRecords: false,
            showVersus: false,
          );
  }
}

void _viewport(WidgetTester tester, Size size) {
  tester.view.devicePixelRatio = 1;
  tester.view.physicalSize = size;
  addTearDown(tester.view.resetDevicePixelRatio);
  addTearDown(tester.view.resetPhysicalSize);
}

Future<void> _tap(WidgetTester tester, Finder control) async {
  await tester.ensureVisible(control);
  await tester.tap(control);
  await tester.pumpAndSettle();
}

Future<void> _precacheCatalog(
  WidgetTester tester,
  List<Game> games, {
  bool light = false,
}) async {
  await tester.runAsync(() async {
    final context = tester.element(
      find.byType(LibraryPage, skipOffstage: false),
    );
    await precacheImage(
      AssetImage(
        'assets/launcher-wordmark-'
        '${light ? 'light' : 'dark'}.png',
      ),
      context,
    );
    for (final game in games) {
      if (game.artwork case final String art) {
        await precacheImage(AssetImage('assets/book-covers/$art'), context);
      }
    }
  });
  await tester.pumpAndSettle();
}

void main() {
  TestWidgetsFlutterBinding.ensureInitialized();
  late List<Game> games;
  late List<LobbyRoom> rooms;
  late Map<String, dynamic> ratingFixture;
  late List<LobbyRoom> ratingRooms;
  final outputPath = Platform.environment['TOUHOU_LOBBY_CAPTURE'];
  final visualPlatform = TargetPlatformVariant.only(
    outputPath == null ? TargetPlatform.android : TargetPlatform.windows,
  );
  setUpAll(() async {
    games = await Game.load();
    final data = jsonDecode(
      File('tools/lobby-directory-fixture.json').readAsStringSync(),
    );
    final lookup = {for (final game in games) game.multiplayerProduct: game};
    rooms = [
      for (final row in data['rooms'] as List) LobbyRoom.read(row, lookup)!,
    ];
    ratingFixture = jsonDecode(
      File('tools/lobby-ratings-fixture.json').readAsStringSync(),
    ) as Map<String, dynamic>;
    ratingRooms = [
      for (final row in ratingFixture['rooms'] as List)
        LobbyRoom.read(row, lookup)!,
    ];
    if (outputPath != null || Platform.isWindows) {
      final fonts = Platform.environment['TOUHOU_CAPTURE_FONTS'];
      await loadFonts(fonts == null ? null : Directory(fonts));
    }
  });
  setUp(() {
    SharedPreferencesAsyncPlatform.instance =
        InMemorySharedPreferencesAsync.empty();
  });

  testWidgets('Room elapsed time stops updating while the lobby is hidden', (
    tester,
  ) async {
    _viewport(tester, const Size(1440, 734));
    final binding = tester.binding;
    final previous = binding.lifecycleState;
    binding.handleAppLifecycleStateChanged(AppLifecycleState.resumed);
    addTearDown(
      () => binding.handleAppLifecycleStateChanged(
        previous ?? AppLifecycleState.resumed,
      ),
    );
    final strings = lookupLauncherStrings(
      LauncherLanguage.simplifiedChinese.locale,
    );
    await tester.pumpWidget(
      launcher.fixture(
        accountApi: launcher.FixtureAccountApi(),
        lobbyDirectory: _LobbyFixture(games, rooms),
        games: games,
      ),
    );
    await tester.pumpAndSettle();
    await _tap(tester, find.text(strings.multiplayer));
    final clock = tester
        .widget<LobbyRoomCard>(find.byType(LobbyRoomCard).first)
        .clock;
    var ticks = 0;
    void tick() => ticks++;
    clock.addListener(tick);
    addTearDown(() => clock.removeListener(tick));
    await tester.pump(const Duration(seconds: 2));
    expect(ticks, greaterThan(0));

    await _tap(tester, find.text(strings.singlePlayer));
    ticks = 0;
    await tester.pump(const Duration(seconds: 3));
    expect(
      ticks,
      0,
      reason: 'An offstage room list must not rebuild every second',
    );

    await _tap(tester, find.text(strings.multiplayer));
    ticks = 0;
    await tester.pump(const Duration(seconds: 2));
    expect(ticks, greaterThan(0));
    binding.handleAppLifecycleStateChanged(AppLifecycleState.inactive);
    ticks = 0;
    await tester.pump(const Duration(seconds: 3));
    expect(
      ticks,
      greaterThan(0),
      reason: 'A visible unfocused window still shows live elapsed time',
    );
    binding.handleAppLifecycleStateChanged(AppLifecycleState.hidden);
    ticks = 0;
    await tester.pump(const Duration(seconds: 3));
    expect(
      ticks,
      0,
      reason: 'A background window must not update elapsed labels',
    );
    binding.handleAppLifecycleStateChanged(AppLifecycleState.inactive);
    binding.handleAppLifecycleStateChanged(AppLifecycleState.resumed);
    ticks = 0;
    await tester.pump(const Duration(seconds: 2));
    expect(ticks, greaterThan(0));
    expect(tester.takeException(), isNull);
  });

  for (final (name, size, language) in [
    ('mobile-narrow', const Size(320, 640), LauncherLanguage.english),
    ('mobile', const Size(390, 844), LauncherLanguage.japanese),
    ('desktop', const Size(1440, 734), LauncherLanguage.simplifiedChinese),
  ]) {
    testWidgets('$name imports a th09 match archive from the lobby', (
      tester,
    ) async {
      _viewport(tester, size);
      final game = games.singleWhere((game) => game.id == 'th09');
      final root = (await tester.runAsync(
        () => Directory.systemTemp.createTemp('th09-replay-view-'),
      ))!;
      addTearDown(() => root.delete(recursive: true));
      final strings = lookupLauncherStrings(language.locale);
      final bytes = Uint8List.fromList(
        utf8.encode('opaque match replay fixture'),
      );
      final watched = <String>[];
      final previewName = 'Match replay.TH09R';
      var selectedName = previewName;
      List<int> selectedBytes = bytes;
      var oversized = false;
      var choseExtension = false;
      Completer<GameFileInput?>? pendingPicker;
      var directoryRequests = 0;
      final files = GameUserFiles(
        appDirectory: () async {
          directoryRequests++;
          return root;
        },
        picker: (extensions) async {
          choseExtension = extensions.contains('th09r');
          if (pendingPicker case final pending?) return pending.future;
          return (
            name: selectedName,
            length: () async =>
                oversized ? 1024 * 1024 + 1 : selectedBytes.length,
            read: () => Stream.value(selectedBytes),
          );
        },
      );
      final runtime = launcher.RecordingGameLauncher(
        onReplay: (path) async {
          watched.add(path);
          expect(await File(path).readAsBytes(), bytes);
        },
      );
      final boundary = GlobalKey();
      await tester.pumpWidget(
        RepaintBoundary(
          key: boundary,
          child: launcher.fixture(
            games: [game],
            lobbyDirectory: _LobbyFixture([game], []),
            gameUserFiles: files,
            gameResources: GameResources(appDirectory: () async => root),
            launcher: runtime,
            preferences: LauncherPreferences(
              SharedPreferencesAsync(),
              ThemeMode.dark,
              true,
              language: language,
            ),
          ),
        ),
      );
      await tester.pumpAndSettle();
      await _tap(tester, find.text(strings.multiplayer).first);
      final watch = find.byTooltip(strings.watchReplay);
      final icon = find.descendant(
        of: watch,
        matching: find.byIcon(Icons.movie_outlined),
      );
      final desktop = name == 'desktop';
      if (desktop) {
        Focus.of(tester.element(icon)).requestFocus();
        await tester.pump();
        await tester.sendKeyEvent(LogicalKeyboardKey.enter);
        await tester.pumpAndSettle();
      } else {
        await _tap(tester, watch);
      }
      final gamePicker = find.text(strings.replayChooseGame);
      if (gamePicker.evaluate().isNotEmpty) {
        await tester.tap(
          find.descendant(
            of: find.byType(SimpleDialog),
            matching: find.text(game.title),
          ),
        );
        await tester.pump();
      }

      Future<void> waitFor(Finder target, {Finder? tap}) async {
        await tester.runAsync(() async {
          if (tap != null) {
            await tester.tap(tap);
            await tester.pump();
          }
          final deadline = DateTime.now().add(const Duration(seconds: 6));
          while (target.evaluate().isEmpty &&
              DateTime.now().isBefore(deadline)) {
            await Future<void>.delayed(const Duration(milliseconds: 20));
            await tester.pump(const Duration(milliseconds: 40));
          }
        });
        expect(target, findsOneWidget);
      }

      final manager = find.byType(ReplayManagerDialog);
      final picker = find.text(strings.replayImport);
      final feedback = find.text(strings.gameReplayUnavailable);
      await waitFor(find.text(strings.gameNoReplay));
      await waitFor(feedback, tap: picker);
      await tester.pump(const Duration(milliseconds: 300));
      expect(find.text(previewName), findsOneWidget);
      expect(runtime.lastMultiplayer, isTrue);
      expect(choseExtension, isTrue);
      expect(watched, hasLength(1));
      expect(File(watched.single).existsSync(), isTrue);
      if (outputPath != null) {
        await _precacheCatalog(tester, games);
        await save(
          tester,
          boundary,
          Directory(outputPath),
          '$name-lobby-match-replay',
        );
      }
      await tester.tap(find.text(strings.gameReturnToReplays));
      await tester.pump();
      await tester.pump(const Duration(milliseconds: 300));
      await tester.runAsync(() async {
        final deadline = DateTime.now().add(const Duration(seconds: 6));
        while (File(watched.single).existsSync() &&
            DateTime.now().isBefore(deadline)) {
          await Future<void>.delayed(const Duration(milliseconds: 20));
          await tester.pump(const Duration(milliseconds: 40));
        }
      });
      expect(File(watched.single).existsSync(), isFalse);
      await tester.pump();
      oversized = true;
      await waitFor(
        find.text(strings.fileIssueImportEmptyOrLarge),
        tap: picker,
      );
      expect(watched, hasLength(1));
      final sample = Platform.environment['TOUHOU_PUBLIC_MATCH_REPLAY'];
      oversized = false;
      selectedName = 'matches.zip';
      selectedBytes = matchReplayZip(
        sample == null ? [1, 2, 3] : File(sample).readAsBytesSync(),
        extraFolders: desktop ? 16 : 0,
      );
      final folder = find.text('对局');
      await waitFor(desktop ? find.text('folder 00') : folder, tap: picker);
      if (desktop) {
        await tester.scrollUntilVisible(
          folder,
          180,
          scrollable: find.descendant(
            of: manager,
            matching: find.byType(Scrollable),
          ),
        );
        await tester.pumpAndSettle();
      }
      final rootOffset = Scrollable.of(tester.element(folder)).position.pixels;
      if (desktop) expect(rootOffset, greaterThan(0));
      final beforeCancel = directoryRequests;
      final dialogPickerIcon = find.descendant(
        of: manager,
        matching: find.byIcon(Icons.file_upload_outlined),
      );
      final pickerFocus = Focus.of(tester.element(dialogPickerIcon));
      pickerFocus.requestFocus();
      await tester.pump();
      final canceled = pendingPicker = Completer<GameFileInput?>();
      await tester.sendKeyEvent(LogicalKeyboardKey.enter);
      await tester.pump();
      canceled.complete(null);
      pendingPicker = null;
      await tester.pumpAndSettle();
      expect(pickerFocus.hasPrimaryFocus, isTrue);
      expect(directoryRequests, beforeCancel);
      expect(Scrollable.of(tester.element(folder)).position.pixels, rootOffset);
      await _tap(tester, folder);
      expect(find.text('灵梦 vs Marisa'), findsOneWidget);
      expect(find.text(strings.th09ReplayWinner('Marisa')), findsOneWidget);
      expect(find.text('th9_02.rpy'), findsOneWidget);
      if (outputPath != null) {
        await _precacheCatalog(tester, games);
        await save(
          tester,
          boundary,
          Directory(outputPath),
          '$name-lobby-replay-metadata',
        );
      }
      Future<void> back() async {
        if (desktop) {
          await tester.sendKeyEvent(LogicalKeyboardKey.escape);
        } else {
          await tester.binding.handlePopRoute();
        }
        await tester.pumpAndSettle();
      }

      await back();
      expect(manager, findsOneWidget);
      expect(find.text('th9_02.rpy'), findsNothing);
      expect(find.widgetWithText(ListTile, '对局'), findsOneWidget);
      expect(Focus.of(tester.element(folder)).hasFocus, isTrue);
      expect(
        Scrollable.of(tester.element(folder)).position.pixels,
        closeTo(rootOffset, 1),
      );
      await back();
      expect(manager, findsNothing);
      expect(find.byType(LobbyPage), findsOneWidget);
      if (desktop) expect(Focus.of(tester.element(icon)).hasFocus, isTrue);
      expect(tester.takeException(), isNull);
    }, variant: visualPlatform);
  }

  for (final (viewport, size) in [
    ('mobile-narrow', const Size(320, 640)),
    if (outputPath != null) ...[
      ('mobile', const Size(390, 844)),
      ('desktop', const Size(1440, 734)),
    ],
  ]) {
    testWidgets(
      '$viewport lobby diagnostics reports four rows and cancels on close',
      (tester) async {
        _viewport(tester, size);
        final directory = _LobbyFixture(games, rooms);
        final boundary = GlobalKey();
        final strings = lookupLauncherStrings(
          LauncherLanguage.simplifiedChinese.locale,
        );
        await tester.pumpWidget(
          RepaintBoundary(
            key: boundary,
            child: launcher.fixture(
              accountApi: launcher.FixtureAccountApi(),
              lobbyDirectory: directory,
              games: games,
              preferences: LauncherPreferences(
                SharedPreferencesAsync(),
                ThemeMode.dark,
                true,
              ),
            ),
          ),
        );
        await tester.pumpAndSettle();
        await _tap(tester, find.text(strings.multiplayer));
        await _tap(tester, find.text(strings.checkNetwork));
        expect(find.byType(NetworkCheckDialog), findsOneWidget);
        expect(find.text(strings.networkCheckChecking), findsNWidgets(4));
        final panel = find.byType(NetworkCheckDialog);
        final pending = find.descendant(
          of: panel,
          matching: find.byIcon(Icons.more_horiz),
        );
        final iconX = tester.getCenter(pending.first).dx;
        final retry = find.widgetWithText(TextButton, strings.checkAgain);
        expect(tester.widget<TextButton>(retry).onPressed, isNull);
        directory.checks.single.finish();
        await tester.pumpAndSettle();
        expect(find.text('38 ms'), findsOneWidget);
        expect(find.text('NAT3'), findsOneWidget);
        expect(find.text(strings.networkCheckUnavailable), findsNWidgets(2));
        for (final icon in [
          Icons.check_circle_outline,
          Icons.remove_circle_outline,
        ]) {
          final results = find.descendant(
            of: panel,
            matching: find.byIcon(icon),
          );
          expect(results, findsNWidgets(2));
          expect(tester.getCenter(results.first).dx, closeTo(iconX, 0.5));
          expect(tester.getCenter(results.last).dx, closeTo(iconX, 0.5));
        }
        if (outputPath != null) {
          final output = Directory(outputPath)..createSync(recursive: true);
          await _precacheCatalog(tester, games);
          await save(tester, boundary, output, '$viewport-network-check');
        }
        await _tap(tester, retry);
        expect(directory.checks, hasLength(2));
        expect(find.text(strings.networkCheckChecking), findsNWidgets(4));
        await tester.binding.handlePopRoute();
        await tester.pumpAndSettle();
        expect(directory.checks.last.closed, isTrue);
        expect(directory.calls, 0);
        expect(find.byType(LobbyPage), findsOneWidget);
        expect(tester.takeException(), isNull);
      },
      variant: visualPlatform,
    );
  }

  testWidgets('320 px game strip and tools share filters with the work menu', (
    tester,
  ) async {
    _viewport(tester, const Size(320, 640));
    final directory = _LobbyFixture(games, rooms);
    await tester.pumpWidget(
      launcher.fixture(
        accountApi: launcher.FixtureAccountApi(),
        lobbyDirectory: directory,
        games: games,
      ),
    );
    await tester.pumpAndSettle();
    await _tap(tester, find.text('联机'));
    expect(find.byType(LobbyPage), findsOneWidget);
    final works = find.byTooltip('联机作品选单');
    final all = find.byTooltip('全部联机');
    final tools = ['筛选房间', '创建房间', '输入房间号', '观赏录像', '刷新'].map(find.byTooltip);
    for (final control in [works, all, ...tools]) {
      final bounds = tester.getRect(control);
      expect(bounds.height, closeTo(bounds.width, 1));
      expect(bounds.right, lessThanOrEqualTo(320));
    }
    expect(
      tools.map((control) => tester.getCenter(control).dy).toSet(),
      hasLength(1),
    );

    await _tap(tester, find.byTooltip('只看 東方花映塚'));
    expect(find.text('1 个房间'), findsOneWidget);
    await _tap(tester, works);
    await _tap(
      tester,
      find.descendant(
        of: find.byType(MultiplayerBookSidebar),
        matching: find.byTooltip('筛选 東方紅魔郷'),
      ),
    );
    await tester.binding.handlePopRoute();
    await tester.pumpAndSettle();
    expect(find.text('2 个房间'), findsOneWidget);

    await _tap(tester, find.byTooltip('筛选房间'));
    await _tap(tester, find.text('可观战'));
    expect(find.text('1 个房间'), findsOneWidget);
    expect(find.textContaining('#5601'), findsOneWidget);
    final before = tester.getRect(find.byKey(const ValueKey('th06-5601')));
    await _tap(tester, find.text('单机'));
    await _tap(tester, find.text('联机'));
    expect(tester.getRect(find.byKey(const ValueKey('th06-5601'))), before);
    expect(find.byType(NavigationBar), findsOneWidget);

    final lobbyState = tester.state(find.byType(LobbyPage));
    for (final width in [781.0, 780.0, 320.0]) {
      tester.view.physicalSize = Size(width, 640);
      await tester.pumpAndSettle();
      expect(tester.state(find.byType(LobbyPage)), same(lobbyState));
      expect(find.text('1 个房间'), findsOneWidget);
      expect(find.textContaining('#5601'), findsOneWidget);
      expect(
        find.byType(NavigationBar),
        width > 780 ? findsNothing : findsOneWidget,
      );
    }

    await _tap(tester, find.byTooltip('输入房间号'));
    await tester.enterText(find.byType(TextField).last, '123');
    await tester.pumpAndSettle();
    final join = find.widgetWithText(FilledButton, '加入');
    expect(tester.widget<FilledButton>(join).onPressed, isNull);
    await tester.enterText(find.byType(TextField).last, '1048');
    await tester.pumpAndSettle();
    await _tap(tester, join);
    expect(find.text('房间 #1048'), findsOneWidget);
    expect(find.text('无法读取联机服务'), findsOneWidget);
    await tester.binding.handlePopRoute();
    await tester.pumpAndSettle();
    expect(find.textContaining('#5601'), findsOneWidget);

    directory.disconnect();
    await tester.pumpAndSettle();
    expect(find.text('大厅连接已中断'), findsOneWidget);
    expect(find.textContaining('#5601'), findsOneWidget);
    await _tap(tester, find.byTooltip('重试连接'));
    expect(directory.calls, 1);
    expect(tester.takeException(), isNull);
  }, variant: visualPlatform);

  for (final (device, size) in [
    ('mobile-narrow', const Size(320, 640)),
    ('mobile', const Size(390, 844)),
    ('desktop', const Size(1440, 734)),
  ]) {
    testWidgets('$device th09 rating cards respect visibility and room order', (
      tester,
    ) async {
      _viewport(tester, size);
      final api = RatingFixtureApi(ratingFixture);
      final rooms = _LobbyFixture(games, ratingRooms);
      final boundary = GlobalKey();
      await tester.pumpWidget(
        RepaintBoundary(
          key: boundary,
          child: launcher.fixture(
            accountApi: api,
            lobbyDirectory: rooms,
            games: games,
            preferences: LauncherPreferences(
              SharedPreferencesAsync(),
              ThemeMode.dark,
              true,
            ),
          ),
        ),
      );
      await tester.pumpAndSettle();
      expect(api.requests, isEmpty);
      await _tap(tester, find.text('联机'));
      expect(api.requests.single.usernames, ['other', 'rated', 'unrated']);
      expect(api.requests.single.authenticated, isFalse);
      final first = find.byKey(const ValueKey('th09-4201'));
      final second = find.byKey(const ValueKey('th09-4202'));
      expect(
        tester.getTopLeft(second).dy,
        lessThan(tester.getTopLeft(first).dy),
      );
      final lobby = tester.widget<LobbyPage>(find.byType(LobbyPage));
      await lobby.session.signIn('fixture', 'valid-test-value');
      await tester.pumpAndSettle();
      expect(api.requests.last.authenticated, isTrue);
      expect(
        tester.getTopLeft(first).dy,
        lessThan(tester.getTopLeft(second).dy),
      );
      expect(find.textContaining('局数 26', findRichText: true), findsOneWidget);

      final trigger = find.byKey(const ValueKey('th09-card-rated'));
      final card = find.byKey(const ValueKey('th09-player-card'));
      final before = tester.getRect(trigger);
      if (device == 'desktop') {
        final mouse = await tester.createGesture(kind: PointerDeviceKind.mouse);
        await mouse.addPointer(location: Offset.zero);
        await mouse.moveTo(tester.getCenter(trigger));
        await tester.pump(const Duration(milliseconds: 200));
        await tester.pumpAndSettle();
        expect(card, findsOneWidget);
        await mouse.removePointer();
      }
      await tester.tap(trigger);
      await tester.pumpAndSettle();
      expect(
        find.descendant(of: card, matching: find.text('1,438')),
        findsOneWidget,
      );
      expect(
        find.descendant(of: card, matching: find.byType(Th09CharacterPortrait)),
        findsNWidgets(3),
      );
      final profile = find.descendant(of: card, matching: find.text('查看个人主页'));
      expect(profile.hitTestable(), findsOneWidget);
      if (outputPath != null) {
        final cardContext = tester.element(card);
        await tester.runAsync(
          () => Future.wait([
            for (final id in [0, 2, 4])
              precacheImage(
                AssetImage('assets/th09-portraits/$id.webp'),
                cardContext,
              ),
          ]),
        );
        await tester.pump();
        final output = Directory(outputPath)..createSync(recursive: true);
        await _precacheCatalog(tester, games);
        await save(tester, boundary, output, '$device-lobby-rating-card');
      }
      if (device == 'desktop') {
        await tester.sendKeyEvent(LogicalKeyboardKey.escape);
      } else if (device == 'mobile') {
        final link = await launcher.openAccountAndReturn(tester, profile);
        expect(link.path, startsWith('/u/'));
      } else {
        await tester.binding.handlePopRoute();
      }
      await tester.pumpAndSettle();
      expect(card, findsNothing);
      expect(tester.getRect(trigger), before);

      if (device == 'mobile-narrow') {
        await tester.drag(
          find.byKey(const PageStorageKey('lobby-room-list')),
          const Offset(0, -140),
        );
        await tester.pumpAndSettle();
      }
      final other = find.byKey(const ValueKey('th09-card-other'));
      expect(other.hitTestable(), findsOneWidget);
      await tester.tap(other);
      await tester.pumpAndSettle();
      expect(
        find.descendant(of: card, matching: find.byType(Th09CharacterPortrait)),
        findsNothing,
      );
      expect(
        find.descendant(of: card, matching: find.text('1,710')),
        findsNothing,
      );
      expect(
        find.descendant(of: card, matching: find.text('@other')),
        findsOneWidget,
      );
      if (device == 'desktop') {
        await tester.sendKeyEvent(LogicalKeyboardKey.escape);
      } else {
        await tester.binding.handlePopRoute();
      }
      await tester.pumpAndSettle();
      if (device == 'mobile-narrow') {
        await _tap(tester, find.text('单机'));
        final count = api.requests.length;
        await tester.pump(const Duration(seconds: 13));
        expect(api.requests, hasLength(count));
      }
      expect(tester.takeException(), isNull);
    }, variant: visualPlatform);
    testWidgets('$device capped lobby reaches a room beyond the first 200', (
      tester,
    ) async {
      _viewport(tester, size);
      final bulkRoom = rooms.firstWhere((room) => room.game.id == 'th06');
      final rareRoom = rooms.firstWhere((room) => room.game.id == 'th09');
      final directory = _CappedLobbyFixture(games, [
        for (var i = 2000; i < 2200; i++)
          LobbyRoom(
            game: bulkRoom.game,
            code: '$i',
            capacity: bulkRoom.capacity,
            difficulty: bulkRoom.difficulty,
            playing: bulkRoom.playing,
            joinable: bulkRoom.joinable,
            liveSpectator: bulkRoom.liveSpectator,
            spectators: bulkRoom.spectators,
            seats: bulkRoom.seats,
            rules: bulkRoom.rules,
          ),
      ], rareRoom);
      final boundary = GlobalKey();
      await tester.pumpWidget(
        RepaintBoundary(
          key: boundary,
          child: launcher.fixture(
            games: games,
            accountApi: launcher.FixtureAccountApi(),
            lobbyDirectory: directory,
            preferences: LauncherPreferences(
              SharedPreferencesAsync(),
              ThemeMode.dark,
              true,
            ),
          ),
        ),
      );
      await tester.pumpAndSettle();
      await _tap(tester, find.text('联机'));
      final scoped = find.byKey(const ValueKey('th09-1048'));
      expect(find.text('200 / 201 个房间'), findsOneWidget);
      expect(scoped, findsNothing);
      final filter = find.byTooltip(
        device == 'desktop' ? '筛选 東方花映塚' : '只看 東方花映塚',
      );
      final refresh = find.byTooltip('刷新');
      final before = tester.getRect(refresh);
      await tester.ensureVisible(filter);
      await tester.tap(filter);
      await tester.pump();
      expect(directory.requestedGame, 'th09');
      expect(find.byType(LinearProgressIndicator), findsOneWidget);
      expect(find.text('没有符合筛选条件的房间'), findsNothing);
      expect(tester.getRect(refresh), before);

      directory.reply();
      await tester.pumpAndSettle();
      expect(find.byType(LinearProgressIndicator), findsNothing);
      expect(scoped, findsOneWidget);
      expect(find.text('1 个房间'), findsOneWidget);
      expect(tester.getRect(refresh), before);
      if (outputPath != null) {
        final output = Directory(outputPath)..createSync(recursive: true);
        await _precacheCatalog(tester, games);
        await save(tester, boundary, output, '$device-lobby-capped');
      }
      expect(tester.takeException(), isNull);
    }, variant: visualPlatform);
  }

  testWidgets('desktop room rows and shared sidebar scroll independently', (
    tester,
  ) async {
    _viewport(tester, const Size(1440, 734));
    final directory = _LobbyFixture(
      games,
      List.generate(15, (index) {
        final room = rooms[index % rooms.length];
        return LobbyRoom(
          game: room.game,
          code: (2000 + index).toString(),
          capacity: room.capacity,
          difficulty: room.difficulty,
          playing: room.playing,
          joinable: room.joinable,
          liveSpectator: room.liveSpectator,
          spectators: room.spectators,
          seats: room.seats,
          rules: room.rules,
        );
      }),
    );
    await tester.pumpWidget(
      launcher.fixture(
        accountApi: launcher.FixtureAccountApi(),
        lobbyDirectory: directory,
        games: games,
      ),
    );
    await tester.pumpAndSettle();
    await _tap(tester, find.text('联机'));
    expect(find.byType(BookSidebar), findsOneWidget);
    expect(find.byType(GameRow), findsWidgets);
    final search = tester.getRect(find.byType(SearchBar));
    final listing = find.byKey(const PageStorageKey('lobby-room-list'));
    await tester.drag(listing, const Offset(0, -340));
    await tester.pumpAndSettle();
    expect(tester.getRect(find.byType(SearchBar)), search);
    expect(find.byKey(const ValueKey('th09-2000')).hitTestable(), findsNothing);
    expect(find.text('15 个房间'), findsOneWidget);
    await _tap(tester, find.byTooltip('筛选 東方花映塚'));
    expect(find.text('4 个房间'), findsOneWidget);
    expect(find.byType(BookSidebar), findsOneWidget);
    expect(tester.takeException(), isNull);
  }, variant: visualPlatform);

  for (final (device, size, language, scale) in [
    (
      'mobile-narrow',
      const Size(320, 640),
      LauncherLanguage.simplifiedChinese,
      1.0,
    ),
    ('mobile', const Size(390, 844), LauncherLanguage.simplifiedChinese, 1.0),
    ('desktop', const Size(1440, 734), LauncherLanguage.simplifiedChinese, 1.0),
    ('mobile-narrow', const Size(320, 640), LauncherLanguage.english, 2.0),
    ('mobile', const Size(390, 844), LauncherLanguage.japanese, 2.0),
    ('desktop', const Size(1440, 734), LauncherLanguage.english, 2.0),
  ]) {
    final scenario = '$device${scale == 2 ? '-${language.tag}-2' : ''}';
    testWidgets(
      '$scenario lobby releases a previous room after acknowledgment',
      (tester) async {
        _viewport(tester, size);
        tester.platformDispatcher.textScaleFactorTestValue = scale;
        addTearDown(tester.platformDispatcher.clearTextScaleFactorTestValue);
        final directory = _OccupiedLobbyFixture(games, rooms);
        final boundary = GlobalKey();
        final strings = lookupLauncherStrings(language.locale);
        await tester.pumpWidget(
          RepaintBoundary(
            key: boundary,
            child: launcher.fixture(
              games: games,
              accountApi: launcher.FixtureAccountApi(),
              lobbyDirectory: directory,
              preferences: LauncherPreferences(
                SharedPreferencesAsync(),
                ThemeMode.dark,
                true,
                language: language,
              ),
            ),
          ),
        );
        await tester.pumpAndSettle();
        if (outputPath != null) await _precacheCatalog(tester, games);
        final tab = find.text(strings.multiplayer);
        await _tap(
          tester,
          tab.evaluate().isEmpty
              ? find
                    .descendant(
                      of: find.byKey(const ValueKey('masthead navigation')),
                      matching: find.byType(prism.PressSurface),
                    )
                    .at(1)
              : tab,
        );
        final notice = find.byKey(const ValueKey('lobby-membership'));
        final release = find.widgetWithText(
          OutlinedButton,
          strings.lobbyMembershipRelease,
        );
        final create = device == 'desktop'
            ? find.widgetWithText(FilledButton, strings.createRoom)
            : find.byWidgetPredicate(
                (widget) =>
                    widget is IconButton &&
                    widget.tooltip == strings.createRoom,
              );
        VoidCallback? createAction() => switch (tester.widget(create)) {
          final IconButton button => button.onPressed,
          final FilledButton button => button.onPressed,
          _ => throw StateError('Create room control is missing'),
        };
        expect(find.text(strings.lobbyMembershipTitle('4162')), findsOneWidget);
        expect(createAction(), isNull);
        if (outputPath != null) {
          final output = Directory(outputPath)..createSync(recursive: true);
          await _precacheCatalog(tester, games);
          await save(tester, boundary, output, '$scenario-lobby-membership');
        }

        await tester.ensureVisible(release);
        await tester.pumpAndSettle();
        final bounds = tester.getRect(notice);
        await _tap(tester, release);
        final waiting = find.widgetWithText(
          OutlinedButton,
          strings.lobbyMembershipReleasing,
        );
        expect(tester.widget<OutlinedButton>(waiting).onPressed, isNull);
        expect(createAction(), isNull);
        expect(tester.getRect(notice), bounds);
        directory.reply(LobbyReleaseResult.unconfirmed);
        await tester.pumpAndSettle();
        expect(createAction(), isNull);
        expect(find.text(strings.lobbyMembershipUnconfirmed), findsOneWidget);
        if (device == 'mobile-narrow') {
          directory.setRecoverySupport(false);
          await tester.pumpAndSettle();
          expect(release, findsNothing);
          expect(find.text(strings.lobbyMembershipUnsupported), findsOneWidget);
          expect(createAction(), isNull);
          directory.setRecoverySupport(true);
          await tester.pumpAndSettle();
        }
        await _tap(tester, release);
        directory.reply(LobbyReleaseResult.released);
        await tester.pumpAndSettle();
        expect(notice, findsNothing);
        expect(createAction(), isNotNull);
        expect(tester.takeException(), isNull);
      },
      variant: TargetPlatformVariant.only(TargetPlatform.windows),
    );
  }

  if (outputPath != null) {
    final output = Directory(outputPath)..createSync(recursive: true);
    for (final (language, device, size, light, scale) in [
      for (final scale in [1.0, 2.0])
        for (final light in [false, true])
          for (final language in LauncherLanguage.values) ...[
            (language, 'desktop', const Size(1440, 734), light, scale),
            (language, 'mobile', const Size(390, 844), light, scale),
            (language, 'mobile-narrow', const Size(320, 640), light, scale),
          ],
    ]) {
      final locale = language == LauncherLanguage.simplifiedChinese
          ? ''
          : '-${language.tag}';
      final suffix = '$locale${light ? '-light' : ''}${scale == 2 ? '-2' : ''}';
      final strings = lookupLauncherStrings(language.locale);
      testWidgets('capture $device$suffix lobby', (tester) async {
        _viewport(tester, size);
        tester.platformDispatcher.textScaleFactorTestValue = scale;
        addTearDown(tester.platformDispatcher.clearTextScaleFactorTestValue);
        final directory = _LobbyFixture(games, List.of(rooms));
        final boundary = GlobalKey();
        await tester.pumpWidget(
          RepaintBoundary(
            key: boundary,
            child: launcher.fixture(
              accountApi: launcher.FixtureAccountApi(),
              lobbyDirectory: directory,
              games: games,
              preferences: LauncherPreferences(
                SharedPreferencesAsync(),
                light ? ThemeMode.light : ThemeMode.dark,
                true,
                language: language,
              ),
            ),
          ),
        );
        await tester.pumpAndSettle();
        await _precacheCatalog(tester, games, light: light);
        final tab = find.text(strings.multiplayer);
        await _tap(
          tester,
          tab.evaluate().isEmpty
              ? find
                    .descendant(
                      of: find.byKey(const ValueKey('masthead navigation')),
                      matching: find.byType(prism.PressSurface),
                    )
                    .at(1)
              : tab,
        );
        expect(find.byType(LobbyRoomCard), findsWidgets);
        expect(find.textContaining(strings.roomLobbyOpen), findsWidgets);
        final surface = find.descendant(
          of: find.byType(LobbyPage),
          matching: find.byType(Card),
        );
        final bounds = tester.getRect(surface);
        final metadata = <String, Object>{
          'surface': [bounds.left, bounds.top, bounds.width, bounds.height],
        };
        final row = tester.getRect(find.byType(LobbyRoomCard).first);
        final firstRoom = tester.element(find.byType(LobbyRoomCard).first);
        metadata['firstRoom'] = [row.left, row.top, row.width, row.height];
        await save(tester, boundary, output, '$device-lobby-live$suffix');
        directory.disconnect();
        await tester.pumpAndSettle();
        await save(
          tester,
          boundary,
          output,
          '$device-lobby-disconnected$suffix',
        );
        expect(tester.getRect(surface), bounds);
        final firstRoomFinder = find.byWidgetPredicate(
          (widget) =>
              widget is LobbyRoomCard && widget.room.id == rooms.first.id,
        );
        if (firstRoomFinder.evaluate().isNotEmpty && firstRoom.mounted) {
          expect(tester.element(firstRoomFinder), same(firstRoom));
        }
        final listing = find.byKey(const PageStorageKey('lobby-room-list'));
        final scrollable = find
            .descendant(of: listing, matching: find.byType(Scrollable))
            .first;
        if (firstRoomFinder.evaluate().isEmpty) {
          // Large notices can put the first room outside the lazy viewport.
          final title = find.descendant(
            of: firstRoomFinder,
            matching: find.text(rooms.first.game.title),
          );
          await tester.scrollUntilVisible(title, 100, scrollable: scrollable);
          await tester.pumpAndSettle();
          expect(title.hitTestable(), findsOneWidget);
          await save(
            tester,
            boundary,
            output,
            '$device-lobby-disconnected-rooms$suffix',
          );
          tester.state<ScrollableState>(scrollable).position.jumpTo(0);
          await tester.pumpAndSettle();
        }
        directory.setConnection(LobbyConnection.loading);
        await tester.pump(const Duration(milliseconds: 100));
        expect(find.text(strings.lobbyDisconnected), findsNothing);
        final loadingRoom = tester.element(firstRoomFinder);
        expect(
          tester.widget<LobbyRoomCard>(firstRoomFinder).room.id,
          rooms.first.id,
        );
        directory.setConnection(LobbyConnection.live);
        await tester.pumpAndSettle();
        expect(tester.getRect(surface), bounds);
        expect(tester.element(firstRoomFinder), same(loadingRoom));
        final join = find.descendant(
          of: firstRoomFinder,
          matching: find.widgetWithText(OutlinedButton, strings.join),
        );
        await tester.ensureVisible(join);
        await tester.pumpAndSettle();
        expect(join.hitTestable(), findsOneWidget);
        expect(tester.widget<OutlinedButton>(join).onPressed, isNotNull);
        directory.clearRooms();
        await tester.pumpAndSettle();
        expect(find.byType(LobbyRoomCard), findsNothing);
        expect(tester.getRect(surface), bounds);
        await save(tester, boundary, output, '$device-lobby-empty$suffix');
        File('${output.path}/$device$suffix.json')
            .writeAsStringSync(jsonEncode(metadata));
        for (final label in [
          strings.multiplayerGuide,
          strings.checkNetwork,
          strings.rankings,
        ]) {
          final link = find.text(label);
          final rect = tester.getRect(link);
          expect(link.hitTestable(), findsOneWidget);
          expect(rect.left, greaterThanOrEqualTo(16));
          expect(rect.right, lessThanOrEqualTo(size.width - 16));
        }
        expect(tester.takeException(), isNull);
      }, variant: TargetPlatformVariant.only(TargetPlatform.windows));
    }
  }
}
