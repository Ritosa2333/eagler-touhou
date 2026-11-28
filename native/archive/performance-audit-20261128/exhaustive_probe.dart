import 'dart:convert';
import 'dart:developer' as developer;
import 'dart:io';
import 'dart:ui' as ui;
import 'package:flutter_test/flutter_test.dart';
import 'package:integration_test/integration_test.dart';
import 'package:material_ui/material_ui.dart';
import 'package:go_router/go_router.dart';
import 'package:flutter_inappwebview/flutter_inappwebview.dart' as web;
import 'package:window_manager/window_manager.dart' as windows;
import 'package:touhouonline/main.dart' as app;
import 'package:touhouonline/catalog.dart';
import 'package:touhouonline/library.dart';
import 'package:touhouonline/game_detail.dart';
import 'package:touhouonline/account_web_page.dart';
import 'package:touhouonline/site_documents.dart';

void main() {
  final binding=IntegrationTestWidgetsFlutterBinding.ensureInitialized()
    ..framePolicy=LiveTestWidgetsFlutterBindingFramePolicy.fullyLive;
  const tracing=bool.fromEnvironment('PERF_TRACE');
  final report=<String,dynamic>{'mode':const bool.fromEnvironment('dart.vm.product')?'release':'profile','tracing':tracing,'pid':pid,'samples':<Map<String,dynamic>>[]};
  final file=File('D:/workspace/eagler/artifacts/native-launcher-85941f0d56/hover-detail-perf/exhaustive-live.json');
  void save()=>file.writeAsStringSync(const JsonEncoder.withIndent('  ').convert(report));
  testWidgets('Repeated production navigation, sort trace and WebView lifetime', (tester) async {
    final startup=Stopwatch()..start();
    await app.main([]); await tester.pumpAndSettle();
    report['bootstrapMs']=startup.elapsedMicroseconds/1000;
    final window=windows.WindowManager.instance.getCurrent()!;
    window.unmaximize();window.setSize(const Size(1280,720).toNative(),false);window.focus();
    await tester.pumpAndSettle();
    final launcher=tester.widget<app.LauncherApp>(find.byType(app.LauncherApp));
    final router=GoRouter.of(tester.element(find.byType(LibraryPage)));
    Future<void> wait(int ms)=>tester.runAsync(()=>Future<void>.delayed(Duration(milliseconds:ms)));
    final allFrames=<ui.FrameTiming>[];
    void frames(List<ui.FrameTiming> batch)=>allFrames.addAll(batch);
    binding.addTimingsCallback(frames);
    void sample(String phase) {
      final cache=PaintingBinding.instance.imageCache;
      report['phase']=phase;
      (report['samples'] as List).add({
        'phase':phase,'atUs':developer.Timeline.now,'rss':ProcessInfo.currentRss,
        'imageBytes':cache.currentSizeBytes,'imageCount':cache.currentSize,
        'liveImages':cache.liveImageCount,'pendingImages':cache.pendingImageCount,
        'elements':find.byWidgetPredicate((_)=>true,skipOffstage:false).evaluate().length,
        'details':find.byType(GameDetail,skipOffstage:false).evaluate().length,
        'webViews':find.byType(web.InAppWebView,skipOffstage:false).evaluate().length,
        'transientCallbacks':binding.transientCallbackCount,
        'focused':window.isFocused,'minimized':window.isMinimized,
      });save();
    }
    Future<void> settled(String phase) async {await tester.pumpAndSettle();await wait(1200);sample(phase);}
    try {
      await settled('shelf_initial');
      await wait(5000);sample('shelf_idle');
      final ids=launcher.book.games.map((g)=>g.id).toList();
      for(var i=0;i<60;i++) {
        router.push('/game/${ids[i%ids.length]}');await tester.pump();await wait(240);
        expect(find.byType(GameDetail).hitTestable(),findsOneWidget);
        router.pop();await tester.pump();await wait(240);
        if(i%10==9)await settled('roundtrip_${i+1}');
        expect(tester.takeException(),isNull);
      }
      await settled('sort_ready');
      final sortFrames=<ui.FrameTiming>[];
      final sortStart=developer.Timeline.now;
      Future<void> sort() async {
        for(var i=0;i<12;i++) {
          launcher.book.changeOrder(i.isEven?GameOrder.reverse:GameOrder.number);
          await tester.pump();await wait(350);
        }
      }
      if(tracing){await binding.traceAction(sort,reportKey:'sort_timeline');}else{await sort();}
      final sortEnd=developer.Timeline.now;
      await wait(1300);
      sortFrames.addAll(allFrames.where((f){final t=f.timestampInMicroseconds(ui.FramePhase.buildStart);return t>=sortStart&&t<=sortEnd;}));
      report['sortFrames']=[for(final f in sortFrames){'buildUs':f.buildDuration.inMicroseconds,'rasterUs':f.rasterDuration.inMicroseconds,'totalUs':f.totalSpan.inMicroseconds}];
      sample('sort_done');
      for(var i=0;i<3;i++) {
        router.push(accountWebLocation(Uri.https('touhouonline.com','/login.html'), 'zh-CN')!);
        await tester.pump();
        for(var j=0;j<40;j++) {
          await wait(250);await tester.pump();
          final view=find.byType(web.InAppWebView);
          if(view.evaluate().isNotEmpty){sample('web_created_$i');break;}
        }
        await wait(5000);sample('web_open_$i');
        if(find.byType(AccountWebPage).evaluate().isNotEmpty){
          expect(router.canPop(),isTrue);router.pop();
        }else{report['webAutomaticallyReturned_$i']=router.routeInformationProvider.value.uri.toString();}
        await settled('web_closed_$i');
        expect(find.byType(web.InAppWebView,skipOffstage:false),findsNothing);
        expect(tester.takeException(),isNull);
      }
      await wait(10000);sample('shelf_final_idle');
      window.minimize();await wait(5000);sample('minimized');
      window.restore();window.focus();await settled('restored');
      report['status']='passed';
      (binding.reportData??=<String,dynamic>{})['audit'] = report;
    } catch(e,s) {report['status']='failed';report['error']='$e';report['stack']='$s';rethrow;}
    finally {binding.removeTimingsCallback(frames);save();}
  });
}
