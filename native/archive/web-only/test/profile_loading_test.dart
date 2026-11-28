import 'dart:async';
import 'dart:convert';
import 'dart:io';
import 'dart:ui' show SemanticsRole;

import 'package:flutter/foundation.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:material_ui/material_ui.dart';
import 'package:shared_preferences/shared_preferences.dart';
import 'package:shared_preferences_platform_interface/in_memory_shared_preferences_async.dart';
import 'package:shared_preferences_platform_interface/shared_preferences_async_platform_interface.dart';
import 'package:touhouonline/account_api.dart';
import 'package:touhouonline/catalog.dart';
import 'package:touhouonline/desktop_masthead.dart';
import 'package:touhouonline/l10n/strings.dart';
import 'package:touhouonline/profile_data.dart';
import 'package:touhouonline/profile_draft.dart';
import 'package:touhouonline/profile_editor.dart';
import 'package:touhouonline/profile_loading.dart';

import '../tools/capture_native_baseline_test.dart' show loadFonts, save;
import 'widget_test.dart' as launcher;

const _loadingPanel = ValueKey('profile-panel-loading');

class _PendingProfileApi extends launcher.FixtureAccountApi {
  _PendingProfileApi(this.state);

  final String state;
  final read = Completer<ProfileDraft>();
  final write = Completer<Map<String, ProfileReview>>();
  final career = Completer<ProfileStatistics>();
  final relations = Completer<ProfileSocial>();
  final duelsRead = Completer<DuelHistory>();
  int reads = 0, writes = 0;

  ProfileDraft get draft => ProfileDraft.fromJson({
    ...jsonDecode(File('tools/profile-editor-fixture.json').readAsStringSync())
        as Map<String, dynamic>,
    'review': {'bio': 'pending'},
  });

  @override
  bool get canRestoreSession => true;

  @override
  Future<AccountIdentity?> restoreSession() async {
    signedIn = true;
    return (username: 'fixture', name: '测试玩家', expiresAt: null);
  }

  @override
  Future<ProfileDraft> myProfileDraft() async {
    reads++;
    if (state == 'editor-loading') return read.future;
    if (state == 'editor-failed' && reads == 1) {
      throw const AccountRequestException(503, 'SERVICE_UNAVAILABLE');
    }
    return draft;
  }

  @override
  Future<Map<String, ProfileReview>> saveProfile(
    ProfileDraft draft, {
    String? avatarThumbnail,
  }) async {
    writes++;
    return write.future;
  }

  @override
  Future<PublicProfile> profile(String username, {bool fresh = false}) async =>
      PublicProfile(
        username: username,
        nickname: '测试玩家',
        showVersus: state == 'th09-loading',
        defaultView: state == 'th09-loading'
            ? ProfileGames.th09
            : ProfileGames.all,
      );

  @override
  Future<ProfileStatistics> profileStatistics(
    String username, {
    required bool owner,
  }) => career.future;

  @override
  Future<DuelHistory> duels(
    String username, {
    required bool owner,
    String? cursor,
  }) => duelsRead.future;

  @override
  Future<ProfileSocial> social(
    String username, {
    required bool signedIn,
    ProfileList? list,
  }) async =>
      list == null ? const ProfileSocial(visible: false) : relations.future;
}

void main() {
  final capture = Platform.environment['TOUHOU_PROFILE_STATE_CAPTURE'];
  late List<Game> games;
  setUpAll(() async {
    games = await Game.load();
    if (capture != null) await loadFonts();
  });
  setUp(
    () => SharedPreferencesAsyncPlatform.instance =
        InMemorySharedPreferencesAsync.empty(),
  );
  for (final (device, size, language) in [
    ('desktop', const Size(1440, 734), LauncherLanguage.simplifiedChinese),
    ('mobile', const Size(390, 844), LauncherLanguage.japanese),
    ('mobile-narrow', const Size(320, 640), LauncherLanguage.english),
  ]) {
    for (final theme in [ThemeMode.dark, ThemeMode.light]) {
      for (final state in [
        'editor-loading',
        'editor-failed',
        'editor-saving',
        'career-loading',
        'th09-loading',
        'social-loading',
      ]) {
        testWidgets('$device ${theme.name} $state', (tester) async {
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
          final api = _PendingProfileApi(state);
          final boundary = GlobalKey();
          final path = switch (state) {
            'career-loading' || 'th09-loading' => '/u/fixture',
            'social-loading' => '/u/fixture?social=followers',
            _ => '/u/fixture/edit',
          };
          await tester.pumpWidget(
            RepaintBoundary(
              key: boundary,
              child: launcher.fixture(
                accountApi: api,
                games: state == 'th09-loading' ? games : const [launcher.game],
                initialLocation: path,
                preferences: LauncherPreferences(
                  SharedPreferencesAsync(),
                  theme,
                  true,
                  language: language,
                ),
              ),
            ),
          );
          for (var i = 0; i < 5; i++) {
            await tester.pump(const Duration(milliseconds: 100));
          }
          final strings = lookupLauncherStrings(language.locale);
          final saveButton = find.widgetWithText(
            FilledButton,
            strings.profileEditorSave,
          );
          Rect? buttonBefore;
          Rect? statusBefore;
          if (state == 'editor-saving') {
            await tester.enterText(
              find.byKey(const ValueKey('profile-name')),
              'Draft',
            );
            await tester.pumpAndSettle();
            await tester.ensureVisible(saveButton);
            await tester.pumpAndSettle();
            buttonBefore = tester.getRect(saveButton);
            statusBefore = tester.getRect(
              find.text(strings.profileEditorSubmitted),
            );
            await tester.tap(saveButton);
            await tester.pump();
            await tester.pump(const Duration(milliseconds: 300));
          }
          if (capture != null) {
            final output = Directory(capture)..createSync(recursive: true);
            final name = '$device-${theme.name}-$state-${language.tag}';
            await save(tester, boundary, output, name);
            if (buttonBefore != null) {
              List<double> rect(Rect r) => [r.left, r.top, r.width, r.height];
              File('${output.path}/$name.geometry.json').writeAsStringSync(
                jsonEncode({
                  'viewport': [size.width, size.height],
                  'before': rect(buttonBefore),
                  'during': rect(tester.getRect(saveButton)),
                  'statusBefore': rect(statusBefore!),
                  'statusDuring': rect(
                    tester.getRect(find.text(strings.profileEditorSubmitted)),
                  ),
                }),
              );
            }
          }
          if (state.endsWith('loading')) {
            expect(find.byKey(_loadingPanel), findsOneWidget);
            final semantics = tester.ensureSemantics();
            try {
              await tester.pump();
              final announcement = tester
                  .getSemantics(find.byType(ProfilePanelLoading))
                  .getSemanticsData();
              expect(announcement.role, SemanticsRole.status);
              expect(
                announcement.label,
                state == 'social-loading'
                    ? strings.profileSocialLoading
                    : strings.profileLoading,
              );
            } finally {
              semantics.dispose();
            }
            expect(find.byType(CircularProgressIndicator), findsNothing);
            expect(tester.getSize(find.byKey(_loadingPanel)).height, 128);
            final header = find.byType(DesktopMasthead).hitTestable();
            final headerRect = header.evaluate().isEmpty
                ? null
                : tester.getRect(header);
            if (state == 'editor-loading') {
              expect(find.byType(LinearProgressIndicator), findsNothing);
              api.read.complete(api.draft);
            } else if (state == 'career-loading') {
              api.career.complete(
                const ProfileStatistics(career: true, records: true, games: {}),
              );
            } else if (state == 'th09-loading') {
              api.duelsRead.complete(
                const DuelHistory(
                  games: 0,
                  wins: 0,
                  rounds: [],
                  nextCursor: null,
                ),
              );
            } else {
              api.relations.complete(const ProfileSocial(visible: true));
            }
            await tester.pumpAndSettle();
            expect(find.byKey(_loadingPanel), findsNothing);
            if (headerRect != null) expect(tester.getRect(header), headerRect);
          } else if (state == 'editor-failed') {
            final retry = find.widgetWithText(
              TextButton,
              strings.profileEditorLoad,
            );
            expect(tester.getTopLeft(retry).dx, size.width > 780 ? 404 : 20);
            await tester.tap(retry);
            await tester.pumpAndSettle();
            expect(api.reads, 2);
            expect(find.byKey(const ValueKey('profile-name')), findsOneWidget);
          } else {
            expect(tester.getRect(saveButton), buttonBefore);
            expect(
              tester.getRect(find.text(strings.profileEditorSubmitted)),
              statusBefore,
            );
            expect(tester.getSize(saveButton).width, greaterThanOrEqualTo(104));
            expect(
              find.descendant(
                of: saveButton,
                matching: find.byType(CircularProgressIndicator),
              ),
              findsOneWidget,
            );
            expect(api.writes, 1);
            await tester.binding.handlePopRoute();
            await tester.pump(const Duration(milliseconds: 300));
            expect(find.byType(ProfileEditorPage), findsOneWidget);
            api.write.complete(const {});
            await tester.pumpAndSettle();
            expect(
              find.descendant(
                of: saveButton,
                matching: find.byType(CircularProgressIndicator),
              ),
              findsNothing,
            );
            expect(
              tester
                  .widget<TextFormField>(
                    find.byKey(const ValueKey('profile-name')),
                  )
                  .controller!
                  .text,
              'Draft',
            );
          }
          expect(tester.takeException(), isNull);
        });
      }
    }
  }
}
