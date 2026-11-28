import 'dart:io';
import 'dart:ui' as ui;

import 'package:flutter/foundation.dart';
import 'package:flutter/rendering.dart' show RenderRepaintBoundary;
import 'package:flutter_test/flutter_test.dart';
import 'package:material_ui/material_ui.dart';
import 'package:touhouonline/launcher_prism.dart';

import '../tools/capture_native_baseline_test.dart' show loadFonts, save;

Future<List<int>> _pixels(WidgetTester tester, GlobalKey key) async =>
    (await tester.runAsync(() async {
      final render =
          key.currentContext!.findRenderObject()! as RenderRepaintBoundary;
      final image = await render.toImage(pixelRatio: 1);
      try {
        return (await image.toByteData(format: ui.ImageByteFormat.rawRgba))!
            .buffer
            .asUint8List()
            .toList();
      } finally {
        image.dispose();
      }
    }))!;

void main() {
  setUpAll(() async {
    if (Platform.isWindows) await loadFonts();
  });
  for (final locale in [
    const Locale('ja'),
    const Locale('en'),
    const Locale('zh', 'CN'),
  ]) {
    for (final weight in [FontWeight.w400, FontWeight.w700]) {
      testWidgets(
        '${locale.languageCode} renders simplified names with real system glyphs | ${weight.value}',
        (tester) async {
          tester.view.devicePixelRatio = 1;
          tester.view.physicalSize = const Size(128, 40);
          addTearDown(tester.view.resetDevicePixelRatio);
          addTearDown(tester.view.resetPhysicalSize);
          final fonts = launcherFonts(TargetPlatform.windows, [locale]);
          final target = GlobalKey(),
              reference = GlobalKey(),
              boundary = GlobalKey();
          Widget text(GlobalKey key, String family, List<String>? fallback) =>
              RepaintBoundary(
                key: key,
                child: ColoredBox(
                  color: const Color(0xFF000000),
                  child: SizedBox(
                    width: 64,
                    height: 40,
                    child: Text(
                      '测试',
                      strutStyle: const StrutStyle(
                        fontFamily: 'Microsoft YaHei UI',
                        fontSize: 24,
                        height: 1,
                        forceStrutHeight: true,
                      ),
                      style: TextStyle(
                        fontFamily: family,
                        fontFamilyFallback: fallback,
                        fontSize: 24,
                        height: 1,
                        fontWeight: weight,
                        color: const Color(0xFFFFFFFF),
                      ),
                    ),
                  ),
                ),
              );
          await tester.pumpWidget(
            Directionality(
              textDirection: TextDirection.ltr,
              child: RepaintBoundary(
                key: boundary,
                child: Row(
                  children: [
                    text(target, fonts.family, fonts.fallback),
                    text(reference, 'Microsoft YaHei UI', null),
                  ],
                ),
              ),
            ),
          );
          await tester.pumpAndSettle();
          final capture = Platform.environment['TOUHOU_FONT_CAPTURE'];
          if (capture != null) {
            await save(
              tester,
              boundary,
              Directory(capture)..createSync(recursive: true),
              'glyphs-${locale.languageCode}-${weight.value}',
            );
          }
          expect(
            listEquals(
              await _pixels(tester, target),
              await _pixels(tester, reference),
            ),
            isTrue,
            reason: 'Simplified characters missing from Japanese fonts must use the system Chinese fallback',
          );
          expect(tester.takeException(), isNull);
        },
        skip: !Platform.isWindows,
      );
    }
  }
}
