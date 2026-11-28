import 'package:material_ui/material_ui.dart';

import 'launcher_select.dart';
import 'l10n/strings.dart';

import 'game_progress_catalog.dart';
import 'save_progress.dart';
import 'statistics_tile.dart';

const _withoutExtra = {'th01', 'th03', 'th075', 'th19'};
const _letterShots = {'th04', 'th06', 'th07', 'th10', 'th11', 'th12', 'th14'};

bool _supports(String game, PlayDifficulty difficulty) => switch (difficulty) {
  PlayDifficulty.extra => !_withoutExtra.contains(game),
  PlayDifficulty.phantasm => game == 'th07',
  PlayDifficulty.overdrive => game == 'th20',
  _ => true,
};

enum _ClearStatus {
  oneCredit(Icons.check),
  cleared(Icons.circle),
  failed(Icons.circle),
  untried(null),
  unavailable(null),
  unread(null);

  const _ClearStatus(this.icon);
  final IconData? icon;

  String label(LauncherStrings strings) => switch (this) {
    oneCredit => strings.progressOneCredit,
    cleared => strings.progressCleared,
    failed => strings.progressFailed,
    untried => strings.progressUntried,
    unavailable => strings.progressUnavailable,
    unread => strings.progressUnread,
  };

  static _ClearStatus from(List<SaveProgress> records) {
    if (records.any((row) => row.oneCreditClear == true)) return oneCredit;
    if (records.any((row) => row.cleared == true)) return cleared;
    if (records.any(
      (row) =>
          row.cleared == false &&
          (row.playCount != 0 || (row.bestScore ?? 0) > 0),
    )) {
      return failed;
    }
    if (records.any((row) => row.cleared == false && row.playCount == 0)) {
      return untried;
    }
    return records.isEmpty ? unread : unavailable;
  }

  (Color, Color) palette(ColorScheme color) => switch (this) {
    oneCredit => (color.primary, color.onPrimary),
    cleared => (
      Color.alphaBlend(color.primary.withValues(alpha: .22), color.surface),
      color.primary,
    ),
    failed => (color.surfaceContainerHighest, color.onSurfaceVariant),
    untried => (
      Color.alphaBlend(
        color.surfaceContainerHighest.withValues(alpha: .85),
        color.surface,
      ),
      color.onSurfaceVariant,
    ),
    unavailable => (Colors.transparent, color.outlineVariant),
    unread => (
      Color.alphaBlend(
        color.surfaceContainerHighest.withValues(alpha: .56),
        color.surface,
      ),
      color.onSurfaceVariant,
    ),
  };
}

typedef WorkProgressRow = ({Widget identity, List<SaveProgress> data});
typedef _ProgressGroup = ({
  Widget identity,
  String? label,
  List<({String? branch, List<SaveProgress> data})> lines,
});

class WorkProgressTable extends StatelessWidget {
  const WorkProgressTable({super.key, required this.rows});
  final List<WorkProgressRow> rows;

  @override
  Widget build(BuildContext context) => _ProgressGrid(
    identityLabel: context.strings.progressWork,
    levels: [
      for (final level in PlayDifficulty.values)
        if (rows.any(
          (row) => row.data.any((record) => record.difficulty == level),
        ))
          level,
    ],
    maxHeight: 220,
    groups: [
      for (final row in rows)
        (
          identity: row.identity,
          label: null,
          lines: [(branch: null, data: row.data)],
        ),
    ],
  );
}

typedef _Identity = ({int character, int? shot});
typedef _ProgressSelection = ({
  int character,
  int? shot,
  PlayDifficulty difficulty,
  String? mode,
});
typedef _Loadout = ({
  int character,
  int? shot,
  String label,
  List<SaveProgress> data,
});

_ProgressSelection _selectionOf(SaveProgress row) => (
  character: row.character,
  shot: row.shot,
  difficulty: row.difficulty,
  mode: row.mode,
);

class GameClearMatrix extends StatefulWidget {
  const GameClearMatrix({
    super.key,
    required this.game,
    required this.progress,
    this.showScores = true,
    this.details,
  });

  final String game;
  final List<SaveProgress> progress;
  final bool showScores;
  final Widget Function(SaveProgress, String)? details;

  @override
  State<GameClearMatrix> createState() => _GameClearMatrixState();
}

class _GameClearMatrixState extends State<GameClearMatrix> {
  String _mode = 'legacy';
  _ProgressSelection? _selected;

  List<_Loadout> _loadouts(BuildContext context, List<SaveProgress> data) {
    final strings = context.strings;
    final language = bundledLanguage(Localizations.localeOf(context));
    final roster = progressRoster[widget.game] ?? const [];
    final labels = <_Identity, String?>{
      for (final entry in roster)
        (character: entry.character, shot: entry.shot):
            entry.labels[language] ?? entry.labels['zh-CN'],
    };
    final standalone = {
      for (final entry in roster)
        if (entry.shot == null) entry.character,
    };
    final entries = <_Identity, List<SaveProgress>>{
      for (final id in labels.keys) id: [],
    };
    for (final record in data) {
      final id = (
        character: record.character,
        shot: record.shot == 0 && standalone.contains(record.character)
            ? null
            : record.shot,
      );
      entries.putIfAbsent(id, () => []).add(record);
    }
    final keys = entries.keys.toList()
      ..sort((a, b) {
        final character = a.character.compareTo(b.character);
        return character != 0
            ? character
            : (a.shot ?? -1).compareTo(b.shot ?? -1);
      });
    return [
      for (final id in keys)
        (
          character: id.character,
          shot: id.shot,
          label:
              labels[id] ??
              '${strings.progressLoadoutNumber(id.character + 1)}${id.shot == null ? '' : ' ${_branch(id.shot!)}'}',
          data: entries[id]!,
        ),
    ];
  }

  String _branch(int shot) =>
      shot < 26 ? String.fromCharCode(65 + shot) : '${shot + 1}';

  @override
  Widget build(BuildContext context) {
    final all = widget.progress;
    final hasPointdevice = all.any((row) => row.mode == 'pointdevice');
    final mode = hasPointdevice ? _mode : 'legacy';
    final progress = all
        .where((row) => row.mode == null || row.mode == mode)
        .toList();
    final strings = context.strings;
    final roster = _loadouts(context, progress);
    final byCharacter = <int, List<_Loadout>>{};
    for (final entry in roster) {
      byCharacter.putIfAbsent(entry.character, () => []).add(entry);
    }
    final groups = <_ProgressGroup>[];
    Widget label(String label) => Tooltip(
      message: label,
      child: Text(
        label,
        maxLines: 1,
        overflow: TextOverflow.ellipsis,
        textAlign: TextAlign.end,
      ),
    );
    for (final entries in byCharacter.values) {
      if (_letterShots.contains(widget.game) && entries.length > 1) {
        final title = entries.first.label.replaceFirst(
          RegExp(r'\s+[ABC]$'),
          '',
        );
        groups.add((
          identity: label(title),
          label: title,
          lines: [
            for (final row in entries)
              (
                branch: row.shot == null ? '—' : _branch(row.shot!),
                data: row.data,
              ),
          ],
        ));
      } else {
        for (final row in entries) {
          groups.add((
            identity: label(row.label),
            label: row.label,
            lines: [(branch: null, data: row.data)],
          ));
        }
      }
    }
    final requested = progress
        .where((row) => _selectionOf(row) == _selected)
        .firstOrNull;
    var selected = requested;
    if (selected == null) {
      for (final row in progress) {
        if (selected == null ||
            (row.playCount ?? 0) > (selected.playCount ?? 0)) {
          selected = row;
        }
      }
    }
    final selectedLabel = roster
        .where((entry) => entry.data.contains(selected))
        .firstOrNull
        ?.label;
    return Column(
      crossAxisAlignment: CrossAxisAlignment.stretch,
      spacing: 10,
      children: [
        if (hasPointdevice)
          Align(
            alignment: AlignmentDirectional.centerEnd,
            child: LauncherSegments<String>(
              segments: [
                ButtonSegment(
                  value: 'legacy',
                  label: Text(strings.progressLegacy),
                ),
                ButtonSegment(
                  value: 'pointdevice',
                  label: Text(strings.progressPointdevice),
                ),
              ],
              value: mode,
              onChanged: (value) => setState(() {
                _mode = value;
                _selected = null;
              }),
            ),
          ),
        _ProgressGrid(
          identityLabel: strings.progressLoadout,
          levels: [
            for (final level in PlayDifficulty.values)
              if (_supports(widget.game, level) ||
                  progress.any((row) => row.difficulty == level))
                level,
          ],
          singleWork: true,
          selected: widget.details == null ? requested : selected,
          showScores: widget.showScores,
          onSelect: (row) => setState(() => _selected = _selectionOf(row)),
          selectable: widget.details != null,
          groups: groups.isEmpty
              ? [
                  (
                    identity: label(strings.progressUnread),
                    label: strings.progressUnread,
                    lines: [(branch: null, data: const [])],
                  ),
                ]
              : groups,
        ),
        if (widget.details != null && selected != null && selectedLabel != null)
          Padding(
            padding: const EdgeInsets.only(top: 12),
            child: widget.details!(selected, selectedLabel),
          ),
      ],
    );
  }
}

class _ProgressGrid extends StatelessWidget {
  const _ProgressGrid({
    required this.identityLabel,
    required this.levels,
    required this.groups,
    this.maxHeight,
    this.singleWork = false,
    this.selected,
    this.showScores = false,
    this.onSelect,
    this.selectable = false,
  });

  final String identityLabel;
  final List<PlayDifficulty> levels;
  final List<_ProgressGroup> groups;
  final double? maxHeight;
  final bool singleWork, showScores, selectable;
  final SaveProgress? selected;
  final ValueChanged<SaveProgress>? onSelect;

  static const _identityEndPadding = 4.0;
  static const _branchGap = 3.0;
  static const _branchBracketWidth = 6.0;
  static const _branchTextWidth = 16.0;

  double _identityWidth(BuildContext context, double width) {
    final compact = width < 480;
    if (!singleWork) return (compact ? 114.0 : 172.0).clamp(0.0, width);

    final minimum = (compact ? 96.0 : 140.0).clamp(0.0, width);
    // Reserve space for each difficulty cell even with large accessibility text.
    final maximum = (width - levels.length * 22).clamp(minimum, width);
    final painter = TextPainter(
      textDirection: Directionality.of(context),
      textScaler: MediaQuery.textScalerOf(context),
      maxLines: 1,
    );
    final style = DefaultTextStyle.of(context).style;
    var widest = minimum;
    for (final group in groups) {
      painter.text = TextSpan(text: group.label, style: style);
      painter.layout();
      final branchWidth = group.lines.length > 1
          ? 2 * _branchGap + _branchBracketWidth + _branchTextWidth
          : 0.0;
      final needed = (painter.width + _identityEndPadding + branchWidth)
          .ceilToDouble();
      if (needed > widest) widest = needed;
    }
    painter.dispose();
    return widest.clamp(minimum, maximum);
  }

  @override
  Widget build(BuildContext context) => Card.outlined(
    margin: EdgeInsets.zero,
    color: Theme.of(context).colorScheme.surface,
    child: Padding(
      padding: const EdgeInsets.all(10),
      child: LayoutBuilder(
        builder: (context, limits) {
          final theme = Theme.of(context);
          final width = limits.maxWidth;
          final first = _identityWidth(context, width);
          final levelWidth = (width - first) / levels.length;
          final levelStyle = DefaultTextStyle.of(context).style
              .merge(theme.textTheme.labelSmall);
          final useAbbreviation =
              levelWidth < 57 ||
              levels.any(
                (level) =>
                    TextPainter.computeWidth(
                          text: TextSpan(text: level.label, style: levelStyle),
                          textDirection: Directionality.of(context),
                          textScaler: MediaQuery.textScalerOf(context),
                          maxLines: 1,
                        ) +
                        8 >
                    levelWidth,
              );
          const height = 33.0;
          final header = Row(
            children: [
              SizedBox(width: first, child: Text(identityLabel)),
              for (final level in levels)
                Expanded(
                  child: Tooltip(
                    message: level.label,
                    child: Text(
                      useAbbreviation ? level.shortLabel : level.label,
                      maxLines: 1,
                      overflow: TextOverflow.ellipsis,
                      textAlign: TextAlign.center,
                      style: theme.textTheme.labelSmall,
                    ),
                  ),
                ),
            ],
          );
          final body = Column(
            spacing: 4,
            children: [
              for (final group in groups)
                Row(
                  children: [
                    SizedBox(
                      width: first,
                      child: Padding(
                        padding: const EdgeInsetsDirectional.only(
                          end: _identityEndPadding,
                        ),
                        child: group.lines.length == 1
                            ? group.identity
                            : Row(
                                spacing: _branchGap,
                                children: [
                                  Expanded(child: group.identity),
                                  Container(
                                    height: height * (group.lines.length - 1),
                                    width: _branchBracketWidth,
                                    decoration: BoxDecoration(
                                      border: Border(
                                        left: BorderSide(
                                          color:
                                              theme.colorScheme.outlineVariant,
                                        ),
                                        top: BorderSide(
                                          color:
                                              theme.colorScheme.outlineVariant,
                                        ),
                                        bottom: BorderSide(
                                          color:
                                              theme.colorScheme.outlineVariant,
                                        ),
                                      ),
                                    ),
                                  ),
                                  Column(
                                    children: [
                                      for (final line in group.lines)
                                        SizedBox(
                                          height: height,
                                          width: _branchTextWidth,
                                          child: Center(
                                            child: Text(
                                              line.branch!,
                                              style: theme.textTheme.labelSmall,
                                            ),
                                          ),
                                        ),
                                    ],
                                  ),
                                ],
                              ),
                      ),
                    ),
                    Expanded(
                      child: !singleWork && group.lines.single.data.isEmpty
                          ? SizedBox(
                              height: height,
                              child: Center(
                                child: Text(
                                  context.strings.progressUnread,
                                  style: theme.textTheme.labelSmall,
                                ),
                              ),
                            )
                          : Column(
                              children: [
                                for (final line in group.lines)
                                  SizedBox(
                                    height: height,
                                    child: Row(
                                      children: [
                                        for (final level in levels)
                                          Expanded(
                                            child: Padding(
                                              padding: const EdgeInsets.all(2),
                                              child: _cell(
                                                context.strings,
                                                level,
                                                line.data,
                                                [
                                                  ?group.label,
                                                  ?line.branch,
                                                ].join(' '),
                                              ),
                                            ),
                                          ),
                                      ],
                                    ),
                                  ),
                              ],
                            ),
                    ),
                  ],
                ),
            ],
          );
          return Column(
            crossAxisAlignment: CrossAxisAlignment.stretch,
            spacing: 8,
            children: [
              header,
              if (maxHeight case final double cap)
                ConstrainedBox(
                  constraints: BoxConstraints(maxHeight: cap),
                  child: SingleChildScrollView(
                    key: const PageStorageKey('profile-career-matrix'),
                    primary: false,
                    child: body,
                  ),
                )
              else
                body,
              Wrap(
                alignment: WrapAlignment.center,
                spacing: 12,
                runSpacing: 6,
                children: [
                  for (final state in _ClearStatus.values)
                    Row(
                      mainAxisSize: MainAxisSize.min,
                      spacing: 5,
                      children: [
                        SizedBox.square(
                          dimension: 10,
                          child: _ProgressPaint(state: state, legend: true),
                        ),
                        Flexible(
                          child: Text(
                            state.label(context.strings),
                            style: theme.textTheme.labelSmall,
                          ),
                        ),
                      ],
                    ),
                ],
              ),
            ],
          );
        },
      ),
    ),
  );

  Widget _cell(
    LauncherStrings strings,
    PlayDifficulty level,
    List<SaveProgress> data,
    String loadout,
  ) {
    final records = data.where((row) => row.difficulty == level).toList();
    if (!singleWork && records.isEmpty) {
      return const SizedBox();
    }
    final row = records.firstOrNull;
    final state = _ClearStatus.from(records);
    final score = showScores ? row?.bestScore : null;
    return _ProgressCell(
      description: '${level.label} | ${state.label(strings)}',
      loadout: loadout,
      state: state,
      score: score,
      active: row != null && selected == row,
      onTap: row != null && onSelect != null && (selectable || score != null)
          ? () => onSelect!(row)
          : null,
    );
  }
}

class _ProgressCell extends StatelessWidget {
  const _ProgressCell({
    required this.description,
    required this.loadout,
    required this.state,
    required this.score,
    required this.active,
    required this.onTap,
  });

  final String description;
  final String loadout;
  final _ClearStatus state;
  final int? score;
  final bool active;
  final VoidCallback? onTap;

  @override
  Widget build(BuildContext context) {
    final best = score;
    final message = best == null
        ? description
        : '$description | ${context.strings.progressBestScore} ${MaterialLocalizations.of(context).formatDecimal(best)}';
    return StatisticsHint(
      message: message,
      semanticLabel: loadout.isEmpty ? null : '$loadout | $message',
      selected: onTap == null ? null : active,
      onTap: onTap,
      showOnTap: onTap != null && best != null,
      child: _ProgressPaint(state: state, active: active),
    );
  }
}

class _ProgressPaint extends StatelessWidget {
  const _ProgressPaint({
    required this.state,
    this.active = false,
    this.legend = false,
  });
  final _ClearStatus state;
  final bool active, legend;

  @override
  Widget build(BuildContext context) {
    final scheme = Theme.of(context).colorScheme;
    final (surface, ink) = state.palette(scheme);
    return StatisticsTile(
      fill: surface,
      radius: legend ? 3 : 5,
      outline: active
          ? scheme.primary
          : state == _ClearStatus.unavailable || state == _ClearStatus.failed
          ? scheme.outlineVariant
          : null,
      dashed: !active && state == _ClearStatus.unavailable,
      child: Center(
        child: legend || state.icon == null
            ? null
            : Icon(
                state.icon,
                color: ink,
                size: state == _ClearStatus.oneCredit ? 15 : 6,
              ),
      ),
    );
  }
}
