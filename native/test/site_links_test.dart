import 'package:flutter/services.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:go_router/go_router.dart';
import 'package:material_ui/material_ui.dart';
import 'package:shared_preferences_platform_interface/in_memory_shared_preferences_async.dart';
import 'package:shared_preferences_platform_interface/shared_preferences_async_platform_interface.dart';
import 'package:touhouonline/site_documents.dart';

import 'widget_test.dart' as launcher;

void main() {
  test(
    'Core website links use the native router without consuming web sections',
    () {
      for (final (href, location) in [
        ('https://touhouonline.com/?lang=ja', '/'),
        ('https://touhouonline.com/index.html', '/'),
        ('https://touhouonline.com/en.html?lang=en', '/'),
        ('https://touhouonline.com/lobby.html?lang=ja', '/lobby'),
        ('https://touhouonline.com/lobby', '/lobby'),
        (
          'https://touhouonline.com/ja.html?game=th07',
          'https://touhouonline.com/?game=th07',
        ),
        (
          'https://touhouonline.com/?j=invite&lang=zh-CN',
          'https://touhouonline.com/?j=invite&lang=zh-CN',
        ),
        (
          'https://touhouonline.com/index.html?mpRoom=th09-code',
          'https://touhouonline.com/?mpRoom=th09-code',
        ),
      ]) {
        expect(nativeSiteLocation(Uri.parse(href)), location, reason: href);
      }
      for (final href in [
        'https://other.example/?game=th06',
        'https://touhouonline.com.attacker.example/?game=th06',
        'https://user@touhouonline.com/?game=th06',
        'https://touhouonline.com:8443/?game=th06',
        'http://touhouonline.com/?game=th06',
        'https://touhouonline.com/?section=messages',
        'https://touhouonline.com/?section=home',
        'https://touhouonline.com/?to=ritosa',
        'https://touhouonline.com/#help',
        'https://touhouonline.com/u/ritosa?social=followers',
        'https://touhouonline.com/privacy.html',
      ]) {
        expect(nativeSiteLocation(Uri.parse(href)), isNull, reason: href);
      }
    },
  );

  test(
    'Account addresses resolve to production web with language and fragments',
    () {
      for (final (native, path, fragment, query) in [
        ('/login?mode=register', '/login.html', '', {'mode': 'register'}),
        ('/account/security', '/profile.html', 'security', <String, String>{}),
        ('/u/Ritosa/edit', '/u/Ritosa', 'edit', <String, String>{}),
        (
          '/u/ritosa/social?tab=followers',
          '/u/ritosa',
          '',
          {'social': 'followers'},
        ),
        (
          '/u/ritosa?character=2#profile-friends',
          '/u/ritosa',
          'profile-friends',
          {'character': '2'},
        ),
      ]) {
        final route = Uri.parse(accountWebLocation(Uri.parse(native), 'ja')!);
        expect(route.path, accountWebPath);
        final target = Uri.parse(route.queryParameters['url']!);
        expect(target.origin, 'https://touhouonline.com');
        expect(target.path, path);
        expect(target.fragment, fragment);
        expect(target.queryParameters, {'lang': 'ja', ...query});
      }
      for (final input in [
        '/web-account',
        '/game/th06',
        'https://other.example/u/ritosa',
        '//other.example/login.html',
        'http://touhouonline.com/login.html',
      ]) {
        expect(accountWebLocation(Uri.parse(input), 'zh-CN'), isNull);
      }
    },
  );

  testWidgets('web links preserve the game page and handle launch failure', (
    tester,
  ) async {
    SharedPreferencesAsyncPlatform.instance =
        InMemorySharedPreferencesAsync.empty();
    const channel = MethodChannel('plugins.flutter.io/url_launcher');
    final requests = <Map<Object?, Object?>>[];
    var succeeds = true;
    final messenger = tester.binding.defaultBinaryMessenger;
    messenger.setMockMethodCallHandler(channel, (call) async {
      requests.add(call.arguments as Map<Object?, Object?>);
      return succeeds;
    });
    addTearDown(() => messenger.setMockMethodCallHandler(channel, null));
    await tester.pumpWidget(launcher.fixture(initialLocation: '/game/th06'));
    await tester.pumpAndSettle();
    final context = tester.element(find.byType(Scaffold).last);
    final router = GoRouter.of(context);
    final original = router.state.uri;
    for (final href in [
      '/faq.html',
      '/docs/multiplayer/th07',
      '/uptime',
      '/ratings.html?character=0',
      '/assets/donation.webp',
    ]) {
      await openSiteLink(context, href);
      await tester.pumpAndSettle();
      final address = Uri.parse(requests.last['url']! as String);
      expect(address.host, 'touhouonline.com');
      expect(address.queryParameters['lang'], 'zh-CN');
      expect(requests.last['useWebView'], isFalse);
      expect(router.state.uri, original);
    }
    await openSiteLink(context, '/about.html?lang=ja#contributors');
    expect(
      Uri.parse(requests.last['url']! as String).queryParameters['lang'],
      'ja',
    );
    expect(Uri.parse(requests.last['url']! as String).fragment, 'contributors');
    final count = requests.length;
    await openSiteLink(context, 'javascript:alert(1)');
    expect(requests.length, count);
    succeeds = false;
    await openSiteLink(context, '/uptime');
    await tester.pump();
    expect(find.byType(SnackBar), findsOneWidget);
    expect(router.state.uri, original);
    expect(tester.takeException(), isNull);
    await tester.pumpWidget(const SizedBox());
    await tester.pumpAndSettle();
  });

  testWidgets('Game links and lobby documents stay in the native application', (
    tester,
  ) async {
    SharedPreferencesAsyncPlatform.instance =
        InMemorySharedPreferencesAsync.empty();
    await tester.pumpWidget(launcher.fixture());
    await tester.pumpAndSettle();
    final router = GoRouter.of(tester.element(find.byType(Scaffold).last));
    for (final (href, target) in [
      ('https://touhouonline.com/index.html?game=th08', '/game/th08'),
      ('https://touhouonline.com/lobby.html?lang=ja', '/lobby'),
      ('https://touhouonline.com/en.html?lang=en', '/'),
    ]) {
      await openSiteLink(tester.element(find.byType(Scaffold).last), href);
      await tester.pumpAndSettle();
      expect(router.state.uri.path, target, reason: href);
      expect(tester.takeException(), isNull);
    }
    await tester.pumpWidget(const SizedBox());
    await tester.pumpAndSettle();
  });
}
