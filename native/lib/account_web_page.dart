import 'dart:async';
import 'dart:io' as io;

import 'package:flutter/foundation.dart'
    show defaultTargetPlatform, mapEquals, TargetPlatform;
import 'package:flutter_inappwebview/flutter_inappwebview.dart' as web;
import 'package:go_router/go_router.dart';
import 'package:material_ui/material_ui.dart';
import 'package:path_provider/path_provider.dart';

import 'account.dart';
import 'l10n/strings.dart';
import 'launcher_navigation.dart';
import 'site_documents.dart';

/// The website owns all account content. This page owns only the native container.
class AccountWebPage extends StatefulWidget {
  const AccountWebPage({
    super.key,
    required this.session,
    required this.address,
  });
  final AccountSession session;
  final Uri address;

  @override
  State<AccountWebPage> createState() => _AccountWebPageState();
}

class _AccountWebPageState extends State<AccountWebPage> {
  static final _origin = Uri.https('touhouonline.com');
  static web.WebViewEnvironment? _windowsEnvironment;
  static Future<web.WebViewEnvironment>? _windowsCreating;
  web.WebViewEnvironment? _environment;
  web.CookieManager? _cookies;
  web.InAppWebViewController? _controller;
  Future<void>? _syncing;
  String? _lastCookie;
  bool _ready = false;
  bool _failed = false;
  bool _allowPop = false;
  bool _closing = false;
  bool _hasWebHistory = false;
  int _historyRevision = 0;
  int _visitRevision = 0;
  Timer? _visitTimer;
  int _progress = 0;

  @override
  void initState() {
    super.initState();
    unawaited(_prepare());
  }

  @override
  void dispose() {
    _visitTimer?.cancel();
    super.dispose();
  }

  Future<web.WebViewEnvironment> _createWindowsEnvironment() async {
    if (await web.WebViewEnvironment.getAvailableVersion() == null) {
      throw const io.FileSystemException('WebView2 Runtime is unavailable');
    }
    final support = await getApplicationSupportDirectory();
    return web.WebViewEnvironment.create(
      settings: web.WebViewEnvironmentSettings(
        userDataFolder: '${support.path}/account-web',
      ),
    );
  }

  Future<void> _prepare() async {
    setState(() {
      _failed = false;
      _ready = false;
      _hasWebHistory = false;
      _historyRevision++;
      _visitRevision++;
      _visitTimer?.cancel();
    });
    try {
      if (!isProductionSiteUrl(widget.address)) {
        throw const FormatException(
          'Account page must use the production origin',
        );
      }
      if (defaultTargetPlatform == TargetPlatform.windows) {
        _environment = _windowsEnvironment;
        if (_environment == null) {
          try {
            _environment = _windowsEnvironment = await (_windowsCreating ??=
                _createWindowsEnvironment());
          } finally {
            _windowsCreating = null;
          }
        }
      }
      _cookies = web.CookieManager.instance(webViewEnvironment: _environment);
      final existing = await _cookies!.getCookie(
        url: web.WebUri.uri(_origin),
        name: widget.session.api.sessionCookieName,
      );
      final native = widget.session.api.webSessionCookie;
      if (existing == null && native != null) {
        await _storeCookie(native);
      }
      if (!mounted) return;
      setState(() => _ready = true);
    } catch (_) {
      if (mounted) setState(() => _failed = true);
    }
  }

  Future<void> _storeCookie(io.Cookie cookie) async {
    final stored = await _cookies!.setCookie(
      url: web.WebUri.uri(_origin),
      name: cookie.name,
      value: cookie.value,
      path: cookie.path ?? '/',
      expiresDate: cookie.expires?.millisecondsSinceEpoch,
      isSecure: true,
      isHttpOnly: true,
      sameSite: web.HTTPCookieSameSitePolicy.LAX,
    );
    if (!stored) throw StateError('Account cookie store is unavailable');
  }

  Future<void> _sync({bool force = false}) {
    if (_syncing case final Future<void> pending) {
      return force
          ? pending
                .catchError((Object error) {})
                .then((_) => _sync(force: true))
          : pending;
    }
    return _syncing = _readSession(force).whenComplete(() => _syncing = null);
  }

  Future<void> _readSession(bool force) async {
    final manager = _cookies;
    if (manager == null) return;
    final value = await manager.getCookie(
      url: web.WebUri.uri(_origin),
      name: widget.session.api.sessionCookieName,
    );
    io.Cookie? cookie;
    if (value != null) {
      cookie = io.Cookie(value.name, value.value)
        ..path = value.path ?? '/'
        ..domain = value.domain
        ..secure = value.isSecure ?? true
        ..httpOnly = value.isHttpOnly ?? true;
      if (value.expiresDate case final int expiry) {
        cookie.expires = DateTime.fromMillisecondsSinceEpoch(
          expiry,
          isUtc: true,
        );
      }
    }
    var signature = cookie == null ? '' : '${cookie.name}=${cookie.value}';
    if (!force && _lastCookie == signature) return;
    await widget.session.syncWebSession(_origin, cookie);
    final refreshed = widget.session.api.webSessionCookie;
    if (refreshed != null && refreshed.value != cookie?.value) {
      final current = await manager.getCookie(
        url: web.WebUri.uri(_origin),
        name: widget.session.api.sessionCookieName,
      );
      // Do not overwrite a logout or another login that occurred during the
      // service request. A queued load/close will read the newer browser state.
      if (current?.name == cookie?.name && current?.value == cookie?.value) {
        await _storeCookie(refreshed);
        signature = '${refreshed.name}=${refreshed.value}';
      }
    }
    _lastCookie = signature;
  }

  void _syncAfterLoad({bool force = false}) {
    unawaited(
      _sync(force: force)
          .then((_) {
            if (mounted &&
                !_closing &&
                widget.address.path == '/login.html' &&
                widget.session.identity != null) {
              unawaited(_close(refreshSession: false));
            }
          })
          .catchError((Object error) {
            // The session model exposes retry; the website stays usable offline.
          }),
    );
  }

  Future<void> _close({
    bool refreshSession = true,
    String? nativeLocation,
  }) async {
    if (!mounted || _closing || ModalRoute.of(context)?.isCurrent == false) {
      return;
    }
    _closing = true;
    _visitTimer?.cancel();
    _visitRevision++;
    if (refreshSession && _ready) _syncAfterLoad(force: true);
    setState(() => _allowPop = true);
    WidgetsBinding.instance.addPostFrameCallback((_) {
      if (!mounted) return;
      if (ModalRoute.of(context)?.isCurrent == false) {
        setState(() {
          _closing = false;
          _allowPop = false;
        });
        return;
      }
      if (nativeLocation != null) {
        final router = GoRouter.of(context);
        // Dismiss only the account overlay before the native caller evaluates
        // its own exit guard. A rejected destination leaves that caller usable.
        if (router.canPop()) closeLauncherPage(context, '/');
        router.go(nativeLocation);
      } else {
        closeLauncherPage(context, '/');
      }
    });
  }

  Future<void> _back() async {
    try {
      if (await _controller?.canGoBack() == true) {
        await _controller!.goBack();
        return;
      }
    } catch (_) {
      // A crashed WebView must not trap the native back button.
    }
    await _close();
  }

  Future<void> _updateHistory() async {
    if (defaultTargetPlatform != TargetPlatform.iOS) return;
    final revision = ++_historyRevision;
    var available = false;
    try {
      available = await _controller?.canGoBack() == true;
    } catch (_) {
      // A failed engine still permits leaving the native account container.
    }
    if (mounted &&
        revision == _historyRevision &&
        available != _hasWebHistory) {
      setState(() => _hasWebHistory = available);
    }
  }

  bool _returnsToNative(Uri address) {
    const roots = {
      '/',
      '/index.html',
      '/en.html',
      '/ja.html',
      '/lobby',
      '/lobby.html',
    };
    if (widget.address.path != '/login.html' ||
        !isProductionSiteUrl(address) ||
        !roots.contains(address.path)) {
      return false;
    }
    final next = _origin.resolve(widget.address.queryParameters['next'] ?? '/');
    if (!isProductionSiteUrl(next)) return false;
    String canonical(String path) => switch (path) {
      '/index.html' || '/en.html' || '/ja.html' => '/',
      '/lobby.html' => '/lobby',
      _ => path,
    };
    final actual = Map<String, String>.of(address.queryParameters)
      ..remove('lang');
    final expected = Map<String, String>.of(next.queryParameters)
      ..remove('lang');
    return canonical(address.path) == canonical(next.path) &&
        mapEquals(actual, expected);
  }

  bool _handleNativeAddress(Uri address) {
    if (_returnsToNative(address)) {
      unawaited(_close());
      return true;
    }
    final native = nativeSiteLocation(address);
    if (native != null) {
      unawaited(_close(nativeLocation: native));
      return true;
    }
    return false;
  }

  void _visited(web.WebUri? url) {
    final revision = ++_visitRevision;
    _visitTimer?.cancel();
    if (url != null) {
      final address = url.uriValue;
      if (_returnsToNative(address) || nativeSiteLocation(address) != null) {
        // Website initialization and responsive updates can briefly replace
        // history with root. Follow a settled address, not each notification.
        _visitTimer = Timer(const Duration(milliseconds: 100), () {
          _visitTimer = null;
          unawaited(_visitNativeAddress(address, revision));
        });
      }
    }
    unawaited(_updateHistory());
    _syncAfterLoad();
  }

  Future<void> _visitNativeAddress(Uri address, int revision) async {
    try {
      // History notifications can report a transient URL during page startup.
      // Confirm the live document and discard reads overtaken by newer events.
      final current = await _controller?.getUrl();
      if (mounted &&
          !_closing &&
          revision == _visitRevision &&
          current?.uriValue == address) {
        _handleNativeAddress(address);
      }
    } catch (_) {
      // An unavailable engine leaves the explicit native back/close usable.
    }
  }

  Future<web.NavigationActionPolicy> _navigate(
    web.NavigationAction action,
  ) async {
    final address = action.request.url?.uriValue;
    if (address == null) return web.NavigationActionPolicy.CANCEL;
    if (action.isForMainFrame != true) return web.NavigationActionPolicy.ALLOW;
    if (_handleNativeAddress(address)) return web.NavigationActionPolicy.CANCEL;
    if (isProductionSiteUrl(address)) return web.NavigationActionPolicy.ALLOW;
    if (mounted && ModalRoute.of(context)?.isCurrent != false) {
      await openSiteLink(context, address.toString());
    }
    return web.NavigationActionPolicy.CANCEL;
  }

  @override
  Widget build(BuildContext context) => PopScope(
    canPop:
        _allowPop ||
        (defaultTargetPlatform == TargetPlatform.iOS &&
            (_failed || !_hasWebHistory)),
    onPopInvokedWithResult: (didPop, result) {
      if (didPop) {
        if (!_closing && _ready) {
          _closing = true;
          _syncAfterLoad(force: true);
        }
      } else {
        unawaited(_back());
      }
    },
    child: Scaffold(
      appBar: AppBar(
        toolbarHeight: 48,
        leading: BackButton(onPressed: _back),
        title: Text(context.strings.account),
        actions: [
          IconButton(
            tooltip: MaterialLocalizations.of(context).closeButtonTooltip,
            onPressed: _close,
            icon: const Icon(Icons.close),
          ),
        ],
        bottom: PreferredSize(
          preferredSize: const Size.fromHeight(2),
          child: SizedBox(
            height: 2,
            child: _ready && _progress < 100
                ? LinearProgressIndicator(value: _progress / 100)
                : null,
          ),
        ),
      ),
      body: SafeArea(
        top: false,
        child: _failed
            ? Center(
                child: TextButton.icon(
                  onPressed: _prepare,
                  icon: const Icon(Icons.refresh),
                  label: Text(context.strings.cannotOpenLink),
                ),
              )
            : !_ready
            ? const Center(child: CircularProgressIndicator())
            : web.InAppWebView(
                webViewEnvironment: _environment,
                initialUrlRequest: web.URLRequest(
                  url: web.WebUri.uri(widget.address),
                ),
                initialSettings: web.InAppWebViewSettings(
                  useShouldOverrideUrlLoading: true,
                  incognito: false,
                  sharedCookiesEnabled: true,
                  thirdPartyCookiesEnabled: false,
                  supportMultipleWindows: false,
                  allowsBackForwardNavigationGestures: true,
                ),
                onWebViewCreated: (controller) {
                  _controller = controller;
                  unawaited(_updateHistory());
                },
                onProgressChanged: (_, progress) {
                  if (mounted) setState(() => _progress = progress);
                },
                onLoadStart: (_, _) {
                  _visitTimer?.cancel();
                  _visitRevision++;
                },
                onLoadStop: (_, url) => _visited(url),
                onUpdateVisitedHistory: (_, url, reload) => _visited(url),
                onReceivedError: (_, request, error) {
                  if (request.isForMainFrame == true && mounted) {
                    setState(() => _failed = true);
                  }
                },
                shouldOverrideUrlLoading: (_, action) => _navigate(action),
              ),
      ),
    ),
  );
}
