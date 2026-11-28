import 'dart:convert';
import 'dart:io';

import 'package:flutter/rendering.dart' show RenderParagraph;
import 'package:flutter/services.dart' show LogicalKeyboardKey;
import 'package:flutter_test/flutter_test.dart';
import 'package:material_ui/material_ui.dart';
import 'package:lucide_icons_flutter/lucide_icons.dart';
import 'package:shared_preferences/shared_preferences.dart';
import 'package:shared_preferences_platform_interface/in_memory_shared_preferences_async.dart';
import 'package:shared_preferences_platform_interface/shared_preferences_async_platform_interface.dart';
import 'package:touhouonline/account_api.dart';
import 'package:touhouonline/catalog.dart';
import 'package:touhouonline/cloud_saves.dart';
import 'package:touhouonline/game_detail.dart';
import 'package:touhouonline/game_clear_matrix.dart';
import 'package:touhouonline/game_friends.dart';
import 'package:touhouonline/game_spell_archive.dart';
import 'package:touhouonline/l10n/strings.dart';
import 'package:touhouonline/library.dart';
import 'package:touhouonline/profile_data.dart';
import 'package:touhouonline/save_progress.dart';
import 'package:touhouonline/statistics_tile.dart';

import '../tools/capture_native_baseline_test.dart' show loadFonts, save;
import 'widget_test.dart' as launcher;

const _th07 = Game(
  id: 'th07',
  number: '07',
  title: '東方妖々夢',
  subtitle: 'Perfect Cherry Blossom',
  category: BookCategory.windows,
  nameOrder: 0,
  multiplayer: true,
  artwork: 'th07.webp',
);

Finder tooltip(String message) => find.byWidgetPredicate(
  (item) => item is Tooltip && item.message == message,
);

Finder scope(String label) => find.byWidgetPredicate(
  (item) =>
      item is Text &&
      (item.data ?? item.textSpan?.toPlainText() ?? '')
          .replaceAll('\u00a0', ' ')
          .contains(label),
);

Future<void> _captureDetailEntry(
  WidgetTester tester,
  GlobalKey boundary,
  Directory output,
  String name,
) async {
  final detail = find.byType(GameDetail);
  final scroll = tester
      .state<ScrollableState>(
        find.descendant(of: detail, matching: find.byType(Scrollable)).first,
      )
      .position;
  scroll.jumpTo(0);
  await tester.pumpAndSettle();
  final heading = find.descendant(
    of: detail,
    matching: find.byType(GameHeading),
  );
  final game = tester.widget<GameDetail>(detail).game;
  List<double> box(Finder item) {
    final rect = tester.getRect(item);
    return [rect.left, rect.top, rect.width, rect.height];
  }

  final measurement = {
    'viewport': [
      tester.view.physicalSize.width,
      tester.view.physicalSize.height,
    ],
    'game': game.id,
    'heading': box(heading),
    'cover': box(
      find.descendant(of: heading, matching: find.byType(GameArtwork)),
    ),
    'title': box(find.descendant(of: heading, matching: find.text(game.title))),
    'subtitle': box(
      find.descendant(of: heading, matching: find.text(game.subtitle)),
    ),
    'progressTitle': box(
      find.descendant(of: detail, matching: find.text('通关进度')),
    ),
    'titleFontSize': tester
        .widget<Text>(
          find.descendant(of: heading, matching: find.text(game.title)),
        )
        .style
        ?.fontSize,
  };
  await save(tester, boundary, output, name);
  await tester.runAsync(() async {
    await File('${output.path}/$name.json').writeAsString(
      '${const JsonEncoder.withIndent('  ').convert(measurement)}\n',
    );
  });
}

class _CloudApi extends launcher.FixtureAccountApi {
  _CloudApi(this.payload);
  final Map<String, dynamic> payload;
  bool rejected = true;
  int requests = 0;

  @override
  Future<Map<String, CloudGameSave>> cloudGameSaves() async {
    requests++;
    if (!signedIn) throw StateError('Guest used private route');
    if (rejected) {
      rejected = false;
      throw const AccountRequestException(503, 'SERVICE_UNAVAILABLE');
    }
    return CloudSaveDirectory.fromJson(payload).statistics;
  }
}

class _FriendsApi extends _CloudApi {
  _FriendsApi(super.payload, this.samples) {
    rejected = false;
  }

  final Map<String, dynamic> samples;
  final checked = <String>[];
  bool rejectSocial = true;
  bool showSocial = true;

  @override
  Future<ProfileSocial> social(
    String username, {
    required bool signedIn,
    ProfileList? list,
  }) async {
    if (username != 'fixture' || list != ProfileList.friends) {
      return super.social(username, signedIn: signedIn, list: list);
    }
    expectSync(signedIn, isTrue);
    if (rejectSocial) {
      throw const AccountRequestException(503, 'SERVICE_UNAVAILABLE');
    }
    return ProfileSocial.fromJson({
      'visible': showSocial,
      'friends': [
        for (final entry in samples.entries)
          {'username': entry.key, 'nickname': entry.value['nickname']},
      ],
    }, list);
  }

  @override
  Future<ProfileStatistics> profileStatistics(
    String username, {
    required bool owner,
  }) async {
    expectSync(owner, isFalse);
    checked.add(username);
    final sample = samples[username] as Map<String, dynamic>;
    return ProfileStatistics.fromJson({
      'career': sample['career'] ?? true,
      'records': sample['records'] ?? true,
      'games': [
        {
          'statistics': {
            'game': sample['game'] ?? 'th07',
            'playCount': sample['playCount'],
            'playtimeSeconds': sample['playtimeSeconds'],
            'progress': [
              {
                'character': 0,
                'difficulty': 'normal',
                'cleared': sample['cleared'],
                'bestScore': sample['bestScore'],
              },
            ],
          },
        },
      ],
    });
  }
}

Future<void> _tap(WidgetTester tester, Finder control) async {
  await tester.ensureVisible(control);
  await tester.pumpAndSettle();
  expect(control.hitTestable(), findsOneWidget);
  await tester.tap(control);
  await tester.pumpAndSettle();
}

void main() {
  final evidence = Platform.environment['TOUHOU_PROGRESS_CAPTURE'];
  final friendEvidence = Platform.environment['TOUHOU_FRIENDS_CAPTURE'];
  final realInput = Platform.environment['TOUHOU_PROGRESS_INPUT'];
  late final Map<String, dynamic> payload;
  late final Map<String, dynamic> friends;
  late final List<Game> catalog;
  late final List<Game> realCatalog;
  late final Map<String, dynamic> realStatistics;
  setUpAll(() async {
    payload = jsonDecode(
      File('tools/game-progress-fixture.json').readAsStringSync(),
    ) as Map<String, dynamic>;
    friends = jsonDecode(
      File('tools/game-friends-fixture.json').readAsStringSync(),
    ) as Map<String, dynamic>;
    realCatalog = await Game.load();
    catalog = friendEvidence == null ? const [_th07] : realCatalog;
    if (realInput != null) {
      realStatistics = jsonDecode(
        File(realInput).readAsStringSync(),
      ) as Map<String, dynamic>;
    }
    if (Platform.isWindows) {
      final fontPath = Platform.environment['TOUHOU_CAPTURE_FONTS'];
      await loadFonts(fontPath == null ? null : Directory(fontPath));
    }
  });
  setUp(
    () => SharedPreferencesAsyncPlatform.instance =
        InMemorySharedPreferencesAsync.empty(),
  );

  for (final (device, size, theme) in [
    ('mobile-narrow', const Size(320, 640), ThemeMode.dark),
    ('desktop', const Size(1440, 734), ThemeMode.dark),
    if (friendEvidence != null) ...[
      ('mobile', const Size(390, 844), ThemeMode.dark),
      ('desktop-light', const Size(1440, 734), ThemeMode.light),
      ('mobile-light', const Size(390, 844), ThemeMode.light),
      ('mobile-narrow-light', const Size(320, 640), ThemeMode.light),
    ],
  ]) {
    testWidgets('$device lists up to three friends with public play activity', (
      tester,
    ) async {
      tester.view.devicePixelRatio = 1;
      tester.view.physicalSize = size;
      addTearDown(tester.view.resetPhysicalSize);
      addTearDown(tester.view.resetDevicePixelRatio);
      final api = _FriendsApi(payload, friends);
      final boundary = GlobalKey();
      final app = launcher.fixture(accountApi: api, games: catalog);
      await app.preferences.setTheme(theme);
      await tester.pumpWidget(RepaintBoundary(key: boundary, child: app));
      await tester.pumpAndSettle();
      if (friendEvidence != null) {
        final context = tester.element(find.byType(LibraryPage));
        await tester.runAsync(() async {
          await precacheImage(
            const AssetImage('assets/launcher-wordmark-dark.png'),
            context,
          );
          for (final game in catalog) {
            if (game.artwork case final String art) {
              await precacheImage(
                AssetImage('assets/book-covers/$art'),
                context,
              );
            }
          }
        });
        await tester.pumpAndSettle();
      }
      await _tap(
        tester,
        find.byWidgetPredicate(
          (widget) =>
              widget is GameArtwork &&
              widget.game.id == 'th07' &&
              widget.size == null,
        ),
      );
      final panel = find.byType(GameFriends);
      expect(panel, findsNothing);
      if (friendEvidence != null) {
        await _captureDetailEntry(
          tester,
          boundary,
          Directory(friendEvidence),
          '$device-detail-entry-guest',
        );
      }
      await launcher.signInFixture(tester);

      Finder inPanel(String label) =>
          find.descendant(of: panel, matching: find.text(label));
      expect(inPanel('无法加载列表，请重试。'), findsOneWidget);
      expect(api.checked, isEmpty);
      api.rejectSocial = false;
      api.showSocial = false;
      await _tap(tester, inPanel('重试'));
      expect(inPanel('暂无可显示的好友游玩记录。'), findsOneWidget);
      expect(api.checked, isEmpty);

      api.showSocial = true;
      tester.widget<GameFriends>(panel).session.socialRevision.value++;
      await tester.pumpAndSettle();
      expect(inPanel('曾经游玩过的好友'), findsOneWidget);
      expect(inPanel('通关玩家'), findsOneWidget);
      expect(inPanel('有分数记录的好友'), findsOneWidget);
      expect(inPanel('暂无记录的玩家'), findsNothing);
      expect(inPanel('玩其他作品的好友'), findsNothing);
      expect(inPanel('第五位玩家'), findsNothing);
      expect(api.checked, [
        'empty',
        'other',
        'played',
        'cleared',
        'scored',
        'recent',
      ]);
      final progress = find.text('通关进度');
      final heading = inPanel('好友');
      if (size.width > 780) {
        expect(tester.getTopLeft(heading).dx, greaterThan(1150));
        expect(
          tester.getTopLeft(heading).dy,
          closeTo(tester.getTopLeft(progress).dy, 1),
        );
      } else {
        expect(tester.getTopLeft(heading).dx, tester.getTopLeft(progress).dx);
        expect(
          tester.getTopLeft(heading).dy,
          greaterThan(tester.getBottomLeft(progress).dy),
        );
      }
      tester
          .state<ScrollableState>(
            find
                .descendant(
                  of: find.byType(GameDetail),
                  matching: find.byType(Scrollable),
                )
                .first,
          )
          .position
          .jumpTo(0);
      await tester.pumpAndSettle();
      expect(
        tester.getTopLeft(find.text('最后运行日期')).dy,
        tester.getTopLeft(find.text('游戏时间')).dy,
        reason: 'Normal-size runtime metrics share a row even at 320 px',
      );
      if (friendEvidence != null) {
        await _captureDetailEntry(
          tester,
          boundary,
          Directory(friendEvidence),
          '$device-detail-entry',
        );
        await tester.ensureVisible(inPanel('查看好友'));
        await tester.pumpAndSettle();
        await save(
          tester,
          boundary,
          Directory(friendEvidence),
          '$device-game-friend-activity',
        );
      }
      final openList = inPanel('查看好友');
      await tester.ensureVisible(openList);
      await tester.pumpAndSettle();
      final listPosition = tester.getTopLeft(openList);
      final link = await launcher.openAccountAndReturn(tester, openList);
      expect(link.path, '/u/fixture');
      expect(link.queryParameters['social'], 'friends');
      await tester.pumpAndSettle();
      expect(panel, findsOneWidget);
      expect(openList.hitTestable(), findsOneWidget);
      expect(tester.getTopLeft(openList), listPosition);
      final playerLink = await launcher.openAccountAndReturn(
        tester,
        inPanel('曾经游玩过的好友'),
      );
      expect(playerLink.path, '/u/played');
      await tester.pumpAndSettle();
      expect(panel, findsOneWidget);
      expect(api.checked, hasLength(6));
      expect(tester.takeException(), isNull);
    }, variant: TargetPlatformVariant.only(TargetPlatform.windows));
  }

  if (Platform.isWindows) {
    for (final width in [320.0, 390.0, 1440.0]) {
      for (final theme in [ThemeMode.dark, ThemeMode.light]) {
        testWidgets('Catalog detail headers align | $width ${theme.name}', (
          tester,
        ) async {
          tester.view.devicePixelRatio = 1;
          tester.view.physicalSize = Size(width, 844);
          addTearDown(tester.view.resetDevicePixelRatio);
          addTearDown(tester.view.resetPhysicalSize);
          final boundary = GlobalKey();
          final app = launcher.fixture(
            games: realCatalog,
            accountApi: launcher.FixtureAccountApi(),
          );
          await app.preferences.setTheme(theme);
          await tester.pumpWidget(RepaintBoundary(key: boundary, child: app));
          await tester.pumpAndSettle();
          for (final game in realCatalog) {
            final search = find.descendant(
              of: find.byType(LibrarySearch).hitTestable().first,
              matching: find.byType(EditableText),
            );
            await tester.enterText(search, game.id);
            await tester.pumpAndSettle();
            await _tap(
              tester,
              find.byWidgetPredicate(
                (widget) =>
                    widget is GameArtwork &&
                    widget.game.id == game.id &&
                    widget.size == null,
              ),
            );
            final detail = find.byType(GameDetail);
            final heading = find.descendant(
              of: detail,
              matching: find.byType(GameHeading),
            );
            final cover = find.descendant(
              of: heading,
              matching: find.byType(GameArtwork),
            );
            final progress = find.descendant(
              of: detail,
              matching: find.text('通关进度'),
            );
            expect(tester.getSize(cover).width, tester.getSize(cover).height);
            expect(
              tester.getTopLeft(cover).dx,
              closeTo(tester.getTopLeft(progress).dx, .01),
            );
            expect(
              tester.getRect(heading).right,
              lessThanOrEqualTo(width - 16),
            );
            final play = find.descendant(
              of: detail,
              matching: find.widgetWithText(FilledButton, '开始游戏'),
            );
            final settings = find
                .ancestor(
                  of: find.descendant(
                    of: detail,
                    matching: find.byIcon(LucideIcons.slidersHorizontal),
                  ),
                  matching: find.byWidgetPredicate(
                    (widget) =>
                        widget is IconButton || widget is ButtonStyleButton,
                  ),
                )
                .first;
            expect(tester.getTopLeft(settings).dy, tester.getTopLeft(play).dy);
            final favorite = find.descendant(
              of: detail,
              matching: find.byType(FavoriteButton),
            );
            if (width <= 780) {
              expect(
                tester.getBottomLeft(favorite).dy,
                lessThan(tester.getTopLeft(heading).dy),
              );
            } else {
              expect(
                tester.getTopLeft(favorite).dy,
                tester.getTopLeft(play).dy,
              );
            }
            expect(tester.takeException(), isNull, reason: game.id);
            final metric = tester.widget<Text>(
              find.descendant(of: detail, matching: find.text('最后运行日期')),
            );
            expect(
              metric.style!.color!.computeLuminance(),
              greaterThan(.8),
              reason:
                  'Hero metadata must use its local foreground in both themes',
            );
            if (friendEvidence != null &&
                const ['th06', 'th11', 'th075'].contains(game.id)) {
              await _captureDetailEntry(
                tester,
                boundary,
                Directory(friendEvidence),
                '${width.toInt()}-${theme.name}-${game.id}-detail',
              );
            }
            await tester.binding.handlePopRoute();
            await tester.pumpAndSettle();
            expect(find.byType(LibraryPage).hitTestable(), findsOneWidget);
          }
        }, variant: TargetPlatformVariant.only(TargetPlatform.windows));
      }
    }
  }

  test(
    'zero loadout totals suppress seeded scores only in the matching mode',
    () {
      const row = {
        'character': 0,
        'shot': 0,
        'difficulty': 'normal',
        'cleared': false,
        'bestScore': 10000000,
      };
      final progress = [
        {...row, 'mode': 'legacy'},
        {...row, 'mode': 'pointdevice'},
        {...row, 'mode': 'legacy', 'playCount': 2},
        {...row, 'mode': 'legacy', 'cleared': true},
        {...row, 'mode': 'legacy', 'cleared': null},
      ];
      const loadouts = [
        {'character': 0, 'shot': 0, 'mode': 'legacy', 'playCount': 0},
        {'character': 0, 'shot': 0, 'mode': 'pointdevice', 'playCount': 3},
      ];
      final rows = readSaveProgress(progress, loadouts: loadouts);
      expect(rows.map((row) => row.playCount), [0, null, 2, null, null]);
      expect(rows.map((row) => row.bestScore), [
        null,
        10000000,
        10000000,
        10000000,
        10000000,
      ]);
      final redacted = readSaveProgress(
        progress,
        loadouts: loadouts,
        records: false,
      );
      expect(
        redacted.every((row) => row.bestScore == null && row.playCount == null),
        isTrue,
      );
    },
  );

  for (final (id, file, rows, width, language) in [
    ('th06', 'score.dat', 20, 390.0, LauncherLanguage.japanese),
    ('th07', 'score.dat', 36, 390.0, LauncherLanguage.japanese),
    ('th08', 'score.dat', 60, 320.0, LauncherLanguage.english),
    ('th10', 'scoreth10.dat', 30, 1440.0, LauncherLanguage.english),
    ('th11', 'scoreth11.dat', 30, 320.0, LauncherLanguage.english),
    ('th12', 'scoreth12.dat', 30, 390.0, LauncherLanguage.japanese),
  ]) {
    testWidgets('real save shows the correct progress for $id', (tester) async {
      tester.view.devicePixelRatio = 1;
      tester.view.physicalSize = Size(
        width,
        width == 320
            ? 640
            : width == 390
            ? 844
            : 734,
      );
      addTearDown(tester.view.resetPhysicalSize);
      addTearDown(tester.view.resetDevicePixelRatio);
      final game = realCatalog.singleWhere((game) => game.id == id);
      final strings = lookupLauncherStrings(language.locale);
      final api = _CloudApi({
        'products': [
          {
            'id': id,
            'game': id,
            'multiplayer': false,
            'files': [file],
            'requiredFiles': [file],
            'revision': 1,
          },
        ],
        'saves': [
          {
            'product': id,
            'revision': 1,
            'uploadedAt': '2026-11-24T10:00:00Z',
            'files': [],
            'statistics': realStatistics[id],
          },
        ],
      })..rejected = false;
      final boundary = GlobalKey();
      await tester.pumpWidget(
        RepaintBoundary(
          key: boundary,
          child: launcher.fixture(
            accountApi: api,
            games: [game],
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
      if (game.artwork case final String art when evidence != null) {
        final context = tester.element(find.byType(LibraryPage));
        await tester.runAsync(
          () => precacheImage(AssetImage('assets/book-covers/$art'), context),
        );
        await tester.pumpAndSettle();
      }
      await _tap(tester, find.byType(GameArtwork).last);
      await launcher.signInFixture(tester);
      final matrix = find.byType(GameClearMatrix);
      final actual = tester.widget<GameClearMatrix>(matrix).progress;
      expect(actual, hasLength(rows));
      Finder inMatrix(Finder descendant) =>
          find.descendant(of: matrix, matching: descendant);
      switch (id) {
        case 'th06':
          expect(actual.any((row) => row.bestScore == 262890), isTrue);
        case 'th07':
          expect(
            actual.any(
              (row) => row.bestScore == 436777350 && row.oneCreditClear == true,
            ),
            isTrue,
          );
          expect(find.byType(GameSpellArchive), findsOneWidget);
        case 'th08':
          expect(inMatrix(find.text('Border Team')), findsOneWidget);
          expect(
            inMatrix(tooltip('Lunatic | ${strings.progressFailed}')),
            findsOneWidget,
          );
          expect(
            inMatrix(tooltip('Lunatic | ${strings.progressUntried}')),
            findsNWidgets(11),
          );
        case 'th10':
          final result = inMatrix(
            tooltip(
              'Easy | ${strings.progressOneCredit} | ${strings.progressBestScore} 730,248,680',
            ),
          );
          await _tap(tester, result);
          expect(find.textContaining('730,248,680'), findsOneWidget);
          Tooltip.dismissAllToolTips();
          await tester.pump(const Duration(milliseconds: 200));
          await tester.pumpAndSettle();
        case 'th11' || 'th12':
          expect(
            inMatrix(tooltip('Easy | ${strings.progressUntried}')),
            findsNWidgets(6),
          );
          expect(
            actual.every((row) => row.bestScore == null && row.playCount == 0),
            isTrue,
          );
      }
      final title = find.text(strings.clearProgress);
      await Scrollable.ensureVisible(tester.element(title), alignment: .05);
      await tester.pumpAndSettle();
      expect(tester.getTopRight(matrix).dx, lessThanOrEqualTo(width));
      if (evidence != null) {
        await save(tester, boundary, Directory(evidence), 'real-progress-$id');
      }
      await tester.binding.handlePopRoute();
      await tester.pumpAndSettle();
      expect(find.byType(LibraryPage), findsOneWidget);
      expect(tester.takeException(), isNull);
    }, skip: realInput == null);
  }

  final th08Input = Platform.environment['TOUHOU_TH08_SPELL_STATISTICS'];
  for (final (device, size, language, team) in [
    (
      'mobile-narrow',
      const Size(320, 640),
      LauncherLanguage.english,
      'Scarlet Team',
    ),
    ('mobile', const Size(390, 844), LauncherLanguage.japanese, '紅魔組'),
    (
      'desktop',
      const Size(1440, 734),
      LauncherLanguage.simplifiedChinese,
      '红魔组',
    ),
  ]) {
    testWidgets('$device th08 separates story, practice, and Last Word', (
      tester,
    ) async {
      tester.view.devicePixelRatio = 1;
      tester.view.physicalSize = size;
      addTearDown(tester.view.resetPhysicalSize);
      addTearDown(tester.view.resetDevicePixelRatio);
      final strings = lookupLauncherStrings(language.locale);
      final game = realCatalog.singleWhere((game) => game.id == 'th08');
      Map<String, Object> tally(
        String difficulty,
        String activity,
        int attempts,
        int captures,
      ) => {
        'character': 2,
        'shot': 0,
        'difficulty': difficulty,
        'activity': activity,
        'attempts': attempts,
        'captures': captures,
      };
      final statistics = th08Input == null
          ? {
              'game': 'th08',
              'parserVersion': 1,
              'progress': [
                {
                  'character': 2,
                  'shot': 0,
                  'difficulty': 'normal',
                  'playCount': 5,
                },
                {
                  'character': 3,
                  'shot': 0,
                  'difficulty': 'normal',
                  'playCount': 0,
                },
              ],
              'spellArchive': {
                'scope': 'loadout',
                'cards': [
                  {
                    'number': 4,
                    'name': '灯符「ファイヤフライフェノメノン」',
                    'loadouts': [
                      tally('normal', 'story', 7, 4),
                      tally('normal', 'practice', 11, 6),
                    ],
                  },
                  {
                    'number': 206,
                    'name': '「季節外れのバタフライストーム」',
                    'loadouts': [tally('last-word', 'practice', 19, 1)],
                  },
                ],
              },
            }
          : jsonDecode(File(th08Input).readAsStringSync());
      final api = _CloudApi({
        'products': [
          {
            'id': 'th08',
            'game': 'th08',
            'multiplayer': false,
            'files': ['score.dat'],
            'requiredFiles': ['score.dat'],
            'revision': 1,
          },
        ],
        'saves': [
          {
            'product': 'th08',
            'revision': 1,
            'uploadedAt': '2026-11-24T10:00:00Z',
            'files': [],
            'statistics': statistics,
          },
        ],
      })..rejected = false;
      final boundary = GlobalKey();
      await tester.pumpWidget(
        RepaintBoundary(
          key: boundary,
          child: launcher.fixture(
            accountApi: api,
            games: [game],
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
      await _tap(tester, find.byType(GameArtwork).last);
      await launcher.signInFixture(tester);

      final archive = find.byType(GameSpellArchive);
      Finder inArchive(Finder descendant) =>
          find.descendant(of: archive, matching: descendant);
      final title = inArchive(find.textContaining(strings.spellCards)).first;
      await Scrollable.ensureVisible(tester.element(title), alignment: .08);
      await tester.pumpAndSettle();
      final initial = tester.getTopLeft(title);
      for (final branch in ['4A', '4B', '6A', '6B']) {
        expect(
          inArchive(find.text(strings.spellBranch(branch))),
          findsOneWidget,
        );
      }
      expect(inArchive(scope('$team | Normal')), findsOneWidget);
      final story = inArchive(tooltip('No.04 灯符「ファイヤフライフェノメノン」 | 4/7'));
      expect(story, findsOneWidget);
      final practice = inArchive(find.text(strings.spellPractice));
      expect(practice.hitTestable(), findsOneWidget);
      await tester.tap(practice);
      await tester.pumpAndSettle();
      expect(tester.getTopLeft(title), initial);
      expect(story, findsNothing);
      expect(
        inArchive(tooltip('No.04 灯符「ファイヤフライフェノメノン」 | 6/11')),
        findsOneWidget,
      );
      final output = evidence == null ? null : Directory(evidence);
      if (output != null) {
        await save(tester, boundary, output, '$device-th08-spell-practice');
      }

      await _tap(tester, inArchive(find.text(strings.spellLastWord)).first);
      expect(inArchive(scope(team)), findsOneWidget);
      expect(inArchive(find.text(strings.spellBranch('4A'))), findsNothing);
      final word = inArchive(
        find.byWidgetPredicate(
          (item) =>
              item is Tooltip && (item.message?.startsWith('No.206 ') ?? false),
        ),
      );
      expect(tester.widget<Tooltip>(word).message, endsWith(' | 1/19'));
      final tiles = inArchive(
        find.byWidgetPredicate(
          (item) =>
              item is Tooltip && (item.message?.startsWith('No.') ?? false),
        ),
      );
      expect(tiles, findsNWidgets(17));
      final tileWidth = tester.getSize(tiles.first).width;
      for (final tile in tiles.evaluate()) {
        expect((tile.renderObject! as RenderBox).size.width, tileWidth);
      }
      if (output != null) {
        await Scrollable.ensureVisible(tester.element(title), alignment: .08);
        await tester.pumpAndSettle();
        await save(tester, boundary, output, '$device-th08-last-word');
      }
      await _tap(tester, inArchive(find.text(strings.spellList)));
      expect(inArchive(find.text('No.206')), findsOneWidget);
      expect(inArchive(find.text('「季節外れのバタフライストーム」')), findsOneWidget);
      expect(inArchive(find.text('1/19')), findsOneWidget);
      if (output != null && device == 'mobile-narrow') {
        await Scrollable.ensureVisible(tester.element(title), alignment: .08);
        await tester.pumpAndSettle();
        await save(tester, boundary, output, '$device-th08-last-word-list');
      }
      await tester.binding.handlePopRoute();
      await tester.pumpAndSettle();
      expect(find.byType(LibraryPage), findsOneWidget);
      expect(tester.takeException(), isNull);
    });
  }

  const zh = LauncherLanguage.simplifiedChinese;
  testWidgets('320 px th15 switches between saved modes', (tester) async {
    tester.view.devicePixelRatio = 1;
    tester.view.physicalSize = const Size(320, 640);
    addTearDown(tester.view.resetPhysicalSize);
    addTearDown(tester.view.resetDevicePixelRatio);
    final game = realCatalog.singleWhere((game) => game.id == 'th15');
    final strings = lookupLauncherStrings(zh.locale);
    final api = _CloudApi({
      'products': [
        {
          'id': game.id,
          'game': game.id,
          'multiplayer': false,
          'files': ['scoreth15.dat'],
          'requiredFiles': ['scoreth15.dat'],
          'revision': 1,
        },
      ],
      'saves': [
        {
          'product': game.id,
          'revision': 1,
          'uploadedAt': '2026-11-24T10:00:00Z',
          'files': [],
          'statistics': {
            'parserVersion': 1,
            'game': game.id,
            'progress': [
              {
                'character': 0,
                'difficulty': 'normal',
                'mode': 'legacy',
                'oneCreditClear': true,
              },
              {
                'character': 0,
                'difficulty': 'normal',
                'mode': 'pointdevice',
                'cleared': true,
              },
              {
                'character': 3,
                'difficulty': 'extra',
                'cleared': false,
                'playCount': 1,
              },
            ],
          },
        },
      ],
    })..rejected = false;
    final boundary = GlobalKey();
    await tester.pumpWidget(
      RepaintBoundary(
        key: boundary,
        child: launcher.fixture(accountApi: api, games: [game]),
      ),
    );
    await tester.pumpAndSettle();
    await _tap(tester, find.byType(GameArtwork).last);
    await launcher.signInFixture(tester);
    expect(find.byType(GameDetail), findsOneWidget);
    expect(api.requests, 1);
    final legacy = tooltip('Normal | ${strings.progressOneCredit}');
    final pointdevice = tooltip('Normal | ${strings.progressCleared}');
    final extra = tooltip('Extra | ${strings.progressFailed}');
    final identity = find.text('铃仙·优昙华院·因幡');
    final progress = find.text(strings.clearProgress);
    final result = tester.widget<GameClearMatrix>(find.byType(GameClearMatrix));
    expect(result.progress, hasLength(3));
    expect(legacy, findsOneWidget);
    expect(pointdevice, findsNothing);
    expect(extra, findsOneWidget);
    expect(
      tester.renderObject<RenderParagraph>(identity).didExceedMaxLines,
      isFalse,
    );
    expect(tester.getTopRight(extra).dx, lessThanOrEqualTo(320));
    await Scrollable.ensureVisible(tester.element(progress), alignment: .05);
    await tester.pumpAndSettle();
    if (evidence != null) {
      await save(
        tester,
        boundary,
        Directory(evidence),
        'mobile-narrow-th15-legacy',
      );
    }
    final position = tester.getTopLeft(progress);
    await _tap(tester, find.text(strings.progressPointdevice));
    expect(pointdevice, findsOneWidget);
    expect(
      tester.renderObject<RenderParagraph>(identity).didExceedMaxLines,
      isFalse,
    );
    expect(legacy, findsNothing);
    expect(extra, findsOneWidget);
    expect(tester.getTopLeft(progress), position);
    if (evidence != null) {
      await save(
        tester,
        boundary,
        Directory(evidence),
        'mobile-narrow-th15-pointdevice',
      );
    }
    await _tap(tester, find.text(strings.progressLegacy));
    expect(legacy, findsOneWidget);
    expect(tester.getTopLeft(progress), position);
    expect(tester.takeException(), isNull);
  });

  for (final (device, size, language, reimu, marisa, mode) in [
    ('mobile-narrow', const Size(320, 640), zh, '灵梦', '魔理沙', ThemeMode.dark),
    (
      'mobile-narrow-en',
      const Size(320, 640),
      LauncherLanguage.english,
      'Reimu',
      'Marisa',
      ThemeMode.dark,
    ),
    (
      'mobile-ja',
      const Size(390, 844),
      LauncherLanguage.japanese,
      '霊夢',
      '魔理沙',
      ThemeMode.dark,
    ),
    if (evidence != null) ...[
      ('mobile', const Size(390, 844), zh, '灵梦', '魔理沙', ThemeMode.dark),
      ('desktop', const Size(1440, 734), zh, '灵梦', '魔理沙', ThemeMode.dark),
      (
        'mobile-narrow-light',
        const Size(320, 640),
        zh,
        '灵梦',
        '魔理沙',
        ThemeMode.light,
      ),
      ('mobile-light', const Size(390, 844), zh, '灵梦', '魔理沙', ThemeMode.light),
      (
        'desktop-light',
        const Size(1440, 734),
        zh,
        '灵梦',
        '魔理沙',
        ThemeMode.light,
      ),
    ],
  ]) {
    testWidgets(
      '$device detail matrix receives current save and preserves position',
      (tester) async {
        tester.view.devicePixelRatio = 1;
        tester.view.physicalSize = size;
        addTearDown(tester.view.resetPhysicalSize);
        addTearDown(tester.view.resetDevicePixelRatio);

        final strings = lookupLauncherStrings(language.locale);
        final boundary = GlobalKey();
        final currentPayload =
            jsonDecode(jsonEncode(payload)) as Map<String, dynamic>;
        final api = _CloudApi(currentPayload);
        await tester.pumpWidget(
          RepaintBoundary(
            key: boundary,
            child: launcher.fixture(
              accountApi: api,
              games: const [_th07],
              preferences: LauncherPreferences(
                SharedPreferencesAsync(),
                mode,
                true,
                language: language,
              ),
            ),
          ),
        );
        await tester.pumpAndSettle();
        await _tap(tester, find.byType(GameArtwork).last);
        expect(find.byType(GameDetail), findsOneWidget);
        expect(api.requests, 0);
        final title = find.text(strings.clearProgress);
        await tester.ensureVisible(title);
        await tester.pumpAndSettle();
        if (evidence != null) {
          final output = Directory(evidence)..createSync(recursive: true);
          await save(tester, boundary, output, '$device-game-progress-guest');
        }
        await launcher.signInFixture(tester);
        expect(find.byType(GameDetail), findsOneWidget);
        expect(api.requests, 1);
        await _tap(tester, find.text(strings.cloudReadFailed));
        expect(api.requests, 2);
        expect(find.text(strings.cloudUpload), findsOneWidget);
        final detailScroll = find
            .descendant(
              of: find.byType(GameDetail),
              matching: find.byType(Scrollable),
            )
            .first;
        await tester.drag(detailScroll, const Offset(0, 600));
        await tester.pumpAndSettle();
        expect(find.text(strings.formatPlaytime(7800)), findsOneWidget);
        await tester.ensureVisible(title);
        await tester.pumpAndSettle();
        expect(find.byType(GameSpellArchive), findsOneWidget);
        final locale = MaterialLocalizations.of(tester.element(title));
        final normal = tooltip(
          'Normal | ${strings.progressOneCredit} | ${strings.progressBestScore} ${locale.formatDecimal(12345678)}',
        );
        final easy = tooltip(
          'Easy | ${strings.progressCleared} | ${strings.progressBestScore} ${locale.formatDecimal(1500000)}',
        );
        expect(normal, findsOneWidget);
        expect(easy, findsOneWidget);
        expect(
          tooltip('Extra | ${strings.progressUnavailable}'),
          findsOneWidget,
        );
        expect(
          tooltip('Phantasm | ${strings.progressUnread}'),
          findsNWidgets(6),
        );
        expect(
          tester.getSize(normal).width,
          closeTo(tester.getSize(easy).width, .01),
        );
        final phantasm = find.byTooltip('Phantasm');
        expect(phantasm, findsOneWidget);
        expect(tester.getTopRight(phantasm).dx, lessThanOrEqualTo(size.width));
        expect(find.text(reimu), findsOneWidget);
        expect(find.text(marisa), findsOneWidget);
        await tester.ensureVisible(title);
        await tester.pumpAndSettle();
        if (evidence != null) {
          await save(
            tester,
            boundary,
            Directory(evidence),
            '$device-game-progress',
          );
        }

        final spell = tooltip('No.08 冬符「フラワーウィザラウェイ」 | 7/11');
        final second = tooltip('No.04 寒符「リンガリングコールド」 | 11/11');
        expect(tester.getSize(spell).width, tester.getSize(second).width);
        expect(find.text(strings.spellStage(1)), findsOneWidget);
        Finder spellNumber(int number) => find.byWidgetPredicate(
          (item) =>
              item is Tooltip &&
              item.message?.startsWith('No.$number ') == true,
        );
        expect(
          tester.getTopLeft(spellNumber(46)).dy,
          tester.getTopLeft(spellNumber(66)).dy,
        );
        expect(
          tester.getTopLeft(spellNumber(94)).dy,
          tester.getTopLeft(spellNumber(114)).dy,
        );
        final section = find.textContaining(strings.spellCards);
        await Scrollable.ensureVisible(tester.element(section), alignment: .05);
        await tester.pumpAndSettle();
        if (evidence != null) {
          await save(
            tester,
            boundary,
            Directory(evidence),
            '$device-spell-map',
          );
        }
        await _tap(tester, spell);
        expect(
          find.text('No.08 冬符「フラワーウィザラウェイ」 | 7/11', findRichText: true),
          findsOneWidget,
        );
        Tooltip.dismissAllToolTips();
        await tester.pumpAndSettle();
        await _tap(tester, find.text(strings.spellList));
        expect(find.text('冬符「フラワーウィザラウェイ」'), findsOneWidget);
        expect(
          find.text('63.6%'),
          size.width >= 780 ? findsOneWidget : findsNothing,
        );
        if (evidence != null) {
          await save(
            tester,
            boundary,
            Directory(evidence),
            '$device-spell-list',
          );
        }
        await _tap(tester, easy);
        expect(scope('$reimu A | Easy'), findsOneWidget);
        expect(find.text('冬符「フラワーウィザラウェイ」'), findsNothing);
        expect(tester.takeException(), isNull);
        if (device == 'mobile-narrow') {
          final beforeRefresh = tester.getTopLeft(title).dy;
          final game = (currentPayload['saves'] as List).last;
          final progress = game['statistics']['progress'] as List;
          final selected = progress.singleWhere(
            (row) => row['difficulty'] == 'easy' && row['character'] == 0,
          );
          selected['bestScore'] = 1600000;
          game['revision'] = 4;
          (currentPayload['products'] as List).first['revision'] = 4;
          tester
              .widget<GameDetail>(find.byType(GameDetail))
              .session
              .cloudSaveRevision
              .value++;
          await tester.pumpAndSettle();
          expect(api.requests, 3);
          expect(scope('$reimu A | Easy'), findsOneWidget);
          expect(tester.getTopLeft(title).dy, closeTo(beforeRefresh, 1));
          expect(
            tooltip(
              'Easy | ${strings.progressCleared} | ${strings.progressBestScore} ${locale.formatDecimal(1600000)}',
            ),
            findsOneWidget,
          );
          if (evidence != null) {
            await _tap(tester, find.text(strings.spellMap));
            await Scrollable.ensureVisible(tester.element(title), alignment: 0);
            await tester.pumpAndSettle();
            await save(
              tester,
              boundary,
              Directory(evidence),
              '$device-refreshed',
            );
          }
          final settings = find.byTooltip(strings.gameSettings);
          await tester.scrollUntilVisible(
            settings,
            -220,
            scrollable: find
                .descendant(
                  of: find.byType(GameDetail),
                  matching: find.byType(Scrollable),
                )
                .first,
          );
          await tester.pumpAndSettle();
          final offset = tester.getTopLeft(title).dy;
          await _tap(tester, settings);
          await tester.binding.handlePopRoute();
          await tester.pumpAndSettle();
          expect(find.byType(GameDetail), findsOneWidget);
          expect(scope('$reimu A | Easy'), findsOneWidget);
          expect(tester.getTopLeft(title).dy, closeTo(offset, 1));
        }
        if (device == 'mobile-narrow-en' || device == 'desktop') {
          await _tap(tester, normal);
          await _tap(tester, find.text(strings.spellMap));
          Tooltip.dismissAllToolTips();
          await tester.pumpAndSettle();
          final manager = FocusManager.instance;
          final previousStrategy = manager.highlightStrategy;
          addTearDown(() => manager.highlightStrategy = previousStrategy);
          manager.highlightStrategy = FocusHighlightStrategy.alwaysTraditional;

          Future<void> focusCell(Finder cell) async {
            await tester.ensureVisible(cell);
            await tester.pumpAndSettle();
            final tile = find.descendant(
              of: cell,
              matching: find.byType(StatisticsTile),
            );
            Focus.of(tester.element(tile)).requestFocus();
            await tester.pumpAndSettle();
          }

          await focusCell(normal);
          final scoreHint = find.text(
            tester.widget<Tooltip>(normal).message!,
            findRichText: true,
          );
          expect(scoreHint, findsOneWidget);
          final semantics = tester.ensureSemantics();
          final scoreLabel =
              '$reimu A | ${tester.widget<Tooltip>(normal).message}';
          expect(find.bySemanticsLabel(scoreLabel), findsOneWidget);
          semantics.dispose();
          await focusCell(second);
          expect(scoreHint, findsNothing);
          final firstHint = find.text(
            'No.04 寒符「リンガリングコールド」 | 11/11',
            findRichText: true,
          );
          expect(firstHint, findsOneWidget);
          await tester.sendKeyEvent(LogicalKeyboardKey.tab);
          await tester.pumpAndSettle();
          final secondHint = find.text(
            'No.08 冬符「フラワーウィザラウェイ」 | 7/11',
            findRichText: true,
          );
          expect(firstHint, findsNothing);
          expect(secondHint, findsOneWidget);
          final hintBounds = tester.getRect(secondHint);
          expect(hintBounds.left, greaterThanOrEqualTo(12));
          expect(hintBounds.right, lessThanOrEqualTo(size.width - 12));
          if (evidence != null) {
            await save(
              tester,
              boundary,
              Directory(evidence),
              '$device-spell-keyboard',
            );
          }
          await tester.sendKeyEvent(LogicalKeyboardKey.escape);
          await tester.pumpAndSettle();
          expect(secondHint, findsNothing);
          expect(scope('$reimu A | Normal'), findsOneWidget);
        }
      },
    );
  }
}
