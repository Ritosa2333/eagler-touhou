import 'dart:async';
import 'dart:convert';
import 'dart:io';

import 'package:flutter_test/flutter_test.dart';
import 'package:go_router/go_router.dart';
import 'package:material_ui/material_ui.dart';
import 'package:shared_preferences/shared_preferences.dart';
import 'package:shared_preferences_platform_interface/in_memory_shared_preferences_async.dart';
import 'package:shared_preferences_platform_interface/shared_preferences_async_platform_interface.dart';
import 'package:touhouonline/account_api.dart';
import 'package:touhouonline/catalog.dart';
import 'package:touhouonline/profile_data.dart';
import 'package:touhouonline/profile_page.dart';
import 'package:touhouonline/profile_social.dart';

import '../tools/capture_native_baseline_test.dart' show loadFonts, save;
import 'widget_test.dart' as launcher;

class _SocialFixture extends launcher.FixtureAccountApi {
  _SocialFixture(this.sample, {this.viewer});

  final Map<String, dynamic> sample;
  final String? viewer;
  final requests = <(String, ProfileList?, bool)>[];
  final writes = <bool>[];
  final openedChats = <String>[];
  Completer<ProfileSocial>? nextSummary;
  Completer<ProfileSocial>? privateRead;
  Completer<void>? nextWrite;
  bool followState = false;
  bool failWrite = false;
  ProfileList? failTab;

  @override
  bool get canRestoreSession => viewer != null;

  @override
  Future<AccountIdentity?> restoreSession() async {
    signedIn = viewer != null;
    return viewer == null
        ? null
        : (username: viewer!, name: 'Fixture viewer', expiresAt: null);
  }

  @override
  Future<PublicProfile> profile(String username, {bool fresh = false}) async =>
      PublicProfile(
        username: username,
        nickname: username == 'peer' ? 'Peer' : 'Owner',
        showCareer: false,
        showRecords: false,
        showVersus: false,
      );

  @override
  Future<ProfileSocial> social(
    String username, {
    required bool signedIn,
    ProfileList? list,
  }) async {
    requests.add((username, list, signedIn));
    if (failTab != null && list == failTab) {
      failTab = null;
      throw const AccountRequestException(503, 'SERVICE_UNAVAILABLE');
    }
    if (nextSummary case final Completer<ProfileSocial> read
        when list == null) {
      nextSummary = null;
      return read.future;
    }
    if (username == 'peer') {
      if (privateRead case final Completer<ProfileSocial> read when signedIn) {
        return read.future;
      }
      return ProfileSocial.fromJson(
        sample['private'] as Map<String, dynamic>,
        list,
      );
    }
    return ProfileSocial.fromJson({
      ...sample['summary'] as Map<String, dynamic>,
      'following': signedIn ? followState : null,
      if (list != null)
        (list == ProfileList.following ? 'followingUsers' : list.name):
            sample[list.name],
    }, list);
  }

  @override
  Future<void> setFollowing(String username, bool follow) async {
    expect(username, 'fixture');
    expect(signedIn, isTrue);
    writes.add(follow);
    if (failWrite) {
      failWrite = false;
      throw const AccountRequestException(503, 'SERVICE_UNAVAILABLE');
    }
    final write = nextWrite;
    nextWrite = null;
    if (write != null) await write.future;
    followState = follow;
  }

  @override
  Future<ConversationPage> conversation(String username, {int? before}) {
    openedChats.add(username);
    return super.conversation(username, before: before);
  }
}

void main() {
  final capture = Platform.environment['TOUHOU_PROFILE_SOCIAL_CAPTURE'];
  late Map<String, dynamic> fixture;
  setUpAll(() async {
    fixture = jsonDecode(
      File('tools/profile-social-fixture.json').readAsStringSync(),
    ) as Map<String, dynamic>;
    if (capture != null) {
      await loadFonts(Directory(Platform.environment['TOUHOU_CAPTURE_FONTS']!));
    }
  });
  setUp(() {
    SharedPreferencesAsyncPlatform.instance =
        InMemorySharedPreferencesAsync.empty();
  });

  for (final testCase in [
    (
      size: const Size(1440, 734),
      locale: LauncherLanguage.simplifiedChinese,
      path: '/u/fixture?social=followers',
      initialTab: ProfileList.followers,
      subject: 'fixture',
      title: '关系列表',
      expected: 'Peer',
      name: 'desktop-social-followers-zh-CN',
    ),
    (
      size: const Size(320, 640),
      locale: LauncherLanguage.english,
      path: '/u/fixture#profile-friends',
      initialTab: ProfileList.friends,
      subject: 'fixture',
      title: 'Friends and following',
      expected: 'No friends yet',
      name: 'mobile-narrow-social-empty-en',
    ),
    (
      size: const Size(390, 844),
      locale: LauncherLanguage.japanese,
      path: '/u/peer/social?tab=followers',
      initialTab: ProfileList.followers,
      subject: 'peer',
      title: 'フレンドとフォロー',
      expected: 'この一覧は非公開です。',
      name: 'mobile-social-private-ja',
    ),
  ]) {
    testWidgets('social routes and language | ${testCase.name}', (
      tester,
    ) async {
      tester.view.devicePixelRatio = 1;
      tester.view.physicalSize = testCase.size;
      addTearDown(tester.view.resetDevicePixelRatio);
      addTearDown(tester.view.resetPhysicalSize);
      final api = _SocialFixture(fixture);
      final boundary = GlobalKey();
      await tester.pumpWidget(
        RepaintBoundary(
          key: boundary,
          child: launcher.fixture(
            accountApi: api,
            initialLocation: testCase.path,
            preferences: LauncherPreferences(
              SharedPreferencesAsync(),
              ThemeMode.dark,
              true,
              language: testCase.locale,
            ),
          ),
        ),
      );
      await tester.pumpAndSettle();
      final social = find.byType(ProfileSocialPage);
      expect(find.text(testCase.title), findsOneWidget);
      expect(find.text(testCase.expected).hitTestable(), findsOneWidget);
      expect(api.requests, [(testCase.subject, testCase.initialTab, false)]);
      if (capture != null) {
        await save(tester, boundary, Directory(capture), testCase.name);
      }

      if (testCase.locale == LauncherLanguage.english) {
        final state = tester.state(social);
        api.failTab = ProfileList.followers;
        await tester.tap(find.text('Followers'));
        await tester.pumpAndSettle();
        expect(tester.state(social), same(state));
        expect(find.text('No friends yet'), findsNothing);
        final address = GoRouterState.of(tester.element(social)).uri;
        expect(address.queryParameters['social'], 'followers');
        expect(address.fragment, isEmpty);
        final retry = find.text('Could not load this list. Try again.');
        expect(retry, findsOneWidget);
        await tester.tap(retry);
        await tester.pumpAndSettle();
        await tester.tap(find.text('Peer'));
        await tester.pumpAndSettle();
        expect(find.text('Sign in to follow'), findsOneWidget);
        expect(find.text('Direct messages'), findsNothing);
        expect(find.text('Followers'), findsNothing);
        await tester.tap(find.text('Sign in to follow'));
        await tester.pumpAndSettle();
        expect(find.byType(TextFormField), findsNWidgets(2));
        await tester.binding.handlePopRoute();
        await tester.pumpAndSettle();
        expect(find.text('Sign in to follow'), findsOneWidget);
        await tester.binding.handlePopRoute();
        await tester.pumpAndSettle();
        expect(tester.state(social), same(state));
        expect(find.text('Peer'), findsOneWidget);
      }

      await tester.tap(find.byType(BackButton));
      await tester.pumpAndSettle();
      expect(social, findsNothing);
      final profile = tester.widget<ProfilePage>(find.byType(ProfilePage));
      expect(profile.username, testCase.subject);
      if (testCase.subject == 'fixture') {
        expect(find.text('1,234'), findsOneWidget);
        expect(find.text('—'), findsOneWidget);
      }
      expect(tester.takeException(), isNull);
    });
  }

  testWidgets('follow mutation stays disabled through the summary refresh', (
    tester,
  ) async {
    tester.view.devicePixelRatio = 1;
    tester.view.physicalSize = const Size(1440, 734);
    addTearDown(tester.view.resetDevicePixelRatio);
    addTearDown(tester.view.resetPhysicalSize);
    final api = _SocialFixture(fixture, viewer: 'viewer')..failWrite = true;
    await tester.pumpWidget(
      launcher.fixture(
        accountApi: api,
        initialLocation: '/u/fixture',
        preferences: LauncherPreferences(
          SharedPreferencesAsync(),
          ThemeMode.dark,
          true,
          language: LauncherLanguage.english,
        ),
      ),
    );
    await tester.pumpAndSettle();
    final follow = find.widgetWithText(FilledButton, 'Follow');
    await tester.tap(follow);
    await tester.pumpAndSettle();
    expect(
      find.text('Could not update your follow. Try again.'),
      findsOneWidget,
    );
    expect(api.writes, [true]);
    final write = Completer<void>();
    final read = Completer<ProfileSocial>();
    api.nextWrite = write;
    api.nextSummary = read;
    await tester.tap(follow);
    await tester.pump();
    expect(tester.widget<FilledButton>(follow).onPressed, isNull);
    write.complete();
    await tester.pump();
    await tester.pump();
    expect(tester.widget<FilledButton>(follow).onPressed, isNull);
    expect(api.writes, [true, true]);
    read.complete(
      ProfileSocial.fromJson({
        ...fixture['summary'] as Map<String, dynamic>,
        'following': true,
      }, null),
    );
    await tester.pumpAndSettle();
    expect(find.text('Unfollow'), findsOneWidget);
    expect(find.text('Could not update your follow. Try again.'), findsNothing);
    await tester.tap(find.text('Direct messages'));
    await tester.pumpAndSettle();
    expect(api.openedChats, contains('fixture'));
    await tester.tap(find.byTooltip('Back'));
    await tester.pumpAndSettle();
    expect(find.text('Unfollow'), findsOneWidget);
    expect(tester.takeException(), isNull);
  });

  testWidgets('a reply for a previous viewer cannot show a private list', (
    tester,
  ) async {
    final oldRead = Completer<ProfileSocial>();
    final api = _SocialFixture(fixture, viewer: 'viewer')
      ..privateRead = oldRead;
    await tester.pumpWidget(
      launcher.fixture(
        accountApi: api,
        initialLocation: '/u/peer?social=followers',
      ),
    );
    await tester.pump();
    final social = tester.widget<ProfileSocialPage>(
      find.byType(ProfileSocialPage),
    );
    expect(api.requests, contains(('peer', ProfileList.followers, true)));
    await social.session.signOut();
    await tester.pumpAndSettle();
    expect(find.text('该玩家未公开关系列表'), findsOneWidget);
    oldRead.complete(
      const ProfileSocial(
        visible: true,
        people: [
          (username: 'reader', nickname: 'Prior viewer only', region: null),
        ],
      ),
    );
    await tester.pumpAndSettle();
    expect(find.text('该玩家未公开关系列表'), findsOneWidget);
    expect(find.text('Prior viewer only'), findsNothing);
    expect(tester.takeException(), isNull);
  });
}
