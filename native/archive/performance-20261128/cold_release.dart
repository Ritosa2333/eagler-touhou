import 'dart:convert';
import 'dart:developer' as developer;
import 'dart:io';
import 'dart:ui' as ui;
import 'package:flutter/gestures.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:integration_test/integration_test.dart';
import 'package:material_ui/material_ui.dart';
import 'package:window_manager/window_manager.dart' as windows;
import 'package:touhouonline/main.dart' as app;
import 'package:touhouonline/library.dart';
import 'package:touhouonline/game_detail.dart';

void main() {
  final binding=IntegrationTestWidgetsFlutterBinding.ensureInitialized()
    ..framePolicy=LiveTestWidgetsFlutterBindingFramePolicy.fullyLive;
  testWidgets('Release first visible cover click without scroll preparation', (tester) async {
    final frames=<ui.FrameTiming>[];
    void record(List<ui.FrameTiming> values)=>frames.addAll(values);
    binding.addTimingsCallback(record);
    await app.main([]);await tester.pumpAndSettle();
    final window=windows.WindowManager.instance.getCurrent()!;
    window.unmaximize();window.setSize(const Size(1280,720).toNative(),false);
    await tester.pumpAndSettle();
    await tester.runAsync(()=>Future<void>.delayed(const Duration(milliseconds:1600)));
    final cover=find.byWidgetPredicate((w)=>w is GameArtwork&&w.game.id=='th06'&&w.size==null).first;
    expect(cover.hitTestable(),findsOneWidget);
    final mouse=await tester.createGesture(kind:PointerDeviceKind.mouse);
    await mouse.addPointer(location:const Offset(310,50));
    frames.clear();
    final start=developer.Timeline.now;
    final at=tester.getCenter(cover);
    await mouse.moveTo(at);await mouse.down(at);await mouse.up();await tester.pump();
    await tester.runAsync(()=>Future<void>.delayed(const Duration(milliseconds:500)));
    final end=developer.Timeline.now;
    await tester.runAsync(()=>Future<void>.delayed(const Duration(milliseconds:1300)));
    final output={
      'mode':'release','input':'First direct click on already-visible th06 cover | no ensureVisible or preparation scroll | foreground native window | same production bootstrap and rendering settings',
      'startUs':start,'endUs':end,'refreshRate':tester.view.display.refreshRate,
      'frames':[for(final f in frames) if(f.timestampInMicroseconds(ui.FramePhase.buildStart)>=start&&f.timestampInMicroseconds(ui.FramePhase.buildStart)<=end)
        {'buildUs':f.buildDuration.inMicroseconds,'rasterUs':f.rasterDuration.inMicroseconds,'totalUs':f.totalSpan.inMicroseconds,'vsyncUs':f.timestampInMicroseconds(ui.FramePhase.vsyncStart)}],
      'status':find.byType(GameDetail).evaluate().length==1?'passed':'failed',
    };
    File('D:/workspace/eagler/artifacts/native-launcher-85941f0d56/hover-detail-perf/cold-release.json').writeAsStringSync(const JsonEncoder.withIndent('  ').convert(output));
    expect(find.byType(GameDetail),findsOneWidget);
    binding.removeTimingsCallback(record);await mouse.removePointer();
  });
}
