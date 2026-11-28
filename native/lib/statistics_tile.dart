import 'package:material_ui/material_ui.dart';

class StatisticsHint extends StatefulWidget {
  const StatisticsHint({
    super.key,
    required this.message,
    this.semanticLabel,
    this.selected,
    this.onTap,
    this.showOnTap = false,
    required this.child,
  });

  final String message;
  final String? semanticLabel;
  final bool? selected;
  final VoidCallback? onTap;
  final bool showOnTap;
  final Widget child;

  @override
  State<StatisticsHint> createState() => _StatisticsHintState();
}

class _StatisticsHintState extends State<StatisticsHint> {
  final _tip = GlobalKey<TooltipState>();
  bool _keyboardFocused = false;

  void _focusChanged(bool focused) {
    if (!focused) {
      if (_keyboardFocused) Tooltip.dismissAllToolTips();
      _keyboardFocused = false;
      return;
    }
    if (FocusManager.instance.highlightMode != FocusHighlightMode.traditional) {
      return;
    }
    _keyboardFocused = true;
    // Defer until the previous cell has processed its focus loss.
    WidgetsBinding.instance.addPostFrameCallback((_) {
      if (mounted && _keyboardFocused) {
        _tip.currentState?.ensureTooltipVisible();
      }
    });
  }

  @override
  Widget build(BuildContext context) {
    final interactive = widget.onTap != null || widget.showOnTap;
    final theme = Theme.of(context);
    final colors = theme.colorScheme;
    return Tooltip(
      key: _tip,
      message: widget.message,
      excludeFromSemantics: true,
      margin: const EdgeInsets.symmetric(horizontal: 12),
      constraints: const BoxConstraints(minHeight: 28, maxWidth: 320),
      padding: const EdgeInsets.symmetric(horizontal: 9, vertical: 5),
      decoration: BoxDecoration(
        color: colors.surfaceContainerHighest,
        border: Border.all(color: colors.outlineVariant),
        borderRadius: BorderRadius.circular(6),
        boxShadow: [
          BoxShadow(
            color: colors.shadow.withValues(alpha: .18),
            blurRadius: 12,
            offset: const Offset(0, 3),
          ),
        ],
      ),
      textStyle: theme.textTheme.bodySmall?.copyWith(color: colors.onSurface),
      child: Semantics(
        label: widget.semanticLabel ?? widget.message,
        button: interactive,
        selected: widget.selected,
        child: interactive
            ? InkWell(
                borderRadius: BorderRadius.circular(5),
                onFocusChange: _focusChanged,
                onTap: () {
                  widget.onTap?.call();
                  if (widget.showOnTap) {
                    _tip.currentState?.ensureTooltipVisible();
                  }
                },
                child: widget.child,
              )
            : widget.child,
      ),
    );
  }
}

class StatisticsTile extends StatelessWidget {
  const StatisticsTile({
    super.key,
    required this.fill,
    this.outline,
    this.dashed = false,
    this.radius = 5,
    this.child,
  });

  final Color fill;
  final Color? outline;
  final bool dashed;
  final double radius;
  final Widget? child;

  @override
  Widget build(BuildContext context) {
    return Ink(
      decoration: BoxDecoration(
        color: fill,
        borderRadius: BorderRadius.circular(radius),
        border: outline == null || dashed ? null : Border.all(color: outline!),
      ),
      child: CustomPaint(
        foregroundPainter: dashed && outline != null
            ? _Outline(outline!, radius)
            : null,
        child: child ?? const SizedBox.expand(),
      ),
    );
  }
}

class _Outline extends CustomPainter {
  const _Outline(this.color, this.radius);
  final Color color;
  final double radius;

  @override
  void paint(Canvas canvas, Size size) {
    if (size.shortestSide <= 1) return;
    final edge = Path()
      ..addRRect(
        RRect.fromRectAndRadius(
          (Offset.zero & size).deflate(.5),
          Radius.circular(radius),
        ),
      );
    final paint = Paint()
      ..color = color
      ..style = PaintingStyle.stroke;
    for (final segment in edge.computeMetrics()) {
      for (var start = 0.0; start < segment.length; start += 4) {
        canvas.drawPath(segment.extractPath(start, start + 2.2), paint);
      }
    }
  }

  @override
  bool shouldRepaint(_Outline oldDelegate) =>
      color != oldDelegate.color || radius != oldDelegate.radius;
}
