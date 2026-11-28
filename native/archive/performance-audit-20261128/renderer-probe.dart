import 'dart:convert';
import 'dart:developer' as developer;
import 'dart:io';
import 'dart:ui' as ui;
import 'dart:async';
import 'package:flutter/rendering.dart';
import 'package:shared_preferences/shared_preferences.dart';
import 'package:shared_preferences_platform_interface/in_memory_shared_preferences_async.dart';
import 'package:shared_preferences_platform_interface/shared_preferences_async_platform_interface.dart';
import 'package:path_provider_platform_interface/path_provider_platform_interface.dart';
import 'package:touhouonline/account_api.dart';
import 'package:touhouonline/desktop_window.dart';
import 'package:touhouonline/game_launcher.dart';
import 'package:touhouonline/game_preferences.dart';
import 'package:touhouonline/launcher_rendering.dart';
import 'package:flutter/foundation.dart';
import 'package:flutter/gestures.dart';
import 'package:flutter/painting.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:integration_test/integration_test.dart';
import 'package:material_ui/material_ui.dart';
import 'package:prism_ui/prism_ui.dart' as prism;
import 'package:window_manager/window_manager.dart' as windows;
import 'package:touhouonline/main.dart' as app;
import 'package:touhouonline/catalog.dart';
import 'package:touhouonline/library.dart';
import 'package:touhouonline/game_detail.dart';
import 'package:touhouonline/game_settings.dart';
import 'package:touhouonline/launcher_select.dart';
import 'package:touhouonline/l10n/strings.dart';
import 'package:touhouonline/site_navigation.dart';

class _Paths extends PathProviderPlatform {
  @override
  Future<String?> getApplicationSupportPath() async => '${Platform.environment['TOUHOU_RENDERER_FOLDER']}/profile';
  @override
  Future<String?> getTemporaryPath() async => '${Platform.environment['TOUHOU_RENDERER_FOLDER']}/cache';
}

void main() {
  final binding = IntegrationTestWidgetsFlutterBinding.ensureInitialized()
    ..framePolicy = LiveTestWidgetsFlutterBindingFramePolicy.fullyLive;
  final file = File(Platform.environment['TOUHOU_RENDERER_RESULT']!);
  final report = <String, dynamic>{'mode':'release', 'pid':pid, 'renderer':Platform.environment['TOUHOU_RENDERER'], 'cases':<String,dynamic>{}, 'focusSamples':<Object?>[]};
  void save() => file.writeAsStringSync(const JsonEncoder.withIndent('  ').convert(report));
  Future<void> capture(WidgetTester tester, String name) async {
    final boundaries=tester.renderObjectList<RenderRepaintBoundary>(find.byType(RepaintBoundary)).where((b)=>b.attached && b.hasSize);
    final boundary=boundaries.reduce((a,b)=>a.size.width*a.size.height >= b.size.width*b.size.height ? a:b);
    final image=await boundary.toImage(pixelRatio:1);
    final bytes=await image.toByteData(format:ui.ImageByteFormat.png);
    await File('${Platform.environment['TOUHOU_RENDERER_FOLDER']}/$name.png').writeAsBytes(bytes!.buffer.asUint8List());
    image.dispose();
  }
  testWidgets('native release runtime audit', (tester) async {
    final received = <ui.FrameTiming>[];
    void timings(List<ui.FrameTiming> frames) => received.addAll(frames);
    binding.addTimingsCallback(timings);
    Future<void> wait(int ms) => tester.runAsync(() => Future<void>.delayed(Duration(milliseconds:ms)));
    expect(kReleaseMode, isTrue);
    final startup = Stopwatch()..start();
    PathProviderPlatform.instance = _Paths();
    SharedPreferencesAsyncPlatform.instance = InMemorySharedPreferencesAsync.empty();
    await const LauncherShaderWarmUp().execute();
    initializeDesktopWindow();
    final storage=SharedPreferencesAsync();
    final games=await Game.load();
    final favorites=GameFavorites(storage,{});
    await tester.pumpWidget(app.LauncherApp(
      book:BookState(games,favorites), lobby:BookState(games,favorites),
      preferences:LauncherPreferences(storage,ThemeMode.dark,false),
      launcher:const NativeGamePlaceholder(),
      gamePreferences:GamePreferenceStore(storage),accountApi:AccountApi()));
    report['bootstrapMs'] = startup.elapsedMicroseconds/1000;
    await tester.pumpAndSettle();
    await wait(1300);
    report['initialFrames'] = [for(final f in received.take(5)) {'buildUs':f.buildDuration.inMicroseconds,'rasterUs':f.rasterDuration.inMicroseconds,'totalUs':f.totalSpan.inMicroseconds}];
    final window = windows.WindowManager.instance.getCurrent()!;
    window.unmaximize();
    window.setSize(const Size(1280,720).toNative(), false);
    window.show();
    window.focus();
    await tester.pumpAndSettle();
    report['environment'] = {'refreshRate':tester.view.display.refreshRate,'dpr':tester.view.devicePixelRatio,'physicalSize':[tester.view.physicalSize.width,tester.view.physicalSize.height]};
    final focusTimer=Timer.periodic(const Duration(milliseconds:100), (_) { (report['focusSamples'] as List).add({'at':DateTime.now().toUtc().toIso8601String(),'focused':window.isFocused}); });
    final launcher = tester.widget<app.LauncherApp>(find.byType(app.LauncherApp));
    final initialReduced = launcher.preferences.lessMotion;
    final strings = tester.element(find.byType(LibraryPage)).strings;
    final mouse = await tester.createGesture(kind: PointerDeviceKind.mouse);
    await mouse.addPointer(location: const Offset(310,50));
    Finder cover() => find.byWidgetPredicate((w)=>w is GameArtwork && w.game.id=='th06' && w.size==null).first;
    Future<void> click(Finder target) async {
      await tester.ensureVisible(target);
      await tester.pump();
      final at = tester.getCenter(target);
      await mouse.moveTo(at);
      await mouse.down(at);
      await mouse.up();
      await tester.pump();
    }
    Future<void> tapAndWait(Finder target,[int delay=450]) async {await click(target);await wait(delay);}
    Future<void> measure(String name, Future<void> Function() action,{bool allowIdle=false}) async {
      final focusedBefore=window.isFocused;
      received.clear();
      final start = developer.Timeline.now;
      final cpuStart = ProcessInfo.currentRss;
      await action();
      final end = developer.Timeline.now;
      await wait(1300);
      final frames = received.where((f){final t=f.timestampInMicroseconds(ui.FramePhase.buildStart);return t>=start&&t<=end;}).toList();
      report['cases'][name] = {
        'focusedAtStartAndEnd':[focusedBefore,window.isFocused],
        'durationUs':end-start,'rssStart':cpuStart,'rssEnd':ProcessInfo.currentRss,
        'imageCacheBytes':PaintingBinding.instance.imageCache.currentSizeBytes,
        'imageCacheCount':PaintingBinding.instance.imageCache.currentSize,
        'physicalSize':[tester.view.physicalSize.width,tester.view.physicalSize.height],
        'frames':[for(final f in frames){'buildUs':f.buildDuration.inMicroseconds,'rasterUs':f.rasterDuration.inMicroseconds,'totalUs':f.totalSpan.inMicroseconds,'vsyncUs':f.timestampInMicroseconds(ui.FramePhase.vsyncStart)}],
      };
      save();
      if(!allowIdle) expect(frames,isNotEmpty,reason:name);
      expect(tester.takeException(),isNull,reason:name);
    }
    Future<void> open() async {await tapAndWait(cover());expect(find.byType(GameDetail),findsOneWidget);}
    Future<void> back() async {await tapAndWait(find.descendant(of:find.byType(GameDetail),matching:find.byType(BackButton)));expect(find.byType(LibraryPage).hitTestable(),findsOneWidget);}
    try {
      await launcher.preferences.setLessMotion(false);
      await measure('idle_shelf',()=>wait(2500),allowIdle:true);
      await measure('detail_first_open',open);
      await back();
      await measure('sidebar_hover',() async {
        final row=find.descendant(of:find.byKey(const PageStorageKey(('game sidebar',false))),matching:find.byType(GameRow)).first;
        for(var i=0;i<12;i++){await mouse.moveTo(tester.getCenter(row));await wait(250);await mouse.moveTo(const Offset(310,50));await wait(250);}
      });
      await measure('cover_hover',() async {
        await tester.ensureVisible(cover());await tester.pump();
        for(var i=0;i<12;i++){await mouse.moveTo(tester.getCenter(cover()));await wait(250);await mouse.moveTo(const Offset(310,50));await wait(250);}
      });
      for(var i=0;i<6;i++){await measure('detail_warm_$i',open);await measure('detail_back_$i',back);}
      final scroll=find.descendant(of:find.byType(LibraryPage),matching:find.byType(Scrollable)).first;
      await measure('shelf_scroll',() async {
        await tester.timedDrag(scroll,const Offset(0,-500),const Duration(milliseconds:700));await wait(500);
        await tester.timedDrag(scroll,const Offset(0,500),const Duration(milliseconds:700));await wait(500);
      });
      await tester.ensureVisible(find.byType(LauncherSelect<GameOrder>));await tester.pumpAndSettle();
      await measure('sort_reverse',() async {await tapAndWait(find.byType(LauncherSelect<GameOrder>),250);await tapAndWait(find.text(GameOrder.reverse.label(strings)).last);});
      await measure('sort_number',() async {await tapAndWait(find.byType(LauncherSelect<GameOrder>),250);await tapAndWait(find.text(GameOrder.number.label(strings)).last);});
      final tabs=find.descendant(of:find.byKey(const ValueKey('masthead navigation')),matching:find.byType(prism.PressSurface));
      await measure('workspace_tabs',() async {for(var i=0;i<3;i++){await tapAndWait(tabs.at(1),350);await tapAndWait(tabs.at(0),350);}});
      await measure('workspace_interrupt',() async {for(final i in [1,0,1,0]){await tapAndWait(tabs.at(i),60);}await wait(400);});
      await measure('theme_overlay',() async {await tapAndWait(find.byTooltip(strings.themeSettings));await tapAndWait(find.byTooltip(strings.close).last);});
      await open();
      Finder settings() {final text=find.text(strings.gameSettings).hitTestable();return text.evaluate().isNotEmpty?text:find.byTooltip(strings.gameSettings);}
      await measure('settings_open',()=>tapAndWait(settings()));
      await measure('settings_fold',() async {final tile=find.byType(ExpansionTile).first;await tapAndWait(find.descendant(of:tile,matching:find.byType(ListTile)).first);await tapAndWait(find.descendant(of:tile,matching:find.byType(ListTile)).first);});
      await measure('settings_back',()=>tapAndWait(find.descendant(of:find.byType(GameSettingsPage),matching:find.byType(BackButton))));
      await back();
      await launcher.preferences.setLessMotion(true);
      await tester.pumpAndSettle();
      await measure('reduced_detail_open',open);
      await measure('reduced_detail_back',back);
      window.setSize(const Size(390,844).toNative(),false);await tester.pumpAndSettle();
      await launcher.preferences.setLessMotion(false);await tester.pumpAndSettle();
      await measure('portrait_detail_open',open);
      await measure('portrait_detail_back',back);
      await measure('portrait_drawer',() async {await tapAndWait(find.byType(DrawerButton));expect(find.byType(SiteDrawer).hitTestable(),findsOneWidget);await tapAndWait(find.byTooltip(strings.closeMenu));});
      await measure('portrait_tabs',() async {for(final i in [1,0,2,0]){await tapAndWait(find.descendant(of:find.byType(NavigationBar),matching:find.byType(NavigationDestination)).at(i),350);}});
      await wait(1000);
      await tester.pumpAndSettle();
      await measure('idle_after_roundtrip',()=>wait(2500),allowIdle:true);
      report['status']='passed';
    } catch(error,stack) {
      report['status']='failed';report['error']='$error';report['stack']='$stack';rethrow;
    } finally {
      await launcher.preferences.setLessMotion(initialReduced);
      await mouse.removePointer();binding.removeTimingsCallback(timings);focusTimer.cancel();save();
      Timer(const Duration(seconds: 2), () => window.close());
    }
  });
}
