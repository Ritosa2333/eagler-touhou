import 'dart:convert';
import 'dart:io';
import 'dart:ui' as ui;

import 'package:flutter/rendering.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:integration_test/integration_test.dart';
import 'package:go_router/go_router.dart';
import 'package:material_ui/material_ui.dart';
import 'package:prism_ui/prism_ui.dart' as prism;
import 'package:window_manager/window_manager.dart' as windows;
import 'package:touhouonline/desktop_masthead.dart';
import 'package:touhouonline/catalog.dart';
import 'package:touhouonline/desktop_window.dart';
import 'package:touhouonline/game_settings.dart';
import 'package:touhouonline/game_detail.dart';
import 'package:touhouonline/library.dart';
import 'package:touhouonline/l10n/strings.dart';
import 'package:touhouonline/main.dart' as app;

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
  }, skip: !Platform.isWindows);
}
