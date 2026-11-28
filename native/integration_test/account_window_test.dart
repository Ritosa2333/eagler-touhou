import 'dart:async';
import 'dart:convert';
import 'dart:ffi' as ffi;
import 'dart:io';
import 'dart:ui' as ui;

import 'package:flutter_test/flutter_test.dart';
import 'package:flutter_inappwebview/flutter_inappwebview.dart' as web;
import 'package:integration_test/integration_test.dart';
import 'package:go_router/go_router.dart';
import 'package:material_ui/material_ui.dart';
import 'package:window_manager/window_manager.dart' as windows;
import 'package:touhouonline/account_web_page.dart';
import 'package:touhouonline/game_detail.dart';
import 'package:touhouonline/library.dart';
import 'package:touhouonline/l10n/strings.dart';
import 'package:touhouonline/main.dart' as app;
import 'package:touhouonline/site_documents.dart';

// The diagnostic host supplies only its own PID and HWND. Ordinary CI uses
// window_manager; the isolated host resizes with the Win32 no-activation flag.
void _resize(windows.Window window, Size size) {
  final target = Platform.environment['TOUHOU_DESKTOP_TEST_WINDOW'];
  if (target == null) {
    window.setSize(size.toNative(), false);
    return;
  }
  final parts = target.split(':');
  if (parts.length != 2 || int.tryParse(parts[0]) != pid) {
    throw StateError('Desktop test window must belong to this process');
  }
  final hwnd = int.parse(parts[1]);
  final user32 = ffi.DynamicLibrary.open('user32.dll');
  final dpi = user32
      .lookupFunction<ffi.Uint32 Function(ffi.IntPtr), int Function(int)>(
        'GetDpiForWindow',
      )(hwnd);
  if (dpi == 0) throw StateError('Desktop test HWND is unavailable');
  final setWindowPos = user32
      .lookupFunction<
        ffi.Int32 Function(
          ffi.IntPtr,
          ffi.IntPtr,
          ffi.Int32,
          ffi.Int32,
          ffi.Int32,
          ffi.Int32,
          ffi.Uint32,
        ),
        int Function(int, int, int, int, int, int, int)
      >('SetWindowPos');
  const swpNoMove = 0x0002;
  const swpNoZOrder = 0x0004;
  const swpNoActivate = 0x0010;
  if (setWindowPos(
        hwnd,
        0,
        0,
        0,
        (size.width * dpi / 96).round(),
        (size.height * dpi / 96).round(),
        swpNoMove | swpNoZOrder | swpNoActivate,
      ) ==
      0) {
    throw StateError('Desktop test resize failed');
  }
}

void main() {
  final binding = IntegrationTestWidgetsFlutterBinding.ensureInitialized();
  final cases = <Map<String, Object?>>[];
  final output = Platform.environment['TOUHOU_DESKTOP_FRAME_CAPTURE'];
  final resultPath = Platform.environment['TOUHOU_DESKTOP_TEST_RESULT'];
  if (resultPath != null) {
    unawaited(
      binding.allTestsPassed.future.then((passed) async {
        final temporary = File('$resultPath.tmp');
        await temporary.writeAsString(
          jsonEncode({
            'passed': passed,
            'scope': Platform.environment['TOUHOU_DESKTOP_TEST_SCOPE'] ?? 'all',
            'pid': pid,
            'cases': cases,
            'failures': binding.results.map(
              (name, result) => MapEntry(name, '$result'),
            ),
          }),
        );
        await temporary.rename(resultPath);
      }),
    );
  }
  testWidgets('Account webpage preserves native return across window widths', (
    tester,
  ) async {
    await app.main([]);
    await tester.pumpAndSettle();
    final window = windows.WindowManager.instance.getCurrent()!;
    final originalSize = window.size;
    addTearDown(
      () => _resize(window, Size(originalSize.width, originalSize.height)),
    );
    _resize(window, const Size(1280, 720));
    await tester.pumpAndSettle();
    final router = GoRouter.of(tester.element(find.byType(LibraryPage)));
    final strings = tester.element(find.byType(LibraryPage)).strings;
    expect(
      find.byTooltip(strings.signIn),
      findsWidgets,
      reason: 'Account document regression runs as an isolated guest',
    );
    // Exercise the real plugin and production login document. Credentials are
    // never entered; the fixture checks container navigation and native state.
    router.push('/game/th06');
    await tester.pumpAndSettle();
    final headingBefore = tester.getRect(find.byType(GameHeading));
    for (final close in [false, true]) {
      unawaited(
        openSiteLink(
          tester.element(find.byType(GameDetail)),
          signInFor('/?game=th06'),
        ),
      );
      final deadline = DateTime.now().add(const Duration(seconds: 30));
      var entered = false;
      while (DateTime.now().isBefore(deadline)) {
        await tester.pump();
        final present = find.byType(AccountWebPage).evaluate().isNotEmpty;
        if (present) entered = true;
        if (entered) {
          expect(
            present,
            isTrue,
            reason:
                'Account must stay open through initial history notifications',
          );
        }
        if (find.byType(web.InAppWebView).evaluate().isNotEmpty &&
            find.byType(LinearProgressIndicator).evaluate().isEmpty) {
          break;
        }
        await tester.runAsync(
          () => Future<void>.delayed(const Duration(milliseconds: 100)),
        );
      }
      expect(find.byType(web.InAppWebView), findsOneWidget);
      expect(find.byType(LinearProgressIndicator), findsNothing);
      final viewState = tester.state(find.byType(web.InAppWebView));
      List<int> textureIds() => tester
          .widgetList<Texture>(
            find.descendant(
              of: find.byType(AccountWebPage),
              matching: find.byType(Texture),
            ),
          )
          .map((texture) => texture.textureId)
          .toList();
      final initialTextures = textureIds();
      expect(initialTextures, isNotEmpty);
      for (final width in [1280.0, 390.0, 320.0, 1280.0]) {
        _resize(window, Size(width, 720));
        await tester.runAsync(
          () => Future<void>.delayed(const Duration(milliseconds: 600)),
        );
        await tester.pump();
        expect(find.byType(AccountWebPage), findsOneWidget);
        expect(
          identical(tester.state(find.byType(web.InAppWebView)), viewState),
          isTrue,
          reason: 'Resizing must preserve the account WebView state',
        );
        expect(
          textureIds(),
          initialTextures,
          reason: 'Resizing must preserve the native texture',
        );
        expect(tester.takeException(), isNull);
        cases.add({
          'exit': close ? 'close' : 'back',
          'width': width,
          'route': GoRouterState.of(tester.element(find.byType(AccountWebPage)))
              .uri
              .toString(),
          'windowWidth': window.size.width,
          'sameWebView': true,
          'textures': initialTextures,
        });
      }
      final exit = find.descendant(
        of: find.byType(AccountWebPage),
        matching: close ? find.byIcon(Icons.close) : find.byType(BackButton),
      );
      await tester.tap(exit);
      await tester.pumpAndSettle();
      expect(find.byType(AccountWebPage), findsNothing);
      expect(find.byType(GameDetail).hitTestable(), findsOneWidget);
      expect(
        GoRouterState.of(tester.element(find.byType(GameDetail))).uri.path,
        '/game/th06',
      );
      expect(
        tester.getRect(find.byType(GameHeading)),
        headingBefore,
        reason: 'Account exit restores the original game layout',
      );
      expect(tester.takeException(), isNull);
    }
    router.pop();
    await tester.pumpAndSettle();
    expect(find.byType(LibraryPage).hitTestable(), findsOneWidget);

    await tester.pumpWidget(const SizedBox());
    await tester.pumpAndSettle();
  }, skip: !Platform.isWindows);

  testWidgets(
    'Production account documents fit desktop and narrow windows',
    (tester) async {
      await app.main([]);
      await tester.pumpAndSettle();
      final window = windows.WindowManager.instance.getCurrent()!;
      final originalSize = window.size;
      addTearDown(
        () => _resize(window, Size(originalSize.width, originalSize.height)),
      );
      _resize(window, const Size(1280, 720));
      await tester.pumpAndSettle();
      unawaited(
        openSiteLink(tester.element(find.byType(LibraryPage)), registerPath),
      );
      final accountDeadline = DateTime.now().add(const Duration(seconds: 30));
      while (DateTime.now().isBefore(accountDeadline)) {
        await tester.pump();
        if (find.byType(web.InAppWebView).evaluate().isNotEmpty &&
            find.byType(LinearProgressIndicator).evaluate().isEmpty) {
          break;
        }
        await tester.runAsync(
          () => Future<void>.delayed(const Duration(milliseconds: 100)),
        );
      }
      expect(find.byType(web.InAppWebView), findsOneWidget);
      final accountView = tester
          .widget<web.InAppWebView>(find.byType(web.InAppWebView))
          .platform
          .params;
      // Verify the pinned plugin's single-window fallback against the real public
      // privacy anchor. No registration fields are filled or submitted.
      web.InAppWebViewController? controller;
      var privacyLoaded = false;
      final visited = <Uri>[];
      await tester.pumpWidget(
        MaterialApp(
          home: Scaffold(
            body: web.InAppWebView(
              webViewEnvironment: web.WebViewEnvironment.fromPlatform(
                platform: accountView.webViewEnvironment!,
              ),
              initialSettings: accountView.initialSettings,
              // The inherited settings enable navigation interception. This
              // standalone document fixture has no native routes to intercept.
              shouldOverrideUrlLoading: (_, _) async =>
                  web.NavigationActionPolicy.ALLOW,
              initialUrlRequest: web.URLRequest(
                url: web.WebUri(
                  'https://touhouonline.com/login.html?mode=register&lang=zh-CN',
                ),
              ),
              onWebViewCreated: (value) => controller = value,
              onLoadStop: (_, url) =>
                  privacyLoaded = url?.path == '/privacy.html',
              onUpdateVisitedHistory: (_, url, reload) {
                if (url != null) visited.add(url.uriValue);
              },
            ),
          ),
        ),
      );
      var privacyLinkFound = false;
      const privacyAnchor =
          'document.querySelector(\'a[target="_blank"][href*="privacy"]\')';
      final linkDeadline = DateTime.now().add(const Duration(seconds: 30));
      while (DateTime.now().isBefore(linkDeadline)) {
        await tester.pump();
        if (controller != null) {
          privacyLinkFound =
              await controller!.evaluateJavascript(
                source: '$privacyAnchor !== null',
              ) ==
              true;
          if (privacyLinkFound) break;
        }
        await tester.runAsync(
          () => Future<void>.delayed(const Duration(milliseconds: 100)),
        );
      }
      expect(
        privacyLinkFound,
        isTrue,
        reason: 'Production registration privacy link',
      );

      // Inspect the real document rather than equating a finished progress bar
      // with successful loading. Record only public UI metadata, never values.
      final documents = <Map<String, Object?>>[];
      Future<void> inspectDocument(String mode) async {
        final expectedFields = mode == 'register'
            ? ['username', 'nickname', 'email', 'password', 'confirmation']
            : ['identity', 'password'];
        for (final width in [1280.0, 390.0, 320.0]) {
          _resize(window, Size(width, 720));
          await tester.pumpAndSettle();
          Map<String, dynamic> document = {};
          final deadline = DateTime.now().add(const Duration(seconds: 30));
          while (DateTime.now().isBefore(deadline)) {
            await tester.pump();
            final result = await controller!.evaluateJavascript(
              source: r"""(() => {
              const form = document.querySelector('form[method="post"]');
              const fields = form ? Array.from(form.querySelectorAll('input[name]'))
                .filter(field => field.getBoundingClientRect().width > 0)
                .map(field => field.name) : [];
              return {url: location.href, title: document.title,
                ready: document.readyState, fonts: document.fonts.status,
                bootReady: Array.from(document.querySelectorAll('#eaglerPreload, #lobbyPreload')).every(element => {
                  const style = getComputedStyle(element);
                  return style.display === 'none' || style.visibility === 'hidden' ||
                    (Number(style.opacity) === 0 && style.pointerEvents === 'none');
                }),
                language: document.documentElement.lang, fields,
                viewportWidth: innerWidth, viewportHeight: innerHeight,
                contentWidth: document.documentElement.scrollWidth};
            })()""",
            );
            if (result is Map) document = Map<String, dynamic>.from(result);
            if (document['ready'] == 'complete' &&
                document['fonts'] == 'loaded' &&
                document['bootReady'] == true &&
                expectedFields.every(
                  (name) => (document['fields'] as List? ?? []).contains(name),
                )) {
              break;
            }
            await tester.runAsync(
              () => Future<void>.delayed(const Duration(milliseconds: 100)),
            );
          }
          final address = Uri.parse(document['url'] as String? ?? '');
          expect(address.host, 'touhouonline.com');
          expect(address.path, '/login.html');
          expect(address.queryParameters['mode'] ?? 'login', mode);
          expect(address.queryParameters['lang'], 'zh-CN');
          expect(document['ready'], 'complete');
          expect(document['fonts'], 'loaded');
          expect(document['bootReady'], isTrue);
          expect(document['title'], isNotEmpty);
          expect(document['fields'], containsAll(expectedFields));
          expect(
            document['contentWidth'] as num,
            lessThanOrEqualTo((document['viewportWidth'] as num) + 1),
            reason: 'Production $mode document must fit the $width px window',
          );
          final screenshot = await controller!.takeScreenshot();
          expect(screenshot, isNotNull);
          final codec = await ui.instantiateImageCodec(screenshot!);
          final frame = await codec.getNextFrame();
          documents.add({
            'mode': mode,
            'windowWidth': width,
            ...document,
            'imageWidth': frame.image.width,
            'imageHeight': frame.image.height,
          });
          frame.image.dispose();
          codec.dispose();
          if (output != null) {
            await File('$output/account-$mode-${width.toInt()}.png')
                .writeAsBytes(screenshot);
          }
        }
      }

      await inspectDocument('register');

      await controller!.evaluateJavascript(source: '$privacyAnchor.click()');
      Uri? privacyUrl;
      final privacyDeadline = DateTime.now().add(const Duration(seconds: 30));
      while (DateTime.now().isBefore(privacyDeadline)) {
        await tester.pump();
        privacyUrl = (await controller!.getUrl())?.uriValue;
        if (privacyUrl?.path == '/privacy.html' && privacyLoaded) break;
        await tester.runAsync(
          () => Future<void>.delayed(const Duration(milliseconds: 100)),
        );
      }
      expect(privacyLoaded, isTrue);
      expect(privacyUrl?.host, 'touhouonline.com');
      expect(
        privacyUrl?.path,
        '/privacy.html',
        reason: 'Popup stays in the original WebView',
      );
      expect(find.byType(web.InAppWebView), findsOneWidget);
      final privacyTitle = await controller!.getTitle();
      await controller!.evaluateJavascript(
        source: "history.pushState(null, '', '/?game=th08')",
      );
      final historyDeadline = DateTime.now().add(const Duration(seconds: 10));
      while (!visited.any((uri) => uri.queryParameters['game'] == 'th08') &&
          DateTime.now().isBefore(historyDeadline)) {
        await tester.pump();
        await tester.runAsync(
          () => Future<void>.delayed(const Duration(milliseconds: 100)),
        );
      }
      expect(
        visited.any((uri) => uri.queryParameters['game'] == 'th08'),
        isTrue,
        reason: 'Real WebView reports SPA history navigation',
      );
      expect(
        await controller!.getTitle(),
        privacyTitle,
        reason: 'History changed without replacing the loaded document',
      );

      await controller!.loadUrl(
        urlRequest: web.URLRequest(
          url: web.WebUri('https://touhouonline.com/login.html?lang=zh-CN'),
        ),
      );
      await inspectDocument('login');
      final documentOutput =
          Platform.environment['TOUHOU_DESKTOP_FRAME_CAPTURE'];
      if (documentOutput != null) {
        await File('$documentOutput/account-documents.json')
            .writeAsString(jsonEncode(documents));
      }

      expect(tester.takeException(), isNull);
      await tester.pumpWidget(const SizedBox());
      await tester.pumpAndSettle();
    },
    skip:
        !Platform.isWindows ||
        Platform.environment['TOUHOU_DESKTOP_TEST_SCOPE'] == 'container',
  );
}
