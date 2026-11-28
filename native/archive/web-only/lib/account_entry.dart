import 'dart:async';

import 'package:flutter/services.dart';
import 'package:go_router/go_router.dart';
import 'package:material_ui/material_ui.dart';

import 'account.dart';
import 'account_api.dart';
import 'account_documents.dart';
import 'account_fields.dart';
import 'mail_captcha.dart';
import 'l10n/strings.dart';

enum _Mode {
  login,
  register,
  reset;

  String title(LauncherStrings strings) => switch (this) {
    login => strings.signIn,
    register => strings.registerTitle,
    reset => strings.resetPassword,
  };
}

class AccountEntry extends StatefulWidget {
  const AccountEntry({super.key, required this.session, this.startingMode});

  final AccountSession session;
  final String? startingMode;

  @override
  State<AccountEntry> createState() => _AccountEntryState();
}

class _AccountEntryState extends State<AccountEntry> {
  final _form = GlobalKey<FormState>();
  final _identity = TextEditingController();
  final _username = TextEditingController();
  final _nickname = TextEditingController();
  final _email = TextEditingController();
  final _password = TextEditingController();
  final _confirmation = TextEditingController();
  final _code = TextEditingController();
  final _codeFocus = FocusNode();
  final _termsFocus = FocusNode();

  late _Mode _mode = switch (widget.startingMode) {
    'register' => _Mode.register,
    'reset' => _Mode.reset,
    _ => _Mode.login,
  };
  bool _busy = false;
  bool _terms = false;
  bool _resetSent = false;
  bool _resetDone = false;
  bool _registrationExpired = false;
  RegistrationChallenge? _challenge;
  String? _issue;
  String? _notice;

  Timer? _cooldown;
  Timer? _expiry;
  int _remaining = 0;

  bool get _login => _mode == _Mode.login;
  bool get _register => _mode == _Mode.register;
  bool get _sent => _challenge != null || _resetSent;
  bool get _canSubmit =>
      !_busy && (!_register || _terms) && !_registrationExpired;
  AccountApi get _api => widget.session.api;
  LauncherStrings get _strings => context.strings;

  void _returnToEntry() {
    if (context.canPop()) {
      context.pop();
    } else {
      context.go('/home');
    }
  }

  void _back() {
    if (_busy) return;
    if (_mode == _Mode.reset) {
      _switchMode(_Mode.login);
    } else {
      _returnToEntry();
    }
  }

  void _clearChallenge() {
    _cooldown?.cancel();
    _expiry?.cancel();
    _challenge = null;
    _resetSent = false;
    _registrationExpired = false;

    _remaining = 0;
    _code.clear();
  }

  void _switchMode(_Mode next, {String? notice}) {
    FocusScope.of(context).unfocus();
    final identity = _identity.text;
    _form.currentState?.reset();
    _identity.text = identity;
    setState(() {
      _mode = next;
      _clearChallenge();
      _password.clear();
      _confirmation.clear();
      _terms = false;
      _resetDone = false;
      _issue = null;
      _notice = notice;
      if (next == _Mode.reset && _identity.text.contains('@')) {
        _email.text = _identity.text;
      }
    });
  }

  Future<void> _consent(bool? checked) async {
    if (checked != true) {
      setState(() => _terms = false);
      return;
    }
    final accepted = await openAccountDocument(
      context,
      AccountDocument.terms,
      consent: true,
    );
    if (!mounted) return;
    setState(() => _terms = accepted);
    _termsFocus.requestFocus();
  }

  void _startCooldown() {
    _cooldown?.cancel();
    const secondsBetweenSends = 60;
    _remaining = secondsBetweenSends;
    _cooldown = Timer.periodic(const Duration(seconds: 1), (timer) {
      final seconds = (secondsBetweenSends - timer.tick).clamp(
        0,
        secondsBetweenSends,
      );
      if (seconds == 0) timer.cancel();
      if (seconds != _remaining) setState(() => _remaining = seconds);
    });
  }

  void _watchExpiry(RegistrationChallenge challenge) {
    _expiry?.cancel();
    _expiry = Timer(challenge.expiresAt.difference(DateTime.now()), () {
      if (_challenge?.id == challenge.id) {
        setState(() {
          _registrationExpired = true;
          _notice = null;
        });
      }
    });
  }

  Future<void> _sendCode() async {
    if (!_canSubmit || _remaining != 0) return;
    if (!_sent && _form.currentState?.validate() != true) return;
    FocusScope.of(context).unfocus();
    setState(() {
      _issue = null;
      _busy = true;
    });
    try {
      final needsCaptcha = await _api.requiresMailCaptcha();
      if (!mounted) return;
      final captcha = needsCaptcha
          ? await showMailCaptcha(context, _api)
          : null;
      if (!mounted || needsCaptcha && captcha == null) return;

      final email = _email.text.trim();
      final current = _challenge;
      if (_register) {
        final next = current == null
            ? await _api.startRegistration(
                email: email,
                username: _username.text.trim(),
                nickname: _nickname.text.trim(),
                password: _password.text,
                captcha: captcha,
              )
            : await _api.resendRegistration(
                challenge: current,
                email: email,
                password: _password.text,
                captcha: captcha,
              );
        if (!mounted) return;
        _challenge = next;
        _watchExpiry(next);
      } else {
        await _api.requestPasswordReset(email, captcha);
        if (!mounted) return;
        _resetSent = true;
      }
      setState(() {
        _notice = _register ? _strings.codeSent : _strings.resetCodeSent;
        _startCooldown();
      });
      WidgetsBinding.instance.addPostFrameCallback((_) {
        if (mounted && _sent && !_registrationExpired) {
          _codeFocus.requestFocus();
        }
      });
    } catch (error) {
      _showFailure(error);
    } finally {
      if (mounted) setState(() => _busy = false);
    }
  }

  Future<void> _submit() async {
    if (!_canSubmit) return;
    if (!_login && !_sent) {
      await _sendCode();
      return;
    }
    if (_form.currentState?.validate() != true) return;
    FocusScope.of(context).unfocus();
    setState(() {
      _issue = null;
      _busy = true;
    });
    try {
      if (_login) {
        await widget.session.signIn(_identity.text.trim(), _password.text);
        if (mounted) _returnToEntry();
        return;
      }

      final email = _email.text.trim();
      final otp = _code.text;
      if (_challenge case final RegistrationChallenge challenge) {
        await _api.verifyRegistration(
          challenge: challenge,
          email: email,
          password: _password.text,
          otp: otp,
        );
        if (!mounted) return;
        var signedIn = false;
        try {
          signedIn = await widget.session.refreshAfterRegistration();
        } catch (_) {
          // Registration has finished. The new account can sign in separately.
        }
        if (!mounted) return;
        if (signedIn) {
          _returnToEntry();
        } else {
          _identity.text = email;
          _switchMode(_Mode.login, notice: _strings.registrationDone);
        }
      } else {
        await _api.resetPassword(email, otp, _password.text);
        if (!mounted) return;
        setState(() {
          _clearChallenge();
          _password.clear();
          _confirmation.clear();
          _notice = _strings.passwordResetDone;
          _resetDone = true;
        });
      }
    } catch (error) {
      _showFailure(error, verifying: !_login);
    } finally {
      if (mounted) setState(() => _busy = false);
    }
  }

  void _showFailure(Object error, {bool verifying = false}) {
    if (!mounted) return;
    final failure = error is AccountRequestException ? error : null;
    setState(() {
      _notice = null;
      if (_register && failure?.status == 410) {
        _registrationExpired = true;
        return;
      }
      if (_register && failure?.code == 'CONTENT_REJECTED') {
        _clearChallenge();
      }
      _issue = switch (failure?.code) {
        'CAPTCHA_INVALID' => _strings.captchaInvalid,
        'CAPTCHA_UNAVAILABLE' => _strings.captchaUnavailable,
        'USERNAME_IS_ALREADY_TAKEN' ||
        'USERNAME_ALREADY_EXISTS' => _strings.usernameInUse,
        'CONTENT_REJECTED' => _strings.nicknameRejected,
        'REVIEW_UNAVAILABLE' => _strings.nicknameReviewUnavailable,
        'INVALID_EMAIL_OR_PASSWORD' ||
        'INVALID_USERNAME_OR_PASSWORD' => _strings.invalidCredentials,
        'USER_ALREADY_EXISTS' ||
        'USER_ALREADY_EXISTS_USE_ANOTHER_EMAIL' => _strings.emailInUse,
        'INVALID_OTP' ||
        'INVALID_EMAIL_OTP' ||
        'OTP_EXPIRED' ||
        'VERIFICATION_CODE_EXPIRED' => _strings.invalidCode,
        _ => switch (failure?.status) {
          400 when verifying => _strings.invalidCode,
          401 when _login => _strings.invalidCredentials,
          409 when _register => _strings.emailInUse,
          429 => _strings.tooManyAttempts,
          _ => _strings.accountRequestFailed,
        },
      };
    });
  }

  @override
  void dispose() {
    _cooldown?.cancel();
    _expiry?.cancel();
    for (final controller in [
      _identity,
      _username,
      _nickname,
      _email,
      _password,
      _confirmation,
      _code,
    ]) {
      controller.dispose();
    }
    _codeFocus.dispose();
    _termsFocus.dispose();
    super.dispose();
  }

  Widget _message(String text, {bool error = false}) => Semantics(
    liveRegion: true,
    child: Text(
      text,
      style: TextStyle(
        color: error ? Theme.of(context).colorScheme.error : null,
      ),
    ),
  );

  Widget _codeField() => AccountField(
    label: _strings.emailCode,
    controller: _code,
    focusNode: _codeFocus,
    enabled: !_busy && !_registrationExpired,
    keyboard: TextInputType.number,
    autofill: AutofillHints.oneTimeCode,
    formatters: [
      FilteringTextInputFormatter.digitsOnly,
      LengthLimitingTextInputFormatter(6),
    ],
    validator: (value) => value?.length == 6 ? null : _strings.sixDigitCode,
    onSubmitted: _register ? (_) => _submit() : null,
    accessory: TextButton(
      onPressed: _canSubmit && _remaining == 0 ? _sendCode : null,
      child: Text(
        _remaining > 0
            ? _strings.codeCooldown(_remaining)
            : _strings.resendCode,
      ),
    ),
  );

  Widget _buildForm(ThemeData theme) {
    final locked = _busy || _sent;
    final needsPassword = _login || _register || _sent;
    final titleSize = MediaQuery.sizeOf(context).width <= 540 ? 24.0 : 28.0;
    return AutofillGroup(
      onDisposeAction: AutofillContextAction.cancel,
      child: Form(
        key: _form,
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.stretch,
          spacing: 16,
          children: [
            Padding(
              padding: const EdgeInsets.only(bottom: 8),
              child: Text(
                _mode.title(_strings),
                style: theme.textTheme.headlineMedium?.copyWith(
                  fontSize: titleSize,
                  fontWeight: FontWeight.w600,
                  height: 1.2,
                  letterSpacing: -titleSize * .025,
                ),
              ),
            ),
            if (_login)
              AccountField(
                label: _strings.accountIdentity,
                controller: _identity,
                autofill: AutofillHints.username,
                enabled: !_busy,
                validator: (value) => value?.trim().isNotEmpty == true
                    ? null
                    : _strings.identityRequired,
              ),
            if (_register) ...[
              AccountField(
                label: _strings.username,
                controller: _username,
                autofill: AutofillHints.newUsername,
                enabled: !locked,
                formatters: [LengthLimitingTextInputFormatter(12)],
                help: _strings.usernameFormat,
                validator: (value) =>
                    RegExp(r'^[A-Za-z0-9_.]{3,12}$')
                        .hasMatch(value?.trim() ?? '')
                    ? null
                    : _strings.usernameFormat,
              ),
              AccountField(
                label: _strings.nickname,
                controller: _nickname,
                enabled: !locked,
                formatters: [LengthLimitingTextInputFormatter(12)],
                help: _strings.nicknameLength,
                validator: (value) => value?.trim().isNotEmpty == true
                    ? null
                    : _strings.nicknameRequired,
              ),
            ],
            if (!_login && !_resetDone)
              AccountField(
                label: _strings.email,
                controller: _email,
                enabled: !locked,
                hint: 'name@example.com',
                keyboard: TextInputType.emailAddress,
                autofill: AutofillHints.email,
                validator: (value) =>
                    validAccountEmail(value) ? null : _strings.emailInvalid,
              ),
            if (!_login && !_register && _sent) _codeField(),
            if (needsPassword)
              AccountField(
                key: ValueKey((_mode, 'password')),
                label: _login || _register
                    ? _strings.password
                    : _strings.newPassword,
                controller: _password,
                enabled: !_busy && (!_register || !_sent),
                password: true,
                autofill: _login
                    ? AutofillHints.password
                    : AutofillHints.newPassword,
                hint: _login ? null : _strings.passwordHint,
                validator: (value) => (value?.length ?? 0) >= (_login ? 1 : 8)
                    ? null
                    : (_login
                          ? _strings.passwordRequired
                          : _strings.passwordLength),
                accessory: _login
                    ? TextButton(
                        onPressed: _busy
                            ? null
                            : () => _switchMode(_Mode.reset),
                        child: Text(_strings.forgotPassword),
                      )
                    : null,
                onSubmitted: _login ? (_) => _submit() : null,
              ),
            if (needsPassword && !_login)
              AccountField(
                key: ValueKey((_mode, 'confirmation')),
                label: _strings.confirmPassword,
                controller: _confirmation,
                enabled: !_busy && (!_register || !_sent),
                password: true,
                autofill: AutofillHints.newPassword,
                validator: (value) =>
                    value == _password.text ? null : _strings.passwordMismatch,
              ),
            if (_register && _sent) _codeField(),
            if (_issue case final String message)
              _message(message, error: true),
            if (_notice case final String message) _message(message),
            if (_registrationExpired)
              _message(_strings.codeExpired, error: true),
            if (_register)
              CheckboxListTile(
                value: _terms,
                focusNode: _termsFocus,
                onChanged: _busy ? null : _consent,
                controlAffinity: ListTileControlAffinity.leading,
                contentPadding: EdgeInsets.zero,
                title: Text(_strings.termsConsent),
              ),
            FilledButton(
              onPressed: _resetDone
                  ? () => _switchMode(_Mode.login)
                  : _canSubmit
                  ? _submit
                  : null,
              child: Text(
                _resetDone || _login
                    ? _strings.signIn
                    : !_sent
                    ? _strings.requestCode
                    : _register
                    ? _strings.register
                    : _strings.resetPassword,
              ),
            ),
            if (_registrationExpired || !_login && !_register && _sent)
              TextButton(
                onPressed: _busy
                    ? null
                    : () => setState(() {
                        _clearChallenge();
                        _issue = null;
                        _notice = null;
                      }),
                child: Text(_strings.editRegistration),
              ),
            if (_login || _register)
              TextButton(
                onPressed: _busy
                    ? null
                    : () => _switchMode(_login ? _Mode.register : _Mode.login),
                child: Text(
                  _login ? _strings.registerTitle : _strings.backToSignIn,
                ),
              ),
            if (_register)
              TextButton(
                onPressed: _busy
                    ? null
                    : () =>
                          openAccountDocument(context, AccountDocument.privacy),
                child: Text(_strings.privacy),
              ),
          ],
        ),
      ),
    );
  }

  @override
  Widget build(BuildContext context) {
    final theme = Theme.of(context);
    final wide = MediaQuery.sizeOf(context).width > 780;
    final gutter = wide
        ? 48.0
        : MediaQuery.sizeOf(context).width <= 540
        ? 24.0
        : 32.0;
    final panelWidth = (MediaQuery.sizeOf(context).width * .44).clamp(
      480.0,
      720.0,
    );
    return PopScope(
      canPop: _mode != _Mode.reset && !_busy,
      onPopInvokedWithResult: (didPop, _) {
        if (!didPop && _mode == _Mode.reset) _back();
      },
      child: Scaffold(
        body: Stack(
          children: [
            if (wide)
              Positioned.fill(
                child: Image.asset(
                  'assets/account-background.webp',
                  fit: BoxFit.cover,
                  alignment: const Alignment(0, -.16),
                  excludeFromSemantics: true,
                  color: theme.colorScheme.surface.withValues(alpha: .28),
                  colorBlendMode: BlendMode.srcOver,
                ),
              ),
            Align(
              alignment: Alignment.topRight,
              child: SizedBox(
                width: wide ? panelWidth : double.infinity,
                height: double.infinity,
                child: Material(
                  color: theme.colorScheme.surface,
                  child: SafeArea(
                    child: Column(
                      crossAxisAlignment: CrossAxisAlignment.stretch,
                      children: [
                        Align(
                          alignment: Alignment.centerLeft,
                          child: BackButton(
                            onPressed: _back,
                            color: _busy ? theme.disabledColor : null,
                          ),
                        ),
                        SizedBox(
                          height: 2,
                          child: _busy
                              ? const LinearProgressIndicator(minHeight: 2)
                              : null,
                        ),
                        Expanded(
                          child: SingleChildScrollView(
                            key: PageStorageKey(_mode),
                            padding: EdgeInsets.fromLTRB(
                              gutter,
                              wide ? 28 : 12,
                              gutter,
                              28,
                            ),
                            child: Center(
                              child: ConstrainedBox(
                                constraints: const BoxConstraints(
                                  maxWidth: 440,
                                ),
                                child: _buildForm(theme),
                              ),
                            ),
                          ),
                        ),
                      ],
                    ),
                  ),
                ),
              ),
            ),
          ],
        ),
      ),
    );
  }
}
