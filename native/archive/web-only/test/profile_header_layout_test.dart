import 'dart:convert';
import 'dart:io';

import 'package:flutter/foundation.dart';
import 'package:flutter/rendering.dart' show RenderParagraph;
import 'package:flutter_test/flutter_test.dart';
import 'package:material_ui/material_ui.dart';
import 'package:shared_preferences/shared_preferences.dart';
import 'package:shared_preferences_platform_interface/in_memory_shared_preferences_async.dart';
import 'package:shared_preferences_platform_interface/shared_preferences_async_platform_interface.dart';
import 'package:touhouonline/account_api.dart';
import 'package:touhouonline/catalog.dart';
import 'package:touhouonline/l10n/strings.dart';
import 'package:touhouonline/profile_data.dart';
import 'package:touhouonline/profile_draft.dart';
import 'package:touhouonline/profile_editor.dart';
import 'package:touhouonline/profile_page.dart';

import '../tools/capture_native_baseline_test.dart' show loadFonts, save;
import 'widget_test.dart' as launcher;

class _HeaderApi extends launcher.FixtureAccountApi {
  _HeaderApi(this.role);
  final String role;
  bool following = false;
  String get username => role == 'owner' ? 'fixture' : 'other';
  String get name => switch (role) {
    'owner' => '测试玩家的十二字昵称展示',
    'peer' => 'LongNickname',
    _ => '测试玩家',
  };
  @override
  bool get canRestoreSession => role != 'guest';
  @override
  Future<AccountIdentity?> restoreSession() async {
    signedIn = true;
    return (username: 'fixture', name: '我的账号', expiresAt: null);
  }

  @override
  Future<PublicProfile> profile(String username, {bool fresh = false}) async =>
      PublicProfile(
        username: username,
        nickname: name,
        country: 'CN',
        showCareer: false,
        showRecords: false,
        showVersus: false,
      );
  @override
  Future<ProfileSocial> social(
    String username, {
    required bool signedIn,
    ProfileList? list,
  }) async => ProfileSocial(
    visible: true,
    following: signedIn ? following : null,
    followingCount: 3,
    followerCount: 7,
    friendCount: 2,
  );
  @override
  Future<void> setFollowing(String username, bool follow) async =>
      following = follow;
  @override
  Future<ProfileDraft> myProfileDraft() async => ProfileDraft.fromJson(
    jsonDecode(File('tools/profile-editor-fixture.json').readAsStringSync())
        as Map<String, dynamic>,
  );
}

void main() {
  final capture = Platform.environment['TOUHOU_PROFILE_HEADER_CAPTURE'];
  setUpAll(() async {
    final fonts = Platform.environment['TOUHOU_CAPTURE_FONTS'];
    await loadFonts(fonts == null ? null : Directory(fonts));
  });
  for (final (device, size, language) in [
    ('narrow', const Size(320, 640), LauncherLanguage.english),
    ('mobile', const Size(390, 844), LauncherLanguage.japanese),
    (
      'compact-desktop',
      const Size(781, 734),
      LauncherLanguage.simplifiedChinese,
    ),
    ('desktop', const Size(1440, 734), LauncherLanguage.english),
  ]) {
    for (final role in ['guest', 'owner', 'peer']) {
      for (final light in [false, true]) {
        for (final scale in [1.0, 2.0]) {
          final stem =
              '$device-$role-${light ? 'light' : 'dark'}-${scale.toInt()}';
          testWidgets('profile identity | $stem', (tester) async {
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
            final api = _HeaderApi(role);
            final boundary = GlobalKey();
            await tester.pumpWidget(
              RepaintBoundary(
                key: boundary,
                child: launcher.fixture(
                  accountApi: api,
                  initialLocation: '/u/${api.username}',
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
            if (capture != null) {
              await save(tester, boundary, Directory(capture), stem);
            }
            expect(tester.takeException(), isNull);
            final name = find.text(api.name);
            if (Platform.isWindows) {
              expect(
                tester.renderObject<RenderParagraph>(name).didExceedMaxLines,
                isFalse,
                reason: 'Full nickname must fit | $stem',
              );
            }
            final cover = tester.getRect(
              find.byKey(const ValueKey('profile cover')),
            );
            final avatar = tester.getRect(
              find.byKey(const ValueKey('profile avatar')),
            );
            expect(cover.top, size.width > 780 ? 44 : 0);
            expect(avatar.top, cover.bottom - (size.width > 780 ? 26 : 24));
            final strings = tester.element(find.byType(ProfilePage)).strings;
            if (Platform.isWindows) {
              for (final label in [
                strings.profileFollowing,
                strings.profileFollowers,
                strings.friends,
              ]) {
                final labelFinder = find.descendant(
                  of: find.byType(TextButton),
                  matching: find.text(label),
                );
                final paragraph = tester.renderObject<RenderParagraph>(
                  labelFinder,
                );
                expect(
                  paragraph.getBoxesForSelection(
                    TextSelection(baseOffset: 0, extentOffset: label.length),
                  ),
                  hasLength(1),
                  reason: 'Relationship label must not split | $stem | $label',
                );
                final button = find
                    .ancestor(
                      of: labelFinder,
                      matching: find.byType(TextButton),
                    )
                    .first;
                final count = find
                    .descendant(of: button, matching: find.byType(Text))
                    .first;
                final countParagraph = tester.renderObject<RenderParagraph>(
                  count,
                );
                expect(
                  tester.getTopLeft(count).dy +
                      countParagraph.getDryBaseline(
                        countParagraph.constraints,
                        TextBaseline.alphabetic,
                      )!,
                  closeTo(
                    tester.getTopLeft(labelFinder).dy +
                        paragraph.getDryBaseline(
                          paragraph.constraints,
                          TextBaseline.alphabetic,
                        )!,
                    .5,
                  ),
                  reason:
                      'Count and label share their text baseline | $stem | $label',
                );
              }
            }
            final action = role == 'owner'
                ? find.byTooltip(strings.editProfile)
                : find.widgetWithText(
                    FilledButton,
                    role == 'guest'
                        ? strings.profileFollowSignIn
                        : strings.profileFollow,
                  );
            // Desktop owner actions have visible labels instead of tooltips.
            final target = role == 'owner' && size.width > 780
                ? find.widgetWithText(OutlinedButton, strings.editProfile)
                : action;
            final actionBounds = tester.getRect(target);
            if (size.width <= 365 || size.width <= 780 && scale > 1) {
              expect(
                actionBounds.top,
                greaterThanOrEqualTo(tester.getRect(name).bottom),
              );
            }
            await tester.ensureVisible(target);
            await tester.tap(target);
            await tester.pumpAndSettle();
            if (role == 'peer') {
              expect(api.following, isTrue);
              expect(find.text(strings.profileUnfollow), findsOneWidget);
            } else {
              if (role == 'owner') {
                expect(find.byType(ProfileEditorPage), findsOneWidget);
              } else {
                expect(find.byType(TextFormField), findsNWidgets(2));
              }
              await tester.binding.handlePopRoute();
              await tester.pumpAndSettle();
              expect(find.byType(ProfilePage), findsOneWidget);
              expect(
                tester.getRect(find.byKey(const ValueKey('profile avatar'))),
                avatar,
              );
            }
            expect(tester.takeException(), isNull);
          });
        }
      }
    }
  }
}
