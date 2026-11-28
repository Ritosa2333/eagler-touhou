import 'package:go_router/go_router.dart';
import 'package:material_ui/material_ui.dart';
import 'package:url_launcher/url_launcher.dart';

import 'l10n/strings.dart';

const accountWebPath = '/web-account';

bool isProductionSiteUrl(Uri url) =>
    url.scheme == 'https' &&
    url.host == 'touhouonline.com' &&
    url.port == 443 &&
    url.userInfo.isEmpty;

bool isAccountSiteUrl(Uri url) =>
    isProductionSiteUrl(url) &&
    (url.path == '/login.html' ||
        url.path == '/profile.html' ||
        url.path.startsWith('/u/'));

/// Core links enter the existing native router; other website sections stay web.
String? nativeSiteLocation(Uri url) {
  if (!isProductionSiteUrl(url)) return null;
  final home = [
    '',
    '/',
    '/index.html',
    '/en.html',
    '/ja.html',
  ].contains(url.path);
  final lobby = ['/lobby', '/lobby.html'].contains(url.path);
  if (!home && !lobby) return null;
  try {
    final query = url.queryParametersAll;
    if (home && ['game', 'j', 'mpRoom'].any(query.containsKey)) {
      return url.replace(path: '/').toString();
    }
    if (url.fragment.isEmpty && query.keys.every((key) => key == 'lang')) {
      return lobby ? '/lobby' : '/';
    }
  } on FormatException {
    // A malformed query is handled by the website, not a partial native route.
  }
  return null;
}

/// Translate persisted native account addresses to the maintained website.
String? accountWebLocation(Uri address, String language) {
  if (address.hasAuthority || address.hasScheme) {
    if (!isAccountSiteUrl(address)) return null;
  }
  var target = Uri.https('touhouonline.com').resolveUri(address);
  final path = target.path;
  if (path == '/login') {
    target = target.replace(path: '/login.html');
  } else if (path == '/account/security') {
    target = target.replace(path: '/profile.html', fragment: 'security');
  } else if (RegExp(r'^/u/[^/]+/edit$').hasMatch(path)) {
    target = target.replace(
      path: path.substring(0, path.length - 5),
      fragment: 'edit',
    );
  } else if (RegExp(r'^/u/[^/]+/social$').hasMatch(path)) {
    final tab = target.queryParameters['tab'];
    final social = ['following', 'followers', 'friends'].contains(tab)
        ? tab!
        : 'friends';
    final query = Map<String, String>.of(target.queryParameters)..remove('tab');
    target = target.replace(
      path: path.substring(0, path.length - 7),
      queryParameters: {...query, 'social': social},
    );
  }
  if (!isAccountSiteUrl(target)) return null;
  if (!target.queryParameters.containsKey('lang')) {
    target = target.replace(
      queryParameters: {...target.queryParametersAll, 'lang': language},
    );
  }
  return Uri(
    path: accountWebPath,
    queryParameters: {'url': target.toString()},
  ).toString();
}

const signInPath = '/login.html';
const registerPath = '/login.html?mode=register';
const resetPasswordPath = '/login.html?mode=reset';

String signInFor(String next) =>
    Uri(path: signInPath, queryParameters: {'next': next}).toString();

const faqPath = '/faq.html';
const firstUsePath = '/guide.html';
const multiplayerGuidePath = '/docs/multiplayer';
const aboutPath = '/about.html';
const termsPath = '/terms.html';
const privacyPath = '/privacy.html';
const uptimePath = '/uptime';

Future<void> openSiteLink(
  BuildContext context,
  String? href, {
  String from = '/',
}) async {
  if (href == null || href.trim().isEmpty) return;
  final link = Uri.tryParse(href);
  if (link == null) return;
  var url = Uri.https('touhouonline.com').resolve(from).resolveUri(link);
  if (url.userInfo.isNotEmpty ||
      !['http', 'https', 'mailto'].contains(url.scheme)) {
    return;
  }
  final native = nativeSiteLocation(url);
  if (native != null) {
    context.go(native);
    return;
  }
  if (url.host == 'touhouonline.com') {
    if (!url.queryParameters.containsKey('lang')) {
      url = url.replace(
        queryParameters: {
          ...url.queryParametersAll,
          'lang': bundledLanguage(Localizations.localeOf(context)),
        },
      );
    }
  }
  if (isAccountSiteUrl(url)) {
    await context.push(
      accountWebLocation(
        url,
        bundledLanguage(Localizations.localeOf(context)),
      )!,
    );
    return;
  }
  try {
    if (await launchUrl(url, mode: LaunchMode.externalApplication)) return;
  } on Exception {
    // The existing page stays open if the system cannot handle this link.
  }
  if (context.mounted) {
    ScaffoldMessenger.of(context)
        .showSnackBar(SnackBar(content: Text(context.strings.cannotOpenLink)));
  }
}
