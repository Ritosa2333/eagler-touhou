import 'package:flutter_test/flutter_test.dart';
import 'package:material_ui/material_ui.dart';
import 'package:shared_preferences_platform_interface/in_memory_shared_preferences_async.dart';
import 'package:shared_preferences_platform_interface/shared_preferences_async_platform_interface.dart';
import 'package:touhouonline/catalog.dart';
import 'package:touhouonline/launcher_language.dart';
import 'package:touhouonline/library.dart';

import 'widget_test.dart' as fixtures;

void main() {
  late List<Game> games;
  setUpAll(() async => games = await Game.load());
  setUp(
    () => SharedPreferencesAsyncPlatform.instance =
        InMemorySharedPreferencesAsync.empty(),
  );

  for (final mode in ['normal', 'app-reduced', 'system-reduced']) {
    testWidgets(
      'Language dialog enters and exits with motion preference | $mode',
      (tester) async {
        tester.view.devicePixelRatio = 1;
        tester.view.physicalSize = const Size(1440, 734);
        addTearDown(tester.view.resetPhysicalSize);
        addTearDown(tester.view.resetDevicePixelRatio);
        if (mode == 'system-reduced') {
          tester.platformDispatcher.accessibilityFeaturesTestValue =
              const FakeAccessibilityFeatures(disableAnimations: true);
          addTearDown(
            tester.platformDispatcher.clearAccessibilityFeaturesTestValue,
          );
        }
        final app = fixtures.fixture(games: games);
        await app.preferences.setLessMotion(mode == 'app-reduced');
        await tester.pumpWidget(app);
        await tester.pumpAndSettle();
        final library = find.byType(LibraryPage);
        final viewport = tester.getRect(library);
        final opening = showLauncherLanguage(
          tester.element(library),
          app.preferences,
        );
        await tester.pump();
        await tester.pump(const Duration(milliseconds: 16));
        final dialog = find.byType(AlertDialog);
        expect(dialog, findsOneWidget);
        final route = ModalRoute.of(tester.element(dialog))!;
        if (mode == 'normal') {
          expect(route.animation!.value, inExclusiveRange(0.0, 1.0));
        } else {
          expect(route.animation!.value, 1.0);
        }
        expect(tester.getRect(library), viewport);
        await tester.pumpAndSettle();
        expect(find.text('简体中文').hitTestable(), findsOneWidget);
        Navigator.of(tester.element(dialog)).pop();
        await tester.pump();
        await tester.pump(const Duration(milliseconds: 16));
        expect(dialog, mode == 'normal' ? findsOneWidget : findsNothing);
        await tester.pumpAndSettle();
        await opening;
        expect(tester.getRect(library), viewport);
        expect(tester.takeException(), isNull);
      },
    );
  }
}
