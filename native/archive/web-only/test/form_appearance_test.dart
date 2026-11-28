import 'dart:async';
import 'dart:convert';
import 'dart:io';

import 'package:flutter/foundation.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:material_ui/material_ui.dart';
import 'package:prism_ui/prism_ui.dart' as prism;
import 'package:shared_preferences/shared_preferences.dart';
import 'package:shared_preferences_platform_interface/in_memory_shared_preferences_async.dart';
import 'package:shared_preferences_platform_interface/shared_preferences_async_platform_interface.dart';
import 'package:touhouonline/account_api.dart';
import 'package:touhouonline/catalog.dart';
import 'package:touhouonline/l10n/strings.dart';

import '../tools/capture_native_baseline_test.dart' show loadFonts, save;
import 'widget_test.dart' as launcher;

class _PendingLogin extends launcher.FixtureAccountApi {
  final response = Completer<AccountIdentity>();
  @override
  Future<AccountIdentity> signIn(String identity, String password) {
    expect(identity, 'fixture');
    expect(password, 'valid-test-value');
    return response.future;
  }
}

void main() {
  final capture = Platform.environment['TOUHOU_FORM_APPEARANCE_CAPTURE'];
  setUpAll(() async {
    final fonts = Platform.environment['TOUHOU_CAPTURE_FONTS'];
    await loadFonts(fonts == null ? null : Directory(fonts));
  });
  for (final (device, size, language) in [
    ('narrow', const Size(320, 640), LauncherLanguage.english),
    ('mobile', const Size(390, 844), LauncherLanguage.japanese),
    ('desktop', const Size(1440, 734), LauncherLanguage.simplifiedChinese),
  ]) {
    for (final light in [false, true]) {
      final stem = '$device-${light ? 'light' : 'dark'}';
      testWidgets('shared form states | $stem', (tester) async {
        debugDefaultTargetPlatformOverride = TargetPlatform.windows;
        addTearDown(() => debugDefaultTargetPlatformOverride = null);
        tester.view.devicePixelRatio = 1;
        tester.view.physicalSize = size;
        addTearDown(tester.view.resetDevicePixelRatio);
        addTearDown(tester.view.resetPhysicalSize);
        SharedPreferencesAsyncPlatform.instance =
            InMemorySharedPreferencesAsync.empty();
        final api = _PendingLogin();
        final boundary = GlobalKey();
        await tester.pumpWidget(
          RepaintBoundary(
            key: boundary,
            child: launcher.fixture(
              accountApi: api,
              initialLocation: '/login',
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
        await tester.runAsync(
          () => precacheImage(
            const AssetImage('assets/account-background.webp'),
            boundary.currentContext!,
          ),
        );
        await tester.pumpAndSettle();
        final fields = find.byType(TextFormField);
        final context = tester.element(fields.first);
        final colors = Theme.of(context).colorScheme;
        final strings = context.strings;
        final submit = find.widgetWithText(FilledButton, strings.signIn);
        final normalBounds = tester.getRect(fields.first);
        Future<void> record(String state) async {
          if (capture == null) return;
          debugPrint('form $stem | $state');
          await tester.pump();
          await save(tester, boundary, Directory(capture), '$stem-$state');
          final rect = tester.getRect(fields.first);
          File('$capture/$stem-$state.json').writeAsStringSync(
            jsonEncode({
              'field': [rect.left, rect.top, rect.width, rect.height],
              'outline': colors.outline.toARGB32(),
              'primary': colors.primary.toARGB32(),
              'error': colors.error.toARGB32(),
              'surface': colors.surface.toARGB32(),
            }),
          );
        }

        await record('ready');
        await tester.tap(fields.first);
        await tester.pumpAndSettle();
        expect(
          tester
              .widget<TextFormField>(fields.first)
              .controller!
              .selection
              .isValid,
          isTrue,
        );
        expect(tester.getRect(fields.first), normalBounds);
        await record('focused');
        FocusScope.of(context).unfocus();
        await tester.ensureVisible(submit);
        await tester.tap(submit);
        await tester.pumpAndSettle();
        expect(find.text(strings.identityRequired), findsOneWidget);
        await record('invalid');
        await tester.enterText(fields.first, 'fixture');
        await tester.enterText(fields.last, 'valid-test-value');
        await tester.ensureVisible(submit);
        await tester.tap(submit);
        await tester.pump();
        await tester.pump(const Duration(milliseconds: 200));
        expect(tester.widget<TextFormField>(fields.first).enabled, isFalse);
        expect(tester.widget<TextFormField>(fields.last).enabled, isFalse);
        await record('pending');
        api.response.completeError(
          const AccountRequestException(503, 'SERVICE_UNAVAILABLE'),
        );
        await tester.pumpAndSettle();
        expect(tester.widget<TextFormField>(fields.first).enabled, isTrue);
        expect(
          tester.widget<TextFormField>(fields.first).controller!.text,
          'fixture',
        );
        expect(tester.takeException(), isNull);
        expect(colors.outline, prism.Theme.of(context).colorScheme.outline);
        final contrast =
            (colors.outline.computeLuminance() + .05) /
            (colors.surface.computeLuminance() + .05);
        expect(
          contrast >= 1 ? contrast : 1 / contrast,
          greaterThanOrEqualTo(3),
        );
      });
    }
  }
}
