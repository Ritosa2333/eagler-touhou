import 'dart:async';
import 'dart:io';
import 'dart:convert';

import 'package:flutter/foundation.dart';

import 'package:flutter_test/flutter_test.dart';
import 'package:material_ui/material_ui.dart';
import 'package:shared_preferences/shared_preferences.dart';
import 'package:shared_preferences_platform_interface/in_memory_shared_preferences_async.dart';
import 'package:shared_preferences_platform_interface/shared_preferences_async_platform_interface.dart';
import 'package:touhouonline/account_api.dart';
import 'package:touhouonline/account_security.dart';
import 'package:touhouonline/catalog.dart';
import 'package:touhouonline/desktop_masthead.dart';
import 'package:touhouonline/l10n/strings.dart';
import 'package:touhouonline/library.dart';

import '../tools/capture_native_baseline_test.dart' show loadFonts, save;
import 'widget_test.dart' as launcher;

class _SecurityFixture extends launcher.FixtureAccountApi {
  final sends = <String>[];
  final emails = <(String, String)>[];
  final passwords = <(String, String)>[];
  bool captchaRequired = false;
  bool expired = false;
  int captchaRequests = 0;
  int checks = 0;
  Completer<void>? pendingPassword;

  @override
  Future<bool> requiresMailCaptcha() async => captchaRequired;

  @override
  Future<Map<String, dynamic>> mailCaptchaChallenge() async {
    captchaRequests++;
    throw const AccountRequestException(503, 'CAPTCHA_UNAVAILABLE');
  }

  @override
  Future<void> requestEmailChange(String email, String? captcha) async {
    if (expired) throw const AccountRequestException(401, 'SIGN_IN_REQUIRED');
    sends.add(email);
  }

  @override
  Future<void> changeEmail(String email, String otp) async {
    emails.add((email, otp));
    if (otp != '123456') {
      throw const AccountRequestException(400, 'INVALID_OTP');
    }
  }

  @override
  Future<void> changePassword(String current, String password) async {
    passwords.add((current, password));
    await pendingPassword?.future;
    if (current != 'fixture-old-password') {
      throw const AccountRequestException(400, 'INVALID_PASSWORD');
    }
  }

  @override
  Future<AccountIdentity?> session() async {
    checks++;
    return (username: 'fixture', name: '测试玩家', expiresAt: null);
  }
}

void _viewport(WidgetTester tester, Size size) {
  tester.view.devicePixelRatio = 1;
  tester.view.physicalSize = size;
  addTearDown(tester.view.resetDevicePixelRatio);
  addTearDown(tester.view.resetPhysicalSize);
}

Future<void> _tap(WidgetTester tester, Finder control) async {
  await tester.ensureVisible(control);
  await tester.tap(control);
  await tester.pumpAndSettle();
}

Finder _field(String suffix) => find.descendant(
  of: find.byKey(ValueKey('security-$suffix')),
  matching: find.byType(TextFormField),
);

Future<void> _enter(WidgetTester tester, Finder finder, String value) async {
  await tester.ensureVisible(finder);
  await tester.enterText(finder, value);
  await tester.pumpAndSettle();
}

Future<void> _openSecurity(WidgetTester tester, {required bool mobile}) async {
  final strings = tester.element(find.byType(LibraryPage)).strings;
  if (mobile) {
    await _tap(tester, find.text(strings.home));
    await _tap(tester, find.text(strings.signIn));
  } else {
    await _tap(tester, find.byTooltip(strings.signIn));
  }
  final credentials = find.byType(TextFormField);
  await tester.enterText(credentials.first, 'fixture');
  await tester.enterText(credentials.last, 'valid-test-value');
  await _tap(tester, find.widgetWithText(FilledButton, strings.signIn));
  if (mobile) {
    await _tap(tester, find.text(strings.accountSettings));
    await _tap(tester, find.text(strings.accountSecurity));
  } else {
    await _tap(tester, find.byTooltip(strings.account));
    await _tap(tester, find.text(strings.accountSettings));
  }
  expect(find.byType(AccountSecurityPage), findsOneWidget);
}

void main() {
  setUp(
    () => SharedPreferencesAsyncPlatform.instance =
        InMemorySharedPreferencesAsync.empty(),
  );

  testWidgets('320 px email change binds the delivered code to one address', (
    tester,
  ) async {
    _viewport(tester, const Size(320, 640));
    final fixture = _SecurityFixture()..captchaRequired = true;
    await tester.pumpWidget(launcher.fixture(accountApi: fixture));
    await tester.pumpAndSettle();
    await _openSecurity(tester, mobile: true);
    final send = find.text('获取验证码');
    final change = find.widgetWithText(OutlinedButton, '修改邮箱');
    final sendButton = find.widgetWithText(OutlinedButton, '获取验证码');
    expect(
      tester.getCenter(sendButton).dy,
      closeTo(tester.getCenter(_field('code')).dy, 2),
    );
    expect(
      tester.getTopRight(_field('code')).dx,
      lessThan(tester.getTopLeft(sendButton).dx),
    );

    await _tap(tester, send);
    expect(find.text('请输入有效的新邮箱地址'), findsOneWidget);
    await _enter(tester, _field('email'), 'new@example.invalid');
    await tester.tap(send);
    await tester.pump();
    await tester.pump(const Duration(milliseconds: 300));
    expect(fixture.captchaRequests, 1);
    await _tap(tester, find.text('取消'));
    expect(fixture.sends, isEmpty);
    fixture.captchaRequired = false;

    await _tap(tester, change);
    expect(find.text('请先向新邮箱获取验证码'), findsOneWidget);
    await _tap(tester, send);
    expect(fixture.sends, ['new@example.invalid']);
    expect(find.text('60s'), findsOneWidget);
    await _enter(tester, _field('code'), 'no012345');
    await _tap(tester, change);
    expect(find.text('验证码有误或已过期'), findsOneWidget);
    expect(fixture.emails, [('new@example.invalid', '012345')]);

    await _enter(tester, _field('email'), 'other@example.invalid');
    expect(tester.widget<TextFormField>(_field('code')).controller!.text, '');
    await _tap(tester, change);
    expect(fixture.emails, hasLength(1));
    await tester.pump(const Duration(seconds: 61));
    await tester.pumpAndSettle();
    await _tap(tester, send);
    expect(fixture.sends, ['new@example.invalid', 'other@example.invalid']);
    await _enter(tester, _field('code'), '123456');
    await _tap(tester, change);
    expect(find.text('邮箱已更换'), findsOneWidget);
    expect(fixture.emails.last, ('other@example.invalid', '123456'));
    expect(tester.widget<TextFormField>(_field('email')).controller!.text, '');
    expect(tester.widget<TextFormField>(_field('code')).controller!.text, '');

    await tester.binding.handlePopRoute();
    await tester.pumpAndSettle();
    expect(find.text('个人资料'), findsOneWidget);
    expect(find.byType(AccountSecurityPage), findsNothing);
    expect(tester.takeException(), isNull);
  });

  testWidgets(
    'Desktop password change refreshes the session and clears fields',
    (tester) async {
      _viewport(tester, const Size(1440, 734));
      final fixture = _SecurityFixture();
      await tester.pumpWidget(launcher.fixture(accountApi: fixture));
      await tester.pumpAndSettle();
      await _openSecurity(tester, mobile: false);
      expect(find.text('个人资料'), findsOneWidget);
      final submit = find.widgetWithText(OutlinedButton, '修改密码');
      await _enter(tester, _field('old-password'), 'wrong-fixture');
      await _enter(tester, _field('password'), 'fixture-new-password');
      await _enter(tester, _field('confirm-password'), 'mismatch');
      await _tap(tester, submit);
      expect(fixture.passwords, isEmpty);
      expect(find.text('两次输入的密码不一致'), findsOneWidget);
      await _enter(tester, _field('confirm-password'), 'fixture-new-password');
      await _tap(tester, submit);
      expect(find.text('当前密码不正确'), findsOneWidget);

      await _enter(tester, _field('old-password'), 'fixture-old-password');
      await _tap(tester, submit);
      expect(fixture.passwords.last, (
        'fixture-old-password',
        'fixture-new-password',
      ));
      expect(fixture.checks, 1);
      expect(find.text('密码已更改，其他会话已退出'), findsOneWidget);
      for (final suffix in ['old-password', 'password', 'confirm-password']) {
        expect(
          tester.widget<TextFormField>(_field(suffix)).controller!.text,
          '',
        );
      }
      final back = find.byKey(const ValueKey('security-back'));
      await _tap(tester, back);
      expect(find.byType(SearchBar), findsOneWidget);
      expect(find.byType(AccountSecurityPage), findsNothing);
      expect(tester.takeException(), isNull);
    },
  );

  testWidgets('masthead waits for a password request before leaving security', (
    tester,
  ) async {
    _viewport(tester, const Size(1440, 734));
    final pending = Completer<void>();
    final fixture = _SecurityFixture()..pendingPassword = pending;
    addTearDown(() => tester.pumpWidget(const SizedBox.shrink()));
    await tester.pumpWidget(launcher.fixture(accountApi: fixture));
    await tester.pumpAndSettle();
    await _openSecurity(tester, mobile: false);
    await _enter(tester, _field('old-password'), 'fixture-old-password');
    await _enter(tester, _field('password'), 'fixture-new-password');
    await _enter(tester, _field('confirm-password'), 'fixture-new-password');
    await tester.tap(find.widgetWithText(OutlinedButton, '修改密码'));
    await tester.pump();
    final library = find.descendant(
      of: find.byType(DesktopMasthead),
      matching: find.text('单机'),
    );
    await tester.tap(library);
    await tester.pump(const Duration(milliseconds: 400));
    expect(find.byType(AccountSecurityPage), findsOneWidget);
    expect(fixture.passwords, hasLength(1));
    pending.complete();
    await tester.pumpAndSettle();
    await _tap(tester, library);
    expect(find.byType(AccountSecurityPage), findsNothing);
    expect(find.byType(SearchBar), findsOneWidget);
    expect(tester.takeException(), isNull);
  });

  for (final language in [
    LauncherLanguage.english,
    LauncherLanguage.japanese,
  ]) {
    testWidgets(
      'Account security translates validation and feedback at 320 px | ${language.tag}',
      (tester) async {
        _viewport(tester, const Size(320, 640));
        final fixture = _SecurityFixture();
        await tester.pumpWidget(
          launcher.fixture(
            accountApi: fixture,
            preferences: LauncherPreferences(
              SharedPreferencesAsync(),
              ThemeMode.dark,
              true,
              language: language,
            ),
          ),
        );
        await tester.pumpAndSettle();
        await _openSecurity(tester, mobile: true);
        final strings = lookupLauncherStrings(language.locale);
        final send = find.widgetWithText(OutlinedButton, strings.getCode);
        await _tap(tester, send);
        expect(find.text(strings.newEmailInvalid), findsOneWidget);
        await _enter(tester, _field('email'), 'other@example.invalid');
        await _tap(tester, send);
        expect(fixture.sends, ['other@example.invalid']);
        expect(find.text(strings.codeCooldown(60)), findsOneWidget);
        await _enter(tester, _field('code'), '123456');
        await _tap(
          tester,
          find.widgetWithText(OutlinedButton, strings.changeEmail),
        );
        expect(find.text(strings.emailChanged), findsOneWidget);
        await tester.binding.handlePopRoute();
        await tester.pumpAndSettle();
        expect(find.text(strings.personalProfile), findsOneWidget);
        expect(tester.takeException(), isNull);
      },
    );
  }

  testWidgets('Session expiration removes the editable fields', (tester) async {
    _viewport(tester, const Size(390, 844));
    final fixture = _SecurityFixture()..expired = true;
    await tester.pumpWidget(launcher.fixture(accountApi: fixture));
    await tester.pumpAndSettle();
    await _openSecurity(tester, mobile: true);
    await _enter(tester, _field('email'), 'new@example.invalid');
    await _tap(tester, find.text('获取验证码'));
    expect(find.text('登录后管理账户安全'), findsOneWidget);
    expect(find.byType(TextFormField), findsNothing);
  });

  testWidgets(
    'Security keeps drafts and focus across desktop and phone layouts',
    (tester) async {
      _viewport(tester, const Size(1440, 734));
      final fixture = _SecurityFixture();
      await tester.pumpWidget(launcher.fixture(accountApi: fixture));
      await tester.pumpAndSettle();
      await _openSecurity(tester, mobile: false);
      await _enter(tester, _field('old-password'), 'draft-password');
      final editable = find.descendant(
        of: _field('old-password'),
        matching: find.byType(EditableText),
      );
      final focus = tester.widget<EditableText>(editable).focusNode;
      for (final size in [const Size(390, 844), const Size(1440, 734)]) {
        tester.view.physicalSize = size;
        await tester.pumpAndSettle();
        expect(
          tester.widget<TextFormField>(_field('old-password')).controller!.text,
          'draft-password',
        );
        expect(tester.widget<EditableText>(editable).focusNode, same(focus));
        expect(focus.hasFocus, isTrue);
        expect(tester.takeException(), isNull);
      }
    },
  );

  testWidgets(
    'Security code countdown keeps the input and action widths stable',
    (tester) async {
      _viewport(tester, const Size(320, 640));
      await tester.pumpWidget(launcher.fixture(accountApi: _SecurityFixture()));
      await tester.pumpAndSettle();
      await _openSecurity(tester, mobile: true);
      await _enter(tester, _field('email'), 'new@example.invalid');
      final button = find.widgetWithText(OutlinedButton, '获取验证码');
      await tester.ensureVisible(button);
      await tester.pumpAndSettle();
      final before = tester.getRect(_field('code'));
      final buttonWidth = tester.getSize(button).width;
      await _tap(tester, button);
      await tester.pump(const Duration(seconds: 1));
      await tester.pumpAndSettle();
      final after = tester.getRect(_field('code'));
      final countdown = find.widgetWithText(OutlinedButton, '59s');
      expect(countdown, findsOneWidget);
      expect(after.left, before.left);
      expect(after.width, before.width);
      expect(tester.getSize(countdown).width, buttonWidth);
    },
  );

  final outputPath = Platform.environment['TOUHOU_ACCOUNT_SECURITY_CAPTURE'];
  if (outputPath != null) {
    final output = Directory(outputPath)..createSync(recursive: true);
    setUpAll(
      () => loadFonts(Directory(Platform.environment['TOUHOU_CAPTURE_FONTS']!)),
    );
    for (final language in LauncherLanguage.values) {
      for (final (device, size) in [
        ('desktop', const Size(1440, 734)),
        ('mobile', const Size(390, 844)),
        ('mobile-narrow', const Size(320, 640)),
      ]) {
        final suffix = language == LauncherLanguage.simplifiedChinese
            ? ''
            : '-${language.tag}';
        testWidgets('capture $device$suffix account security', (tester) async {
          debugDefaultTargetPlatformOverride = TargetPlatform.windows;
          addTearDown(() => debugDefaultTargetPlatformOverride = null);
          _viewport(tester, size);
          final boundary = GlobalKey();
          await tester.pumpWidget(
            RepaintBoundary(
              key: boundary,
              child: launcher.fixture(
                accountApi: _SecurityFixture(),
                preferences: LauncherPreferences(
                  SharedPreferencesAsync(),
                  ThemeMode.dark,
                  true,
                  language: language,
                ),
              ),
            ),
          );
          await tester.pumpAndSettle();
          await tester.runAsync(
            () => precacheImage(
              const AssetImage('assets/launcher-wordmark-dark.png'),
              tester.element(find.byType(LibraryPage)),
            ),
          );
          await tester.pumpAndSettle();
          await _openSecurity(tester, mobile: device != 'desktop');
          await tester.runAsync(
            () => precacheImage(
              const AssetImage('assets/launcher-wordmark-dark.png'),
              tester.element(find.byType(AccountSecurityPage)),
            ),
          );
          await tester.pumpAndSettle();
          await save(
            tester,
            boundary,
            output,
            '$device-account-security$suffix',
          );
          List<double> box(Finder finder) {
            final rect = tester.getRect(finder);
            return [rect.left, rect.top, rect.right, rect.bottom];
          }

          File('${output.path}/$device-account-security$suffix.geometry.json')
              .writeAsStringSync(
                const JsonEncoder.withIndent('  ').convert({
                  'viewport': [size.width, size.height],
                  'email': box(_field('email')),
                  'code': box(_field('code')),
                  'codeButton': box(
                    find.widgetWithText(
                      OutlinedButton,
                      tester.element(_field('email')).strings.getCode,
                    ),
                  ),
                }),
              );
          await Scrollable.ensureVisible(
            tester.element(_field('old-password')),
            alignment: 0,
          );
          await tester.pumpAndSettle();
          await save(
            tester,
            boundary,
            output,
            '$device-account-security-password$suffix',
          );
          expect(tester.takeException(), isNull);
        });
      }
    }
  }
}
