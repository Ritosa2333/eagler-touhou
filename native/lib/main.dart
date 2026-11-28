import 'dart:async';

import 'package:flutter/foundation.dart' show defaultTargetPlatform;
import 'package:flutter_secure_storage/flutter_secure_storage.dart';
import 'package:material_ui/material_ui.dart';
import 'package:go_router/go_router.dart';
import 'package:lucide_icons_flutter/lucide_icons.dart';
import 'package:prism_ui/prism_ui.dart' as prism;
import 'package:shared_preferences/shared_preferences.dart';

import 'catalog.dart';
import 'cloud_save_page.dart';
import 'desktop_masthead.dart';
import 'desktop_window.dart';
import 'game_detail.dart';
import 'game_launcher.dart';
import 'game_preferences.dart';
import 'game_resources.dart';
import 'game_settings.dart';
import 'game_user_files.dart';
import 'library.dart';
import 'l10n/strings.dart';
import 'site_documents.dart';
import 'launcher_appearance.dart';
import 'launcher_background.dart';
import 'launcher_language.dart';
import 'launcher_navigation.dart';
import 'launcher_prism.dart';
import 'launcher_rendering.dart';
import 'launcher_select.dart';
import 'launcher_wallpaper.dart';
import 'lobby_directory.dart';
import 'lobby_page.dart';
import 'message_highlights.dart';
import 'messages.dart';
import 'account.dart';
import 'account_web_page.dart';
import 'account_api.dart';
import 'profile_identity.dart';

import 'room_page.dart';
import 'room_session.dart';
import 'site_navigation.dart';

Future<void> main(List<String> arguments) async {
  WidgetsFlutterBinding.ensureInitialized();
  if (defaultTargetPlatform == TargetPlatform.windows) {
    await const LauncherShaderWarmUp().execute();
  }
  initializeDesktopWindow();
  final storage = SharedPreferencesAsync();
  final games = await Game.load();
  final favorites = await GameFavorites.load(storage);
  final preferences = await LauncherPreferences.load(storage);
  final history = await GamePlayHistory.load(storage);
  final directory = LobbyDirectory(games, preferences: storage);
  final link = arguments.length == 1 ? Uri.tryParse(arguments.single) : null;
  runApp(
    LauncherApp(
      book: BookState(games, favorites, history: history),
      lobby: BookState(games, favorites),
      preferences: preferences,
      launcher: const NativeGamePlaceholder(),
      initialLocation: link == null
          ? null
          : launcherEntryLocation(link, games, directory.origin) ??
                accountWebLocation(
                  link,
                  bundledLanguage(preferences.language.locale),
                ),
      lobbyDirectory: directory,
      gamePreferences: GamePreferenceStore(storage),
      accountApi: AccountApi(
        sessionStorage: const FlutterSecureStorage(
          mOptions: MacOsOptions(usesDataProtectionKeychain: false),
        ),
      ),
    ),
  );
}

class LauncherApp extends StatefulWidget {
  const LauncherApp({
    super.key,
    required this.book,
    required this.lobby,
    required this.preferences,
    required this.launcher,
    required this.gamePreferences,
    this.initialLocation,
    this.accountApi,
    this.gameResources,
    this.gameUserFiles,
    this.lobbyDirectory,
  });
  final BookState book;
  final BookState lobby;
  final LauncherPreferences preferences;
  final GameLauncher launcher;
  final GamePreferenceStore gamePreferences;
  final String? initialLocation;
  final AccountApi? accountApi;
  final GameResources? gameResources;
  final GameUserFiles? gameUserFiles;
  final LobbyDirectory? lobbyDirectory;

  @override
  State<LauncherApp> createState() => _LauncherAppState();
}

class _LauncherAppState extends State<LauncherApp> {
  late final resources = widget.gameResources ?? GameResources();
  late final gameFiles = widget.gameUserFiles ?? GameUserFiles();
  late final gameLaunch = GameLaunchService(
    widget.launcher,
    widget.gamePreferences,
    resources,
    gameFiles,
    history: widget.book.history,
  );
  late final session = AccountSession(widget.accountApi ?? AccountApi());
  late final highlights = MessageHighlights(session, SharedPreferencesAsync());
  late final lobbyDirectory =
      widget.lobbyDirectory ??
      LobbyDirectory(widget.lobby.games, preferences: SharedPreferencesAsync());
  late final lobbySelection = LobbySelection();

  @override
  void initState() {
    super.initState();
    lobbySelection.addListener(_filterLobbyDirectory);
    unawaited(session.restore());
  }

  void _filterLobbyDirectory() {
    final ids = lobbySelection.value;
    lobbyDirectory.filterGame(ids.length == 1 ? ids.single : null);
  }

  final _shellKey = GlobalKey<ScaffoldState>();
  final _wallpaperKey = GlobalKey();
  final _rootNavigator = GlobalKey<NavigatorState>();
  final _mainNavigator = GlobalKey<NavigatorState>();
  final _workspaceNavigator = GlobalKey<NavigatorState>();
  final _exitGuards = LauncherExitGuards();
  late final router = GoRouter(
    navigatorKey: _rootNavigator,
    initialLocation: widget.initialLocation ?? '/',
    redirect: (_, state) =>
        launcherEntryLocation(
          state.uri,
          widget.lobby.games,
          lobbyDirectory.origin,
        ) ??
        accountWebLocation(
          state.uri,
          bundledLanguage(widget.preferences.language.locale),
        ) ??
        (state.uri.hasScheme || state.uri.hasAuthority ? '/' : null),
    routes: [
      GoRoute(
        path: accountWebPath,
        builder: (_, state) {
          final address = Uri.tryParse(state.uri.queryParameters['url'] ?? '');
          return DesktopWindowPage(
            child: AccountWebPage(
              session: session,
              address: address != null && isAccountSiteUrl(address)
                  ? address
                  : Uri.https('touhouonline.com', '/profile.html'),
            ),
          );
        },
      ),
      GoRoute(
        path: '/room/:id/:code',
        redirect: (_, state) => '/lobby${state.uri}',
      ),
      ShellRoute(
        navigatorKey: _mainNavigator,
        builder: _mainFrame,
        routes: [
          GoRoute(
            path: '/account/cloud-saves',
            onExit: (_, state) => _exitGuards.allowExit(state.pageKey),
            builder: (_, state) {
              final game = state.uri.queryParameters['game'];
              return CloudSavePage(
                exitGuard: _exitGuards.page(state.pageKey),
                session: session,
                games: widget.book.games,
                files: gameFiles,
                preferences: widget.gamePreferences,
                initialGame: widget.book.games.any((entry) => entry.id == game)
                    ? game
                    : null,
              );
            },
          ),
          for (final section in MessageSection.values)
            GoRoute(
              path: section.path,
              builder: (_, state) => _messagePage(state, section),
            ),
          GoRoute(
            path: '/messages/:username',
            builder: (_, state) => _messagePage(
              state,
              MessageSection.inbox,
              peer: state.pathParameters['username'],
            ),
          ),
          ShellRoute(
            navigatorKey: _workspaceNavigator,
            builder: (context, state, child) => _Shell(
              scaffoldKey: _shellKey,
              sidebars: _sidebars(context, state.uri),
              address: state.uri,
              highlights: highlights,
              preferences: widget.preferences,
              child: child,
            ),
            routes: [
              StatefulShellRoute(
                pageBuilder: (context, state, navigation) =>
                    launcherGamePage(state, _RootTabs(navigation: navigation)),
                navigatorContainerBuilder: (context, navigation, children) =>
                    LauncherBranches(
                      key: const ValueKey('launcher branches'),
                      index: navigation.currentIndex,
                      children: children,
                    ),
                branches: [
                  StatefulShellBranch(
                    routes: [
                      GoRoute(
                        path: '/',
                        pageBuilder: (context, state) =>
                            launcherGamePage(state, _library(context)),
                        routes: [_gameRoute()],
                      ),
                    ],
                  ),
                  StatefulShellBranch(
                    routes: [
                      GoRoute(
                        path: '/lobby',
                        pageBuilder: (context, state) => launcherGamePage(
                          state,
                          _primaryPage(
                            context,
                            LobbyPage(
                              book: widget.lobby,
                              directory: lobbyDirectory,
                              selection: lobbySelection,
                              session: session,
                              launcher: gameLaunch,
                            ),
                          ),
                        ),
                        routes: [
                          _gameRoute(),
                          GoRoute(
                            path: 'room/:id/:code',
                            parentNavigatorKey: _rootNavigator,
                            builder: _room,
                          ),
                          GoRoute(
                            path: 'invite-unavailable',
                            parentNavigatorKey: _rootNavigator,
                            builder: (context, _) => _unavailableRoom(context),
                          ),
                        ],
                      ),
                    ],
                  ),
                  StatefulShellBranch(
                    routes: [
                      GoRoute(
                        path: '/home',
                        builder: (context, _) => _primaryPage(
                          context,
                          _HomePage(
                            preferences: widget.preferences,
                            session: session,
                          ),
                        ),
                      ),
                    ],
                  ),
                ],
              ),
            ],
          ),
        ],
      ),
    ],
  );

  Widget _room(BuildContext context, GoRouterState state) {
    final id = state.pathParameters['id'];
    final code = state.pathParameters['code']!;
    final game = widget.lobby.games
        .where((game) => game.multiplayer && game.id == id)
        .firstOrNull;
    if (game == null || !validRoomCode(code)) return _unavailableRoom(context);
    final extra = state.extra;
    final request =
        extra is RoomRequest && extra.game.id == id && extra.code == code
        ? extra
        : RoomRequest(
            game: game,
            code: code,
            watch: state.uri.queryParameters['watch'] == '1',
            autoSeat: state.uri.queryParameters['join'] == '1',
          );
    return DesktopWindowPage(
      child: RoomPage(
        key: ValueKey((id, code)),
        request: request,
        directory: lobbyDirectory,
        account: session,
        launcher: gameLaunch,
        preferences: widget.preferences,
      ),
    );
  }

  Widget _unavailableRoom(BuildContext context) => DesktopWindowPage(
    child: Scaffold(
      appBar: AppBar(
        title: Text(context.strings.roomInviteUnavailable),
        leading: BackButton(
          onPressed: () => closeLauncherPage(context, '/lobby'),
        ),
      ),
      body: Center(
        child: Padding(
          padding: const EdgeInsets.all(20),
          child: Text(context.strings.roomInviteUnavailableHint),
        ),
      ),
    ),
  );

  Widget _primaryPage(BuildContext context, Widget child) {
    final compact = MediaQuery.sizeOf(context).width <= launcherCompactWidth;
    return SafeArea(
      top: false,
      bottom: false,
      left: compact,
      right: compact,
      child: child,
    );
  }

  Widget _library(BuildContext context) {
    final book = widget.book;
    final content = LibraryPage(book: book);
    final compact = MediaQuery.sizeOf(context).width <= launcherCompactWidth;
    return Scaffold(
      backgroundColor: LauncherBackdrop.page(context),
      appBar: compact
          ? AppBar(
              key: const ValueKey('mobile library toolbar'),
              toolbarHeight: 48,
              leadingWidth: 56,
              scrolledUnderElevation: 0,
              surfaceTintColor: Colors.transparent,
              foregroundColor: Theme.of(context).colorScheme.onSurfaceVariant,
              backgroundColor: LauncherBackdrop.tint(
                context,
                BackgroundLayer.navigation,
                Theme.of(context).colorScheme.surfaceContainerLow,
              ),
              shape: Border(
                bottom: BorderSide(
                  color: Theme.of(context).colorScheme.outlineVariant,
                ),
              ),
              leading: ActionIconTheme(
                data: ActionIconThemeData(
                  drawerButtonIconBuilder: (_) =>
                      const Icon(LucideIcons.menu, size: 18),
                ),
                child: DrawerButton(
                  onPressed: () {
                    FocusManager.instance.primaryFocus?.unfocus();
                    _shellKey.currentState?.openDrawer();
                  },
                ),
              ),
              titleSpacing: 4,
              actionsPadding: const EdgeInsetsDirectional.only(end: 6),
              title: ListenableBuilder(
                listenable: book,
                builder: (_, _) => LibrarySearch(book: book, toolbar: true),
              ),
              actions: [MessageActions(highlights: highlights)],
            )
          : null,
      body: _primaryPage(context, content),
    );
  }

  Widget _sidebars(BuildContext context, Uri address) {
    final width = bookSidebarWidth(context);
    final segments = address.pathSegments;
    final selected = segments.length >= 2 && segments.first == 'game'
        ? segments[1]
        : null;
    return SizedBox(
      width: width,
      child: LauncherBranches(
        key: const ValueKey('launcher sidebars'),
        index: _workspaceIndex(address) == 1 ? 1 : 0,
        children: [
          BookSidebar(
            book: widget.book,
            multiplayer: false,
            selectedGame: selected,
            width: width,
          ),
          MultiplayerBookSidebar(
            book: widget.lobby,
            directory: lobbyDirectory,
            selection: lobbySelection,
            width: width,
          ),
        ],
      ),
    );
  }

  MessagesPage _messagePage(
    GoRouterState state,
    MessageSection section, {
    String? peer,
  }) => MessagesPage(
    highlights: highlights,
    section: section,
    peer: peer,
    returnToInbox: state.extra == MessageEntryOrigin.inbox,
  );

  Widget _mainFrame(BuildContext context, GoRouterState state, Widget child) {
    final path = state.uri.path;
    final workspace =
        path == '/' ||
        path == '/home' ||
        path.startsWith('/game/') ||
        path.startsWith('/lobby');
    final message = MessageSection.values.any(
      (section) => path == section.path || path.startsWith('${section.path}/'),
    );
    final compact = MediaQuery.sizeOf(context).width <= launcherCompactWidth;
    return Scaffold(
      backgroundColor: LauncherBackdrop.page(context),
      appBar: compact
          ? null
          : DesktopMasthead(
              preferences: widget.preferences,
              highlights: highlights,
              selectedIndex: workspace ? _workspaceIndex(state.uri) : -1,
              onSelect: (index) {
                final next = const ['/', '/lobby', '/home'][index];
                if (path == next) return;
                FocusManager.instance.primaryFocus?.unfocus();
                context.go(next);
              },
              onLibrary: () {
                if (path == '/') return;
                FocusManager.instance.primaryFocus?.unfocus();
                context.go('/');
              },
              onOpenMessageSection: !message
                  ? null
                  : (next) {
                      if (next.path != path) context.replace(next.path);
                    },
            ),
      body: child,
    );
  }

  GoRoute _gameRoute() => GoRoute(
    path: 'game/:id',
    parentNavigatorKey: _workspaceNavigator,
    pageBuilder: (_, state) =>
        launcherGamePage(state, _game(state, settings: false)),
    routes: [
      GoRoute(
        path: 'settings',
        parentNavigatorKey: _workspaceNavigator,
        pageBuilder: (_, state) =>
            launcherGamePage(state, _game(state, settings: true)),
      ),
    ],
  );

  Widget _game(GoRouterState state, {required bool settings}) {
    final id = state.pathParameters['id'];
    final multiplayer = state.uri.path.startsWith('/lobby/');
    final book = multiplayer ? widget.lobby : widget.book;
    final game = book.games
        .where((game) => game.id == id && (!multiplayer || game.multiplayer))
        .firstOrNull;
    if (game == null) {
      return Builder(
        builder: (context) => SafeArea(
          top: false,
          child: Column(
            crossAxisAlignment: CrossAxisAlignment.stretch,
            children: [
              Align(
                alignment: AlignmentDirectional.centerStart,
                child: BackButton(
                  onPressed: () =>
                      closeLauncherPage(context, multiplayer ? '/lobby' : '/'),
                ),
              ),
              Expanded(
                child: Center(child: Text(context.strings.gameNotFound)),
              ),
            ],
          ),
        ),
      );
    }
    final key = ValueKey((game.id, multiplayer));
    if (settings) {
      return SafeArea(
        top: false,
        child: GameSettingsPage(
          key: key,
          game: game,
          multiplayer: multiplayer,
          store: widget.gamePreferences,
          files: gameFiles,
          launchService: gameLaunch,
          launcherPreferences: widget.preferences,
        ),
      );
    }
    return SafeArea(
      top: false,
      child: GameDetail(
        key: key,
        book: book,
        game: game,
        multiplayer: multiplayer,
        launchService: gameLaunch,
        resources: resources,
        session: session,
      ),
    );
  }

  @override
  void dispose() {
    router.dispose();
    widget.gamePreferences.midi.dispose();
    lobbySelection.removeListener(_filterLobbyDirectory);
    lobbyDirectory.dispose();
    lobbySelection.dispose();
    highlights.dispose();
    session.dispose();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) => ListenableBuilder(
    listenable: widget.preferences,
    builder: (context, _) => MaterialApp.router(
      title: 'TouhouOnline',
      debugShowCheckedModeBanner: false,
      themeMode: widget.preferences.theme,
      theme: _theme(Brightness.light),
      darkTheme: _theme(Brightness.dark),
      routerConfig: router,
      locale: widget.preferences.language.locale,
      supportedLocales: LauncherStrings.supportedLocales,
      localizationsDelegates: const [
        LauncherStrings.delegate,
        ...GlobalMaterialLocalizations.delegates,
      ],
      builder: (context, child) {
        final media = MediaQuery.of(context);
        return MediaQuery(
          data: media.copyWith(
            disableAnimations:
                media.disableAnimations || widget.preferences.lessMotion,
          ),
          child: LauncherPrismScope(
            child: DesktopWindowFrame(
              child: LayoutBuilder(
                builder: (context, constraints) => LauncherBackdrop(
                  background: widget.preferences.background,
                  wallpaperKey: _wallpaperKey,
                  wallpaperSize: constraints.biggest,
                  child: Stack(
                    fit: StackFit.expand,
                    children: [
                      LauncherWallpaper(key: _wallpaperKey),
                      child!,
                    ],
                  ),
                ),
              ),
            ),
          ),
        );
      },
    ),
  );

  ThemeData _theme(Brightness brightness) {
    final colors = launcherMaterialColors(brightness);
    final fonts = launcherFonts(defaultTargetPlatform, [
      widget.preferences.language.locale,
      ...WidgetsBinding.instance.platformDispatcher.locales,
    ]);
    return ThemeData(
      useMaterial3: true,
      colorScheme: colors,
      visualDensity: VisualDensity.standard,
      pageTransitionsTheme: LauncherPageTransitions(
        reduced: widget.preferences.lessMotion,
      ),
      fontFamily: fonts.family,
      fontFamilyFallback: fonts.fallback,
      filledButtonTheme: const FilledButtonThemeData(
        style: LauncherControl.geometry,
      ),
      outlinedButtonTheme: const OutlinedButtonThemeData(
        style: LauncherControl.geometry,
      ),
      textButtonTheme: const TextButtonThemeData(
        style: LauncherControl.geometry,
      ),
      dialogTheme: const DialogThemeData(shape: prism.AppShape.md),
      navigationBarTheme: const NavigationBarThemeData(
        height: launcherBottomBarHeight,
      ),
    );
  }
}

List<NavigationDestination> _destinations(LauncherStrings strings) => [
  NavigationDestination(
    icon: const Icon(LucideIcons.gamepad2, size: 20),
    label: strings.singlePlayer,
  ),
  NavigationDestination(
    icon: const Icon(LucideIcons.usersRound, size: 20),
    label: strings.multiplayer,
  ),
  NavigationDestination(
    icon: const Icon(LucideIcons.house, size: 20),
    label: strings.home,
  ),
];

int _workspaceIndex(Uri address) => address.path.startsWith('/lobby')
    ? 1
    : address.path == '/home'
    ? 2
    : 0;

class _RootTabs extends StatelessWidget {
  const _RootTabs({required this.navigation});

  final StatefulNavigationShell navigation;

  @override
  Widget build(BuildContext context) {
    final compact = MediaQuery.sizeOf(context).width <= launcherCompactWidth;
    return Scaffold(
      backgroundColor: LauncherBackdrop.page(context),
      body: navigation,
      bottomNavigationBar: compact
          ? NavigationBar(
              animationDuration: MediaQuery.disableAnimationsOf(context)
                  ? Duration.zero
                  : null,
              selectedIndex: navigation.currentIndex,
              destinations: _destinations(context.strings),
              onDestinationSelected: (index) {
                if (index == navigation.currentIndex) return;
                FocusManager.instance.primaryFocus?.unfocus();
                navigation.goBranch(index, initialLocation: true);
              },
            )
          : null,
    );
  }
}

class _Shell extends StatefulWidget {
  const _Shell({
    required this.scaffoldKey,
    required this.child,
    required this.sidebars,
    required this.address,
    required this.highlights,
    required this.preferences,
  });
  final GlobalKey<ScaffoldState> scaffoldKey;
  final Widget child;
  final Widget sidebars;
  final Uri address;
  final MessageHighlights highlights;
  final LauncherPreferences preferences;

  @override
  State<_Shell> createState() => _ShellState();
}

class _ShellState extends State<_Shell> {
  bool _sidebarsLoaded = false;

  @override
  void didChangeDependencies() {
    super.didChangeDependencies();
    if (MediaQuery.sizeOf(context).width > launcherCompactWidth) {
      // Clear the open state before Scaffold removes the drawer.
      widget.scaffoldKey.currentState?.closeDrawer();
    }
  }

  @override
  Widget build(BuildContext context) {
    final _Shell(
      :scaffoldKey,
      :child,
      :sidebars,
      :address,
      :highlights,
      :preferences,
    ) = widget;
    final compact = MediaQuery.sizeOf(context).width <= launcherCompactWidth;
    final current = _workspaceIndex(address);
    _sidebarsLoaded |= !compact && current <= 1;
    return Scaffold(
      key: scaffoldKey,
      backgroundColor: LauncherBackdrop.page(context),
      drawer: compact
          ? SiteDrawer(session: highlights.session, preferences: preferences)
          : null,
      drawerEnableOpenDragGesture: compact && address.path == '/',
      body: SafeArea(
        bottom: !compact,
        child: ClipRect(
          child: Row(
            crossAxisAlignment: CrossAxisAlignment.stretch,
            children: [
              Offstage(
                offstage: compact || current > 1,
                child: ExcludeFocus(
                  excluding: compact || current > 1,
                  child: TickerMode(
                    enabled: !compact && current <= 1,
                    child: _sidebarsLoaded ? sidebars : const SizedBox.shrink(),
                  ),
                ),
              ),
              Offstage(
                offstage: compact || current > 1,
                child: const VerticalDivider(width: 1),
              ),
              Expanded(
                key: const ValueKey('launcher-branch-content'),
                child: child,
              ),
            ],
          ),
        ),
      ),
    );
  }
}

class _HomePage extends StatelessWidget {
  const _HomePage({required this.preferences, required this.session});
  final AccountSession session;
  final LauncherPreferences preferences;

  @override
  Widget build(BuildContext context) => ListenableBuilder(
    listenable: Listenable.merge([preferences, session]),
    builder: (context, _) {
      final member = session.identity;
      final strings = context.strings;
      final theme = Theme.of(context);
      const detailPadding = EdgeInsetsDirectional.fromSTEB(48, 0, 16, 0);
      return ListView(
        key: const PageStorageKey('home settings'),
        padding: const EdgeInsets.all(16),
        children: [
          if (MediaQuery.sizeOf(context).width <= launcherCompactWidth)
            Padding(
              padding: const EdgeInsets.only(bottom: 20),
              child: Text(
                strings.home,
                style: theme.textTheme.titleLarge?.copyWith(
                  fontWeight: FontWeight.w700,
                ),
              ),
            ),
          Padding(
            padding: const EdgeInsets.symmetric(horizontal: 12),
            child: AccountSummary(session: session),
          ),
          if (member != null)
            AccountRelationships(
              key: ValueKey(member.username),
              session: session,
              username: member.username,
            ),
          const SizedBox(height: 24),
          Card(
            margin: EdgeInsets.zero,
            elevation: 0,
            shape: RoundedRectangleBorder(
              borderRadius: BorderRadius.circular(12),
              side: BorderSide(color: theme.colorScheme.outlineVariant),
            ),
            color: LauncherBackdrop.tint(
              context,
              BackgroundLayer.panel,
              theme.colorScheme.surfaceContainerLow,
            ),
            clipBehavior: Clip.antiAlias,
            child: ListTileTheme.merge(
              minLeadingWidth: 20,
              horizontalTitleGap: 12,
              titleTextStyle: theme.textTheme.bodyMedium?.copyWith(
                color: theme.colorScheme.onSurfaceVariant,
              ),
              child: Column(
                children: [
                  ExpansionTile(
                    key: const PageStorageKey('launcher-settings'),
                    splashColor: Colors.transparent,
                    shape: const Border(),
                    leading: const Icon(LucideIcons.settings2, size: 18),
                    title: Text(
                      strings.launcherSettings,
                      style: theme.textTheme.titleSmall,
                    ),
                    children: [
                      LauncherLanguageRow(
                        preferences: preferences,
                        contentPadding: detailPadding,
                      ),
                      const Divider(height: 1, indent: 48, endIndent: 16),
                      SwitchListTile(
                        contentPadding: detailPadding,
                        title: Text(strings.lessMotion),
                        value: preferences.lessMotion,
                        onChanged: preferences.setLessMotion,
                      ),
                    ],
                  ),
                  const Divider(height: 1),
                  ExpansionTile(
                    key: const PageStorageKey('theme-settings'),
                    splashColor: Colors.transparent,
                    shape: const Border(),
                    leading: const Icon(LucideIcons.palette, size: 18),
                    title: Text(
                      strings.themeSettings,
                      style: theme.textTheme.titleSmall,
                    ),
                    children: [
                      Padding(
                        padding: detailPadding,
                        child: LauncherThemeBaseRow(preferences: preferences),
                      ),
                      const Divider(height: 1, indent: 48, endIndent: 16),
                      ListTile(
                        contentPadding: detailPadding,
                        title: Text(strings.backgroundSettings),
                        trailing: const Icon(Icons.chevron_right),
                        onTap: () => showBackgroundSettings(
                          context,
                          preferences,
                          includeTheme: false,
                        ),
                      ),
                    ],
                  ),
                  if (member != null) ...[
                    const Divider(height: 1),
                    ExpansionTile(
                      key: const PageStorageKey('account-settings'),
                      splashColor: Colors.transparent,
                      shape: const Border(),
                      leading: const Icon(LucideIcons.shieldCheck, size: 18),
                      title: Text(
                        strings.accountSettings,
                        style: theme.textTheme.titleSmall,
                      ),
                      children: [
                        ListTile(
                          contentPadding: detailPadding,
                          title: Text(strings.personalProfile),
                          trailing: const Icon(Icons.chevron_right),
                          onTap: () => openSiteLink(
                            context,
                            profileEditPath(member.username),
                          ),
                        ),
                        const Divider(height: 1, indent: 48, endIndent: 16),
                        ListTile(
                          contentPadding: detailPadding,
                          title: Text(strings.accountSecurity),
                          trailing: const Icon(Icons.chevron_right),
                          onTap: () => openSiteLink(
                            context,
                            '${profilePath(member.username)}#security',
                          ),
                        ),
                        const Divider(height: 1, indent: 48, endIndent: 16),
                        ListTile(
                          contentPadding: detailPadding,
                          title: Text(strings.cloudSaves),
                          trailing: const Icon(Icons.chevron_right),
                          onTap: () => context.push('/account/cloud-saves'),
                        ),
                      ],
                    ),
                  ],
                ],
              ),
            ),
          ),
          if (member != null)
            Align(
              alignment: Alignment.centerRight,
              child: TextButton(
                onPressed: () => signOutAccount(context, session),
                child: Text(strings.signOut),
              ),
            ),
        ],
      );
    },
  );
}
