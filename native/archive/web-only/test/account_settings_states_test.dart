import 'dart:async';
import 'dart:io';

import 'package:flutter/foundation.dart';
import 'package:flutter/services.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:material_ui/material_ui.dart';
import 'package:shared_preferences/shared_preferences.dart';
import 'package:shared_preferences_platform_interface/in_memory_shared_preferences_async.dart';
import 'package:shared_preferences_platform_interface/shared_preferences_async_platform_interface.dart';
import 'package:touhouonline/account_api.dart';
import 'package:touhouonline/catalog.dart';
import 'package:touhouonline/cloud_saves.dart';
import 'package:touhouonline/l10n/strings.dart';
import 'package:touhouonline/launcher_select.dart';
import 'package:touhouonline/save_progress.dart' show CloudGameSave;

import '../tools/capture_native_baseline_test.dart' show loadFonts, save;
import 'cloud_saves_test.dart' show cloudFixture, testCloudGame;
import 'widget_test.dart' as launcher;

class _StateApi extends launcher.FixtureAccountApi {
  _StateApi(this.state)
    : directory = CloudSaveDirectory.fromJson({
        ...cloudFixture(),
        if (state == 'empty') 'saves': <dynamic>[],
      });

  final String state;
  final CloudSaveDirectory directory;
  final pending = Completer<CloudSaveDirectory>();
  int reads = 0;

  @override
  bool get canRestoreSession => true;

  @override
  Future<AccountIdentity?> restoreSession() async {
    signedIn = !state.startsWith('guest');
    return signedIn
        ? (username: 'fixture', name: '测试玩家', expiresAt: null)
        : null;
  }

  @override
  Future<Map<String, CloudGameSave>> cloudGameSaves() async =>
      directory.statistics;

  @override
  Future<CloudSaveDirectory> cloudSaveDirectory() async {
    reads++;
    if (state == 'loading') return pending.future;
    if (state == 'failed' && reads == 1) {
      throw const AccountRequestException(503, 'SERVICE_UNAVAILABLE');
    }
    return directory;
  }
}

void main() {
  final capture = Platform.environment['TOUHOU_SETTINGS_STATE_CAPTURE'];
  if (capture != null) setUpAll(() => loadFonts());
  setUp(
    () => SharedPreferencesAsyncPlatform.instance =
        InMemorySharedPreferencesAsync.empty(),
  );

  for (final (device, size, language) in [
    ('desktop', const Size(1440, 734), LauncherLanguage.simplifiedChinese),
    ('mobile', const Size(390, 844), LauncherLanguage.japanese),
    ('mobile-narrow', const Size(320, 640), LauncherLanguage.english),
  ]) {
    for (final state in [
      'guest-cloud',
      'guest-security',
      'guest-profile',
      'other-profile',
      'loading',
      'failed',
      'empty',
    ]) {
      testWidgets('account settings state | $device | $state', (tester) async {
        debugDefaultTargetPlatformOverride = TargetPlatform.windows;
        addTearDown(() => debugDefaultTargetPlatformOverride = null);
        tester.view.devicePixelRatio = 1;
        tester.view.physicalSize = size;
        addTearDown(tester.view.resetDevicePixelRatio);
        addTearDown(tester.view.resetPhysicalSize);
        addTearDown(() => tester.pumpWidget(const SizedBox.shrink()));
        if (capture != null) {
          late BuildContext imageContext;
          await tester.pumpWidget(
            Builder(
              builder: (context) {
                imageContext = context;
                return const SizedBox.shrink();
              },
            ),
          );
          await tester.runAsync(
            () => precacheImage(
              const AssetImage('assets/launcher-wordmark-dark.png'),
              imageContext,
            ),
          );
        }
        final api = _StateApi(state);
        final boundary = GlobalKey();
        final path = switch (state) {
          'guest-security' => '/account/security',
          'guest-profile' => '/u/fixture/edit',
          'other-profile' => '/u/another/edit',
          _ => '/account/cloud-saves',
        };
        await tester.pumpWidget(
          RepaintBoundary(
            key: boundary,
            child: launcher.fixture(
              accountApi: api,
              games: const [testCloudGame],
              initialLocation: path,
              preferences: LauncherPreferences(
                SharedPreferencesAsync(),
                ThemeMode.dark,
                true,
                language: language,
              ),
            ),
          ),
        );
        for (var i = 0; i < 4; i++) {
          await tester.pump(const Duration(milliseconds: 100));
        }
        final strings = lookupLauncherStrings(language.locale);
        if (capture != null) {
          await save(
            tester,
            boundary,
            Directory(capture)..createSync(recursive: true),
            '$device-settings-$state-${language.tag}',
          );
        }
        expect(
          find.byType(AppBar),
          size.width > 780 ? findsNothing : findsOneWidget,
        );
        if (state.startsWith('guest')) {
          expect(api.reads, 0);
        }
        if (state.startsWith('guest') || state == 'other-profile') {
          expect(find.byType(TextFormField), findsNothing);
        }
        if (state == 'empty') {
          expect(find.text(strings.cloudNoSave), findsOneWidget);
        }
        if (state == 'failed') {
          expect(find.text(strings.cloudReadFailed), findsOneWidget);
        }
        if (state.startsWith('guest')) {
          final label = switch (state) {
            'guest-security' => strings.signInForSecurity,
            'guest-profile' => strings.profileEditorSignIn,
            _ => strings.cloudSignIn,
          };
          await tester.tap(find.text(label));
          await tester.pumpAndSettle();
          expect(find.byType(TextFormField), findsNWidgets(2));
          await tester.binding.handlePopRoute();
          await tester.pumpAndSettle();
          expect(find.text(label), findsOneWidget);
          expect(find.byType(TextFormField), findsNothing);
          expect(api.reads, 0);
        }
        if (state == 'empty') {
          final selector = find.byType(LauncherSelect<String>);
          await tester.ensureVisible(selector);
          await tester.tap(selector);
          await tester.pumpAndSettle();
          expect(find.byType(MenuItemButton), findsWidgets);
          await tester.sendKeyEvent(LogicalKeyboardKey.escape);
          await tester.pumpAndSettle();
          expect(find.byType(MenuItemButton), findsNothing);
          expect(selector, findsOneWidget);
          expect(find.text(strings.cloudNoSave), findsOneWidget);
        }
        if (state == 'failed') {
          await tester.tap(find.text(strings.cloudReadFailed));
          await tester.pumpAndSettle();
          expect(api.reads, 2);
          expect(find.text(strings.cloudReadFailed), findsNothing);
        }
        if (state == 'loading') {
          api.pending.complete(api.directory);
          await tester.pumpAndSettle();
          expect(api.reads, 1);
        }
        expect(tester.takeException(), isNull);
        await tester.pumpWidget(const SizedBox.shrink());
      });
    }
  }
}
