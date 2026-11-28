import 'package:material_ui/material_ui.dart';

import 'launcher_prism.dart';

import 'launcher_select.dart';
import 'l10n/strings.dart';

import 'game_progress_catalog.dart';
import 'save_progress.dart';
import 'statistics_tile.dart';

typedef _Spell = ({
  int number,
  bool midboss,
  String name,
  int attempts,
  int captures,
});

typedef _Stage = ({String label, List<_Spell> spells});

enum _Career { story, practice, lastWord }

class GameSpellArchive extends StatefulWidget {
  const GameSpellArchive({
    super.key,
    required this.statistics,
    required this.selection,
    required this.loadoutLabel,
  });

  final SaveStatistics statistics;
  final SaveProgress selection;
  final String loadoutLabel;

  @override
  State<GameSpellArchive> createState() => _GameSpellArchiveState();
}

class _GameSpellArchiveState extends State<GameSpellArchive> {
  bool _showList = false;
  _Career _career = _Career.story;

  bool get _hasCareers => widget.statistics.game == 'th08';
  String get _difficulty => _hasCareers && _career == _Career.lastWord
      ? 'last-word'
      : widget.selection.difficulty.name;

  String _stageLabel(LauncherStrings strings, int stage) => switch (stage) {
    7 => 'Extra',
    8 => 'Phantasm',
    9 => strings.spellLastWord,
    41 => strings.spellBranch('4A'),
    42 => strings.spellBranch('4B'),
    61 => strings.spellBranch('6A'),
    62 => strings.spellBranch('6B'),
    _ => strings.spellStage(stage),
  };

  List<_Stage> _stages(LauncherStrings strings) {
    final selected = widget.selection;
    final stats = widget.statistics;
    final index = selected.character * 2 + (selected.shot ?? 0);
    final group = spellStageCatalog[stats.game]?[_difficulty] ?? const [];
    _Spell card(SpellLayoutCard layout) {
      final record = stats.spells[layout.number];
      final tally =
          (stats.totalSpells
              ? record?.total
              : record?.loadouts[(
                  character: selected.character,
                  shot: selected.shot,
                  difficulty: _difficulty,
                  activity: _hasCareers
                      ? (_career == _Career.story
                            ? SpellActivity.story
                            : SpellActivity.practice)
                      : null,
                )]) ??
          (record != null && index < record.legacy.length
              ? record.legacy[index]
              : null);
      final attempts = tally?.attempts ?? 0;
      return (
        number: layout.number,
        midboss: layout.midboss,
        name: attempts > 0
            ? record?.name ?? '？' * layout.nameLength
            : '？' * layout.nameLength,
        attempts: attempts,
        captures: tally?.captures ?? 0,
      );
    }

    return [
      for (final stage in group)
        (
          label: _stageLabel(strings, stage.stage),
          spells: stage.cards.map(card).toList(),
        ),
    ];
  }

  @override
  Widget build(BuildContext context) {
    final theme = Theme.of(context);
    final strings = context.strings;
    final stages = _stages(strings);
    final loaded = widget.statistics.hasSpellData && stages.isNotEmpty;
    final label = widget.statistics.totalSpells
        ? strings.spellAllLoadouts
        : _hasCareers && _career == _Career.lastWord
        ? widget.loadoutLabel
        : '${widget.loadoutLabel} | ${widget.selection.difficulty.label}';
    return Column(
      crossAxisAlignment: CrossAxisAlignment.stretch,
      spacing: 10,
      children: [
        Wrap(
          spacing: 10,
          runSpacing: 6,
          alignment: WrapAlignment.spaceBetween,
          crossAxisAlignment: WrapCrossAlignment.center,
          children: [
            Text.rich(
              TextSpan(
                children: [
                  TextSpan(
                    text: strings.spellCards,
                    style: launcherSectionTitle(context),
                  ),
                  TextSpan(
                    text: '  ${label.replaceAll(' ', '\u00a0')}',
                    semanticsLabel: '  $label',
                    style: theme.textTheme.labelSmall,
                  ),
                ],
              ),
            ),
            if (loaded)
              LauncherSegments<bool>(
                segments: [
                  ButtonSegment(value: false, label: Text(strings.spellMap)),
                  ButtonSegment(value: true, label: Text(strings.spellList)),
                ],
                value: _showList,
                onChanged: (next) => setState(() => _showList = next),
              ),
          ],
        ),
        if (loaded && _hasCareers)
          Align(
            alignment: AlignmentDirectional.centerStart,
            child: LauncherSegments<_Career>(
              segments: [
                ButtonSegment(
                  value: _Career.story,
                  label: Text(strings.spellStory),
                ),
                ButtonSegment(
                  value: _Career.practice,
                  label: Text(strings.spellPractice),
                ),
                ButtonSegment(
                  value: _Career.lastWord,
                  label: Text(strings.spellLastWord),
                ),
              ],
              value: _career,
              onChanged: (next) => setState(() => _career = next),
            ),
          ),
        if (loaded)
          TweenAnimationBuilder<double>(
            key: ValueKey((_showList, _difficulty, _career)),
            tween: Tween(begin: 0, end: 1),
            duration: MediaQuery.disableAnimationsOf(context)
                ? Duration.zero
                : const Duration(milliseconds: 160),
            builder: (_, opacity, body) =>
                Opacity(opacity: opacity, child: body),
            child: _showList
                ? _SpellList(stages, split: !_hasCareers)
                : _SpellMap(stages, split: !_hasCareers),
          )
        else
          Text(strings.spellUnavailable),
      ],
    );
  }
}

String _number(_Spell spell) => 'No.${spell.number.toString().padLeft(2, '0')}';
String _fraction(_Spell spell) => '${spell.captures}/${spell.attempts}';

String _rate(_Spell spell) => spell.attempts == 0
    ? '—'
    : '${(100 * spell.captures / spell.attempts).toStringAsFixed(1).replaceFirst(RegExp(r'\.0$'), '')}%';

class _StageProgress extends StatelessWidget {
  const _StageProgress(this.stage);
  final _Stage stage;

  @override
  Widget build(BuildContext context) {
    final size = stage.spells.length;
    final captured = stage.spells.where((spell) => spell.captures > 0).length;
    final bar = LinearProgressIndicator(
      value: size == 0 ? 0 : captured / size,
      minHeight: 4,
      borderRadius: BorderRadius.circular(3),
      semanticsLabel: context.strings.spellStageProgress(
        stage.label,
        captured,
        size,
      ),
    );
    final count = Text(
      '$captured/$size',
      style: Theme.of(context).textTheme.labelSmall,
    );
    return LayoutBuilder(
      builder: (context, limits) => limits.maxWidth >= 72
          ? Row(
              spacing: 6,
              children: [
                Expanded(child: bar),
                count,
              ],
            )
          : Column(
              crossAxisAlignment: CrossAxisAlignment.end,
              spacing: 3,
              children: [bar, count],
            ),
    );
  }
}

class _SpellMap extends StatelessWidget {
  const _SpellMap(this.stages, {required this.split});
  final List<_Stage> stages;
  final bool split;

  @override
  Widget build(BuildContext context) => Card.outlined(
    margin: EdgeInsets.zero,
    color: Theme.of(context).colorScheme.surface,
    child: Padding(
      padding: const EdgeInsets.all(10),
      child: LayoutBuilder(
        builder: (context, limits) {
          const columnGap = 5.0;
          const tilesPerSingleRow = 6;
          final compact = limits.maxWidth < 520;
          final baseRowWidth = !compact
              ? 62.0
              : split
              ? 45.0
              : 52.0;
          final baseMidbossWidth = !split
              ? 0.0
              : compact
              ? 46.0
              : 96.0;
          final strings = context.strings;
          final letter = Theme.of(context).textTheme.labelSmall;
          double labelWidth(String value) => TextPainter.computeWidth(
            text: TextSpan(text: value, style: letter),
            textDirection: Directionality.of(context),
            textScaler: MediaQuery.textScalerOf(context),
            maxLines: 1,
          ).ceilToDouble();
          var rowWidth = baseRowWidth;
          for (final text in [
            strings.spellStageColumn,
            ...stages.map((stage) => stage.label),
          ]) {
            for (final word in compact ? text.split(RegExp(r'\s+')) : [text]) {
              rowWidth = rowWidth.clamp(labelWidth(word), double.infinity);
            }
          }
          final midbossWidth = split
              ? baseMidbossWidth.clamp(labelWidth('Midboss'), double.infinity)
              : 0.0;
          final progressWidth = (compact ? 41.0 : 94.0).clamp(
            labelWidth(strings.spellCaptured),
            double.infinity,
          );
          final labelMinimum =
              rowWidth +
              midbossWidth +
              progressWidth +
              (split ? 3 : 2) * columnGap +
              labelWidth(split ? 'Boss' : strings.spellCardColumn)
                  .clamp(20.0, double.infinity);
          // Preserve full column labels when accessibility text exhausts the
          // available width. Only this table scrolls, as in the web archive.
          final tableWidth = limits.maxWidth.clamp(
            labelMinimum,
            double.infinity,
          );
          final bossWidth =
              tableWidth -
              rowWidth -
              midbossWidth -
              progressWidth -
              (split ? 3 : 2) * columnGap;
          final expandedLabels =
              rowWidth > baseRowWidth || midbossWidth > baseMidbossWidth;
          final tileWidth =
              ((bossWidth - (tilesPerSingleRow - 1) * _TileGroup.gap) /
                      tilesPerSingleRow)
                  .floorToDouble()
                  .clamp(
                    expandedLabels ? 20.0.clamp(1.0, bossWidth) : 1.0,
                    54.0,
                  );
          final table = Column(
            crossAxisAlignment: CrossAxisAlignment.stretch,
            spacing: 9,
            children: [
              Row(
                spacing: columnGap,
                children: [
                  SizedBox(
                    width: rowWidth,
                    child: Text(strings.spellStageColumn, style: letter),
                  ),
                  if (split)
                    SizedBox(
                      width: midbossWidth,
                      child: Text(
                        'Midboss',
                        textAlign: TextAlign.center,
                        maxLines: 1,
                        style: letter,
                      ),
                    ),
                  Expanded(
                    child: Text(
                      split ? 'Boss' : strings.spellCardColumn,
                      style: letter,
                    ),
                  ),
                  SizedBox(
                    width: progressWidth,
                    child: Text(
                      strings.spellCaptured,
                      textAlign: TextAlign.end,
                      style: letter,
                    ),
                  ),
                ],
              ),
              for (final stage in stages)
                Row(
                  spacing: columnGap,
                  children: [
                    SizedBox(
                      width: rowWidth,
                      child: Text(stage.label, style: letter),
                    ),
                    if (split)
                      SizedBox(
                        width: midbossWidth,
                        child: _TileGroup(
                          spells: stage.spells
                              .where((spell) => spell.midboss)
                              .toList(),
                          tileWidth: tileWidth,
                          centered: true,
                        ),
                      ),
                    Expanded(
                      child: _TileGroup(
                        spells: stage.spells
                            .where((spell) => !spell.midboss)
                            .toList(),
                        tileWidth: tileWidth,
                      ),
                    ),
                    SizedBox(
                      width: progressWidth,
                      child: _StageProgress(stage),
                    ),
                  ],
                ),
              _SpellLegend(),
            ],
          );
          return SingleChildScrollView(
            scrollDirection: Axis.horizontal,
            primary: false,
            child: SizedBox(width: tableWidth, child: table),
          );
        },
      ),
    ),
  );
}

class _TileGroup extends StatelessWidget {
  const _TileGroup({
    required this.spells,
    required this.tileWidth,
    this.centered = false,
  });
  final List<_Spell> spells;
  final double tileWidth;
  final bool centered;
  static const gap = 4.0;

  @override
  Widget build(BuildContext context) => spells.isEmpty
      ? const Center(child: Text('—'))
      : Wrap(
          alignment: centered ? WrapAlignment.center : WrapAlignment.start,
          spacing: gap,
          runSpacing: gap,
          children: [
            for (final spell in spells)
              _SpellTile(
                key: ValueKey(spell.number),
                spell: spell,
                width: tileWidth,
              ),
          ],
        );
}

class _SpellTile extends StatelessWidget {
  const _SpellTile({super.key, required this.spell, required this.width});

  final _Spell spell;
  final double width;

  @override
  Widget build(BuildContext context) {
    final fraction = _fraction(spell);
    final description = '${_number(spell)} ${spell.name} | $fraction';
    final minimumCountWidth = spell.attempts == 0
        ? double.infinity
        : TextPainter.computeMaxIntrinsicWidth(
            text: TextSpan(
              text: fraction,
              style: Theme.of(context).textTheme.labelSmall
                  ?.copyWith(fontSize: 8),
            ),
            textDirection: Directionality.of(context),
            textScaler: MediaQuery.textScalerOf(context),
          );
    return SizedBox(
      width: width,
      height: 29,
      child: StatisticsHint(
        message: description,
        showOnTap: true,
        child: _SpellPaint(
          attempts: spell.attempts,
          captures: spell.captures,
          child: ExcludeSemantics(
            child: Padding(
              padding: const EdgeInsets.all(2),
              child: Center(
                child: minimumCountWidth > width - 4
                    ? null
                    : FittedBox(fit: BoxFit.scaleDown, child: Text(fraction)),
              ),
            ),
          ),
        ),
      ),
    );
  }
}

class _SpellPaint extends StatelessWidget {
  const _SpellPaint({
    required this.attempts,
    required this.captures,
    this.child,
  });

  final int attempts, captures;
  final Widget? child;

  @override
  Widget build(BuildContext context) {
    final theme = Theme.of(context);
    final colors = theme.colorScheme;
    final ratio = attempts == 0 ? 0.0 : captures / attempts;
    final surface = captures > 0
        ? Color.alphaBlend(
            colors.primary.withValues(alpha: .28 + .72 * ratio),
            colors.surface,
          )
        : attempts > 0
        ? colors.surfaceContainerHighest
        : Colors.transparent;
    final foreground = ratio >= .5 ? colors.onPrimary : colors.onSurface;
    return StatisticsTile(
      fill: surface,
      outline: captures > 0 ? null : colors.outlineVariant,
      dashed: attempts == 0,
      child: DefaultTextStyle(
        style: theme.textTheme.labelSmall!.copyWith(color: foreground),
        child: child ?? const SizedBox(),
      ),
    );
  }
}

class _SpellLegend extends StatelessWidget {
  @override
  Widget build(BuildContext context) => Wrap(
    alignment: WrapAlignment.center,
    spacing: 10,
    runSpacing: 6,
    children: [
      for (final (spells, label) in [
        ([(5, 1), (5, 3), (5, 5)], context.strings.spellCaptured),
        ([(1, 0)], context.strings.spellAttempted),
        ([(0, 0)], context.strings.spellUnknown),
      ])
        Row(
          mainAxisSize: MainAxisSize.min,
          spacing: 4,
          children: [
            for (final (attempts, captures) in spells)
              SizedBox.square(
                dimension: 10,
                child: _SpellPaint(attempts: attempts, captures: captures),
              ),
            Text(label, style: Theme.of(context).textTheme.labelSmall),
          ],
        ),
    ],
  );
}

class _SpellList extends StatelessWidget {
  const _SpellList(this.stages, {required this.split});
  final List<_Stage> stages;
  final bool split;

  @override
  Widget build(BuildContext context) {
    final theme = Theme.of(context);
    final label = theme.textTheme.labelSmall;
    final spells = stages.expand((stage) => stage.spells);
    double widest(Iterable<String> values, double minimum) => values.fold(
      minimum,
      (width, value) => TextPainter.computeWidth(
        text: TextSpan(text: value, style: label),
        textDirection: Directionality.of(context),
        textScaler: MediaQuery.textScalerOf(context),
        maxLines: 1,
      ).ceilToDouble().clamp(width, double.infinity),
    );
    final columns = (
      number: widest(spells.map(_number), 43),
      rate: widest(spells.map(_rate), 52),
      count: widest(spells.map(_fraction), 52),
    );
    return Column(
      crossAxisAlignment: CrossAxisAlignment.stretch,
      spacing: 8,
      children: [
        for (final stage in stages)
          Card.outlined(
            margin: EdgeInsets.zero,
            color: theme.colorScheme.surface,
            child: Padding(
              padding: const EdgeInsets.all(10),
              child: Column(
                crossAxisAlignment: CrossAxisAlignment.stretch,
                spacing: 7,
                children: [
                  Row(
                    children: [
                      Expanded(
                        child: Text(
                          stage.label,
                          style: theme.textTheme.titleSmall,
                        ),
                      ),
                      SizedBox(width: 73, child: _StageProgress(stage)),
                    ],
                  ),
                  if (!split)
                    for (final spell in stage.spells)
                      _SpellListRow(spell, columns)
                  else
                    for (final (group, title) in [
                      (true, 'Midboss'),
                      (false, 'Boss'),
                    ])
                      if (stage.spells.any(
                        (spell) => spell.midboss == group,
                      )) ...[
                        Text(title, style: theme.textTheme.labelSmall),
                        for (final spell in stage.spells.where(
                          (spell) => spell.midboss == group,
                        ))
                          _SpellListRow(spell, columns),
                      ],
                ],
              ),
            ),
          ),
      ],
    );
  }
}

class _SpellListRow extends StatelessWidget {
  const _SpellListRow(this.spell, this.columns);
  final _Spell spell;
  final ({double number, double rate, double count}) columns;

  @override
  Widget build(BuildContext context) {
    final theme = Theme.of(context);
    final label = theme.textTheme.labelSmall;
    final rate = _rate(spell);
    return LayoutBuilder(
      builder: (context, limits) => Row(
        crossAxisAlignment: CrossAxisAlignment.start,
        spacing: 6,
        children: [
          SizedBox(
            width: columns.number,
            child: Text(_number(spell), style: label),
          ),
          Expanded(child: Text(spell.name, style: theme.textTheme.bodySmall)),
          if (limits.maxWidth >= 460)
            SizedBox(
              width: columns.rate,
              child: Text(
                rate,
                textAlign: TextAlign.end,
                style: label,
                maxLines: 1,
                overflow: TextOverflow.ellipsis,
              ),
            ),
          SizedBox(
            width: columns.count,
            child: Tooltip(
              message: _fraction(spell),
              child: Text(
                spell.attempts == 0 ? '—' : _fraction(spell),
                textAlign: TextAlign.end,
                style: label,
              ),
            ),
          ),
        ],
      ),
    );
  }
}
