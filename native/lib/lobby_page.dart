import 'dart:async';

import 'package:flutter/services.dart';
import 'package:go_router/go_router.dart';
import 'package:material_ui/material_ui.dart';

import 'account.dart';
import 'catalog.dart';
import 'game_launcher.dart';
import 'library.dart';
import 'launcher_background.dart';
import 'launcher_select.dart';
import 'l10n/strings.dart';
import 'lobby_directory.dart';
import 'lobby_room_card.dart';
import 'network_check_dialog.dart';
import 'replay_manager.dart';
import 'room_options_fields.dart';
import 'room_session.dart';
import 'site_documents.dart';
import 'th09_rating_feed.dart';
import 'th09_ratings.dart';

class LobbyPage extends StatefulWidget {
  const LobbyPage({
    super.key,
    required this.book,
    required this.directory,
    required this.selection,
    required this.session,
    required this.launcher,
  });

  final BookState book;
  final LobbyDirectory directory;
  final LobbySelection selection;
  final AccountSession session;
  final GameLaunchService launcher;

  @override
  State<LobbyPage> createState() => _LobbyPageState();
}

class _LobbyPageState extends State<LobbyPage> with WidgetsBindingObserver {
  LobbyRoomFilter _filter = LobbyRoomFilter.all;
  final _clock = ValueNotifier(DateTime.now());
  Timer? _timer;
  bool _visible = false;
  bool _foreground = true;
  final _rows = ScrollController();

  LobbyDirectory get _directory => widget.directory;
  BookState get _book => widget.book;
  LobbySelection get _selection => widget.selection;

  List<Game> get _available => [
    for (final category in BookCategory.multiplayerGroups.skip(1))
      for (final game in _book.filter(category, multiplayer: true))
        if (_directory.availableGameIds.value.contains(game.id)) game,
  ];

  @override
  void initState() {
    super.initState();
    final binding = WidgetsBinding.instance;
    binding.addObserver(this);
    _foreground =
        binding.lifecycleState == null ||
        binding.lifecycleState == AppLifecycleState.resumed ||
        binding.lifecycleState == AppLifecycleState.inactive;
    WidgetsBinding.instance.addPostFrameCallback((_) {
      if (mounted) _directory.start();
    });
  }

  @override
  void didChangeDependencies() {
    super.didChangeDependencies();
    _visible =
        TickerMode.valuesOf(context).enabled &&
        ModalRoute.isCurrentOf(context) != false;
    _syncClock();
  }

  @override
  void didChangeAppLifecycleState(AppLifecycleState state) {
    _foreground =
        state == AppLifecycleState.resumed ||
        state == AppLifecycleState.inactive;
    _syncClock();
  }

  void _syncClock() {
    if (!_visible || !_foreground) {
      _timer?.cancel();
      _timer = null;
    } else if (_timer == null) {
      _clock.value = DateTime.now();
      _timer = Timer.periodic(
        const Duration(seconds: 1),
        (_) => _clock.value = DateTime.now(),
      );
    }
  }

  void _orderTh09Rooms(
    List<LobbyRoom> rooms,
    Map<String, Th09LobbyRating> players,
    Th09LobbyRating own,
  ) {
    final waiting = <({int index, LobbyRoom room, int? distance})>[];
    for (final (index, room) in rooms.indexed) {
      if (room.game.id != 'th09' || !room.canJoin || room.occupied != 1) {
        continue;
      }
      final host = room.seats.whereType<LobbySeat>().firstOrNull;
      final rating = host?.online == true
          ? players[host?.account?.toLowerCase()]
          : null;
      waiting.add((
        index: index,
        room: room,
        distance: rating?.group == own.group
            ? (rating!.score - own.score).abs()
            : null,
      ));
    }
    final order = waiting.map((row) => row.index).toList();
    waiting.sort((a, b) {
      final difference = a.distance == null
          ? (b.distance == null ? 0 : 1)
          : (b.distance == null ? -1 : a.distance!.compareTo(b.distance!));
      return difference == 0 ? a.index.compareTo(b.index) : difference;
    });
    for (final (index, slot) in order.indexed) {
      rooms[slot] = waiting[index].room;
    }
  }

  @override
  void dispose() {
    WidgetsBinding.instance.removeObserver(this);
    _timer?.cancel();
    _clock.dispose();
    _rows.dispose();
    super.dispose();
  }

  void _notice(String text) =>
      ScaffoldMessenger.of(context).showSnackBar(SnackBar(content: Text(text)));

  void _openGames() => showModalBottomSheet<void>(
    context: context,
    sheetAnimationStyle: LauncherControl.overlayMotion(context),
    isScrollControlled: true,
    showDragHandle: true,
    useSafeArea: true,
    builder: (context) => FractionallySizedBox(
      heightFactor: .82,
      child: MultiplayerBookSidebar(
        book: _book,
        selection: _selection,
        directory: _directory,
        width: double.infinity,
      ),
    ),
  );

  void _checkNetwork() => showDialog<void>(
    context: context,
    animationStyle: LauncherControl.overlayMotion(context),
    builder: (_) =>
        NetworkCheckDialog(createDiagnostics: _directory.diagnostics),
  );

  Future<void> _openRoom({required bool create, LobbyRoom? room}) async {
    if (_directory.occupiedRoomCode != null) return;
    final games = _available;
    if (games.isEmpty) return;
    final chosen = room == null
        ? await showDialog<RoomRequest>(
            context: context,
            animationStyle: LauncherControl.overlayMotion(context),
            builder: (_) => _RoomEntryDialog(
              games: games,
              game: games.where(_selection.includes).firstOrNull ?? games.first,
              create: create,
              directCode: _directory.globalRoomCodes,
              origin: _directory.origin,
            ),
          )
        : RoomRequest(game: room.game, code: room.code, watch: room.watchEntry);
    if (!mounted ||
        chosen == null ||
        _directory.occupiedRoomCode != null ||
        _directory.connection != LobbyConnection.live) {
      return;
    }
    final request = RoomRequest(
      game: chosen.create == null && _directory.globalRoomCodes
          ? _directory.rooms
                    .where((room) => room.code == chosen.code)
                    .firstOrNull
                    ?.game ??
                chosen.game
          : chosen.game,
      code: chosen.create == null
          ? chosen.code
          : RoomRequest.newCode(_directory.rooms.map((room) => room.code)),
      create: chosen.create,
      watch: chosen.watch,
      autoSeat: chosen.autoSeat,
    );
    context.push(request.location, extra: request);
  }

  Future<void> _releaseMembership() async {
    final result = await _directory.releaseMembership();
    if (!mounted) return;
    final strings = context.strings;
    _notice(switch (result) {
      LobbyReleaseResult.released => strings.lobbyMembershipReleased,
      LobbyReleaseResult.changed => strings.lobbyMembershipChanged,
      LobbyReleaseResult.unconfirmed => strings.lobbyMembershipUnconfirmed,
    });
  }

  Widget _panel({Key? key, required Widget child}) => Card.outlined(
    key: key,
    margin: EdgeInsets.zero,
    shape: RoundedRectangleBorder(
      borderRadius: BorderRadius.circular(8),
      side: BorderSide(color: Theme.of(context).colorScheme.outlineVariant),
    ),
    color: LauncherBackdrop.tint(
      context,
      BackgroundLayer.panel,
      Theme.of(context).colorScheme.surfaceContainerLow,
    ),
    clipBehavior: Clip.antiAlias,
    child: child,
  );

  Widget _membershipNotice(String code) {
    final strings = context.strings;
    final supportsRelease = _directory.supportsMembershipRelease;
    return _panel(
      key: const ValueKey('lobby-membership'),
      child: Padding(
        padding: const EdgeInsets.all(12),
        child: LayoutBuilder(
          builder: (context, bounds) {
            final message = Semantics(
              liveRegion: true,
              child: Column(
                crossAxisAlignment: CrossAxisAlignment.start,
                spacing: 4,
                children: [
                  Text(
                    strings.lobbyMembershipTitle(code),
                    style: Theme.of(context).textTheme.titleSmall,
                  ),
                  Text(
                    supportsRelease
                        ? strings.lobbyMembershipHint
                        : strings.lobbyMembershipUnsupported,
                    style: Theme.of(context).textTheme.bodySmall,
                  ),
                ],
              ),
            );
            if (!supportsRelease) return message;
            final action = OutlinedButton(
              style: LauncherControl.style(context),
              onPressed: _directory.canReleaseMembership
                  ? _releaseMembership
                  : null,
              child: IndexedStack(
                alignment: Alignment.center,
                index: _directory.releasingMembership ? 1 : 0,
                children: [
                  Text(
                    strings.lobbyMembershipRelease,
                    textAlign: TextAlign.center,
                  ),
                  Text(
                    strings.lobbyMembershipReleasing,
                    textAlign: TextAlign.center,
                  ),
                ],
              ),
            );
            if (bounds.maxWidth < 480) {
              return Column(
                crossAxisAlignment: CrossAxisAlignment.stretch,
                spacing: 8,
                children: [
                  message,
                  Align(
                    alignment: AlignmentDirectional.centerEnd,
                    child: action,
                  ),
                ],
              );
            }
            return Row(
              spacing: 16,
              children: [
                Expanded(child: message),
                action,
              ],
            );
          },
        ),
      ),
    );
  }

  Future<void> _replays() async {
    final choices = _available
        .where((game) => game.files?.multiplayer != null)
        .toList();
    if (choices.isEmpty) {
      _notice(context.strings.fileIssueReplayUnsupported);
      return;
    }
    final selected = _selection.value;
    final game = selected.length == 1
        ? choices.where((game) => selected.contains(game.id)).firstOrNull
        : null;
    final chosen =
        game ??
        await showDialog<Game>(
          context: context,
          animationStyle: LauncherControl.overlayMotion(context),
          builder: (dialog) => SimpleDialog(
            title: Text(dialog.strings.replayChooseGame),
            children: [
              for (final game in choices)
                ListTile(
                  leading: GameArtwork(game: game, size: 32),
                  title: Text(game.title),
                  onTap: () => Navigator.of(dialog).pop(game),
                ),
            ],
          ),
        );
    if (!mounted || chosen == null) return;
    try {
      final service = widget.launcher;
      final options = await service.preferences.load(chosen, multiplayer: true);
      final workspace = service.files.workspace(
        chosen,
        multiplayer: true,
        language: options.values['language'] as String,
      );
      if (!mounted || workspace == null) return;
      await showDialog<void>(
        context: context,
        animationStyle: LauncherControl.overlayMotion(context),
        barrierDismissible: false,
        builder: (dialog) => ReplayManagerDialog(
          workspace: workspace,
          play: (context, file) => service.start(
            chosen,
            multiplayer: true,
            replayFile: file,
            isActive: () => context.mounted,
          ),
        ),
      );
    } on Exception {
      if (mounted) _notice(context.strings.replayFileFailed);
    }
  }

  @override
  Widget build(BuildContext context) => ListenableBuilder(
    listenable: Listenable.merge([_book, _directory, _selection]),
    builder: (context, _) {
      final candidates = _directory.rooms.where(
        (room) => _selection.includes(room.game),
      );
      final rooms = [
        ...candidates.where(
          (room) => _book.isFavorite(room.game) && _filter.includes(room),
        ),
        ...candidates.where(
          (room) => !_book.isFavorite(room.game) && _filter.includes(room),
        ),
      ];
      final th09Rooms = rooms.where((room) => room.game.id == 'th09');
      return Th09RatingFeed(
        session: widget.session,
        roster: th09Rooms.isEmpty
            ? null
            : [
                for (final room in th09Rooms)
                  for (final seat in room.seats)
                    if (seat?.account case final String username) username,
              ],
        builder: (context, ratings, ownRating) =>
            _buildRooms(context, candidates, rooms, ratings, ownRating),
      );
    },
  );

  Widget _buildRooms(
    BuildContext context,
    Iterable<LobbyRoom> candidates,
    List<LobbyRoom> source,
    Th09LobbyRatings ratings,
    Th09LobbyRating? ownRating,
  ) {
    final rooms = [...source];
    if (ownRating != null) _orderTh09Rooms(rooms, ratings.players, ownRating);
    final indices = {for (var i = 0; i < rooms.length; i++) rooms[i].id: i};
    final live = _directory.connection == LobbyConnection.live;
    final showConnectionNotice =
        !live && _directory.connection != LobbyConnection.loading;
    final occupied = _directory.occupiedRoomCode;
    final canEnter = live && !_directory.refreshing && occupied == null;
    final countsFullSelection = _directory.publishedProduct.isEmpty
        ? _selection.value.isEmpty
        : _selection.value.length == 1 &&
              candidates.firstOrNull?.game.multiplayerProduct ==
                  _directory.publishedProduct;
    final mobile = MediaQuery.sizeOf(context).width <= 780;
    return LayoutBuilder(
      builder: (context, box) {
        final compact = mobile || box.maxWidth < 720;
        return Padding(
          padding: const EdgeInsets.all(16),
          child: Column(
            key: const ValueKey('lobby-body'),
            crossAxisAlignment: CrossAxisAlignment.stretch,
            spacing: 8,
            children: [
              ConstrainedBox(
                // Leave a room action and count visible when controls grow.
                constraints: BoxConstraints(
                  maxHeight: (box.maxHeight - 32 - 8 - 96).clamp(
                    0.0,
                    double.infinity,
                  ),
                ),
                child: SingleChildScrollView(
                  key: const PageStorageKey('lobby-controls'),
                  primary: false,
                  child: Column(
                    crossAxisAlignment: CrossAxisAlignment.stretch,
                    spacing: 8,
                    children: [
                      Wrap(
                        spacing: 16,
                        runSpacing: 16,
                        children: [
                          _Link(
                            label: context.strings.multiplayerGuide,
                            open: () =>
                                openSiteLink(context, multiplayerGuidePath),
                          ),
                          _Link(
                            label: context.strings.checkNetwork,
                            open: _checkNetwork,
                          ),
                          _Link(
                            label: context.strings.rankings,
                            open: () => openSiteLink(context, '/ratings.html'),
                          ),
                        ],
                      ),
                      if (occupied != null) _membershipNotice(occupied),
                      if (mobile) _mobileGames(),
                      Row(
                        spacing: 6,
                        children: [
                          MenuAnchor(
                            animated: !MediaQuery.disableAnimationsOf(context),
                            menuChildren: [
                              for (final filter in LobbyRoomFilter.values)
                                MenuItemButton(
                                  leadingIcon: Icon(
                                    filter == _filter
                                        ? Icons.radio_button_checked
                                        : Icons.radio_button_unchecked,
                                  ),
                                  trailingIcon: Text(
                                    '${candidates.where(filter.includes).length}',
                                  ),
                                  onPressed: () =>
                                      setState(() => _filter = filter),
                                  child: Text(filter.label(context.strings)),
                                ),
                            ],
                            builder: (_, controller, _) => _Tool(
                              icon: Icons.filter_list,
                              label: context.strings.roomFilters,
                              compact: compact,
                              selected: _filter != LobbyRoomFilter.all,
                              onPressed: controller.open,
                            ),
                          ),
                          const Spacer(),
                          _Tool(
                            icon: Icons.add,
                            label: context.strings.createRoom,
                            compact: compact,
                            filled: true,
                            onPressed: canEnter
                                ? () => _openRoom(create: true)
                                : null,
                          ),
                          _Tool(
                            icon: Icons.key_outlined,
                            label: context.strings.roomByCode,
                            compact: compact,
                            onPressed: canEnter
                                ? () => _openRoom(create: false)
                                : null,
                          ),
                          _Tool(
                            icon: Icons.movie_outlined,
                            label: context.strings.watchReplay,
                            compact: true,
                            onPressed: _replays,
                          ),
                          _Tool(
                            icon: Icons.refresh,
                            label: context.strings.refreshLobby,
                            compact: true,
                            onPressed: _directory.refresh,
                          ),
                        ],
                      ),
                    ],
                  ),
                ),
              ),
              Expanded(
                child: _panel(
                  child: Column(
                    children: [
                      SizedBox(
                        height: 2,
                        child:
                            _directory.refreshing ||
                                _directory.connection == LobbyConnection.loading
                            ? const LinearProgressIndicator(minHeight: 2)
                            : null,
                      ),
                      Expanded(
                        child: rooms.isNotEmpty
                            ? ListView.builder(
                                key: const PageStorageKey('lobby-room-list'),
                                controller: _rows,
                                itemCount:
                                    rooms.length +
                                    (showConnectionNotice ? 1 : 0),
                                findChildIndexCallback: (key) {
                                  if (key is ValueKey<LobbyConnection>) {
                                    return showConnectionNotice ? 0 : null;
                                  }
                                  final index = key is ValueKey<String>
                                      ? indices[key.value]
                                      : null;
                                  return index == null
                                      ? null
                                      : index + (showConnectionNotice ? 1 : 0);
                                },
                                itemBuilder: (_, index) {
                                  if (showConnectionNotice && index == 0) {
                                    return DecoratedBox(
                                      key: ValueKey(_directory.connection),
                                      decoration: BoxDecoration(
                                        border: Border(
                                          bottom: BorderSide(
                                            color: Theme.of(context)
                                                .colorScheme
                                                .outlineVariant,
                                          ),
                                        ),
                                      ),
                                      child: _connectionNotice(true),
                                    );
                                  }
                                  final roomIndex =
                                      index - (showConnectionNotice ? 1 : 0);
                                  final room = rooms[roomIndex];
                                  return DecoratedBox(
                                    key: ValueKey(room.id),
                                    decoration: BoxDecoration(
                                      border: roomIndex == rooms.length - 1
                                          ? null
                                          : Border(
                                              bottom: BorderSide(
                                                color: Theme.of(context)
                                                    .colorScheme
                                                    .outlineVariant,
                                              ),
                                            ),
                                    ),
                                    child: LobbyRoomCard(
                                      room: room,
                                      session: widget.session,
                                      clock: _clock,
                                      ratings: ratings,
                                      ownRating: ownRating,
                                      onEnter:
                                          canEnter &&
                                              (room.canJoin || room.watchEntry)
                                          ? () => _openRoom(
                                              create: false,
                                              room: room,
                                            )
                                          : null,
                                    ),
                                  );
                                },
                              )
                            : showConnectionNotice
                            ? Center(
                                child: SingleChildScrollView(
                                  child: _connectionNotice(false),
                                ),
                              )
                            : live && !_directory.refreshing
                            ? _empty()
                            : const SizedBox.shrink(),
                      ),
                      if (rooms.isNotEmpty)
                        Align(
                          alignment: AlignmentDirectional.centerEnd,
                          child: Padding(
                            padding: const EdgeInsets.fromLTRB(8, 4, 12, 8),
                            child: Text(
                              !_directory.refreshing &&
                                      countsFullSelection &&
                                      _filter == LobbyRoomFilter.all &&
                                      _directory.total > rooms.length
                                  ? context.strings.roomCountPartial(
                                      rooms.length,
                                      _directory.total,
                                    )
                                  : context.strings.roomCount(rooms.length),
                              style: Theme.of(context).textTheme.labelSmall,
                            ),
                          ),
                        ),
                    ],
                  ),
                ),
              ),
            ],
          ),
        );
      },
    );
  }

  Widget _mobileGames() => Row(
    spacing: 6,
    children: [
      _Tool(
        icon: Icons.format_list_bulleted,
        label: context.strings.lobbyGames,
        compact: true,
        onPressed: _openGames,
      ),
      _Tool(
        icon: Icons.grid_view,
        label: context.strings.multiplayerAll,
        selected: _selection.value.isEmpty,
        compact: true,
        onPressed: _selection.clear,
      ),
      Expanded(
        child: SizedBox(
          height: 48,
          child: ListView.separated(
            key: const PageStorageKey('lobby covers'),
            scrollDirection: Axis.horizontal,
            itemCount: _available.length,
            separatorBuilder: (_, _) => const SizedBox(width: 6),
            itemBuilder: (_, index) {
              final game = _available[index];
              final selected = _selection.value.contains(game.id);
              return SizedBox.square(
                dimension: 48,
                child: Stack(
                  children: [
                    Positioned.fill(
                      child: Semantics(
                        selected: selected,
                        child: Tooltip(
                          message: context.strings.onlyGame(game.title),
                          child: InkWell(
                            borderRadius: BorderRadius.circular(8),
                            onTap: () => _selection.showOnly(game),
                            child: Center(
                              child: GameArtwork(game: game, size: 34),
                            ),
                          ),
                        ),
                      ),
                    ),
                    PositionedDirectional(
                      start: -4,
                      top: -4,
                      child: GameSelectionMarker(
                        game: game,
                        selected: selected,
                        onPressed: () => _selection.toggle(game),
                      ),
                    ),
                    if (game.shortTitle.isNotEmpty)
                      PositionedDirectional(
                        end: 0,
                        bottom: 0,
                        child: IgnorePointer(
                          child: DecoratedBox(
                            decoration: BoxDecoration(
                              color: Theme.of(context).colorScheme.surface,
                              borderRadius: BorderRadius.circular(3),
                            ),
                            child: Padding(
                              padding: const EdgeInsets.symmetric(
                                horizontal: 3,
                              ),
                              child: Text(
                                game.shortTitle,
                                style: Theme.of(context).textTheme.labelSmall,
                              ),
                            ),
                          ),
                        ),
                      ),
                  ],
                ),
              );
            },
          ),
        ),
      ),
    ],
  );

  Widget _empty() {
    final filtered =
        _filter != LobbyRoomFilter.all || _selection.value.isNotEmpty;
    return Center(
      child: SingleChildScrollView(
        child: Padding(
          padding: const EdgeInsets.all(16),
          child: Column(
            mainAxisSize: MainAxisSize.min,
            spacing: 8,
            children: [
              const Icon(Icons.groups_outlined, size: 32),
              Text(
                filtered
                    ? context.strings.noMatchingRooms
                    : context.strings.noRooms,
                textAlign: TextAlign.center,
              ),
              if (filtered || _directory.occupiedRoomCode == null)
                OutlinedButton(
                  onPressed: () {
                    if (filtered) {
                      setState(() => _filter = LobbyRoomFilter.all);
                      _selection.clear();
                    } else {
                      _openRoom(create: true);
                    }
                  },
                  child: Text(
                    filtered
                        ? context.strings.clearRoomFilters
                        : context.strings.createRoom,
                  ),
                ),
            ],
          ),
        ),
      ),
    );
  }

  Widget _connectionNotice(bool hasRows) {
    final strings = context.strings;
    final title = hasRows ? strings.lobbyDisconnected : strings.lobbyOffline;
    final description = _directory.connection == LobbyConnection.unconfigured
        ? strings.lobbyNotConfigured
        : _directory.failure?.label(strings) ?? strings.lobbyOfflineHint;
    const warning = Icon(Icons.warning_amber_outlined);
    final retry = IconButton(
      tooltip: strings.retryLobby,
      icon: const Icon(Icons.refresh),
      onPressed: _directory.refresh,
    );
    final theme = Theme.of(context);
    return Semantics(
      liveRegion: true,
      child: LayoutBuilder(
        builder: (context, constraints) {
          if (constraints.maxWidth < 480 &&
              MediaQuery.textScalerOf(context).scale(16) > 21) {
            return Padding(
              padding: const EdgeInsets.fromLTRB(16, 0, 16, 16),
              child: Column(
                crossAxisAlignment: CrossAxisAlignment.stretch,
                spacing: 8,
                children: [
                  Row(
                    mainAxisAlignment: MainAxisAlignment.spaceBetween,
                    children: [warning, retry],
                  ),
                  Text(title, style: theme.textTheme.bodyLarge),
                  Text(
                    description,
                    style: theme.textTheme.bodyMedium?.copyWith(
                      color: theme.colorScheme.onSurfaceVariant,
                    ),
                  ),
                ],
              ),
            );
          }
          return ListTile(
            leading: warning,
            title: Text(title),
            subtitle: Text(description),
            trailing: retry,
          );
        },
      ),
    );
  }
}

class MultiplayerBookSidebar extends StatelessWidget {
  const MultiplayerBookSidebar({
    super.key,
    required this.book,
    required this.selection,
    required this.directory,
    this.width,
  });

  final BookState book;
  final LobbySelection selection;
  final LobbyDirectory directory;
  final double? width;

  @override
  Widget build(BuildContext context) => ListenableBuilder(
    listenable: Listenable.merge([selection, directory.availableGameIds]),
    builder: (_, _) => BookSidebar(
      book: book,
      multiplayer: true,
      selection: selection,
      available: directory.availableGameIds.value,
      width: width,
    ),
  );
}

class _Tool extends StatelessWidget {
  const _Tool({
    required this.icon,
    required this.label,
    required this.compact,
    required this.onPressed,
    this.filled = false,
    this.selected = false,
  });

  final IconData icon;
  final String label;
  final bool compact;
  final VoidCallback? onPressed;
  final bool filled;
  final bool selected;

  @override
  Widget build(BuildContext context) {
    final colors = Theme.of(context).colorScheme;
    if (compact) {
      final style = IconButton.styleFrom(
        fixedSize: const Size.square(44),
        backgroundColor: selected ? colors.surfaceContainerHighest : null,
        shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(8)),
      );
      return filled
          ? IconButton.filled(
              tooltip: label,
              onPressed: onPressed,
              style: style,
              icon: Icon(icon),
            )
          : IconButton.outlined(
              tooltip: label,
              onPressed: onPressed,
              style: style,
              icon: Icon(icon),
            );
    }
    return filled
        ? FilledButton.icon(
            onPressed: onPressed,
            icon: Icon(icon),
            label: Text(label),
          )
        : OutlinedButton.icon(
            onPressed: onPressed,
            icon: Icon(icon),
            label: Text(label),
            style: selected
                ? OutlinedButton.styleFrom(
                    backgroundColor: colors.surfaceContainerHighest,
                  )
                : null,
          );
  }
}

class _Link extends StatelessWidget {
  const _Link({required this.label, required this.open});
  final String label;
  final VoidCallback open;

  @override
  Widget build(BuildContext context) => TextButton(
    onPressed: open,
    style: TextButton.styleFrom(
      padding: const EdgeInsets.symmetric(horizontal: 4),
      foregroundColor: Theme.of(context).colorScheme.onSurfaceVariant,
      textStyle: LauncherControl.label(context),
    ),
    child: Row(
      mainAxisSize: MainAxisSize.min,
      spacing: 2,
      children: [
        Flexible(child: Text(label)),
        const Icon(Icons.chevron_right, size: 16),
      ],
    ),
  );
}

class _RoomEntryDialog extends StatefulWidget {
  const _RoomEntryDialog({
    required this.games,
    required this.game,
    required this.create,
    required this.directCode,
    required this.origin,
  });

  final List<Game> games;
  final Game game;
  final bool create;
  final bool directCode;
  final Uri origin;

  @override
  State<_RoomEntryDialog> createState() => _RoomEntryDialogState();
}

class _RoomEntryDialogState extends State<_RoomEntryDialog> {
  late Game _game = widget.game;
  final _entry = TextEditingController();
  RoomRequest? _invite;
  late RoomOptions _options = initialRoomOptions(_game);

  @override
  void dispose() {
    _entry.dispose();
    super.dispose();
  }

  Future<void> _pasteRoom() async {
    final value = (await Clipboard.getData(Clipboard.kTextPlain))?.text?.trim();
    if (!mounted || value == null || value.isEmpty) return;
    _updateEntry(value, fromClipboard: true);
  }

  void _updateEntry(String value, {bool fromClipboard = false}) {
    final invite = RoomRequest.fromInviteLink(
      value.trim(),
      widget.origin,
      widget.games,
    );
    if (fromClipboard || invite != null) {
      final text = invite?.code ?? value;
      _entry.value = TextEditingValue(
        text: text,
        selection: TextSelection.collapsed(offset: text.length),
      );
    }
    setState(() => _invite = invite);
  }

  @override
  Widget build(BuildContext context) {
    final input = _entry.text.trim();
    final manual = validRoomCode(input);
    final invite = _invite;
    final request = widget.create
        ? RoomRequest(game: _game, code: '', create: _options)
        : invite ?? (manual ? RoomRequest(game: _game, code: input) : null);
    final invalid =
        input.isNotEmpty &&
        request == null &&
        !RegExp(r'^[0-9]{1,3}$').hasMatch(input);
    final strings = context.strings;
    return AlertDialog(
      title: Text(
        widget.create ? context.strings.createRoom : context.strings.roomByCode,
      ),
      scrollable: true,
      content: SizedBox(
        width: 360,
        child: Column(
          mainAxisSize: MainAxisSize.min,
          crossAxisAlignment: CrossAxisAlignment.stretch,
          spacing: 12,
          children: [
            if (widget.create || !widget.directCode)
              DropdownButtonFormField<Game>(
                initialValue: _game,
                isExpanded: true,
                decoration: InputDecoration(
                  labelText: context.strings.roomGame,
                ),
                items: [
                  for (final game in widget.games)
                    DropdownMenuItem(
                      value: game,
                      child: Text(game.title, overflow: TextOverflow.ellipsis),
                    ),
                ],
                onChanged: (game) {
                  if (game == null) return;
                  setState(() {
                    _game = game;
                    _options = initialRoomOptions(game);
                  });
                },
              ),
            if (widget.create)
              RoomOptionsFields(
                key: ValueKey(_game.id),
                game: _game,
                value: _options,
                onChanged: (value) => setState(() => _options = value),
              )
            else
              TextField(
                controller: _entry,
                autofocus: true,
                keyboardType: TextInputType.number,
                decoration: InputDecoration(
                  labelText: strings.roomCodeOrInvite,
                  helperText: invite == null
                      ? strings.roomCodeFormat
                      : '${invite.game.title} | ${strings.roomNumber(invite.code)}',
                  helperMaxLines: 2,
                  errorText: invalid ? strings.roomCodeOrInviteInvalid : null,
                  errorMaxLines: 2,
                  suffixIcon: IconButton(
                    tooltip: MaterialLocalizations.of(context).pasteButtonLabel,
                    icon: const Icon(Icons.content_paste_outlined, size: 18),
                    onPressed: _pasteRoom,
                  ),
                ),
                onChanged: _updateEntry,
              ),
          ],
        ),
      ),
      actions: [
        TextButton(
          onPressed: () => Navigator.of(context).pop(),
          child: Text(context.strings.cancel),
        ),
        FilledButton(
          onPressed: request == null
              ? null
              : () => Navigator.of(context).pop(request),
          child: Text(
            widget.create
                ? strings.create
                : invite?.watch == true
                ? strings.roomWatch
                : strings.join,
          ),
        ),
      ],
    );
  }
}
