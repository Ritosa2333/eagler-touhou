import 'dart:convert';
import 'dart:io';
import 'dart:ui' as ui;

import 'package:flutter/rendering.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:integration_test/integration_test.dart';
import 'package:material_ui/material_ui.dart';
import 'package:go_router/go_router.dart';
import 'package:flutter_inappwebview/flutter_inappwebview.dart' as web;
import 'package:path_provider_platform_interface/path_provider_platform_interface.dart';
import 'package:shared_preferences/shared_preferences.dart';
import 'package:shared_preferences_platform_interface/in_memory_shared_preferences_async.dart';
import 'package:shared_preferences_platform_interface/shared_preferences_async_platform_interface.dart';
import 'package:window_manager/window_manager.dart' as windows;
import 'package:touhouonline/account_api.dart';
import 'package:touhouonline/account_web_page.dart';
import 'package:touhouonline/catalog.dart';
import 'package:touhouonline/desktop_window.dart';
import 'package:touhouonline/game_detail.dart';
import 'package:touhouonline/game_launcher.dart';
import 'package:touhouonline/game_preferences.dart';
import 'package:touhouonline/library.dart';
import 'package:touhouonline/main.dart';

const output = 'D:/Temp/native-performance-20261128/navigation';

class _Paths extends PathProviderPlatform {
  @override
  Future<String?> getApplicationSupportPath() async =>
      '$output/isolated-profile';
  @override
  Future<String?> getTemporaryPath() async => '$output/isolated-cache';
}

void main() {
  IntegrationTestWidgetsFlutterBinding.ensureInitialized()
    ..framePolicy = LiveTestWidgetsFlutterBindingFramePolicy.fullyLive;
  final report = <String, Object?>{
    'scope': 'Real Windows engine and production LauncherApp | isolated guest settings and WebView data | no credentials or global input',
    'pid': pid,
    'states': <Object?>[],
  };
  void save() => File('$output/live-result.json')
      .writeAsStringSync(const JsonEncoder.withIndent('  ').convert(report));
  testWidgets(
    'Production account entry survives transient history and resize',
    (tester) async {
      PathProviderPlatform.instance = _Paths();
      SharedPreferencesAsyncPlatform.instance =
          InMemorySharedPreferencesAsync.empty();
      initializeDesktopWindow();
      final window = windows.WindowManager.instance.getCurrent()!;
      window.position = const Offset(-4000, 0).toNative();
      window.setSize(const Size(1280, 720).toNative(), false);
      final storage = SharedPreferencesAsync();
      final games = await Game.load();
      final favorites = GameFavorites(storage, {});
      await tester.pumpWidget(
        LauncherApp(
          book: BookState(games, favorites),
          lobby: BookState(games, favorites),
          preferences: LauncherPreferences(storage, ThemeMode.dark, false),
          launcher: const NativeGamePlaceholder(),
          gamePreferences: GamePreferenceStore(storage),
          accountApi: AccountApi(),
        ),
      );
      Future<void> wait(int ms) async {
        await tester.runAsync(
          () => Future<void>.delayed(Duration(milliseconds: ms)),
        );
        await tester.pump();
      }

      Future<void> capture(String name) async {
        final boundary = tester
            .renderObjectList<RenderRepaintBoundary>(
              find.byType(RepaintBoundary),
            )
            .where((b) => b.attached && b.hasSize)
            .reduce(
              (a, b) =>
                  a.size.width * a.size.height >= b.size.width * b.size.height
                  ? a
                  : b,
            );
        final image = await boundary.toImage(pixelRatio: 1);
        final bytes = await image.toByteData(format: ui.ImageByteFormat.png);
        await File('$output/$name.png')
            .writeAsBytes(bytes!.buffer.asUint8List());
        image.dispose();
      }

      try {
        await tester.pumpAndSettle();
        final router = GoRouter.of(tester.element(find.byType(LibraryPage)));
        router.push('/game/th08');
        await tester.pumpAndSettle();
        expect(router.state.uri.path, '/game/th08');
        expect(tester.widget<GameDetail>(find.byType(GameDetail)).game.id, 'th08');
        final headingBefore = tester.getRect(find.byType(GameHeading));
        for (var cycle = 0; cycle < 2; cycle++) {
          await tester.tap(find.byTooltip('登录').first);
          final deadline = DateTime.now().add(const Duration(seconds: 40));
          while (DateTime.now().isBefore(deadline)) {
            await wait(100);
            if (find.byType(AccountWebPage).evaluate().isEmpty)
              throw StateError('Account exited during loading');
            if (find.byType(web.InAppWebView).evaluate().isNotEmpty &&
                find.descendant(of: find.byType(AccountWebPage), matching: find.byType(Texture)).evaluate().isNotEmpty &&
                find.byType(LinearProgressIndicator).evaluate().isEmpty &&
                find.byType(CircularProgressIndicator).evaluate().isEmpty)
              break;
          }
          expect(find.byType(web.InAppWebView), findsOneWidget);
          expect(find.byType(LinearProgressIndicator), findsNothing);
          expect(find.byType(CircularProgressIndicator), findsNothing);
          expect(find.descendant(of: find.byType(AccountWebPage), matching: find.byType(Texture)), findsOneWidget);
          for (final width in [1280.0, 390.0, 320.0, 1280.0]) {
            window.setSize(Size(width, 720).toNative(), false);
            await wait(1500);
            expect(
              find.byType(AccountWebPage),
              findsOneWidget,
              reason: 'Account retained at $width',
            );
            expect(tester.takeException(), isNull);
            (report['states']! as List).add({
              'cycle': cycle,
              'width': width,
              'route': router.state.uri.path,
              'webViews': find.byType(web.InAppWebView).evaluate().length,
            });
            save();
            if (cycle == 0) await capture('live-login-${width.toInt()}');
          }
          await tester.tap(
            find.descendant(
              of: find.byType(AccountWebPage),
              matching: cycle == 0
                  ? find.byType(BackButton)
                  : find.byIcon(Icons.close),
            ),
          );
          await tester.pumpAndSettle();
          expect(find.byType(AccountWebPage), findsNothing);
          expect(router.state.uri.path, '/game/th08');
          expect(tester.widget<GameDetail>(find.byType(GameDetail)).game.id, 'th08');
          expect(tester.getRect(find.byType(GameHeading)), headingBefore);
        }
        report['status'] = 'passed';
      } catch (error, stack) {
        report['status'] = 'failed';
        report['error'] = '$error';
        report['stack'] = '$stack';
        rethrow;
      } finally {
        save();
      }
    },
  );
}
