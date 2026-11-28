import 'dart:ui' as ui show ImageFilter, TileMode;

import 'package:go_router/go_router.dart';
import 'package:lucide_icons_flutter/lucide_icons.dart';
import 'package:material_ui/material_ui.dart';

import 'account.dart';
import 'catalog.dart';
import 'game_clear_matrix.dart';
import 'game_friends.dart';
import 'game_progress_catalog.dart';
import 'game_spell_archive.dart';
import 'game_import_dialog.dart';
import 'game_launcher.dart';

import 'game_resources.dart';
import 'library.dart';
import 'l10n/strings.dart';
import 'site_documents.dart';
import 'launcher_navigation.dart';
import 'launcher_prism.dart';
import 'launcher_select.dart';
import 'save_progress.dart';

class GameHeading extends StatelessWidget {
  const GameHeading({
    super.key,
    required this.game,
    required this.coverSize,
    this.metrics,
    this.small = false,
  });

  final Game game;
  final double coverSize;
  final Widget? metrics;
  final bool small;

  @override
  Widget build(BuildContext context) {
    final theme = Theme.of(context);
    final viewport = MediaQuery.sizeOf(context).width;
    final compact = viewport <= launcherCompactWidth;
    final numberGap = compact || small ? 6.0 : 8.0;
    final gap = small
        ? 12.0
        : compact
        ? 12.0
        : (viewport * .024).clamp(18.0, 34.0);
    final titleStyle = theme.textTheme.titleLarge?.copyWith(
      fontSize: small
          ? 18
          : compact
          ? 20
          : (viewport * .034).clamp(30.0, 48.0),
      fontWeight: FontWeight.w700,
      letterSpacing: 0,
      height: 1.25,
      leadingDistribution: TextLeadingDistribution.even,
    );
    final codeStyle = theme.textTheme.labelMedium?.copyWith(
      color: theme.colorScheme.onSurfaceVariant,
      fontSize: compact || small ? 11 : 13,
      fontWeight: FontWeight.w400,
    );
    return LayoutBuilder(
      builder: (context, constraints) {
        final scaler = MediaQuery.textScalerOf(context);
        double width(String value, TextStyle? style) =>
            TextPainter.computeWidth(
              text: TextSpan(text: value, style: style),
              textDirection: Directionality.of(context),
              textScaler: scaler,
              maxLines: 1,
            );
        final stacked =
            !small &&
            scaler.scale(titleStyle!.fontSize!) > titleStyle.fontSize! * 1.3 &&
            constraints.maxWidth - coverSize - gap <
                scaler.scale(titleStyle.fontSize!) * 5;
        final textWidth = stacked
            ? constraints.maxWidth
            : constraints.maxWidth - coverSize - gap;
        final title = Text(game.title, style: titleStyle);
        final code = Text(game.displayCode, style: codeStyle);
        final heading =
            width(game.title, titleStyle) +
                    width(game.displayCode, codeStyle) +
                    numberGap <=
                textWidth
            ? Row(
                spacing: numberGap,
                crossAxisAlignment: CrossAxisAlignment.baseline,
                textBaseline: TextBaseline.alphabetic,
                children: [
                  Flexible(child: title),
                  code,
                ],
              )
            : Column(
                spacing: 4,
                crossAxisAlignment: CrossAxisAlignment.start,
                children: [title, code],
              );
        final details = Column(
          spacing: 4,
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            heading,
            Text(
              game.subtitle,
              style: theme.textTheme.bodySmall?.copyWith(
                color: theme.colorScheme.onSurfaceVariant,
                fontSize: compact || small ? 12 : 16,
                height: 1.45,
                leadingDistribution: TextLeadingDistribution.even,
              ),
            ),
            if (metrics case final Widget item) item,
          ],
        );
        if (stacked) {
          return Column(
            spacing: 12,
            crossAxisAlignment: CrossAxisAlignment.start,
            children: [
              GameArtwork(
                game: game,
                size: coverSize,
                borderRadius: BorderRadius.circular(8),
              ),
              details,
            ],
          );
        }
        return Row(
          spacing: gap,
          crossAxisAlignment: CrossAxisAlignment.center,
          children: [
            GameArtwork(
              game: game,
              size: coverSize,
              borderRadius: BorderRadius.circular(8),
            ),
            Expanded(child: details),
          ],
        );
      },
    );
  }
}

class GameDetail extends StatefulWidget {
  const GameDetail({
    super.key,
    required this.book,
    required this.game,
    required this.launchService,
    required this.resources,
    required this.multiplayer,
    required this.session,
  });

  final BookState book;
  final Game game;
  final GameLaunchService launchService;
  final GameResources resources;
  final bool multiplayer;
  final AccountSession session;

  @override
  State<GameDetail> createState() => _GameDetailState();
}

enum _CloudState { guest, loading, ready, error }

class _GameDetailState extends State<GameDetail> {
  bool loading = false;
  bool _openingImport = false;
  _CloudState _cloud = _CloudState.guest;
  CloudGameSave? _save;
  String? _owner;
  int _readGeneration = 0;

  @override
  void initState() {
    super.initState();
    widget.session.addListener(_syncAccount);
    widget.session.cloudSaveRevision.addListener(_cloudUpdated);
    _syncAccount();
  }

  @override
  void dispose() {
    widget.session.removeListener(_syncAccount);
    widget.session.cloudSaveRevision.removeListener(_cloudUpdated);
    super.dispose();
  }

  void _syncAccount() {
    final name = widget.session.identity?.username;
    if (widget.multiplayer || name == _owner) return;
    _load(name);
  }

  void _cloudUpdated() {
    if (_owner != null) _load(_owner, refresh: true);
  }

  Future<void> _load(String? owner, {bool refresh = false}) async {
    final generation = ++_readGeneration;
    setState(() {
      _owner = owner;
      if (!refresh || owner == null) _save = null;
      _cloud = owner == null ? _CloudState.guest : _CloudState.loading;
    });
    if (owner == null) return;
    try {
      final saves = await widget.session.api.cloudGameSaves();
      if (!mounted || generation != _readGeneration) return;
      setState(() {
        _save = saves[widget.game.id];
        _cloud = _CloudState.ready;
      });
    } catch (error) {
      if (!mounted || generation != _readGeneration) return;
      widget.session.expireIfUnauthorized(error);
      if (generation == _readGeneration) {
        setState(() => _cloud = _CloudState.error);
      }
    }
  }

  String get _parent => widget.multiplayer ? '/lobby' : '/';
  String get _settingsPath =>
      '${gamePath(widget.game.id, multiplayer: widget.multiplayer)}/settings';

  Future<void> _import() async {
    final data = widget.game.data;
    if (_openingImport || data == null) return;
    _openingImport = true;
    try {
      final existing = await widget.resources.installed(data);
      if (!mounted) return;
      final imported = await showDialog<bool>(
        context: context,
        animationStyle: LauncherControl.overlayMotion(context),
        barrierDismissible: false,
        builder: (_) => GameDataImportDialog(
          game: widget.game,
          resources: widget.resources,
          installed: existing != null,
        ),
      );
      if (imported == true && mounted) {
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(content: Text(context.strings.resourcesImported)),
        );
      }
    } on Exception {
      if (mounted) {
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(content: Text(context.strings.resourceFolderError)),
        );
      }
    } finally {
      _openingImport = false;
    }
  }

  Future<void> _start() async {
    if (loading) return;
    setState(() => loading = true);
    try {
      final result = await widget.launchService.start(
        widget.game,
        multiplayer: widget.multiplayer,
        isActive: () => mounted,
      );
      if (!mounted) return;
      if (result == GameLaunchStatus.nativeRuntimeUnavailable) {
        await showDialog<void>(
          context: context,
          animationStyle: LauncherControl.overlayMotion(context),
          builder: (dialog) => AlertDialog(
            title: Text(dialog.strings.runtimeUnavailable),
            content: Text(widget.game.title),
            actions: [
              TextButton(
                onPressed: () => Navigator.of(dialog).pop(),
                child: Text(dialog.strings.backToGame),
              ),
            ],
          ),
        );
      }
    } on Exception {
      if (!mounted) return;
      ScaffoldMessenger.of(context)
          .showSnackBar(SnackBar(content: Text(context.strings.startFailed)));
    } finally {
      if (mounted) setState(() => loading = false);
    }
  }

  @override
  Widget build(BuildContext context) => LayoutBuilder(
    builder: (context, constraints) => _detail(context, constraints.maxWidth),
  );

  Widget _detail(BuildContext context, double availableWidth) {
    final save = _save;
    final game = widget.game;
    final viewport = MediaQuery.sizeOf(context).width;
    final compact = viewport <= launcherCompactWidth;
    final inset = compact ? 16.0 : (viewport * .039).clamp(20.0, 56.0);
    final metricsInActions = !compact && availableWidth - 2 * inset >= 692;
    Widget frame(Widget child) => Center(
      child: ConstrainedBox(
        constraints: const BoxConstraints(maxWidth: 1040),
        child: child,
      ),
    );
    return ListView(
      key: PageStorageKey('detail.${game.id}.${widget.multiplayer}'),
      padding: EdgeInsets.zero,
      children: [
        _hero(
          context,
          compact,
          (context) => Padding(
            padding: EdgeInsets.fromLTRB(
              inset,
              compact ? 6 : 40,
              inset,
              compact ? 12 : 18,
            ),
            child: frame(
              Column(
                crossAxisAlignment: CrossAxisAlignment.stretch,
                children: [
                  if (compact) ...[
                    Row(
                      mainAxisAlignment: MainAxisAlignment.spaceBetween,
                      children: [
                        BackButton(
                          onPressed: () => closeLauncherPage(context, _parent),
                        ),
                        FavoriteButton(
                          book: widget.book,
                          game: game,
                          outlined: true,
                        ),
                      ],
                    ),
                    const SizedBox(height: 8),
                  ],
                  GameHeading(
                    game: game,
                    coverSize: compact
                        ? (viewport * .277).clamp(76.0, 116.0)
                        : (viewport * .121).clamp(102.0, 174.0),
                    metrics: !widget.multiplayer && !metricsInActions
                        ? Padding(
                            padding: const EdgeInsets.only(top: 4),
                            child: _metrics(context, save),
                          )
                        : null,
                  ),
                  SizedBox(
                    height: compact ? 30 : (viewport * .0415).clamp(28.0, 60.0),
                  ),
                  _actions(context, save, metricsInActions: metricsInActions),
                ],
              ),
            ),
          ),
        ),
        if (!widget.multiplayer)
          Padding(
            padding: EdgeInsets.fromLTRB(
              inset,
              compact ? 18 : 26,
              inset,
              compact ? 24 : 42,
            ),
            child: frame(_detailContent(game, save)),
          ),
      ],
    );
  }

  Widget _hero(BuildContext context, bool compact, WidgetBuilder content) {
    final theme = Theme.of(context);
    const text = Color(0xFFF8F5F8);
    final heroTheme = theme.copyWith(
      brightness: Brightness.dark,
      colorScheme: theme.colorScheme.copyWith(
        brightness: Brightness.dark,
        surface: const Color(0xFF25222D),
        surfaceContainerLow: const Color(0x4214131F),
        onSurface: text,
        onSurfaceVariant: const Color(0xFFE1D8E1),
        outlineVariant: const Color(0x55FFFFFF),
      ),
      textTheme: theme.textTheme.apply(bodyColor: text, displayColor: text),
      iconTheme: theme.iconTheme.copyWith(color: text),
    );
    return Theme(
      data: heroTheme,
      child: Builder(
        builder: (context) => ClipRect(
          child: DecoratedBox(
            decoration: const BoxDecoration(
              color: Color(0xFF25222D),
              border: Border(bottom: BorderSide(color: Color(0x30FFFFFF))),
            ),
            child: Stack(
              children: [
                if (widget.game.artwork case final String cover)
                  Positioned.fill(
                    child: ExcludeSemantics(
                      child: RepaintBoundary(
                        child: ImageFiltered(
                          imageFilter: ui.ImageFilter.blur(
                            sigmaX: compact ? 32 : 80,
                            sigmaY: compact ? 32 : 80,
                            tileMode: ui.TileMode.clamp,
                          ),
                          child: Image.asset(
                            'assets/book-covers/$cover',
                            fit: BoxFit.cover,
                            // Reuse the cover's cached image for the backdrop.
                            color: const Color(0x73000000),
                            colorBlendMode: BlendMode.srcOver,
                            errorBuilder: (_, _, _) => const SizedBox.expand(),
                          ),
                        ),
                      ),
                    ),
                  ),
                const Positioned.fill(
                  child: DecoratedBox(
                    decoration: BoxDecoration(
                      gradient: LinearGradient(
                        begin: Alignment.topLeft,
                        end: Alignment.bottomRight,
                        colors: [Color(0x6617131C), Color(0xD1151520)],
                      ),
                    ),
                  ),
                ),
                content(context),
                if (!compact)
                  PositionedDirectional(
                    start: 9,
                    top: 5,
                    child: BackButton(
                      onPressed: () => closeLauncherPage(context, _parent),
                    ),
                  ),
              ],
            ),
          ),
        ),
      ),
    );
  }

  Widget _actions(
    BuildContext context,
    CloudGameSave? save, {
    required bool metricsInActions,
  }) {
    final compact = MediaQuery.sizeOf(context).width <= launcherCompactWidth;
    final game = widget.game;
    const playBackground = Color(0xFF95C796);
    const playForeground = Color(0xFF132C1C);
    return LayoutBuilder(
      builder: (context, constraints) {
        final strings = context.strings;
        final controls = LauncherControl.style(context);
        final hasImport = game.data != null;
        final actionCount = hasImport ? 2 : 1;
        final playWidth =
            LauncherControl.labelWidth(
              context,
              strings.startGame,
              weight: FontWeight.w700,
            ) +
            52;
        final roomForLabels =
            constraints.maxWidth -
            playWidth -
            actionCount * (LauncherControl.height + 8) -
            (compact
                ? 0
                : metricsInActions
                ? 272
                : 60);
        final importExtra =
            LauncherControl.iconButtonWidth(context, strings.gameFileImport) -
            LauncherControl.height;
        final settingsExtra =
            LauncherControl.iconButtonWidth(context, strings.gameSettings) -
            LauncherControl.height;
        final showImportLabel = hasImport && roomForLabels >= importExtra;
        final showSettingsLabel =
            constraints.maxWidth >= 340 &&
            roomForLabels >=
                settingsExtra + (showImportLabel ? importExtra : 0);
        final settings = !showSettingsLabel
            ? IconButton.outlined(
                style: controls,
                tooltip: context.strings.gameSettings,
                onPressed: () => context.push(_settingsPath),
                icon: const Icon(
                  LucideIcons.slidersHorizontal,
                  size: LauncherControl.iconSize,
                ),
              )
            : OutlinedButton.icon(
                style: controls,
                onPressed: () => context.push(_settingsPath),
                icon: const Icon(
                  LucideIcons.slidersHorizontal,
                  size: LauncherControl.iconSize,
                ),
                label: Text(context.strings.gameSettings),
              );
        final import = !showImportLabel
            ? IconButton.outlined(
                style: controls,
                tooltip: context.strings.importingResources,
                onPressed: _import,
                icon: const Icon(
                  LucideIcons.folderInput,
                  size: LauncherControl.iconSize,
                ),
              )
            : OutlinedButton.icon(
                style: controls,
                onPressed: _import,
                icon: const Icon(
                  LucideIcons.folderInput,
                  size: LauncherControl.iconSize,
                ),
                label: Text(context.strings.gameFileImport),
              );
        final play = FilledButton.icon(
          style: FilledButton.styleFrom(
            backgroundColor: playBackground,
            foregroundColor: playForeground,
            textStyle: LauncherControl.label(context)
                .copyWith(fontWeight: FontWeight.w700),
            minimumSize: const Size(48, LauncherControl.height),
            shape: LauncherControl.shape,
            iconSize: LauncherControl.iconSize,
            tapTargetSize: MaterialTapTargetSize.shrinkWrap,
            padding: const EdgeInsets.symmetric(horizontal: 10),
          ),
          onPressed: loading ? null : _start,
          icon: const Icon(Icons.play_arrow),
          label: Text(
            loading ? context.strings.openingGame : context.strings.startGame,
            maxLines: 1,
            overflow: TextOverflow.ellipsis,
          ),
        );
        return Row(
          spacing: 8,
          children: [
            if (compact) Expanded(child: play) else play,
            if (!compact && !widget.multiplayer && metricsInActions)
              _metrics(context, save),
            if (!compact) const Spacer(),
            if (game.data != null) import,
            settings,
            if (!compact)
              FavoriteButton(book: widget.book, game: game, outlined: true),
          ],
        );
      },
    );
  }

  Widget _metrics(BuildContext context, CloudGameSave? save) {
    final compact = MediaQuery.sizeOf(context).width <= launcherCompactWidth;
    final duration = save?.statistics.playtime;
    final playedAt = widget.book.history?.lastPlayed(widget.game.id);
    final date = playedAt == null
        ? '—'
        : MaterialLocalizations.of(context)
              .formatShortMonthDay(playedAt.toLocal());
    final style = Theme.of(context).textTheme.bodySmall?.copyWith(fontSize: 12);
    final values = Wrap(
      spacing: compact ? 10 : 14,
      runSpacing: 5,
      children: [
        for (final (label, value) in [
          (context.strings.lastPlayed, date),
          (
            context.strings.playTime,
            duration == null ? '—' : context.strings.formatPlaytime(duration),
          ),
        ])
          Column(
            crossAxisAlignment: CrossAxisAlignment.start,
            children: [
              Text(label, style: style),
              Text(value, style: style),
            ],
          ),
      ],
    );
    return Row(
      mainAxisSize: MainAxisSize.min,
      spacing: compact ? 8 : 12,
      children: [
        if (save != null)
          Tooltip(
            message: context.strings.cloudSaves,
            child: Icon(LucideIcons.cloudCheck, size: compact ? 18 : 21),
          ),
        Flexible(child: values),
      ],
    );
  }

  Widget _detailContent(Game game, CloudGameSave? save) {
    final statistics = _statisticsSection(game, save);
    final owner = _owner;
    if (owner == null) return statistics;
    final friends = GameFriends(
      key: ValueKey((owner, game.id)),
      session: widget.session,
      owner: owner,
      game: game.id,
    );
    return LayoutBuilder(
      builder: (context, constraints) {
        final beside = constraints.maxWidth > 740;
        final divider = BorderSide(
          color: Theme.of(context).colorScheme.outlineVariant,
        );
        return Flex(
          direction: beside ? Axis.horizontal : Axis.vertical,
          spacing: 16,
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            Flexible(flex: beside ? 1 : 0, child: statistics),
            SizedBox(
              width: beside ? 184 : double.infinity,
              child: DecoratedBox(
                decoration: BoxDecoration(
                  border: BorderDirectional(
                    start: beside ? divider : BorderSide.none,
                    top: beside ? BorderSide.none : divider,
                  ),
                ),
                child: Padding(
                  padding: EdgeInsetsDirectional.only(
                    start: beside ? 16 : 0,
                    top: beside ? 0 : 12,
                  ),
                  child: friends,
                ),
              ),
            ),
          ],
        );
      },
    );
  }

  Widget _statisticsSection(Game game, CloudGameSave? save) {
    final stats = save?.statistics;
    return Column(
      crossAxisAlignment: CrossAxisAlignment.stretch,
      spacing: 10,
      children: [
        Row(
          children: [
            Expanded(
              child: Text(
                context.strings.clearProgress,
                style: launcherSectionTitle(context),
              ),
            ),
            if (save != null)
              Tooltip(
                message: MaterialLocalizations.of(context)
                    .formatCompactDate(save.uploadedAt.toLocal()),
                child: Text(context.strings.cloudUpload),
              ),
          ],
        ),
        GameClearMatrix(
          key: ValueKey(game.id),
          game: game.id,
          progress: stats?.progress ?? const [],
          details: stats != null && spellStageCatalog.containsKey(game.id)
              ? (selection, label) => GameSpellArchive(
                  statistics: stats,
                  selection: selection,
                  loadoutLabel: label,
                )
              : null,
        ),
        if (_cloud == _CloudState.guest)
          Align(
            alignment: AlignmentDirectional.centerStart,
            child: OutlinedButton(
              style: LauncherControl.style(context),
              onPressed: () =>
                  openSiteLink(context, signInFor('/?game=${game.id}')),
              child: Text(context.strings.signIn),
            ),
          )
        else if (_cloud == _CloudState.loading)
          LinearProgressIndicator(
            semanticsLabel: context.strings.cloudProgressLoading,
          )
        else if (_cloud == _CloudState.error)
          Align(
            alignment: AlignmentDirectional.centerStart,
            child: TextButton.icon(
              onPressed: () => _load(_owner),
              icon: const Icon(Icons.refresh),
              label: Text(context.strings.cloudReadFailed),
            ),
          )
        else if (save == null)
          Text(context.strings.cloudNoSave),
      ],
    );
  }
}
