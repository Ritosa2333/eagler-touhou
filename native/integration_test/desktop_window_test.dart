import 'dart:async';
import 'dart:convert';
import 'dart:io';
import 'dart:ui' as ui;

import 'package:flutter/rendering.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:flutter_inappwebview/flutter_inappwebview.dart' as web;
import 'package:integration_test/integration_test.dart';
import 'package:go_router/go_router.dart';
import 'package:material_ui/material_ui.dart';
import 'package:prism_ui/prism_ui.dart' as prism;
import 'package:window_manager/window_manager.dart' as windows;
import 'package:touhouonline/desktop_masthead.dart';
import 'package:touhouonline/catalog.dart';
import 'package:touhouonline/account_web_page.dart';
import 'package:touhouonline/desktop_window.dart';
import 'package:touhouonline/game_settings.dart';
import 'package:touhouonline/game_detail.dart';
import 'package:touhouonline/library.dart';
import 'package:touhouonline/l10n/strings.dart';
import 'package:touhouonline/main.dart' as app;
import 'package:touhouonline/site_documents.dart';

void main() {
  IntegrationTestWidgetsFlutterBinding.ensureInitialized();

  testWidgets('Desktop caption operates the real launcher window', (
    tester,
  ) async {
    await app.main([]);
    await tester.pumpAndSettle();
    expect(hasDesktopWindow, isTrue);
    final window = windows.WindowManager.instance.getCurrent()!;
    expect(window.titleBarStyle, windows.TitleBarStyle.hidden);
    final originalSize = window.size;
    final originalPosition = window.position;
    final originallyMaximized = window.isMaximized;
    addTearDown(() {
      window.unmaximize();
      window.setSize(originalSize, false);
      window.position = originalPosition;
      if (originallyMaximized) window.maximize();
    });

    // Capture the actual engine frame, with the real system fonts and caption.
    Future<void> capture(String name) async {
      final output = Platform.environment['TOUHOU_DESKTOP_FRAME_CAPTURE'];
      if (output == null) return;
      final boundary = tester
          .renderObjectList<RenderRepaintBoundary>(find.byType(RepaintBoundary))
          .where((render) => render.attached && render.hasSize)
          .reduce(
            (a, b) =>
                a.size.width * a.size.height >= b.size.width * b.size.height
                ? a
                : b,
          );
      final image = await boundary.toImage(pixelRatio: 1);
      final bytes = await image.toByteData(format: ui.ImageByteFormat.png);
      await Directory(output).create(recursive: true);
      await File('$output/$name.png').writeAsBytes(bytes!.buffer.asUint8List());
      image.dispose();
    }

    window.unmaximize();
    window.setSize(const Size(1280, 720).toNative(), false);
    await tester.pumpAndSettle();
    expect(find.byType(DesktopMasthead), findsOneWidget);
    final controls = find.byType(windows.WindowCaptionButton);
    expect(controls, findsNWidgets(3));
    expect(tester.takeException(), isNull);
    await capture('desktop-caption');

    await tester.tap(controls.at(1));
    await tester.pumpAndSettle();
    expect(window.isMaximized, isTrue);
    await capture('desktop-caption-maximized');
    await tester.tap(controls.at(1));
    await tester.pumpAndSettle();
    expect(window.isMaximized, isFalse);

    await tester.tap(controls.first);
    expect(window.isMinimized, isTrue);
    window.restore();
    await tester.pumpAndSettle();
    expect(window.isMinimized, isFalse);

    var closeRequested = false;
    final closeListener = window.addListener((event) {
      if (event is windows.WindowCloseRequestedEvent) {
        closeRequested = true;
        event.request.cancel();
      }
    });
    try {
      await tester.tap(controls.last);
      await tester.pumpAndSettle();
      expect(closeRequested, isTrue);
    } finally {
      window.removeListener(closeListener);
    }

    final router = GoRouter.of(tester.element(find.byType(DesktopMasthead)));
    final navigator = find.byType(Navigator).first;
    final viewport = tester.getRect(navigator);
    router.push('/game/th06/settings');
    await tester.pump();
    await tester.pump(const Duration(milliseconds: 80));
    expect(tester.getRect(navigator), viewport);
    await tester.pumpAndSettle();
    expect(find.byType(GameSettingsPage), findsOneWidget);
    expect(find.byType(windows.WindowCaptionButton), findsNWidgets(3));
    expect(tester.takeException(), isNull);
    await capture('desktop-caption-game-settings');
    router.pop();
    await tester.pumpAndSettle();
    expect(tester.getRect(navigator), viewport);
    expect(find.byType(DesktopMasthead), findsOneWidget);

    for (final width in [1024.0, 801.0, 390.0, 320.0]) {
      window.setSize(Size(width, 720).toNative(), false);
      await tester.pumpAndSettle();
      expect(find.byType(windows.WindowCaptionButton), findsNWidgets(3));
      expect(tester.takeException(), isNull, reason: 'Caption at $width');
      if (width > 780) {
        final semantics = tester.ensureSemantics();
        try {
          final tabs = find.descendant(
            of: find.byKey(const ValueKey('masthead navigation')),
            matching: find.byType(prism.PressSurface),
          );
          final strings = tester.element(find.byType(DesktopMasthead)).strings;
          for (final (index, label) in [
            (0, strings.singlePlayer),
            (1, strings.multiplayer),
          ]) {
            final data = tester.getSemantics(tabs.at(index)).getSemanticsData();
            expect(
              data.label,
              label,
              reason: 'Native navigation label at $width',
            );
            expect(data.flagsCollection.isButton, isTrue);
            expect(
              data.flagsCollection.isSelected == ui.Tristate.isTrue,
              index == 0,
            );
          }
        } finally {
          semantics.dispose();
        }
      }
      await capture('desktop-caption-${width.toInt()}');
    }

    final captionSemantics = tester.ensureSemantics();
    final preferences = tester
        .widget<app.LauncherApp>(find.byType(app.LauncherApp))
        .preferences;
    final originalLanguage = preferences.language;
    final captionRecords = <Map<String, Object?>>[];
    void activateCaption(int index) {
      final node = tester.getSemantics(
        find.byType(windows.WindowCaptionButton).at(index),
      );
      node.owner!.performAction(node.id, ui.SemanticsAction.tap);
    }

    try {
      for (final width in [1280.0, 320.0]) {
        window.setSize(Size(width, 720).toNative(), false);
        await tester.pumpAndSettle();
        final captionRects = [
          for (var i = 0; i < 3; i++)
            tester.getRect(find.byType(windows.WindowCaptionButton).at(i)),
        ];
        for (final language in LauncherLanguage.values) {
          await preferences.setLanguage(language);
          await tester.pumpAndSettle();
          final context = tester.element(
            find.byType(DesktopWindowControls).first,
          );
          final strings = context.strings;
          final labels = [
            strings.windowMinimize,
            strings.windowMaximize,
            MaterialLocalizations.of(context).closeButtonTooltip,
          ];
          for (var i = 0; i < 3; i++) {
            final button = find.byType(windows.WindowCaptionButton).at(i);
            final data = tester.getSemantics(button).getSemanticsData();
            expect(data.label, labels[i]);
            expect(data.flagsCollection.isButton, isTrue);
            expect(data.hasAction(ui.SemanticsAction.tap), isTrue);
            expect(tester.getRect(button), captionRects[i]);
          }
          captionRecords.add({
            'width': width,
            'language': language.locale.languageCode,
            'labels': labels,
          });
          await capture(
            'caption-labels-${language.locale.languageCode}-${width.toInt()}',
          );
          activateCaption(1);
          await tester.pumpAndSettle();
          expect(window.isMaximized, isTrue);
          expect(
            tester
                .getSemantics(find.byType(windows.WindowCaptionButton).at(1))
                .getSemanticsData()
                .label,
            strings.windowRestore,
          );
          activateCaption(1);
          await tester.pumpAndSettle();
          expect(window.isMaximized, isFalse);
          expect(
            tester
                .getSemantics(find.byType(windows.WindowCaptionButton).at(1))
                .getSemanticsData()
                .label,
            strings.windowMaximize,
          );
        }
      }
      final drawerButton = find.byType(DrawerButton);
      expect(drawerButton, findsOneWidget);
      await tester.tap(drawerButton);
      await tester.pumpAndSettle();
      final drawerContext = tester.element(
        find.byType(DesktopWindowControls).first,
      );
      final drawerLabels = [
        drawerContext.strings.windowMinimize,
        drawerContext.strings.windowMaximize,
        MaterialLocalizations.of(drawerContext).closeButtonTooltip,
      ];
      for (var i = 0; i < 3; i++) {
        expect(
          tester
              .getSemantics(find.byType(windows.WindowCaptionButton).at(i))
              .getSemanticsData()
              .label,
          drawerLabels[i],
          reason: 'Body drawer cannot hide the separate window caption',
        );
      }
      await tester.binding.handlePopRoute();
      await tester.pumpAndSettle();
      activateCaption(0);
      expect(window.isMinimized, isTrue);
      window.restore();
      await tester.pumpAndSettle();
      var semanticCloseRequested = false;
      final semanticCloseListener = window.addListener((event) {
        if (event is windows.WindowCloseRequestedEvent) {
          semanticCloseRequested = true;
          event.request.cancel();
        }
      });
      try {
        activateCaption(2);
        await tester.pumpAndSettle();
        expect(semanticCloseRequested, isTrue);
        expect(find.byType(windows.WindowCaptionButton), findsNWidgets(3));
      } finally {
        window.removeListener(semanticCloseListener);
      }
      final output = Platform.environment['TOUHOU_DESKTOP_FRAME_CAPTURE'];
      if (output != null) {
        await File('$output/caption-actions.json').writeAsString(
          jsonEncode({
            'cases': captionRecords,
            'maximizeRestore': true,
            'minimizeRestore': true,
            'closeVeto': true,
          }),
        );
      }
    } finally {
      await preferences.setLanguage(originalLanguage);
      await tester.pumpAndSettle();
      captionSemantics.dispose();
    }

    for (final width in [1280.0, 390.0, 320.0]) {
      window.setSize(Size(width, 720).toNative(), false);
      await tester.pumpAndSettle();
      final cover = find
          .byWidgetPredicate(
            (widget) =>
                widget is GameArtwork &&
                widget.game.id == 'th06' &&
                widget.size == null,
          )
          .first;
      await tester.ensureVisible(cover);
      await tester.pumpAndSettle();
      await tester.tap(cover);
      await tester.pumpAndSettle();
      final detail = find.byType(GameDetail);
      expect(detail, findsOneWidget);
      final strings = tester.element(detail).strings;
      final artwork = find.descendant(
        of: find.byType(GameHeading),
        matching: find.byType(GameArtwork),
      );
      final progress = find.descendant(
        of: detail,
        matching: find.text(strings.clearProgress),
      );
      expect(
        tester.getTopLeft(artwork).dx,
        closeTo(tester.getTopLeft(progress).dx, .01),
      );
      expect(find.byType(windows.WindowCaptionButton), findsNWidgets(3));
      expect(tester.takeException(), isNull, reason: 'Detail at $width');
      await capture('detail-entry-${width.toInt()}');
      await tester.tap(
        find.descendant(of: detail, matching: find.byType(BackButton)),
      );
      await tester.pumpAndSettle();
      expect(find.byType(LibraryPage).hitTestable(), findsOneWidget);
      expect(tester.takeException(), isNull, reason: 'Return at $width');
    }

    // Exercise the real plugin and production login document. Credentials are
    // never entered; the fixture checks container navigation and native state.
    web.PlatformInAppWebViewWidgetCreationParams? accountView;
    for (final width in [1280.0, 390.0, 320.0]) {
      window.setSize(Size(width, 720).toNative(), false);
      router.push('/game/th06');
      await tester.pumpAndSettle();
      final detail = find.byType(GameDetail);
      final heading = find.byType(GameHeading);
      final headingBefore = tester.getRect(heading);
      for (final close in [true, false]) {
        unawaited(
          openSiteLink(tester.element(detail), signInFor('/?game=th06')),
        );
        final deadline = DateTime.now().add(const Duration(seconds: 30));
        while (DateTime.now().isBefore(deadline)) {
          await tester.pump();
          if (find.byType(web.InAppWebView).evaluate().isNotEmpty &&
              find.byType(LinearProgressIndicator).evaluate().isEmpty) {
            break;
          }
          await tester.runAsync(
            () => Future<void>.delayed(const Duration(milliseconds: 100)),
          );
        }
        expect(
          find.byType(web.InAppWebView),
          findsOneWidget,
          reason: 'Real account engine at $width',
        );
        expect(
          find.byType(LinearProgressIndicator),
          findsNothing,
          reason: 'Production document finished loading at $width',
        );
        expect(find.byType(AccountWebPage), findsOneWidget);
        accountView = tester
            .widget<web.InAppWebView>(find.byType(web.InAppWebView))
            .platform
            .params;
        final exit = find.descendant(
          of: find.byType(AccountWebPage),
          matching: close ? find.byIcon(Icons.close) : find.byType(BackButton),
        );
        await tester.tap(exit);
        await tester.pumpAndSettle();
        expect(find.byType(AccountWebPage), findsNothing);
        expect(router.state.uri.path, '/game/th06');
        expect(
          tester.getRect(heading),
          headingBefore,
          reason: 'Account return preserves game layout at $width',
        );
        expect(tester.takeException(), isNull);
      }
      router.pop();
      await tester.pumpAndSettle();
      expect(find.byType(LibraryPage).hitTestable(), findsOneWidget);
    }

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
              platform: accountView!.webViewEnvironment!,
            ),
            initialSettings: accountView.initialSettings,
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
        window.setSize(Size(width, 720).toNative(), false);
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
                language: document.documentElement.lang, fields,
                viewportWidth: innerWidth, viewportHeight: innerHeight,
                contentWidth: document.documentElement.scrollWidth};
            })()""",
          );
          if (result is Map) document = Map<String, dynamic>.from(result);
          if (document['ready'] == 'complete' &&
              document['fonts'] == 'loaded' &&
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
        final output = Platform.environment['TOUHOU_DESKTOP_FRAME_CAPTURE'];
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
    final documentOutput = Platform.environment['TOUHOU_DESKTOP_FRAME_CAPTURE'];
    if (documentOutput != null) {
      await File('$documentOutput/account-documents.json')
          .writeAsString(jsonEncode(documents));
    }

    expect(tester.takeException(), isNull);
    await tester.pumpWidget(const SizedBox());
    await tester.pumpAndSettle();
  }, skip: !Platform.isWindows);
}
