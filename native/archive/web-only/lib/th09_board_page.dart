import 'dart:async';

import 'package:fl_chart/fl_chart.dart';
import 'package:go_router/go_router.dart';
import 'package:intl/intl.dart';
import 'package:material_ui/material_ui.dart';
import 'package:super_sliver_list/super_sliver_list.dart';

import 'account.dart';
import 'account_api.dart';
import 'l10n/strings.dart';
import 'launcher_select.dart';
import 'profile_data.dart';
import 'profile_identity.dart';
import 'profile_social.dart';
import 'th09_rating_chart.dart';
import 'th09_player_card.dart';
import 'th09_ratings.dart';

typedef _Standing = ({
  Th09BoardPlayer player,
  Th09CharacterRating? character,
  int position,
});

enum _Sort {
  position(38, numeric: true, ascending: true),
  name(0, ascending: true),
  score(70, numeric: true),
  interval(78, numeric: true),
  overall(58, numeric: true),
  games(58, numeric: true),
  winRate(64, numeric: true),
  group(76, ascending: true),
  lastPlayed(98, numeric: true),
  status(84, ascending: true);

  const _Sort(this.width, {this.numeric = false, this.ascending = false});

  final double width;
  final bool numeric;
  final bool ascending;

  static Iterable<_Sort> forBoard({required bool character}) =>
      values.where((column) => character || column != overall);

  String label(LauncherStrings strings, {required bool character}) =>
      switch (this) {
        score =>
          character ? strings.th09BoardCharacterScore : strings.th09BoardRating,
        overall => strings.th09BoardRating,
        position => strings.th09BoardRank,
        name => strings.th09BoardPlayer,
        games => strings.th09BoardGames,
        interval =>
          character
              ? strings.th09BoardCharacterDeviation
              : strings.th09BoardDeviation,
        lastPlayed => strings.th09BoardLastGame,
        winRate => strings.th09BoardWinRateColumn,
        group => strings.th09BoardGroup,
        status => strings.th09BoardStatus,
      };
}

class Th09BoardPage extends StatefulWidget {
  const Th09BoardPage({super.key, required this.api, this.initialCharacter});

  final AccountApi api;
  final int? initialCharacter;

  @override
  State<Th09BoardPage> createState() => _Th09BoardPageState();
}

class _Th09BoardPageState extends State<Th09BoardPage>
    with WidgetsBindingObserver {
  final _search = TextEditingController();
  final _minimum = TextEditingController();
  final _scroll = ScrollController(keepScrollOffset: false);
  final _rows = ListController();
  final _locatedFocus = FocusNode();
  final _sortFocus = FocusNode();
  Th09Board? _board;
  bool _loading = false;
  bool _requesting = false;
  bool _foreground = true;
  bool _visible = false;
  late final Timer _refreshTimer;
  bool _failed = false;
  Th09RatingStatus? _status = Th09RatingStatus.active;
  int? _group;
  late int? _character = widget.initialCharacter;
  _Sort _sort = _Sort.score;
  bool _descending = true;
  bool _wide = false;
  String? _locatedPlayer;

  @override
  void initState() {
    super.initState();
    final binding = WidgetsBinding.instance;
    binding.addObserver(this);
    final lifecycle = binding.lifecycleState;
    _foreground = lifecycle == null || lifecycle == AppLifecycleState.resumed;
    _refreshTimer = Timer.periodic(
      th09RatingsRefreshPeriod,
      (_) => _refreshVisible(),
    );
    unawaited(_refresh());
  }

  @override
  void didChangeDependencies() {
    super.didChangeDependencies();
    final wasVisible = _visible;
    _visible =
        TickerMode.valuesOf(context).enabled &&
        ModalRoute.isCurrentOf(context) != false;
    if (_visible && !wasVisible) {
      WidgetsBinding.instance.addPostFrameCallback((_) {
        if (mounted) _refreshVisible();
      });
    }
    _updateBoardLayout();
  }

  void _updateBoardLayout() {
    final media = MediaQuery.of(context);
    final scale = media.textScaler.scale(12) / 12;
    final wide =
        scale <= 1.35 &&
        media.size.width - media.padding.horizontal >
            800 * scale.clamp(1, 1.35);
    if (wide == _wide) return;
    _wide = wide;

    final range = _rows.isAttached ? _rows.visibleRange : null;
    final focusedPlayer = _locatedFocus.hasPrimaryFocus ? _locatedPlayer : null;
    if (focusedPlayer == null && (range == null || range.$1 == 0)) return;
    WidgetsBinding.instance.addPostFrameCallback((_) {
      if (!mounted || _wide != wide) return;
      if (focusedPlayer != null && focusedPlayer == _locatedPlayer) {
        _jumpToPlayer(focusedPlayer);
      } else if (range != null && range.$1 > 0) {
        _jumpToRow(range.$1, alignment: 0);
      }
    });
  }

  @override
  void didChangeAppLifecycleState(AppLifecycleState state) {
    _foreground = state == AppLifecycleState.resumed;
    _refreshVisible();
  }

  @override
  void dispose() {
    WidgetsBinding.instance.removeObserver(this);
    _refreshTimer.cancel();
    _search.dispose();
    _minimum.dispose();
    _locatedFocus.dispose();
    _sortFocus.dispose();
    _rows.dispose();
    _scroll.dispose();
    super.dispose();
  }

  void _refreshVisible() {
    if (_visible && _foreground) unawaited(_refresh(background: true));
  }

  Future<void> _refresh({bool background = false}) async {
    if (_requesting) return;
    _requesting = true;
    if (!background) {
      setState(() {
        _loading = true;
        _failed = false;
      });
    }
    try {
      final board = await widget.api.th09Board();
      if (mounted) {
        setState(() {
          _board = board;
          _failed = false;
        });
      }
    } catch (_) {
      if (mounted && _board == null) setState(() => _failed = true);
      if (mounted && _board != null && !background) {
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(content: Text(context.strings.th09BoardFailed)),
        );
      }
    } finally {
      _requesting = false;
      if (mounted && _loading) setState(() => _loading = false);
    }
  }

  int _score(_Standing row) => row.character?.score ?? row.player.rating;
  int _games(_Standing row) => row.character?.games ?? row.player.games;
  int _interval(_Standing row) =>
      row.character?.interval ?? row.player.interval;

  double _winRate(Th09BoardPlayer player) =>
      player.games == 0 ? 0 : player.wins / player.games;

  int _compare(_Standing a, _Standing b) {
    final order = switch (_sort) {
      _Sort.score => _score(a).compareTo(_score(b)),
      _Sort.games => _games(a).compareTo(_games(b)),
      _Sort.interval => _interval(a).compareTo(_interval(b)),
      _Sort.position => a.position.compareTo(b.position),
      _Sort.name => a.player.nickname.toLowerCase().compareTo(
        b.player.nickname.toLowerCase(),
      ),
      _Sort.lastPlayed => a.player.lastPlayed.compareTo(b.player.lastPlayed),
      _Sort.overall => a.player.rating.compareTo(b.player.rating),
      _Sort.winRate => _winRate(a.player).compareTo(_winRate(b.player)),
      _Sort.group => a.player.group.compareTo(b.player.group),
      _Sort.status => a.player.status.name.compareTo(b.player.status.name),
    };
    return order == 0
        ? a.position.compareTo(b.position)
        : _descending
        ? -order
        : order;
  }

  void _sortBy(_Sort value) {
    final located = _locatedPlayer;
    setState(() {
      _descending = value == _sort ? !_descending : !value.ascending;
      _sort = value;
    });
    if (located != null) _locatePlayer(located);
  }

  List<_Standing> _rank(Th09Board board) {
    final character = _character;
    if (character == null) {
      return [
        for (final (index, player) in board.players.indexed)
          (player: player, character: null, position: index + 1),
      ];
    }
    final entries =
        [
          for (final player in board.players)
            if (player.characterRatings
                    .where((rating) => rating.character == character)
                    .firstOrNull
                case final Th09CharacterRating score)
              (player: player, character: score, position: 0),
        ]..sort((a, b) {
          final rating = b.character.score.compareTo(a.character.score);
          if (rating != 0) return rating;
          final games = b.character.games.compareTo(a.character.games);
          return games == 0 ? a.player.id.compareTo(b.player.id) : games;
        });
    return [
      for (final (index, row) in entries.indexed)
        (player: row.player, character: row.character, position: index + 1),
    ];
  }

  void _resetScroll() {
    _locatedPlayer = null;
    if (_scroll.hasClients) _scroll.jumpTo(0);
  }

  List<_Standing> _filteredStandings(Th09Board board) {
    final minimum = (int.tryParse(_minimum.text.trim()) ?? board.minimumGames)
        .clamp(1, 1000000);
    return _rank(board)
        .where(
          (row) =>
              _games(row) >= minimum &&
              (_character != null ||
                  _group == null ||
                  row.player.group == _group) &&
              (_status == null || row.player.status == _status),
        )
        .toList();
  }

  void _locatePlayer(String player) {
    FocusManager.instance.primaryFocus?.unfocus();
    setState(() => _locatedPlayer = player);
    WidgetsBinding.instance.addPostFrameCallback((_) => _jumpToPlayer(player));
  }

  void _jumpToPlayer(String player) {
    final board = _board;
    if (!mounted || _locatedPlayer != player || board == null) return;
    final ordered = _filteredStandings(board)..sort(_compare);
    final index = ordered.indexWhere((row) => row.player.id == player);
    if (!_jumpToRow(index, alignment: .5)) return;
    WidgetsBinding.instance.addPostFrameCallback((_) {
      if (mounted &&
          _locatedPlayer == player &&
          _locatedFocus.context != null) {
        _locatedFocus.requestFocus();
      }
    });
  }

  bool _jumpToRow(int index, {required double alignment}) {
    if (!_rows.isAttached ||
        !_scroll.hasClients ||
        index < 0 ||
        index >= _rows.numberOfItems) {
      return false;
    }
    _rows.jumpToItem(
      index: index,
      scrollController: _scroll,
      alignment: alignment,
    );
    return true;
  }

  String _statusLabel(Th09RatingStatus? status, LauncherStrings strings) =>
      switch (status) {
        Th09RatingStatus.active => strings.th09BoardActive,
        Th09RatingStatus.provisional => strings.th09BoardProvisional,
        Th09RatingStatus.inactive => strings.th09BoardInactive,
        null => strings.th09BoardAll,
      };

  void _clearFilters() {
    _search.clear();
    _minimum.text = '1';
    setState(() {
      _resetScroll();
      _status = null;
      _group = null;
    });
  }

  InputDecoration _fieldDecoration() {
    final colors = Theme.of(context).colorScheme;
    final border = OutlineInputBorder(
      borderRadius: LauncherControl.shape.borderRadius.resolve(
        Directionality.of(context),
      ),
      borderSide: BorderSide(color: colors.outlineVariant),
    );
    return InputDecoration(
      isDense: true,
      filled: true,
      fillColor: colors.surfaceContainerLow,
      contentPadding: const EdgeInsets.symmetric(horizontal: 10, vertical: 12),
      border: border,
      enabledBorder: border,
      focusedBorder: border.copyWith(
        borderSide: BorderSide(color: colors.primary),
      ),
      constraints: const BoxConstraints(minHeight: LauncherControl.height),
    );
  }

  void _openFilters(Th09Board board) {
    final groups = board.players.map((player) => player.group).toSet().toList()
      ..sort();
    showDialog<void>(
      context: context,
      builder: (context) => StatefulBuilder(
        builder: (context, updateDialog) {
          final strings = context.strings;
          void change(VoidCallback update) {
            setState(() {
              update();
              _resetScroll();
            });
            updateDialog(() {});
          }

          return AlertDialog(
            shape: LauncherControl.shape,
            title: Text(strings.th09BoardFilter),
            scrollable: true,
            content: SizedBox(
              width: 300,
              child: Column(
                crossAxisAlignment: CrossAxisAlignment.stretch,
                spacing: 8,
                children: [
                  TextField(
                    controller: _minimum,
                    keyboardType: TextInputType.number,
                    textAlignVertical: TextAlignVertical.center,
                    style: LauncherControl.label(context),
                    decoration: _fieldDecoration().copyWith(
                      labelText: strings.th09BoardMinimum,
                      floatingLabelBehavior: FloatingLabelBehavior.always,
                      hintText: board.minimumGames.toString(),
                    ),
                    onChanged: (_) => change(() {}),
                  ),
                  if (_character == null) ...[
                    Text(strings.th09BoardGroup),
                    LauncherSelect<int?>(
                      label: strings.th09BoardGroup,
                      value: _group,
                      labels: {
                        null: strings.th09BoardAll,
                        for (final group in groups)
                          group: strings.th09BoardGroupNumber(group.toString()),
                      },
                      onChanged: (group) => change(() => _group = group),
                      expand: true,
                    ),
                  ],
                  Text(strings.th09BoardStatus),
                  LauncherSelect<Th09RatingStatus?>(
                    label: strings.th09BoardStatus,
                    value: _status,
                    labels: {
                      for (final status in [null, ...Th09RatingStatus.values])
                        status: _statusLabel(status, strings),
                    },
                    onChanged: (status) => change(() => _status = status),
                    expand: true,
                  ),
                ],
              ),
            ),
            actions: [
              TextButton(
                onPressed: () {
                  _clearFilters();
                  updateDialog(() {});
                },
                child: Text(strings.th09BoardClear),
              ),
              TextButton(
                onPressed: () => Navigator.of(context).pop(),
                child: Text(strings.close),
              ),
            ],
          );
        },
      ),
    );
  }

  Widget _toolbar(Th09Board board, VoidCallback? locate, {required bool wide}) {
    final strings = context.strings;
    final characterBoard = _character != null;
    final style = LauncherControl.style(context);
    final filterActive =
        _status != null ||
        (!characterBoard && _group != null) ||
        (int.tryParse(_minimum.text.trim()) ?? board.minimumGames) > 1;
    final scope = LauncherSelect<int?>(
      label: switch (_character) {
        final int id => strings.th09BoardCharacter(
          th09CharacterLabel(id, strings.localeName),
        ),
        null => strings.th09BoardSelect,
      },
      value: _character,
      labels: {
        null: strings.th09BoardOverall,
        for (final (id, _) in th09CharacterNames.indexed)
          id: th09CharacterLabel(id, strings.localeName),
      },
      onChanged: (character) => setState(() {
        _resetScroll();
        _character = character;
        _sort = _Sort.score;
        _descending = true;
      }),
      expand: true,
    );
    final search = Semantics(
      label: strings.th09BoardSearch,
      child: TextField(
        controller: _search,
        textAlignVertical: TextAlignVertical.center,
        style: LauncherControl.label(context),
        decoration: _fieldDecoration().copyWith(
          hintText: strings.th09BoardSearch,
          prefixIcon: const Icon(Icons.search, size: LauncherControl.iconSize),
          prefixIconConstraints: const BoxConstraints(minWidth: 36),
          suffixIcon: Row(
            mainAxisSize: MainAxisSize.min,
            children: [
              if (_search.text.isNotEmpty)
                IconButton(
                  tooltip: strings.clearSearch,
                  onPressed: () {
                    _search.clear();
                    setState(_resetScroll);
                  },
                  icon: const Icon(Icons.close, size: LauncherControl.iconSize),
                ),
              IconButton(
                tooltip: strings.th09BoardLocate,
                onPressed: locate,
                icon: const Icon(
                  Icons.my_location,
                  size: LauncherControl.iconSize,
                ),
              ),
            ],
          ),
          suffixIconConstraints: const BoxConstraints.tightFor(
            height: LauncherControl.height,
          ),
        ),
        onChanged: (_) => setState(_resetScroll),
      ),
    );
    final filter = Semantics(
      selected: filterActive,
      child: Tooltip(
        message: filterActive
            ? strings.th09BoardFilterOn
            : strings.th09BoardFilter,
        child: OutlinedButton.icon(
          style: style,
          onPressed: () => _openFilters(board),
          icon: Icon(
            Icons.tune,
            color: filterActive ? Theme.of(context).colorScheme.primary : null,
          ),
          label: Text(strings.th09BoardFilter),
        ),
      ),
    );
    final sort = MenuAnchor(
      childFocusNode: _sortFocus,
      animated: !MediaQuery.disableAnimationsOf(context),
      menuChildren: [
        for (final value in _Sort.forBoard(character: characterBoard))
          MenuItemButton(
            autofocus: _sort == value,
            trailingIcon: _sort == value
                ? Tooltip(
                    message: _descending
                        ? strings.th09BoardDescending
                        : strings.th09BoardAscending,
                    child: Icon(
                      _descending ? Icons.arrow_downward : Icons.arrow_upward,
                    ),
                  )
                : null,
            onPressed: () => _sortBy(value),
            child: Semantics(
              selected: _sort == value,
              child: Text(value.label(strings, character: characterBoard)),
            ),
          ),
      ],
      builder: (context, controller, _) => Tooltip(
        message: strings.th09BoardSort,
        child: OutlinedButton(
          focusNode: _sortFocus,
          style: style,
          onPressed: () =>
              controller.isOpen ? controller.close() : controller.open(),
          child: const Icon(Icons.swap_vert),
        ),
      ),
    );
    return LayoutBuilder(
      builder: (context, constraints) {
        if (constraints.maxWidth >= 620) {
          final controls = Row(
            spacing: 8,
            children: [
              SizedBox(width: 186, child: scope),
              Expanded(child: search),
              filter,
              if (!wide) sort,
            ],
          );
          return wide
              ? Align(
                  alignment: AlignmentDirectional.centerEnd,
                  child: SizedBox(width: 520, child: controls),
                )
              : controls;
        }
        return Column(
          spacing: 8,
          children: [
            Row(
              spacing: 8,
              children: [
                Expanded(child: scope),
                filter,
              ],
            ),
            Row(
              spacing: 8,
              children: [
                Expanded(child: search),
                sort,
              ],
            ),
          ],
        );
      },
    );
  }

  void _showRatingHelp() {
    final strings = context.strings;
    final character = _character != null;
    showDialog<void>(
      context: context,
      builder: (context) => AlertDialog(
        shape: LauncherControl.shape,
        scrollable: true,
        title: Text(
          character ? strings.th09BoardCharacterHelp : strings.th09BoardHelp,
        ),
        content: Text(
          character
              ? strings.th09BoardCharacterHelpText
              : strings.th09BoardHelpText,
        ),
        actions: [
          TextButton(
            onPressed: () => Navigator.of(context).pop(),
            child: Text(strings.close),
          ),
        ],
      ),
    );
  }

  Widget _columns(List<_Sort> columns, Widget Function(_Sort) cell) => Row(
    children: [
      for (final column in columns)
        if (column == _Sort.name)
          Expanded(key: const ValueKey('board-name'), child: cell(column))
        else
          SizedBox(
            key: ValueKey('board-${column.name}'),
            width: column.width,
            child: cell(column),
          ),
    ],
  );

  Widget _columnHeader(List<_Sort> columns) {
    final theme = Theme.of(context);
    final strings = context.strings;
    final character = _character != null;
    return Material(
      key: const ValueKey('board-column-header'),
      color: theme.colorScheme.surfaceContainerHighest,
      borderRadius: const BorderRadius.vertical(top: Radius.circular(8)),
      child: _columns(columns, (column) {
        final label = column.label(strings, character: character);
        final selected = column == _sort;
        return Semantics(
          header: true,
          selected: selected,
          hint: selected
              ? (_descending
                    ? strings.th09BoardDescending
                    : strings.th09BoardAscending)
              : null,
          child: TextButton(
            style: TextButton.styleFrom(
              minimumSize: const Size(0, 44),
              padding: const EdgeInsets.symmetric(horizontal: 6),
              tapTargetSize: MaterialTapTargetSize.shrinkWrap,
              alignment: column.numeric
                  ? AlignmentDirectional.centerEnd
                  : AlignmentDirectional.centerStart,
              foregroundColor: selected
                  ? theme.colorScheme.primary
                  : theme.colorScheme.onSurfaceVariant,
              textStyle: LauncherControl.label(context),
            ),
            onPressed: () => _sortBy(column),
            child: Row(
              mainAxisSize: MainAxisSize.min,
              spacing: 3,
              children: [
                Flexible(
                  child: Text(
                    column == _Sort.position ? '#' : label,
                    semanticsLabel: column == _Sort.position ? label : null,
                    maxLines: 2,
                    overflow: TextOverflow.ellipsis,
                  ),
                ),
                if (selected)
                  Icon(
                    _descending ? Icons.arrow_downward : Icons.arrow_upward,
                    size: 12,
                  ),
              ],
            ),
          ),
        );
      }),
    );
  }

  Widget _row(
    BuildContext context,
    _Standing row,
    bool highlight,
    List<_Sort> columns,
  ) {
    final wide = columns.isNotEmpty;
    final player = row.player;
    final theme = Theme.of(context);
    final strings = context.strings;
    final number = NumberFormat.decimalPattern(strings.localeName);
    final percent = NumberFormat.decimalPercentPattern(
      locale: strings.localeName,
      decimalDigits: 1,
    );
    final date = MaterialLocalizations.of(context)
        .formatCompactDate(player.lastPlayed.toLocal());
    final username = player.username;
    final registered = username != null && isPublicUsername(username);
    final avatar = registered
        ? widget.api.th09BoardAvatar(username, player.avatar)
        : null;
    final playerPicture = AccountAvatar(
      api: widget.api,
      username: null,
      address: avatar,
      size: 28,
    );
    final playerAvatar = registered
        ? Th09PlayerCardButton(
            api: widget.api,
            username: username,
            name: player.nickname,
            profileCharacter: _character,
            scores: player.scoreCard,
            child: playerPicture,
          )
        : playerPicture;
    void openProfile() => context.push(
      Uri(
        path: profilePath(username!),
        queryParameters: _character == null
            ? null
            : {'character': '$_character'},
      ).toString(),
    );

    final nickname = Text(
      player.nickname,
      maxLines: 1,
      overflow: TextOverflow.ellipsis,
    );
    final status = _statusLabel(player.status, strings);
    final group = strings.th09BoardGroupNumber(number.format(player.group));
    final opponents = strings.th09BoardOpponentsCount(
      number.format(player.opponents),
    );
    final victory = player.games > 0 ? percent.format(_winRate(player)) : '—';
    final score = number.format(_score(row));
    final uncertainty = number.format(_interval(row));
    Widget desktopCell(_Sort column) {
      if (column == _Sort.name) {
        final identity = Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          mainAxisSize: MainAxisSize.min,
          children: [
            DefaultTextStyle.merge(
              style: const TextStyle(fontWeight: FontWeight.w600),
              child: nickname,
            ),
            if (registered)
              Text(
                '@$username',
                style: theme.textTheme.bodySmall?.copyWith(
                  color: theme.colorScheme.onSurfaceVariant,
                ),
                maxLines: 1,
                overflow: TextOverflow.ellipsis,
              ),
          ],
        );
        return Padding(
          padding: const EdgeInsets.symmetric(horizontal: 6),
          child: Row(
            spacing: 8,
            children: [
              playerAvatar,
              Expanded(
                child: Tooltip(
                  message: opponents,
                  child: registered
                      ? TextButton(
                          onPressed: openProfile,
                          style: TextButton.styleFrom(
                            foregroundColor: theme.colorScheme.onSurface,
                            minimumSize: const Size(0, 44),
                            padding: EdgeInsets.zero,
                            alignment: AlignmentDirectional.centerStart,
                            tapTargetSize: MaterialTapTargetSize.shrinkWrap,
                            textStyle: theme.textTheme.bodyMedium,
                          ),
                          child: identity,
                        )
                      : identity,
                ),
              ),
            ],
          ),
        );
      }
      final value = switch (column) {
        _Sort.position => number.format(row.position),
        _Sort.score => score,
        _Sort.interval => '± $uncertainty',
        _Sort.overall => number.format(player.rating),
        _Sort.games => number.format(_games(row)),
        _Sort.winRate => victory,
        _Sort.group => group,
        _Sort.lastPlayed => date,
        _Sort.status => status,
        _Sort.name => player.nickname,
      };
      return Padding(
        padding: const EdgeInsets.symmetric(horizontal: 6),
        child: Text(
          value,
          textAlign: column.numeric ? TextAlign.end : TextAlign.start,
          maxLines: 1,
          overflow: TextOverflow.ellipsis,
          style: (theme.textTheme.bodySmall ?? const TextStyle()).copyWith(
            fontFeatures: const [FontFeature.tabularFigures()],
            fontWeight: column == _Sort.score ? FontWeight.w600 : null,
            color: column == _Sort.score
                ? theme.colorScheme.onSurface
                : theme.colorScheme.onSurfaceVariant,
          ),
        ),
      );
    }

    final tile = ExpansionTile(
      key: PageStorageKey((_character, player.id)),
      tilePadding: wide
          ? EdgeInsets.zero
          : const EdgeInsets.symmetric(horizontal: 10),
      shape: const Border(),
      collapsedShape: const Border(),
      minTileHeight: wide ? 48 : null,
      showTrailingIcon: !wide,
      leading: wide
          ? null
          : Row(
              mainAxisSize: MainAxisSize.min,
              spacing: 4,
              children: [
                SizedBox(
                  width: 30,
                  child: Text('#${row.position}', textAlign: TextAlign.center),
                ),
                if (registered) playerAvatar,
              ],
            ),
      title: wide ? _columns(columns, desktopCell) : nickname,
      subtitle: wide
          ? null
          : Text(
              '${strings.th09BoardGamesCount(number.format(_games(row)))} | $status',
              maxLines: 1,
              overflow: TextOverflow.ellipsis,
            ),
      trailing: wide ? null : Text('$score  ±$uncertainty'),
      childrenPadding: const EdgeInsets.fromLTRB(12, 0, 12, 10),
      children: [
        Align(
          alignment: AlignmentDirectional.centerStart,
          child: Wrap(
            spacing: 12,
            runSpacing: 8,
            crossAxisAlignment: WrapCrossAlignment.center,
            children: [
              if (!wide && registered)
                TextButton.icon(
                  onPressed: openProfile,
                  icon: AccountAvatar(
                    api: widget.api,
                    username: null,
                    address: avatar,
                    size: 24,
                  ),
                  label: Text('@$username'),
                ),
              if (!wide) Text(group),
              if (!wide && row.character != null)
                Text(
                  '${strings.th09BoardRating} ${number.format(player.rating)}',
                ),
              Text(opponents),
              if (!wide && player.games > 0)
                Text(strings.th09BoardWinRate(victory)),
              if (!wide) Text(strings.th09BoardLastPlayed(date)),
              for (final id in player.topCharacters)
                Tooltip(
                  message: th09CharacterLabel(id, strings.localeName),
                  child: Th09CharacterPortrait(id, size: 26),
                ),
            ],
          ),
        ),
      ],
    );
    final selectedColor = highlight ? theme.colorScheme.primaryContainer : null;
    return KeyedSubtree(
      key: ValueKey(player.id),
      child: Focus(
        focusNode: player.id == _locatedPlayer ? _locatedFocus : null,
        canRequestFocus: player.id == _locatedPlayer,
        skipTraversal: true,
        child: wide
            ? Material(
                color: selectedColor ?? theme.colorScheme.surfaceContainerLow,
                shape: Border(
                  bottom: BorderSide(color: theme.colorScheme.outlineVariant),
                ),
                child: ListTileTheme.merge(minVerticalPadding: 2, child: tile),
              )
            : Card(
                margin: const EdgeInsets.only(bottom: 6),
                color: selectedColor,
                child: tile,
              ),
      ),
    );
  }

  @override
  Widget build(BuildContext context) {
    final board = _board;
    final dates = MaterialLocalizations.of(context);
    final theme = Theme.of(context);
    final strings = context.strings;
    final number = NumberFormat.decimalPattern(strings.localeName);
    final character = _character;
    final columns = _wide
        ? _Sort.forBoard(character: character != null).toList(growable: false)
        : const <_Sort>[];
    final query = _search.text.trim().toLowerCase().replaceFirst(
      RegExp(r'^@'),
      '',
    );
    final pool = board == null
        ? const <_Standing>[]
        : _filteredStandings(board);
    final matches = {
      for (final row in pool)
        if (query.isEmpty ||
            row.player.nickname.toLowerCase().contains(query) ||
            (row.player.username?.toLowerCase().contains(query) ?? false))
          row.player.id,
    };
    final showContext = _locatedPlayer != null;
    final visible =
        pool
            .where((row) => showContext || matches.contains(row.player.id))
            .toList()
          ..sort(_compare);
    final positions = {
      for (final (index, row) in visible.indexed) row.player.id: index,
    };
    final firstMatch = matches.firstOrNull;
    return Scaffold(
      appBar: AppBar(
        leading: BackButton(onPressed: () => profileBack(context, '/lobby')),
        titleSpacing: 0,
        title: Text(strings.th09BoardTitle, style: theme.textTheme.titleMedium),
        actions: [
          IconButton(
            tooltip: strings.th09BoardRefresh,
            onPressed: _loading ? null : _refresh,
            icon: const Icon(Icons.refresh),
          ),
        ],
      ),
      body: SafeArea(
        child: Center(
          child: ConstrainedBox(
            constraints: const BoxConstraints(maxWidth: 1280),
            child: CustomScrollView(
              controller: _scroll,
              slivers: [
                SliverToBoxAdapter(
                  child: SizedBox(
                    height: 2,
                    child: _loading
                        ? LinearProgressIndicator(
                            minHeight: 2,
                            semanticsLabel: strings.th09BoardLoading,
                          )
                        : null,
                  ),
                ),
                if (_failed)
                  SliverToBoxAdapter(
                    child: ListTile(
                      title: Text(strings.th09BoardFailed),
                      trailing: TextButton(
                        onPressed: _refresh,
                        child: Text(strings.th09BoardRetry),
                      ),
                    ),
                  ),
                if (board != null) ...[
                  SliverPadding(
                    padding: const EdgeInsets.all(16),
                    sliver: SliverToBoxAdapter(
                      child: Column(
                        crossAxisAlignment: CrossAxisAlignment.stretch,
                        spacing: 14,
                        children: [
                          _toolbar(
                            board,
                            query.isNotEmpty &&
                                    firstMatch != null &&
                                    !showContext
                                ? () => _locatePlayer(firstMatch)
                                : null,
                            wide: _wide,
                          ),
                          ExpansionTile(
                            title: Text(strings.th09BoardDistribution),
                            leading: IconButton(
                              tooltip: character == null
                                  ? strings.th09BoardHelp
                                  : strings.th09BoardCharacterHelp,
                              onPressed: _showRatingHelp,
                              icon: const Icon(Icons.help_outline),
                            ),
                            children: [
                              _Distribution(
                                scores: pool.map(_score).toList(),
                                matching: query.isEmpty
                                    ? const []
                                    : pool
                                          .where(
                                            (row) =>
                                                matches.contains(row.player.id),
                                          )
                                          .map(_score)
                                          .toList(),
                              ),
                            ],
                          ),
                          Text(
                            '${strings.th09BoardUpdated(dates.formatCompactDate(board.updatedAt.toLocal()))} | ${strings.th09BoardPlayerCount(number.format(visible.length))}',
                            style: theme.textTheme.bodySmall,
                          ),
                          if (showContext && query.isNotEmpty)
                            Text(
                              strings.th09BoardHighlights(
                                number.format(matches.length),
                              ),
                            ),
                          if (visible.isEmpty)
                            Text(
                              board.players.isEmpty
                                  ? strings.th09BoardEmpty
                                  : strings.th09BoardNoMatches,
                            ),
                          if (visible.isEmpty && board.players.isNotEmpty)
                            Align(
                              alignment: AlignmentDirectional.centerStart,
                              child: TextButton(
                                onPressed: _clearFilters,
                                child: Text(strings.th09BoardClear),
                              ),
                            ),
                        ],
                      ),
                    ),
                  ),
                  if (_wide && visible.isNotEmpty)
                    SliverPadding(
                      padding: const EdgeInsets.symmetric(horizontal: 16),
                      sliver: SliverToBoxAdapter(child: _columnHeader(columns)),
                    ),
                  SliverPadding(
                    padding: const EdgeInsets.symmetric(horizontal: 16),
                    sliver: SuperSliverList(
                      listController: _rows,
                      delegate: SliverChildBuilderDelegate(
                        (context, index) => _row(
                          context,
                          visible[index],
                          showContext &&
                              matches.contains(visible[index].player.id),
                          columns,
                        ),
                        childCount: visible.length,
                        findChildIndexCallback: (key) => key is ValueKey<String>
                            ? positions[key.value]
                            : null,
                      ),
                    ),
                  ),
                ],
              ],
            ),
          ),
        ),
      ),
    );
  }
}

class _Distribution extends StatelessWidget {
  const _Distribution({required this.scores, required this.matching});

  final List<int> scores;
  final List<int> matching;

  @override
  Widget build(BuildContext context) {
    final strings = context.strings;
    if (scores.isEmpty) return Text(strings.th09BoardEmpty);
    final number = NumberFormat.decimalPattern(strings.localeName);
    final first = scores.reduce((a, b) => a < b ? a : b);
    final last = scores.reduce((a, b) => a > b ? a : b);
    final step =
        (((last - first) / 100).ceil() / 12).ceil().clamp(1, 1000000) * 100;
    final base = (first / step).floor() * step;
    final counts = <int, int>{};
    final highlighted = <int, int>{};
    for (final score in scores) {
      counts.update((score - base) ~/ step, (n) => n + 1, ifAbsent: () => 1);
    }
    for (final score in matching) {
      highlighted.update(
        (score - base) ~/ step,
        (n) => n + 1,
        ifAbsent: () => 1,
      );
    }
    final colors = Theme.of(context).colorScheme;
    final bands = (last - base) ~/ step + 1;
    return Padding(
      padding: const EdgeInsets.fromLTRB(12, 8, 12, 16),
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.stretch,
        spacing: 8,
        children: [
          Text(
            '${number.format(first)}–${number.format(last)} | ${strings.th09BoardBinSize(number.format(step))}',
          ),
          SizedBox(
            height: 100,
            child: Semantics(
              label: [
                for (final entry in counts.entries)
                  strings.th09BoardBin(
                    number.format(base + entry.key * step),
                    number.format(entry.value),
                  ),
              ].join('\n'),
              excludeSemantics: true,
              child: BarChart(
                BarChartData(
                  titlesData: const FlTitlesData(show: false),
                  gridData: const FlGridData(show: false),
                  borderData: FlBorderData(show: false),
                  barGroups: [
                    for (var i = 0; i < bands; i++)
                      BarChartGroupData(
                        x: i,
                        barRods: [
                          BarChartRodData(
                            toY: (counts[i] ?? 0).toDouble(),
                            color: colors.secondaryContainer,
                            width: 12,
                            rodStackItems: [
                              if (highlighted.containsKey(i))
                                BarChartRodStackItem(
                                  0,
                                  highlighted[i]!.toDouble(),
                                  colors.primary,
                                ),
                            ],
                          ),
                        ],
                      ),
                  ],
                  barTouchData: BarTouchData(
                    touchTooltipData: BarTouchTooltipData(
                      getTooltipItem: (group, _, rod, _) => BarTooltipItem(
                        strings.th09BoardBin(
                          number.format(base + group.x * step),
                          number.format(rod.toY.round()),
                        ),
                        TextStyle(color: colors.onInverseSurface),
                      ),
                    ),
                  ),
                ),
                duration: Duration.zero,
              ),
            ),
          ),
        ],
      ),
    );
  }
}
