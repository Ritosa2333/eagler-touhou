import 'dart:convert';
import 'dart:io';
import 'dart:ui' as ui;

import 'package:flutter/services.dart';
import 'package:flutter/foundation.dart';
import 'package:flutter/rendering.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:go_router/go_router.dart';
import 'package:material_ui/material_ui.dart';
import 'package:shared_preferences/shared_preferences.dart';
import 'package:shared_preferences_platform_interface/in_memory_shared_preferences_async.dart';
import 'package:shared_preferences_platform_interface/shared_preferences_async_platform_interface.dart';
import 'package:touhouonline/account_api.dart';
import 'package:touhouonline/catalog.dart';
import 'package:touhouonline/launcher_select.dart';
import 'package:touhouonline/game_launcher.dart';
import 'package:touhouonline/game_preferences.dart';
import 'package:touhouonline/game_resources.dart';
import 'package:touhouonline/game_user_files.dart';
import 'package:touhouonline/library.dart';
import 'package:touhouonline/l10n/strings.dart';
import 'package:touhouonline/main.dart';

Future<void> save(
  WidgetTester tester,
  GlobalKey boundary,
  Directory output,
  String name,
) async {
  await tester.runAsync(() async {
    final renderObject = boundary.currentContext!.findRenderObject();
    final image = await (renderObject! as RenderRepaintBoundary).toImage();
    try {
      final png = (await image.toByteData(format: ui.ImageByteFormat.png))!;
      await File('${output.path}/$name.png').writeAsBytes(
        png.buffer.asUint8List(png.offsetInBytes, png.lengthInBytes),
      );
    } finally {
      image.dispose();
    }
  });
}

Future<void> loadFonts([Directory? sdkFonts]) async {
  if (sdkFonts == null) {
    final config = File('.dart_tool/package_config.json').absolute;
    final packages = jsonDecode(config.readAsStringSync())['packages'] as List;
    final flutter = packages.singleWhere(
      (dynamic item) => item['name'] == 'flutter',
    );
    final sdk = Directory.fromUri(
      config.uri.resolve(flutter['rootUri'] as String),
    ).parent.parent;
    sdkFonts = Directory('${sdk.path}/bin/cache/artifacts/material_fonts');
  }
  final windows = Directory(Platform.environment['WINDIR']!);
  final fonts = {
    'Roboto': [
      File('${sdkFonts.path}/roboto-regular.ttf'),
      File('${sdkFonts.path}/roboto-medium.ttf'),
      File('${sdkFonts.path}/roboto-bold.ttf'),
    ],
    'Segoe UI': [
      File('${windows.path}/Fonts/segoeui.ttf'),
      File('${windows.path}/Fonts/segoeuib.ttf'),
    ],
    if (File('${windows.path}/Fonts/NotoSansSC-VF.ttf').existsSync())
      'Noto Sans SC': [File('${windows.path}/Fonts/NotoSansSC-VF.ttf')],
    'Microsoft YaHei': [
      File('${windows.path}/Fonts/msyh.ttc'),
      File('${windows.path}/Fonts/msyhbd.ttc'),
    ],
    'Microsoft YaHei UI': [
      File('${windows.path}/Fonts/msyh.ttc'),
      File('${windows.path}/Fonts/msyhbd.ttc'),
    ],
    if (File('${windows.path}/Fonts/YuGothM.ttc').existsSync())
      'Yu Gothic UI': [File('${windows.path}/Fonts/YuGothM.ttc')],
    'Meiryo UI': [
      if (File('${windows.path}/Fonts/meiryo.ttc').existsSync())
        File('${windows.path}/Fonts/meiryo.ttc')
      else ...[
        // The test engine substitutes Ahem for unknown families. Reusing the
        // preferred system face gives an absent fallback no additional glyphs.
        File('${windows.path}/Fonts/segoeui.ttf'),
        File('${windows.path}/Fonts/segoeuib.ttf'),
      ],
    ],
    'MaterialIcons': [File('${sdkFonts.path}/materialicons-regular.otf')],
  };
  final lucide = FontLoader('packages/lucide_icons_flutter/Lucide')
    ..addFont(
      rootBundle.load('packages/lucide_icons_flutter/assets/lucide.ttf'),
    );
  await lucide.load();
  for (final entry in fonts.entries) {
    final loader = FontLoader(entry.key);
    for (final file in entry.value) {
      loader.addFont(file.readAsBytes().then(ByteData.sublistView));
    }
    await loader.load();
  }
}

class _CaptureGameResources extends GameResources {
  @override
  Future<File?> installed(GameData? data) async => null;
}

class _CaptureHttp extends HttpOverrides {}

class _CaptureAnnouncements extends AccountApi {
  _CaptureAnnouncements(this.page, this.avatars);

  final AnnouncementPage page;
  final Map<String, Uri> avatars;
  @override
  Future<AnnouncementPage> announcements({int? before}) async => page;

  @override
  Future<Uri?> avatar(String username) async => avatars[username];
}

Future<_CaptureAnnouncements> _fetchGuestAnnouncements() =>
    HttpOverrides.runWithHttpOverrides(() async {
      final api = AccountApi();
      final http = HttpClient()
        ..connectionTimeout = const Duration(seconds: 10);
      try {
        final page = await api.announcements();
        final avatars = <String, Uri>{};
        for (final username
            in page.items
                .map((item) => item.authorUsername)
                .whereType<String>()
                .toSet()) {
          final avatar = await api.avatar(username);
          if (avatar == null) continue;
          final response = await (await http.getUrl(avatar))
              .close()
              .timeout(const Duration(seconds: 10));
          if (response.statusCode != 200) {
            throw HttpException('Cannot load public avatar', uri: avatar);
          }
          final bytes = await consolidateHttpClientResponseBytes(response);
          final image = await decodeImageFromList(bytes);
          PaintingBinding.instance.imageCache.putIfAbsent(
            NetworkImage(avatar.toString()),
            () => OneFrameImageStreamCompleter(
              SynchronousFuture(ImageInfo(image: image)),
            ),
          );
          avatars[username] = avatar;
        }
        return _CaptureAnnouncements(page, avatars);
      } finally {
        api.close();
        http.close(force: true);
      }
    }, _CaptureHttp());

void main() {
  final output = Directory(Platform.environment['TOUHOU_CAPTURE_DIR']!);
  final sdkFonts = Directory(Platform.environment['TOUHOU_CAPTURE_FONTS']!);

  for (final (name, size) in [
    ('desktop', const Size(1440, 734)),
    ('mobile', const Size(390, 844)),
    ('mobile-narrow', const Size(320, 640)),
  ]) {
    testWidgets('Render $name library and detail', (tester) async {
      tester.view.devicePixelRatio = 1;
      tester.view.physicalSize = size;
      addTearDown(tester.view.resetPhysicalSize);
      addTearDown(tester.view.resetDevicePixelRatio);

      SharedPreferencesAsyncPlatform.instance =
          InMemorySharedPreferencesAsync.empty();
      final storage = SharedPreferencesAsync();
      final games = (await tester.runAsync(Game.load))!;
      await tester.runAsync(() => loadFonts(sdkFonts));
      final announcements = (await tester.runAsync(_fetchGuestAnnouncements))!;
      final gamePreferences = GamePreferenceStore(storage);
      await tester.runAsync(() async {
        await gamePreferences.keyboard.load(
          games.firstWhere((game) => game.id == 'th06'),
        );
        await gamePreferences.gamepad.load('th06');
        await gamePreferences.localDuel.load();
        await gamePreferences.touch.load('th06');
      });
      tester.binding.defaultBinaryMessenger.setMockMethodCallHandler(
        const MethodChannel('just_gamepads/events'),
        (_) async => null,
      );
      final favorites = GameFavorites(storage, {});
      final history = await GamePlayHistory.load(storage);
      final boundary = GlobalKey();
      final storageRoot = (await tester.runAsync(
        () => output.createTemp('game-file-screen-'),
      ))!;
      addTearDown(() => storageRoot.delete(recursive: true));
      final gameFiles = GameUserFiles(appDirectory: () async => storageRoot);
      await tester.pumpWidget(
        RepaintBoundary(
          key: boundary,
          child: LauncherApp(
            book: BookState(games, favorites, history: history),
            lobby: BookState(games, favorites),
            preferences: LauncherPreferences(storage, ThemeMode.dark, true),
            launcher: const NativeGamePlaceholder(),
            gamePreferences: gamePreferences,
            gameResources: _CaptureGameResources(),
            gameUserFiles: gameFiles,
            accountApi: announcements,
          ),
        ),
      );
      await tester.pumpAndSettle();
      expect(tester.takeException(), isNull);
      final context = tester.element(find.byType(LibraryPage));
      final router = GoRouter.of(context);
      await tester.runAsync(() async {
        for (final game in games) {
          if (game.artwork case final String cover) {
            await precacheImage(
              AssetImage('assets/book-covers/$cover'),
              context,
            );
          }
        }
      });
      await tester.pumpAndSettle();
      await save(tester, boundary, output, '$name-library');
      if (name == 'desktop') {
        await tester.tap(find.byTooltip('系统公告'));
        await tester.pumpAndSettle();
        expect(find.text('最新公告'), findsOneWidget);
        await save(tester, boundary, output, '$name-announcement-preview');
        await tester.tap(find.byTooltip('私信'));
        await tester.pumpAndSettle();
        expect(find.text('打开私信收件箱'), findsOneWidget);
        await save(tester, boundary, output, '$name-inbox-preview-guest');
        await tester.tap(find.byTooltip('私信'));
        await tester.pumpAndSettle();
      }

      Future<void> chooseOrder(GameOrder order) async {
        await tester.tap(find.byType(LauncherSelect<GameOrder>));
        await tester.pumpAndSettle();
        await tester.tap(find.text(order.label(context.strings)).last);
        await tester.pumpAndSettle();
      }

      await chooseOrder(GameOrder.reverse);
      await save(tester, boundary, output, '$name-library-reverse');
      await tester.tap(find.text('列表'));
      await tester.pumpAndSettle();
      await save(tester, boundary, output, '$name-library-list-reverse');
      await chooseOrder(GameOrder.number);
      await save(tester, boundary, output, '$name-library-list');
      await tester.tap(find.text('封面'));
      await tester.pumpAndSettle();

      final card = find.byWidgetPredicate(
        (widget) =>
            widget is GameArtwork &&
            widget.game.id == 'th06' &&
            widget.size == null,
      );
      await tester.tap(card);
      await tester.pumpAndSettle();
      expect(find.text('开始游戏'), findsOneWidget);
      expect(tester.takeException(), isNull);
      await save(tester, boundary, output, '$name-game-detail');
      final import = find.widgetWithText(OutlinedButton, '导入');
      await tester.tap(import);
      await tester.pumpAndSettle();
      expect(find.text('导入游戏资源'), findsOneWidget);
      await save(tester, boundary, output, '$name-game-data-import');
      await tester.tap(find.byTooltip('关闭'));
      await tester.pumpAndSettle();
      final settings = name == 'mobile-narrow'
          ? find.byTooltip('游戏设置')
          : find.widgetWithText(OutlinedButton, '游戏设置');
      await tester.tap(settings);
      await tester.pumpAndSettle();
      expect(find.text('显示设置'), findsOneWidget);
      expect(tester.takeException(), isNull);
      await save(tester, boundary, output, '$name-game-settings');
      if (name == 'desktop') {
        await tester.tap(find.text('显示设置'));
        await tester.pumpAndSettle();
        expect(find.text('高刷新率'), findsOneWidget);
        expect(tester.takeException(), isNull);
        await save(tester, boundary, output, '$name-game-settings-display');
      } else {
        expect(find.text('高刷新率'), findsOneWidget);
      }
      await tester.ensureVisible(find.text('输入设置'));
      await tester.tap(find.text('输入设置'));
      await tester.pumpAndSettle();
      final keyboard = find.text('键盘改键');
      await tester.ensureVisible(keyboard);
      await tester.tap(keyboard);
      await tester.pumpAndSettle();
      expect(find.text('键盘布局'), findsOneWidget);
      expect(tester.takeException(), isNull);
      await save(tester, boundary, output, '$name-game-keyboard');
      await tester.tap(find.text('完成'));
      await tester.pumpAndSettle();
      final gamepad = find.text('手柄改键');
      await tester.ensureVisible(gamepad);
      await tester.tap(gamepad);
      await tester.pumpAndSettle();
      expect(find.text('请选择手柄'), findsOneWidget);
      expect(tester.takeException(), isNull);
      await save(tester, boundary, output, '$name-game-gamepad');
      await tester.tap(find.text('完成'));
      await tester.pumpAndSettle();
      Future<void> captureTouch(String screen) async {
        final group = find.text('触控设置');
        await tester.ensureVisible(group);
        await tester.pumpAndSettle();
        await tester.tap(group);
        await tester.pumpAndSettle();
        final entry = find.text('按键布局与触控设置');
        await tester.ensureVisible(entry);
        await tester.pumpAndSettle();
        await tester.tap(entry);
        await tester.pumpAndSettle();
        expect(
          find.byKey(const ValueKey('touch-preview-fire')),
          findsOneWidget,
        );
        expect(tester.takeException(), isNull);
        await save(tester, boundary, output, '$name-$screen');
        await tester.tap(find.text('退出'));
        await tester.pumpAndSettle();
      }

      await captureTouch('touch-layout');
      await tester.tap(find.byType(BackButton));
      await tester.pumpAndSettle();
      expect(find.text('开始游戏'), findsOneWidget);
      await tester.tap(find.byType(BackButton));
      await tester.pumpAndSettle();
      final search = find.byType(SearchBar);
      await tester.enterText(search, 'th09');
      await tester.testTextInput.receiveAction(TextInputAction.done);
      await tester.pumpAndSettle();
      final th09 = find.byWidgetPredicate(
        (widget) =>
            widget is GameArtwork &&
            widget.game.id == 'th09' &&
            widget.size == null,
      );
      await tester.ensureVisible(th09);
      await tester.tap(th09);
      await tester.pumpAndSettle();
      await tester.tap(settings);
      await tester.pumpAndSettle();
      final input = find.text('输入设置');
      await tester.ensureVisible(input);
      await tester.tap(input);
      await tester.pumpAndSettle();
      final th09Keyboard = find.text('键盘改键');
      await tester.ensureVisible(th09Keyboard);
      await tester.tap(th09Keyboard);
      await tester.pumpAndSettle();
      await tester.tap(find.text('单人 / 联机'));
      await tester.pumpAndSettle();
      await tester.tap(find.text('本地双人'));
      await tester.pumpAndSettle();
      expect(find.text('原版左区'), findsOneWidget);
      expect(tester.takeException(), isNull);
      await save(tester, boundary, output, '$name-th09-local-duel');
      await tester.tap(find.text('完成'));
      await tester.pumpAndSettle();

      final fileGroup = find.text('存档与录像');
      await tester.ensureVisible(fileGroup);
      await tester.tap(fileGroup);
      await tester.pumpAndSettle();
      final manage = find.byKey(const ValueKey((GameUserFileKind.replay, 1)));
      await tester.ensureVisible(manage);
      await tester.pumpAndSettle();
      expect(tester.takeException(), isNull);
      await save(tester, boundary, output, '$name-game-files');
      final workspace = gameFiles.workspace(
        games.singleWhere((game) => game.id == 'th09'),
        multiplayer: false,
        language: 'ja',
      )!;
      await tester.runAsync(() async {
        await tester.tap(manage);
        await tester.pump();
        await workspace.replays();
      });
      await tester.pumpAndSettle();
      expect(find.text('这里还没有录像'), findsOneWidget);
      await save(tester, boundary, output, '$name-replay-manager-empty');
      await tester.tap(find.byTooltip('关闭'));
      await tester.pumpAndSettle();
      expect(manage, findsOneWidget);

      if (Platform.environment['TOUHOU_CAPTURE_TH09_REPLAY']
          case final sample?) {
        await tester.runAsync(() async {
          final replay = File(sample);
          await workspace.importFile(GameUserFileKind.replay, (
            name: replay.uri.pathSegments.last,
            length: replay.length,
            read: replay.openRead,
          ));
          await tester.tap(manage);
          await tester.pump();
          await workspace.replays();
        });
        await tester.pumpAndSettle();
        expect(find.text('th9_01.rpy'), findsOneWidget);
        await tester.runAsync(() async {
          await tester.tap(find.byTooltip('录像操作'));
          await tester.pumpAndSettle();
        });
        await save(tester, boundary, output, '$name-replay-manager-actions');
        await tester.runAsync(() async {
          await tester.tap(find.text('播放'));
          final deadline = DateTime.now().add(const Duration(seconds: 3));
          final dialog = find.text('原生录像播放暂不可用');
          while (dialog.evaluate().isEmpty &&
              DateTime.now().isBefore(deadline)) {
            await Future<void>.delayed(const Duration(milliseconds: 20));
            await tester.pump(const Duration(milliseconds: 20));
          }
          expect(dialog, findsOneWidget);
        });
        await tester.runAsync(() async {
          await tester.pump(const Duration(milliseconds: 250));
          await tester.tap(find.text('返回录像管理'));
          await tester.pumpAndSettle();
        });
        expect(find.text('th9_01.rpy'), findsOneWidget);
        await tester.tap(find.byTooltip('关闭'));
        await tester.pumpAndSettle();
        expect(manage, findsOneWidget);
      }

      for (var depth = 0; depth < 2; depth++) {
        await tester.tap(find.byType(BackButton));
        await tester.pumpAndSettle();
      }
      await tester.enterText(search, 'th075');
      await tester.testTextInput.receiveAction(TextInputAction.done);
      await tester.pumpAndSettle();
      final fighting = find.byWidgetPredicate(
        (widget) =>
            widget is GameArtwork &&
            widget.game.id == 'th075' &&
            widget.size == null,
      );
      await tester.ensureVisible(fighting);
      await tester.pumpAndSettle();
      await tester.tap(fighting);
      await tester.pumpAndSettle();
      await tester.tap(settings);
      await tester.pumpAndSettle();
      await captureTouch('touch-layout-fighting');
      for (var depth = 0; depth < 2; depth++) {
        await tester.tap(find.byType(BackButton));
        await tester.pumpAndSettle();
      }
      router.go('/');
      await tester.pumpAndSettle();
      await tester.tap(find.byTooltip('系统公告'));
      await tester.pumpAndSettle();
      if (name == 'desktop') {
        await tester.tap(find.text('查看全部'));
        await tester.pumpAndSettle();
      }
      expect(find.text('消息中心'), findsOneWidget);
      expect(find.text('发布公告'), findsNothing);
      expect(tester.takeException(), isNull);
      await save(tester, boundary, output, '$name-announcements-guest');
      await tester.tap(find.text('私信'));
      await tester.pumpAndSettle();
      expect(find.text('登录后查看私信'), findsOneWidget);
      await save(tester, boundary, output, '$name-messages-guest');

      final fixture = (await tester.runAsync(
        () => File('tools/recent-library-fixture.json').readAsString(),
      ))!;
      final dates =
          (jsonDecode(fixture) as Map<String, dynamic>)['dates']
              as Map<String, dynamic>;
      for (final MapEntry(:key, :value) in dates.entries.toList().reversed) {
        await history.record(
          key,
          playedAt: DateTime.fromMillisecondsSinceEpoch(value as int),
        );
      }
      router.go('/');
      await tester.pumpAndSettle();
      final clearSearch = find.byTooltip('清空搜索');
      if (clearSearch.evaluate().isNotEmpty) {
        await tester.tap(clearSearch);
        await tester.pumpAndSettle();
      }
      expect(find.text('最近游玩'), findsOneWidget);
      expect(find.text('${games.length} 部作品'), findsOneWidget);
      await save(tester, boundary, output, '$name-library-recent');
      final recentRail = find.byKey(
        const PageStorageKey('recently played games'),
      );
      await tester.tap(
        find.descendant(
          of: recentRail,
          matching: find.byKey(const ValueKey('th06')),
        ),
      );
      await tester.pumpAndSettle();
      final localizations = MaterialLocalizations.of(
        tester.element(find.byType(BackButton)),
      );
      final date = localizations.formatShortMonthDay(
        history.lastPlayed('th06')!.toLocal(),
      );
      expect(find.text(date), findsOneWidget);
      await save(tester, boundary, output, '$name-game-last-played');
    }, variant: TargetPlatformVariant.only(TargetPlatform.windows));
  }
}
