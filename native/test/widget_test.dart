import 'dart:async';
import 'dart:io';

import 'package:material_ui/material_ui.dart';
import 'package:flutter/services.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:shared_preferences/shared_preferences.dart';
import 'package:shared_preferences_platform_interface/in_memory_shared_preferences_async.dart';
import 'package:shared_preferences_platform_interface/shared_preferences_async_platform_interface.dart';
import 'package:touhouonline/account_api.dart';
import 'package:touhouonline/account.dart';
import 'package:touhouonline/account_web_page.dart';
import 'package:touhouonline/cloud_save_page.dart';
import 'package:touhouonline/game_detail.dart';
import 'package:touhouonline/messages.dart';

import 'package:touhouonline/profile_data.dart';
import 'package:touhouonline/catalog.dart';
import 'package:touhouonline/game_launcher.dart';
import 'package:touhouonline/game_resources.dart';
import 'package:touhouonline/game_user_files.dart';
import 'package:touhouonline/game_preferences.dart';
import 'package:touhouonline/gamepad_preferences.dart';
import 'package:touhouonline/input_settings_frame.dart';
import 'package:touhouonline/library.dart';
import 'package:touhouonline/lobby_directory.dart';
import 'package:touhouonline/local_duel_preferences.dart';
import 'package:touhouonline/main.dart';
import 'package:touhouonline/touch_preferences.dart';

import '../tools/capture_native_baseline_test.dart' show loadFonts, save;

/// Supply an already verified test identity. Account forms now live on the website.
Future<void> signInFixture(WidgetTester tester, {bool settle = true}) async {
  AccountSession session;
  if (find.byType(GameDetail).evaluate().isNotEmpty) {
    session = tester.widget<GameDetail>(find.byType(GameDetail).last).session;
  } else if (find.byType(CloudSavePage).evaluate().isNotEmpty) {
    session = tester
        .widget<CloudSavePage>(find.byType(CloudSavePage).last)
        .session;
  } else if (find.byType(MessagesPage).evaluate().isNotEmpty) {
    session = tester
        .widget<MessagesPage>(find.byType(MessagesPage).last)
        .highlights
        .session;
  } else if (find.byType(AccountSummary).evaluate().isNotEmpty) {
    session = tester
        .widget<AccountSummary>(find.byType(AccountSummary).first)
        .session;
  } else {
    session = tester
        .widget<MessageActions>(find.byType(MessageActions).first)
        .highlights
        .session;
  }
  await session.signIn('fixture', 'valid-test-value');
  if (settle) {
    await tester.pumpAndSettle();
  } else {
    await tester.pump();
  }
}

Future<Uri> visitAccountWeb(
  WidgetTester tester,
  FutureOr<void> Function() open,
) async {
  await open();
  await tester.pumpAndSettle();
  final page = find.byType(AccountWebPage);
  expect(page, findsOneWidget);
  final url = tester.widget<AccountWebPage>(page).address;
  expect(url.origin, 'https://touhouonline.com');
  expect(find.byType(TextFormField), findsNothing);
  await tester.tap(find.byIcon(Icons.close).last);
  await tester.pumpAndSettle();
  expect(page, findsNothing);
  return url;
}

Future<Uri> openAccountAndReturn(WidgetTester tester, Finder trigger) =>
    visitAccountWeb(tester, () => tester.tap(trigger));

const game = Game(
  id: 'th06',
  title: '東方紅魔郷',
  subtitle: 'the Embodiment of Scarlet Devil',
  number: '06',
  category: BookCategory.windows,
  nameOrder: 0,
  multiplayer: true,
  settings: GameSettingsCatalog(
    highRefresh: true,
    languages: [
      (id: 'ja', title: '日本語', bytes: 0),
      (id: 'lang_zh-hans', title: '中文（简体）', bytes: 301002),
    ],
  ),
);

class RecordingGameLauncher implements GameLauncher {
  RecordingGameLauncher({
    this.status = GameLaunchStatus.nativeRuntimeUnavailable,
    this.onReplay,
  });

  final GameLaunchStatus status;
  final Future<void> Function(String path)? onReplay;
  GamePreferences? lastOptions;
  bool? lastMultiplayer;
  Map<String, String>? lastKeyboardAliases;
  PadMappings? lastGamepadMappings;
  LocalDuelPreferences? lastLocalDuel;
  TouchConfiguration? lastTouch;
  String? data;

  @override
  Future<GameLaunchStatus> launch(
    Game game, {
    required bool multiplayer,
    required GamePreferences preferences,
    required Map<String, String> keyboardAliases,
    required PadMappings gamepadMappings,
    required LocalDuelPreferences? localDuel,
    required TouchConfiguration touch,
    required String? dataPath,
    required String? userDataPath,
    required String? replayPath,
  }) async {
    lastOptions = preferences;
    lastMultiplayer = multiplayer;
    lastKeyboardAliases = keyboardAliases;
    lastGamepadMappings = gamepadMappings;
    lastLocalDuel = localDuel;
    lastTouch = touch;
    data = dataPath;
    if (replayPath != null) await onReplay?.call(replayPath);
    return status;
  }
}

LauncherApp fixture({
  AccountApi? accountApi,
  String? initialLocation,
  GameLauncher? launcher,
  GamePreferenceStore? gamePreferences,
  GameUserFiles? gameUserFiles,
  GameResources? gameResources,
  GamePlayHistory? history,
  LauncherPreferences? preferences,
  LobbyDirectory? lobbyDirectory,
  List<Game> games = const [game],
}) {
  final storage = SharedPreferencesAsync();
  final favorites = GameFavorites(storage, <String>{});
  return LauncherApp(
    book: BookState(games, favorites, history: history),
    lobby: BookState(games, favorites),
    preferences:
        preferences ?? LauncherPreferences(storage, ThemeMode.dark, true),
    launcher: launcher ?? const NativeGamePlaceholder(),
    initialLocation: initialLocation,
    gamePreferences: gamePreferences ?? GamePreferenceStore(storage),
    accountApi: accountApi,
    lobbyDirectory: lobbyDirectory,
    gameUserFiles: gameUserFiles,
    gameResources: gameResources,
  );
}

class FixtureAccountApi extends AccountApi {
  bool signedIn = false;
  bool get canPublish => signedIn;
  String? attemptedId;
  String? sent;
  String? posted;
  int? readThrough;

  DirectMessage get incoming => DirectMessage(
    id: 1,
    body: '第一条测试私信',
    outgoing: false,
    sentAt: DateTime(2026, 11, 25),
  );

  @override
  Future<AnnouncementPage> announcements({int? before}) async => (
    items: [
      if (posted case final String content)
        PublicAnnouncement(
          id: 1,
          author: '测试玩家',
          authorUsername: 'fixture',
          body: content,
          publishedAt: DateTime(2026, 11, 25),
        ),
    ],
    nextBefore: null,
    canPublish: canPublish,
  );

  @override
  Future<void> publishAnnouncement(String body, String requestId) async {
    if (!canPublish) throw const AccountRequestException(403, 'FORBIDDEN');
    posted = body;
  }

  @override
  Future<InboxPage> inbox({int? before, bool summary = false}) async => (
    threads: [
      (
        peer: (username: 'peer', nickname: '另一位玩家'),
        lastMessage: incoming,
        unread: readThrough == null ? 1 : 0,
      ),
    ],
    unread: readThrough == null ? 1 : 0,
    nextBefore: null,
  );

  @override
  Future<ConversationPage> conversation(String username, {int? before}) async =>
      (
        peer: (username: username, nickname: '另一位玩家'),
        messages: [
          incoming,
          if (sent case final String text)
            DirectMessage(
              id: 2,
              body: text,
              outgoing: true,
              sentAt: DateTime(2026, 11, 25),
            ),
        ],
        nextBefore: null,
      );

  @override
  Future<void> sendMessage(
    String username,
    String body,
    String requestId,
  ) async {
    if (attemptedId == null) {
      attemptedId = requestId;
      throw const AccountRequestException(503, 'SERVICE_UNAVAILABLE');
    }
    if (attemptedId != requestId) {
      throw StateError('Expected unchanged request ID');
    }
    sent = body;
  }

  @override
  Future<void> markRead(String username, int through) async {
    readThrough = through;
  }

  @override
  Future<AccountIdentity> signIn(String identity, String password) async {
    if (identity != 'fixture' || password != 'valid-test-value') {
      throw const AccountRequestException(401, 'INVALID_USERNAME_OR_PASSWORD');
    }
    signedIn = true;
    return (username: 'fixture', name: '测试玩家', expiresAt: null);
  }

  @override
  Future<Uri?> avatar(String username) async => null;

  @override
  Future<PublicProfile> profile(String username, {bool fresh = false}) async =>
      PublicProfile(username: username, nickname: '测试玩家');

  @override
  Future<ProfileSocial> social(
    String username, {
    required bool signedIn,
    ProfileList? list,
  }) async => const ProfileSocial(visible: false);

  @override
  Future<void> signOut() async {
    signedIn = false;
  }
}

class _RestoreApi extends FixtureAccountApi {
  @override
  bool get canRestoreSession => true;

  final attempts = <Completer<AccountIdentity?>>[];

  @override
  Future<AccountIdentity?> restoreSession() async {
    final pending = Completer<AccountIdentity?>();
    attempts.add(pending);
    final user = await pending.future;
    signedIn = user != null;
    return user;
  }
}

void main() {
  setUp(() {
    SharedPreferencesAsyncPlatform.instance =
        InMemorySharedPreferencesAsync.empty();
  });
  testWidgets(
    'Desktop launcher restores sign-in after a failed startup check',
    (tester) async {
      tester.view.devicePixelRatio = 1;
      tester.view.physicalSize = const Size(1440, 734);
      addTearDown(tester.view.resetPhysicalSize);
      addTearDown(tester.view.resetDevicePixelRatio);
      final api = _RestoreApi();
      await tester.pumpWidget(fixture(accountApi: api));
      expect(api.attempts, hasLength(1));
      expect(find.byTooltip('正在检查登录状态'), findsOneWidget);
      expect(find.text('测试玩家'), findsNothing);
      final search = find.byType(TextField).first;
      final searchPosition = tester.getTopLeft(search);
      api.attempts.single.completeError(
        const AccountRequestException(503, 'SERVICE_UNAVAILABLE'),
      );
      await tester.pumpAndSettle();
      await tester.tap(find.byTooltip('重试恢复登录'));
      await tester.pumpAndSettle();
      await tester.tap(find.text('重试恢复登录'));
      await tester.pump(const Duration(milliseconds: 200));
      await tester.pump();
      expect(api.attempts, hasLength(2));
      expect(find.byTooltip('正在检查登录状态'), findsOneWidget);
      api.attempts.last.complete((
        username: 'fixture',
        name: '测试玩家',
        expiresAt: null,
      ));
      await tester.pumpAndSettle();
      expect(find.text('测试玩家'), findsOneWidget);
      expect(find.byTooltip('正在检查登录状态'), findsNothing);
      expect(tester.getTopLeft(search), searchPosition);
      expect(tester.takeException(), isNull);
    },
  );
  testWidgets('320 px home shows loading, retry, and the verified profile', (
    tester,
  ) async {
    tester.view.devicePixelRatio = 1;
    tester.view.physicalSize = const Size(320, 640);
    addTearDown(tester.view.resetPhysicalSize);
    addTearDown(tester.view.resetDevicePixelRatio);
    final api = _RestoreApi();
    final output = Platform.environment['TOUHOU_ACCOUNT_RESTORE_CAPTURE'];
    if (output != null) {
      await tester.runAsync(() async {
        Directory(output).createSync(recursive: true);
        await loadFonts(
          Directory(Platform.environment['TOUHOU_CAPTURE_FONTS']!),
        );
      });
    }
    final boundary = GlobalKey();
    await tester.pumpWidget(
      RepaintBoundary(
        key: boundary,
        child: fixture(accountApi: api),
      ),
    );
    await tester.tap(find.text('主页'));
    await tester.pumpAndSettle();
    expect(find.text('正在检查登录状态'), findsOneWidget);
    expect(find.text('登录'), findsNothing);
    final settingsPosition = tester.getTopLeft(find.text('启动器设置'));
    if (output != null) {
      await save(
        tester,
        boundary,
        Directory(output),
        'mobile-session-checking',
      );
    }
    api.attempts.single.completeError(
      const AccountRequestException(503, 'SERVICE_UNAVAILABLE'),
    );
    await tester.pumpAndSettle();
    final retry = find.byTooltip('重试恢复登录');
    expect(retry.hitTestable(), findsOneWidget);
    expect(find.text('登录').hitTestable(), findsOneWidget);
    expect(tester.getTopLeft(find.text('启动器设置')), settingsPosition);
    if (output != null) {
      await save(tester, boundary, Directory(output), 'mobile-session-retry');
    }
    await tester.tap(retry);
    await tester.pumpAndSettle();
    expect(api.attempts, hasLength(2));
    expect(find.text('正在检查登录状态'), findsOneWidget);
    api.attempts.last.complete((
      username: 'fixture',
      name: '测试玩家',
      expiresAt: null,
    ));
    await tester.pumpAndSettle();
    expect(find.text('测试玩家'), findsOneWidget);
    expect(retry, findsNothing);
    if (output != null) {
      await save(
        tester,
        boundary,
        Directory(output),
        'mobile-session-verified',
      );
    }
    await tester.tap(find.text('单机'));
    await tester.pumpAndSettle();
    expect(find.byType(GameArtwork), findsWidgets);
    expect(tester.takeException(), isNull);
  });
  testWidgets(
    'Narrow shelf keeps the library state through game detail and placeholder',
    (tester) async {
      tester.view.devicePixelRatio = 1;
      tester.view.physicalSize = const Size(320, 640);
      addTearDown(tester.view.resetPhysicalSize);
      addTearDown(tester.view.resetDevicePixelRatio);

      final launcher = RecordingGameLauncher();
      final inputPreferences = GamePreferenceStore(SharedPreferencesAsync());
      await tester.runAsync(() => inputPreferences.touch.load(game.id));
      await tester.pumpWidget(
        fixture(launcher: launcher, gamePreferences: inputPreferences),
      );
      await tester.pumpAndSettle();
      expect(tester.takeException(), isNull);
      final search = find.byType(SearchBar);
      expect(search, findsOneWidget);
      await tester.enterText(search, 'th06');
      await tester.testTextInput.receiveAction(TextInputAction.done);
      await tester.pumpAndSettle();
      final card = find.byType(GameArtwork);
      expect(card, findsOneWidget);
      await tester.tap(card);
      await tester.pumpAndSettle();
      await tester.tap(find.byTooltip('收藏 東方紅魔郷'));
      await tester.pumpAndSettle();
      expect(find.byTooltip('取消收藏 東方紅魔郷'), findsOneWidget);
      await tester.tap(find.byTooltip('游戏设置'));
      await tester.pumpAndSettle();
      for (final name in ['显示设置', '输入设置', '触控设置', '音量设置', '高级设置']) {
        expect(find.text(name), findsOneWidget);
      }
      await tester.tap(find.text('高刷新率'));
      await tester.pumpAndSettle();
      await tester.tap(find.byKey(const ValueKey('language')));
      await tester.pumpAndSettle();
      await tester.tap(find.text('日本語').last);
      await tester.pumpAndSettle();
      await tester.tap(find.text('显示设置'));
      await tester.pumpAndSettle();
      await tester.tap(find.text('输入设置'));
      await tester.pumpAndSettle();
      await tester.tap(find.text('键盘改键'));
      await tester.pumpAndSettle();
      final binding = find.byKey(const ValueKey('keyboard-up'));
      await tester.ensureVisible(binding);
      await tester.tap(binding);
      await tester.pumpAndSettle();
      await tester.sendKeyEvent(LogicalKeyboardKey.escape);
      await tester.pumpAndSettle();
      expect(
        find.descendant(of: binding, matching: find.text('↑')),
        findsOneWidget,
      );
      await tester.tap(binding);
      await tester.pumpAndSettle();
      await tester.sendKeyEvent(
        LogicalKeyboardKey.keyQ,
        physicalKey: PhysicalKeyboardKey.keyJ,
      );
      await tester.pumpAndSettle();
      expect(
        find.descendant(of: binding, matching: find.text('J / ↑')),
        findsOneWidget,
      );
      await tester.ensureVisible(find.text('保存布局'));
      await tester.tap(find.text('保存布局'));
      await tester.pumpAndSettle();
      await tester.enterText(find.byType(TextFormField), '测试键位');
      await tester.pumpAndSettle();
      await tester.tap(find.text('保存'));
      await tester.pumpAndSettle();
      expect(find.text('测试键位'), findsOneWidget);
      await tester.tap(find.text('完成'));
      await tester.pumpAndSettle();
      await tester.tap(find.text('输入设置'));
      await tester.pumpAndSettle();
      await tester.tap(find.text('音量设置'));
      await tester.pumpAndSettle();
      final music = find.byType(Slider).first;
      final bounds = tester.getRect(music);
      await tester.tapAt(
        Offset(bounds.left + bounds.width * .38, bounds.center.dy),
      );
      await tester.pumpAndSettle();
      final volume = tester.widget<Slider>(music).value;
      expect(volume, lessThan(100));
      expect(find.text('快捷发言语音'), findsOneWidget);
      final voiceSlider = find.byType(Slider).last;
      await tester.ensureVisible(voiceSlider);
      final voiceBounds = tester.getRect(voiceSlider);
      await tester.tapAt(
        Offset(
          voiceBounds.left + voiceBounds.width * .24,
          voiceBounds.center.dy,
        ),
      );
      await tester.pumpAndSettle();
      final voiceVolume = tester.widget<Slider>(voiceSlider).value;
      expect(voiceVolume, lessThan(100));
      final savedLauncher = await LauncherPreferences.load(
        SharedPreferencesAsync(),
      );
      expect(savedLauncher.quickChatVolume, voiceVolume.round());
      await tester.tap(find.byType(BackButton));
      await tester.pumpAndSettle();

      final store = GamePreferenceStore(SharedPreferencesAsync());
      final keyboard = await store.keyboard.load(game);
      expect(keyboard.saved[keyboard.selected]?.name, '测试键位');
      expect(keyboard.active.aliases, {'KeyJ': 'ArrowUp'});
      final otherMode = await store.load(game, multiplayer: true);
      expect(otherMode.enabled(GameToggle.frameLimit60Enabled), isTrue);
      expect(otherMode.values['language'], 'lang_zh-hans');
      await tester.tap(find.text('开始游戏'));
      await tester.pumpAndSettle();
      expect(find.text('原生游戏暂不可用'), findsOneWidget);
      expect(launcher.lastMultiplayer, isFalse);
      expect(launcher.lastKeyboardAliases, {'KeyJ': 'ArrowUp'});
      expect(launcher.lastOptions?.values['language'], 'ja');
      expect(launcher.lastOptions?.values['bgmVolume'], volume.toInt());
      expect(
        launcher.lastOptions?.enabled(GameToggle.frameLimit60Enabled),
        isFalse,
      );
      await tester.tap(find.text('返回详情'));
      await tester.pumpAndSettle();
      await tester.tap(find.byType(BackButton));
      await tester.pumpAndSettle();
      expect(tester.widget<SearchBar>(search).controller?.text, 'th06');
      expect(tester.takeException(), isNull);
    },
  );

  testWidgets(
    'Gamepad settings wait for release and persist the learned axis',
    (tester) async {
      tester.view.devicePixelRatio = 1;
      tester.view.physicalSize = const Size(320, 640);
      addTearDown(tester.view.resetPhysicalSize);
      addTearDown(tester.view.resetDevicePixelRatio);
      const channel = MethodChannel('just_gamepads/events');
      final messenger = tester.binding.defaultBinaryMessenger;
      var didCancel = false;
      messenger.setMockMethodCallHandler(channel, (call) async {
        if (call.method == 'cancel') didCancel = true;
        return null;
      });
      Future<void> padEvent(Map<String, Object> event) =>
          messenger.handlePlatformMessage(
            channel.name,
            channel.codec.encodeSuccessEnvelope(event),
            null,
          );
      final preferences = GamePreferenceStore(SharedPreferencesAsync());
      await tester.runAsync(() async {
        await preferences.gamepad.load(game.id);
        await preferences.keyboard.load(game);
        await preferences.touch.load(game.id);
      });
      final launcher = RecordingGameLauncher();
      await tester.pumpWidget(
        fixture(launcher: launcher, gamePreferences: preferences),
      );
      await tester.pumpAndSettle();
      await tester.tap(find.byType(GameArtwork));
      await tester.pumpAndSettle();
      await tester.tap(find.byTooltip('游戏设置'));
      await tester.pumpAndSettle();
      await tester.tap(find.text('输入设置'));
      await tester.pumpAndSettle();
      await tester.tap(find.text('手柄改键'));
      await tester.pumpAndSettle();
      expect(find.text('连接手柄后，先按一下按钮以识别设备。'), findsOneWidget);

      Map<String, Object> controller(String id) => {
        'event': 'connected',
        'id': id,
        'name': 'Xbox Wireless Controller',
        'vendorId': 1118,
        'productId': 2835,
      };

      await padEvent(controller('usb-1'));
      await tester.pumpAndSettle();
      await tester.tap(find.text('请选择手柄'));
      await tester.pumpAndSettle();
      await tester.tap(find.text('Xbox Wireless Controller (#1)').last);
      await tester.pumpAndSettle();
      await tester.tap(find.text('启用手柄映射'));
      await tester.pumpAndSettle();

      final shoot = find.byKey(const ValueKey('gamepad-shoot'));
      await tester.ensureVisible(shoot);
      await padEvent({
        'event': 'state',
        'id': 'usb-1',
        'buttons': [0],
      });
      await tester.pumpAndSettle();
      await tester.tap(shoot);
      await tester.pumpAndSettle();
      await padEvent({
        'event': 'state',
        'id': 'usb-1',
        'buttons': [0],
        'axes': {'rightStickY': -1.0},
      });
      await tester.pumpAndSettle();
      expect(find.text('等待输入…'), findsOneWidget);
      await padEvent({'event': 'state', 'id': 'usb-1', 'buttons': <int>[]});
      await tester.pumpAndSettle();
      await padEvent({
        'event': 'state',
        'id': 'usb-1',
        'axes': {'rightStickY': -.8},
      });
      await tester.pumpAndSettle();
      expect(
        find.descendant(of: shoot, matching: find.text('轴 4 −')),
        findsOneWidget,
      );

      final slider = find.byType(Slider);
      await tester.ensureVisible(slider);
      final bounds = tester.getRect(slider);
      await tester.tapAt(
        Offset(bounds.left + bounds.width * .7, bounds.center.dy),
      );
      await tester.pumpAndSettle();
      final threshold = tester.widget<Slider>(slider).value;
      expect(threshold, greaterThan(.5));

      await padEvent({'event': 'disconnected', 'id': 'usb-1'});
      await tester.pumpAndSettle();
      expect(find.text('已保存的手柄未连接'), findsOneWidget);
      await padEvent(controller('usb-2'));
      await tester.pumpAndSettle();
      expect(
        find.descendant(of: shoot, matching: find.text('轴 4 −')),
        findsOneWidget,
      );
      await tester.ensureVisible(shoot);
      await tester.tap(shoot);
      await tester.pumpAndSettle();
      expect(find.text('等待输入…'), findsOneWidget);
      await tester.sendKeyEvent(LogicalKeyboardKey.escape);
      await tester.pumpAndSettle();
      expect(find.text('等待输入…'), findsNothing);
      expect(shoot, findsOneWidget);
      await tester.tap(shoot);
      await tester.pumpAndSettle();
      expect(find.text('等待输入…'), findsOneWidget);
      await tester.binding.handlePopRoute();
      await tester.pumpAndSettle();
      expect(find.text('等待输入…'), findsNothing);
      expect(shoot, findsOneWidget);
      await tester.tap(shoot);
      await tester.pumpAndSettle();
      await tester.tap(find.text('键盘'));
      await tester.pumpAndSettle();
      expect(shoot, findsNothing);
      expect(find.byKey(const ValueKey('keyboard-up')), findsOneWidget);
      expect(didCancel, isTrue);
      await tester.tap(find.text('完成'));
      await tester.pumpAndSettle();
      expect(didCancel, isTrue);
      await tester.tap(find.byType(BackButton));
      await tester.pumpAndSettle();
      await tester.tap(find.text('开始游戏'));
      await tester.pumpAndSettle();
      final mapping = launcher.lastGamepadMappings!;
      expect(mapping.enabled, isTrue);
      expect(mapping.profile.threshold, threshold);
      expect(mapping.profile.bindings['shoot']?.single.toJson(), {
        'kind': 'axis',
        'index': 3,
        'rest': 0,
        'value': -1,
      });
      expect(find.text('原生游戏暂不可用'), findsOneWidget);
      expect(tester.takeException(), isNull);
    },
  );

  testWidgets(
    'th09 local duel separates player keys and controller assignments',
    (tester) async {
      tester.view.devicePixelRatio = 1;
      tester.view.physicalSize = const Size(320, 640);
      addTearDown(tester.view.resetPhysicalSize);
      addTearDown(tester.view.resetDevicePixelRatio);
      const th09 = Game(
        id: 'th09',
        title: '東方花映塚',
        subtitle: 'Phantasmagoria of Flower View',
        number: '09',
        category: BookCategory.versus,
        nameOrder: 0,
        multiplayer: true,
      );
      final preferences = GamePreferenceStore(SharedPreferencesAsync());
      await tester.runAsync(() async {
        await preferences.keyboard.load(th09);
        await preferences.gamepad.load(th09.id);
        await preferences.localDuel.load();
        await preferences.touch.load(th09.id);
      });
      const channel = MethodChannel('just_gamepads/events');
      final messenger = tester.binding.defaultBinaryMessenger;
      messenger.setMockMethodCallHandler(channel, (_) async => null);
      final launcher = RecordingGameLauncher();
      await tester.pumpWidget(
        fixture(
          launcher: launcher,
          gamePreferences: preferences,
          games: const [th09],
        ),
      );
      await tester.pumpAndSettle();

      Future<void> tap(Finder finder) async {
        await tester.ensureVisible(finder);
        await tester.tap(finder);
        await tester.pumpAndSettle();
      }

      Future<void> chooseDevice(String label) async {
        final local = find.byType(InputSettingSelect<bool>).last;
        await tap(
          find.descendant(of: local, matching: find.byType(TextButton)),
        );
        await tap(find.widgetWithText(MenuItemButton, label));
      }

      Future<void> openController() async {
        await tap(find.text('分配与配置手柄'));
        await messenger.handlePlatformMessage(
          channel.name,
          channel.codec.encodeSuccessEnvelope({
            'event': 'connected',
            'id': 'pad-1',
            'name': 'Xbox Wireless Controller',
            'vendorId': 1118,
            'productId': 2835,
          }),
          null,
        );
        await tester.pumpAndSettle();
        await tap(find.text('请选择手柄'));
        await tap(find.text('Xbox Wireless Controller (#1)').last);
      }

      await tap(find.byType(GameArtwork));
      await tap(find.byTooltip('游戏设置'));
      await tap(find.text('输入设置'));
      await tap(find.text('键盘改键'));
      await tap(find.text('单人 / 联机'));
      await tap(find.text('本地双人'));
      await tap(find.text('启用本地双人配置'));
      expect(find.text('原版左区'), findsOneWidget);
      await tap(find.text('玩家 2'));
      expect(find.text('原版右区'), findsOneWidget);
      final shoot = find.byKey(const ValueKey('duel-shoot'));
      await tap(shoot);
      await tester.sendKeyEvent(LogicalKeyboardKey.keyZ);
      await tester.pumpAndSettle();
      expect(find.text('Z 已被另一位玩家使用'), findsOneWidget);
      await tester.sendKeyEvent(LogicalKeyboardKey.keyK);
      await tester.pumpAndSettle();
      expect(
        find.descendant(of: shoot, matching: find.text('K')),
        findsOneWidget,
      );
      await tap(find.text('保存布局'));
      await tester.enterText(find.byType(TextFormField), '右区备用');
      await tester.pumpAndSettle();
      await tap(find.text('保存'));
      expect(find.text('右区备用'), findsOneWidget);

      await tap(find.text('玩家 1'));
      await chooseDevice('手柄');
      await openController();
      expect(find.text('Xbox Wireless Controller (#1)'), findsOneWidget);
      expect(find.text('启用手柄映射'), findsNothing);
      await tap(find.text('完成').last);
      await tap(find.text('玩家 2'));
      await chooseDevice('手柄');
      await openController();
      expect(find.text('这个手柄已分配给另一位玩家'), findsOneWidget);
      expect(find.text('请选择手柄'), findsOneWidget);
      await tap(find.text('完成').last);
      await chooseDevice('键盘');
      expect(find.text('右区备用'), findsOneWidget);

      await tap(find.text('完成'));
      await tap(find.byType(BackButton));
      await tap(find.text('开始游戏'));
      final local = launcher.lastLocalDuel!;
      expect(local.enabled, isTrue);
      expect(local.players[0].gamepad, isTrue);
      expect(local.players[0].controller, contains('Xbox Wireless Controller'));
      expect(local.players[1].gamepad, isFalse);
      expect(local.players[1].controller, isEmpty);
      expect(local.players[1].bindings['shoot'], ['KeyK']);
      expect(local.players[1].saved[local.players[1].layout]?.name, '右区备用');
      expect(launcher.lastGamepadMappings?.selected, isEmpty);
      expect(find.text('原生游戏暂不可用'), findsOneWidget);
      expect(tester.takeException(), isNull);
    },
  );

  testWidgets(
    'Touch layouts save each orientation and discard only draft changes',
    (tester) async {
      tester.view.devicePixelRatio = 1;
      tester.view.physicalSize = const Size(320, 640);
      addTearDown(tester.view.resetPhysicalSize);
      addTearDown(tester.view.resetDevicePixelRatio);
      final preferences = GamePreferenceStore(SharedPreferencesAsync());
      final initial = (await tester.runAsync(() async {
        await preferences.keyboard.load(game);
        await preferences.gamepad.load(game.id);
        return preferences.touch.load(game.id);
      }))!;
      final launcher = RecordingGameLauncher();
      await tester.pumpWidget(
        fixture(launcher: launcher, gamePreferences: preferences),
      );
      await tester.pumpAndSettle();

      Future<void> tap(Finder finder) async {
        await tester.ensureVisible(finder);
        await tester.pumpAndSettle();
        await tester.tap(finder);
        await tester.pumpAndSettle();
      }

      await tap(find.byType(GameArtwork));
      await tap(find.byTooltip('游戏设置'));
      await tap(find.text('触控设置'));
      await tap(find.text('按键布局与触控设置'));
      await tap(find.byTooltip('收起面板'));
      expect(find.text('选择按键'), findsNothing);
      final fire = find.byKey(const ValueKey('touch-preview-fire'));
      final before = tester.getCenter(fire);
      await tester.drag(fire, const Offset(50, 0));
      await tester.pumpAndSettle();
      expect(tester.getCenter(fire).dx, greaterThan(before.dx + 12));
      await tap(find.byTooltip('展开面板'));
      await tap(find.text('调整画面位置'));
      final viewport = find.byKey(const ValueKey('touch-viewport-gesture'));
      await tester.ensureVisible(viewport);
      await tester.pumpAndSettle();
      await tester.drag(viewport, const Offset(30, 0));
      await tester.pumpAndSettle();
      await tap(find.text('调整完成'));
      await tap(find.text('保存布局'));
      expect(find.text('触控布局已保存'), findsOneWidget);
      final saved = await preferences.touch.load(game.id);
      final portrait = saved.layouts[Orientation.portrait]!;
      expect(portrait.viewportX, greaterThan(0));
      expect(
        portrait.controls[TouchControl.fire]!.position.dx,
        greaterThan(
          initial
              .templates[Orientation.portrait]!
              .defaultProfile
              .controls[TouchControl.fire]!
              .position
              .dx,
        ),
      );

      await tap(find.text('横屏'));
      final size = find.byKey(const ValueKey('按键大小'));
      await tester.ensureVisible(size);
      await tester.pumpAndSettle();
      final bounds = tester.getRect(size);
      await tester.tapAt(
        Offset(bounds.left + bounds.width * .75, bounds.center.dy),
      );
      await tester.pumpAndSettle();
      await tap(find.widgetWithText(ChoiceChip, '200%'));
      await tap(find.text('退出'));
      expect(find.text('退出触控布局？'), findsOneWidget);
      await tap(find.text('继续编辑'));
      await tap(find.text('退出'));
      await tap(find.text('放弃修改'));
      expect(find.text('触控设置'), findsOneWidget);
      await tap(find.byType(BackButton));
      await tap(find.text('开始游戏'));
      final touch = launcher.lastTouch!;
      expect(touch.layouts.keys, [Orientation.portrait]);
      expect(touch.layouts[Orientation.portrait]!.matches(portrait), isTrue);
      expect(touch.options.sensitivity, 200);
      final other = await preferences.touch.load('th07');
      expect(other.layouts[Orientation.portrait]!.matches(portrait), isTrue);
      final fighting = await preferences.touch.load('th075');
      expect(fighting.layouts, isEmpty);
      expect(fighting.options.movement, 'joystick');
      expect(tester.takeException(), isNull);
    },
  );

  testWidgets(
    'Wide library shares the layout without a bottom navigation bar',
    (tester) async {
      tester.view.devicePixelRatio = 1;
      tester.view.physicalSize = const Size(1440, 734);
      addTearDown(tester.view.resetPhysicalSize);
      addTearDown(tester.view.resetDevicePixelRatio);

      await tester.pumpWidget(fixture());
      await tester.pumpAndSettle();
      expect(find.byType(NavigationBar), findsNothing);
      expect(find.byType(SearchBar), findsOneWidget);
      expect(find.text('正作'), findsOneWidget);
      await tester.tap(find.text('列表'));
      await tester.pumpAndSettle();
      expect(find.text(game.subtitle), findsWidgets);
      expect(tester.takeException(), isNull);
    },
  );

  testWidgets('Mobile account web preserves its native parent', (tester) async {
    tester.view.devicePixelRatio = 1;
    tester.view.physicalSize = const Size(320, 640);
    addTearDown(tester.view.resetPhysicalSize);
    addTearDown(tester.view.resetDevicePixelRatio);
    await tester.pumpWidget(fixture(accountApi: FixtureAccountApi()));
    await tester.pumpAndSettle();
    await tester.tap(find.text('主页'));
    await tester.pumpAndSettle();
    final position = tester.getTopLeft(find.text('启动器设置'));
    final login = await openAccountAndReturn(tester, find.text('登录'));
    expect(login.path, '/login.html');
    expect(tester.getTopLeft(find.text('启动器设置')), position);
    final register = await openAccountAndReturn(tester, find.text('注册'));
    expect(register.queryParameters['mode'], 'register');
    expect(tester.getTopLeft(find.text('启动器设置')), position);
    expect(tester.takeException(), isNull);
  });

  testWidgets(
    'Inbox restored session, read acknowledgement, and retry use the same stack',
    (tester) async {
      tester.view.devicePixelRatio = 1;
      tester.view.physicalSize = const Size(390, 844);
      addTearDown(tester.view.resetPhysicalSize);
      addTearDown(tester.view.resetDevicePixelRatio);
      final api = FixtureAccountApi();
      await tester.pumpWidget(fixture(accountApi: api));
      await tester.pumpAndSettle();

      await tester.tap(find.byTooltip('私信'));
      await tester.pumpAndSettle();
      await signInFixture(tester);

      await tester.tap(find.text('另一位玩家'));
      await tester.pumpAndSettle();
      expect(find.text('第一条测试私信'), findsOneWidget);
      expect(api.readThrough, 1);

      await tester.enterText(find.byType(TextField), '本地重试样例');
      await tester.pumpAndSettle();
      await tester.tap(find.byTooltip('发送私信'));
      await tester.pumpAndSettle();
      expect(find.text('发送失败，请重试'), findsOneWidget);
      expect(api.sent, isNull);
      await tester.tap(find.byTooltip('发送私信'));
      await tester.pumpAndSettle();
      expect(find.text('本地重试样例'), findsOneWidget);
      expect(api.sent, '本地重试样例');

      await tester.tap(find.byType(BackButton));
      await tester.pumpAndSettle();
      expect(find.text('另一位玩家'), findsOneWidget);
      await tester.tap(find.byType(BackButton));
      await tester.pumpAndSettle();
      expect(find.byType(SearchBar), findsOneWidget);
    },
  );

  testWidgets(
    'Only a publisher sees the composer even when the list is empty',
    (tester) async {
      tester.view.devicePixelRatio = 1;
      tester.view.physicalSize = const Size(390, 844);
      addTearDown(tester.view.resetPhysicalSize);
      addTearDown(tester.view.resetDevicePixelRatio);
      final api = FixtureAccountApi();
      await tester.pumpWidget(fixture(accountApi: api));
      await tester.pumpAndSettle();

      await tester.tap(find.byTooltip('系统公告'));
      await tester.pumpAndSettle();
      expect(find.text('发布公告'), findsNothing);
      await tester.tap(find.text('私信'));
      await tester.pumpAndSettle();
      await signInFixture(tester);
      await tester.tap(find.text('系统公告'));
      await tester.pumpAndSettle();
      expect(find.text('暂无公告'), findsOneWidget);
      expect(find.text('发布公告'), findsOneWidget);
      await tester.enterText(find.byType(TextField), '本地发布样例');
      await tester.pumpAndSettle();
      await tester.tap(find.text('发布公告'));
      await tester.pumpAndSettle();
      expect(api.posted, '本地发布样例');
      expect(find.text('本地发布样例'), findsOneWidget);
    },
  );
}
