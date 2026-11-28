import 'package:flutter/rendering.dart';
import 'package:go_router/go_router.dart';
import 'package:intl/intl.dart';
import 'package:material_ui/material_ui.dart';

import 'account.dart';
import 'account_api.dart';
import 'catalog.dart';
import 'game_clear_matrix.dart';
import 'game_launcher.dart';
import 'library.dart';
import 'launcher_select.dart';
import 'profile_data.dart';
import 'profile_identity.dart';
import 'profile_loading.dart';
import 'l10n/strings.dart';
import 'th09_rating_chart.dart';
import 'th09_duel_replay_list.dart';
import 'th09_player_card.dart';
import 'th09_ratings.dart';

String _winRate(LauncherStrings strings, int wins, int games) =>
    strings.th09BoardWinRate(
      (NumberFormat.percentPattern(
        strings.localeName,
      )..maximumFractionDigits = 1).format(wins / games),
    );

class ProfileGameData extends StatefulWidget {
  const ProfileGameData({
    super.key,
    required this.api,
    required this.profile,
    required this.games,
    required this.owner,
    required this.viewer,
    required this.launcher,
    this.cloudRevision,
    this.initialCharacter,
  });

  final AccountApi api;
  final PublicProfile profile;
  final List<Game> games;
  final bool owner;
  final String? viewer;
  final GameLaunchService launcher;
  final Listenable? cloudRevision;
  final int? initialCharacter;

  @override
  State<ProfileGameData> createState() => _ProfileGameDataState();
}

class _ProfileGameDataState extends State<ProfileGameData> {
  final _content = GlobalKey();
  late ProfileGames _selected =
      widget.initialCharacter != null && widget.profile.showVersus
      ? ProfileGames.th09
      : _views.contains(widget.profile.defaultView)
      ? widget.profile.defaultView
      : _views.first;
  Future<ProfileStatistics>? _statistics;
  DuelHistory? _duels;
  bool _loadingDuels = false;
  bool _expandedDuels = false;
  bool _expandedCharacters = false;
  late int? _character = widget.initialCharacter;
  bool _duelsFailed = false;
  double _minimumHeight = 0;
  Game? _work;

  List<ProfileGames> get _views => [
    if (widget.profile.showVersus) ProfileGames.th09,
    if (widget.profile.showCareer || widget.profile.showRecords)
      ProfileGames.all,
  ];

  @override
  void initState() {
    super.initState();
    _loadSelected();
    widget.cloudRevision?.addListener(_onNewSave);
  }

  void _onNewSave() {
    if (_selected == ProfileGames.all) {
      final latest = _readStatistics();
      latest.ignore();
      setState(() => _statistics = latest);
    } else {
      _statistics = null;
    }
  }

  @override
  void dispose() {
    widget.cloudRevision?.removeListener(_onNewSave);
    super.dispose();
  }

  void _loadSelected() {
    if (_selected == ProfileGames.th09 && _duels == null) {
      _loadDuels();
    } else if (_selected == ProfileGames.all) {
      _statistics ??= _readStatistics();
    }
  }

  Future<ProfileStatistics> _readStatistics() => widget.api.profileStatistics(
    widget.profile.username,
    owner: widget.owner,
  );

  Future<void> _loadDuels({bool more = false}) async {
    if (_loadingDuels) return;
    setState(() {
      _loadingDuels = true;
      _duelsFailed = false;
    });
    try {
      final page = await widget.api.duels(
        widget.profile.username,
        owner: widget.owner,
        cursor: more ? _duels?.nextCursor : null,
      );
      if (!mounted) return;
      setState(() => _duels = more ? _duels?.append(page) ?? page : page);
    } catch (_) {
      if (mounted) setState(() => _duelsFailed = true);
    } finally {
      if (mounted) setState(() => _loadingDuels = false);
    }
  }

  void _select(ProfileGames view) {
    if (view == _selected) return;
    setState(() {
      _minimumHeight = _visiblePaneHeight();
      _selected = view;
    });
    _loadSelected();
  }

  void _selectWork(Game? work) {
    if (_work == work) return;
    setState(() {
      _minimumHeight = _visiblePaneHeight();
      _work = work;
    });
  }

  double _visiblePaneHeight() {
    final pane = _content.currentContext;
    final render = pane?.findRenderObject();
    final position = pane == null ? null : Scrollable.maybeOf(pane)?.position;
    final viewport = RenderAbstractViewport.maybeOf(render);
    return render != null &&
            viewport != null &&
            position != null &&
            position.pixels > 0
        ? (position.pixels +
                  position.viewportDimension -
                  viewport.getOffsetToReveal(render, 0).offset)
              .clamp(0.0, position.viewportDimension)
        : 0.0;
  }

  Widget _loading() =>
      ProfilePanelLoading(label: context.strings.profileLoading);

  @override
  Widget build(BuildContext context) {
    final theme = Theme.of(context);
    final strings = context.strings;
    final th09 = widget.games.where((game) => game.id == 'th09').firstOrNull;
    return Column(
      crossAxisAlignment: CrossAxisAlignment.stretch,
      spacing: 12,
      children: [
        Wrap(
          alignment: WrapAlignment.spaceBetween,
          crossAxisAlignment: WrapCrossAlignment.center,
          spacing: 12,
          runSpacing: 8,
          children: [
            Text(strings.profileGameData, style: theme.textTheme.titleMedium),
            if (_views.length > 1)
              LauncherSegments<ProfileGames>(
                segments: [
                  ButtonSegment(
                    value: ProfileGames.th09,
                    icon: th09 == null
                        ? null
                        : GameArtwork(game: th09, size: 18),
                    label: Text(strings.th09ProfileLabel),
                  ),
                  ButtonSegment(
                    value: ProfileGames.all,
                    icon: const Icon(Icons.grid_view),
                    label: Text(strings.profileGameDataAll),
                  ),
                ],
                value: _selected,
                onChanged: _select,
              ),
          ],
        ),
        ConstrainedBox(
          key: _content,
          constraints: BoxConstraints(minHeight: _minimumHeight),
          child: _selected == ProfileGames.th09
              ? _duelPane()
              : FutureBuilder<ProfileStatistics>(
                  future: _statistics,
                  builder: (context, snapshot) {
                    if (snapshot.hasError) {
                      return Align(
                        alignment: Alignment.topLeft,
                        child: TextButton.icon(
                          onPressed: () {
                            final read = _readStatistics();
                            setState(() {
                              _statistics = read;
                            });
                          },
                          icon: const Icon(Icons.refresh),
                          label: Text(strings.profileDataRetry),
                        ),
                      );
                    }
                    final data = snapshot.data;
                    return data == null
                        ? _loading()
                        : _ProfileCareer(
                            games: widget.games,
                            profile: widget.profile,
                            statistics: data,
                            work: _work,
                            onSelectWork: _selectWork,
                          );
                  },
                ),
        ),
      ],
    );
  }

  Widget _duelPane() {
    final history = _duels;
    final game = widget.games.where((game) => game.id == 'th09').firstOrNull;
    if (history == null && _loadingDuels) return _loading();
    final standings = history?.characterRatings ?? const [];
    final comparable = standings.where((entry) => entry.history.isNotEmpty);
    final selected = comparable
        .where((entry) => entry.character == _character)
        .firstOrNull;
    final scores = {for (final entry in standings) entry.character: entry};
    final results = [...?history?.characters];
    results.sort((a, b) {
      final games = b.games.compareTo(a.games);
      return games == 0 ? a.character.compareTo(b.character) : games;
    });
    final byCharacter = {for (final entry in results) entry.character: entry};
    final characters = {...byCharacter.keys, ...scores.keys};
    final theme = Theme.of(context);
    final strings = context.strings;
    final numbers = NumberFormat.decimalPattern(strings.localeName);
    final dates = MaterialLocalizations.of(context);
    final rounds = _expandedDuels
        ? history?.rounds ?? const <DuelRecord>[]
        : history?.rounds.take(2).toList() ?? const <DuelRecord>[];
    String day(DuelRecord duel) => dates.formatFullDate(duel.playedAt);
    void choose(int? character) {
      if (MediaQuery.sizeOf(context).width > 780 || character == null) {
        setState(() => _character = character);
        return;
      }
      final score = scores[character];
      if (score == null || history == null) return;
      showModalBottomSheet<void>(
        context: context,
        useSafeArea: true,
        showDragHandle: true,
        isScrollControlled: true,
        builder: (sheet) => SafeArea(
          top: false,
          child: SingleChildScrollView(
            padding: const EdgeInsets.fromLTRB(12, 0, 12, 12),
            child: Th09RatingChart(
              points: history.ratingHistory,
              ranks: history.rankHistory,
              character: score,
              onCloseCharacter: () => Navigator.of(sheet).pop(),
            ),
          ),
        ),
      );
    }

    return Column(
      crossAxisAlignment: CrossAxisAlignment.stretch,
      spacing: 12,
      children: [
        if (history != null &&
            (history.ratingHistory.isNotEmpty ||
                history.rankHistory.isNotEmpty ||
                history.ratingPreview != null ||
                selected != null))
          Th09RatingChart(
            points: history.ratingHistory,
            ranks: history.rankHistory,
            preview: history.ratingPreview,
            character: selected,
            onCloseCharacter: () => choose(null),
          ),
        if (history != null &&
            history.games == 0 &&
            results.isEmpty &&
            standings.isEmpty)
          Text(strings.th09NoMatches),
        if (history != null && history.games > 0)
          Wrap(
            spacing: 16,
            runSpacing: 4,
            children: [
              Text(
                strings.th09MatchSummary(
                  numbers.format(history.wins),
                  numbers.format(history.games),
                ),
                style: theme.textTheme.titleMedium,
              ),
              Text(_winRate(strings, history.wins, history.games)),
              if (history.durationFrames case final int frames)
                Text(
                  strings.th09MatchDuration(
                    strings.formatPlaytime(frames / 60),
                  ),
                ),
            ],
          ),
        if (results.isNotEmpty || standings.isNotEmpty) ...[
          Wrap(
            alignment: WrapAlignment.spaceBetween,
            crossAxisAlignment: WrapCrossAlignment.center,
            runSpacing: 4,
            children: [
              Text(
                strings.th09CharacterRecord,
                style: theme.textTheme.titleMedium,
              ),
              if (comparable.isNotEmpty)
                LauncherSelect<int?>(
                  label: strings.th09CharacterRecord,
                  value: selected?.character,
                  onChanged: choose,
                  labels: {
                    null: strings.th09AllCharacters,
                    for (final entry in comparable)
                      entry.character: th09CharacterLabel(
                        entry.character,
                        strings.localeName,
                      ),
                  },
                ),
            ],
          ),
          LayoutBuilder(
            builder: (context, box) {
              final scale = MediaQuery.textScalerOf(context).scale(14) / 14;
              final adjusted = box.maxWidth / scale;
              final columns = adjusted >= 650
                  ? 3
                  : adjusted >= 348
                  ? 2
                  : 1;
              const gap = 10.0;
              final width = (box.maxWidth - (columns - 1) * gap) / columns;
              return Wrap(
                spacing: gap,
                runSpacing: gap,
                children: [
                  for (final id
                      in _expandedCharacters ? characters : characters.take(3))
                    SizedBox(
                      width: width,
                      child: _characterCard(
                        context,
                        id,
                        byCharacter[id],
                        scores[id],
                        selected?.character == id,
                        choose,
                      ),
                    ),
                ],
              );
            },
          ),
          if (characters.length > 3)
            Align(
              alignment: AlignmentDirectional.centerEnd,
              child: TextButton(
                onPressed: () =>
                    setState(() => _expandedCharacters = !_expandedCharacters),
                child: Text(
                  _expandedCharacters
                      ? strings.th09CollapseCharacters
                      : strings.th09ExpandCharacters,
                ),
              ),
            ),
        ],
        if (history != null && history.games > 0) ...[
          Text(strings.th09MatchHistory, style: theme.textTheme.titleMedium),
          if (rounds.isEmpty) Text(strings.th09NoMatches),
          for (final (index, duel) in rounds.indexed) ...[
            if (index == 0 || day(rounds[index - 1]) != day(duel))
              Text(day(duel), style: theme.textTheme.labelMedium),
            Card(
              key: ValueKey(duel.id),
              margin: EdgeInsets.zero,
              child: Column(
                crossAxisAlignment: CrossAxisAlignment.stretch,
                children: [
                  ListTile(
                    contentPadding: const EdgeInsets.symmetric(horizontal: 10),
                    leading: _opponentAvatar(duel),
                    title: Text(
                      duel.name ?? strings.th09Guest,
                      maxLines: 1,
                      overflow: TextOverflow.ellipsis,
                    ),
                    subtitle: duel.opponent == null
                        ? null
                        : Text(
                            '@${duel.opponent}',
                            maxLines: 1,
                            overflow: TextOverflow.ellipsis,
                          ),
                    trailing: _DuelScore(duel: duel),
                    onTap:
                        duel.opponent == null ||
                            !isPublicUsername(duel.opponent!)
                        ? null
                        : () => context.push(profilePath(duel.opponent!)),
                  ),
                  if (game != null && duel.replayCount > 0)
                    Padding(
                      padding: const EdgeInsets.symmetric(horizontal: 10),
                      child: Th09DuelReplayList(
                        key: ValueKey((duel.id, widget.viewer)),
                        api: widget.api,
                        duel: duel.id,
                        count: duel.replayCount,
                        game: game,
                        launcher: widget.launcher,
                      ),
                    ),
                ],
              ),
            ),
          ],
        ],
        if ((history?.rounds.length ?? 0) > 2)
          Align(
            alignment: AlignmentDirectional.centerEnd,
            child: TextButton(
              onPressed: () => setState(() => _expandedDuels = !_expandedDuels),
              child: Text(
                _expandedDuels
                    ? strings.th09LessMatches
                    : strings.th09MoreMatches,
              ),
            ),
          ),
        if (_loadingDuels) const LinearProgressIndicator(),
        if (_duelsFailed ||
            history?.nextCursor != null &&
                (_expandedDuels || history!.rounds.length <= 2))
          Align(
            alignment: AlignmentDirectional.centerEnd,
            child: TextButton(
              onPressed: _loadingDuels
                  ? null
                  : () {
                      if (history != null && !_expandedDuels) {
                        setState(() => _expandedDuels = true);
                      }
                      _loadDuels(more: history != null);
                    },
              child: Text(
                _duelsFailed
                    ? strings.th09HistoryRetry
                    : strings.th09LoadMoreMatches,
              ),
            ),
          ),
      ],
    );
  }

  Widget _opponentAvatar(DuelRecord duel) {
    final username = duel.opponent;
    final avatar = AccountAvatar(api: widget.api, username: username, size: 36);
    if (username == null || !isPublicUsername(username)) return avatar;
    return Th09PlayerCardButton(
      api: widget.api,
      username: username,
      name: duel.name ?? username,
      scores: null,
      child: avatar,
    );
  }

  Widget _characterCard(
    BuildContext context,
    int id,
    Th09CharacterResult? record,
    Th09CharacterRating? score,
    bool selected,
    ValueChanged<int?> choose,
  ) {
    final theme = Theme.of(context);
    final strings = context.strings;
    final numbers = NumberFormat.decimalPattern(strings.localeName);
    final opensChart = score?.history.isNotEmpty ?? false;
    final name = th09CharacterLabel(id, strings.localeName);
    return Semantics(
      button: opensChart,
      selected: opensChart ? selected : null,
      child: Material(
        color: selected
            ? theme.colorScheme.primaryContainer
            : Colors.transparent,
        borderRadius: BorderRadius.circular(8),
        child: InkWell(
          borderRadius: BorderRadius.circular(8),
          onTap: opensChart ? () => choose(selected ? null : id) : null,
          child: Padding(
            padding: const EdgeInsets.all(8),
            child: Row(
              spacing: 8,
              children: [
                Th09CharacterPortrait(id),
                Expanded(
                  child: Column(
                    crossAxisAlignment: CrossAxisAlignment.start,
                    spacing: 2,
                    children: [
                      Text(
                        name,
                        style: theme.textTheme.titleSmall,
                        maxLines: 1,
                        overflow: TextOverflow.ellipsis,
                      ),
                      Text(
                        [
                          strings.th09BoardGamesCount(
                            numbers.format(record?.games ?? score!.games),
                          ),
                          if (record != null)
                            _winRate(strings, record.wins, record.games),
                        ].join(' | '),
                        style: theme.textTheme.bodySmall,
                      ),
                      if (score != null)
                        Text(
                          '${strings.th09BoardCharacterScore} ${numbers.format(score.score)}',
                          style: theme.textTheme.labelMedium?.copyWith(
                            color: opensChart
                                ? theme.colorScheme.primary
                                : null,
                          ),
                        ),
                    ],
                  ),
                ),
              ],
            ),
          ),
        ),
      ),
    );
  }
}

class _DuelScore extends StatelessWidget {
  const _DuelScore({required this.duel});

  final DuelRecord duel;

  @override
  Widget build(BuildContext context) {
    final theme = Theme.of(context);
    final strings = context.strings;
    final numbers = NumberFormat.decimalPattern(strings.localeName);
    final result = duel.wins.compareTo(duel.losses);
    final label = result > 0
        ? strings.profileMatchWin
        : result < 0
        ? strings.profileMatchLoss
        : strings.profileMatchDraw;
    final score =
        '${numbers.format(duel.wins)} : ${numbers.format(duel.losses)}';
    final color = result > 0
        ? theme.colorScheme.tertiary
        : result < 0
        ? theme.colorScheme.error
        : theme.colorScheme.onSurfaceVariant;
    return Column(
      mainAxisAlignment: MainAxisAlignment.center,
      crossAxisAlignment: CrossAxisAlignment.end,
      children: [
        Row(
          mainAxisSize: MainAxisSize.min,
          spacing: 6,
          children: [
            Text(
              label,
              style: theme.textTheme.labelMedium?.copyWith(color: color),
            ),
            Text(
              score,
              style: theme.textTheme.titleMedium?.copyWith(color: color),
            ),
          ],
        ),
        if (duel.durationFrames case final int frames)
          Text(
            strings.formatPlaytime(frames / 60),
            style: theme.textTheme.labelSmall,
          ),
      ],
    );
  }
}

class _ProfileCareer extends StatelessWidget {
  const _ProfileCareer({
    required this.games,
    required this.profile,
    required this.statistics,
    required this.work,
    required this.onSelectWork,
  });
  final List<Game> games;
  final PublicProfile profile;
  final ProfileStatistics statistics;
  final Game? work;
  final ValueChanged<Game?> onSelectWork;

  @override
  Widget build(BuildContext context) {
    final theme = Theme.of(context);
    final strings = context.strings;
    if (work case final Game game) return _workDetail(game, strings);
    final career =
        profile.showCareer &&
        statistics.career &&
        games.any(
          (game) => statistics.games[game.id]?.progress.isNotEmpty == true,
        );
    final records = profile.showRecords && statistics.records;
    final played =
        games
            .where((game) => (statistics.games[game.id]?.playtime ?? 0) > 0)
            .toList()
          ..sort(
            (a, b) => statistics.games[b.id]!.playtime!.compareTo(
              statistics.games[a.id]!.playtime!,
            ),
          );
    final ordered = [
      ...games.where(
        (game) => statistics.games[game.id]?.progress.isNotEmpty == true,
      ),
      ...games.where(
        (game) => statistics.games[game.id]?.progress.isNotEmpty != true,
      ),
    ];
    if (!career && (!records || played.isEmpty)) {
      return Text(strings.profileGameDataEmpty);
    }
    return Column(
      crossAxisAlignment: CrossAxisAlignment.stretch,
      spacing: 12,
      children: [
        if (career) ...[
          Text(strings.profileCareerTitle, style: theme.textTheme.titleMedium),
          WorkProgressTable(
            rows: [
              for (final game in ordered)
                (
                  identity: _WorkIdentity(
                    game: game,
                    details:
                        statistics.games[game.id]?.progress.isNotEmpty == true
                        ? () => onSelectWork(game)
                        : null,
                  ),
                  data: statistics.games[game.id]?.progress ?? [],
                ),
            ],
          ),
        ],
        if (records && played.isNotEmpty) _playtime(played, theme, strings),
      ],
    );
  }

  Widget _playtime(
    List<Game> played,
    ThemeData theme,
    LauncherStrings strings,
  ) {
    const fills = [
      Color(0xFFE9A9D7),
      Color(0xFFA3DABE),
      Color(0xFFCCB5F7),
      Color(0xFFEDAAAB),
    ];
    final timeStyle = theme.textTheme.bodySmall!.copyWith(
      fontSize: 12,
      fontWeight: FontWeight.w500,
    );
    return Card.outlined(
      margin: EdgeInsets.zero,
      shape: RoundedRectangleBorder(
        borderRadius: BorderRadius.circular(10),
        side: BorderSide(color: theme.colorScheme.outlineVariant),
      ),
      child: Padding(
        padding: const EdgeInsets.all(12),
        child: LayoutBuilder(
          builder: (context, box) {
            final compact = box.maxWidth < 600;
            final workWidth =
                (compact ? 104.0 : 120.0) *
                MediaQuery.textScalerOf(context).scale(12) /
                12;
            final gap = compact ? 8.0 : 10.0;
            var timeWidth = compact ? 76.0 : 94.0;
            for (final game in played) {
              final text = TextPainter(
                text: TextSpan(
                  text: strings.formatPlaytime(
                    statistics.games[game.id]!.playtime!,
                  ),
                  style: timeStyle,
                ),
                textDirection: Directionality.of(context),
                textScaler: MediaQuery.textScalerOf(context),
                maxLines: 1,
              )..layout();
              if (text.width > timeWidth) timeWidth = text.width.ceilToDouble();
              text.dispose();
            }
            final stacked = box.maxWidth - workWidth - timeWidth - 2 * gap < 28;
            return Column(
              spacing: 8,
              children: [
                Row(
                  spacing: 8,
                  children: [
                    Expanded(
                      child: Text(
                        strings.profileTotalPlaytime,
                        style: theme.textTheme.bodyMedium,
                      ),
                    ),
                    if (statistics.playtime case final num time)
                      Text(strings.formatPlaytime(time), style: timeStyle),
                  ],
                ),
                for (final (index, game) in played.indexed)
                  Builder(
                    builder: (context) {
                      final time = statistics.games[game.id]!.playtime!;
                      final bar = LinearProgressIndicator(
                        value:
                            time / statistics.games[played.first.id]!.playtime!,
                        minHeight: 12,
                        borderRadius: BorderRadius.circular(3),
                        trackGap: 0,
                        stopIndicatorRadius: 0,
                        color: fills[index % fills.length],
                        backgroundColor:
                            theme.colorScheme.surfaceContainerHighest,
                        semanticsLabel:
                            '${game.title} | ${strings.formatPlaytime(time)}',
                      );
                      final identity = _WorkIdentity(
                        game: game,
                        details: () => onSelectWork(game),
                        compact: true,
                      );
                      final duration = SizedBox(
                        width: timeWidth,
                        child: Text(
                          strings.formatPlaytime(time),
                          style: timeStyle,
                          textAlign: TextAlign.end,
                        ),
                      );
                      if (stacked) {
                        return Column(
                          crossAxisAlignment: CrossAxisAlignment.stretch,
                          children: [
                            Row(
                              spacing: gap,
                              children: [
                                Expanded(child: identity),
                                duration,
                              ],
                            ),
                            bar,
                          ],
                        );
                      }
                      return Row(
                        spacing: gap,
                        children: [
                          SizedBox(width: workWidth, child: identity),
                          Expanded(child: bar),
                          duration,
                        ],
                      );
                    },
                  ),
              ],
            );
          },
        ),
      ),
    );
  }

  Widget _workDetail(Game game, LauncherStrings strings) {
    final stats = statistics.games[game.id];
    return PopScope(
      canPop: false,
      onPopInvokedWithResult: (didPop, _) {
        if (!didPop) onSelectWork(null);
      },
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.stretch,
        spacing: 10,
        children: [
          Row(
            spacing: 8,
            children: [
              IconButton(
                tooltip: strings.profileBackAllGames,
                onPressed: () => onSelectWork(null),
                icon: const Icon(Icons.arrow_back),
              ),
              GameArtwork(game: game, size: 30),
              Expanded(child: Text(game.title)),
            ],
          ),
          if (profile.showCareer && statistics.career)
            GameClearMatrix(
              key: ValueKey(game.id),
              game: game.id,
              progress: stats?.progress ?? const [],
              showScores: profile.showRecords && statistics.records,
            ),
          if (profile.showRecords &&
              statistics.records &&
              stats?.playtime != null)
            ListTile(
              title: Text(strings.profileTotalPlaytime),
              trailing: Text(strings.formatPlaytime(stats!.playtime!)),
            ),
        ],
      ),
    );
  }
}

class _WorkIdentity extends StatelessWidget {
  const _WorkIdentity({
    required this.game,
    required this.details,
    this.compact = false,
  });
  final Game game;
  final VoidCallback? details;
  final bool compact;

  @override
  Widget build(BuildContext context) => Tooltip(
    message: game.title,
    child: TextButton(
      style: TextButton.styleFrom(
        padding: EdgeInsets.symmetric(horizontal: compact ? 0 : 4),
        foregroundColor: compact
            ? Theme.of(context).colorScheme.onSurface
            : null,
        textStyle: compact
            ? Theme.of(context).textTheme.bodySmall!.copyWith(
                fontSize: MediaQuery.sizeOf(context).width > 780 ? 13 : 12,
                fontWeight: FontWeight.w400,
              )
            : null,
      ),
      onPressed: details,
      child: Row(
        spacing: compact ? 5 : 6,
        children: [
          GameArtwork(
            game: game,
            size: compact ? 25 : 24,
            borderRadius: BorderRadius.all(Radius.circular(compact ? 3 : 12)),
          ),
          Expanded(
            child: Text(
              game.title,
              maxLines: 1,
              overflow: TextOverflow.ellipsis,
            ),
          ),
        ],
      ),
    ),
  );
}
