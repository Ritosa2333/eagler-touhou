import 'dart:async';
import 'dart:convert';
import 'dart:io';

import 'package:flutter/foundation.dart';
import 'package:flutter/services.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:image/image.dart' as image;
import 'package:material_ui/material_ui.dart';
import 'package:shared_preferences/shared_preferences.dart';
import 'package:shared_preferences_platform_interface/in_memory_shared_preferences_async.dart';
import 'package:shared_preferences_platform_interface/shared_preferences_async_platform_interface.dart';
import 'package:touhouonline/l10n/strings.dart';
import 'package:touhouonline/account_api.dart';
import 'package:touhouonline/account_security.dart';
import 'package:touhouonline/cloud_save_page.dart';
import 'package:touhouonline/cloud_saves.dart';
import 'package:touhouonline/desktop_masthead.dart';
import 'package:touhouonline/catalog.dart';
import 'package:touhouonline/profile_crop.dart';
import 'package:touhouonline/profile_display.dart';
import 'package:touhouonline/profile_draft.dart';
import 'package:touhouonline/profile_editor.dart';
import 'package:touhouonline/profile_image.dart';

import '../tools/capture_native_baseline_test.dart' show loadFonts, save;
import 'widget_test.dart' as launcher;
import 'cloud_saves_test.dart' show cloudFixture;

class _EditorFixture extends launcher.FixtureAccountApi {
  int reads = 0;
  ProfileDraft? submitted;

  @override
  Future<CloudSaveDirectory> cloudSaveDirectory() async =>
      CloudSaveDirectory.fromJson(cloudFixture());

  @override
  Future<ProfileDraft> myProfileDraft() async {
    reads++;
    return ProfileDraft.fromJson(
      jsonDecode(File('tools/profile-editor-fixture.json').readAsStringSync())
          as Map<String, dynamic>,
    );
  }

  @override
  Future<Map<String, ProfileReview>> saveProfile(
    ProfileDraft draft, {
    String? avatarThumbnail,
  }) async {
    submitted = draft;
    return const {'name': ProfileReview.pending};
  }
}

class _RestoredEditorFixture extends _EditorFixture {
  bool failRead = true;
  bool failSave = true;
  int writes = 0;

  @override
  bool get canRestoreSession => true;

  @override
  Future<AccountIdentity?> restoreSession() async {
    signedIn = true;
    return (username: 'fixture', name: '测试玩家', expiresAt: null);
  }

  @override
  Future<ProfileDraft> myProfileDraft() async {
    if (failRead) {
      failRead = false;
      throw const AccountRequestException(503, 'SERVICE_UNAVAILABLE');
    }
    return super.myProfileDraft();
  }

  @override
  Future<Map<String, ProfileReview>> saveProfile(
    ProfileDraft draft, {
    String? avatarThumbnail,
  }) async {
    writes++;
    if (failSave) {
      failSave = false;
      throw const AccountRequestException(400, 'CONTENT_REJECTED');
    }
    return super.saveProfile(draft, avatarThumbnail: avatarThumbnail);
  }
}

void _viewport(WidgetTester tester, Size size) {
  debugDefaultTargetPlatformOverride = TargetPlatform.windows;
  addTearDown(() => debugDefaultTargetPlatformOverride = null);
  tester.view.devicePixelRatio = 1;
  tester.view.physicalSize = size;
  addTearDown(tester.view.resetDevicePixelRatio);
  addTearDown(tester.view.resetPhysicalSize);
}

Future<void> _tap(WidgetTester tester, Finder finder) async {
  await tester.ensureVisible(finder);
  await tester.tap(finder);
  await tester.pumpAndSettle();
}

Future<void> _openEditor(WidgetTester tester, {required bool mobile}) async {
  if (mobile) {
    await _tap(tester, find.text('主页'));
    await _tap(tester, find.text('登录'));
  } else {
    await _tap(tester, find.byTooltip('登录'));
  }
  final loginFields = find.byType(TextFormField);
  await tester.enterText(loginFields.first, 'fixture');
  await tester.enterText(loginFields.last, 'valid-test-value');
  await _tap(tester, find.widgetWithText(FilledButton, '登录'));
  if (mobile) {
    await _tap(tester, find.text('账户设置'));
    await _tap(tester, find.text('个人资料'));
  } else {
    await _tap(tester, find.byTooltip('账户'));
    await _tap(tester, find.text('编辑资料'));
  }
  expect(find.byType(ProfileEditorPage), findsOneWidget);
}

Future<void> _waitUntil(WidgetTester tester, bool Function() ready) async {
  final deadline = DateTime.now().add(const Duration(seconds: 12));
  do {
    await Future<void>.delayed(const Duration(milliseconds: 20));
    await tester.pump(const Duration(milliseconds: 20));
    if (ready()) return;
  } while (DateTime.now().isBefore(deadline));
  fail('The crop dialog did not finish processing');
}

void main() {
  final localeOutput = Platform.environment['TOUHOU_EDITOR_LOCALE_CAPTURE'];
  if (localeOutput != null) {
    setUpAll(
      () => loadFonts(Directory(Platform.environment['TOUHOU_CAPTURE_FONTS']!)),
    );
  }
  setUp(
    () => SharedPreferencesAsyncPlatform.instance =
        InMemorySharedPreferencesAsync.empty(),
  );
  for (final (size, language, stem) in [
    (
      const Size(1440, 734),
      LauncherLanguage.simplifiedChinese,
      'desktop-profile-edit-zh-CN',
    ),
    (
      const Size(320, 640),
      LauncherLanguage.english,
      'mobile-narrow-profile-edit-en',
    ),
    (const Size(390, 844), LauncherLanguage.japanese, 'mobile-profile-edit-ja'),
  ]) {
    testWidgets('editor language, retry and draft return | $stem', (
      tester,
    ) async {
      _viewport(tester, size);
      if (localeOutput != null) {
        late BuildContext imageContext;
        await tester.pumpWidget(
          Builder(
            builder: (context) {
              imageContext = context;
              return const SizedBox.shrink();
            },
          ),
        );
        await tester.runAsync(
          () => precacheImage(
            const AssetImage('assets/launcher-wordmark-dark.png'),
            imageContext,
          ),
        );
      }
      final fixture = _RestoredEditorFixture();
      final boundary = GlobalKey();
      await tester.pumpWidget(
        RepaintBoundary(
          key: boundary,
          child: launcher.fixture(
            accountApi: fixture,
            initialLocation: '/u/fixture/edit',
            preferences: LauncherPreferences(
              SharedPreferencesAsync(),
              ThemeMode.dark,
              true,
              language: language,
            ),
          ),
        ),
      );
      await tester.pumpAndSettle();
      final editor = find.byType(ProfileEditorPage);
      expect(editor, findsOneWidget);
      final strings = tester.element(editor).strings;
      await _tap(tester, find.text(strings.profileEditorLoad));
      final name = find.byKey(const ValueKey('profile-name'));
      expect(tester.widget<TextFormField>(name).controller!.text, '测试玩家');
      expect(find.byKey(const ValueKey('choose-avatar')), findsOneWidget);
      expect(find.byKey(const ValueKey('choose-cover')), findsOneWidget);
      if (size.width < 400) {
        expect(
          find.byTooltip(strings.profileEditorChooseImage),
          findsNWidgets(2),
        );
      }
      if (localeOutput != null) {
        List<double> bounds(Finder finder) {
          final rect = tester.getRect(finder);
          return [rect.left, rect.top, rect.width, rect.height];
        }

        final geometry = {
          'viewport': [size.width, size.height],
          'preview': bounds(
            find.byKey(const ValueKey('profile-editor-preview')),
          ),
          'name': bounds(name),
          'linkName': bounds(
            find.byWidgetPredicate(
              (widget) =>
                  widget is TextFormField && widget.controller?.text == '项目主页',
            ),
          ),
          'linkAddress': bounds(
            find.byWidgetPredicate(
              (widget) =>
                  widget is TextFormField &&
                  widget.controller?.text == 'https://example.org/',
            ),
          ),
        };
        Directory(localeOutput).createSync(recursive: true);
        File('$localeOutput/$stem.geometry.json')
            .writeAsStringSync('${jsonEncode(geometry)}\n');
        await save(
          tester,
          boundary,
          Directory(localeOutput)..createSync(recursive: true),
          stem,
        );
      }

      await tester.ensureVisible(name);
      await tester.enterText(name, 'Draft');
      await tester.pumpAndSettle();
      await _tap(tester, find.text(strings.profileEditorSave));
      expect(find.text(strings.profileEditorRejected), findsOneWidget);
      if (localeOutput != null) {
        await save(tester, boundary, Directory(localeOutput), '$stem-rejected');
      }
      expect(fixture.writes, 1);
      expect(tester.widget<TextFormField>(name).controller!.text, 'Draft');
      await tester.binding.handlePopRoute();
      await tester.pumpAndSettle();
      expect(find.text(strings.profileEditorDiscardTitle), findsOneWidget);
      if (localeOutput != null) {
        await save(tester, boundary, Directory(localeOutput), '$stem-discard');
      }
      await _tap(tester, find.text(strings.profileEditorContinue));
      await _tap(tester, find.text(strings.profileEditorSave));
      expect(fixture.writes, 2);
      expect(fixture.submitted!.name, 'Draft');
      expect(find.text(strings.profileEditorSubmitted), findsOneWidget);
      expect(tester.takeException(), isNull);
      await _tap(tester, find.byKey(const ValueKey('profile-editor-back')));
      expect(editor, findsNothing);
      await tester.pumpWidget(const SizedBox.shrink());
    });
  }
  testWidgets('320 px editor returns home, validates links, saves review', (
    tester,
  ) async {
    _viewport(tester, const Size(320, 640));
    final fixture = _EditorFixture();
    final changes = <String>[];
    final subscription = fixture.profileChanges
        .map((change) => change.username)
        .listen(changes.add);
    addTearDown(subscription.cancel);
    await tester.pumpWidget(launcher.fixture(accountApi: fixture));
    await tester.pumpAndSettle();
    await _openEditor(tester, mobile: true);

    expect(fixture.reads, 1);
    final name = find.byKey(const ValueKey('profile-name'));
    expect(tester.widget<TextFormField>(name).controller!.text, '测试玩家');
    await tester.ensureVisible(name);
    await tester.enterText(name, '改后的昵称');
    await tester.pump();
    await tester.binding.handlePopRoute();
    await tester.pumpAndSettle();
    expect(find.text('放弃未保存的修改？'), findsOneWidget);
    await _tap(tester, find.text('继续编辑'));
    expect(tester.widget<TextFormField>(name).controller!.text, '改后的昵称');
    expect(fixture.submitted, isNull);

    await _tap(tester, find.byTooltip('查看公开个人主页'));
    expect(find.text('改后的昵称'), findsNothing);
    await _tap(tester, find.byType(BackButton));
    expect(tester.widget<TextFormField>(name).controller!.text, '改后的昵称');

    final link = find.widgetWithText(TextFormField, '链接地址');
    await tester.ensureVisible(link);
    await tester.enterText(link, 'invalid');
    await _tap(tester, find.text('保存'));
    expect(find.text('请输入有效的 HTTP 或 HTTPS 地址'), findsOneWidget);
    expect(fixture.submitted, isNull);
    await tester.enterText(link, 'https://example.org/project');
    await tester.testTextInput.receiveAction(TextInputAction.done);
    await tester.pumpAndSettle();

    await _tap(tester, find.byKey(const ValueKey('visibility-th09')));
    await _tap(tester, find.text('公开关系列表'));
    await _tap(tester, find.text('保存'));
    expect(fixture.submitted!.name, '改后的昵称');
    expect(fixture.submitted!.links.single.url, 'https://example.org/project');
    expect(fixture.submitted!.display.views, [ProfileGames.all]);
    expect(fixture.submitted!.display.friends, isFalse);
    expect(changes, ['fixture']);
    expect(find.text('资料已提交审核'), findsOneWidget);
    await tester.pump(const Duration(seconds: 10));
    await tester.pumpAndSettle();
    expect(find.text('个人资料已保存'), findsOneWidget);
    expect(tester.widget<TextFormField>(name).controller!.text, '改后的昵称');

    await _tap(tester, find.byType(BackButton));
    expect(find.text('账户设置'), findsOneWidget);
    expect(find.byType(ProfileEditorPage), findsNothing);
    expect(tester.takeException(), isNull);
    await tester.pumpWidget(const SizedBox.shrink());
  });

  testWidgets('desktop editor discards changes and returns to library label', (
    tester,
  ) async {
    _viewport(tester, const Size(1440, 734));
    final fixture = _EditorFixture();
    await tester.pumpWidget(launcher.fixture(accountApi: fixture));
    await tester.pumpAndSettle();
    await _openEditor(tester, mobile: false);
    final name = find.byKey(const ValueKey('profile-name'));
    await tester.enterText(name, '临时修改');
    await _tap(tester, find.byKey(const ValueKey('profile-editor-back')));
    await _tap(tester, find.text('放弃修改'));
    expect(fixture.submitted, isNull);
    expect(find.byType(SearchBar), findsOneWidget);
    expect(find.byType(ProfileEditorPage), findsNothing);
  });

  testWidgets(
    'settings navigation protects drafts and returns to its library entry',
    (tester) async {
      _viewport(tester, const Size(1440, 734));
      final fixture = _EditorFixture();
      await tester.pumpWidget(launcher.fixture(accountApi: fixture));
      await tester.pumpAndSettle();
      await _openEditor(tester, mobile: false);
      final masthead = find.byType(DesktopMasthead);
      final headerElement = tester.element(masthead);
      final headerBounds = tester.getRect(masthead);
      final security = find.byKey(const ValueKey('account-settings-security'));
      final navigationBounds = tester.getRect(security);
      final name = find.byKey(const ValueKey('profile-name'));
      await tester.enterText(name, 'Local draft');
      await _tap(tester, security);
      expect(find.text('放弃未保存的修改？'), findsOneWidget);
      await _tap(tester, find.text('继续编辑'));
      expect(
        tester.widget<TextFormField>(name).controller!.text,
        'Local draft',
      );
      await _tap(tester, security);
      await _tap(tester, find.text('放弃修改'));
      expect(find.byType(AccountSecurityPage), findsOneWidget);
      expect(fixture.submitted, isNull);
      expect(tester.element(masthead), same(headerElement));
      expect(tester.getRect(masthead), headerBounds);
      expect(tester.getRect(security), navigationBounds);
      await _tap(tester, find.byKey(const ValueKey('account-settings-games')));
      expect(find.byType(CloudSavePage), findsOneWidget);
      expect(tester.element(masthead), same(headerElement));
      expect(tester.getRect(masthead), headerBounds);
      expect(tester.getRect(security), navigationBounds);
      await _tap(tester, find.byKey(const ValueKey('cloud-settings-back')));
      expect(find.byType(CloudSavePage), findsNothing);
      expect(find.byType(SearchBar), findsOneWidget);
      expect(tester.takeException(), isNull);
      await tester.pumpWidget(const SizedBox.shrink());
    },
  );

  for (final target in ['单机', '联机']) {
    testWidgets(
      'masthead $target protects an unsaved profile on repeated exits',
      (tester) async {
        _viewport(tester, const Size(1440, 734));
        final fixture = _EditorFixture();
        addTearDown(() => tester.pumpWidget(const SizedBox.shrink()));
        await tester.pumpWidget(launcher.fixture(accountApi: fixture));
        await tester.pumpAndSettle();
        await _openEditor(tester, mobile: false);
        final name = find.byKey(const ValueKey('profile-name'));
        await tester.enterText(name, 'Draft');
        final destination = find.descendant(
          of: find.byType(DesktopMasthead),
          matching: find.text(target),
        );
        for (var attempt = 0; attempt < 2; attempt++) {
          await _tap(tester, destination);
          expect(find.text('放弃未保存的修改？'), findsOneWidget);
          await _tap(tester, find.text('继续编辑'));
          expect(tester.widget<TextFormField>(name).controller!.text, 'Draft');
        }
        await _tap(tester, destination);
        await _tap(tester, find.text('放弃修改'));
        expect(find.byType(ProfileEditorPage), findsNothing);
        expect(fixture.submitted, isNull);
        expect(find.text('放弃未保存的修改？'), findsNothing);
        expect(tester.takeException(), isNull);
      },
    );
  }

  testWidgets(
    'stacked profile editors protect each mounted page independently',
    (tester) async {
      _viewport(tester, const Size(1440, 734));
      final fixture = _EditorFixture();
      addTearDown(() => tester.pumpWidget(const SizedBox.shrink()));
      await tester.pumpWidget(launcher.fixture(accountApi: fixture));
      await tester.pumpAndSettle();
      await _openEditor(tester, mobile: false);
      final name = find.byKey(const ValueKey('profile-name'));
      await tester.enterText(name, 'First draft');
      await _tap(tester, find.byTooltip('查看公开个人主页'));
      await _tap(tester, find.byTooltip('账户').hitTestable());
      await _tap(tester, find.text('编辑资料').hitTestable());
      expect(tester.widget<TextFormField>(name).controller!.text, '测试玩家');
      final library = find
          .descendant(
            of: find.byType(DesktopMasthead),
            matching: find.text('单机'),
          )
          .hitTestable();
      await _tap(tester, library);
      expect(find.text('放弃未保存的修改？'), findsOneWidget);
      await _tap(tester, find.text('继续编辑'));
      await _tap(tester, find.byKey(const ValueKey('profile-editor-back')));
      await tester.binding.handlePopRoute();
      await tester.pumpAndSettle();
      expect(
        tester.widget<TextFormField>(name).controller!.text,
        'First draft',
      );
      await _tap(tester, library);
      expect(find.text('放弃未保存的修改？'), findsOneWidget);
      await _tap(tester, find.text('放弃修改'));
      expect(find.byType(ProfileEditorPage), findsNothing);
      expect(fixture.submitted, isNull);
      expect(tester.takeException(), isNull);
    },
  );

  testWidgets(
    'profile draft and input focus survive layout and text-size changes',
    (tester) async {
      _viewport(tester, const Size(1440, 734));
      await tester.pumpWidget(launcher.fixture(accountApi: _EditorFixture()));
      await tester.pumpAndSettle();
      await _openEditor(tester, mobile: false);
      final name = find.byKey(const ValueKey('profile-name'));
      await tester.tap(name);
      await tester.enterText(name, 'Draft');
      final fieldState = tester.state(name);
      for (final size in [const Size(390, 844), const Size(1440, 734)]) {
        tester.view.physicalSize = size;
        await tester.pumpAndSettle();
        expect(tester.state(name), same(fieldState));
        expect(tester.widget<TextFormField>(name).controller!.text, 'Draft');
        final input = find.descendant(
          of: name,
          matching: find.byType(EditableText),
        );
        expect(tester.widget<EditableText>(input).focusNode.hasFocus, isTrue);
        expect(tester.takeException(), isNull);
      }
      tester.platformDispatcher.textScaleFactorTestValue = 2;
      addTearDown(tester.platformDispatcher.clearTextScaleFactorTestValue);
      await tester.pumpAndSettle();
      expect(tester.state(name), same(fieldState));
      expect(tester.takeException(), isNull);
      await tester.pumpWidget(const SizedBox.shrink());
    },
  );

  test('real artwork encodes both upload shapes and tiny files', () async {
    final bytes = await File('assets/account-background.webp').readAsBytes();
    final source = await prepareProfileSource(bytes);
    final avatar = await encodeProfileCrop(
      source,
      ProfileImageKind.avatar,
      fitProfileCrop(source, ProfileImageKind.avatar),
    );
    final cover = await encodeProfileCrop(
      source,
      ProfileImageKind.cover,
      fitProfileCrop(source, ProfileImageKind.cover),
    );
    final avatarBytes = profileImageBytes(avatar.data)!;
    final coverBytes = profileImageBytes(cover.data)!;
    final thumbnailBytes = profileImageBytes(avatar.thumbnail!)!;
    expect(avatarBytes.length, lessThanOrEqualTo(profileImageLimit));
    expect(coverBytes.length, lessThanOrEqualTo(profileImageLimit));
    expect(thumbnailBytes.length, lessThanOrEqualTo(profileThumbnailLimit));
    final avatarImage = image.decodeImage(avatarBytes)!;
    final thumbImage = image.decodeImage(thumbnailBytes)!;
    final coverImage = image.decodeImage(coverBytes)!;
    expect(avatarImage.width, avatarImage.height);
    expect(avatarImage.width, lessThanOrEqualTo(512));
    expect(thumbImage.width, thumbImage.height);
    expect(thumbImage.width, lessThanOrEqualTo(64));
    expect(coverImage.width, coverImage.height * 6);
    expect(cover.thumbnail, isNull);
    final storedThumbnail = profileImageBytes(
      await encodeStoredProfileThumbnail(avatarBytes),
    )!;
    expect(storedThumbnail.length, lessThanOrEqualTo(profileThumbnailLimit));
    expect(image.decodeImage(storedThumbnail)!.width, 64);
    final tiny = await prepareProfileSource(
      image.encodePng(image.Image(width: 1, height: 1)),
    );
    final tinyResult = await encodeProfileCrop(
      tiny,
      ProfileImageKind.avatar,
      fitProfileCrop(tiny, ProfileImageKind.avatar),
    );
    expect(image.decodeImage(profileImageBytes(tinyResult.data)!)!.width, 1);
    await expectLater(
      prepareProfileSource(Uint8List.fromList([1, 2, 3])),
      throwsA(isA<ProfileImageException>()),
    );
  });

  final cropOutput = Platform.environment['TOUHOU_PROFILE_CROP_CAPTURE'];
  for (final (device, size) in [
    ('desktop', const Size(1440, 734)),
    ('mobile', const Size(390, 844)),
    ('mobile-narrow', const Size(320, 640)),
  ]) {
    testWidgets('$device crop zoom, previews and return', (tester) async {
      _viewport(tester, size);
      final source = (await tester.runAsync(() async {
        if (cropOutput != null) {
          await loadFonts(
            Directory(Platform.environment['TOUHOU_CAPTURE_FONTS']!),
          );
        }
        return prepareProfileSource(
          await File('assets/account-background.webp').readAsBytes(),
        );
      }))!;
      final root = GlobalKey();
      await tester.pumpWidget(
        RepaintBoundary(
          key: root,
          child: launcher.fixture(accountApi: _EditorFixture()),
        ),
      );
      await tester.pumpAndSettle();
      await _openEditor(tester, mobile: device != 'desktop');
      final editor = tester.element(find.byType(ProfileEditorPage));
      final strings = editor.strings;
      final preview = find.byKey(const ValueKey('profile-crop-preview'));
      final decoded = find.descendant(
        of: find.byType(AlertDialog),
        matching: find.byWidgetPredicate(
          (widget) => widget is RawImage && widget.image != null,
        ),
      );
      final slider = find.byType(Slider);
      PreparedProfileImage? chosen;

      for (final kind in ProfileImageKind.values) {
        var closed = false;
        unawaited(
          cropProfileImage(editor, source, kind).then((result) {
            chosen = result;
            closed = true;
          }),
        );
        await tester.runAsync(() async {
          final count = kind == ProfileImageKind.avatar ? 1 : 2;
          await _waitUntil(
            tester,
            () =>
                preview.evaluate().length == count &&
                decoded.evaluate().length >= count + 1,
          );
        });
        await tester.pumpAndSettle();
        if (cropOutput != null) {
          final output = Directory(cropOutput)..createSync(recursive: true);
          await save(tester, root, output, '$device-profile-crop-${kind.name}');
        }
        final original =
            (tester.widget<Image>(preview.first).image as MemoryImage).bytes;
        if (kind == ProfileImageKind.cover) {
          expect(find.text(strings.profileCropMobile), findsOneWidget);
          expect(find.text(strings.profileCropDesktop), findsOneWidget);
          final formats = preview.evaluate().map(
            (element) => element
                .findAncestorWidgetOfExactType<AspectRatio>()!
                .aspectRatio,
          );
          expect(formats, [3, 6]);
          final mobile = tester.getTopLeft(preview.first);
          final desktop = tester.getTopLeft(preview.last);
          if (size.width <= 480) {
            expect(desktop.dx, closeTo(mobile.dx, .1));
            expect(desktop.dy, greaterThan(mobile.dy));
          } else {
            expect(desktop.dx, greaterThan(mobile.dx));
            expect(desktop.dy, closeTo(mobile.dy, .1));
          }
          expect(
            find.text(strings.profileCropConfirm).hitTestable(),
            findsOneWidget,
          );
          if (device == 'desktop') {
            await tester.sendKeyEvent(LogicalKeyboardKey.escape);
          } else {
            await tester.binding.handlePopRoute();
          }
          await tester.pumpAndSettle();
          expect(closed, isTrue);
          expect(chosen, isNull);
        } else {
          final start = tester.widget<Slider>(slider).value;
          await _tap(tester, find.byTooltip(strings.profileCropZoomIn));
          expect(tester.widget<Slider>(slider).value, greaterThan(start));
          await _tap(tester, find.byTooltip(strings.profileCropReset));
          expect(tester.widget<Slider>(slider).value, closeTo(start, .001));
          await tester.tapAt(tester.getCenter(slider));
          await tester.pumpAndSettle();
          expect(tester.widget<Slider>(slider).value, greaterThan(2));
          final workspace = find.byKey(
            const ValueKey('profile-crop-workspace'),
          );
          Uint8List previewBytes() =>
              (tester.widget<Image>(preview).image as MemoryImage).bytes;
          await tester.runAsync(() async {
            await _waitUntil(
              tester,
              () => !identical(previewBytes(), original),
            );
            final beforeKey = previewBytes();
            Focus.of(tester.element(workspace.first)).requestFocus();
            await tester.pump();
            await tester.sendKeyEvent(LogicalKeyboardKey.arrowRight);
            await _waitUntil(
              tester,
              () => !identical(previewBytes(), beforeKey),
            );
            final thumbnail = previewBytes();
            await tester.tap(
              find.widgetWithText(FilledButton, strings.profileCropConfirm),
            );
            await _waitUntil(tester, () => closed);
            expect(
              profileImageBytes(chosen!.thumbnail!),
              orderedEquals(thumbnail),
            );
          });
        }
        await tester.pumpAndSettle();
        expect(find.byType(AlertDialog), findsNothing);
        expect(find.byType(ProfileEditorPage), findsOneWidget);
        expect(tester.takeException(), isNull);
      }
    });
  }

  final outputPath = Platform.environment['TOUHOU_PROFILE_EDITOR_CAPTURE'];
  if (outputPath != null) {
    final output = Directory(outputPath)..createSync(recursive: true);
    setUpAll(
      () => loadFonts(Directory(Platform.environment['TOUHOU_CAPTURE_FONTS']!)),
    );
    for (final (device, size) in [
      ('desktop', const Size(1440, 734)),
      ('mobile', const Size(390, 844)),
      ('mobile-narrow', const Size(320, 640)),
    ]) {
      for (final light in [false, true]) {
        final stem = light ? '$device-light' : device;
        testWidgets('capture $stem profile editor', (tester) async {
          _viewport(tester, size);
          final boundary = GlobalKey();
          await tester.pumpWidget(
            RepaintBoundary(
              key: boundary,
              child: launcher.fixture(
                accountApi: _EditorFixture(),
                preferences: LauncherPreferences(
                  SharedPreferencesAsync(),
                  light ? ThemeMode.light : ThemeMode.dark,
                  true,
                ),
              ),
            ),
          );
          await tester.pumpAndSettle();
          await _openEditor(tester, mobile: device != 'desktop');
          await save(tester, boundary, output, '$stem-profile-edit');
          await tester.ensureVisible(find.text('全部作品'));
          await tester.pumpAndSettle();
          await save(tester, boundary, output, '$stem-profile-display');
          expect(tester.takeException(), isNull);
        });
      }
    }
  }
}
