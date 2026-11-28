import 'dart:async';
import 'dart:io';
import 'dart:typed_data';

import 'package:flutter/rendering.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:material_ui/material_ui.dart';
import 'package:shared_preferences_platform_interface/in_memory_shared_preferences_async.dart';
import 'package:shared_preferences_platform_interface/shared_preferences_async_platform_interface.dart';
import 'package:touhouonline/account.dart';
import 'package:touhouonline/account_api.dart';

import '../tools/capture_native_baseline_test.dart' show loadFonts, save;
import 'widget_test.dart' as launcher;

class _Avatars extends launcher.FixtureAccountApi {
  final reads = <String, Future<Uri?> Function()>{};
  @override
  bool get canRestoreSession => true;
  @override
  Future<AccountIdentity?> restoreSession() async =>
      (username: 'fixture', name: '头像样例', expiresAt: null);
  @override
  Future<Uri?> avatar(String username) =>
      reads[username.toLowerCase()]?.call() ?? Future.value(null);
  @override
  Future<AnnouncementPage> announcements({int? before}) async => (
    items: [
      PublicAnnouncement(
        id: 14,
        author: '头像样例',
        authorUsername: 'fixture',
        body: '验证公告和账户入口使用同一张已更新头像。',
        publishedAt: DateTime.utc(2026, 11, 25),
      ),
    ],
    nextBefore: null,
    canPublish: false,
  );
}

final _avatars = find.byType(AccountAvatar);
Finder _photos() => find.descendant(of: _avatars, matching: find.byType(Image));

Future<void> _paint(WidgetTester tester) async {
  for (final element in _photos().evaluate().toList()) {
    await tester.runAsync(
      () => precacheImage(
        (element.widget as Image).image,
        element,
        onError: (_, _) {},
      ),
    );
  }
  await tester.pumpAndSettle();
}

void _photoEquals(WidgetTester tester, Uint8List expected, int count) {
  expect(_photos(), findsNWidgets(count));
  for (final image in tester.widgetList<Image>(_photos())) {
    expect((image.image as MemoryImage).bytes, orderedEquals(expected));
  }
  final painted = find.descendant(
    of: _avatars,
    matching: find.byType(RawImage),
  );
  expect(painted, findsNWidgets(count));
  for (final render in painted.evaluate()) {
    expect((render.renderObject as RenderImage).image, isNotNull);
  }
}

void main() {
  TestWidgetsFlutterBinding.ensureInitialized();
  final output = Platform.environment['TOUHOU_AVATAR_CAPTURE'];
  late Uint8List first, second;
  late Uri firstUri, secondUri;
  setUpAll(() async {
    first = File('assets/th09-portraits/0.webp').readAsBytesSync();
    second = File('assets/th09-portraits/1.webp').readAsBytesSync();
    firstUri = Uri.dataFromBytes(first, mimeType: 'image/webp');
    secondUri = Uri.dataFromBytes(second, mimeType: 'image/webp');
    if (output != null) {
      await loadFonts(Directory(Platform.environment['TOUHOU_CAPTURE_FONTS']!));
    }
  });
  setUp(() {
    SharedPreferencesAsyncPlatform.instance =
        InMemorySharedPreferencesAsync.empty();
  });

  testWidgets(
    'Shared avatar ignores a previous identity and handles missing media',
    (tester) async {
      final api = _Avatars();
      addTearDown(api.close);
      final slowFirst = Completer<Uri?>(), slowSecond = Completer<Uri?>();
      api.reads['first'] = () => slowFirst.future;
      api.reads['second'] = () => slowSecond.future;
      Widget avatar(String? name) => MaterialApp(
        home: Scaffold(
          body: Center(
            child: AccountAvatar(api: api, username: name, size: 54),
          ),
        ),
      );
      await tester.pumpWidget(avatar('first'));
      final bounds = tester.getRect(_avatars);
      expect(find.byIcon(Icons.person_outline), findsOneWidget);
      await tester.pumpWidget(avatar('second'));
      slowFirst.complete(firstUri);
      await tester.pumpAndSettle();
      expect(_photos(), findsNothing);
      slowSecond.complete(secondUri);
      await tester.pumpAndSettle();
      await _paint(tester);
      _photoEquals(tester, second, 1);
      expect(tester.getRect(_avatars), bounds);

      api.reads['second'] = () async => firstUri;
      api.invalidateProfile('SECOND');
      await tester.pumpAndSettle();
      await _paint(tester);
      _photoEquals(tester, first, 1);
      api.reads['second'] = () =>
          Future.error(const AccountRequestException(404, 'PROFILE_NOT_FOUND'));
      api.invalidateProfile('second');
      await tester.pumpAndSettle();
      expect(find.byIcon(Icons.person_outline), findsOneWidget);
      expect(_photos(), findsNothing);
      api.reads['second'] = () async =>
          Uri.dataFromBytes([1, 2, 3], mimeType: 'image/webp');
      api.invalidateProfile('second');
      await tester.pumpAndSettle();
      await _paint(tester);
      expect(find.byIcon(Icons.person_outline), findsOneWidget);
      expect(tester.getRect(_avatars), bounds);
      await tester.pumpWidget(avatar('missing'));
      await tester.pumpAndSettle();
      expect(find.byIcon(Icons.person_outline), findsOneWidget);
      await tester.pumpWidget(avatar(null));
      await tester.pumpAndSettle();
      expect(find.byIcon(Icons.person_outline), findsOneWidget);
      expect(_photos(), findsNothing);
      expect(tester.takeException(), isNull);
      await tester.pumpWidget(const SizedBox.shrink());
      await tester.pumpAndSettle();
    },
  );

  for (final width in [320.0, 1440.0]) {
    testWidgets('Announcement entry refreshes every shared avatar | $width', (
      tester,
    ) async {
      tester.view.physicalSize = Size(width, width == 320 ? 640 : 734);
      tester.view.devicePixelRatio = 1;
      addTearDown(tester.view.resetPhysicalSize);
      addTearDown(tester.view.resetDevicePixelRatio);
      final api = _Avatars();
      addTearDown(api.close);
      var media = firstUri;
      api.reads['fixture'] = () async => media;
      final boundary = GlobalKey();
      await tester.pumpWidget(
        RepaintBoundary(
          key: boundary,
          child: launcher.fixture(
            accountApi: api,
            initialLocation: '/announcements',
          ),
        ),
      );
      await tester.pumpAndSettle();
      await _paint(tester);
      final count = width == 320 ? 1 : 2;
      _photoEquals(tester, first, count);
      final rects = _avatars
          .evaluate()
          .map((e) => tester.getRect(find.byWidget(e.widget)))
          .toList();
      media = secondUri;
      api.invalidateProfile('fixture');
      await tester.pumpAndSettle();
      await _paint(tester);
      _photoEquals(tester, second, count);
      expect(
        _avatars
            .evaluate()
            .map((e) => tester.getRect(find.byWidget(e.widget)))
            .toList(),
        rects,
      );
      if (output != null) {
        await save(
          tester,
          boundary,
          Directory(output),
          'announcement-avatar-${width.toInt()}',
        );
      }
      expect(tester.takeException(), isNull);
      await tester.pumpWidget(const SizedBox.shrink());
      await tester.pumpAndSettle();
    });
  }
}
