import 'dart:math' as math;

import 'package:fl_chart/fl_chart.dart';
import 'package:intl/intl.dart';
import 'package:material_ui/material_ui.dart';

import 'l10n/strings.dart';
import 'launcher_select.dart';
import 'th09_ratings.dart';

class Th09CharacterPortrait extends StatelessWidget {
  const Th09CharacterPortrait(this.character, {super.key, this.size = 40});

  final int character;
  final double size;

  @override
  Widget build(BuildContext context) => Image.asset(
    'assets/th09-portraits/$character.webp',
    width: size,
    height: size,
    excludeFromSemantics: true,
    fit: BoxFit.contain,
  );
}

class Th09RatingChart extends StatefulWidget {
  const Th09RatingChart({
    super.key,
    required this.points,
    required this.ranks,
    this.character,
    this.preview,
    this.onCloseCharacter,
  });

  final List<Th09Point> points;
  final List<Th09Point> ranks;
  final Th09CharacterRating? character;
  final int? preview;
  final VoidCallback? onCloseCharacter;

  @override
  State<Th09RatingChart> createState() => _Th09RatingChartState();
}

class _Th09RatingChartState extends State<Th09RatingChart> {
  bool _rank = false;
  int? _selectedAt;

  @override
  void didUpdateWidget(covariant Th09RatingChart oldWidget) {
    super.didUpdateWidget(oldWidget);
    if (oldWidget.character != widget.character) _selectedAt = null;
  }

  @override
  Widget build(BuildContext context) => LayoutBuilder(
    builder: (context, bounds) => _content(context, bounds.maxWidth),
  );

  Widget _content(BuildContext context, double width) {
    final character = widget.character;
    final rank =
        character == null &&
        widget.ranks.isNotEmpty &&
        (_rank || widget.points.isEmpty && widget.preview == null);
    final points = rank ? widget.ranks : widget.points;
    final compared = character?.history ?? const <Th09Point>[];
    final strings = context.strings;
    final number = NumberFormat.decimalPattern(strings.localeName);
    final characterName = character == null
        ? null
        : th09CharacterLabel(character.character, strings.localeName);
    final mainTitle = rank
        ? strings.th09RankHistory
        : strings.th09RatingHistory;
    final title = character == null
        ? mainTitle
        : strings.th09CharacterComparison;
    final series = [
      (name: rank ? strings.th09BoardRank : 'WHR', data: points),
      if (characterName != null) (name: characterName, data: compared),
    ];
    final all = [...points, ...compared];
    final timestamps = all.map((point) => point.at).toSet().toList()..sort();
    final selected = timestamps.contains(_selectedAt)
        ? timestamps.indexOf(_selectedAt!)
        : timestamps.length - 1;
    final theme = Theme.of(context);
    final colors = theme.colorScheme;
    final muted = colors.onSurfaceVariant;
    final alt = colors.tertiary;
    final localizations = MaterialLocalizations.of(context);
    DateTime timestamp(int at) =>
        DateTime.fromMillisecondsSinceEpoch(at, isUtc: true);
    String date(int at) => localizations.formatShortDate(timestamp(at));
    final axisFormat =
        timestamps.isNotEmpty &&
            timestamp(timestamps.first).year == timestamp(timestamps.last).year
        ? DateFormat.MMMd(strings.localeName)
        : DateFormat.yMMMd(strings.localeName);
    void select(int at) {
      if (at != _selectedAt) setState(() => _selectedAt = at);
    }

    Widget metric(String label, String value) => Column(
      crossAxisAlignment: CrossAxisAlignment.start,
      children: [
        Text(label, style: theme.textTheme.bodySmall?.copyWith(color: muted)),
        Text(
          value,
          style: theme.textTheme.headlineSmall?.copyWith(
            fontSize: (width * .067).clamp(22.0, 31.0),
            height: 1.15,
            fontWeight: FontWeight.w600,
          ),
        ),
      ],
    );
    final content = Column(
      crossAxisAlignment: CrossAxisAlignment.stretch,
      spacing: 10,
      children: [
        Wrap(
          alignment: WrapAlignment.spaceBetween,
          crossAxisAlignment: WrapCrossAlignment.center,
          spacing: 8,
          runSpacing: 6,
          children: [
            if (character == null)
              Wrap(
                spacing: 18,
                runSpacing: 6,
                children: [
                  if ((widget.points.lastOrNull?.value ?? widget.preview)
                      case final int score)
                    metric('WHR', number.format(score)),
                  if (widget.ranks.lastOrNull?.value case final int position)
                    metric(
                      strings.th09BoardRank,
                      '#${number.format(position)}',
                    ),
                ],
              )
            else
              Row(
                mainAxisSize: MainAxisSize.min,
                children: [
                  if (widget.onCloseCharacter != null)
                    IconButton(
                      tooltip: strings.th09BackToRating,
                      onPressed: widget.onCloseCharacter,
                      icon: const Icon(Icons.arrow_back),
                    ),
                  Flexible(
                    child: Text(title, style: theme.textTheme.titleMedium),
                  ),
                ],
              ),
            if (character == null &&
                widget.ranks.isNotEmpty &&
                (widget.points.isNotEmpty || widget.preview != null))
              LauncherSegments<bool>(
                segments: [
                  const ButtonSegment(value: false, label: Text('WHR')),
                  ButtonSegment(
                    value: true,
                    label: Text(strings.th09BoardRank),
                  ),
                ],
                value: rank,
                onChanged: (value) => setState(() {
                  _rank = value;
                  _selectedAt = null;
                }),
              ),
          ],
        ),
        if (character != null)
          Wrap(
            spacing: 16,
            runSpacing: 4,
            crossAxisAlignment: WrapCrossAlignment.center,
            children: [
              if ((widget.points.lastOrNull?.value ?? widget.preview)
                  case final int score)
                Text(
                  strings.th09CurrentWhr(number.format(score)),
                  style: theme.textTheme.titleMedium,
                ),
              if (widget.ranks.lastOrNull?.value case final int rank)
                Text(
                  strings.th09CurrentRank(number.format(rank)),
                  style: theme.textTheme.titleMedium,
                ),
              Text(
                '$characterName ${number.format(character.score)}',
                style: theme.textTheme.titleMedium?.copyWith(color: alt),
              ),
            ],
          ),
        if (all.isNotEmpty) ...[
          LayoutBuilder(
            builder: (_, bounds) {
              final start = timestamps.first;
              final end = timestamps.last;
              final minX = start == end ? -1.0 : 0.0;
              final maxX = start == end ? 1.0 : (end - start).toDouble();
              final values = all.map((point) => point.value);
              final low = values.reduce(math.min);
              final high = values.reduce(math.max);
              final range = math.max(1, high - low);
              final step = rank
                  ? math.max(1, (range / 4).ceil()).toDouble()
                  : math.max(5, (range / 20).ceil() * 5).toDouble();
              final padding = step * .6;
              final minY = (rank ? -high : low).toDouble() - padding;
              final maxY = (rank ? -low : high).toDouble() + padding;
              return SizedBox(
                height: (bounds.maxWidth * .18).clamp(64.0, 108.0),
                child: ExcludeSemantics(
                  child: LineChart(
                    LineChartData(
                      minX: minX,
                      maxX: maxX,
                      minY: minY,
                      maxY: maxY,
                      titlesData: const FlTitlesData(show: false),
                      gridData: const FlGridData(show: false),
                      borderData: FlBorderData(show: false),
                      extraLinesData: ExtraLinesData(
                        verticalLines: [
                          VerticalLine(
                            x: (timestamps[selected] - start).toDouble(),
                            color: muted,
                            strokeWidth: 1,
                            dashArray: [3, 4],
                          ),
                        ],
                      ),
                      lineTouchData: LineTouchData(
                        handleBuiltInTouches: false,
                        touchCallback: (event, response) {
                          if (!event.isInterestedForInteractions) return;
                          final spots = response?.lineBarSpots;
                          if (spots != null && spots.isNotEmpty) {
                            select(start + spots.first.x.round());
                          }
                        },
                      ),
                      lineBarsData: [
                        for (final (index, entry) in series.indexed)
                          if (entry.data.isNotEmpty)
                            LineChartBarData(
                              spots: [
                                for (final point in entry.data)
                                  FlSpot(
                                    (point.at - start).toDouble(),
                                    (rank ? -point.value : point.value)
                                        .toDouble(),
                                  ),
                              ],
                              color: index == 0 ? colors.primary : alt,
                              barWidth: 2,
                              isCurved: true,
                              preventCurveOverShooting: true,
                              dotData: FlDotData(
                                show: true,
                                checkToShowDot: (spot, _) =>
                                    entry.data.length == 1 ||
                                    spot.x == timestamps[selected] - start,
                                getDotPainter: (spot, _, bar, _) =>
                                    FlDotCirclePainter(
                                      radius: 3,
                                      color: colors.surface,
                                      strokeWidth: 1.5,
                                      strokeColor: bar.color ?? colors.primary,
                                    ),
                              ),
                            ),
                      ],
                    ),
                    duration: Duration.zero,
                  ),
                ),
              );
            },
          ),
          Row(
            mainAxisAlignment: MainAxisAlignment.spaceBetween,
            children: [
              Flexible(
                child: Text(
                  axisFormat.format(timestamp(timestamps.first)),
                  style: theme.textTheme.labelSmall,
                ),
              ),
              if (timestamps.length > 1)
                Flexible(
                  child: Text(
                    axisFormat.format(timestamp(timestamps.last)),
                    style: theme.textTheme.labelSmall,
                    textAlign: TextAlign.end,
                  ),
                ),
            ],
          ),
          Row(
            children: [
              IconButton(
                tooltip: strings.th09PreviousPoint,
                onPressed: selected > 0
                    ? () => select(timestamps[selected - 1])
                    : null,
                icon: const Icon(Icons.chevron_left),
              ),
              Expanded(
                child: Semantics(
                  liveRegion: true,
                  child: Wrap(
                    alignment: WrapAlignment.center,
                    spacing: 10,
                    runSpacing: 4,
                    children: [
                      Text(date(timestamps[selected])),
                      for (final (index, entry) in series.indexed)
                        if (entry.data
                                .where((p) => p.at <= timestamps[selected])
                                .lastOrNull
                            case final Th09Point point)
                          Text(
                            '${entry.name} ${number.format(point.value)}',
                            style: TextStyle(
                              color: index == 0 ? colors.primary : alt,
                            ),
                          ),
                    ],
                  ),
                ),
              ),
              IconButton(
                tooltip: strings.th09NextPoint,
                onPressed: selected + 1 < timestamps.length
                    ? () => select(timestamps[selected + 1])
                    : null,
                icon: const Icon(Icons.chevron_right),
              ),
            ],
          ),
        ],
      ],
    );
    return Semantics(
      container: true,
      explicitChildNodes: true,
      label: title,
      child: character == null
          ? content
          : Card(
              margin: EdgeInsets.zero,
              shape: RoundedRectangleBorder(
                borderRadius: BorderRadius.circular(12),
                side: BorderSide(color: colors.outlineVariant),
              ),
              elevation: 0,
              child: Padding(padding: const EdgeInsets.all(12), child: content),
            ),
    );
  }
}
