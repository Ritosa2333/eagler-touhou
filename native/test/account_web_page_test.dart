import 'dart:async';
import 'dart:io' as io;

import 'package:flutter/foundation.dart' show defaultTargetPlatform;
import 'package:flutter/services.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:flutter_inappwebview/flutter_inappwebview.dart' as web;
import 'package:go_router/go_router.dart';
import 'package:material_ui/material_ui.dart';
import 'package:touhouonline/account.dart';
import 'package:touhouonline/account_api.dart';
import 'package:touhouonline/account_web_page.dart';
import 'package:touhouonline/l10n/strings.dart';
import 'package:touhouonline/launcher_navigation.dart';

class _Session extends AccountSession {
  _Session([AccountApi? api]) : super(api ?? AccountApi());
  final checked = <Uri>[];
  Completer<void>? verification;
  bool confirmLogin = false;
  bool loggedIn = false;
  void Function()? verified;
  @override
  AccountIdentity? get identity =>
      loggedIn ? (username: 'fixture', name: '网页玩家', expiresAt: null) : null;
  @override
  Future<void> syncWebSession(Uri source, io.Cookie? cookie) async {
    checked.add(source);
    await verification?.future;
    loggedIn = confirmLogin;
    verified?.call();
  }
}

class _SavedSessionApi extends AccountApi {
  String token = 'fixture-saved-session';
  @override
  io.Cookie get webSessionCookie => io.Cookie(sessionCookieName, token)
    ..path = '/'
    ..secure = true
    ..httpOnly = true;
}

class _Environment extends web.PlatformWebViewEnvironment {
  _Environment([web.WebViewEnvironmentSettings? settings])
    : super.implementation(
        web.PlatformWebViewEnvironmentCreationParams(settings: settings),
      );
  @override
  String get id => 'account-web-fixture';
  @override
  Future<String?> getAvailableVersion({
    String? browserExecutableFolder,
  }) async => 'test-runtime';
  @override
  Future<web.PlatformWebViewEnvironment> create({
    web.WebViewEnvironmentSettings? settings,
  }) async => _Environment(settings);
}

class _Cookies extends web.PlatformCookieManager {
  _Cookies(super.params) : super.implementation();
  static web.Cookie? saved;
  static bool acceptsWrites = true;
  @override
  Future<web.Cookie?> getCookie({
    required web.WebUri url,
    required String name,
    web.PlatformInAppWebViewController? iosBelow11WebViewController,
    web.PlatformInAppWebViewController? webViewController,
  }) async {
    expectSync(url.host, 'touhouonline.com');
    return saved;
  }

  @override
  Future<bool> setCookie({
    required web.WebUri url,
    required String name,
    required String value,
    String path = '/',
    String? domain,
    int? expiresDate,
    int? maxAge,
    bool? isSecure,
    bool? isHttpOnly,
    web.HTTPCookieSameSitePolicy? sameSite,
    web.PlatformInAppWebViewController? iosBelow11WebViewController,
    web.PlatformInAppWebViewController? webViewController,
  }) async {
    expectSync(url.host, 'touhouonline.com');
    if (!acceptsWrites) return false;
    saved = web.Cookie(
      name: name,
      value: value,
      path: path,
      domain: domain ?? url.host,
      expiresDate: expiresDate,
      isSecure: isSecure,
      isHttpOnly: isHttpOnly,
      sameSite: sameSite,
    );
    return true;
  }
}

class _Controller extends web.PlatformInAppWebViewController {
  _Controller()
    : super.implementation(
        const web.PlatformInAppWebViewControllerCreationParams(id: 1),
      );
  bool backAvailable = false;
  bool crashed = false;
  web.WebUri? currentUrl;
  Completer<web.WebUri?>? readingUrl;
  int backs = 0;
  @override
  Future<web.WebUri?> getUrl() async {
    if (crashed) throw StateError('fixture controller unavailable');
    return readingUrl == null ? currentUrl : await readingUrl!.future;
  }

  @override
  Future<bool> canGoBack() async {
    if (crashed) throw StateError('fixture controller unavailable');
    return backAvailable;
  }

  @override
  Future<void> goBack() async {
    backs++;
  }

  @override
  void dispose({bool isKeepAlive = false}) {}
}

class _WebWidget extends web.PlatformInAppWebViewWidget {
  _WebWidget(super.params) : super.implementation();
  late _Controller controller;
  dynamic get wrapped => params.controllerFromPlatform!(controller);
  @override
  T controllerFromPlatform<T>(web.PlatformInAppWebViewController value) =>
      params.controllerFromPlatform!(value) as T;
  @override
  Widget build(BuildContext context) => _EngineView(engine: this);

  void finish() {
    params.onProgressChanged?.call(wrapped, 100);
    params.onLoadStop?.call(wrapped, params.initialUrlRequest?.url);
  }

  @override
  void dispose() {}
}

// A native view survives rebuilds of the progress/header around it.
class _EngineView extends StatefulWidget {
  const _EngineView({required this.engine});
  final _WebWidget engine;
  @override
  State<_EngineView> createState() => _EngineViewState();
}

class _EngineViewState extends State<_EngineView> {
  final controller = _Controller();
  @override
  void initState() {
    super.initState();
    controller.currentUrl = widget.engine.params.initialUrlRequest?.url;
    WidgetsBinding.instance.addPostFrameCallback((_) {
      if (mounted) {
        widget.engine.params.onWebViewCreated?.call(widget.engine.wrapped);
      }
    });
  }

  @override
  Widget build(BuildContext context) {
    widget.engine.controller = controller;
    return const SizedBox.expand(key: ValueKey('embedded-account-engine'));
  }
}

class _Platform extends web.InAppWebViewPlatform {
  _WebWidget? latest;
  @override
  web.PlatformCookieManager createPlatformCookieManager(
    web.PlatformCookieManagerCreationParams params,
  ) => _Cookies(params);
  @override
  web.PlatformWebViewEnvironment createPlatformWebViewEnvironmentStatic() =>
      _Environment();
  @override
  web.PlatformWebViewEnvironment createPlatformWebViewEnvironment(
    web.PlatformWebViewEnvironmentCreationParams params,
  ) => _Environment(params.settings);
  @override
  web.PlatformInAppWebViewWidget createPlatformInAppWebViewWidget(
    web.PlatformInAppWebViewWidgetCreationParams params,
  ) => latest = _WebWidget(params);
}

void main() {
  final platform = _Platform();
  final nativePlatforms = TargetPlatformVariant({
    TargetPlatform.windows,
    TargetPlatform.linux,
    TargetPlatform.macOS,
    TargetPlatform.iOS,
    TargetPlatform.android,
  });
  setUpAll(() {
    web.InAppWebViewPlatform.instance = platform;
  });
  setUp(() {
    _Cookies.saved = null;
    _Cookies.acceptsWrites = true;
    platform.latest = null;
    TestDefaultBinaryMessengerBinding.instance.defaultBinaryMessenger
        .setMockMethodCallHandler(
          const MethodChannel('plugins.flutter.io/path_provider'),
          (_) async => 'D:/workspace/eagler/artifacts/native-launcher-85941f0d56/account-web/widget-data',
        );
  });

  Future<GoRouter> show(
    WidgetTester tester,
    _Session session,
    Uri address, {
    bool reduced = false,
    FutureOr<bool> Function()? canLeaveParent,
    bool direct = false,
  }) async {
    final router = GoRouter(
      initialLocation: direct
          ? '/account'
          : canLeaveParent == null
          ? '/'
          : '/protected',
      redirect: (_, state) => launcherGameLink(state.uri),
      routes: [
        GoRoute(
          path: '/',
          builder: (_, _) => const Scaffold(body: Text('native parent')),
        ),
        GoRoute(
          path: '/account',
          builder: (_, _) => AccountWebPage(session: session, address: address),
        ),
        GoRoute(
          path: '/protected',
          onExit: (_, state) => canLeaveParent?.call() ?? true,
          builder: (_, _) => const Scaffold(body: Text('protected parent')),
        ),
        GoRoute(
          path: '/cover',
          builder: (_, _) => const Scaffold(body: Text('cover page')),
        ),
        GoRoute(
          path: '/game/:id',
          builder: (_, _) => const Scaffold(body: Text('native game')),
        ),
        GoRoute(
          path: '/lobby',
          builder: (_, _) => const Scaffold(body: Text('native lobby')),
        ),
      ],
    );
    addTearDown(() {
      router.dispose();
      session.dispose();
    });
    await tester.pumpWidget(
      MaterialApp.router(
        routerConfig: router,
        theme: ThemeData(
          pageTransitionsTheme: LauncherPageTransitions(reduced: reduced),
        ),
        builder: (context, child) => MediaQuery(
          data: MediaQuery.of(context).copyWith(disableAnimations: reduced),
          child: child!,
        ),
        locale: const Locale('zh', 'CN'),
        localizationsDelegates: const [
          LauncherStrings.delegate,
          ...GlobalMaterialLocalizations.delegates,
        ],
        supportedLocales: LauncherStrings.supportedLocales,
      ),
    );
    if (!direct) router.push('/account');
    await tester.pumpAndSettle();
    return router;
  }

  for (final delayed in [false, true]) {
    testWidgets('Native caller can reject account handoff | delayed=$delayed', (
      tester,
    ) async {
      final decision = Completer<bool>();
      var allowed = false;
      final router = await show(
        tester,
        _Session(),
        Uri.https('touhouonline.com', '/u/ritosa'),
        canLeaveParent: () => allowed
            ? true
            : delayed
            ? decision.future
            : false,
      );
      final engine = platform.latest!;
      expect(
        await engine.params.shouldOverrideUrlLoading!(
          engine.wrapped,
          web.NavigationAction(
            request: web.URLRequest(
              url: web.WebUri('https://touhouonline.com/lobby.html'),
            ),
            isForMainFrame: true,
          ),
        ),
        web.NavigationActionPolicy.CANCEL,
      );
      await tester.pumpAndSettle();
      expect(
        find.byType(AccountWebPage),
        findsNothing,
        reason: 'Account overlay closes before native caller decides to leave',
      );
      if (delayed) {
        decision.complete(false);
        await tester.pumpAndSettle();
      }
      expect(router.state.uri.path, '/protected');
      expect(find.text('protected parent'), findsOneWidget);
      allowed = true;
      router.go('/lobby');
      await tester.pumpAndSettle();
      expect(router.state.uri.path, '/lobby');
      expect(find.text('native lobby'), findsOneWidget);
      expect(tester.takeException(), isNull);
    }, variant: nativePlatforms);
  }

  testWidgets('Direct account entry can open a native destination', (
    tester,
  ) async {
    final router = await show(
      tester,
      _Session(),
      Uri.https('touhouonline.com', '/u/ritosa'),
      direct: true,
    );
    final engine = platform.latest!;
    await engine.params.shouldOverrideUrlLoading!(
      engine.wrapped,
      web.NavigationAction(
        request: web.URLRequest(
          url: web.WebUri('https://touhouonline.com/?game=th08'),
        ),
        isForMainFrame: true,
      ),
    );
    await tester.pumpAndSettle();
    expect(router.state.uri.path, '/game/th08');
    expect(find.byType(AccountWebPage), findsNothing);
    expect(tester.takeException(), isNull);
  }, variant: nativePlatforms);

  testWidgets('A covered close attempt does not disable a later close', (
    tester,
  ) async {
    final router = await show(
      tester,
      _Session(),
      Uri.https('touhouonline.com', '/u/ritosa'),
    );
    await tester.tap(find.byIcon(Icons.close));
    router.push('/cover');
    await tester.pumpAndSettle();
    expect(find.text('cover page'), findsOneWidget);
    router.pop();
    await tester.pumpAndSettle();
    expect(find.byType(AccountWebPage), findsOneWidget);
    await tester.tap(find.byIcon(Icons.close));
    await tester.pumpAndSettle();
    expect(router.state.uri.path, '/');
    expect(find.text('native parent'), findsOneWidget);
    expect(tester.takeException(), isNull);
  }, variant: nativePlatforms);

  testWidgets(
    'Profile core links enter native pages while account links stay web',
    (tester) async {
      for (final (href, target) in [
        ('https://touhouonline.com/?game=th08&lang=ja', '/game/th08'),
        ('https://touhouonline.com/en.html?game=th06', '/game/th06'),
        ('https://touhouonline.com/lobby.html?lang=zh-CN', '/lobby'),
        ('https://touhouonline.com/ja.html', '/'),
      ]) {
        final router = await show(
          tester,
          _Session(),
          Uri.https('touhouonline.com', '/u/ritosa'),
        );
        final engine = platform.latest!;
        FutureOr<web.NavigationActionPolicy?> navigate(
          String uri, {
          bool main = true,
        }) => engine.params.shouldOverrideUrlLoading!(
          engine.wrapped,
          web.NavigationAction(
            request: web.URLRequest(url: web.WebUri(uri)),
            isForMainFrame: main,
          ),
        );
        expect(
          await navigate(href, main: false),
          web.NavigationActionPolicy.ALLOW,
        );
        expect(
          await navigate('https://touhouonline.com/u/ritosa#edit'),
          web.NavigationActionPolicy.ALLOW,
        );
        expect(
          await navigate('https://touhouonline.com/privacy.html'),
          web.NavigationActionPolicy.ALLOW,
        );
        expect(
          await navigate('https://touhouonline.com/?section=messages'),
          web.NavigationActionPolicy.ALLOW,
        );
        expect(router.state.uri.path, '/account');
        expect(await navigate(href), web.NavigationActionPolicy.CANCEL);
        await tester.pumpAndSettle();
        expect(router.state.uri.path, target, reason: href);
        expect(find.byType(AccountWebPage), findsNothing);
        expect(tester.takeException(), isNull);
      }
    },
    variant: nativePlatforms,
  );

  testWidgets('Website history and load callbacks enter the native game', (
    tester,
  ) async {
    for (final history in [true, false]) {
      final router = await show(
        tester,
        _Session(),
        Uri.https('touhouonline.com', '/u/ritosa'),
      );
      final engine = platform.latest!;
      engine.finish();
      await tester.pumpAndSettle();
      expect(
        await engine.params.shouldOverrideUrlLoading!(
          engine.wrapped,
          web.NavigationAction(
            request: web.URLRequest(
              url: web.WebUri('https://touhouonline.com/u/ritosa#edit'),
            ),
            isForMainFrame: true,
          ),
        ),
        web.NavigationActionPolicy.ALLOW,
      );
      engine.controller.currentUrl = web.WebUri(
        'https://touhouonline.com/u/ritosa#edit',
      );
      engine.params.onUpdateVisitedHistory!(
        engine.wrapped,
        web.WebUri('https://touhouonline.com/u/ritosa#edit'),
        false,
      );
      await tester.pumpAndSettle();
      expect(router.state.uri.path, '/account');
      final address = web.WebUri('https://touhouonline.com/?game=th08');
      engine.controller.currentUrl = address;
      if (history) {
        engine.params.onUpdateVisitedHistory!(engine.wrapped, address, false);
      } else {
        engine.params.onLoadStop!(engine.wrapped, address);
      }
      await tester.pumpAndSettle();
      expect(router.state.uri.path, '/game/th08');
      expect(find.byType(AccountWebPage), findsNothing);
      expect(tester.takeException(), isNull);
    }
  }, variant: nativePlatforms);

  testWidgets('Transient login history never closes the account container', (
    tester,
  ) async {
    // Production WebView2 reported root history, then the login URL 5 ms later,
    // while the session remained a guest. A late URL read must also be ignored.
    for (final scenario in ['loading', 'responsive update', 'late read']) {
      final lateRead = scenario == 'late read';
      final login = web.WebUri(
        'https://touhouonline.com/login.html?lang=zh-CN',
      );
      final router = await show(tester, _Session(), login.uriValue);
      final engine = platform.latest!;
      final root = web.WebUri('https://touhouonline.com/');
      if (scenario != 'loading') {
        engine.finish();
        await tester.pumpAndSettle();
      }
      if (lateRead) {
        engine.controller.readingUrl = Completer<web.WebUri?>();
      } else {
        engine.controller.currentUrl = root;
      }
      engine.params.onUpdateVisitedHistory!(engine.wrapped, root, false);
      await tester.pump(Duration(milliseconds: lateRead ? 100 : 20));
      engine.controller.currentUrl = login;
      engine.params.onUpdateVisitedHistory!(engine.wrapped, login, false);
      engine.controller.readingUrl?.complete(root);
      await tester.pumpAndSettle();
      expect(router.state.uri.path, '/account');
      expect(find.byType(AccountWebPage), findsOneWidget);
      expect(tester.takeException(), isNull);
      await tester.tap(find.byIcon(Icons.close));
      await tester.pumpAndSettle();
      expect(router.state.uri.path, '/');
    }
  }, variant: nativePlatforms);

  testWidgets(
    'Embedded website retains URL and closes while session verification waits',
    (tester) async {
      final session = _Session()..verification = Completer<void>();
      final address = Uri.https('touhouonline.com', '/login.html', {
        'lang': 'zh-CN',
        'next': '/profile.html#edit',
      });
      final router = await show(tester, session, address);
      expect(
        find.byKey(const ValueKey('embedded-account-engine')),
        findsOneWidget,
      );
      expect(platform.latest!.params.initialUrlRequest!.url!.uriValue, address);
      final environment = platform.latest!.params.webViewEnvironment;
      if (defaultTargetPlatform == TargetPlatform.windows) {
        expect(environment!.settings!.userDataFolder, contains('account-web'));
      } else {
        // Native cookie stores and the embedded view must share the default
        // platform session instead of a separate Windows environment.
        expect(environment, isNull);
      }
      platform.latest!.finish();
      await tester.pump();
      expect(session.checked, [Uri.https('touhouonline.com')]);
      await tester.tap(find.byIcon(Icons.close));
      await tester.pumpAndSettle();
      expect(router.state.uri.path, '/');
      expect(find.text('native parent'), findsOneWidget);
      expect(session.verification!.isCompleted, isFalse);
      session.verification!.complete();
      await tester.pumpAndSettle();
      expect(tester.takeException(), isNull);
    },
    variant: nativePlatforms,
  );

  testWidgets('Navigation cancellation preserves the loaded account document', (
    tester,
  ) async {
    final address = Uri.https('touhouonline.com', '/login.html');
    final router = await show(tester, _Session(), address);
    var engine = platform.latest!;
    engine.finish();
    await tester.pumpAndSettle();
    final state = tester.state(find.byType(_EngineView));
    for (final type in [
      web.WebResourceErrorType.CANCELLED,
      web.WebResourceErrorType.CONNECTION_ABORTED,
    ]) {
      engine = platform.latest!;
      engine.params.onReceivedError!(
        engine.wrapped,
        web.WebResourceRequest(
          url: web.WebUri.uri(address),
          isForMainFrame: true,
        ),
        web.WebResourceError(description: 'Navigation cancelled', type: type),
      );
      await tester.pumpAndSettle();
      expect(router.state.uri.path, '/account');
      expect(
        find.byKey(const ValueKey('embedded-account-engine')),
        findsOneWidget,
      );
      expect(identical(tester.state(find.byType(_EngineView)), state), isTrue);
    }
    // A subsequent allowed navigation must still surface a real load failure.
    engine = platform.latest!;
    engine.params.onLoadStart!(engine.wrapped, web.WebUri.uri(address));
    engine.params.onReceivedError!(
      engine.wrapped,
      web.WebResourceRequest(
        url: web.WebUri.uri(address),
        isForMainFrame: true,
      ),
      web.WebResourceError(
        description: 'Connection stopped before the document loaded',
        type: web.WebResourceErrorType.CONNECTION_ABORTED,
      ),
    );
    await tester.pumpAndSettle();
    expect(find.byKey(const ValueKey('embedded-account-engine')), findsNothing);
    expect(find.byIcon(Icons.refresh), findsOneWidget);
    await tester.tap(find.byIcon(Icons.refresh));
    await tester.pumpAndSettle();
    expect(
      find.byKey(const ValueKey('embedded-account-engine')),
      findsOneWidget,
    );
    await tester.tap(find.byIcon(Icons.close));
    await tester.pumpAndSettle();
    expect(router.state.uri.path, '/');
  }, variant: nativePlatforms);

  testWidgets('System back uses web history and escapes a failed controller', (
    tester,
  ) async {
    final session = _Session();
    final router = await show(
      tester,
      session,
      Uri.https('touhouonline.com', '/u/ritosa'),
    );
    final engine = platform.latest!;
    engine.controller.backAvailable = true;
    engine.finish();
    await tester.pumpAndSettle();
    await tester.binding.handlePopRoute();
    await tester.pumpAndSettle();
    expect(engine.controller.backs, 1);
    expect(router.state.uri.path, '/account');
    engine.controller.crashed = true;
    await tester.binding.handlePopRoute();
    await tester.pumpAndSettle();
    expect(router.state.uri.path, '/');
    expect(find.text('native parent'), findsOneWidget);
    expect(tester.takeException(), isNull);
  }, variant: nativePlatforms);

  for (final reduced in [false, true]) {
    testWidgets(
      'iOS edge back cancels and completes account return | reduced=$reduced',
      (tester) async {
        await tester.binding.setSurfaceSize(const Size(390, 844));
        addTearDown(() => tester.binding.setSurfaceSize(null));
        final session = _Session();
        final router = await show(
          tester,
          session,
          Uri.https('touhouonline.com', '/u/ritosa'),
          reduced: reduced,
        );
        final interrupted = await tester.startGesture(const Offset(1, 300));
        await interrupted.moveBy(
          const Offset(60, 0),
          timeStamp: const Duration(milliseconds: 300),
        );
        await tester.pump(const Duration(milliseconds: 500));
        await interrupted.up(timeStamp: const Duration(milliseconds: 900));
        await tester.pumpAndSettle();
        expect(router.state.uri.path, '/account');
        expect(session.checked, isEmpty);

        final completed = await tester.startGesture(const Offset(1, 300));
        await completed.moveBy(
          const Offset(300, 0),
          timeStamp: const Duration(milliseconds: 300),
        );
        await tester.pump(const Duration(milliseconds: 500));
        await completed.up(timeStamp: const Duration(milliseconds: 900));
        await tester.pumpAndSettle();
        expect(router.state.uri.path, '/');
        expect(find.text('native parent'), findsOneWidget);
        expect(session.checked, [Uri.https('touhouonline.com')]);
        expect(tester.takeException(), isNull);
      },
      variant: TargetPlatformVariant.only(TargetPlatform.iOS),
    );
  }

  testWidgets(
    'iOS web history takes back navigation before the native caller',
    (tester) async {
      final router = await show(
        tester,
        _Session(),
        Uri.https('touhouonline.com', '/u/ritosa'),
      );
      final engine = platform.latest!;
      engine.controller.backAvailable = true;
      engine.finish();
      await tester.pumpAndSettle();
      expect(
        engine.params.initialSettings!.allowsBackForwardNavigationGestures,
        isTrue,
      );
      expect(
        tester
            .widget<PopScope>(
              find.byWidgetPredicate((widget) => widget is PopScope).last,
            )
            .canPop,
        isFalse,
      );
      await tester.binding.handlePopRoute();
      await tester.pumpAndSettle();
      expect(engine.controller.backs, 1);
      expect(router.state.uri.path, '/account');

      engine.controller.backAvailable = false;
      engine.finish();
      await tester.pumpAndSettle();
      expect(
        tester
            .widget<PopScope>(
              find.byWidgetPredicate((widget) => widget is PopScope).last,
            )
            .canPop,
        isTrue,
      );
      expect(tester.takeException(), isNull);
    },
    variant: TargetPlatformVariant.only(TargetPlatform.iOS),
  );

  testWidgets('A verified login returns to its native caller', (tester) async {
    final session = _Session()..confirmLogin = true;
    final router = await show(
      tester,
      session,
      Uri.https('touhouonline.com', '/login.html', {
        'next': '/profile.html#game-settings',
      }),
    );
    platform.latest!.finish();
    await tester.pumpAndSettle();
    expect(router.state.uri.path, '/');
    expect(session.identity?.username, 'fixture');
    expect(find.text('native parent'), findsOneWidget);
    expect(tester.takeException(), isNull);
  }, variant: nativePlatforms);

  testWidgets(
    'Login return cancels a web room before restoring its native caller',
    (tester) async {
      final session = _Session()..verification = Completer<void>();
      final router = await show(
        tester,
        session,
        Uri.https('touhouonline.com', '/login.html', {
          'next': 'https://touhouonline.com/?j=fixture&lang=zh-CN',
        }),
      );
      final engine = platform.latest!;
      final policy = await engine.params.shouldOverrideUrlLoading!(
        engine.wrapped,
        web.NavigationAction(
          request: web.URLRequest(
            url: web.WebUri('https://touhouonline.com/?lang=ja&j=fixture'),
          ),
          isForMainFrame: true,
        ),
      );
      expect(policy, web.NavigationActionPolicy.CANCEL);
      await tester.pumpAndSettle();
      expect(router.state.uri.path, '/');
      expect(session.verification!.isCompleted, isFalse);
      session.verification!.complete();
      await tester.pumpAndSettle();
      expect(tester.takeException(), isNull);
    },
    variant: nativePlatforms,
  );

  testWidgets('Another origin never creates an embedded account engine', (
    tester,
  ) async {
    platform.latest = null;
    final router = await show(
      tester,
      _Session(),
      Uri.https('other.example', '/login.html'),
    );
    expect(platform.latest, isNull);
    expect(find.text('无法打开链接'), findsOneWidget);
    await tester.tap(find.byIcon(Icons.close));
    await tester.pumpAndSettle();
    expect(router.state.uri.path, '/');
  }, variant: nativePlatforms);

  testWidgets('A saved native session enters the system cookie store', (
    tester,
  ) async {
    final session = _Session(_SavedSessionApi());
    await show(tester, session, Uri.https('touhouonline.com', '/profile.html'));
    expect(platform.latest, isNotNull);
    expect(_Cookies.saved?.value, 'fixture-saved-session');
    expect(_Cookies.saved?.name, session.api.sessionCookieName);
    expect(_Cookies.saved?.isSecure, isTrue);
    expect(_Cookies.saved?.isHttpOnly, isTrue);
    expect(_Cookies.saved?.sameSite, web.HTTPCookieSameSitePolicy.LAX);
    expect(session.checked, isEmpty);
  }, variant: nativePlatforms);

  testWidgets('A failed cookie store leaves the native session untouched', (
    tester,
  ) async {
    _Cookies.acceptsWrites = false;
    final session = _Session(_SavedSessionApi());
    final router = await show(
      tester,
      session,
      Uri.https('touhouonline.com', '/profile.html'),
    );
    expect(platform.latest, isNull);
    expect(find.text('无法打开链接'), findsOneWidget);
    await tester.tap(find.byIcon(Icons.close));
    await tester.pumpAndSettle();
    expect(router.state.uri.path, '/');
    expect(session.checked, isEmpty);
    expect(session.api.webSessionCookie?.value, 'fixture-saved-session');
  }, variant: nativePlatforms);

  testWidgets('Service rotation updates the browser cookie once', (
    tester,
  ) async {
    final api = _SavedSessionApi();
    final session = _Session(api)
      ..verified = () => api.token = 'fixture-rotated-session';
    await show(tester, session, Uri.https('touhouonline.com', '/profile.html'));
    final engine = platform.latest!;
    engine.finish();
    await tester.pumpAndSettle();
    expect(api.token, 'fixture-rotated-session');
    expect(session.checked, hasLength(1));
    expect(_Cookies.saved?.value, 'fixture-rotated-session');
    engine.finish();
    await tester.pumpAndSettle();
    expect(session.checked, hasLength(1));
  }, variant: nativePlatforms);

  testWidgets('Late service rotation cannot overwrite a browser logout', (
    tester,
  ) async {
    final api = _SavedSessionApi();
    final session = _Session(api)
      ..verification = Completer<void>()
      ..verified = () => api.token = 'fixture-rotated-session';
    await show(tester, session, Uri.https('touhouonline.com', '/profile.html'));
    platform.latest!.finish();
    await tester.pump();
    _Cookies.saved = null;
    session.verification!.complete();
    await tester.pumpAndSettle();
    expect(_Cookies.saved, isNull);
  }, variant: nativePlatforms);
}
