import 'dart:async';
import 'dart:convert';
import 'dart:io';

import 'package:flutter/rendering.dart';
import 'package:flutter/services.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:material_ui/material_ui.dart';
import 'package:shared_preferences/shared_preferences.dart';
import 'package:shared_preferences_platform_interface/in_memory_shared_preferences_async.dart';
import 'package:shared_preferences_platform_interface/shared_preferences_async_platform_interface.dart';
import 'package:touhouonline/catalog.dart';
import 'package:touhouonline/l10n/strings.dart';
import 'package:touhouonline/library.dart';
import 'package:touhouonline/site_navigation.dart';
import 'package:touhouonline/uptime_page.dart';

import '../tools/capture_native_baseline_test.dart' show loadFonts, save;
import 'widget_test.dart' as launcher;

class _SnapshotSource {
  _SnapshotSource(this.ranges);
  final Map<UptimeRange, UptimeSummary> ranges;
  bool fail = false;
  bool empty = false;
  final requests = <UptimeRange>[];

  Future<UptimeSummary> read(UptimeRange range) async {
    requests.add(range);
    if (fail) throw StateError('isolated unavailable service');
    final data = ranges[range]!;
    // Replay the captured measurements in a fresh 90-second status window.
    // Sample times and the response's update time remain the actual input.
    return UptimeSummary(
      data.range,
      data.updatedAt,
      empty
          ? []
          : [
              for (final row in data.services)
                (
                  id: row.id,
                  label: row.label,
                  group: row.group,
                  status: row.status,
                  checkedAt: DateTime.now(),
                  latency: row.latency,
                  availability: row.availability,
                  points: row.points,
                ),
            ],
    );
  }
}

void main() {
  final output = Platform.environment['TOUHOU_UPTIME_LAYOUT_CAPTURE'];
  late Map<UptimeRange, UptimeSummary> ranges;
  late Game game;
  setUpAll(() async {
    await loadFonts();
    game = (await Game.load()).firstWhere((game) => game.id == 'th06');
    final fixture = jsonDecode(
      File('tools/uptime-status-fixture.json').readAsStringSync(),
    ) as Map<String, dynamic>;
    ranges = {
      for (final range in UptimeRange.values)
        range: UptimeSummary.decode(
          jsonEncode(fixture['ranges'][range.key]),
          range,
        ),
    };
  });

  for (final size in [
    const Size(320, 640),
    const Size(390, 844),
    const Size(1440, 734),
  ]) {
    for (final language in LauncherLanguage.values) {
      for (final light in [false, true]) {
        for (final scale in [1.0, 2.0]) {
          final scene =
              '${size.width.toInt()}-${language.locale.languageCode}-${light ? 'light' : 'dark'}-${scale.toInt()}';
          testWidgets('status table | $scene', (tester) async {
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
            final source = _SnapshotSource(ranges);
            final frame = GlobalKey();
            await tester.pumpWidget(
              RepaintBoundary(
                key: frame,
                child: launcher.fixture(
                  games: [game],
                  accountApi: launcher.FixtureAccountApi(),
                  uptimeSource: source.read,
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
              final context = tester.element(find.byType(LibraryPage));
              await precacheImage(
                AssetImage('assets/book-covers/${game.artwork}'),
                context,
              );
              await precacheImage(
                AssetImage(
                  'assets/launcher-wordmark-${light ? 'light' : 'dark'}.png',
                ),
                context,
              );
            });
            final mobile = size.width < 780;
            await tester.tap(
              mobile
                  ? find.byType(DrawerButton)
                  : find.byTooltip(strings.siteMenu),
            );
            await tester.pumpAndSettle();
            final link = mobile
                ? find.descendant(
                    of: find.byType(SiteDrawer),
                    matching: find.text(strings.serviceStatus),
                  )
                : find.text(strings.serviceStatus);
            await tester.ensureVisible(link);
            await tester.pumpAndSettle();
            await tester.tap(link);
            await tester.pumpAndSettle();
            final page = find.byType(UptimePage);
            expect(page, findsOneWidget);
            final errors = <Object>[];
            void collect() {
              Object? error;
              while ((error = tester.takeException()) != null) {
                errors.add(error!);
              }
            }

            void check(void Function() run) {
              try {
                run();
              } on TestFailure catch (error) {
                errors.add(error);
              }
            }

            Future<void> capture(String state) async {
              collect();
              if (output != null) {
                await save(tester, frame, Directory(output), '$scene-$state');
              }
            }

            Finder rangeLabel(UptimeRange range) {
              final label = find.text(range.label(strings));
              return label.evaluate().isNotEmpty
                  ? label
                  : find.byTooltip(range.label(strings));
            }

            Future<void> select(UptimeRange range) async {
              await tester.tap(rangeLabel(range));
              await tester.pumpAndSettle();
              expect(
                tester
                    .widget<SegmentedButton<UptimeRange>>(
                      find.byType(SegmentedButton<UptimeRange>),
                    )
                    .selected,
                {range},
              );
              expect(source.requests.last, range);
            }

            await capture('ready');
            final selector = tester.getRect(
              find.byType(SegmentedButton<UptimeRange>),
            );
            check(() {
              final cards = find.descendant(
                of: page,
                matching: find.byType(Card),
              );
              expect(
                cards,
                findsOneWidget,
                reason: 'Services share the original grouped table surface',
              );
              for (final text
                  in find
                      .descendant(
                        of: find.byType(SegmentedButton<UptimeRange>),
                        matching: find.byType(Text),
                      )
                      .evaluate()) {
                final label = tester.renderObject<RenderParagraph>(
                  find.byWidget(text.widget),
                );
                expect(label.didExceedMaxLines, isFalse);
                expect(
                  label.size.height,
                  lessThanOrEqualTo(38),
                  reason: 'Range labels must fit inside the control',
                );
              }
            });
            final sample = find.byTooltip(strings.uptimeShowSample).first;
            final icon = find.descendant(
              of: sample,
              matching: find.byIcon(Icons.history),
            );
            Focus.of(tester.element(icon)).requestFocus();
            await tester.pump();
            await tester.sendKeyEvent(LogicalKeyboardKey.enter);
            await tester.pumpAndSettle();
            expect(find.byTooltip(strings.uptimeHideSample), findsOneWidget);
            final previous = find.byTooltip(strings.uptimePreviousSample);
            await tester.ensureVisible(previous);
            await tester.pumpAndSettle();
            await tester.tap(previous);
            await tester.pumpAndSettle();
            await capture('sample');
            await tester.ensureVisible(
              find.byTooltip(strings.uptimeHideSample),
            );
            await tester.pumpAndSettle();
            await tester.tap(find.byTooltip(strings.uptimeHideSample));
            await tester.pumpAndSettle();
            await select(UptimeRange.week);
            await select(UptimeRange.quarter);
            check(() {
              final first = ranges[UptimeRange.quarter]!.services.first;
              final bars =
                  [0, first.points.length ~/ 2, first.points.length - 1]
                      .map(
                        (index) => find.byKey(
                          ValueKey(('uptime-point', first.id, index)),
                        ),
                      )
                      .toList();
              if (bars.first.evaluate().isNotEmpty) {
                final width = tester.getSize(bars.first).width;
                expect(width, greaterThan(0));
                for (final bar in bars) {
                  expect(tester.getSize(bar).width, closeTo(width, .001));
                }
              }
              final table = find.byKey(const ValueKey('uptime-services'));
              if (table.evaluate().isNotEmpty) {
                final names = <double>{},
                    latencies = <double>{},
                    availability = <double>{};
                for (final service in ranges[UptimeRange.quarter]!.services) {
                  names.add(
                    tester
                        .getRect(
                          find.byKey(ValueKey(('uptime-name', service.id))),
                        )
                        .left,
                  );
                  latencies.add(
                    tester
                        .getRect(
                          find.byKey(ValueKey(('uptime-latency', service.id))),
                        )
                        .right,
                  );
                  availability.add(
                    tester
                        .getRect(
                          find.byKey(
                            ValueKey(('uptime-availability', service.id)),
                          ),
                        )
                        .right,
                  );
                }
                expect(names, hasLength(1));
                expect(latencies, hasLength(1));
                expect(availability, hasLength(1));
              }
            });
            expect(
              tester.getRect(find.byType(SegmentedButton<UptimeRange>)),
              selector,
            );
            source.fail = true;
            await tester.tap(find.byTooltip(strings.uptimeRefresh));
            await tester.pumpAndSettle();
            expect(find.text(strings.uptimeFailed), findsOneWidget);
            expect(find.text(strings.uptimeUnknown), findsWidgets);
            await capture('failed');
            source.fail = false;
            source.empty = true;
            await tester.tap(
              find.widgetWithText(TextButton, strings.uptimeRetry),
            );
            await tester.pumpAndSettle();
            expect(find.text(strings.uptimeEmpty), findsOneWidget);
            await capture('empty');
            if (output != null) {
              File('$output/$scene.json').writeAsStringSync(
                jsonEncode({
                  'viewport': [size.width, size.height],
                  'language': language.locale.toLanguageTag(),
                  'theme': light ? 'light' : 'dark',
                  'textScale': scale,
                  'selector': [
                    selector.left,
                    selector.top,
                    selector.width,
                    selector.height,
                  ],
                  'source': 'tools/uptime-status-fixture.json',
                }),
              );
            }
            await tester.tap(find.byType(BackButton));
            await tester.pumpAndSettle();
            expect(find.byType(LibraryPage), findsOneWidget);
            collect();
            expect(
              errors,
              isEmpty,
              reason:
                  'The page must keep its grouped layout and fit all states',
            );
          }, variant: TargetPlatformVariant.only(TargetPlatform.windows));
        }
      }
    }
  }

  testWidgets(
    'Status refresh ignores a late range and preserves data on failure',
    (tester) async {
      SharedPreferencesAsyncPlatform.instance =
          InMemorySharedPreferencesAsync.empty();
      final requests =
          <({UptimeRange range, Completer<UptimeSummary> result})>[];
      Future<UptimeSummary> read(UptimeRange range) {
        final result = Completer<UptimeSummary>();
        requests.add((range: range, result: result));
        return result.future;
      }

      await tester.pumpWidget(
        launcher.fixture(initialLocation: '/uptime', uptimeSource: read),
      );
      await tester.pump(const Duration(milliseconds: 300));
      expect(requests.single.range, UptimeRange.day);
      await tester.tap(find.text('7 天'));
      await tester.pump();
      expect(requests.last.range, UptimeRange.week);
      final week = ranges[UptimeRange.week]!;
      requests.last.result.complete(week);
      await tester.pumpAndSettle();
      requests.first.result.complete(
        UptimeSummary(UptimeRange.day, week.updatedAt, []),
      );
      await tester.pumpAndSettle();
      expect(find.text('网站入口 N01'), findsOneWidget);
      await tester.tap(find.byTooltip('刷新服务状态'));
      await tester.pump();
      requests.last.result.completeError(
        StateError('isolated network failure'),
      );
      await tester.pumpAndSettle();
      expect(find.text('网站入口 N01'), findsOneWidget);
      expect(find.text('未知'), findsWidgets);
      await tester.tap(find.text('重试'));
      await tester.pump();
      requests.last.result.complete(
        UptimeSummary(UptimeRange.week, week.updatedAt, []),
      );
      await tester.pumpAndSettle();
      expect(
        find.text(lookupLauncherStrings(const Locale('zh')).uptimeEmpty),
        findsOneWidget,
      );
      await tester.pumpWidget(const SizedBox.shrink());
      expect(tester.takeException(), isNull);
    },
    variant: TargetPlatformVariant.only(TargetPlatform.windows),
  );

  testWidgets(
    'Status refresh pauses in the background and avoids duplicate reads',
    (tester) async {
      SharedPreferencesAsyncPlatform.instance =
          InMemorySharedPreferencesAsync.empty();
      final source = _SnapshotSource(ranges);
      await tester.pumpWidget(
        launcher.fixture(initialLocation: '/uptime', uptimeSource: source.read),
      );
      await tester.pumpAndSettle();
      expect(source.requests, [UptimeRange.day]);
      tester.binding.handleAppLifecycleStateChanged(AppLifecycleState.paused);
      await tester.pump(const Duration(seconds: 60));
      expect(source.requests, [UptimeRange.day]);
      tester.binding.handleAppLifecycleStateChanged(AppLifecycleState.resumed);
      await tester.pumpAndSettle();
      expect(source.requests, [UptimeRange.day, UptimeRange.day]);
      await tester.pump(const Duration(seconds: 30));
      await tester.pumpAndSettle();
      expect(source.requests, [
        UptimeRange.day,
        UptimeRange.day,
        UptimeRange.day,
      ]);
      await tester.pumpWidget(const SizedBox.shrink());
      expect(tester.takeException(), isNull);
    },
    variant: TargetPlatformVariant.only(TargetPlatform.windows),
  );

  testWidgets(
    'Status sample survives window changes with a meaningful button name',
    (tester) async {
      SharedPreferencesAsyncPlatform.instance =
          InMemorySharedPreferencesAsync.empty();
      tester.view.devicePixelRatio = 1;
      tester.view.physicalSize = const Size(1440, 734);
      addTearDown(tester.view.resetDevicePixelRatio);
      addTearDown(tester.view.resetPhysicalSize);
      final semantics = tester.ensureSemantics();
      final source = _SnapshotSource(ranges);
      await tester.pumpWidget(
        launcher.fixture(initialLocation: '/uptime', uptimeSource: source.read),
      );
      await tester.pumpAndSettle();
      final service = ranges[UptimeRange.day]!.services.first;
      final button = find.byKey(ValueKey(('uptime-sample-button', service.id)));
      final label = tester.getSemantics(button).getSemanticsData().label;
      expect(label, contains(service.label));
      expect(
        label,
        contains(lookupLauncherStrings(const Locale('zh')).uptimeShowSample),
      );
      await tester.tap(
        find
            .byTooltip(
              lookupLauncherStrings(const Locale('zh')).uptimeShowSample,
            )
            .first,
      );
      await tester.pumpAndSettle();
      final name = find.text(service.label);
      final before = tester.getRect(name);
      for (final size in [
        const Size(781, 734),
        const Size(780, 734),
        const Size(390, 844),
        const Size(1440, 734),
      ]) {
        tester.view.physicalSize = size;
        await tester.pumpAndSettle();
        expect(
          find.byTooltip(
            lookupLauncherStrings(const Locale('zh')).uptimeHideSample,
          ),
          findsOneWidget,
        );
        expect(source.requests, [UptimeRange.day]);
        expect(tester.takeException(), isNull);
      }
      expect(tester.getRect(name), before);
      await tester.pumpWidget(const SizedBox.shrink());
      semantics.dispose();
    },
    variant: TargetPlatformVariant.only(TargetPlatform.windows),
  );
}
