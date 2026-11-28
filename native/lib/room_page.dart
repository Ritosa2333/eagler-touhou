import 'dart:async';
import 'dart:math' as math;

import 'package:flutter/services.dart';
import 'package:material_ui/material_ui.dart';
import 'package:prism_ui/icons/icons.dart' as prism_icons;
import 'package:prism_ui/prism_ui.dart' as prism;

import 'account.dart';
import 'catalog.dart';
import 'game_launcher.dart';
import 'game_preferences.dart';
import 'game_settings.dart';
import 'l10n/strings.dart';
import 'launcher_select.dart';
import 'site_documents.dart';
import 'library.dart';
import 'launcher_background.dart';
import 'launcher_navigation.dart';
import 'launcher_prism.dart';
import 'lobby_directory.dart';
import 'profile_identity.dart';
import 'room_chat_pane.dart';
import 'room_options_fields.dart';
import 'room_session.dart';
import 'room_voice.dart';
import 'th09_player_card.dart';
import 'th09_rating_feed.dart';
import 'th09_ratings.dart';
import 'touch_preferences.dart';

const _roomMaxWidth = 1040.0;
const _roomGutter = 16.0;
const _coverSize = 56.0;
const _coverGap = 12.0;

class RoomPage extends StatefulWidget {
  const RoomPage({
    super.key,
    required this.request,
    required this.directory,
    required this.account,
    required this.launcher,
    required this.preferences,
  });

  final RoomRequest request;
  final LobbyDirectory directory;
  final AccountSession account;
  final GameLaunchService launcher;
  final LauncherPreferences preferences;

  @override
  State<RoomPage> createState() => _RoomPageState();
}

class _RoomPageState extends State<RoomPage> {
  RoomSession? _session;
  bool _leaving = false;
  bool _launching = false;
  Future<TouchOptions?>? _movementDecision;
  String? _username;
  final _voice = RoomVoice();

  @override
  void initState() {
    super.initState();
    widget.directory.addListener(_connectRoom);
    _username = widget.account.identity?.username;
    widget.account.addListener(_accountChanged);
    WidgetsBinding.instance.addPostFrameCallback((_) {
      if (!mounted) return;
      widget.directory.start();
      _connectRoom();
    });
  }

  void _connectRoom() {
    if (!mounted || _leaving || _session != null) return;
    final directory = widget.directory;
    final relay = directory.roomRelay;
    if (relay == null ||
        directory.occupiedRoomCode != null ||
        !relay.products.containsKey(widget.request.game.multiplayerProduct)) {
      return;
    }
    final account = widget.account;
    final room = RoomSession(
      relay: relay,
      request: widget.request,
      readInput: _readRoomInput,
      onMovementRestricted: () => unawaited(_correctRoomMovement()),
      onPhrase: (phrase) {
        if (mounted && !_leaving && ModalRoute.of(context)?.isActive == true) {
          _voice.play(phrase, widget.preferences.quickChatVolume);
        }
      },
      identityTicket: (challenge) async {
        final username = account.identity?.username;
        if (username == null) return null;
        try {
          final ticket = await account.api.roomIdentityTicket(challenge);
          return account.identity?.username == username ? ticket : null;
        } catch (error) {
          if (mounted) account.expireIfUnauthorized(error);
          return null;
        }
      },
    );
    setState(() => _session = room);
    room.start();
  }

  void _accountChanged() {
    final username = widget.account.identity?.username;
    if (_username == username) return;
    setState(() => _username = username);
    if (!_leaving) _session?.refreshIdentity();
  }

  @override
  void dispose() {
    widget.directory.removeListener(_connectRoom);
    widget.account.removeListener(_accountChanged);
    _session?.dispose();
    _voice.dispose();
    super.dispose();
  }

  Future<void> _leave() async {
    if (_leaving) return;
    setState(() => _leaving = true);
    _voice.dispose();
    await _session?.leave();
    if (mounted) closeLauncherPage(context, '/lobby');
  }

  Future<void> _copyCode() async {
    await Clipboard.setData(ClipboardData(text: widget.request.code));
    if (mounted) _notice(context.strings.roomCodeCopied);
  }

  Future<void> _copyInvite(RoomSession session) async {
    final language = bundledLanguage(Localizations.localeOf(context));
    final address = RoomRequest(
      game: session.game,
      code: session.code,
    ).inviteLink(widget.directory.origin, language);
    await Clipboard.setData(ClipboardData(text: address.toString()));
    if (mounted) _notice(context.strings.roomInviteCopied);
  }

  void _notice(String message) =>
      ScaffoldMessenger.of(context)
          .showSnackBar(SnackBar(content: Text(message)));

  bool _stillInRoom(Game game) =>
      mounted &&
      !_leaving &&
      _session?.connected == true &&
      identical(_session?.game, game);

  Future<RoomInput?> _readRoomInput(
    Game game, {
    required bool restrictMovement,
  }) async {
    final store = widget.launcher.preferences;
    final settings = await store.load(game, multiplayer: true);
    final touch = await store.touch.loadOptions(game.id);
    final prepared = await _ensureRoomMovement(game, touch, restrictMovement);
    return prepared == null
        ? null
        : (
            movementMode: prepared.movement,
            touchEnabled: settings.enabled(GameToggle.touchEnabled),
          );
  }

  Future<TouchOptions?> _ensureRoomMovement(
    Game game,
    TouchOptions options,
    bool restricted,
  ) async {
    if (!restricted || options.movement != 'touch-unlimited') return options;
    if (_movementDecision case final pending?) return pending;
    if (!_stillInRoom(game) || ModalRoute.of(context)?.isCurrent != true) {
      return null;
    }
    final decision = _selectAllowedMovement(game);
    _movementDecision = decision;
    try {
      return await decision;
    } finally {
      if (identical(_movementDecision, decision)) _movementDecision = null;
    }
  }

  Future<TouchOptions?> _selectAllowedMovement(Game game) async {
    final strings = context.strings;
    final choice = await showDialog<String>(
      context: context,
      animationStyle: LauncherControl.overlayMotion(context),
      builder: (dialog) => AlertDialog(
        insetPadding: const EdgeInsets.symmetric(horizontal: 16, vertical: 24),
        scrollable: true,
        title: Text(strings.roomMovementRequired),
        content: Text(strings.roomMovementRequiredHint),
        actionsOverflowButtonSpacing: 4,
        actions: [
          TextButton(
            onPressed: () => Navigator.of(dialog).pop(),
            child: Text(strings.cancel),
          ),
          OutlinedButton(
            onPressed: () => Navigator.of(dialog).pop('joystick'),
            child: Text(strings.roomUseJoystick),
          ),
          FilledButton(
            onPressed: () => Navigator.of(dialog).pop('touch'),
            child: Text(strings.roomUseTouch),
          ),
        ],
      ),
    );
    if (choice == null || !_stillInRoom(game)) return null;
    return widget.launcher.preferences.touch.setOption(
      game.id,
      'touchMovementMode',
      choice,
    );
  }

  Future<void> _correctRoomMovement() async {
    final room = _session;
    if (room == null) return;
    final game = room.game;
    try {
      final options = await widget.launcher.preferences.touch.loadOptions(
        game.id,
      );
      final prepared = await _ensureRoomMovement(
        game,
        options,
        room.restrictsLocalMovement,
      );
      if (prepared != null && prepared.movement != options.movement) {
        await room.refreshMovement();
      }
    } on Exception {
      if (mounted && _stillInRoom(game)) {
        _notice(context.strings.roomPrepareError);
      }
    }
  }

  Future<void> _launch(RoomSession session) async {
    if (_launching || _leaving) return;
    setState(() => _launching = true);
    final game = session.game;
    final seat = session.localSeat;
    String? preparedMovement;
    try {
      final result = await widget.launcher.start(
        game,
        multiplayer: true,
        isActive: () =>
            _stillInRoom(game) &&
            session.localSeat == seat &&
            (seat != null || session.watching && session.watchReady) &&
            (!session.restrictsLocalMovement ||
                preparedMovement != 'touch-unlimited'),
        prepareTouch: (options) async {
          final prepared = await _ensureRoomMovement(
            game,
            options,
            session.restrictsLocalMovement,
          );
          preparedMovement = prepared?.movement;
          if (prepared != null && prepared.movement != options.movement) {
            await session.refreshMovement();
          }
          return prepared;
        },
      );
      if (mounted &&
          !_leaving &&
          result == GameLaunchStatus.nativeRuntimeUnavailable) {
        _notice(context.strings.runtimeUnavailable);
      }
    } on Exception {
      if (mounted && !_leaving) _notice(context.strings.roomPrepareError);
    } finally {
      if (mounted) setState(() => _launching = false);
    }
  }

  Future<void> _gameSettings(RoomSession room) => showDialog<void>(
    context: context,
    animationStyle: LauncherControl.overlayMotion(context),
    builder: (dialog) {
      final settings = GameSettingsPage(
        game: room.game,
        multiplayer: true,
        store: widget.launcher.preferences,
        files: widget.launcher.files,
        launchService: widget.launcher,
        launcherPreferences: widget.preferences,
        room: room,
        onBack: () => Navigator.of(dialog).pop(),
      );
      return MediaQuery.sizeOf(dialog).width <= launcherCompactWidth
          ? Dialog.fullscreen(child: settings)
          : Dialog(
              alignment: AlignmentDirectional.centerEnd,
              insetPadding: const EdgeInsets.all(16),
              constraints: const BoxConstraints(maxWidth: 440),
              clipBehavior: Clip.antiAlias,
              child: settings,
            );
    },
  );

  Future<void> _settings(RoomSession session, RoomSnapshot snapshot) async {
    final strings = context.strings;
    var draft = snapshot.options;
    var timing = snapshot.timing;
    final timingPolicy = session.game.multiplayerRules?.timing;
    final rules = <RoomRule, Object>{};
    final saved = await showDialog<bool>(
      context: context,
      animationStyle: LauncherControl.overlayMotion(context),
      builder: (dialog) => StatefulBuilder(
        builder: (_, update) => AlertDialog(
          title: Text(strings.roomPinnedSettings),
          insetPadding: const EdgeInsets.symmetric(
            horizontal: 16,
            vertical: 24,
          ),
          content: SizedBox(
            width: 360,
            child: SingleChildScrollView(
              child: Column(
                mainAxisSize: MainAxisSize.min,
                crossAxisAlignment: CrossAxisAlignment.stretch,
                children: [
                  RoomOptionsFields(
                    game: session.game,
                    value: draft,
                    onChanged: (value) => update(() => draft = value),
                  ),
                  if (timingPolicy != null &&
                      (timingPolicy.rollback || !timingPolicy.runtimeTiming))
                    if (timing case final RoomTiming value)
                      RoomTimingFields(
                        policy: timingPolicy,
                        value: value,
                        onChanged: (value) => update(() => timing = value),
                      ),
                  if (snapshot.rules.isNotEmpty)
                    RoomRulesFields(
                      game: session.game,
                      rules: {...snapshot.rules, ...rules},
                      onChanged: (rule, value) => update(() {
                        rules[rule] = value;
                        if (rule == RoomRule.challengeMode && value == false) {
                          rules.remove(RoomRule.challengeKeepBullets);
                        }
                      }),
                    ),
                ],
              ),
            ),
          ),
          actions: [
            TextButton(
              onPressed: () => Navigator.of(dialog).pop(),
              child: Text(strings.cancel),
            ),
            FilledButton(
              onPressed: () => Navigator.of(dialog).pop(true),
              child: Text(strings.roomSave),
            ),
          ],
        ),
      ),
    );
    if (mounted && !_leaving && saved == true) {
      if (draft != snapshot.options || rules.isNotEmpty) {
        session.changeOptions(draft, rules: rules);
      }
      if (timing case final RoomTiming value when value != snapshot.timing) {
        session.changeTiming(value);
      }
    }
  }

  Future<void> _remove(
    RoomSession room,
    RoomParticipant target, {
    int? seat,
  }) async {
    final strings = context.strings;
    final confirmed = await showDialog<bool>(
      context: context,
      animationStyle: LauncherControl.overlayMotion(context),
      builder: (dialog) => AlertDialog(
        title: Text(strings.roomRemove),
        content: Text(
          seat == null
              ? strings.roomRemoveSpectatorConfirm
              : strings.roomRemoveSeatConfirm(seat + 1),
        ),
        actions: [
          TextButton(
            onPressed: () => Navigator.of(dialog).pop(false),
            child: Text(strings.cancel),
          ),
          FilledButton(
            onPressed: () => Navigator.of(dialog).pop(true),
            child: Text(strings.roomRemoveConfirmAction),
          ),
        ],
      ),
    );
    if (confirmed != true || !mounted || _leaving) return;
    if (seat == null) {
      room.removeSpectator(target.id);
    } else {
      room.manageSeat(seat, target.id, remove: true);
    }
  }

  @override
  Widget build(BuildContext context) => PopScope(
    canPop: false,
    onPopInvokedWithResult: (didPop, _) {
      if (!didPop) _leave();
    },
    child: ListenableBuilder(
      listenable: _session ?? widget.directory,
      builder: (context, _) {
        final strings = context.strings;
        final session = _session;
        final snapshot = session?.snapshot;
        final online = session?.connected == true;
        final game = session?.game ?? widget.request.game;
        final roomLabel = strings.roomNumber(widget.request.code);
        final titleStyle = prism.AppTextStyle.headline.of(context);
        final compactBar = prism.isCompactBar(
          context,
          prism.ViewScope.isMobileOf(context),
        );
        const barHeight = 56.0;
        return Scaffold(
          backgroundColor: LauncherBackdrop.page(context),
          appBar: PreferredSize(
            preferredSize: const Size.fromHeight(barHeight),
            child: Center(
              child: ConstrainedBox(
                constraints: const BoxConstraints(
                  maxWidth: _roomMaxWidth + 2 * _roomGutter,
                ),
                child: prism.AppBar(
                  toolbarHeight: barHeight,
                  titleSpacing:
                      _roomGutter +
                      _coverSize +
                      _coverGap -
                      prism.appBarLeadingWidth(context, compactBar),
                  title: prism.TextButton(
                    onPressed: _copyCode,
                    style: prism.ButtonStyle.from(
                      padding: EdgeInsets.zero,
                      minimumSize: const Size.square(44),
                      foregroundColor: prism.Theme.of(context)
                          .colorScheme
                          .onSurface,
                      textStyle: titleStyle,
                    ),
                    child: Tooltip(
                      message: strings.roomCodeCopy,
                      child: LayoutBuilder(
                        builder: (context, bounds) {
                          final labelWidth = TextPainter.computeWidth(
                            text: TextSpan(text: roomLabel, style: titleStyle),
                            textDirection: Directionality.of(context),
                            textScaler: MediaQuery.textScalerOf(context),
                            maxLines: 1,
                          );
                          return FittedBox(
                            alignment: AlignmentDirectional.centerStart,
                            fit: BoxFit.scaleDown,
                            child: Text(
                              labelWidth <= bounds.maxWidth
                                  ? roomLabel
                                  : '#${widget.request.code}',
                              key: const ValueKey('room-code-title'),
                              semanticsLabel: roomLabel,
                              maxLines: 1,
                            ),
                          );
                        },
                      ),
                    ),
                  ),
                  leading: prism.IconButton(
                    tooltip: MaterialLocalizations.of(context)
                        .backButtonTooltip,
                    icon: const prism_icons.GlyphIcon(
                      prism_icons.Glyphs.arrowBack,
                    ),
                    onPressed: _leave,
                  ),
                  actions: [
                    if (online)
                      Padding(
                        padding: const EdgeInsetsDirectional.only(end: 8),
                        child: prism.TonalButtonGroup(
                          children: [
                            prism.IconButton(
                              tooltip: strings.roomInviteCopy,
                              icon: const Icon(Icons.link),
                              onPressed: () => _copyInvite(session!),
                            ),
                            prism.IconButton(
                              tooltip: strings.gameSettings,
                              icon: const Icon(Icons.settings_outlined),
                              onPressed:
                                  snapshot?.waiting == true &&
                                      !_launching &&
                                      !_leaving
                                  ? () => _gameSettings(session!)
                                  : null,
                            ),
                            if (session?.ownsRoom == true &&
                                snapshot?.waiting == true)
                              prism.IconButton(
                                tooltip: strings.roomPinnedSettings,
                                icon: const Icon(Icons.tune),
                                onPressed: () => _settings(session!, snapshot!),
                              ),
                          ],
                        ),
                      ),
                  ],
                ),
              ),
            ),
          ),
          body: SafeArea(
            bottom: false,
            child: LayoutBuilder(
              builder: (_, constraints) => SingleChildScrollView(
                key: const PageStorageKey('room content'),
                padding: const EdgeInsets.all(_roomGutter),
                child: Center(
                  child: ConstrainedBox(
                    constraints: const BoxConstraints(maxWidth: _roomMaxWidth),
                    child: Column(
                      crossAxisAlignment: CrossAxisAlignment.stretch,
                      spacing: 16,
                      children: [
                        _overview(context, game, snapshot, session),
                        if (session != null && snapshot != null)
                          LauncherPrismPanel(
                            child: Th09RatingFeed(
                              session: widget.account,
                              roster: game.id == 'th09'
                                  ? [
                                      for (final person in [
                                        ...snapshot.seats,
                                        ...snapshot.spectators,
                                      ])
                                        if (person?.account
                                            case final String username)
                                          username,
                                    ]
                                  : null,
                              builder: (context, ratings, own) => _workspace(
                                context,
                                session,
                                snapshot,
                                constraints.maxWidth,
                                game.id == 'th09' ? ratings : null,
                                own,
                              ),
                            ),
                          ),
                      ],
                    ),
                  ),
                ),
              ),
            ),
          ),
          bottomNavigationBar: session == null || snapshot == null
              ? null
              : _actions(context, session, snapshot),
        );
      },
    ),
  );

  Widget _workspace(
    BuildContext context,
    RoomSession room,
    RoomSnapshot snapshot,
    double width,
    Th09LobbyRatings? ratings,
    Th09LobbyRating? ownRating,
  ) {
    final seats = Column(
      crossAxisAlignment: CrossAxisAlignment.stretch,
      spacing: 16,
      children: [
        prism.Card(
          inert: true,
          child: IntrinsicHeight(
            child: Row(
              crossAxisAlignment: CrossAxisAlignment.stretch,
              children: [
                for (final (index, participant) in snapshot.seats.indexed) ...[
                  if (index > 0)
                    const VerticalDivider(width: 1, indent: 16, endIndent: 16),
                  Expanded(
                    child: _seat(
                      context,
                      room,
                      index,
                      participant,
                      width < 500,
                      ratings,
                      ownRating,
                    ),
                  ),
                ],
              ],
            ),
          ),
        ),
        _spectators(context, room, snapshot, ratings),
      ],
    );
    final chat = RoomChatPane(
      preferences: widget.preferences,
      session: room,
      signedIn: _username != null,
      signIn: () => openSiteLink(
        context,
        signInFor(
          RoomRequest(game: room.game, code: room.code)
              .inviteLink(
                Uri.https('touhouonline.com'),
                bundledLanguage(Localizations.localeOf(context)),
              )
              .toString(),
        ),
      ),
      avatar: (row) =>
          _avatar(room.participant(row.client), size: 28, fallback: row.name),
    );
    final wide = width >= 820;
    return Flex(
      direction: wide ? Axis.horizontal : Axis.vertical,
      crossAxisAlignment: wide
          ? CrossAxisAlignment.start
          : CrossAxisAlignment.stretch,
      spacing: 16,
      children: [
        Flexible(flex: wide ? 1 : 0, child: seats),
        SizedBox(width: wide ? 330 : null, child: chat),
      ],
    );
  }

  Widget _overview(
    BuildContext context,
    Game game,
    RoomSnapshot? snapshot,
    RoomSession? session,
  ) {
    final strings = context.strings;
    final theme = Theme.of(context);
    final text = theme.textTheme;
    final muted = theme.colorScheme.onSurfaceVariant;
    final timing = snapshot?.timing;
    final policy = game.multiplayerRules?.timing;
    return Row(
      crossAxisAlignment: CrossAxisAlignment.start,
      spacing: _coverGap,
      children: [
        GameArtwork(game: game, size: _coverSize),
        Expanded(
          child: Column(
            crossAxisAlignment: CrossAxisAlignment.start,
            spacing: 3,
            children: [
              Text(
                snapshot?.title ?? game.title,
                style: prism.AppTextStyle.title2.of(context),
              ),
              Text(
                snapshot?.title == null ? game.subtitle : game.title,
                style: text.bodyMedium?.copyWith(color: muted),
                maxLines: 2,
                overflow: TextOverflow.ellipsis,
              ),
              if (snapshot != null)
                Text(
                  [
                    snapshot.options.private
                        ? strings.roomPrivate
                        : strings.roomPublic,
                    strings.roomPlayerCount(snapshot.options.players),
                    if (game.multiplayerRules?.selectInGame != true)
                      roomDifficulty(game, snapshot.options.difficulty),
                    if (snapshot.options.limitMovement)
                      strings.roomLimitMovement,
                    if (snapshot.options.unrated) strings.roomUnrated,
                    if (snapshot.rules[RoomRule.challengeMode] == true)
                      strings.roomChallengeMode,
                    if (snapshot.waiting && timing != null)
                      if (timing.rollback)
                        strings.roomRollback
                      else if (policy?.runtimeTiming == false)
                        if (timing.frames case final int frames)
                          '${strings.roomInputDelay} ${frames}f'
                        else
                          policy?.measuredStartup == true
                              ? strings.roomInputDelayAutoToggle
                              : '${strings.roomInputDelay} ${strings.roomTimingAuto}',
                  ].join(' | '),
                  style: text.bodySmall?.copyWith(color: muted),
                ),
              _status(context, session),
            ],
          ),
        ),
      ],
    );
  }

  Widget _status(BuildContext context, RoomSession? room) {
    final connection = room?.connection;
    final directory = widget.directory;
    final blocked = room == null && directory.occupiedRoomCode != null;
    final unsupported =
        room == null &&
        directory.roomRelay?.products.containsKey(
              widget.request.game.multiplayerProduct,
            ) ==
            false;
    final waitingForDirectory =
        room == null &&
        !blocked &&
        directory.connection == LobbyConnection.loading;
    final unavailable = room == null && !waitingForDirectory;
    final loading =
        waitingForDirectory || connection == RoomConnection.connecting;
    final reconnecting = connection == RoomConnection.reconnecting;
    final strings = context.strings;
    final language = bundledLanguage(Localizations.localeOf(context));
    final message = switch (connection) {
      RoomConnection.connected =>
        room!.statusText(strings, language) ??
            switch (room.snapshot?.phase) {
              'starting' => strings.roomStarting,
              'running' => strings.roomPlaying,
              _ => strings.roomWaiting,
            },
      RoomConnection.reconnecting => strings.roomReconnecting,
      RoomConnection.closed =>
        room!.statusText(strings, language) ?? strings.roomGone,
      RoomConnection.connecting => strings.roomConnecting,
      null when blocked => strings.roomConflict,
      null when unsupported => strings.roomGameMissing,
      null => switch (directory.connection) {
        LobbyConnection.loading => strings.lobbyConnecting,
        LobbyConnection.unconfigured => strings.roomOfflineService,
        _ => strings.roomServiceUnavailable,
      },
    };
    final theme = Theme.of(context);
    final colors = theme.colorScheme;
    final attention =
        unavailable ||
        reconnecting ||
        connection == RoomConnection.closed ||
        room?.notice != null;
    return Padding(
      padding: const EdgeInsets.only(top: 3),
      child: Row(
        spacing: 8,
        children: [
          Flexible(
            child: Semantics(
              liveRegion: true,
              child: Text(
                message,
                style: theme.textTheme.bodySmall?.copyWith(
                  color: attention ? colors.error : colors.onSurfaceVariant,
                ),
              ),
            ),
          ),
          if (loading || reconnecting)
            const SizedBox.square(
              dimension: 14,
              child: CircularProgressIndicator(strokeWidth: 2),
            ),
          if ((unavailable && !blocked && !unsupported) || reconnecting)
            TextButton(
              onPressed: unavailable ? directory.refresh : room!.reconnect,
              child: Text(strings.roomRetry),
            ),
        ],
      ),
    );
  }

  Widget _avatar(
    RoomParticipant? participant, {
    double size = 38,
    String fallback = '',
    Th09LobbyRatings? ratings,
  }) {
    if (participant?.account case final String name) {
      final avatar = AccountAvatar(
        api: widget.account.api,
        username: name,
        size: size,
      );
      if (ratings == null) return avatar;
      return Th09PlayerCardButton(
        api: widget.account.api,
        username: name,
        name: participant!.name.isEmpty ? name : participant.name,
        scores: ratings.cards[name.toLowerCase()],
        child: avatar,
      );
    }
    final name = participant?.name ?? fallback;
    return CircleAvatar(
      radius: size / 2,
      child: name.isEmpty
          ? const Icon(Icons.person_outline, size: 16)
          : Text(String.fromCharCode(name.runes.first)),
    );
  }

  Widget _seat(
    BuildContext context,
    RoomSession room,
    int index,
    RoomParticipant? participant,
    bool compact,
    Th09LobbyRatings? ratings,
    Th09LobbyRating? ownRating,
  ) {
    final strings = context.strings;
    final mine = room.localSeat == index;
    final waiting = room.snapshot?.waiting == true;
    final manageable =
        room.ownsRoom && index > 0 && waiting && participant != null;
    final label = participant?.name.isNotEmpty == true
        ? participant!.name
        : 'P${index + 1}';
    final theme = Theme.of(context);
    final text = theme.textTheme;
    final muted = theme.colorScheme.onSurfaceVariant;
    return Padding(
      padding: EdgeInsets.fromLTRB(compact ? 8 : 12, 4, compact ? 8 : 12, 16),
      child: Column(
        spacing: 4,
        children: [
          SizedBox(
            width: double.infinity,
            height: 36,
            child: Stack(
              alignment: AlignmentDirectional.center,
              children: [
                Text(
                  mine ? 'P${index + 1} | ${strings.roomYou}' : 'P${index + 1}',
                  style: text.labelMedium?.copyWith(
                    color: mine ? theme.colorScheme.primary : muted,
                  ),
                  maxLines: 1,
                  overflow: TextOverflow.ellipsis,
                ),
                if (manageable)
                  PositionedDirectional(
                    end: 0,
                    child: SizedBox.square(
                      dimension: 34,
                      child: PopupMenuButton<bool>(
                        tooltip: strings.roomManageSeat(index + 1),
                        padding: EdgeInsets.zero,
                        iconSize: 18,
                        constraints: const BoxConstraints(minWidth: 156),
                        onSelected: (remove) => remove
                            ? _remove(room, participant, seat: index)
                            : room.manageSeat(
                                index,
                                participant.id,
                                remove: false,
                              ),
                        itemBuilder: (_) => [
                          if (!participant.offline)
                            PopupMenuItem(
                              value: false,
                              child: Text(strings.roomMoveSpectator),
                            ),
                          PopupMenuItem(
                            value: true,
                            child: Text(strings.roomRemove),
                          ),
                        ],
                      ),
                    ),
                  ),
              ],
            ),
          ),
          SizedBox.square(
            dimension: ratings == null ? (compact ? 32 : 44) : 48,
            child: Center(
              child: _avatar(
                participant,
                size: compact ? 32 : 44,
                ratings: ratings,
              ),
            ),
          ),
          if (participant == null)
            TextButton(
              onPressed: waiting && room.connected
                  ? () => room.takeSeat(index)
                  : null,
              child: Text(
                index == 0 ? strings.roomTakeHostSeat : strings.roomTakeSeat,
                textAlign: TextAlign.center,
              ),
            )
          else ...[
            if (participant.name.isNotEmpty)
              Text(
                label,
                style: text.titleSmall,
                maxLines: 1,
                overflow: TextOverflow.ellipsis,
              ),
            if (ratings != null && participant.account != null)
              Th09RatingText(
                rating: ratings.players[participant.account!.toLowerCase()],
                viewer: ownRating,
                games: participant.ratingGames,
                alignment: TextAlign.center,
              ),
            Text(
              participant.offline
                  ? strings.roomOffline
                  : participant.ready
                  ? strings.roomSeatReady
                  : strings.roomSeatWaiting,
              style: text.bodySmall?.copyWith(color: muted),
              textAlign: TextAlign.center,
            ),
            if (participant.movement case final RoomMovement movement)
              Text(
                movement.label(strings),
                style: text.bodySmall?.copyWith(color: muted),
                textAlign: TextAlign.center,
              ),
          ],
        ],
      ),
    );
  }

  Widget _spectators(
    BuildContext context,
    RoomSession room,
    RoomSnapshot snapshot,
    Th09LobbyRatings? ratings,
  ) => prism.Card(
    inert: true,
    child: ExpansionTile(
      key: const PageStorageKey('room spectators'),
      leading: const Icon(Icons.groups_outlined),
      title: Text(
        '${context.strings.roomSpectators} | ${snapshot.spectators.length}',
      ),
      children: [
        if (snapshot.spectators.isEmpty)
          ListTile(title: Text(context.strings.roomSpectatorEmpty)),
        for (final participant in snapshot.spectators)
          ListTile(
            leading: _avatar(participant, size: 30, ratings: ratings),
            title: Text(
              participant.name.isEmpty
                  ? context.strings.roomUnnamedSpectator
                  : participant.name,
            ),
            onTap: participant.account == null
                ? null
                : () =>
                      openSiteLink(context, profilePath(participant.account!)),
            trailing: room.ownsRoom && snapshot.waiting
                ? IconButton(
                    tooltip: context.strings.roomRemoveSpectator,
                    icon: const Icon(Icons.person_remove_outlined),
                    onPressed: () => _remove(room, participant),
                  )
                : null,
          ),
      ],
    ),
  );

  double _roomButtonWidth(BuildContext context, String label, double insets) {
    final painter = TextPainter(
      text: TextSpan(
        text: label,
        style: prism.AppTextStyle.subheadline.of(context, emphasized: true),
      ),
      textDirection: Directionality.of(context),
      textScaler: MediaQuery.textScalerOf(context),
      maxLines: 1,
    )..layout();
    final width = math.max(64.0, painter.width.ceilToDouble() + insets);
    painter.dispose();
    return width;
  }

  Widget _actions(
    BuildContext context,
    RoomSession room,
    RoomSnapshot snapshot,
  ) {
    final connected = room.connected && !_leaving;
    final seated = room.localSeat != null;
    final strings = context.strings;
    final watchLabel = room.watching
        ? strings.roomLeaveSpectator
        : strings.roomWatch;
    final watch = connected && (!seated || snapshot.waiting)
        ? room.watching
              ? room.stopSpectating
              : room.spectate
        : null;
    final width = MediaQuery.sizeOf(context).width;
    final narrow = width <= 370;
    const compactPadding = EdgeInsets.symmetric(horizontal: 12);
    final launchLabel = room.watching
        ? strings.roomWatchGame
        : strings.startGame;
    final safePadding = MediaQuery.paddingOf(context);
    final available =
        width -
        math.max(_roomGutter, safePadding.left) -
        math.max(_roomGutter, safePadding.right);
    const spacing = 8.0;
    const iconTarget = 48.0;
    final launchButtonWidth = narrow
        ? _roomButtonWidth(context, launchLabel, compactPadding.horizontal + 26)
        : 0.0;
    final compactLeaveSeat =
        narrow &&
        seated &&
        snapshot.waiting &&
        _roomButtonWidth(
                  context,
                  strings.roomLeaveSeat,
                  compactPadding.horizontal,
                ) +
                launchButtonWidth +
                iconTarget +
                2 * spacing >
            available;
    final textOnlyLaunch =
        compactLeaveSeat &&
        launchButtonWidth + 2 * (iconTarget + spacing) > available;
    final launch = connected && !_launching ? () => _launch(room) : null;
    final colors = prism.Theme.of(context).colorScheme;
    final secondary = prism.ButtonStyle.from(
      foregroundColor: colors.onSurface,
      padding: compactPadding,
    );
    const primary = prism.ButtonStyle(padding: compactPadding);
    return Material(
      color: colors.surfaceContainerLow,
      child: SafeArea(
        top: false,
        minimum: const EdgeInsets.symmetric(
          horizontal: _roomGutter,
          vertical: 6,
        ),
        child: Center(
          heightFactor: 1,
          child: ConstrainedBox(
            constraints: const BoxConstraints.tightFor(width: _roomMaxWidth),
            child: Wrap(
              alignment: width >= 820
                  ? WrapAlignment.end
                  : WrapAlignment.center,
              crossAxisAlignment: WrapCrossAlignment.center,
              spacing: spacing,
              runSpacing: 4,
              children: [
                if (seated && snapshot.waiting)
                  if (compactLeaveSeat)
                    prism.IconButton(
                      tooltip: strings.roomLeaveSeat,
                      onPressed: connected ? room.standUp : null,
                      icon: const Icon(Icons.event_seat_outlined),
                    )
                  else
                    prism.TextButton(
                      onPressed: connected ? room.standUp : null,
                      style: secondary,
                      child: Text(strings.roomLeaveSeat),
                    ),
                if (narrow && seated)
                  prism.IconButton(
                    tooltip: watchLabel,
                    onPressed: watch,
                    icon: const prism_icons.GlyphIcon(prism_icons.Glyphs.eye),
                  )
                else
                  prism.TextButton(
                    onPressed: watch,
                    style: secondary,
                    child: Text(watchLabel),
                  ),
                if (seated || room.watching && room.watchReady)
                  if (textOnlyLaunch)
                    prism.FilledButton(
                      onPressed: launch,
                      style: primary,
                      child: Text(launchLabel),
                    )
                  else
                    prism.FilledButton.icon(
                      onPressed: launch,
                      style: width < 500 ? primary : null,
                      icon: const Icon(Icons.play_arrow),
                      label: Text(launchLabel),
                    ),
              ],
            ),
          ),
        ),
      ),
    );
  }
}
