import 'dart:async';
import 'dart:io';
import 'dart:ui' as ui;

import 'package:flutter/foundation.dart' show listEquals;
import 'package:flutter/rendering.dart' show RenderRepaintBoundary;
import 'package:flutter_test/flutter_test.dart';
import 'package:go_router/go_router.dart';
import 'package:skeletonizer/skeletonizer.dart';
import 'package:material_ui/material_ui.dart';
import 'package:shared_preferences/shared_preferences.dart';
import 'package:shared_preferences_platform_interface/in_memory_shared_preferences_async.dart';
import 'package:shared_preferences_platform_interface/shared_preferences_async_platform_interface.dart';
import 'package:touhouonline/library.dart';
import 'package:touhouonline/catalog.dart';
import 'package:touhouonline/desktop_masthead.dart';
import 'package:touhouonline/profile_data.dart';
import 'package:touhouonline/profile_page.dart';
import 'package:touhouonline/profile_games.dart';
import 'package:touhouonline/statistics_tile.dart';

import '../tools/capture_native_baseline_test.dart' show loadFonts, save;
import 'widget_test.dart' as launcher;

Future<List<int>> _sitePixels(WidgetTester tester, GlobalKey boundary) async {
  final site = tester.getRect(find.byKey(const ValueKey('masthead site')));
  final render =
      boundary.currentContext!.findRenderObject()! as RenderRepaintBoundary;
  return (await tester.runAsync(() async {
    final image = await render.toImage(pixelRatio: 1);
    try {
      final data = (await image.toByteData(
        format: ui.ImageByteFormat.rawRgba,
      ))!;
      final bytes = data.buffer.asUint8List(
        data.offsetInBytes,
        data.lengthInBytes,
      );
      return [
        for (var y = site.top.ceil(); y < site.bottom.floor(); y++)
          ...bytes.sublist(
            (y * image.width + site.left.ceil()) * 4,
            (y * image.width + site.right.floor()) * 4,
          ),
      ];
    } finally {
      image.dispose();
    }
  }))!;
}

class _ProfileFixture extends launcher.FixtureAccountApi {
  final calls = <(String, ProfileList?)>[];
  bool following = false;

  @override
  Future<PublicProfile> profile(String username, {bool fresh = false}) async =>
      PublicProfile(
        username: username,
        nickname: username == 'fixture' ? '测试玩家' : '其他玩家',
        bio: username == 'fixture' ? '用于验证主页的简介' : '公开资料',
      );

  @override
  Future<ProfileSocial> social(
    String username, {
    required bool signedIn,
    ProfileList? list,
  }) async {
    calls.add((username, list));
    if (username == 'other') {
      return ProfileSocial(
        visible: false,
        following: signedIn ? following : null,
      );
    }
    return ProfileSocial(
      visible: true,
      followingCount: 3,
      followerCount: 7,
      people: list == ProfileList.followers
          ? [(username: 'other', nickname: '其他玩家', region: null)]
          : [],
    );
  }

  @override
  Future<void> setFollowing(String username, bool follow) async {
    expect(username, 'other');
    following = follow;
  }

  @override
  Future<ProfileStatistics> profileStatistics(
    String username, {
    required bool owner,
  }) async => const ProfileStatistics(
    career: true,
    records: true,
    playtime: 105,
    games: {
      'th06': (
        game: 'th06',
        playtime: 105,
        playCount: 1,
        progress: [
          (
            difficulty: PlayDifficulty.normal,
            character: 0,
            shot: 0,
            mode: null,
            cleared: true,
            oneCreditClear: true,
            bestScore: 1111,
            playCount: 1,
          ),
        ],
      ),
    },
  );

  @override
  Future<DuelHistory> duels(
    String username, {
    required bool owner,
    String? cursor,
  }) async =>
      const DuelHistory(games: 0, wins: 0, rounds: [], nextCursor: null);
}

class _SlowProfile extends _ProfileFixture {
  final ready = Completer<PublicProfile>();
  @override
  Future<PublicProfile> profile(String username, {bool fresh = false}) =>
      ready.future;
}

Future<void> _signIn(WidgetTester tester) async {
  final fields = find.byType(TextFormField);
  await tester.enterText(fields.first, 'fixture');
  await tester.enterText(fields.last, 'valid-test-value');
  await tester.tap(find.widgetWithText(FilledButton, '登录'));
  await tester.pumpAndSettle();
}

Future<void> _tapText(WidgetTester tester, String text) async {
  final control = find.text(text);
  await tester.ensureVisible(control);
  await tester.tap(control);
  await tester.pumpAndSettle();
}

void main() {
  setUp(
    () => SharedPreferencesAsyncPlatform.instance =
        InMemorySharedPreferencesAsync.empty(),
  );
  for (final width in [781.0, 1440.0]) {
    testWidgets(
      'Desktop masthead stays painted while profile enters and leaves | $width',
      (tester) async {
        tester.view.devicePixelRatio = 1;
        tester.view.physicalSize = Size(width, 734);
        addTearDown(tester.view.resetPhysicalSize);
        addTearDown(tester.view.resetDevicePixelRatio);
        final capture = Platform.environment['TOUHOU_PROFILE_FRAME_CAPTURE'];
        if (capture != null) {
          Directory(capture).createSync(recursive: true);
          await tester.runAsync(
            () => loadFonts(
              Directory(Platform.environment['TOUHOU_CAPTURE_FONTS']!),
            ),
          );
        }
        final boundary = GlobalKey();
        await tester.pumpWidget(
          RepaintBoundary(
            key: boundary,
            child: launcher.fixture(
              accountApi: _ProfileFixture(),
              preferences: LauncherPreferences(
                SharedPreferencesAsync(),
                ThemeMode.dark,
                false,
              ),
            ),
          ),
        );
        await tester.pumpAndSettle();
        final header = find.byType(DesktopMasthead);
        await tester.runAsync(
          () => precacheImage(
            const AssetImage('assets/launcher-wordmark-dark.png'),
            tester.element(header),
          ),
        );
        await tester.pumpAndSettle();
        final element = tester.element(header);
        final bounds = tester.getRect(header);
        final pixels = await _sitePixels(tester, boundary);
        final router = GoRouter.of(tester.element(find.byType(LibraryPage)));
        for (final entering in [true, false]) {
          if (entering) {
            router.push('/u/fixture');
          } else {
            router.pop();
          }
          await tester.pump();
          for (var frame = 0; frame < 6; frame++) {
            await tester.pump(const Duration(milliseconds: 40));
            expect(header, findsOneWidget);
            expect(tester.element(header), same(element));
            expect(tester.getRect(header), bounds);
            expect(
              listEquals(await _sitePixels(tester, boundary), pixels),
              isTrue,
              reason: 'Fixed site controls must not fade with the child page',
            );
            if (capture != null && frame == 1) {
              await save(
                tester,
                boundary,
                Directory(capture),
                '${width.toInt()}-profile-${entering ? 'enter' : 'leave'}',
              );
            }
          }
          await tester.pumpAndSettle();
          expect(
            find.byType(ProfilePage),
            entering ? findsOneWidget : findsNothing,
          );
          expect(tester.takeException(), isNull);
        }
      },
      variant: TargetPlatformVariant.only(TargetPlatform.windows),
    );
  }
  for (final width in [320.0, 1440.0]) {
    testWidgets('Slow profile keeps the cover and avatar in place | $width', (
      tester,
    ) async {
      tester.view.devicePixelRatio = 1;
      tester.view.physicalSize = Size(width, 734);
      addTearDown(tester.view.resetDevicePixelRatio);
      addTearDown(tester.view.resetPhysicalSize);
      final capture = Platform.environment['TOUHOU_MOTION_CAPTURE'];
      final boundary = GlobalKey();
      if (capture != null) {
        await tester.runAsync(
          () => loadFonts(
            Directory(Platform.environment['TOUHOU_CAPTURE_FONTS']!),
          ),
        );
      }
      final profile = _SlowProfile();
      await tester.pumpWidget(
        RepaintBoundary(
          key: boundary,
          child: launcher.fixture(accountApi: profile),
        ),
      );
      await tester.pumpAndSettle();
      GoRouter.of(tester.element(find.byType(LibraryPage))).push('/u/fixture');
      await tester.pumpAndSettle();
      final cover = find.byKey(const ValueKey('profile cover'));
      final avatar = find.byKey(const ValueKey('profile avatar'));
      final loading = find.byWidgetPredicate(
        (widget) => widget is Skeletonizer,
      );
      expect(loading, findsOneWidget);
      expect(find.byType(BackButton).hitTestable(), findsOneWidget);
      expect(find.byType(CircularProgressIndicator), findsNothing);
      final coverBounds = tester.getRect(cover);
      final avatarBounds = tester.getRect(avatar);
      if (capture != null) {
        await save(
          tester,
          boundary,
          Directory(capture),
          'profile-loading-${width.toInt()}',
        );
      }
      profile.ready.complete(
        PublicProfile(username: 'fixture', nickname: '测试玩家', bio: '测试用简介'),
      );
      await tester.pumpAndSettle();
      expect(loading, findsNothing);
      expect(tester.getRect(cover), coverBounds);
      expect(tester.getRect(avatar), avatarBounds);
      expect(find.text('测试玩家').hitTestable(), findsOneWidget);
      if (capture != null) {
        await save(
          tester,
          boundary,
          Directory(capture),
          'profile-loaded-${width.toInt()}',
        );
      }
    });
  }
  testWidgets('320 px home and social list keep entry and privacy', (
    tester,
  ) async {
    tester.view.devicePixelRatio = 1;
    tester.view.physicalSize = const Size(320, 640);
    addTearDown(tester.view.resetDevicePixelRatio);
    addTearDown(tester.view.resetPhysicalSize);
    final fixture = _ProfileFixture();
    await tester.pumpWidget(launcher.fixture(accountApi: fixture));
    await tester.pumpAndSettle();
    await _tapText(tester, '主页');
    await _tapText(tester, '登录');
    await _signIn(tester);
    await _tapText(tester, '粉丝');
    expect(fixture.calls, contains(('fixture', ProfileList.followers)));
    expect(find.text('关系列表'), findsOneWidget);
    await _tapText(tester, '其他玩家');
    expect(find.byType(ProfilePage), findsOneWidget);
    expect(find.text('公开资料'), findsOneWidget);
    expect(find.text('粉丝'), findsNothing);
    expect(find.byTooltip('私信').hitTestable(), findsOneWidget);
    await _tapText(tester, '关注');
    expect(fixture.following, isTrue);
    expect(find.text('取消关注'), findsOneWidget);
    await tester.tap(find.byType(BackButton));
    await tester.pumpAndSettle();
    expect(find.text('关系列表'), findsOneWidget);
    expect(find.text('其他玩家'), findsOneWidget);
    await tester.tap(find.byType(BackButton));
    await tester.pumpAndSettle();
    expect(find.text('启动器设置'), findsOneWidget);
    expect(tester.takeException(), isNull);
  });

  testWidgets(
    'Desktop account menu opens profile and work keeps career position',
    (tester) async {
      final capture = Platform.environment['TOUHOU_PROFILE_WORK_CAPTURE'];
      final boundary = GlobalKey();
      if (capture != null) await tester.runAsync(loadFonts);
      final games = (await tester.runAsync(Game.load))!;
      final game = games.singleWhere((entry) => entry.id == 'th06');
      tester.view.devicePixelRatio = 1;
      tester.view.physicalSize = const Size(1440, 734);
      addTearDown(tester.view.resetDevicePixelRatio);
      addTearDown(tester.view.resetPhysicalSize);
      await tester.pumpWidget(
        RepaintBoundary(
          key: boundary,
          child: launcher.fixture(accountApi: _ProfileFixture(), games: [game]),
        ),
      );
      await tester.pumpAndSettle();
      final mastheadBounds = tester.getRect(find.byType(DesktopMasthead));
      if (capture != null) {
        await tester.runAsync(
          () => precacheImage(
            AssetImage('assets/book-covers/${game.artwork}'),
            tester.element(find.byType(Scaffold).first),
          ),
        );
        await tester.pumpAndSettle();
      }
      await tester.tap(find.byTooltip('登录'));
      await tester.pumpAndSettle();
      await _signIn(tester);
      await tester.tap(find.byTooltip('账户'));
      await tester.pumpAndSettle();
      await _tapText(tester, '个人主页');
      expect(find.byType(ProfilePage), findsOneWidget);
      expect(tester.getRect(find.byType(DesktopMasthead)), mastheadBounds);
      expect(
        tester.getRect(find.byKey(const ValueKey('profile cover'))).top,
        mastheadBounds.bottom,
      );
      expect(
        tester.getRect(find.byKey(const ValueKey('profile cover'))).bottom,
        204,
      );
      expect(
        tester.getTopLeft(find.byType(BackButton)).dy,
        greaterThanOrEqualTo(mastheadBounds.bottom),
      );
      expect(find.text('暂无花映塚对局记录'), findsOneWidget);
      await _tapText(tester, '全部作品');
      expect(find.text('单机生涯'), findsOneWidget);
      final gameDataState = tester.state(find.byType(ProfileGameData));
      tester.view.physicalSize = const Size(320, 640);
      await tester.pumpAndSettle();
      expect(find.byType(DesktopMasthead).hitTestable(), findsNothing);
      expect(tester.state(find.byType(ProfileGameData)), same(gameDataState));
      expect(find.text('单机生涯'), findsOneWidget);
      tester.view.physicalSize = const Size(1440, 734);
      await tester.pumpAndSettle();
      expect(tester.getRect(find.byType(DesktopMasthead)), mastheadBounds);
      expect(tester.state(find.byType(ProfileGameData)), same(gameDataState));
      expect(find.text('单机生涯'), findsOneWidget);
      final before = tester.getTopLeft(find.text('单机生涯')).dy;
      final work = find.widgetWithText(TextButton, game.title).first;
      await tester.ensureVisible(work);
      await tester.tap(work);
      await tester.pumpAndSettle();
      expect(find.text('灵梦'), findsOneWidget);
      final score = find.byWidgetPredicate(
        (widget) =>
            widget is StatisticsHint && widget.message.endsWith('1,111'),
      );
      expect(score, findsOneWidget);
      expect(
        tester.widget<StatisticsHint>(score).semanticLabel,
        contains('灵梦 A'),
      );
      final scoreText = tester.widget<StatisticsHint>(score).message;
      await tester.tap(score);
      await tester.pump();
      await tester.pump(const Duration(milliseconds: 200));
      expect(find.text(scoreText), findsOneWidget);
      if (capture != null) {
        final output = Directory(capture)..createSync(recursive: true);
        await save(tester, boundary, output, 'desktop-profile-work-score');
      }
      Tooltip.dismissAllToolTips();
      await tester.pumpAndSettle();
      expect(find.byType(ProfilePage), findsOneWidget);
      expect(find.text('游戏数据'), findsOneWidget);
      await tester.tap(find.byTooltip('返回全部作品'));
      await tester.pumpAndSettle();
      expect(tester.getTopLeft(find.text('单机生涯')).dy, closeTo(before, 1));
      await _tapText(tester, '花映塚');
      expect(find.text('暂无花映塚对局记录'), findsOneWidget);
      await tester.tap(find.byType(BackButton));
      await tester.pumpAndSettle();
      expect(find.byType(ProfilePage), findsNothing);
      expect(find.byType(SearchBar), findsOneWidget);
      expect(tester.takeException(), isNull);
    },
    variant: TargetPlatformVariant.only(TargetPlatform.windows),
  );
}
