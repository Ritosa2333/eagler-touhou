import 'dart:convert';
import 'dart:io';

import 'package:flutter/foundation.dart';
import 'package:flutter_markdown_plus/flutter_markdown_plus.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:go_router/go_router.dart';
import 'package:material_ui/material_ui.dart';
import 'package:shared_preferences/shared_preferences.dart';
import 'package:shared_preferences_platform_interface/in_memory_shared_preferences_async.dart';
import 'package:shared_preferences_platform_interface/shared_preferences_async_platform_interface.dart';
import 'package:touhouonline/account_entry.dart';
import 'package:touhouonline/account_fields.dart';
import 'package:touhouonline/catalog.dart';
import 'package:touhouonline/l10n/strings.dart';
import 'package:touhouonline/library.dart';

import '../test/widget_test.dart' as fixtures;
import 'capture_native_baseline_test.dart' show loadFonts, save;

Future<void> _tap(WidgetTester tester, Finder finder) async {
  await tester.ensureVisible(finder);
  await tester.pumpAndSettle();
  expect(finder.hitTestable(), findsOneWidget);
  await tester.tap(finder);
  await tester.pumpAndSettle();
}

Future<void> _document(WidgetTester tester, Finder content) async {
  await tester.runAsync(() async {
    final deadline = DateTime.now().add(const Duration(seconds: 3));
    while (DateTime.now().isBefore(deadline) && content.evaluate().isEmpty) {
      await tester.pump(const Duration(milliseconds: 20));
      await Future<void>.delayed(const Duration(milliseconds: 20));
    }
    await tester.pumpAndSettle();
  });
  expect(content, findsWidgets);
}

void main() {
  TestWidgetsFlutterBinding.ensureInitialized();
  final location = Platform.environment['TOUHOU_ACCOUNT_LANGUAGE_CAPTURE'];
  if (location == null) return;
  final output = Directory(location);

  setUpAll(() async {
    await output.create(recursive: true);
    await loadFonts(Directory(Platform.environment['TOUHOU_CAPTURE_FONTS']!));
  });

  for (final language in [
    LauncherLanguage.simplifiedChinese,
    LauncherLanguage.english,
    LauncherLanguage.japanese,
  ]) {
    for (final (device, width, height) in [
      ('desktop', 1440.0, 734.0),
      ('mobile', 390.0, 844.0),
      ('mobile-narrow', 320.0, 640.0),
    ]) {
      testWidgets('Render ${language.tag} account screens at $device', (
        tester,
      ) async {
        debugDefaultTargetPlatformOverride = TargetPlatform.windows;
        addTearDown(() => debugDefaultTargetPlatformOverride = null);
        tester.view.devicePixelRatio = 1;
        tester.view.physicalSize = Size(width, height);
        addTearDown(tester.view.resetDevicePixelRatio);
        addTearDown(tester.view.resetPhysicalSize);
        SharedPreferencesAsyncPlatform.instance =
            InMemorySharedPreferencesAsync.empty();
        final preferences = LauncherPreferences(
          SharedPreferencesAsync(),
          ThemeMode.dark,
          true,
          language: language,
        );
        final boundary = GlobalKey();
        await tester.pumpWidget(
          RepaintBoundary(
            key: boundary,
            child: fixtures.fixture(
              preferences: preferences,
              accountApi: fixtures.FixtureAccountApi(),
            ),
          ),
        );
        await tester.pumpAndSettle();
        final root = tester.element(find.byType(LibraryPage));
        final router = GoRouter.of(root);
        final strings = root.strings;
        await tester.runAsync(
          () => precacheImage(
            const AssetImage('assets/account-background.webp'),
            root,
          ),
        );

        Future<void> capture(String state) async {
          expect(tester.takeException(), isNull);
          List<double> box(Finder finder) {
            final rect = tester.getRect(finder);
            return [rect.left, rect.top, rect.right, rect.bottom];
          }

          if (state == 'login' || state == 'register' || state == 'reset') {
            File(
              '${output.path}/$device-$state-${language.tag}.geometry.json',
            ).writeAsStringSync(
              const JsonEncoder.withIndent('  ').convert({
                'viewport': [width, height],
                'fields': [
                  for (final element in find.byType(AccountField).evaluate())
                    {
                      'label': (element.widget as AccountField).label,
                      'labelRect': box(
                        find
                            .descendant(
                              of: find.byWidget(element.widget),
                              matching: find.text(
                                (element.widget as AccountField).label,
                              ),
                            )
                            .first,
                      ),
                      'inputRect': box(
                        find.descendant(
                          of: find.byWidget(element.widget),
                          matching: find.byType(TextFormField),
                        ),
                      ),
                    },
                ],
              }),
            );
          }
          await save(
            tester,
            boundary,
            output,
            '$device-$state-${language.tag}',
          );
        }

        router.push('/login');
        await tester.pumpAndSettle();
        final account = find.byType(AccountEntry);
        expect(account, findsOneWidget);
        await capture('login');
        await _tap(tester, find.text(strings.registerTitle));
        await capture('register');

        final consent = find.text(strings.termsConsent);
        await tester.ensureVisible(consent);
        await tester.pumpAndSettle();
        await tester.tap(consent);
        await _document(tester, find.byType(Markdown));
        final terms = tester.widget<Markdown>(find.byType(Markdown)).data;
        expect(
          terms,
          contains(switch (language) {
            LauncherLanguage.simplifiedChinese => '哪些内容可能被删除',
            LauncherLanguage.english => 'What we may remove',
            LauncherLanguage.japanese => '削除することがある内容',
          }),
        );
        await capture('registration-terms');
        await _tap(tester, find.text(strings.cancel));
        expect(
          tester.widget<CheckboxListTile>(find.byType(CheckboxListTile)).value,
          isFalse,
        );

        await _tap(tester, find.text(strings.backToSignIn));
        await _tap(tester, find.text(strings.forgotPassword));
        expect(find.byType(TextFormField), findsOneWidget);
        await capture('reset');
      });
    }
  }
}
