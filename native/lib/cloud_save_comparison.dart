import 'package:material_ui/material_ui.dart';

import 'cloud_saves.dart';
import 'l10n/strings.dart';
import 'save_progress.dart';

class SaveVersionCards extends StatelessWidget {
  const SaveVersionCards({
    super.key,
    required this.local,
    required this.localStatistics,
    required this.cloud,
    required this.match,
    this.localLoading = false,
  });

  final List<CloudFilePayload> local;
  final SaveStatistics? localStatistics;
  final CloudSaveSnapshot? cloud;
  final bool match;
  final bool localLoading;

  @override
  Widget build(BuildContext context) {
    final remote = cloud;
    final left = _VersionCard(
      title: context.strings.cloudLocalSave,
      dateLabel: context.strings.cloudLocalModified,
      date: local
          .map((file) => file.modifiedAt)
          .whereType<DateTime>()
          .fold<DateTime?>(
            null,
            (last, next) => last == null || next.isAfter(last) ? next : last,
          ),
      files: [
        for (final file in local) (name: file.name, size: file.bytes.length),
      ],
      stats: match ? remote?.record.statistics : localStatistics,
      emptyLabel: localLoading
          ? context.strings.cloudReadingLocal
          : context.strings.cloudNoVersion,
    );
    final right = _VersionCard(
      title: context.strings.cloudAccountSave,
      dateLabel: context.strings.cloudUploadedAt,
      date: remote?.record.uploadedAt,
      files: [
        for (final file in remote?.files ?? const <CloudFileInfo>[])
          (name: file.name, size: file.size),
      ],
      stats: remote?.record.statistics,
      emptyLabel: context.strings.cloudNoVersion,
    );
    return LayoutBuilder(
      builder: (context, space) => Column(
        crossAxisAlignment: CrossAxisAlignment.stretch,
        spacing: 6,
        children: [
          if (match) Text(context.strings.cloudIdentical),
          if (space.maxWidth >= 580)
            IntrinsicHeight(
              child: Row(
                crossAxisAlignment: CrossAxisAlignment.stretch,
                spacing: 12,
                children: [
                  Expanded(child: left),
                  Expanded(child: right),
                ],
              ),
            )
          else
            Column(
              crossAxisAlignment: CrossAxisAlignment.stretch,
              spacing: 12,
              children: [left, right],
            ),
        ],
      ),
    );
  }
}

class _VersionCard extends StatelessWidget {
  const _VersionCard({
    required this.title,
    required this.dateLabel,
    required this.date,
    required this.files,
    required this.stats,
    required this.emptyLabel,
  });

  final String title;
  final String dateLabel;
  final DateTime? date;
  final List<({String name, int size})> files;
  final SaveStatistics? stats;
  final String emptyLabel;

  @override
  Widget build(BuildContext context) {
    const fileListShape = RoundedRectangleBorder(
      borderRadius: BorderRadius.all(Radius.circular(6)),
    );
    final theme = Theme.of(context);
    final locale = MaterialLocalizations.of(context);
    final current = stats;
    final known = current?.progress ?? const <SaveProgress>[];
    final clear = known
        .where((row) => row.cleared == true)
        .map((row) => row.difficulty)
        .toSet();
    final score = known
        .map((row) => row.bestScore)
        .whereType<int>()
        .fold<int?>(
          null,
          (highest, next) => highest == null || next > highest ? next : highest,
        );
    final duration = current?.playtime;
    final count = current?.playCount;
    return Card.outlined(
      color: theme.colorScheme.surface,
      margin: EdgeInsets.zero,
      child: Padding(
        padding: const EdgeInsets.symmetric(horizontal: 16, vertical: 14),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.stretch,
          children: [
            Semantics(
              header: true,
              child: ConstrainedBox(
                constraints: const BoxConstraints(minHeight: 26),
                child: Text(
                  title,
                  style: theme.textTheme.titleSmall?.copyWith(
                    fontSize: 14,
                    fontWeight: FontWeight.w600,
                    height: 1.45,
                  ),
                ),
              ),
            ),
            if (files.isEmpty)
              Padding(
                padding: const EdgeInsets.symmetric(vertical: 10),
                child: Text(
                  emptyLabel,
                  style: theme.textTheme.bodyMedium?.copyWith(
                    fontSize: 13,
                    color: theme.colorScheme.onSurfaceVariant,
                  ),
                ),
              )
            else ...[
              Padding(
                padding: const EdgeInsets.only(top: 3, bottom: 10),
                child: Column(
                  crossAxisAlignment: CrossAxisAlignment.stretch,
                  spacing: 1,
                  children: [
                    Text(
                      dateLabel,
                      style: theme.textTheme.bodySmall?.copyWith(
                        fontSize: 11,
                        color: theme.colorScheme.onSurfaceVariant,
                      ),
                    ),
                    Text(
                      cloudSaveDateTime(context, date),
                      style: theme.textTheme.bodyMedium?.copyWith(fontSize: 13),
                    ),
                  ],
                ),
              ),
              for (final (label, value) in [
                (
                  context.strings.cloudPlaytime,
                  duration == null
                      ? '—'
                      : context.strings.formatPlaytime(duration),
                ),
                (
                  context.strings.cloudPlayCount,
                  count == null ? '—' : locale.formatDecimal(count),
                ),
                (
                  context.strings.cloudClearLevels,
                  clear.isEmpty
                      ? (known.any((entry) => entry.cleared != null)
                            ? context.strings.cloudNotCleared
                            : '—')
                      : clear.map((item) => item.label).join(' | '),
                ),
                (
                  context.strings.cloudBestScore,
                  score == null ? '—' : locale.formatDecimal(score),
                ),
              ])
                Padding(
                  padding: const EdgeInsets.symmetric(vertical: 4),
                  child: _VersionField(label: label, value: value),
                ),
              const SizedBox(height: 7),
              ExpansionTile(
                dense: true,
                tilePadding: EdgeInsets.zero,
                childrenPadding: const EdgeInsets.only(bottom: 4),
                minTileHeight: 44,
                shape: fileListShape,
                collapsedShape: fileListShape,
                title: Text(
                  '${context.strings.cloudFileList} (${files.length})',
                  style: theme.textTheme.bodySmall,
                ),
                children: [
                  for (final file in files)
                    _VersionField(label: file.name, value: saveSize(file.size)),
                ],
              ),
            ],
          ],
        ),
      ),
    );
  }
}

String saveSize(int bytes) =>
    bytes < 1024 ? '$bytes B' : '${(bytes / 1024).toStringAsFixed(1)} KB';

String cloudSaveDateTime(BuildContext context, DateTime? date) {
  if (date == null) return '—';
  final local = date.toLocal();
  final locale = MaterialLocalizations.of(context);
  return '${locale.formatCompactDate(local)} ${locale.formatTimeOfDay(TimeOfDay.fromDateTime(local), alwaysUse24HourFormat: MediaQuery.alwaysUse24HourFormatOf(context))}';
}

class _VersionField extends StatelessWidget {
  const _VersionField({required this.label, required this.value});
  final String label, value;

  @override
  Widget build(BuildContext context) => Row(
    crossAxisAlignment: CrossAxisAlignment.start,
    spacing: 8,
    children: [
      Expanded(
        child: Text(
          label,
          style: Theme.of(context).textTheme.bodyMedium?.copyWith(
            fontSize: 13,
            color: Theme.of(context).colorScheme.onSurfaceVariant,
          ),
        ),
      ),
      Expanded(
        child: Text(
          value,
          textAlign: TextAlign.end,
          style: Theme.of(context).textTheme.bodyMedium?.copyWith(fontSize: 13),
        ),
      ),
    ],
  );
}
