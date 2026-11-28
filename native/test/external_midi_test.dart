import 'dart:async';
import 'dart:convert';
import 'dart:io';

import 'package:flutter/services.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:material_ui/material_ui.dart';
import 'package:shared_preferences/shared_preferences.dart';
import 'package:shared_preferences_platform_interface/in_memory_shared_preferences_async.dart';
import 'package:shared_preferences_platform_interface/shared_preferences_async_platform_interface.dart';
import 'package:touhouonline/catalog.dart';
import 'package:touhouonline/external_midi.dart';
import 'package:touhouonline/game_launcher.dart';
import 'package:touhouonline/game_preferences.dart';
import 'package:touhouonline/game_resources.dart';
import 'package:touhouonline/game_settings.dart';
import 'package:touhouonline/game_user_files.dart';
import 'package:touhouonline/l10n/strings.dart';

import '../tools/capture_native_baseline_test.dart' show loadFonts, save;
import 'widget_test.dart' as launcher;

Future<void> _tap(WidgetTester tester, Finder target) async {
  await tester.ensureVisible(target);
  await tester.pumpAndSettle();
  await tester.tap(target);
  await tester.pumpAndSettle();
}

void main() {
  TestWidgetsFlutterBinding.ensureInitialized();
  final capture = Platform.environment['TOUHOU_MIDI_CAPTURE'];
  late Game th06;
  setUpAll(() async {
    // The shared product contract supports MIDI on th06. This availability
    // fixture covers a publisher enabling MIDI resources; the pinned site has none.
    final catalog = jsonDecode(
      await rootBundle.loadString('assets/game_catalog.json'),
    ) as List;
    final row = Map<String, dynamic>.from(
      catalog.singleWhere((row) => row['id'] == 'th06') as Map,
    );
    final settings = Map<String, dynamic>.from(row['settings'] as Map);
    settings['music'] = [...settings['music'] as List, 'midi'];
    th06 = Game.fromJson({...row, 'settings': settings});
    if (capture != null) {
      Directory(capture).createSync(recursive: true);
      await loadFonts(Directory(Platform.environment['TOUHOU_CAPTURE_FONTS']!));
    }
  });
  setUp(() {
    SharedPreferencesAsyncPlatform.instance =
        InMemorySharedPreferencesAsync.empty();
  });

  test(
    'MIDI discovery cannot restore a disabled or newer device snapshot',
    () async {
      final requests = <Completer<List<MidiOutput>>>[];
      final changes = StreamController<void>.broadcast();
      final midi = ExternalMidi(
        read: () {
          final result = Completer<List<MidiOutput>>();
          requests.add(result);
          return result.future;
        },
        changes: () => changes.stream,
      );
      addTearDown(midi.dispose);
      addTearDown(changes.close);
      final initial = midi.setEnabled(true);
      final newer = midi.refresh();
      requests[1].complete([(id: 'new', name: 'New output')]);
      await newer;
      requests[0].complete([(id: 'old', name: 'Removed output')]);
      await initial;
      expect(midi.outputs.single.id, 'new');
      final pending = midi.refresh();
      await midi.setEnabled(false);
      requests[2].complete([(id: 'late', name: 'Late result')]);
      await pending;
      expect(midi.enabled, isFalse);
      expect(midi.outputs, isEmpty);
      expect(changes.hasListener, isFalse);
    },
  );

  test(
    'Saved MIDI destination reaches launch while opt-in stays session-only',
    () async {
      final storage = SharedPreferencesAsync();
      final midi = ExternalMidi(
        read: () async => [(id: 'sc55', name: 'SC-55')],
        changes: () => const Stream.empty(),
      );
      addTearDown(midi.dispose);
      final store = GamePreferenceStore(storage, midi: midi);
      await store.save(th06, 'music', 'midi', multiplayer: false);
      await store.save(
        th06,
        'externalMidiDeviceId',
        'sc55',
        multiplayer: false,
      );
      await midi.setEnabled(true);
      final root = await Directory.systemTemp.createTemp('native-midi-launch-');
      addTearDown(() => root.delete(recursive: true));
      final runtime = launcher.RecordingGameLauncher();
      final service = GameLaunchService(
        runtime,
        store,
        GameResources(appDirectory: () async => root),
        GameUserFiles(appDirectory: () async => root),
      );
      await service.start(th06, multiplayer: false, isActive: () => true);
      expect(runtime.lastOptions!.values['externalMidiDeviceId'], 'sc55');
      expect(runtime.lastOptions!.values['externalMidiEnabled'], isTrue);
      final restarted = GamePreferenceStore(storage);
      addTearDown(restarted.midi.dispose);
      expect(restarted.midi.enabled, isFalse);
      expect(
        (await restarted.load(
          th06,
          multiplayer: false,
        )).values['externalMidiDeviceId'],
        'sc55',
      );
      expect(
        (await store.load(th06, multiplayer: false)).values,
        isNot(contains('externalMidiEnabled')),
      );
      await store.save(th06, 'music', 'none', multiplayer: false);
      await service.start(th06, multiplayer: false, isActive: () => true);
      expect(runtime.lastOptions!.values['externalMidiEnabled'], isFalse);
    },
  );

  for (final (name, size, language) in [
    ('mobile-narrow', const Size(320, 640), LauncherLanguage.english),
    ('mobile', const Size(390, 844), LauncherLanguage.japanese),
    ('desktop', const Size(1440, 734), LauncherLanguage.simplifiedChinese),
  ]) {
    testWidgets(
      '$name selects MIDI output, handles unplug and retries discovery',
      (tester) async {
        tester.view.devicePixelRatio = 1;
        tester.view.physicalSize = size;
        addTearDown(tester.view.reset);
        final strings = lookupLauncherStrings(language.locale);
        final changes = StreamController<void>.broadcast();
        addTearDown(changes.close);
        var rows = <MidiOutput>[
          (id: 'one', name: 'Microsoft GS Wavetable Synth'),
          (id: 'two', name: 'SC-55 USB MIDI'),
        ];
        var fail = false;
        final midi = ExternalMidi(
          read: () async {
            if (fail) throw PlatformException(code: 'DISCOVERY_FAILED');
            return rows;
          },
          changes: () => changes.stream,
        );
        final storage = SharedPreferencesAsync();
        final store = GamePreferenceStore(storage, midi: midi);
        await store.save(th06, 'music', 'midi', multiplayer: false);
        final boundary = GlobalKey();
        await tester.pumpWidget(
          RepaintBoundary(
            key: boundary,
            child: launcher.fixture(
              games: [th06],
              initialLocation: '/game/th06/settings',
              gamePreferences: store,
              accountApi: launcher.FixtureAccountApi(),
              preferences: LauncherPreferences(
                storage,
                ThemeMode.dark,
                true,
                language: language,
              ),
            ),
          ),
        );
        await tester.pumpAndSettle();
        if (capture != null) {
          final context = tester.element(find.byType(GameSettingsPage));
          await tester.runAsync(() async {
            await precacheImage(
              AssetImage('assets/book-covers/${th06.artwork}'),
              context,
            );
            await precacheImage(
              const AssetImage('assets/launcher-wordmark-dark.png'),
              context,
            );
          });
          await tester.pumpAndSettle();
        }
        await _tap(tester, find.text(strings.gameAdvancedSettings));
        await _tap(tester, find.byKey(const ValueKey('external MIDI enabled')));
        final selector = find.byKey(const ValueKey('external MIDI device'));
        await _tap(tester, selector);
        await _tap(tester, find.text('SC-55 USB MIDI').last);
        expect(
          (await store.load(
            th06,
            multiplayer: false,
          )).values['externalMidiDeviceId'],
          'two',
        );
        await tester.ensureVisible(selector);
        await tester.pumpAndSettle();
        expect(
          tester.getTopLeft(find.text(strings.externalMidiDevice)).dx,
          tester.getTopLeft(find.text(strings.gameMusic)).dx,
        );
        expect(
          tester.getCenter(selector).dy,
          closeTo(
            tester.getCenter(find.text(strings.externalMidiDevice)).dy,
            .5,
          ),
        );
        if (capture != null) {
          await save(
            tester,
            boundary,
            Directory(capture),
            '$name-midi-selected',
          );
        }
        rows = [rows.first];
        changes.add(null);
        await tester.pumpAndSettle();
        expect(find.text(strings.externalMidiDisconnected), findsOneWidget);
        if (capture != null) {
          await save(
            tester,
            boundary,
            Directory(capture),
            '$name-midi-disconnected',
          );
        }
        rows = [];
        changes.add(null);
        await tester.pumpAndSettle();
        expect(find.text(strings.externalMidiNoDevice), findsOneWidget);
        fail = true;
        await _tap(tester, find.byTooltip(strings.externalMidiRefresh));
        expect(find.text(strings.externalMidiReadFailed), findsOneWidget);
        fail = false;
        rows = [(id: 'two', name: 'SC-55 USB MIDI')];
        await _tap(tester, find.byTooltip(strings.externalMidiRefresh));
        expect(find.text('SC-55 USB MIDI'), findsOneWidget);
        changes.addError(PlatformException(code: 'MONITOR_FAILED'));
        await tester.pumpAndSettle();
        expect(find.text(strings.externalMidiReadFailed), findsOneWidget);
        await _tap(tester, find.byTooltip(strings.externalMidiRefresh));
        expect(find.text('SC-55 USB MIDI'), findsOneWidget);
        final music = find.byKey(const ValueKey('music'));
        await _tap(tester, music);
        await _tap(tester, find.text(strings.gameMusicNone).last);
        expect(midi.enabled, isFalse);
        expect(
          find.byKey(const ValueKey('external MIDI enabled')),
          findsNothing,
        );
        expect(changes.hasListener, isFalse);
        expect(find.byType(GameSettingsPage), findsOneWidget);
        expect(tester.takeException(), isNull);
      },
    );
  }
}
