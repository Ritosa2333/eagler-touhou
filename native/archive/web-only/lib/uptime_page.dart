import 'dart:async';
import 'dart:convert';
import 'dart:math' as math;
import 'dart:typed_data';
import 'dart:ui' show FontFeature;

import 'package:http/http.dart' as http;
import 'package:material_ui/material_ui.dart';

import 'l10n/strings.dart';
import 'launcher_background.dart';
import 'launcher_select.dart';
import 'profile_social.dart';

enum UptimeRange {
  day('24h'),
  week('7d'),
  quarter('90d');

  const UptimeRange(this.key);
  final String key;

  String label(LauncherStrings strings) => switch (this) {
    day => strings.uptimeDay,
    week => strings.uptimeWeek,
    quarter => strings.uptimeQuarter,
  };
}

enum UptimeGroup {
  site,
  download,
  netplay,
  turn;

  String label(LauncherStrings strings) => switch (this) {
    site => strings.uptimeSite,
    download => strings.uptimeDownload,
    netplay => strings.uptimeNetplay,
    turn => strings.uptimeTurn,
  };
}

typedef UptimePoint = ({
  DateTime at,
  int samples,
  double? availability,
  double? latency,
});
typedef UptimeService = ({
  String id,
  String label,
  UptimeGroup group,
  String status,
  DateTime? checkedAt,
  double? latency,
  double? availability,
  List<UptimePoint> points,
});

class UptimeSummary {
  const UptimeSummary(this.range, this.updatedAt, this.services);
  final UptimeRange range;
  final DateTime updatedAt;
  final List<UptimeService> services;

  factory UptimeSummary.decode(String text, UptimeRange range) {
    final value = jsonDecode(text);
    if (value is! Map<String, dynamic> ||
        value['schema'] != 'eagler-uptime/1' ||
        value['range'] != range.key) {
      throw const FormatException('Unexpected uptime response');
    }
    final rows = _list(value['services'], 24);
    final services = <UptimeService>[];
    final ids = <String>{};
    for (final row in rows) {
      final id = _string(row['id'], 128);
      final status = row['status'];
      final points = <UptimePoint>[];
      final times = <DateTime>{};
      if (!ids.add(id) ||
          status is! String ||
          !const [
            'operational',
            'outage',
            'stale',
            'unknown',
          ].contains(status)) {
        throw const FormatException('Invalid uptime service');
      }
      for (final item in _list(row['points'], 90)) {
        final at = _date(item['at']);
        final samples = item['samples'];
        if (!times.add(at) ||
            samples is! int ||
            samples < 0 ||
            samples > 9007199254740991) {
          throw const FormatException('Invalid uptime sample');
        }
        points.add((
          at: at,
          samples: samples,
          availability: _number(item['availability'], max: 1),
          latency: _number(item['latencyMs']),
        ));
      }
      points.sort((a, b) => a.at.compareTo(b.at));
      final group = UptimeGroup.values
          .where((group) => group.name == row['group'])
          .firstOrNull;
      if (group == null) throw const FormatException('Invalid uptime group');
      services.add((
        id: id,
        label: _string(row['label'], 160),
        group: group,
        status: status,
        checkedAt: row['checkedAt'] == null ? null : _date(row['checkedAt']),
        latency: _number(row['latencyMs']),
        availability: _number(row['availability'], max: 1),
        points: points,
      ));
    }
    return UptimeSummary(range, _date(value['updatedAt']), services);
  }

  static List<Map<String, dynamic>> _list(Object? list, int max) {
    if (list is! List ||
        list.length > max ||
        list.any((item) => item is! Map<String, dynamic>)) {
      throw const FormatException('Invalid uptime list');
    }
    return list.cast<Map<String, dynamic>>();
  }

  static String _string(Object? value, int max) {
    if (value is String && value.isNotEmpty && value.length <= max) {
      return value;
    }
    throw const FormatException('Invalid uptime string');
  }

  static DateTime _date(Object? value) {
    final date = DateTime.tryParse(_string(value, 64));
    if (date != null) return date.toUtc();
    throw const FormatException('Invalid uptime date');
  }

  static double? _number(Object? value, {double? max}) {
    if (value == null) return null;
    if (value is num &&
        value.isFinite &&
        value >= 0 &&
        (max == null || value <= max)) {
      return value.toDouble();
    }
    throw const FormatException('Invalid uptime measurement');
  }

  static Future<UptimeSummary> read(UptimeRange range) async {
    final client = http.Client();
    final address = Uri.https('touhouonline.com', '/uptime/summary', {
      'range': range.key,
    });
    final request = http.Request('GET', address)
      ..followRedirects = false
      ..headers.addAll({
        'Accept': 'application/json',
        'Cache-Control': 'no-cache',
      });
    Future<UptimeSummary> fetch() async {
      final response = await client.send(request);
      if (response.statusCode != 200) {
        throw const FormatException('Status service unavailable');
      }
      final data = BytesBuilder(copy: false);
      await for (final chunk in response.stream) {
        if (data.length + chunk.length > 1024 * 1024) {
          throw const FormatException('Status response too large');
        }
        data.add(chunk);
      }
      return UptimeSummary.decode(utf8.decode(data.takeBytes()), range);
    }

    try {
      return await fetch().timeout(const Duration(seconds: 10));
    } finally {
      client.close();
    }
  }
}

typedef UptimeSource = Future<UptimeSummary> Function(UptimeRange range);

class UptimePage extends StatefulWidget {
  const UptimePage({super.key, this.read = UptimeSummary.read});
  final UptimeSource read;

  @override
  State<UptimePage> createState() => _UptimePageState();
}

class _UptimePageState extends State<UptimePage> with WidgetsBindingObserver {
  final _cache = <UptimeRange, UptimeSummary>{};
  UptimeRange _range = UptimeRange.day;
  UptimeSummary? _last;
  late final Timer _refreshTimer;
  bool _pending = false;
  bool _failed = false;
  int _request = 0;
  bool _active = true;

  @override
  void initState() {
    super.initState();
    WidgetsBinding.instance.addObserver(this);
    _refreshTimer = Timer.periodic(const Duration(seconds: 30), (_) {
      if (_active && ModalRoute.of(context)?.isCurrent == true) _load();
    });
    _load();
  }

  @override
  void didChangeAppLifecycleState(AppLifecycleState state) {
    _active = state == AppLifecycleState.resumed;
    if (_active && ModalRoute.of(context)?.isCurrent == true) _load();
  }

  @override
  void dispose() {
    _refreshTimer.cancel();
    WidgetsBinding.instance.removeObserver(this);
    super.dispose();
  }

  Future<void> _load({UptimeRange? select}) async {
    if (_pending && select == null) return;
    final range = select ?? _range;
    final request = ++_request;
    setState(() {
      _range = range;
      _pending = true;
      _failed = false;
    });
    try {
      final data = await widget.read(range);
      if (data.range != range) {
        throw const FormatException('Unexpected uptime range');
      }
      if (!mounted || _request != request) return;
      setState(() {
        _cache[range] = data;
        _last = data;
      });
    } catch (_) {
      if (mounted && _request == request) setState(() => _failed = true);
    } finally {
      if (mounted && _request == request) setState(() => _pending = false);
    }
  }

  @override
  Widget build(BuildContext context) {
    final strings = context.strings;
    final data = _cache[_range] ?? _last;
    final inset = MediaQuery.sizeOf(context).width <= 780 ? 16.0 : 24.0;
    return Scaffold(
      appBar: AppBar(
        leading: BackButton(onPressed: () => profileBack(context, '/')),
        title: Text(strings.uptimeTitle),
        actions: [
          IconButton(
            tooltip: strings.uptimeRefresh,
            onPressed: _pending ? null : _load,
            icon: const Icon(Icons.refresh),
          ),
        ],
      ),
      body: SafeArea(
        child: Center(
          child: ConstrainedBox(
            constraints: const BoxConstraints(maxWidth: 1160),
            child: Column(
              crossAxisAlignment: CrossAxisAlignment.stretch,
              children: [
                Padding(
                  padding: EdgeInsets.fromLTRB(inset, 8, inset, 0),
                  child: Align(
                    alignment: AlignmentDirectional.centerEnd,
                    child: SizedBox(
                      width: 360,
                      child: LayoutBuilder(
                        builder: (context, box) {
                          final fullLabels = UptimeRange.values.every(
                            (range) =>
                                (LauncherControl.labelWidth(
                                          context,
                                          range.label(strings),
                                        ) +
                                        24) *
                                    UptimeRange.values.length <=
                                box.maxWidth,
                          );
                          return Semantics(
                            container: true,
                            label: strings.uptimeHistoryRange,
                            child: LauncherSegments<UptimeRange>(
                              segments: [
                                for (final range in UptimeRange.values)
                                  ButtonSegment(
                                    value: range,
                                    tooltip: range.label(strings),
                                    label: Text(
                                      fullLabels
                                          ? range.label(strings)
                                          : range.key,
                                      maxLines: 1,
                                    ),
                                  ),
                              ],
                              value: _range,
                              onChanged: (range) {
                                if (_range != range) {
                                  _load(select: range);
                                }
                              },
                            ),
                          );
                        },
                      ),
                    ),
                  ),
                ),
                Padding(
                  padding: EdgeInsets.symmetric(horizontal: inset),
                  child: ConstrainedBox(
                    constraints: const BoxConstraints(minHeight: 44),
                    child: Row(
                      spacing: 12,
                      children: [
                        Expanded(
                          child: _failed
                              ? Semantics(
                                  liveRegion: true,
                                  child: Text(
                                    strings.uptimeFailed,
                                    style: Theme.of(context).textTheme.bodySmall
                                        ?.copyWith(
                                          color: Theme.of(context)
                                              .colorScheme
                                              .error,
                                        ),
                                  ),
                                )
                              : _pending
                              ? const LinearProgressIndicator(minHeight: 2)
                              : const SizedBox(height: 2),
                        ),
                        if (_failed)
                          TextButton(
                            style: LauncherControl.style(context),
                            onPressed: _load,
                            child: Text(strings.uptimeRetry),
                          ),
                      ],
                    ),
                  ),
                ),
                if (data != null)
                  Expanded(
                    child: ListView(
                      key: const PageStorageKey('uptime-list'),
                      padding: EdgeInsets.fromLTRB(inset, 8, inset, 24),
                      children: [
                        _UptimeTable(data: data, failed: _failed),
                        Padding(
                          padding: const EdgeInsets.only(top: 16),
                          child: Text(
                            strings.uptimeUpdated(
                              _localDate(context, data.updatedAt),
                            ),
                            style: Theme.of(context).textTheme.labelSmall
                                ?.copyWith(
                                  color: Theme.of(context)
                                      .colorScheme
                                      .onSurfaceVariant,
                                ),
                          ),
                        ),
                      ],
                    ),
                  )
                else if (_pending)
                  Expanded(child: Center(child: Text(strings.uptimeLoading))),
              ],
            ),
          ),
        ),
      ),
    );
  }
}

String _availability(double? fraction) =>
    fraction == null ? '—' : '${(fraction * 100).toStringAsFixed(2)}%';
String _latency(double? value) => value == null ? '—' : '${value.round()} ms';
String _localDate(BuildContext context, DateTime value) {
  final labels = MaterialLocalizations.of(context);
  final local = value.toLocal();
  return '${labels.formatCompactDate(local)} ${labels.formatTimeOfDay(TimeOfDay.fromDateTime(local))}';
}

typedef _ColumnWidths = ({double status, double latency, double availability});

TextStyle _serviceText(BuildContext context, bool wide) =>
    Theme.of(context).textTheme.bodyMedium!.copyWith(
      fontSize: wide ? 14 : 13,
      height: 1.4,
      fontFeatures: const [FontFeature.tabularFigures()],
    );

class _UptimeTable extends StatelessWidget {
  const _UptimeTable({required this.data, required this.failed});
  final UptimeSummary data;
  final bool failed;

  @override
  Widget build(BuildContext context) => LayoutBuilder(
    builder: (context, box) {
      final strings = context.strings;
      final theme = Theme.of(context);
      final colors = theme.colorScheme;
      final heading = theme.textTheme.bodySmall!.copyWith(
        fontSize: 12,
        height: 1.4,
        color: colors.onSurfaceVariant,
      );
      double widest(Iterable<String> values, TextStyle style, double minimum) =>
          values.fold<double>(
            minimum,
            (width, value) => math.max(
              width,
              TextPainter.computeWidth(
                text: TextSpan(text: value, style: style),
                textDirection: Directionality.of(context),
                textScaler: MediaQuery.textScalerOf(context),
                maxLines: 1,
              ).ceilToDouble(),
            ),
          );
      _ColumnWidths columns(bool wide) {
        final body = _serviceText(context, wide);
        final status = body.copyWith(fontSize: wide ? 13 : 12);
        return (
          status: widest(
            [strings.uptimeNormal, strings.uptimeOutage, strings.uptimeUnknown],
            status,
            wide ? 80 : 44,
          ),
          latency: widest(
            data.services.map((row) => _latency(row.latency)),
            body,
            wide ? widest([strings.uptimeResponse], heading, 90) : 64,
          ),
          availability: widest(
            data.services.map((row) => _availability(row.availability)),
            body,
            wide ? widest([strings.uptimeAvailability], heading, 100) : 68,
          ),
        );
      }

      final desktop = columns(true);
      final wide =
          box.maxWidth >=
          desktop.status +
              desktop.latency +
              desktop.availability +
              130 +
              180 +
              64 +
              40;
      final widths = wide ? desktop : columns(false);
      final groups = UptimeGroup.values
          .where((group) => data.services.any((row) => row.group == group))
          .toList();
      return Card.outlined(
        key: const ValueKey('uptime-services'),
        margin: EdgeInsets.zero,
        clipBehavior: Clip.antiAlias,
        color: LauncherBackdrop.tint(
          context,
          BackgroundLayer.panel,
          colors.surfaceContainerLow,
        ),
        shape: RoundedRectangleBorder(
          borderRadius: BorderRadius.circular(8),
          side: BorderSide(color: colors.outlineVariant),
        ),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.stretch,
          children: [
            if (wide)
              Padding(
                padding: const EdgeInsets.symmetric(
                  horizontal: 20,
                  vertical: 16,
                ),
                child: ExcludeSemantics(
                  child: DefaultTextStyle(
                    style: heading,
                    child: _ServiceColumns(
                      widths: widths,
                      children: [
                        Text(strings.uptimeStatus),
                        Text(strings.uptimeService),
                        Text(strings.uptimeHistory(data.range.key)),
                        Text(strings.uptimeResponse, textAlign: TextAlign.end),
                        Text(
                          strings.uptimeAvailability,
                          textAlign: TextAlign.end,
                        ),
                      ],
                    ),
                  ),
                ),
              ),
            for (final group in groups) ...[
              DecoratedBox(
                decoration: BoxDecoration(
                  color: LauncherBackdrop.tint(
                    context,
                    BackgroundLayer.panel,
                    colors.surfaceContainer,
                  ),
                  border: Border(
                    top: wide || group != groups.first
                        ? BorderSide(color: colors.outlineVariant)
                        : BorderSide.none,
                  ),
                ),
                child: Padding(
                  padding: EdgeInsets.symmetric(
                    horizontal: wide ? 20 : 14,
                    vertical: 10,
                  ),
                  child: Semantics(
                    header: true,
                    child: Text(
                      group.label(strings),
                      style: theme.textTheme.bodySmall?.copyWith(
                        fontSize: 13,
                        height: 1.5,
                        fontWeight: FontWeight.w600,
                      ),
                    ),
                  ),
                ),
              ),
              for (final service in data.services.where(
                (row) => row.group == group,
              ))
                _ServiceHistory(
                  key: ValueKey((data.range, service.id)),
                  service: service,
                  range: data.range,
                  failed: failed,
                  wide: wide,
                  columns: widths,
                ),
            ],
            if (data.services.isEmpty)
              Padding(
                padding: const EdgeInsets.all(24),
                child: Text(strings.uptimeEmpty),
              ),
          ],
        ),
      );
    },
  );
}

/// The heading and every service share these five column boundaries.
class _ServiceColumns extends StatelessWidget {
  const _ServiceColumns({required this.widths, required this.children});
  final _ColumnWidths widths;
  final List<Widget> children;

  @override
  Widget build(BuildContext context) => Row(
    spacing: 16,
    children: [
      SizedBox(width: widths.status, child: children[0]),
      Expanded(flex: 10, child: children[1]),
      Expanded(flex: 14, child: children[2]),
      SizedBox(width: widths.latency, child: children[3]),
      SizedBox(width: widths.availability, child: children[4]),
    ],
  );
}

class _ServiceHistory extends StatefulWidget {
  const _ServiceHistory({
    super.key,
    required this.service,
    required this.range,
    required this.failed,
    required this.wide,
    required this.columns,
  });
  final UptimeService service;
  final UptimeRange range;
  final bool failed;
  final bool wide;
  final _ColumnWidths columns;

  @override
  State<_ServiceHistory> createState() => _ServiceHistoryState();
}

class _ServiceHistoryState extends State<_ServiceHistory> {
  int? _sample;

  void _selected(int? index) => setState(() => _sample = index);

  @override
  Widget build(BuildContext context) {
    final service = widget.service;
    final strings = context.strings;
    final colors = Theme.of(context).colorScheme;
    final txt = Theme.of(context).textTheme;
    final age = DateTime.now().difference(
      service.checkedAt ?? DateTime.fromMillisecondsSinceEpoch(0),
    );
    final stale = widget.failed || age.inSeconds > 90 || age.inSeconds < -30;
    final status = stale ? 'unknown' : service.status;
    final history = service.points;
    final index = history.isEmpty || _sample == null
        ? null
        : _sample!.clamp(0, history.length - 1);
    final dark = Theme.of(context).brightness == Brightness.dark;
    final healthy = dark ? const Color(0xFF8FDAB0) : const Color(0xFF2C754A);
    final partial = dark ? const Color(0xFFE5BA73) : const Color(0xFF936015);
    final (label, color) = switch (status) {
      'operational' => (strings.uptimeNormal, healthy),
      'outage' => (strings.uptimeOutage, colors.error),
      _ => (strings.uptimeUnknown, colors.onSurfaceVariant),
    };
    String describe(UptimePoint point) =>
        '${_localDate(context, point.at)} | '
        '${_availability(point.availability)} | ${_latency(point.latency)}';
    Color band(double? ratio) => switch (ratio) {
      null => colors.surfaceContainerHighest,
      0 => colors.error,
      1 => healthy,
      _ => partial,
    };

    final body = _serviceText(context, widget.wide);
    final state = Text(
      label,
      key: ValueKey(('uptime-status', service.id)),
      style: body.copyWith(fontSize: widget.wide ? 13 : 12, color: color),
    );
    final name = Text(
      service.label,
      key: ValueKey(('uptime-name', service.id)),
      style: body.copyWith(fontWeight: FontWeight.w500),
    );
    Widget metric(String label, String value, String column) => Semantics(
      label: '$label $value',
      excludeSemantics: true,
      child: Text(
        value,
        key: ValueKey(('uptime-$column', service.id)),
        textAlign: TextAlign.end,
        style: body,
      ),
    );
    final latency = metric(
      strings.uptimeResponse,
      _latency(stale ? null : service.latency),
      'latency',
    );
    final availability = Tooltip(
      message: strings.uptimeAvailabilityHint,
      child: metric(
        strings.uptimeAvailability,
        _availability(service.availability),
        'availability',
      ),
    );
    final iconStyle = IconButton.styleFrom(
      minimumSize: const Size(44, 44),
      tapTargetSize: MaterialTapTargetSize.shrinkWrap,
      shape: LauncherControl.shape,
      foregroundColor: colors.onSurfaceVariant,
    );
    final chart = Row(
      children: [
        Expanded(
          child: Semantics(
            key: ValueKey(('uptime-history', service.id)),
            label:
                '${service.label} | ${strings.uptimeHistory(widget.range.label(strings))} | '
                '${strings.uptimeSampleCount(history.length)}',
            child: SizedBox(
              height: widget.wide ? 24 : 14,
              child: history.isEmpty
                  ? Align(
                      alignment: AlignmentDirectional.centerStart,
                      child: Text(
                        strings.uptimeNoHistory,
                        style: txt.bodySmall,
                      ),
                    )
                  : LayoutBuilder(
                      builder: (context, box) {
                        final gap = history.length < 2
                            ? 0.0
                            : ((box.maxWidth - history.length * 2) /
                                      (history.length - 1))
                                  .clamp(0.0, 3.0);
                        return Row(
                          spacing: gap,
                          children: [
                            for (final (position, point) in history.indexed)
                              Expanded(
                                child: Tooltip(
                                  message: describe(point),
                                  child: GestureDetector(
                                    onTap: () => _selected(position),
                                    child: Container(
                                      key: ValueKey((
                                        'uptime-point',
                                        service.id,
                                        position,
                                      )),
                                      height: widget.wide ? 24 : 14,
                                      decoration: BoxDecoration(
                                        color: band(point.availability),
                                        borderRadius: BorderRadius.circular(2),
                                        border: position == index
                                            ? Border.all(
                                                color: colors.onSurface,
                                              )
                                            : null,
                                      ),
                                    ),
                                  ),
                                ),
                              ),
                          ],
                        );
                      },
                    ),
            ),
          ),
        ),
        if (history.isNotEmpty)
          MergeSemantics(
            key: ValueKey(('uptime-sample-button', service.id)),
            child: Semantics(
              label:
                  '${index == null ? strings.uptimeShowSample : strings.uptimeHideSample} | ${service.label}',
              child: IconButton(
                style: iconStyle,
                tooltip: index == null
                    ? strings.uptimeShowSample
                    : strings.uptimeHideSample,
                onPressed: () =>
                    _selected(index == null ? history.length - 1 : null),
                icon: Icon(
                  index == null ? Icons.history : Icons.close,
                  size: 18,
                ),
              ),
            ),
          ),
      ],
    );
    return DecoratedBox(
      decoration: BoxDecoration(
        border: Border(top: BorderSide(color: colors.outlineVariant)),
      ),
      child: ConstrainedBox(
        constraints: BoxConstraints(minHeight: widget.wide ? 72 : 84),
        child: Padding(
          padding: EdgeInsets.symmetric(
            horizontal: widget.wide ? 20 : 14,
            vertical: 14,
          ),
          child: Column(
            crossAxisAlignment: CrossAxisAlignment.stretch,
            children: [
              if (widget.wide)
                _ServiceColumns(
                  widths: widget.columns,
                  children: [state, name, chart, latency, availability],
                )
              else
                LayoutBuilder(
                  builder: (context, box) {
                    final columns = widget.columns;
                    final inline =
                        box.maxWidth >=
                        columns.status +
                            columns.latency +
                            columns.availability +
                            24 +
                            56;
                    return Column(
                      crossAxisAlignment: CrossAxisAlignment.stretch,
                      children: [
                        if (inline)
                          Row(
                            spacing: 8,
                            children: [
                              SizedBox(width: columns.status, child: state),
                              Expanded(child: name),
                              SizedBox(width: columns.latency, child: latency),
                              SizedBox(
                                width: columns.availability,
                                child: availability,
                              ),
                            ],
                          )
                        else ...[
                          Row(
                            spacing: 8,
                            children: [
                              SizedBox(width: columns.status, child: state),
                              Expanded(child: name),
                            ],
                          ),
                          const SizedBox(height: 8),
                          Wrap(
                            alignment: WrapAlignment.spaceBetween,
                            spacing: 8,
                            runSpacing: 4,
                            children: [
                              SizedBox(width: columns.latency, child: latency),
                              SizedBox(
                                width: columns.availability,
                                child: availability,
                              ),
                            ],
                          ),
                        ],
                        const SizedBox(height: 3),
                        chart,
                      ],
                    );
                  },
                ),
              if (index != null)
                Row(
                  children: [
                    IconButton(
                      style: iconStyle,
                      tooltip: strings.uptimePreviousSample,
                      onPressed: index > 0 ? () => _selected(index - 1) : null,
                      icon: const Icon(Icons.chevron_left),
                    ),
                    Expanded(
                      child: Semantics(
                        liveRegion: true,
                        child: Text(
                          describe(history[index]),
                          textAlign: TextAlign.center,
                          style: txt.bodySmall,
                        ),
                      ),
                    ),
                    IconButton(
                      style: iconStyle,
                      tooltip: strings.uptimeNextSample,
                      onPressed: index + 1 < history.length
                          ? () => _selected(index + 1)
                          : null,
                      icon: const Icon(Icons.chevron_right),
                    ),
                  ],
                ),
            ],
          ),
        ),
      ),
    );
  }
}
