import 'dart:convert';
import 'dart:io';
import 'dart:math' as math;

import 'package:flutter/foundation.dart';
import 'package:flutter/services.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:material_ui/material_ui.dart';
import 'package:go_router/go_router.dart';
import 'package:shared_preferences/shared_preferences.dart';
import 'package:shared_preferences_platform_interface/in_memory_shared_preferences_async.dart';
import 'package:shared_preferences_platform_interface/shared_preferences_async_platform_interface.dart';
import 'package:touhouonline/account_api.dart';
import 'package:touhouonline/catalog.dart';
import 'package:touhouonline/l10n/strings.dart';
import 'package:touhouonline/launcher_language.dart';
import 'package:touhouonline/profile_data.dart';
import 'package:touhouonline/site_navigation.dart';

import '../tools/capture_native_baseline_test.dart' show loadFonts, save;
import 'widget_test.dart' as launcher;

class _DrawerApi extends launcher.FixtureAccountApi {
  _DrawerApi(this.member);
  final bool member;
  @override
  bool get canRestoreSession => member;
  @override
  Future<AccountIdentity?> restoreSession() async {
    signedIn = true;
    return (username: 'fixture', name: '测试玩家', expiresAt: null);
  }

  @override
  Future<PublicProfile> profile(String username, {bool fresh = false}) async =>
      PublicProfile(username: username, nickname: '测试玩家', country: 'CN');
}

void main() {
  final capture = Platform.environment['TOUHOU_DRAWER_CAPTURE'];
  late List<Game> games;
  setUpAll(() async {
    await loadFonts();
    games = await Game.load();
  });
  for (final (device, size, language) in [
    ('320-en', const Size(320, 640), LauncherLanguage.english),
    ('390-ja', const Size(390, 844), LauncherLanguage.japanese),
    ('390-zh', const Size(390, 844), LauncherLanguage.simplifiedChinese),
  ]) {
    for (final light in [false, true]) {
      for (final member in [false, true]) {
        for (final scale in [1.0, 2.0]) {
          final name =
              '$device-${light ? 'light' : 'dark'}-'
              '${member ? 'member' : 'guest'}-${scale.toInt()}';
          testWidgets('drawer layout and return | $name', (tester) async {
            debugDefaultTargetPlatformOverride = TargetPlatform.windows;
            addTearDown(() => debugDefaultTargetPlatformOverride = null);
            tester.view.devicePixelRatio = 1;
            tester.view.physicalSize = size;
            addTearDown(tester.view.resetDevicePixelRatio);
            addTearDown(tester.view.resetPhysicalSize);
            tester.platformDispatcher.textScaleFactorTestValue = scale;
            addTearDown(
              tester.platformDispatcher.clearTextScaleFactorTestValue,
            );
            SharedPreferencesAsyncPlatform.instance =
                InMemorySharedPreferencesAsync.empty();
            final prefs = LauncherPreferences(
              SharedPreferencesAsync(),
              light ? ThemeMode.light : ThemeMode.dark,
              true,
              language: language,
            );
            final launched = <Uri>[];
            const channel = MethodChannel('plugins.flutter.io/url_launcher');
            final messenger = tester.binding.defaultBinaryMessenger;
            messenger.setMockMethodCallHandler(channel, (call) async {
              launched.add(Uri.parse((call.arguments as Map)['url'] as String));
              return true;
            });
            addTearDown(
              () => messenger.setMockMethodCallHandler(channel, null),
            );
            final strings = lookupLauncherStrings(language.locale);
            final boundary = GlobalKey();
            await tester.pumpWidget(
              RepaintBoundary(
                key: boundary,
                child: launcher.fixture(
                  games: games,
                  accountApi: _DrawerApi(member),
                  preferences: prefs,
                ),
              ),
            );
            await tester.pumpAndSettle();
            await tester.runAsync(() async {
              final context = tester.element(find.byType(SearchBar));
              await precacheImage(
                AssetImage(
                  'assets/launcher-wordmark-'
                  '${light ? 'light' : 'dark'}.png',
                ),
                context,
              );
            });
            final search = find.byType(SearchBar);
            await tester.enterText(search, 'th06');
            await tester.testTextInput.receiveAction(TextInputAction.done);
            await tester.tap(find.byType(DrawerButton));
            await tester.pumpAndSettle();
            final drawer = find.byType(SiteDrawer);
            final box = tester.getRect(
              find.descendant(of: drawer, matching: find.byType(Drawer)),
            );
            final faq = find.descendant(
              of: drawer,
              matching: find.text(strings.faq),
            );
            final textBox = tester.getRect(faq);
            if (capture != null) {
              await save(tester, boundary, Directory(capture), '$name-main');
              File('$capture/$name.json').writeAsStringSync(
                jsonEncode({
                  'drawer': [box.left, box.top, box.width, box.height],
                  'faq': [
                    textBox.left,
                    textBox.top,
                    textBox.width,
                    textBox.height,
                  ],
                }),
              );
            }
            final languageRow = find.descendant(
              of: drawer,
              matching: find.text(strings.language),
            );
            await tester.scrollUntilVisible(
              languageRow,
              150,
              scrollable: find
                  .descendant(of: drawer, matching: find.byType(Scrollable))
                  .first,
            );
            await tester.pumpAndSettle();
            await tester.tap(languageRow);
            await tester.pumpAndSettle();
            expect(find.byType(LauncherLanguageRow), findsOneWidget);
            if (capture != null) {
              await save(
                tester,
                boundary,
                Directory(capture),
                '$name-language',
              );
            }
            await tester.binding.handlePopRoute();
            await tester.pumpAndSettle();
            expect(find.byType(LauncherLanguageRow), findsNothing);
            final about = find.descendant(
              of: drawer,
              matching: find.text(strings.about),
            );
            await tester.scrollUntilVisible(
              about,
              150,
              scrollable: find
                  .descendant(of: drawer, matching: find.byType(Scrollable))
                  .first,
            );
            await tester.tap(about);
            await tester.pumpAndSettle();
            final terms = find.descendant(
              of: drawer,
              matching: find.text(strings.terms),
            );
            expect(terms, findsOneWidget);
            if (capture != null) {
              await save(tester, boundary, Directory(capture), '$name-about');
            }
            if (scale == 1) {
              await tester.tap(
                find.descendant(
                  of: drawer,
                  matching: find.text(strings.siteInformation),
                ),
              );
              await tester.pumpAndSettle();
              expect(launched.single.path, '/about.html');
              expect(launched.single.host, 'touhouonline.com');
              expect(
                launched.single.queryParameters['lang'],
                bundledLanguage(language.locale),
              );
              final router = GoRouter.of(
                tester.element(find.byType(Scaffold).last),
              );
              expect(router.state.uri.path, '/');
              expect(tester.widget<SearchBar>(search).controller!.text, 'th06');
              await tester.tap(find.byType(DrawerButton));
              await tester.pumpAndSettle();
            } else {
              await tester.binding.handlePopRoute();
              await tester.pumpAndSettle();
            }
            expect(terms, findsNothing);
            await tester.tap(find.byTooltip(strings.closeMenu));
            await tester.pumpAndSettle();
            expect(tester.widget<SearchBar>(search).controller!.text, 'th06');
            expect(tester.takeException(), isNull);
            expect(box.width, math.min(292, size.width - 32));
            expect(textBox.left, closeTo(64, .5));
          });
        }
      }
    }
  }
}
