import 'dart:convert';
import 'dart:io';

import 'package:flutter/services.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:material_ui/material_ui.dart';
import 'package:shared_preferences/shared_preferences.dart';
import 'package:shared_preferences_platform_interface/in_memory_shared_preferences_async.dart';
import 'package:shared_preferences_platform_interface/shared_preferences_async_platform_interface.dart';
import 'package:touhouonline/catalog.dart';
import 'package:touhouonline/profile_data.dart';
import 'package:touhouonline/profile_page.dart';
import 'package:touhouonline/th09_rating_chart.dart';
import 'package:touhouonline/th09_ratings.dart';
import 'package:touhouonline/launcher_select.dart';

import '../tools/capture_native_baseline_test.dart' show loadFonts, save;
import 'widget_test.dart' as launcher;

class _DuelFixture extends launcher.FixtureAccountApi {
  _DuelFixture(this.history, this.replays);

  final DuelHistory history;
  final List<Th09DuelReplay> replays;
  bool failed = true;
  bool failReplays = true;

  @override
  Future<PublicProfile> profile(String username, {bool fresh = false}) async =>
      PublicProfile(
        username: username,
        nickname: username == 'peer' ? 'Peer' : '测试玩家',
      );

  @override
  Future<ProfileSocial> social(
    String username, {
    required bool signedIn,
    ProfileList? list,
  }) async => const ProfileSocial(visible: false);

  @override
  Future<ProfileStatistics> profileStatistics(
    String username, {
    required bool owner,
  }) async => const ProfileStatistics(career: false, records: false);

  @override
  Future<DuelHistory> duels(
    String username, {
    required bool owner,
    String? cursor,
  }) async {
    expect(username, 'fixture');
    expect(owner, isFalse);
    if (failed) throw StateError('Fixture history failure');
    return history;
  }

  @override
  Future<List<Th09DuelReplay>> th09DuelReplays(String duel) async {
    expect(duel, history.rounds.first.id);
    if (failReplays) throw StateError('Fixture replay failure');
    return replays;
  }
}

void main() {
  final capture = Platform.environment['TOUHOU_PROFILE_CAPTURE'];
  late DuelHistory history;
  late List<Th09DuelReplay> replays;
  setUpAll(() async {
    final fixture = jsonDecode(
      File('tools/profile-th09-fixture.json').readAsStringSync(),
    ) as Map<String, dynamic>;
    history = DuelHistory.fromJson(fixture['history'] as Map<String, dynamic>);
    replays = th09DuelReplaysFromJson(fixture['replays']['files']);
    if (capture != null) {
      await loadFonts(Directory(Platform.environment['TOUHOU_CAPTURE_FONTS']!));
    }
  });

  for (final scenario in [
    (
      device: 'desktop',
      size: const Size(1440, 734),
      language: LauncherLanguage.simplifiedChinese,
      title: '游戏数据',
      profile: '个人主页',
      tab: '全部作品',
      otherTab: '花映塚',
      retry: '无法读取对局，重试',
      whr: 'WHR 走势',
      rank: '名次',
      rankTitle: '名次走势',
      previous: '上一个数据点',
      compare: '机体分对照',
      back: '返回 WHR 走势',
      character: '灵梦',
      picker: '全部机体',
      result: '胜利',
      loss: '失败',
      draw: '平局',
      showMore: '更多对局',
      showLess: '收起对局记录',
      replayList: '1 局录像',
      replayRetry: '重新读取录像',
      characters: '灵梦 | 铃仙',
      download: '下载第 1 局',
      empty: '暂无公开的游戏数据',
      screenshot: 'rank',
    ),
    (
      device: 'mobile-narrow',
      size: const Size(320, 640),
      language: LauncherLanguage.english,
      title: 'Game data',
      profile: 'Player profile',
      tab: 'All games',
      otherTab: 'Flower View',
      retry: 'Could not load matches. Retry',
      whr: 'WHR history',
      rank: 'Rank',
      rankTitle: 'Rank history',
      previous: 'Previous data point',
      compare: 'Character rating comparison',
      back: 'Back to WHR history',
      character: 'Reimu',
      picker: 'All characters',
      result: 'Win',
      loss: 'Loss',
      draw: 'Draw',
      showMore: 'More matches',
      showLess: 'Show fewer matches',
      replayList: '1 replay',
      replayRetry: 'Reload replays',
      characters: 'Reimu | Reisen',
      download: 'Download round 1',
      empty: 'No game data yet',
      screenshot: 'comparison',
    ),
    (
      device: 'mobile',
      size: const Size(390, 844),
      language: LauncherLanguage.japanese,
      title: 'ゲームデータ',
      profile: 'プロフィール',
      tab: '全作品',
      otherTab: '花映塚',
      retry: '対戦履歴を読み込めませんでした。再試行',
      whr: 'WHR 推移',
      rank: '順位',
      rankTitle: '順位推移',
      previous: '前のデータ点',
      compare: '機体レート比較',
      back: 'WHR 推移に戻る',
      character: '霊夢',
      picker: '全機体',
      result: '勝',
      loss: '負',
      draw: '分',
      showMore: '対戦履歴をさらに表示',
      showLess: '対戦履歴を折りたたむ',
      replayList: 'リプレイ 1 件',
      replayRetry: 'リプレイを再読み込み',
      characters: '霊夢 | 鈴仙',
      download: '第 1 ラウンドをダウンロード',
      empty: 'ゲームデータはまだありません',
      screenshot: 'replays',
    ),
  ]) {
    testWidgets(
      '${scenario.device} profile th09 uses ${scenario.language.tag}',
      (tester) async {
        tester.view.devicePixelRatio = 1;
        tester.view.physicalSize = scenario.size;
        addTearDown(tester.view.resetDevicePixelRatio);
        addTearDown(tester.view.resetPhysicalSize);
        SharedPreferencesAsyncPlatform.instance =
            InMemorySharedPreferencesAsync.empty();
        final api = _DuelFixture(history, replays);
        final boundary = GlobalKey();

        Future<void> reveal(Finder finder) async {
          await tester.ensureVisible(finder);
          await tester.pumpAndSettle();
          expect(finder.hitTestable(), findsOneWidget);
        }

        Future<void> tap(Finder finder) async {
          await reveal(finder);
          await tester.tap(finder);
          await tester.pumpAndSettle();
        }

        Future<void> record(String state) async {
          if (capture != null && state == scenario.screenshot) {
            await save(
              tester,
              boundary,
              Directory(capture),
              '${scenario.device}-profile-th09-$state-${scenario.language.tag}',
            );
          }
        }

        await tester.pumpWidget(
          RepaintBoundary(
            key: boundary,
            child: launcher.fixture(
              accountApi: api,
              initialLocation: '/u/fixture',
              games: const [
                launcher.game,
                Game(
                  id: 'th09',
                  title: '東方花映塚',
                  subtitle: 'Phantasmagoria of Flower View',
                  number: '09',
                  category: BookCategory.versus,
                  nameOrder: 1,
                  multiplayer: true,
                ),
              ],
              preferences: LauncherPreferences(
                SharedPreferencesAsync(),
                ThemeMode.dark,
                true,
                language: scenario.language,
              ),
            ),
          ),
        );
        await tester.pumpAndSettle();
        expect(find.bySemanticsLabel(scenario.profile), findsOneWidget);
        expect(find.text(scenario.title), findsOneWidget);
        api.failed = false;
        await tap(find.text(scenario.retry));
        expect(find.bySemanticsLabel(scenario.whr), findsOneWidget);
        await tap(
          find.descendant(
            of: find.byType(LauncherSegments<bool>),
            matching: find.text(scenario.rank),
          ),
        );
        await tester.ensureVisible(find.byType(Th09RatingChart));
        await tester.pumpAndSettle();
        expect(
          find.bySemanticsLabel(scenario.rankTitle).hitTestable(),
          findsOneWidget,
        );
        await record('rank');
        await tap(find.byTooltip(scenario.previous));
        expect(find.text('${scenario.rank} 72'), findsOneWidget);
        await tap(find.text(scenario.picker));
        await tap(find.widgetWithText(MenuItemButton, scenario.character));
        await reveal(find.text(scenario.compare));
        expect(find.text('${scenario.character} 1,410'), findsWidgets);
        await record('comparison');
        if (scenario.device == 'mobile-narrow') {
          await tester.binding.handlePopRoute();
          await tester.pumpAndSettle();
        } else {
          await tap(find.byTooltip(scenario.back));
        }
        expect(find.byType(ProfilePage), findsOneWidget);
        expect(find.text(scenario.compare), findsNothing);
        final win = find.text(scenario.result);
        await reveal(win);
        expect(find.text(scenario.loss), findsOneWidget);
        expect(find.text(scenario.draw), findsNothing);
        await tap(find.text(scenario.showMore));
        expect(find.text(scenario.draw), findsOneWidget);
        await tap(find.text(scenario.showLess));
        expect(find.text(scenario.draw), findsNothing);
        final avatar = find.byKey(const ValueKey('th09-card-peer'));
        await tap(avatar);
        final card = find.byKey(const ValueKey('th09-player-card'));
        expect(card, findsOneWidget);
        if (scenario.device == 'desktop') {
          await tester.sendKeyEvent(LogicalKeyboardKey.escape);
        } else {
          await tester.binding.handlePopRoute();
        }
        await tester.pumpAndSettle();
        expect(card, findsNothing);
        await tap(find.text(scenario.replayList));
        api.failReplays = false;
        await tap(find.text(scenario.replayRetry));
        final download = find.byTooltip(scenario.download);
        await reveal(download);
        expect(find.text(scenario.characters), findsOneWidget);
        await record('replays');
        await tap(find.text(scenario.tab));
        expect(find.text(scenario.empty), findsOneWidget);
        await tap(find.text(scenario.otherTab));
        expect(find.byType(Th09RatingChart), findsOneWidget);
        expect(tester.takeException(), isNull);
      },
    );
  }
}
