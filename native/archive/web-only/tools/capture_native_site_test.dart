import 'dart:async';
import 'dart:io';
import 'dart:typed_data' show ByteData;

import 'package:flutter/foundation.dart' show SynchronousFuture;
import 'package:flutter/services.dart'
    show CachingAssetBundle, MethodCall, SystemChannels, rootBundle;
import 'package:flutter_test/flutter_test.dart';
import 'package:flutter_markdown_plus/flutter_markdown_plus.dart';
import 'package:go_router/go_router.dart';
import 'package:material_ui/material_ui.dart';
import 'package:shared_preferences/shared_preferences.dart';
import 'package:shared_preferences_platform_interface/in_memory_shared_preferences_async.dart';
import 'package:shared_preferences_platform_interface/shared_preferences_async_platform_interface.dart';
import 'package:touhouonline/catalog.dart';
import 'package:touhouonline/game_launcher.dart';
import 'package:touhouonline/game_preferences.dart';
import 'package:touhouonline/library.dart';
import 'package:touhouonline/l10n/strings.dart';
import 'package:touhouonline/main.dart';
import 'package:touhouonline/site_documents.dart';
import 'package:touhouonline/site_navigation.dart';
import 'package:touhouonline/uptime_page.dart';

import 'capture_native_baseline_test.dart' show loadFonts, save;

class _Network extends HttpOverrides {}

class _DocumentAssets extends CachingAssetBundle {
  _DocumentAssets(this.contents);

  final Map<String, String> contents;

  @override
  Future<ByteData> load(String key) => rootBundle.load(key);

  @override
  Future<String> loadString(String key, {bool cache = true}) =>
      contents.containsKey(key)
      ? SynchronousFuture(contents[key]!)
      : rootBundle.loadString(key, cache: cache);
}

Future<Map<UptimeRange, UptimeSummary>> _officialStatus() =>
    HttpOverrides.runWithHttpOverrides(() async {
      final ranges = <UptimeRange, UptimeSummary>{};
      for (final range in UptimeRange.values) {
        final result = await UptimeSummary.read(range);
        expect(result.range, range);
        expect(result.services, isNotEmpty);
        expect(
          result.services.any((service) => service.points.isNotEmpty),
          isTrue,
        );
        ranges[range] = result;
      }
      return ranges;
    }, _Network());

Future<GoRouter> _app(
  WidgetTester tester,
  Size size,
  GlobalKey boundary,
  UptimeSource read, {
  AssetBundle? bundle,
  LauncherLanguage language = LauncherLanguage.simplifiedChinese,
}) async {
  tester.view.physicalSize = size;
  tester.view.devicePixelRatio = 1;
  addTearDown(tester.view.resetPhysicalSize);
  addTearDown(tester.view.resetDevicePixelRatio);
  SharedPreferencesAsyncPlatform.instance =
      InMemorySharedPreferencesAsync.empty();
  final storage = SharedPreferencesAsync();
  final games = (await tester.runAsync(Game.load))!;
  final favorites = GameFavorites(storage, {});
  await tester.pumpWidget(
    RepaintBoundary(
      key: boundary,
      child: DefaultAssetBundle(
        bundle: bundle ?? rootBundle,
        child: LauncherApp(
          book: BookState(games, favorites),
          lobby: BookState(games, favorites),
          preferences: LauncherPreferences(
            storage,
            ThemeMode.dark,
            true,
            language: language,
          ),
          launcher: const NativeGamePlaceholder(),
          gamePreferences: GamePreferenceStore(storage),
          uptimeSource: read,
        ),
      ),
    ),
  );
  await tester.pumpAndSettle();
  return GoRouter.of(tester.element(find.byType(LibraryPage)));
}

Finder _within<T extends Widget>(Finder text) =>
    find.descendant(of: find.byType(T), matching: text);

void main() {
  final output = Directory(Platform.environment['TOUHOU_SITE_CAPTURE']!);
  final sdkFonts = Directory(Platform.environment['TOUHOU_CAPTURE_FONTS']!);
  final chinese = lookupLauncherStrings(const Locale('zh'));

  for (final (device, size) in [
    ('desktop', const Size(1440, 734)),
    ('mobile', const Size(390, 844)),
    ('mobile-narrow', const Size(320, 640)),
  ]) {
    testWidgets('Site resources through launcher entry $device', (
      tester,
    ) async {
      final source = (await tester.runAsync(_officialStatus))!;
      final docs = (await tester.runAsync(() async {
        await loadFonts(sdkFonts);
        return _DocumentAssets({
          for (final key in [
            'site.zh-CN.json',
            'terms.zh-CN.md',
            'privacy.zh-CN.md',
          ])
            'assets/documents/$key': await rootBundle.loadString(
              'assets/documents/$key',
              cache: false,
            ),
        });
      }))!;
      final boundary = GlobalKey();
      final router = await _app(
        tester,
        size,
        boundary,
        (range) async => source[range]!,
        bundle: docs,
      );
      final compact = device != 'desktop';
      Future<void> menu() async {
        if (compact) {
          await tester.tap(find.byType(DrawerButton));
        } else {
          await tester.tap(find.byTooltip('站点菜单'));
        }
        await tester.pumpAndSettle();
      }

      Future<void> open(String label) async {
        await menu();
        final finder = compact
            ? _within<SiteDrawer>(find.text(label))
            : find.text(label);
        await tester.ensureVisible(finder);
        await tester.pumpAndSettle();
        await tester.tap(finder);
        await tester.pumpAndSettle();
      }

      Future<void> back() async {
        router.pop();
        await tester.pumpAndSettle();
        expect(tester.takeException(), isNull);
      }

      Future<void> capture(String screen) =>
          save(tester, boundary, output, '$device-$screen');
      String? documentPath() => tester
          .widget<SiteDocumentPage>(find.byType(SiteDocumentPage))
          .address
          .path;

      if (compact) {
        await menu();
        expect(_within<SiteDrawer>(find.text('常见问题')), findsOneWidget);
        expect(_within<SiteDrawer>(find.text('登录')), findsOneWidget);
        await capture('library-drawer');
        await tester.tap(_within<SiteDrawer>(find.text('注册')));
        await tester.pumpAndSettle();
        expect(find.text('注册账号'), findsOneWidget);
        await back();
      }
      await tester.enterText(find.byType(EditableText).first, 'th06');
      await tester.pumpAndSettle();
      await open('常见问题');
      expect(documentPath(), faqPath);
      expect(find.byType(SiteDocumentPage), findsOneWidget);
      expect(find.textContaining('游戏闪退'), findsWidgets);
      await capture('site-faq');
      await back();
      expect(find.text('th06'), findsWidgets);
      if (compact) {
        expect(
          _within<SiteDrawer>(find.text('常见问题')).hitTestable(),
          findsNothing,
        );
      }

      await open('关于本站');
      expect(find.text('贡献方式'), findsOneWidget);
      await capture('site-about');
      await back();

      await open('支持我们');
      expect(find.text('捐赠以支持服务器运行'), findsOneWidget);
      expect(find.bySemanticsLabel('Tenko 的赞赏码'), findsOneWidget);
      if (!compact) await capture('site-donation');
      await tester.tap(find.widgetWithText(TextButton, '关闭'));
      await tester.pumpAndSettle();
      expect(find.byType(LibraryPage), findsOneWidget);

      await open('服务状态');
      expect(find.byType(UptimePage), findsOneWidget);
      expect(
        find.text(source[UptimeRange.day]!.services.first.label),
        findsOneWidget,
      );
      await capture('site-uptime');
      await tester.tap(find.text('7 天'));
      await tester.pumpAndSettle();
      expect(find.text(chinese.uptimeFailed), findsNothing);
      await tester.tap(find.byTooltip('查看历史采样').first);
      await tester.pumpAndSettle();
      final previous = find.byTooltip('上一条采样');
      expect(previous, findsOneWidget);
      await tester.tap(previous);
      await tester.pumpAndSettle();
      await tester.tap(find.text('90 天'));
      await tester.pumpAndSettle();
      expect(find.text(chinese.uptimeFailed), findsNothing);
      await back();

      router.go('/lobby');
      await tester.pumpAndSettle();
      await tester.tap(find.text('联机玩法介绍'));
      await tester.pumpAndSettle();
      expect(documentPath(), multiplayerGuidePath);
      expect(find.text('协作'), findsOneWidget);
      await capture('site-guide');
      if (compact) {
        await tester.tap(find.byTooltip('切换联机玩法文档'));
        await tester.pumpAndSettle();
      }
      final coop = find.widgetWithText(
        compact ? PopupMenuItem<String> : ListTile,
        '协作通用规则',
      );
      await tester.tap(coop);
      await tester.pumpAndSettle();
      expect(documentPath(), '$multiplayerGuidePath/coop');
      await capture('site-guide-coop');
      await tester.tap(find.byTooltip('本文目录'));
      await tester.pumpAndSettle();
      final section = find.widgetWithText(PopupMenuItem<String>, '残机转移');
      await tester.ensureVisible(section);
      await tester.pumpAndSettle();
      await tester.tap(section);
      await tester.pumpAndSettle();
      expect(find.text('残机转移').hitTestable(), findsWidgets);
      await back();
      expect(documentPath(), multiplayerGuidePath);
      await back();
      expect(router.routeInformationProvider.value.uri.path, '/lobby');
      expect(find.text('联机玩法介绍'), findsOneWidget);

      for (final topic in [
        'th06',
        'th07',
        'th08',
        'th10',
        'th11',
        'th15',
        'th09',
        'th19',
      ]) {
        router.push('$multiplayerGuidePath/$topic');
        await tester.pumpAndSettle();
        expect(find.text('找不到该文档'), findsNothing);
        await capture('site-$topic');
        await back();
      }
      for (final (screen, path) in [
        ('site-terms', termsPath),
        ('site-privacy', privacyPath),
        ('site-first-use', firstUsePath),
        ('site-progress', '$faqPath?view=progress'),
      ]) {
        router.push(path);
        await tester.pumpAndSettle();
        expect(find.text('找不到该文档'), findsNothing);
        await capture(screen);
        await back();
      }
    });
  }

  final localeLocation = Platform.environment['TOUHOU_SITE_LOCALE_CAPTURE'];
  if (localeLocation != null) {
    final localized = Directory(localeLocation);
    for (final language in [
      LauncherLanguage.english,
      LauncherLanguage.japanese,
    ]) {
      final strings = lookupLauncherStrings(language.locale);
      for (final (device, size) in [
        ('desktop', const Size(1440, 734)),
        ('mobile', const Size(390, 844)),
        ('mobile-narrow', const Size(320, 640)),
      ]) {
        testWidgets('Localized help pages |${language.tag} | $device', (
          tester,
        ) async {
          final key = 'assets/documents/site.${language.tag}.json';
          final assets = (await tester.runAsync(() async {
            await localized.create(recursive: true);
            await loadFonts(sdkFonts);
            return _DocumentAssets({
              key: await rootBundle.loadString(key, cache: false),
            });
          }))!;
          final frame = GlobalKey();
          final router = await _app(
            tester,
            size,
            frame,
            (_) async => throw StateError('Unexpected status request'),
            bundle: assets,
            language: language,
          );

          Future<void> snapshot(String name) async {
            expect(find.byType(SiteDocumentPage), findsOneWidget);
            expect(find.text(strings.documentMissing), findsNothing);
            expect(find.text(strings.retryDocument), findsNothing);
            expect(tester.takeException(), isNull);
            await save(
              tester,
              frame,
              localized,
              '$device-$name-${language.tag}',
            );
          }

          final mobile = device != 'desktop';
          if (mobile) {
            await tester.tap(find.byType(DrawerButton));
            await tester.pumpAndSettle();
            await tester.tap(_within<SiteDrawer>(find.text(strings.faq)));
          } else {
            await tester.tap(find.byTooltip(strings.faq));
          }
          await tester.pumpAndSettle();
          expect(
            tester
                .widget<SiteDocumentPage>(find.byType(SiteDocumentPage))
                .address
                .path,
            faqPath,
          );
          expect(
            find.byWidgetPredicate(
              (widget) =>
                  widget is MarkdownBody &&
                  widget.data.contains('/compatibility.${language.tag}.html'),
            ),
            findsOneWidget,
          );
          expect(
            find.byWidgetPredicate(
              (widget) =>
                  widget is Icon &&
                  widget.semanticLabel == strings.documentNote,
            ),
            findsOneWidget,
          );
          expect(
            find.byWidgetPredicate(
              (widget) =>
                  widget is Icon &&
                  widget.semanticLabel == strings.documentWarning,
            ),
            findsOneWidget,
          );
          expect(
            find.byWidgetPredicate(
              (widget) => widget is MarkdownBody && widget.data.contains('[!'),
            ),
            findsNothing,
          );
          await snapshot('site-faq');
          final toc = find.byTooltip(strings.documentToc);
          await tester.tap(toc);
          await tester.pumpAndSettle();
          final first = find.byType(PopupMenuItem<String>).first;
          await tester.tap(first);
          await tester.pumpAndSettle();
          final heading = find.textContaining(
            language == LauncherLanguage.english
                ? 'What should I do about crashes'
                : '強制終了、処理落ち',
          );
          expect(heading.hitTestable(), findsOneWidget);
          expect(find.text(strings.documentModified), findsOneWidget);
          final next = find.text(strings.documentNext);
          await tester.ensureVisible(next);
          await tester.pumpAndSettle();
          await tester.tap(next);
          await tester.pumpAndSettle();
          await snapshot('site-guide');
          expect(
            tester
                .widget<SiteDocumentPage>(find.byType(SiteDocumentPage))
                .address
                .path,
            multiplayerGuidePath,
          );
          await tester.tap(find.byType(BackButton));
          await tester.pumpAndSettle();
          expect(
            tester
                .widget<SiteDocumentPage>(find.byType(SiteDocumentPage))
                .address
                .path,
            faqPath,
          );
          await tester.tap(find.byType(BackButton));
          await tester.pumpAndSettle();
          expect(find.byType(LibraryPage), findsOneWidget);

          for (final (name, path) in [
            ('site-about', aboutPath),
            ('site-first-use', firstUsePath),
            ('site-guide-coop', '$multiplayerGuidePath/coop'),
            for (final game in [
              'th06',
              'th07',
              'th08',
              'th10',
              'th11',
              'th15',
              'th09',
              'th19',
            ])
              ('site-$game', '$multiplayerGuidePath/$game'),
          ]) {
            router.push(path);
            await tester.pumpAndSettle();
            await snapshot(name);
            if (name == 'site-guide-coop') {
              final cell = find.byWidgetPredicate(
                (widget) =>
                    widget is MarkdownBody &&
                    widget.data.contains('#life-transfer'),
              );
              expect(cell, findsOneWidget);
              final target = find.text(
                language == LauncherLanguage.english
                    ? 'Life transfer'
                    : '残機の譲渡',
              );
              await tester.ensureVisible(cell);
              await tester.pumpAndSettle();
              expect(target.hitTestable(), findsNothing);
              await tester.tapAt(
                tester.getTopLeft(cell) + const Offset(10, 10),
              );
              await tester.pumpAndSettle();
              expect(target.hitTestable(), findsOneWidget);
            }
            router.pop();
            await tester.pumpAndSettle();
          }
        });
      }
    }

    testWidgets('A localized document link does not change launcher language', (
      tester,
    ) async {
      final docs = (await tester.runAsync(() async {
        final key = 'assets/documents/site.ja.json';
        return _DocumentAssets({key: await rootBundle.loadString(key)});
      }))!;
      final router = await _app(
        tester,
        const Size(390, 844),
        GlobalKey(),
        (_) async => throw StateError('Unexpected status request'),
        bundle: docs,
        language: LauncherLanguage.english,
      );
      router.push('$multiplayerGuidePath/coop?lang=ja#life-transfer');
      await tester.pumpAndSettle();
      expect(find.text('残機の譲渡').hitTestable(), findsWidgets);
      await tester.tap(
        find.byTooltip(
          lookupLauncherStrings(const Locale('ja')).guideNavigation,
        ),
      );
      await tester.pumpAndSettle();
      final work = find.widgetWithText(PopupMenuItem<String>, '妖々夢');
      await tester.ensureVisible(work);
      await tester.tap(work);
      await tester.pumpAndSettle();
      final address = tester
          .widget<SiteDocumentPage>(find.byType(SiteDocumentPage))
          .address;
      expect(address.path, '$multiplayerGuidePath/th07');
      expect(address.queryParameters['lang'], 'ja');
      final systemBack = SystemChannels.navigation.codec.encodeMethodCall(
        const MethodCall('popRoute'),
      );
      final messenger = tester.binding.defaultBinaryMessenger;
      await messenger.handlePlatformMessage(
        SystemChannels.navigation.name,
        systemBack,
        null,
      );
      await tester.pumpAndSettle();
      expect(find.text('残機の譲渡').hitTestable(), findsWidgets);
      await messenger.handlePlatformMessage(
        SystemChannels.navigation.name,
        systemBack,
        null,
      );
      await tester.pumpAndSettle();
      expect(find.text('All games'), findsWidgets);
    });
  }

  Map<UptimeRange, UptimeSummary>? statusSamples;
  for (final language in [
    LauncherLanguage.english,
    LauncherLanguage.japanese,
  ]) {
    final strings = lookupLauncherStrings(language.locale);
    for (final (device, size) in [
      ('desktop', const Size(1440, 734)),
      ('mobile', const Size(390, 844)),
      ('mobile-narrow', const Size(320, 640)),
    ]) {
      testWidgets('Localized service status | ${language.tag} | $device', (
        tester,
      ) async {
        final source = (statusSamples ??= (await tester.runAsync(() async {
          await loadFonts(sdkFonts);
          return _officialStatus();
        }))!);
        final frame = GlobalKey();
        var fail = false;
        final router = await _app(tester, size, frame, (range) async {
          if (fail) throw StateError('unavailable');
          return source[range]!;
        }, language: language);
        final mobile = device != 'desktop';
        await tester.tap(
          mobile ? find.byType(DrawerButton) : find.byTooltip(strings.siteMenu),
        );
        await tester.pumpAndSettle();
        final link = mobile
            ? _within<SiteDrawer>(find.text(strings.serviceStatus))
            : find.text(strings.serviceStatus);
        await tester.ensureVisible(link);
        await tester.pumpAndSettle();
        await tester.tap(link);
        await tester.pumpAndSettle();
        final title = find.widgetWithText(AppBar, strings.uptimeTitle);
        expect(title, findsOneWidget);
        expect(find.text(strings.uptimeSite), findsOneWidget);
        expect(
          find.text(source[UptimeRange.day]!.services.first.label),
          findsOneWidget,
        );
        expect(find.byTooltip(strings.uptimeAvailabilityHint), findsWidgets);
        expect(
          tester.getSize(find.byType(SegmentedButton<UptimeRange>)).width,
          lessThanOrEqualTo(360),
        );
        expect(tester.takeException(), isNull);
        final target = Directory(localeLocation ?? output.path);
        await tester.runAsync(() => target.create(recursive: true));
        await save(
          tester,
          frame,
          target,
          '$device-site-uptime-${language.tag}',
        );

        await tester.tap(find.text(strings.uptimeWeek));
        await tester.pumpAndSettle();
        expect(
          tester
              .widget<SegmentedButton<UptimeRange>>(
                find.byType(SegmentedButton<UptimeRange>),
              )
              .selected,
          {UptimeRange.week},
        );
        await tester.tap(find.byTooltip(strings.uptimeShowSample).first);
        await tester.pumpAndSettle();
        await tester.tap(find.byTooltip(strings.uptimePreviousSample));
        await tester.pumpAndSettle();
        expect(find.byTooltip(strings.uptimeNextSample), findsOneWidget);
        fail = true;
        await tester.tap(find.byTooltip(strings.uptimeRefresh));
        await tester.pumpAndSettle();
        expect(find.text(strings.uptimeFailed), findsOneWidget);
        expect(find.text(strings.uptimeUnknown), findsWidgets);
        expect(find.text(strings.uptimeSite), findsOneWidget);
        fail = false;
        await tester.tap(find.widgetWithText(TextButton, strings.uptimeRetry));
        await tester.pumpAndSettle();
        expect(find.text(strings.uptimeFailed), findsNothing);
        router.pop();
        await tester.pumpAndSettle();
        final library = find.byType(LibraryPage);
        expect(library.hitTestable(), findsOneWidget);

        if (device == 'mobile-narrow') {
          final override = language == LauncherLanguage.english
              ? LauncherLanguage.japanese
              : LauncherLanguage.english;
          final local = lookupLauncherStrings(override.locale);
          unawaited(
            openSiteLink(
              tester.element(library),
              'https://touhouonline.com/uptime/index.html?lang=${override.tag}',
            ),
          );
          await tester.pumpAndSettle();
          expect(
            find.widgetWithText(AppBar, local.uptimeTitle),
            findsOneWidget,
          );
          expect(
            Localizations.localeOf(tester.element(find.byType(UptimePage))),
            override.locale,
          );
          router.pop();
          await tester.pumpAndSettle();
          expect(
            Localizations.localeOf(tester.element(library)),
            language.locale,
          );
        }
        expect(tester.takeException(), isNull);
      });
    }
  }

  testWidgets(
    'Only the active status range wins and a failed refresh keeps the list',
    (tester) async {
      final requests = <({UptimeRange range, Completer<UptimeSummary> load})>[];
      final boundary = GlobalKey();
      final router = await _app(tester, const Size(320, 640), boundary, (
        range,
      ) {
        final load = Completer<UptimeSummary>();
        requests.add((range: range, load: load));
        return load.future;
      });
      final now = DateTime.now();
      UptimeSummary status(UptimeRange range, String label) =>
          UptimeSummary(range, now, [
            (
              id: 'site',
              label: label,
              group: UptimeGroup.site,
              status: 'operational',
              checkedAt: now,
              latency: 22,
              availability: 1,
              points: const [],
            ),
          ]);
      router.push(uptimePath);
      await tester.pump();
      await tester.pump(const Duration(milliseconds: 300));
      expect(find.text(chinese.uptimeLoading), findsOneWidget);
      await tester.tap(find.text('7 天'));
      await tester.pump();
      requests.last.load.complete(status(UptimeRange.week, '本周服务'));
      await tester.pumpAndSettle();
      requests.first.load.complete(status(UptimeRange.day, '延迟返回的服务'));
      await tester.pumpAndSettle();
      expect(find.text('本周服务'), findsOneWidget);
      expect(find.text('延迟返回的服务'), findsNothing);
      await tester.tap(find.byTooltip('刷新服务状态'));
      await tester.pump();
      requests.last.load.completeError(StateError('unavailable'));
      await tester.pumpAndSettle();
      expect(find.text(chinese.uptimeFailed), findsOneWidget);
      expect(find.text('本周服务'), findsOneWidget);
      await tester.tap(find.widgetWithText(TextButton, '重试'));
      await tester.pump();
      requests.last.load.complete(
        UptimeSummary(UptimeRange.week, now, const []),
      );
      await tester.pumpAndSettle();
      expect(find.text(chinese.uptimeEmpty), findsOneWidget);
      router.pop();
      await tester.pumpAndSettle();
      expect(tester.takeException(), isNull);
    },
  );
}
