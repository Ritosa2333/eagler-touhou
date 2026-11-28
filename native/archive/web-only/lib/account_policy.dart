import 'package:flutter/services.dart' show AssetBundle;
import 'package:flutter/widgets.dart' show Locale;

import 'l10n/strings.dart';

enum AccountDocument {
  terms,
  privacy;

  String title(LauncherStrings strings) => switch (this) {
    terms => strings.terms,
    privacy => strings.privacy,
  };

  Future<String> load(AssetBundle bundle, Locale locale) async {
    final language = bundledLanguage(locale);
    final source = await bundle.loadString(
      'assets/documents/$name.$language.md',
    );
    return source.replaceFirst(RegExp(r'^# [^\r\n]+(?:\r?\n)+'), '');
  }
}
