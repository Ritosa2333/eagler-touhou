import 'dart:convert';
import 'dart:io';

import 'package:flutter/foundation.dart';
import 'package:flutter/rendering.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:material_ui/material_ui.dart';
import 'package:shared_preferences/shared_preferences.dart';
import 'package:shared_preferences_platform_interface/in_memory_shared_preferences_async.dart';
import 'package:shared_preferences_platform_interface/shared_preferences_async_platform_interface.dart';
import 'package:touhouonline/catalog.dart';
import 'package:touhouonline/cloud_saves.dart';
import 'package:touhouonline/game_detail.dart';
import 'package:touhouonline/game_clear_matrix.dart';
import 'package:touhouonline/game_spell_archive.dart';
import 'package:touhouonline/l10n/strings.dart';
import 'package:touhouonline/library.dart';
import 'package:touhouonline/save_progress.dart';

import '../tools/capture_native_baseline_test.dart' show loadFonts, save;
import 'widget_test.dart' as launcher;

class _ProgressApi extends launcher.FixtureAccountApi {
  _ProgressApi(this.payload);
  final Map<String, dynamic> payload;

  @override
  Future<Map<String, CloudGameSave>> cloudGameSaves() async =>
      CloudSaveDirectory.fromJson(payload).statistics;
}

void main() {
  final output = Platform.environment['TOUHOU_DETAIL_ACCESSIBILITY_CAPTURE'];
  late Game game;
  late Game th07;
  late Map<String, dynamic> progress;
  setUpAll(() async {
    await loadFonts();
    game = (await Game.load()).firstWhere((game) => game.id == 'th06');
    th07 = (await Game.load()).firstWhere((game) => game.id == 'th07');
    progress = jsonDecode(
      File('tools/game-progress-fixture.json').readAsStringSync(),
    ) as Map<String, dynamic>;
  });
  for (final (name, size, language) in [
    ('320-ja', const Size(320, 640), LauncherLanguage.japanese),
    ('320-en', const Size(320, 640), LauncherLanguage.english),
    ('390-zh', const Size(390, 844), LauncherLanguage.simplifiedChinese),
    ('781-en', const Size(781, 734), LauncherLanguage.english),
    ('1440-en', const Size(1440, 734), LauncherLanguage.english),
  ]) {
    for (final light in [false, true]) {
      for (final scale in [1.0, 2.0]) {
        final scene = '$name-${light ? 'light' : 'dark'}-${scale.toInt()}';
        testWidgets('detail accessible action labels | $scene', (tester) async {
          debugDefaultTargetPlatformOverride = TargetPlatform.windows;
          addTearDown(() => debugDefaultTargetPlatformOverride = null);
          tester.view.devicePixelRatio = 1;
          tester.view.physicalSize = size;
          tester.platformDispatcher.textScaleFactorTestValue = scale;
          addTearDown(tester.view.resetDevicePixelRatio);
          addTearDown(tester.view.resetPhysicalSize);
          addTearDown(tester.platformDispatcher.clearTextScaleFactorTestValue);
          SharedPreferencesAsyncPlatform.instance =
              InMemorySharedPreferencesAsync.empty();
          final preferences = LauncherPreferences(
            SharedPreferencesAsync(),
            light ? ThemeMode.light : ThemeMode.dark,
            true,
            language: language,
          );
          final strings = lookupLauncherStrings(language.locale);
          final boundary = GlobalKey();
          await tester.pumpWidget(
            RepaintBoundary(
              key: boundary,
              child: launcher.fixture(
                games: [game],
                preferences: preferences,
                accountApi: launcher.FixtureAccountApi(),
              ),
            ),
          );
          await tester.pumpAndSettle();
          await tester.runAsync(() async {
            final context = tester.element(find.byType(LibraryPage));
            await precacheImage(
              AssetImage('assets/book-covers/${game.artwork}'),
              context,
            );
          });
          final artwork = find.descendant(
            of: find.byType(LibraryPage),
            matching: find.byType(GameArtwork),
          );
          await tester.tap(artwork);
          await tester.pumpAndSettle();
          final start = find.text(strings.startGame);
          final paragraph = tester.renderObject<RenderParagraph>(start);
          final truncated = paragraph.didExceedMaxLines;
          final clippedLevels = <String>[];
          const levelLabels = {
            'Easy',
            'Normal',
            'Hard',
            'Lunatic',
            'Extra',
            'Phantasm',
            'E',
            'N',
            'H',
            'L',
            'Ex',
            'Ph',
          };
          for (final element
              in find
                  .descendant(
                    of: find.byType(GameClearMatrix),
                    matching: find.byType(Text),
                  )
                  .evaluate()) {
            final text = element.widget as Text;
            if (levelLabels.contains(text.data) &&
                tester
                    .renderObject<RenderParagraph>(find.byWidget(text))
                    .didExceedMaxLines) {
              clippedLevels.add(text.data!);
            }
          }
          final cover = find.descendant(
            of: find.byType(GameHeading),
            matching: find.byType(GameArtwork),
          );
          final box = tester.getRect(cover);
          if (output != null) {
            await save(tester, boundary, Directory(output), scene);
          }
          final settings = find.widgetWithText(
            OutlinedButton,
            strings.gameSettings,
          );
          final settingsEntry = settings.evaluate().isEmpty
              ? find.byTooltip(strings.gameSettings)
              : settings;
          await tester.ensureVisible(settingsEntry);
          await tester.pumpAndSettle();
          final beforeSettings = tester.getRect(cover);
          await tester.tap(settingsEntry);
          await tester.pumpAndSettle();
          await tester.tap(find.byType(BackButton));
          await tester.pumpAndSettle();
          expect(find.byType(GameDetail), findsOneWidget);
          expect(tester.getRect(cover), beforeSettings);
          await tester.ensureVisible(find.byType(BackButton));
          await tester.pumpAndSettle();
          await tester.tap(find.byType(BackButton));
          await tester.pumpAndSettle();
          expect(find.byType(LibraryPage), findsOneWidget);
          expect(tester.takeException(), isNull);
          expect(box.width, closeTo(box.height, .001));
          expect(
            clippedLevels,
            isEmpty,
            reason: 'Difficulty captions must fit their cells',
          );
          expect(
            truncated,
            isFalse,
            reason: 'The primary action must show its complete label',
          );
        });
      }
    }
  }
  final progressOutput = Platform.environment['TOUHOU_DETAIL_CONTENT_CAPTURE'];
  for (final (widthName, size) in [
    ('320', const Size(320, 640)),
    ('390', const Size(390, 844)),
    ('1440', const Size(1440, 734)),
  ]) {
    for (final language in LauncherLanguage.values) {
      final name = '$widthName-${language.locale.languageCode}';
      for (final light in [false, true]) {
        for (final scale in [1.0, 2.0]) {
          final scene = '$name-${light ? 'light' : 'dark'}-${scale.toInt()}';
          testWidgets('populated detail | $scene', (tester) async {
            tester.view.devicePixelRatio = 1;
            tester.view.physicalSize = size;
            tester.platformDispatcher.textScaleFactorTestValue = scale;
            addTearDown(tester.view.resetDevicePixelRatio);
            addTearDown(tester.view.resetPhysicalSize);
            addTearDown(
              tester.platformDispatcher.clearTextScaleFactorTestValue,
            );
            SharedPreferencesAsyncPlatform.instance =
                InMemorySharedPreferencesAsync.empty();
            final strings = lookupLauncherStrings(language.locale);
            final boundary = GlobalKey();
            await tester.pumpWidget(
              RepaintBoundary(
                key: boundary,
                child: launcher.fixture(
                  games: [th07],
                  accountApi: _ProgressApi(progress),
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
              await precacheImage(
                AssetImage('assets/book-covers/${th07.artwork}'),
                tester.element(find.byType(LibraryPage)),
              );
              await precacheImage(
                AssetImage(
                  'assets/launcher-wordmark-${light ? 'light' : 'dark'}.png',
                ),
                tester.element(find.byType(LibraryPage)),
              );
            });
            await tester.tap(
              find.descendant(
                of: find.byType(LibraryPage),
                matching: find.byType(GameArtwork),
              ),
            );
            await tester.pumpAndSettle();
            await tester.runAsync(
              () => tester
                  .widget<GameDetail>(find.byType(GameDetail))
                  .session
                  .signIn('fixture', 'valid-test-value'),
            );
            await tester.pumpAndSettle();
            final sidebar = find.byType(BookSidebar);
            final sidebarRect = sidebar.evaluate().isEmpty
                ? null
                : tester.getRect(sidebar.first);
            final errors = <Object>[];
            void collectErrors() {
              Object? error;
              while ((error = tester.takeException()) != null) {
                errors.add(error!);
              }
            }

            void verify(void Function() check) {
              try {
                check();
              } on TestFailure catch (error) {
                errors.add(error);
              }
            }

            collectErrors();
            if (progressOutput != null) {
              await save(
                tester,
                boundary,
                Directory(progressOutput),
                '$scene-entry',
              );
            }
            final spell = find.byType(GameSpellArchive);
            expect(spell, findsOneWidget);
            await tester.ensureVisible(find.textContaining(strings.spellCards));
            await tester.pumpAndSettle();
            collectErrors();
            verify(() {
              final heading = find.textContaining(strings.spellCards);
              final paragraph = tester.renderObject<RenderParagraph>(heading);
              final text =
                  (tester.widget<Text>(heading).textSpan?.toPlainText() ?? '');
              if (text.isNotEmpty) {
                final boxes = paragraph.getBoxesForSelection(
                  TextSelection(
                    baseOffset: strings.spellCards.length + 2,
                    extentOffset: text.length,
                  ),
                );
                expect(boxes, isNotEmpty);
                expect(
                  boxes.every(
                    (box) => box.bottom - box.top <= 11 * scale * 1.5,
                  ),
                  isTrue,
                  reason: 'The scope caption must use the small text role at the requested scale',
                );
              }
            });
            if (progressOutput != null) {
              await save(
                tester,
                boundary,
                Directory(progressOutput),
                '$scene-map',
              );
            }
            verify(() {
              for (final label in [
                strings.spellStageColumn,
                'Midboss',
                'Boss',
                strings.spellCaptured,
              ]) {
                final text = find
                    .descendant(of: spell, matching: find.text(label))
                    .first;
                final paragraph = tester.renderObject<RenderParagraph>(text);
                expect(
                  paragraph.didExceedMaxLines,
                  isFalse,
                  reason: '$label must remain readable',
                );
                expect(
                  paragraph
                      .getBoxesForSelection(
                        TextSelection(
                          baseOffset: 0,
                          extentOffset: label.length,
                        ),
                      )
                      .map((box) => box.top)
                      .toSet(),
                  hasLength(1),
                  reason: '$label must not break inside a word',
                );
              }
              if (size.width == 1440) {
                final label = strings.spellStage(1);
                final paragraph = tester.renderObject<RenderParagraph>(
                  find.descendant(of: spell, matching: find.text(label)),
                );
                final tops =
                    paragraph
                        .getBoxesForSelection(
                          TextSelection(
                            baseOffset: 0,
                            extentOffset: label.length,
                          ),
                        )
                        .map((box) => box.top)
                        .toList()
                      ..sort();
                expect(
                  tops.last - tops.first,
                  lessThan(11 * scale),
                  reason: 'Wide tables keep stage labels together',
                );
              }
            });
            final horizontalMap = find.descendant(
              of: spell,
              matching: find.byWidgetPredicate(
                (widget) =>
                    widget is SingleChildScrollView &&
                    widget.scrollDirection == Axis.horizontal,
              ),
            );
            double horizontalExtent = 0;
            if (horizontalMap.evaluate().isNotEmpty) {
              final position = tester
                  .state<ScrollableState>(
                    find.descendant(
                      of: horizontalMap,
                      matching: find.byType(Scrollable),
                    ),
                  )
                  .position;
              horizontalExtent = position.maxScrollExtent;
              if (scale == 1) {
                expect(
                  horizontalExtent,
                  0,
                  reason: 'Normal text uses shrinking tiles',
                );
              }
              position.jumpTo(horizontalExtent);
              await tester.pumpAndSettle();
              if (horizontalExtent > 0 && progressOutput != null) {
                await save(
                  tester,
                  boundary,
                  Directory(progressOutput),
                  '$scene-map-end',
                );
              }
            }
            await tester.ensureVisible(find.text(strings.spellList));
            await tester.pumpAndSettle();
            await tester.tap(find.text(strings.spellList));
            await tester.pumpAndSettle();
            collectErrors();
            verify(() {
              final numbers = find.descendant(
                of: spell,
                matching: find.byWidgetPredicate(
                  (widget) =>
                      widget is Text &&
                      (widget.data?.startsWith('No.') ?? false),
                ),
              );
              expect(numbers.evaluate(), isNotEmpty);
              final numberEdges = <double>{};
              for (final number in numbers.evaluate()) {
                final finder = find.byWidget(number.widget);
                final text = (number.widget as Text).data!;
                final paragraph = tester.renderObject<RenderParagraph>(finder);
                expect(
                  paragraph
                      .getBoxesForSelection(
                        TextSelection(baseOffset: 0, extentOffset: text.length),
                      )
                      .map((box) => box.top)
                      .toSet(),
                  hasLength(1),
                  reason: '$text must stay on one line',
                );
                numberEdges.add(tester.getRect(finder).right);
              }
              expect(
                numberEdges,
                hasLength(1),
                reason: 'All names share the same leading column',
              );
              for (final fraction in ['11/11', '7/11', '0/3']) {
                final paragraph = tester.renderObject<RenderParagraph>(
                  find
                      .descendant(of: spell, matching: find.text(fraction))
                      .first,
                );
                expect(
                  paragraph
                      .getBoxesForSelection(
                        TextSelection(
                          baseOffset: 0,
                          extentOffset: fraction.length,
                        ),
                      )
                      .map((box) => box.top)
                      .toSet(),
                  hasLength(1),
                  reason: '$fraction must stay on one line',
                );
              }
              if (sidebarRect != null) {
                expect(tester.getRect(sidebar.first), sidebarRect);
              }
            });
            if (progressOutput != null) {
              await save(
                tester,
                boundary,
                Directory(progressOutput),
                '$scene-list',
              );
              File('$progressOutput/$scene.json').writeAsStringSync(
                jsonEncode({
                  'viewport': [size.width, size.height],
                  'language': language.locale.toLanguageTag(),
                  'theme': light ? 'light' : 'dark',
                  'textScale': scale,
                  'horizontalMapExtent': horizontalExtent,
                }),
              );
            }
            await tester.binding.handlePopRoute();
            await tester.pumpAndSettle();
            expect(find.byType(LibraryPage), findsOneWidget);
            collectErrors();
            expect(
              errors,
              isEmpty,
              reason: 'Full save content must fit, including the map and list',
            );
          }, variant: TargetPlatformVariant.only(TargetPlatform.windows));
        }
      }
    }
  }
}
