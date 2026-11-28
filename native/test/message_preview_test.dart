import 'dart:io';

import 'package:flutter/services.dart';
import 'package:flutter/foundation.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:go_router/go_router.dart';
import 'package:material_ui/material_ui.dart';
import 'package:shared_preferences/shared_preferences.dart';
import 'package:shared_preferences_platform_interface/in_memory_shared_preferences_async.dart';
import 'package:shared_preferences_platform_interface/shared_preferences_async_platform_interface.dart';
import 'package:touhouonline/account_api.dart';
import 'package:touhouonline/catalog.dart';
import 'package:touhouonline/l10n/strings.dart';
import 'package:touhouonline/library.dart';

import '../tools/capture_native_baseline_test.dart' show loadFonts, save;
import 'widget_test.dart' as fixtures;

class _MessagesApi extends fixtures.FixtureAccountApi {
  bool failFeed = false;
  int summaryRequests = 0;
  static final longAnnouncement = List.generate(
    12,
    (i) => '公告正文第 ${i + 1} 行',
  ).join('\n');
  static const earlierAnnouncement = '上一条公告';

  @override
  Future<AnnouncementPage> announcements({int? before}) async {
    if (failFeed) throw const AccountRequestException(503, 'UNAVAILABLE');
    return (
      canPublish: canPublish,
      nextBefore: null,
      items: [
        for (final (id, body) in [
          (2, longAnnouncement),
          (1, earlierAnnouncement),
        ])
          PublicAnnouncement(
            id: id,
            author: '测试玩家',
            authorUsername: 'fixture',
            body: body,
            publishedAt: DateTime.utc(2026, 11, 24, 16, 20),
          ),
      ],
    );
  }

  @override
  Future<InboxPage> inbox({int? before, bool summary = false}) async {
    if (summary) summaryRequests++;
    final page = await super.inbox(before: before, summary: summary);
    return (
      threads: [
        ...page.threads,
        (
          peer: (username: 'second', nickname: '第二位玩家'),
          lastMessage: DirectMessage(
            id: 2,
            body: '准备开始了吗？',
            outgoing: false,
            sentAt: DateTime(2026, 11, 24, 18),
          ),
          unread: 0,
        ),
      ],
      unread: page.unread,
      nextBefore: page.nextBefore,
    );
  }

  @override
  Future<ConversationPage> conversation(String username, {int? before}) async {
    final page = await super.conversation(username, before: before);
    if (username != 'second') return page;
    return (
      peer: (username: username, nickname: '第二位玩家'),
      messages: [
        DirectMessage(
          id: 2,
          body: '准备开始了吗？',
          outgoing: false,
          sentAt: DateTime(2026, 11, 24, 18),
        ),
      ],
      nextBefore: null,
    );
  }
}

class _BubbleApi extends _MessagesApi {
  @override
  bool get canRestoreSession => true;
  @override
  Future<AccountIdentity?> restoreSession() async {
    signedIn = true;
    return (username: 'fixture', name: '测试玩家', expiresAt: null);
  }

  static const short = '开始吗？';
  static final long = List.filled(160, 'A').join();
  @override
  Future<ConversationPage> conversation(String username, {int? before}) async =>
      (
        peer: (username: username, nickname: '另一位玩家'),
        messages: [
          for (final (id, body, outgoing) in [
            (1, short, false),
            (2, long, false),
            (3, '好，马上开始。', true),
          ])
            DirectMessage(
              id: id,
              body: body,
              outgoing: outgoing,
              sentAt: DateTime(2026, 11, 25, 10, id),
            ),
        ],
        nextBefore: null,
      );
}

class _HistoryApi extends _MessagesApi {
  int latestId = 25;
  int latestRequests = 0;
  int olderRequests = 0;
  int inboxRequests = 0;

  DirectMessage message(int id) => DirectMessage(
    id: id,
    body: id.isEven ? '会话消息 $id' : '会话消息 $id\n这是第二行',
    outgoing: false,
    sentAt: DateTime(2026, 11, 25, 10, id),
  );

  @override
  Future<InboxPage> inbox({int? before, bool summary = false}) async {
    if (summary) {
      summaryRequests++;
    } else {
      inboxRequests++;
    }
    final unread = latestId > (readThrough ?? 0) ? 1 : 0;
    return (
      threads: [
        (
          peer: (username: 'peer', nickname: '另一位玩家'),
          lastMessage: message(latestId),
          unread: unread,
        ),
      ],
      unread: unread,
      nextBefore: null,
    );
  }

  @override
  Future<ConversationPage> conversation(String username, {int? before}) async {
    final end = before ?? latestId + 1;
    final start = (end - 15).clamp(1, end);
    if (before == null) {
      latestRequests++;
    } else {
      olderRequests++;
    }
    return (
      peer: (username: username, nickname: '另一位玩家'),
      messages: [for (var id = start; id < end; id++) message(id)],
      nextBefore: start > 1 ? start : null,
    );
  }
}

Finder _badge(String path) => find.byKey(ValueKey((path, 'unread')));
Finder _panel(String path) => find.byKey(ValueKey((path, 'preview')));

Future<void> _tap(WidgetTester tester, Finder target) async {
  expect(target.hitTestable(), findsOneWidget);
  await tester.tap(target);
  await tester.pumpAndSettle();
}

Future<GoRouter> _open(
  WidgetTester tester,
  Size size,
  _MessagesApi api, {
  GlobalKey? boundary,
}) async {
  tester.view.devicePixelRatio = 1;
  tester.view.physicalSize = size;
  addTearDown(tester.view.resetPhysicalSize);
  addTearDown(tester.view.resetDevicePixelRatio);
  final app = fixtures.fixture(accountApi: api);
  await tester.pumpWidget(
    boundary == null ? app : RepaintBoundary(key: boundary, child: app),
  );
  await tester.pumpAndSettle();
  return GoRouter.of(tester.element(find.byType(LibraryPage)));
}

void main() {
  setUp(() {
    SharedPreferencesAsyncPlatform.instance =
        InMemorySharedPreferencesAsync.empty();
  });

  if (Platform.environment['TOUHOU_BUBBLE_CAPTURE'] case final String capture) {
    for (final (device, size, language) in [
      ('desktop', const Size(1440, 734), LauncherLanguage.simplifiedChinese),
      ('mobile', const Size(390, 844), LauncherLanguage.japanese),
      ('narrow', const Size(320, 640), LauncherLanguage.english),
    ]) {
      for (final light in [false, true]) {
        for (final scale in [1.0, 2.0]) {
          final name = '$device-${light ? 'light' : 'dark'}-${scale.toInt()}';
          testWidgets('message bubble layout | $name', (tester) async {
            debugDefaultTargetPlatformOverride = TargetPlatform.windows;
            addTearDown(() => debugDefaultTargetPlatformOverride = null);
            await tester.runAsync(() => loadFonts());
            tester.view.devicePixelRatio = 1;
            tester.view.physicalSize = size;
            addTearDown(tester.view.resetDevicePixelRatio);
            addTearDown(tester.view.resetPhysicalSize);
            tester.platformDispatcher.textScaleFactorTestValue = scale;
            addTearDown(
              tester.platformDispatcher.clearTextScaleFactorTestValue,
            );
            final boundary = GlobalKey();
            final strings = lookupLauncherStrings(language.locale);
            final app = fixtures.fixture(
              accountApi: _BubbleApi(),
              preferences: LauncherPreferences(
                SharedPreferencesAsync(),
                light ? ThemeMode.light : ThemeMode.dark,
                true,
                language: language,
              ),
            );
            await tester.pumpWidget(RepaintBoundary(key: boundary, child: app));
            await tester.pumpAndSettle();
            final router = GoRouter.of(
              tester.element(find.byType(LibraryPage)),
            );
            router.push('/messages/peer');
            await tester.pumpAndSettle();
            final short = find.ancestor(
              of: find.text(_BubbleApi.short),
              matching: find.byType(Card),
            );
            final long = find.ancestor(
              of: find.text(_BubbleApi.long),
              matching: find.byType(Card),
            );
            for (
              var attempt = 0;
              short.evaluate().isEmpty && attempt < 8;
              attempt++
            ) {
              await tester.drag(
                find.byType(CustomScrollView).last,
                const Offset(0, 300),
              );
              await tester.pumpAndSettle();
            }
            final shortBox = tester.getRect(short);
            final longBox = tester.getRect(long);
            final incomingBounds = tester.getRect(
              find.byKey(const ValueKey(1)),
            );
            expect(
              longBox.width,
              lessThanOrEqualTo(incomingBounds.width * .9 + .5),
            );
            expect(shortBox.left, closeTo(incomingBounds.left, .5));
            final outgoing = find.ancestor(
              of: find.text('好，马上开始。'),
              matching: find.byType(Card),
            );
            if (outgoing.evaluate().isNotEmpty) {
              final bounds = tester.getRect(find.byKey(const ValueKey(3)));
              expect(tester.getRect(outgoing).right, closeTo(bounds.right, .5));
            }
            await save(tester, boundary, Directory(capture), 'bubble-$name');
            expect(find.byTooltip(strings.sendPrivateMessage), findsOneWidget);
            await tester.binding.handlePopRoute();
            await tester.pumpAndSettle();
            expect(find.byType(LibraryPage).hitTestable(), findsOneWidget);
            expect(tester.takeException(), isNull);
            expect(shortBox.width, lessThan(longBox.width));
          });
        }
      }
    }
  }

  testWidgets(
    'Desktop announcement opens a six-line preview and records only that entry',
    (tester) async {
      final api = _MessagesApi();
      final router = await _open(tester, const Size(1440, 734), api);
      final trigger = find.byTooltip('系统公告');
      final announcement = find.text(_MessagesApi.longAnnouncement);
      expect(
        (tester.widget<Badge>(_badge('/announcements')).label as Text).data,
        '2',
      );

      await _tap(tester, trigger);
      expect(
        router.routerDelegate.currentConfiguration.last.matchedLocation,
        '/',
      );
      final panelRect = tester.getRect(_panel('/announcements'));
      final anchorRect = tester.getRect(trigger);
      expect(panelRect.width, 392);
      expect(panelRect.height, lessThan(320));
      expect(panelRect.right, closeTo(anchorRect.right, 1));
      expect(panelRect.top, closeTo(anchorRect.bottom + 7, 1));
      expect(tester.widget<Text>(announcement).maxLines, 6);
      expect(find.text(_MessagesApi.earlierAnnouncement), findsNothing);
      expect(
        (tester.widget<Badge>(_badge('/announcements')).label as Text).data,
        '1',
      );

      await _tap(tester, find.text('展开'));
      expect(tester.widget<Text>(announcement).maxLines, isNull);
      expect(
        tester.getSize(_panel('/announcements')).height,
        greaterThan(panelRect.height),
      );
      await tester.sendKeyEvent(LogicalKeyboardKey.escape);
      await tester.pumpAndSettle();
      expect(_panel('/announcements'), findsNothing);
      expect(
        router.routerDelegate.currentConfiguration.last.matchedLocation,
        '/',
      );

      await _tap(tester, trigger);
      await _tap(tester, find.text('查看全部'));
      expect(_panel('/announcements'), findsNothing);
      expect(tester.widget<Text>(announcement).maxLines, 10);
      expect(find.text(_MessagesApi.earlierAnnouncement), findsOneWidget);
      await tester.binding.handlePopRoute();
      await tester.pumpAndSettle();
      expect(
        router.routerDelegate.currentConfiguration.last.matchedLocation,
        '/',
      );
      expect(
        tester.widget<Badge>(_badge('/announcements')).isLabelVisible,
        isFalse,
      );
      expect(api.summaryRequests, 0);

      await tester.pumpWidget(const SizedBox());
      await tester.pumpWidget(fixtures.fixture(accountApi: _MessagesApi()));
      await tester.pumpAndSettle();
      expect(
        tester.widget<Badge>(_badge('/announcements')).isLabelVisible,
        isFalse,
      );
    },
  );

  testWidgets(
    'Desktop inbox preview shares unread status with the conversation',
    (tester) async {
      final api = _MessagesApi();
      final router = await _open(tester, const Size(781, 640), api);
      await _tap(tester, find.byTooltip('私信'));
      expect(find.text('打开私信收件箱'), findsOneWidget);
      expect(tester.getSize(_panel('/messages')).height, lessThan(125));
      expect(
        router.routerDelegate.currentConfiguration.last.matchedLocation,
        '/',
      );
      expect(api.summaryRequests, 0);
      await _tap(tester, find.byTooltip('系统公告'));
      expect(_panel('/messages'), findsNothing);
      expect(_panel('/announcements'), findsOneWidget);
      await _tap(tester, find.byTooltip('系统公告'));

      await fixtures.signInFixture(tester);
      expect(
        (tester.widget<Badge>(_badge('/messages')).label as Text).data,
        '1',
      );

      await _tap(tester, find.byTooltip('私信'));
      expect(find.text('1 条未读'), findsOneWidget);
      final panel = tester.getRect(_panel('/messages'));
      expect(panel.left, greaterThanOrEqualTo(12));
      expect(panel.right, lessThanOrEqualTo(781 - 12));
      await _tap(tester, find.text('查看全部'));
      expect(
        router.routerDelegate.currentConfiguration.last.matchedLocation,
        '/messages',
      );
      await _tap(tester, find.text('另一位玩家'));
      expect(api.readThrough, 1);
      await tester.binding.handlePopRoute();
      await tester.pumpAndSettle();
      await tester.binding.handlePopRoute();
      await tester.pumpAndSettle();
      expect(tester.widget<Badge>(_badge('/messages')).isLabelVisible, isFalse);
      await _tap(tester, find.byTooltip('私信'));
      expect(find.text('0 条未读'), findsOneWidget);
    },
  );

  for (final width in [390.0, 1440.0]) {
    testWidgets(
      'Foreground refresh and earlier messages preserve the viewport | $width',
      (tester) async {
        final api = _HistoryApi();
        await _open(tester, Size(width, 734), api);
        tester.binding.handleAppLifecycleStateChanged(
          AppLifecycleState.resumed,
        );
        addTearDown(() {
          for (final state in [
            AppLifecycleState.inactive,
            AppLifecycleState.hidden,
            AppLifecycleState.paused,
            AppLifecycleState.detached,
          ]) {
            tester.binding.handleAppLifecycleStateChanged(state);
          }
        });
        await tester.pump(const Duration(seconds: 30));
        expect(api.summaryRequests, 0);

        if (width <= 780) {
          await _tap(tester, find.byTooltip('私信'));
        }
        await fixtures.signInFixture(tester);
        if (width > 780) {
          await _tap(tester, find.byTooltip('私信'));
          await _tap(tester, find.text('查看全部'));
        }
        await _tap(tester, find.text('另一位玩家'));

        final history = find.byType(CustomScrollView);
        final position = tester
            .state<ScrollableState>(
              find
                  .descendant(of: history, matching: find.byType(Scrollable))
                  .first,
            )
            .position;
        position.jumpTo(
          width > 780 ? position.minScrollExtent : position.maxScrollExtent,
        );
        await tester.pumpAndSettle();
        final reference = find.text(api.message(11).body);
        final marker = tester.getTopLeft(reference);
        final older = find.text('更早的消息');
        expect(tester.getTopLeft(older).dy, lessThan(marker.dy));
        await _tap(tester, older);
        expect(api.olderRequests, 1);
        expect(tester.getTopLeft(reference).dy, closeTo(marker.dy, 1));
        expect(api.readThrough, 25);

        api.latestId = 44;
        final before = api.latestRequests;
        await tester.pump(const Duration(seconds: 30));
        await tester.pumpAndSettle();
        expect(api.latestRequests, greaterThan(before));
        expect(api.olderRequests, 2);
        expect(tester.getTopLeft(reference).dy, closeTo(marker.dy, 1));
        expect(api.readThrough, 25);
        final foregroundRequests = api.summaryRequests;
        tester.binding.handleAppLifecycleStateChanged(
          AppLifecycleState.inactive,
        );
        await tester.pump(const Duration(seconds: 60));
        expect(api.summaryRequests, foregroundRequests);

        tester.binding.handleAppLifecycleStateChanged(
          AppLifecycleState.resumed,
        );
        await tester.pumpAndSettle();
        expect(api.summaryRequests, greaterThan(foregroundRequests));
        expect(tester.getTopLeft(reference).dy, closeTo(marker.dy, 1));
        position.jumpTo(
          width > 780 ? position.maxScrollExtent : position.minScrollExtent,
        );
        await tester.pumpAndSettle();
        final newest = find.descendant(
          of: history,
          matching: find.text(api.message(44).body),
        );
        expect(newest.hitTestable(), findsOneWidget);
        expect(api.readThrough, 44);
        await tester.pumpWidget(const SizedBox());
      },
    );
  }

  testWidgets('Desktop changes the right pane without rebuilding the inbox', (
    tester,
  ) async {
    final capture = Platform.environment['TOUHOU_PRIVATE_CAPTURE'];
    final boundary = GlobalKey();
    if (capture != null) {
      await tester.runAsync(
        () =>
            loadFonts(Directory(Platform.environment['TOUHOU_CAPTURE_FONTS']!)),
      );
    }
    final api = _MessagesApi();
    final router = await _open(
      tester,
      const Size(1440, 734),
      api,
      boundary: boundary,
    );
    await fixtures.signInFixture(tester);
    await _tap(tester, find.byTooltip('私信'));
    await _tap(tester, find.text('查看全部'));

    final mailbox = find.byKey(const ValueKey('desktop mailbox'));
    final threads = find.byKey(const ValueKey('desktop threads'));
    final pane = find.byKey(const ValueKey('desktop conversation pane'));
    final originalList = tester.element(threads);
    final originalBounds = tester.getRect(pane);
    expect(tester.getSize(threads).width, 250);
    expect(tester.getRect(mailbox).left, 465);
    expect(tester.getRect(mailbox).top, 115);
    expect(find.text('选择一条会话'), findsOneWidget);
    if (capture != null) {
      await save(tester, boundary, Directory(capture), 'desktop-inbox-member');
    }

    final first = find.descendant(
      of: threads,
      matching: find.byKey(const ValueKey('peer')),
    );
    final second = find.descendant(
      of: threads,
      matching: find.byKey(const ValueKey('second')),
    );
    await _tap(tester, first);
    expect(tester.widget<ListTile>(first).selected, isTrue);
    expect(api.readThrough, 1);
    expect(
      find.byWidgetPredicate(
        (w) => w is SelectableText && w.data == api.incoming.body,
      ),
      findsOneWidget,
    );
    expect(
      find.descendant(of: first, matching: find.byType(Badge)),
      findsNothing,
    );
    expect(tester.getRect(pane), originalBounds);
    expect(tester.element(threads), same(originalList));
    if (capture != null) {
      await save(tester, boundary, Directory(capture), 'desktop-inbox-thread');
    }

    await tester.enterText(
      find.descendant(of: pane, matching: find.byType(TextField)),
      '桌面私信测试',
    );
    await tester.pumpAndSettle();
    await _tap(tester, find.byTooltip('发送私信'));
    expect(find.text('发送失败，请重试'), findsOneWidget);
    await _tap(tester, find.byTooltip('发送私信'));
    expect(api.sent, '桌面私信测试');
    expect(tester.element(threads), same(originalList));

    await _tap(tester, second);
    expect(tester.widget<ListTile>(second).selected, isTrue);
    expect(
      router.routerDelegate.currentConfiguration.last.matchedLocation,
      '/messages/second',
    );
    expect(tester.element(threads), same(originalList));

    tester.view.physicalSize = const Size(781, 640);
    await tester.pumpAndSettle();
    expect(tester.getSize(threads).width, 164);
    expect(tester.getSize(pane).width, greaterThan(290));
    expect(tester.takeException(), isNull);
    if (capture != null) {
      await save(
        tester,
        boundary,
        Directory(capture),
        'desktop-narrow-inbox-thread',
      );
    }

    tester.view.physicalSize = const Size(390, 844);
    await tester.pumpAndSettle();
    expect(mailbox, findsNothing);
    expect(find.text('@second'), findsOneWidget);
    await tester.binding.handlePopRoute();
    await tester.pumpAndSettle();
    expect(
      router.routerDelegate.currentConfiguration.last.matchedLocation,
      '/messages',
    );
    await tester.binding.handlePopRoute();
    await tester.pumpAndSettle();
    expect(
      router.routerDelegate.currentConfiguration.last.matchedLocation,
      '/',
    );
  });

  testWidgets(
    'Desktop center keeps its columns and replaces the selected section',
    (tester) async {
      final router = await _open(tester, const Size(1440, 734), _MessagesApi());
      await _tap(tester, find.byTooltip('系统公告'));
      await _tap(tester, find.text('查看全部'));

      final body = find.byKey(const ValueKey('message center body'));
      final pane = find.byKey(const ValueKey('message center content'));
      final header = find.byKey(const ValueKey('masthead navigation'));
      final inbox = find.byKey(const ValueKey(('/messages', 'sidebar')));
      final announcements = find.byKey(
        const ValueKey(('/announcements', 'sidebar')),
      );
      final bodyRect = tester.getRect(body);
      final paneRect = tester.getRect(pane);
      expect(bodyRect.topLeft, const Offset(205, 102));
      expect(bodyRect.width, 1030);
      expect(paneRect.left, 465);
      expect(tester.getSize(announcements), const Size(191, 42));
      expect(
        tester.getRect(inbox).top - tester.getRect(announcements).bottom,
        4,
      );
      expect(tester.getCenter(header).dx, 720);
      expect(
        find.byKey(const ValueKey('announcement timeline divider')),
        findsNWidgets(2),
      );

      await _tap(tester, inbox);
      expect(
        router.routerDelegate.currentConfiguration.last.matchedLocation,
        '/messages',
      );
      expect(find.text('登录后查看私信'), findsOneWidget);
      expect(tester.getRect(body), bodyRect);
      expect(tester.getRect(pane), paneRect);

      final mastheadInbox = find.descendant(
        of: find.byKey(const ValueKey('masthead account')),
        matching: find.byTooltip('私信'),
      );
      await _tap(tester, mastheadInbox);
      await _tap(tester, find.text('查看全部'));
      expect(_panel('/messages'), findsNothing);
      tester.view.physicalSize = const Size(781, 640);
      await tester.pumpAndSettle();
      expect(tester.getRect(body).left, 24);
      expect(tester.getRect(pane).left, 284);

      await _tap(tester, announcements);
      expect(
        router.routerDelegate.currentConfiguration.last.matchedLocation,
        '/announcements',
      );
      await tester.binding.handlePopRoute();
      await tester.pumpAndSettle();
      expect(
        router.routerDelegate.currentConfiguration.last.matchedLocation,
        '/',
      );
      expect(body, findsNothing);
      expect(find.byType(LibraryPage), findsOneWidget);

      router.go('/messages');
      await tester.pumpAndSettle();
      await _tap(tester, announcements);
      expect(
        router.routerDelegate.currentConfiguration.last.matchedLocation,
        '/announcements',
      );
      expect(router.canPop(), isFalse);
      final context = tester.element(body);
      final backLabel = MaterialLocalizations.of(context).backButtonTooltip;
      await _tap(tester, find.byTooltip(backLabel));
      expect(find.byType(LibraryPage), findsOneWidget);
    },
  );

  testWidgets(
    'Failed desktop headline can retry and mobile opens the full page directly',
    (tester) async {
      final api = _MessagesApi()..failFeed = true;
      final router = await _open(tester, const Size(781, 640), api);
      await _tap(tester, find.byTooltip('系统公告'));
      expect(find.text('加载失败，重试'), findsOneWidget);
      api.failFeed = false;
      await _tap(tester, find.text('加载失败，重试'));
      expect(find.text(_MessagesApi.longAnnouncement), findsOneWidget);
      await tester.tapAt(const Offset(3, 100));
      await tester.pumpAndSettle();
      expect(_panel('/announcements'), findsNothing);

      tester.view.physicalSize = const Size(390, 844);
      await tester.pumpAndSettle();
      await _tap(tester, find.byTooltip('系统公告'));
      expect(_panel('/announcements'), findsNothing);
      expect(
        router.routerDelegate.currentConfiguration.last.matchedLocation,
        '/announcements',
      );
      expect(find.text('消息中心'), findsOneWidget);
    },
  );
}
