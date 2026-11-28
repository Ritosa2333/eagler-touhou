import 'package:flutter/widgets.dart';

import 'generated/launcher_strings.dart';

export 'generated/launcher_strings.dart';

String bundledLanguage(Locale locale) => switch (locale.languageCode) {
  'en' => 'en',
  'ja' => 'ja',
  _ => 'zh-CN',
};

extension LauncherPlaytime on LauncherStrings {
  String formatPlaytime(num seconds) {
    final minutes = seconds ~/ 60;
    if (minutes == 0) {
      return seconds > 0 && seconds < 1
          ? playtimeUnderSecond
          : playtimeSeconds(seconds.floor());
    }
    final hours = minutes ~/ 60;
    if (hours == 0) return playtimeMinutes(minutes);
    return minutes % 60 == 0
        ? playtimeHours(hours)
        : playtimeHoursMinutes(hours, minutes % 60);
  }
}

extension LauncherBuildStrings on BuildContext {
  LauncherStrings get strings => LauncherStrings.of(this);
}
