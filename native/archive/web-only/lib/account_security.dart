import 'dart:async';

import 'package:flutter/services.dart';
import 'package:go_router/go_router.dart';
import 'package:material_ui/material_ui.dart';

import 'account.dart';
import 'account_api.dart';
import 'account_fields.dart';
import 'mail_captcha.dart';
import 'l10n/strings.dart';
import 'account_settings_layout.dart';
import 'launcher_select.dart';
import 'launcher_navigation.dart' show LauncherExitGuard;
import 'profile_social.dart';

class AccountSecurityPage extends StatelessWidget {
  const AccountSecurityPage({super.key, required this.session, this.exitGuard});

  final AccountSession session;
  final LauncherExitGuard? exitGuard;

  @override
  Widget build(BuildContext context) => ListenableBuilder(
    listenable: session,
    builder: (context, _) {
      final member = session.identity;
      if (member == null) {
        return AccountSettingsLayout(
          section: AccountSettingsSection.security,
          username: null,
          title: context.strings.accountSecurity,
          onBack: () => profileBack(context, '/'),
          child: Align(
            alignment: AlignmentDirectional.centerStart,
            child: FilledButton(
              onPressed: () => context.push('/login'),
              child: Text(context.strings.signInForSecurity),
            ),
          ),
        );
      }
      return _SecurityForm(
        key: ValueKey(member.username),
        session: session,
        username: member.username,
        exitGuard: exitGuard,
      );
    },
  );
}

class _SecurityForm extends StatefulWidget {
  const _SecurityForm({
    super.key,
    required this.session,
    required this.username,
    this.exitGuard,
  });
  final AccountSession session;
  final String username;
  final LauncherExitGuard? exitGuard;

  @override
  State<_SecurityForm> createState() => _SecurityFormState();
}

enum _SecurityAction { sendCode, changeEmail, changePassword }

class _SecurityFormState extends State<_SecurityForm> {
  VoidCallback? _unbindExit;

  @override
  void initState() {
    super.initState();
    _unbindExit = widget.exitGuard?.bind(() async => !_busy);
  }

  final _emailForm = GlobalKey<FormState>();
  final _passwordForm = GlobalKey<FormState>();
  final _email = TextEditingController();
  final _code = TextEditingController();
  final _oldPassword = TextEditingController();
  final _newPassword = TextEditingController();
  final _confirmation = TextEditingController();
  final _codeFocus = FocusNode();
  final _codeFormatters = [
    FilteringTextInputFormatter.digitsOnly,
    LengthLimitingTextInputFormatter(6),
  ];

  String? _sentTo;
  Timer? _cooldown;
  int _remaining = 0;
  _SecurityAction? _working;
  String? _emailStatus;
  bool _emailFailed = false;
  String? _passwordStatus;
  bool _passwordFailed = false;
  bool get _busy => _working != null;
  AccountApi get _api => widget.session.api;
  LauncherStrings get _strings => context.strings;

  void _startCooldown() {
    const duration = 60;
    _cooldown?.cancel();
    _remaining = duration;
    _cooldown = Timer.periodic(const Duration(seconds: 1), (timer) {
      setState(() => _remaining = duration - timer.tick);
      if (_remaining == 0) timer.cancel();
    });
  }

  void _status(_SecurityAction action, String? message, {bool failed = false}) {
    if (action == _SecurityAction.changePassword) {
      _passwordStatus = message;
      _passwordFailed = failed;
    } else {
      _emailStatus = message;
      _emailFailed = failed;
    }
  }

  Future<void> _run(
    _SecurityAction action,
    Future<void> Function() submit,
  ) async {
    if (_busy) return;
    FocusScope.of(context).unfocus();
    setState(() {
      _working = action;
      _status(action, null);
    });
    try {
      await submit();
    } catch (error) {
      if (!mounted) return;
      widget.session.expireIfUnauthorized(error);
      final request = error is AccountRequestException ? error : null;
      final message = switch (request?.code) {
        'INVALID_EMAIL' => _strings.newEmailInvalid,
        'INVALID_OTP' ||
        'OTP_EXPIRED' ||
        'INVALID_EMAIL_OTP' => _strings.invalidCode,
        'TOO_MANY_ATTEMPTS' => _strings.codeAttemptsExhausted,
        'INVALID_PASSWORD' => _strings.invalidCurrentPassword,
        'PASSWORD_TOO_SHORT' ||
        'PASSWORD_TOO_LONG' => _strings.newPasswordRange,
        'CAPTCHA_INVALID' => _strings.captchaInvalid,
        'CAPTCHA_UNAVAILABLE' => _strings.captchaUnavailable,
        _ when request?.status == 429 => _strings.tooManyAttempts,
        _ => _strings.securityRequestFailed,
      };
      setState(() => _status(action, message, failed: true));
    } finally {
      if (mounted) setState(() => _working = null);
    }
  }

  void _sendCode() {
    if (_remaining != 0) return;
    final email = _email.text.trim();
    if (!validAccountEmail(email)) {
      setState(() {
        _emailStatus = _strings.newEmailInvalid;
        _emailFailed = true;
      });
      return;
    }
    _run(_SecurityAction.sendCode, () async {
      final required = await _api.requiresMailCaptcha();
      if (!mounted) return;
      final proof = required ? await showMailCaptcha(context, _api) : null;
      if (!mounted || required && proof == null) return;
      await _api.requestEmailChange(email, proof);
      if (!mounted) return;
      setState(() {
        _sentTo = email;
        _code.clear();
        _startCooldown();
        _emailStatus = _strings.codeSent;
        _emailFailed = false;
      });
      WidgetsBinding.instance.addPostFrameCallback((_) {
        if (mounted) _codeFocus.requestFocus();
      });
    });
  }

  void _changeEmail() {
    final email = _email.text.trim();
    if (_sentTo != email) {
      setState(() {
        _emailStatus = validAccountEmail(email)
            ? _strings.newEmailCodeRequired
            : _strings.newEmailInvalid;
        _emailFailed = true;
      });
      return;
    }
    if (_emailForm.currentState?.validate() != true) return;
    _run(_SecurityAction.changeEmail, () async {
      await _api.changeEmail(email, _code.text);
      if (!mounted) return;
      setState(() {
        _sentTo = null;
        _emailForm.currentState?.reset();
        _code.clear();
        _email.clear();
        _emailStatus = _strings.emailChanged;
        _emailFailed = false;
      });
    });
  }

  void _changePassword() {
    if (_passwordForm.currentState?.validate() != true) return;
    _run(_SecurityAction.changePassword, () async {
      await _api.changePassword(_oldPassword.text, _newPassword.text);
      if (!mounted) return;
      setState(() {
        _passwordForm.currentState?.reset();
        _oldPassword.clear();
        _newPassword.clear();
        _confirmation.clear();
        _passwordStatus = _strings.passwordChanged;
        _passwordFailed = false;
      });
      // The mutation can rotate the current session cookie and expiry.
      await widget.session.refreshIdentity().catchError((Object error) {
        widget.session.expireIfUnauthorized(error);
      });
    });
  }

  @override
  void dispose() {
    _unbindExit?.call();
    _cooldown?.cancel();
    _codeFocus.dispose();
    for (final controller in [
      _email,
      _code,
      _oldPassword,
      _newPassword,
      _confirmation,
    ]) {
      controller.dispose();
    }
    super.dispose();
  }

  double? get _labelWidth =>
      AccountSettingsLayout.wide(context) &&
          MediaQuery.textScalerOf(context).scale(14) <= 18.2
      ? 116
      : null;

  @override
  Widget build(BuildContext context) => PopScope(
    canPop: !_busy,
    child: AccountSettingsLayout(
      section: AccountSettingsSection.security,
      username: widget.username,
      title: context.strings.accountSecurity,
      backKey: const ValueKey('security-back'),
      onBack: _busy ? null : () => profileBack(context, '/'),
      busy: _busy,
      progress: _busy,
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.stretch,
        children: [
          Padding(
            padding: const EdgeInsets.only(bottom: 24),
            child: _emailSection(),
          ),
          const Divider(height: 1),
          Padding(
            padding: const EdgeInsets.only(top: 24),
            child: _passwordSection(),
          ),
        ],
      ),
    ),
  );

  Widget _emailSection() => _section(
    title: _strings.changeEmail,
    form: _emailForm,

    submit: _changeEmail,
    status: _emailStatus,
    failed: _emailFailed,
    fields: [
      AccountField(
        key: const ValueKey('security-email'),
        label: _strings.newEmail,
        labelWidth: _labelWidth,
        controller: _email,
        enabled: !_busy,
        keyboard: TextInputType.emailAddress,
        autofill: AutofillHints.email,
        validator: (value) =>
            validAccountEmail(value) ? null : _strings.newEmailInvalid,
        onChanged: (_) {
          setState(() {
            _sentTo = null;
            _code.clear();
            _emailStatus = null;
          });
        },
      ),
      AccountField(
        key: const ValueKey('security-code'),
        label: _strings.emailCode,
        labelWidth: _labelWidth,
        controller: _code,
        enabled: !_busy,
        keyboard: TextInputType.number,
        autofill: AutofillHints.oneTimeCode,
        formatters: _codeFormatters,
        focusNode: _codeFocus,
        validator: (value) => value?.length == 6 ? null : _strings.sixDigitCode,
        onSubmitted: (_) => _changeEmail(),
        inputAccessory: SizedBox(
          width:
              [_strings.getCode, _strings.codeCooldown(60)]
                  .map((label) => LauncherControl.labelWidth(context, label))
                  .reduce((a, b) => a > b ? a : b) +
              20,
          child: OutlinedButton(
            style: OutlinedButton.styleFrom(
              textStyle: LauncherControl.label(context),
            ),
            onPressed: _busy || _remaining > 0 ? null : _sendCode,
            child: Text(
              _remaining > 0
                  ? _strings.codeCooldown(_remaining)
                  : _strings.getCode,
            ),
          ),
        ),
      ),
    ],
  );

  Widget _passwordSection() => _section(
    title: _strings.changePassword,
    form: _passwordForm,

    submit: _changePassword,
    status: _passwordStatus,
    failed: _passwordFailed,
    fields: [
      AccountField(
        key: const ValueKey('security-old-password'),
        label: _strings.currentPassword,
        labelWidth: _labelWidth,
        controller: _oldPassword,
        enabled: !_busy,
        password: true,
        autofill: AutofillHints.password,
        validator: (value) =>
            value?.isNotEmpty == true ? null : _strings.currentPasswordRequired,
      ),
      AccountField(
        key: const ValueKey('security-password'),
        label: _strings.newPassword,
        labelWidth: _labelWidth,
        controller: _newPassword,
        enabled: !_busy,
        password: true,
        autofill: AutofillHints.newPassword,
        validator: (value) =>
            (value?.length ?? 0) >= 8 ? null : _strings.newPasswordLength,
      ),
      AccountField(
        key: const ValueKey('security-confirm-password'),
        label: _strings.confirmNewPassword,
        labelWidth: _labelWidth,
        controller: _confirmation,
        enabled: !_busy,
        password: true,
        autofill: AutofillHints.newPassword,
        validator: (value) =>
            value == _newPassword.text ? null : _strings.passwordMismatch,
        onSubmitted: (_) => _changePassword(),
      ),
    ],
  );

  Widget _section({
    required String title,
    required GlobalKey<FormState> form,
    required List<Widget> fields,

    required VoidCallback submit,
    required String? status,
    required bool failed,
  }) {
    final theme = Theme.of(context);
    return AutofillGroup(
      onDisposeAction: AutofillContextAction.cancel,
      child: Form(
        key: form,
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.stretch,
          spacing: 16,
          children: [
            Semantics(
              header: true,
              child: Text(
                title,
                style: theme.textTheme.titleMedium?.copyWith(
                  fontSize: 15,
                  fontWeight: FontWeight.w600,
                ),
              ),
            ),
            ...fields,
            if (status != null)
              Semantics(
                liveRegion: true,
                child: Text(
                  status,
                  style: failed
                      ? TextStyle(color: theme.colorScheme.error)
                      : null,
                ),
              ),
            Padding(
              padding: EdgeInsetsDirectional.only(
                start: _labelWidth == null ? 0 : _labelWidth! + 20,
                top: 0,
              ),
              child: Align(
                alignment: AlignmentDirectional.centerStart,
                child: OutlinedButton(
                  onPressed: _busy ? null : submit,
                  child: Text(title),
                ),
              ),
            ),
          ],
        ),
      ),
    );
  }
}
