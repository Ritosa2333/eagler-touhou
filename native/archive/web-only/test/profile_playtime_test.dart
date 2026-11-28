import 'dart:io';

import 'package:flutter/foundation.dart';
import 'package:flutter/rendering.dart' show RenderParagraph;
import 'package:flutter_test/flutter_test.dart';
import 'package:material_ui/material_ui.dart';
import 'package:shared_preferences/shared_preferences.dart';
import 'package:shared_preferences_platform_interface/in_memory_shared_preferences_async.dart';
import 'package:shared_preferences_platform_interface/shared_preferences_async_platform_interface.dart';
import 'package:touhouonline/catalog.dart';
import 'package:touhouonline/l10n/strings.dart';
import 'package:touhouonline/profile_data.dart';

import '../tools/capture_native_baseline_test.dart' show loadFonts, save;
import 'widget_test.dart' as launcher;

class _PlaytimeApi extends launcher.FixtureAccountApi {
  @override
  Future<PublicProfile> profile(String username, {bool fresh = false}) async =>
      PublicProfile(
        username: username,
        nickname: '测试玩家',
        showCareer: false,
        showVersus: false,
        defaultView: ProfileGames.all,
      );

  @override
  Future<ProfileStatistics> profileStatistics(
    String username, {
    required bool owner,
  }) async => ProfileStatistics(
    career: false,
    records: true,
    playtime: 11400,
    games: {
      for (final (id, seconds) in [
        ('th06', 7200),
        ('th07', 3600),
        ('th11', 600),
      ])
        id: (game: id, playtime: seconds, playCount: 1, progress: const []),
    },
  );
}

void main() {
  final capture = Platform.environment['TOUHOU_PLAYTIME_CAPTURE'];
  late List<Game> games;
  setUpAll(() async {
    games = await Game.load();
    if (Platform.isWindows || capture != null) {
      final fonts = Platform.environment['TOUHOU_CAPTURE_FONTS'];
      await loadFonts(fonts == null ? null : Directory(fonts));
    }
  });
  for (final (device, size, language) in [
    ('desktop', const Size(1440, 734), LauncherLanguage.simplifiedChinese),
    ('mobile', const Size(390, 844), LauncherLanguage.japanese),
    ('narrow', const Size(320, 640), LauncherLanguage.english),
  ]) {
    for (final light in [false, true]) {
      for (final scale in [1.0, 2.0]) {
        final stem =
            '$device-${light ? 'light' : 'dark'}-playtime-${scale.toInt()}';
        testWidgets(stem, (tester) async {
          debugDefaultTargetPlatformOverride = TargetPlatform.windows;
          addTearDown(() => debugDefaultTargetPlatformOverride = null);
          tester.view.devicePixelRatio = 1;
          tester.view.physicalSize = size;
          addTearDown(tester.view.resetDevicePixelRatio);
          addTearDown(tester.view.resetPhysicalSize);
          tester.platformDispatcher.textScaleFactorTestValue = scale;
          addTearDown(tester.platformDispatcher.clearTextScaleFactorTestValue);
          SharedPreferencesAsyncPlatform.instance =
              InMemorySharedPreferencesAsync.empty();
          final boundary = GlobalKey();
          await tester.pumpWidget(
            RepaintBoundary(
              key: boundary,
              child: launcher.fixture(
                games: games,
                accountApi: _PlaytimeApi(),
                initialLocation: '/u/fixture',
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
          await tester.runAsync(() async {
            final context = boundary.currentContext!;
            for (final id in ['th06', 'th07', 'th11']) {
              final game = games.firstWhere((game) => game.id == id);
              await precacheImage(
                AssetImage('assets/book-covers/${game.artwork}'),
                context,
              );
            }
          });
          await tester.pumpAndSettle();
          final strings = tester.element(find.byType(Scaffold).last).strings;
          final panel = find
              .ancestor(
                of: find.text(strings.profileTotalPlaytime),
                matching: find.byType(Card),
              )
              .first;
          await tester.ensureVisible(panel);
          await tester.pumpAndSettle();
          if (capture != null) {
            await save(tester, boundary, Directory(capture), stem);
          }
          expect(tester.takeException(), isNull);
          final bars = find.descendant(
            of: panel,
            matching: find.byType(LinearProgressIndicator),
          );
          expect(bars, findsNWidgets(3));
          for (final id
              in Platform.isWindows ? ['th06', 'th07', 'th11'] : <String>[]) {
            final title = games.firstWhere((game) => game.id == id).title;
            final label = find.descendant(
              of: panel,
              matching: find.text(title),
            );
            final paragraph = tester.renderObject<RenderParagraph>(label);
            expect(
              paragraph.didExceedMaxLines,
              isFalse,
              reason: '$stem | $title',
            );
          }
          final bounds = tester.getRect(panel);
          double? barLeft;
          for (final bar in bars.evaluate()) {
            final rect = tester.getRect(find.byWidget(bar.widget));
            expect(rect.width, greaterThanOrEqualTo(28));
            expect(bounds.contains(rect.topLeft), isTrue);
            expect(bounds.contains(rect.bottomRight), isTrue);
            if (barLeft != null) expect(rect.left, closeTo(barLeft, .5));
            barLeft = rect.left;
          }
          for (final seconds in [7200, 3600, 600]) {
            final duration = find.descendant(
              of: panel,
              matching: find.text(strings.formatPlaytime(seconds)),
            );
            expect(
              tester.getRect(duration).right,
              closeTo(bounds.right - 12, .5),
            );
          }
          final work = find.descendant(
            of: panel,
            matching: find.widgetWithText(
              TextButton,
              games.firstWhere((game) => game.id == 'th07').title,
            ),
          );
          await tester.ensureVisible(work);
          final scroll = tester
              .state<ScrollableState>(
                find
                    .ancestor(of: panel, matching: find.byType(Scrollable))
                    .first,
              )
              .position;
          final offset = scroll.pixels;
          await tester.tap(work);
          await tester.pumpAndSettle();
          expect(find.byTooltip(strings.profileBackAllGames), findsOneWidget);
          await tester.tap(find.byTooltip(strings.profileBackAllGames));
          await tester.pumpAndSettle();
          expect(find.text(strings.profileTotalPlaytime), findsOneWidget);
          expect(scroll.pixels, closeTo(offset, 1));
          expect(tester.takeException(), isNull);
        });
      }
    }
  }
}
