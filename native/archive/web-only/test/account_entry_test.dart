import 'package:flutter_markdown_plus/flutter_markdown_plus.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:go_router/go_router.dart';
import 'package:material_ui/material_ui.dart';
import 'package:touhouonline/account.dart';
import 'package:touhouonline/account_api.dart';
import 'package:touhouonline/account_entry.dart';
import 'package:touhouonline/l10n/strings.dart';

const _sampleEmail = 'player@example.invalid';

class _AccountFixture extends AccountApi {
  final calls = <String>[];
  bool captcha = false;
  bool conflict = false;
  AccountIdentity? member;

  @override
  Future<bool> requiresMailCaptcha() async => captcha;

  @override
  Future<RegistrationChallenge> startRegistration({
    required String email,
    required String username,
    required String nickname,
    required String password,
    required String? captcha,
  }) async {
    calls.add('register');
    expect(email, _sampleEmail);
    expect(username, 'sample_01');
    expect(nickname, '测试玩家');
    expect(password, 'sample123');
    expect(captcha, isNull);
    if (conflict) {
      throw const AccountRequestException(409, 'USERNAME_ALREADY_EXISTS');
    }
    return (
      id: 'registration-fixture',
      expiresAt: DateTime.now().add(const Duration(minutes: 15)),
    );
  }

  @override
  Future<RegistrationChallenge> resendRegistration({
    required RegistrationChallenge challenge,
    required String email,
    required String password,
    required String? captcha,
  }) async {
    calls.add('resend');
    expect(challenge.id, 'registration-fixture');
    expect(email, _sampleEmail);
    expect(password, 'sample123');
    return challenge;
  }

  @override
  Future<void> verifyRegistration({
    required RegistrationChallenge challenge,
    required String email,
    required String password,
    required String otp,
  }) async {
    calls.add('verify');
    expect(challenge.id, 'registration-fixture');
    expect(email, _sampleEmail);
    expect(password, 'sample123');
    if (otp == '012345') {
      throw const AccountRequestException(410, 'REGISTRATION_EXPIRED');
    }
    expect(otp, '123456');
    member = (username: 'sample_01', name: '测试玩家', expiresAt: null);
  }

  @override
  Future<AccountIdentity?> session() async => member;

  @override
  Future<void> requestPasswordReset(String email, String? captcha) async {
    calls.add('reset-request');
    expect(email, _sampleEmail);
  }

  @override
  Future<void> resetPassword(String email, String otp, String password) async {
    calls.add('reset-verify');
    expect(email, _sampleEmail);
    expect(password, 'sample234');
    if (otp != '234567') {
      throw const AccountRequestException(400, 'INVALID_OTP');
    }
  }

  @override
  Future<Map<String, dynamic>> mailCaptchaChallenge() async {
    calls.add('captcha');
    throw const AccountRequestException(503, 'CAPTCHA_UNAVAILABLE');
  }
}

Future<AccountSession> _openFromHome(
  WidgetTester tester,
  _AccountFixture api,
  Size size, {
  Locale locale = const Locale('zh'),
}) async {
  tester.view.devicePixelRatio = 1;
  tester.view.physicalSize = size;
  addTearDown(tester.view.resetDevicePixelRatio);
  addTearDown(tester.view.resetPhysicalSize);
  final session = AccountSession(api);
  final router = GoRouter(
    initialLocation: '/home',
    routes: [
      GoRoute(
        path: '/home',
        builder: (context, _) =>
            Scaffold(body: AccountSummary(session: session)),
      ),
      GoRoute(
        path: '/login',
        builder: (_, state) => AccountEntry(
          session: session,
          startingMode: state.uri.queryParameters['mode'],
        ),
      ),
    ],
  );
  addTearDown(router.dispose);
  addTearDown(session.dispose);
  await tester.pumpWidget(
    MaterialApp.router(
      routerConfig: router,
      locale: locale,
      supportedLocales: LauncherStrings.supportedLocales,
      localizationsDelegates: const [
        LauncherStrings.delegate,
        ...GlobalMaterialLocalizations.delegates,
      ],
    ),
  );
  await tester.pumpAndSettle();
  return session;
}

Future<void> _tap(WidgetTester tester, Finder finder) async {
  await tester.ensureVisible(finder);
  await tester.pumpAndSettle();
  await tester.tap(finder);
  await tester.pumpAndSettle();
}

Future<void> _fill(
  WidgetTester tester,
  List<String> values, {
  int start = 0,
}) async {
  final fields = find.byType(TextFormField);
  for (final (index, value) in values.indexed) {
    final field = fields.at(index + start);
    await tester.ensureVisible(field);
    await tester.enterText(field, value);
    await tester.pumpAndSettle();
  }
}

Future<void> _readTerms(
  WidgetTester tester, {
  required bool accept,
  String? expectedBody,
}) async {
  final strings = tester.element(find.byType(AccountEntry)).strings;
  final entry = find.text(strings.termsConsent);
  await tester.ensureVisible(entry);
  await tester.pumpAndSettle();
  await tester.runAsync(() async {
    await tester.tap(entry);
    final agree = _primary(strings.termsAgree);
    final deadline = DateTime.now().add(const Duration(seconds: 3));
    while (DateTime.now().isBefore(deadline)) {
      await tester.pump(const Duration(milliseconds: 20));
      if (agree.evaluate().isNotEmpty &&
          tester.widget<FilledButton>(agree).onPressed != null) {
        break;
      }
      await Future<void>.delayed(const Duration(milliseconds: 20));
    }
    expect(find.text(strings.terms), findsOneWidget);
    if (expectedBody != null) {
      expect(
        tester.widget<Markdown>(find.byType(Markdown)).data,
        contains(expectedBody),
      );
    }
    expect(tester.widget<FilledButton>(agree).onPressed, isNotNull);
    await tester.pump(const Duration(milliseconds: 300));
    await tester.tap(accept ? agree : find.text(strings.cancel));
    await tester.pumpAndSettle();
  });
  await tester.pumpAndSettle();
}

Finder _primary(String label) => find.widgetWithText(FilledButton, label);

void main() {
  testWidgets('320 px registration holds the sent draft and returns to home', (
    tester,
  ) async {
    final api = _AccountFixture()..conflict = true;
    final session = await _openFromHome(tester, api, const Size(320, 640));
    await _tap(tester, find.text('注册'));
    expect(find.text('注册账号'), findsOneWidget);
    expect(tester.widget<FilledButton>(_primary('发送验证码')).onPressed, isNull);
    await _fill(tester, [
      'sample_01',
      '测试玩家',
      _sampleEmail,
      'sample123',
      'sample123',
    ]);
    await _readTerms(tester, accept: false);
    expect(tester.widget<FilledButton>(_primary('发送验证码')).onPressed, isNull);
    await _readTerms(tester, accept: true);
    expect(tester.widget<FilledButton>(_primary('发送验证码')).onPressed, isNotNull);
    await _tap(tester, _primary('发送验证码'));
    expect(api.calls, ['register']);
    expect(find.text('此用户名已被使用'), findsOneWidget);

    api.conflict = false;
    await _tap(tester, _primary('发送验证码'));
    final fields = find.byType(TextFormField);
    expect(tester.widget<TextFormField>(fields.first).enabled, isFalse);
    expect(
      tester.widget<TextFormField>(fields.at(2)).controller!.text,
      _sampleEmail,
    );
    expect(
      tester
          .widget<TextButton>(find.widgetWithText(TextButton, '60s'))
          .onPressed,
      isNull,
    );
    await tester.pump(const Duration(seconds: 61));
    await _tap(tester, find.text('重发验证码'));
    expect(api.calls.last, 'resend');

    await tester.enterText(fields.last, '012345');
    await _tap(tester, _primary('注册'));
    expect(find.text('验证码已过期，请重新填写资料'), findsOneWidget);
    await _tap(tester, find.text('重新填写资料'));
    expect(tester.widget<TextFormField>(fields.first).enabled, isTrue);
    expect(
      tester.widget<TextFormField>(fields.at(2)).controller!.text,
      _sampleEmail,
    );
    await _tap(tester, _primary('发送验证码'));
    await tester.enterText(fields.last, '123456');
    await _tap(tester, _primary('注册'));
    expect(session.identity?.username, 'sample_01');
    expect(find.text('@sample_01'), findsOneWidget);
    expect(tester.takeException(), isNull);
  });

  testWidgets(
    'English registration retains inputs when reading the original terms',
    (tester) async {
      final api = _AccountFixture()..conflict = true;
      await _openFromHome(
        tester,
        api,
        const Size(320, 640),
        locale: const Locale('en'),
      );
      final strings = lookupLauncherStrings(const Locale('en'));
      await _tap(tester, find.text(strings.register));
      await _fill(tester, [
        'sample_01',
        '测试玩家',
        _sampleEmail,
        'sample123',
        'sample123',
      ]);
      await _readTerms(
        tester,
        accept: false,
        expectedBody: 'What we may remove',
      );
      expect(
        tester.widget<FilledButton>(_primary(strings.requestCode)).onPressed,
        isNull,
      );
      expect(api.calls, isEmpty);
      final fields = find.byType(TextFormField);
      expect(
        tester.widget<TextFormField>(fields.at(0)).controller!.text,
        'sample_01',
      );
      expect(
        tester.widget<TextFormField>(fields.at(2)).controller!.text,
        _sampleEmail,
      );
      await _readTerms(
        tester,
        accept: true,
        expectedBody: 'What we may remove',
      );
      await _fill(tester, ['bad name']);
      await _tap(tester, _primary(strings.requestCode));
      expect(find.text(strings.usernameFormat), findsOneWidget);
      expect(api.calls, isEmpty);
      await _fill(tester, ['sample_01']);
      await _tap(tester, _primary(strings.requestCode));
      expect(api.calls, ['register']);
      expect(find.text(strings.usernameInUse), findsOneWidget);
      expect(find.text('此用户名已被使用'), findsNothing);
      expect(tester.takeException(), isNull);
    },
  );

  testWidgets(
    'Japanese reset remains operable with a simulated soft keyboard',
    (tester) async {
      final api = _AccountFixture()..captcha = true;
      await _openFromHome(
        tester,
        api,
        const Size(320, 640),
        locale: const Locale('ja'),
      );
      final strings = lookupLauncherStrings(const Locale('ja'));
      await _tap(tester, find.text(strings.signIn));
      await _fill(tester, [_sampleEmail, 'old-password']);
      await _tap(tester, find.byTooltip(strings.showPassword));
      expect(find.byTooltip(strings.hidePassword), findsOneWidget);
      await _tap(tester, find.text(strings.forgotPassword));

      tester.view.viewInsets = const FakeViewPadding(bottom: 240);
      addTearDown(tester.view.resetViewInsets);
      await tester.pumpAndSettle();
      await tester.ensureVisible(_primary(strings.requestCode));
      await tester.tap(_primary(strings.requestCode));
      await tester.pump();
      await tester.pump(const Duration(milliseconds: 350));
      expect(find.text(strings.captchaTitle), findsOneWidget);
      expect(find.text(strings.captchaLoadFailed), findsOneWidget);
      final cancel = find.text(strings.cancel);
      expect(cancel.hitTestable(), findsOneWidget);
      await tester.tap(cancel);
      await tester.pumpAndSettle();
      expect(
        tester
            .widget<TextFormField>(find.byType(TextFormField))
            .controller!
            .text,
        _sampleEmail,
      );

      api.captcha = false;
      await _tap(tester, _primary(strings.requestCode));
      await _fill(tester, ['999999', 'sample234', 'sample234'], start: 1);
      await _tap(tester, _primary(strings.resetPassword));
      expect(find.text(strings.invalidCode), findsOneWidget);
      await tester.enterText(find.byType(TextFormField).at(1), '234567');
      await _tap(tester, _primary(strings.resetPassword));
      expect(find.text(strings.passwordResetDone), findsOneWidget);
      await _tap(tester, _primary(strings.signIn));
      expect(
        tester
            .widget<TextFormField>(find.byType(TextFormField).last)
            .controller!
            .text,
        '',
      );
      await _tap(tester, find.byType(BackButton));
      expect(find.text(strings.register), findsOneWidget);
      expect(tester.takeException(), isNull);
    },
  );

  testWidgets(
    'Desktop password reset keeps entry and handles captcha cancellation',
    (tester) async {
      final api = _AccountFixture()..captcha = true;
      await _openFromHome(tester, api, const Size(1440, 734));
      await _tap(tester, find.text('登录'));
      await _fill(tester, [_sampleEmail, 'old-password']);
      await _tap(tester, find.text('忘记密码？'));
      final email = find.byType(TextFormField);
      expect(email, findsOneWidget);
      expect(
        tester.widget<TextFormField>(email).controller!.text,
        _sampleEmail,
      );

      await tester.tap(_primary('发送验证码'));
      await tester.pump();
      await tester.pump(const Duration(milliseconds: 350));
      expect(find.text('安全验证'), findsOneWidget);
      expect(find.text('无法加载安全验证，请重试'), findsOneWidget);
      await tester.tap(find.text('取消'));
      await tester.pumpAndSettle();
      expect(api.calls, ['captcha']);
      api.captcha = false;
      await _tap(tester, _primary('发送验证码'));
      final fields = find.byType(TextFormField);
      for (final (index, value) in [
        '999999',
        'sample234',
        'sample234',
      ].indexed) {
        final field = fields.at(index + 1);
        await tester.ensureVisible(field);
        await tester.enterText(field, value);
      }
      await tester.pumpAndSettle();
      await _tap(tester, _primary('重置密码'));
      expect(find.text('验证码有误或已过期'), findsOneWidget);
      await tester.enterText(find.byType(TextFormField).at(1), '234567');
      await _tap(tester, _primary('重置密码'));
      expect(find.text('密码已重置'), findsOneWidget);
      await _tap(tester, _primary('登录'));
      expect(
        tester
            .widget<TextFormField>(find.byType(TextFormField).first)
            .controller!
            .text,
        _sampleEmail,
      );
      expect(
        tester
            .widget<TextFormField>(find.byType(TextFormField).last)
            .controller!
            .text,
        '',
      );
      await _tap(tester, find.byType(BackButton));
      expect(find.text('注册'), findsOneWidget);
      expect(tester.takeException(), isNull);
    },
  );
}
