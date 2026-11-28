import 'dart:io';
import 'dart:convert';

import 'package:flutter/foundation.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:go_router/go_router.dart';
import 'package:material_ui/material_ui.dart';
import 'package:shared_preferences/shared_preferences.dart';
import 'package:shared_preferences_platform_interface/in_memory_shared_preferences_async.dart';
import 'package:shared_preferences_platform_interface/shared_preferences_async_platform_interface.dart';
import 'package:touhouonline/account_api.dart';
import 'package:touhouonline/catalog.dart';
import 'package:touhouonline/desktop_masthead.dart';
import 'package:touhouonline/game_resources.dart';
import 'package:touhouonline/game_user_files.dart';
import 'package:touhouonline/game_launcher.dart';
import 'package:touhouonline/game_preferences.dart';
import 'package:touhouonline/library.dart';
import 'package:touhouonline/main.dart';
import 'package:touhouonline/profile_data.dart';
import 'package:touhouonline/profile_identity.dart';
import 'package:touhouonline/th09_ratings.dart';
import 'package:touhouonline/th09_rating_chart.dart';
import 'package:touhouonline/launcher_select.dart';

import 'capture_native_baseline_test.dart' show loadFonts, save;

class _Network extends HttpOverrides {}

class _PublicProfileCapture extends AccountApi {
  _PublicProfileCapture(
    this.view,
    this.summary,
    this.people,
    this.activity,
    this.history,
    this.duelId,
    this.replays,
    this.downloaded,
  );

  final PublicProfile view;
  final ProfileSocial summary;
  final ProfileSocial people;
  final ProfileStatistics activity;
  final DuelHistory history;
  final String duelId;
  final List<Th09DuelReplay> replays;
  final ({Uint8List bytes, String filename}) downloaded;

  @override
  Future<List<Th09DuelReplay>> th09DuelReplays(String duel) async {
    expect(duel, duelId);
    return replays;
  }

  @override
  Future<PublicProfile> profile(String username, {bool fresh = false}) async =>
      view;

  @override
  Future<ProfileSocial> social(
    String username, {
    required bool signedIn,
    ProfileList? list,
  }) async => list == null ? summary : people;

  @override
  Future<ProfileStatistics> profileStatistics(
    String username, {
    required bool owner,
  }) async => activity;

  @override
  Future<DuelHistory> duels(
    String username, {
    required bool owner,
    String? cursor,
  }) async => history;

  @override
  Future<Uri?> avatar(String username) async =>
      username == view.username ? view.avatar : null;
}

Future<void> _preloadMedia(HttpClient http, Uri url) async {
  final response = await (await http.getUrl(url))
      .close()
      .timeout(const Duration(seconds: 10));
  expect(response.statusCode, HttpStatus.ok);
  final bytes = await consolidateHttpClientResponseBytes(response);
  final image = await decodeImageFromList(bytes);
  PaintingBinding.instance.imageCache.putIfAbsent(
    NetworkImage(url.toString()),
    () => OneFrameImageStreamCompleter(
      SynchronousFuture(ImageInfo(image: image)),
    ),
  );
}

Future<_PublicProfileCapture> _read() =>
    HttpOverrides.runWithHttpOverrides(() async {
      final api = AccountApi();
      final http = HttpClient()
        ..connectionTimeout = const Duration(seconds: 10);
      try {
        const username = 'ritosa';
        final profile = await api.profile(username);
        final summary = await api.social(username, signedIn: false);
        final followers = await api.social(
          username,
          signedIn: false,
          list: ProfileList.followers,
        );
        final stats = await api.profileStatistics(username, owner: false);
        final duels = await api.duels(username, owner: false);
        expect(profile.username, username);
        expect(profile.avatar, isNotNull);
        expect(profile.cover, isNotNull);
        expect(await api.avatar('Ritosa'), profile.avatar);
        expect(followers.visible, isTrue);
        expect(stats.career, isTrue);
        expect(stats.games, isNotEmpty);
        expect(duels.games, greaterThan(0));
        await _preloadMedia(http, profile.avatar!);
        await _preloadMedia(http, profile.cover!);
        expect(duels.ratingHistory, isNotEmpty);
        expect(duels.rankHistory, isNotEmpty);
        expect(
          duels.characterRatings.any((entry) => entry.history.isNotEmpty),
          isTrue,
        );
        final duel = duels.rounds.firstWhere((round) => round.replayCount > 0);
        final replays = await api.th09DuelReplays(duel.id);
        expect(replays, isNotEmpty);
        final replay = await api.downloadTh09Replay(duel.id, replays.first);
        expect(replay.bytes.length, replays.first.bytes);
        return _PublicProfileCapture(
          profile,
          summary,
          followers,
          stats,
          duels,
          duel.id,
          replays,
          replay,
        );
      } finally {
        api.close();
        http.close(force: true);
      }
    }, _Network());

void main() {
  final output = Directory(Platform.environment['TOUHOU_CAPTURE_DIR']!)
    ..createSync(recursive: true);
  final sdkFonts = Directory(Platform.environment['TOUHOU_CAPTURE_FONTS']!);

  for (final (name, size) in [
    ('desktop', const Size(1440, 734)),
    ('mobile', const Size(390, 844)),
    ('mobile-narrow', const Size(320, 640)),
  ]) {
    testWidgets('Render live public profile $name', (tester) async {
      tester.view.devicePixelRatio = 1;
      tester.view.physicalSize = size;
      addTearDown(tester.view.resetPhysicalSize);
      addTearDown(tester.view.resetDevicePixelRatio);
      SharedPreferencesAsyncPlatform.instance =
          InMemorySharedPreferencesAsync.empty();
      final storage = SharedPreferencesAsync();
      final games = (await tester.runAsync(Game.load))!;
      await tester.runAsync(() => loadFonts(sdkFonts));
      final profile = (await tester.runAsync(_read))!;
      final favorites = GameFavorites(storage, {});
      final boundary = GlobalKey();
      await tester.pumpWidget(
        RepaintBoundary(
          key: boundary,
          child: LauncherApp(
            book: BookState(games, favorites),
            lobby: BookState(games, favorites),
            preferences: LauncherPreferences(storage, ThemeMode.dark, true),
            launcher: const NativeGamePlaceholder(),
            gamePreferences: GamePreferenceStore(storage),
            accountApi: profile,
          ),
        ),
      );
      await tester.pumpAndSettle();
      await tester.runAsync(
        () => precacheImage(
          const AssetImage('assets/launcher-wordmark-dark.png'),
          tester.element(find.byType(LibraryPage)),
        ),
      );
      await tester.pumpAndSettle();
      final address = GoRouter.of(tester.element(find.byType(LibraryPage)));
      address.push(profilePath(profile.view.username));
      await tester.pumpAndSettle();
      expect(find.text(profile.view.nickname), findsOneWidget);
      expect(find.text(profile.view.bio), findsOneWidget);
      expect(find.text('对局记录'), findsOneWidget);
      expect(tester.takeException(), isNull);
      List<double>? rect(Finder target) {
        if (target.evaluate().isEmpty) return null;
        final bounds = tester.getRect(target);
        return [bounds.left, bounds.top, bounds.width, bounds.height];
      }

      File('${output.path}/$name-profile-geometry.json').writeAsStringSync(
        const JsonEncoder.withIndent('  ').convert({
          'viewport': [size.width, size.height],
          'profile': profile.view.username,
          'masthead': rect(find.byType(DesktopMasthead)),
          'cover': rect(find.byKey(const ValueKey('profile cover'))),
          'avatar': rect(find.byKey(const ValueKey('profile avatar'))),
          'nickname': rect(find.text(profile.view.nickname)),
          'gameData': rect(find.text('游戏数据')),
          'viewSwitch': rect(find.byType(LauncherSegments<ProfileGames>)),
          'gameDataFontSize': tester
              .widget<Text>(find.text('游戏数据'))
              .style
              ?.fontSize,
        }),
      );
      await save(tester, boundary, output, '$name-profile-public');

      final chartTitle = find.bySemanticsLabel('WHR 走势');
      await tester.ensureVisible(find.byType(Th09RatingChart));
      await tester.pumpAndSettle();
      expect(chartTitle, findsOneWidget);
      await save(tester, boundary, output, '$name-profile-whr');
      await tester.tap(
        find.descendant(
          of: find.byType(LauncherSegments<bool>),
          matching: find.text('名次'),
        ),
      );
      await tester.pumpAndSettle();
      expect(find.bySemanticsLabel('名次走势'), findsOneWidget);
      await tester.tap(find.byTooltip('上一个数据点'));
      await tester.pumpAndSettle();
      expect(tester.takeException(), isNull);
      await tester.tap(
        find.descendant(
          of: find.byType(LauncherSegments<bool>),
          matching: find.text('WHR'),
        ),
      );
      await tester.pumpAndSettle();

      final picker = find.text('全部机体');
      await tester.ensureVisible(picker);
      await tester.pumpAndSettle();
      await tester.tap(picker);
      await tester.pumpAndSettle();
      final character = profile.history.characterRatings.firstWhere(
        (entry) => entry.history.isNotEmpty,
      );
      await tester.tap(find.text(th09CharacterNames[character.character]).last);
      await tester.pumpAndSettle();
      if (size.width > 780) {
        await tester.ensureVisible(find.text('机体分对照'));
        await tester.pumpAndSettle();
      }
      expect(find.text('机体分对照'), findsOneWidget);
      expect(tester.takeException(), isNull);
      await save(tester, boundary, output, '$name-profile-whr-comparison');
      await tester.tap(find.byTooltip('返回 WHR 走势'));
      await tester.pumpAndSettle();

      final replayGroup = find.descendant(
        of: find.byKey(ValueKey<(String, String?)>((profile.duelId, null))),
        matching: find.byIcon(Icons.movie_outlined),
      );
      await tester.ensureVisible(replayGroup);
      await tester.pumpAndSettle();
      await tester.tap(replayGroup);
      await tester.pumpAndSettle();
      final firstReplay = find.byTooltip('下载第 1 局');
      await tester.ensureVisible(firstReplay);
      await tester.pumpAndSettle();
      expect(firstReplay, findsOneWidget);
      expect(tester.takeException(), isNull);
      await save(tester, boundary, output, '$name-profile-duel-replays');
      await tester.ensureVisible(replayGroup);
      await tester.pumpAndSettle();
      await tester.tap(replayGroup);
      await tester.pumpAndSettle();

      final choices = find.text('全部作品');
      await tester.ensureVisible(choices);
      await tester.pumpAndSettle();
      await tester.tap(choices);
      await tester.pumpAndSettle();
      expect(find.text('单机生涯'), findsOneWidget);
      expect(find.text('累计游玩'), findsOneWidget);
      final matrix = find.byKey(const PageStorageKey('profile-career-matrix'));
      final scroll = find.descendant(
        of: matrix,
        matching: find.byType(Scrollable),
      );
      expect(tester.state<ScrollableState>(scroll).position.pixels, 0);
      expect(tester.takeException(), isNull);
      await save(tester, boundary, output, '$name-profile-all-games');

      final choice = find.text('花映塚');
      final position = Scrollable.of(tester.element(choice)).position;
      expect(position.maxScrollExtent, greaterThan(0));
      position.jumpTo(position.maxScrollExtent.clamp(0.0, 136.0));
      await tester.pumpAndSettle();
      final before = position.pixels;
      await tester.tap(choice);
      await tester.pumpAndSettle();
      expect(position.pixels, closeTo(before, 1));

      final followers = find.text('粉丝');
      await tester.ensureVisible(followers);
      await tester.pumpAndSettle();
      await tester.tap(followers);
      await tester.pumpAndSettle();
      expect(find.text('关系列表'), findsOneWidget);
      expect(tester.takeException(), isNull);
      await save(tester, boundary, output, '$name-profile-social-followers');

      if (name == 'desktop') {
        await tester.runAsync(() async {
          final fixture = await Directory(output.path)
              .createTemp('verified-replay-');
          final archive = profile.downloaded;
          File? staged;
          try {
            final files = GameUserFiles(
              appDirectory: () async => fixture,
              exporter: (name, file) async {
                expect(name, archive.filename);
                expect(await file.readAsBytes(), orderedEquals(archive.bytes));
                return true;
              },
            );
            final launcher = GameLaunchService(
              const NativeGamePlaceholder(),
              GamePreferenceStore(storage),
              GameResources(appDirectory: () async => fixture),
              files,
            );
            final game = games.firstWhere((game) => game.id == 'th09');
            final result = await launcher.withArchivedReplay(
              game,
              archive.filename,
              archive.bytes,
              (file, export) async {
                staged = file;
                expect(await export(), isTrue);
                return launcher.start(
                  game,
                  multiplayer: true,
                  isActive: () => true,
                  replayFile: file,
                );
              },
            );
            expect(result, GameLaunchStatus.nativeRuntimeUnavailable);
            expect(await staged!.exists(), isFalse);
          } finally {
            await fixture.delete(recursive: true);
          }
        });
      }
    }, variant: TargetPlatformVariant.only(TargetPlatform.windows));
  }
}
