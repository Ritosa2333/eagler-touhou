import 'dart:math' as math;

import 'package:flutter/gestures.dart';
import 'package:material_ui/material_ui.dart';

import 'catalog.dart';
import 'l10n/strings.dart';
import 'touch_preferences.dart';

extension TouchControlLabels on TouchControl {
  String labelFor(String gameId, LauncherStrings strings) {
    if (gameId == 'th075') {
      switch (this) {
        case TouchControl.fire:
          return 'A ${strings.touchEditorLight}';
        case TouchControl.focus:
          return 'B ${strings.touchEditorHeavy}';
        case TouchControl.bomb:
          return 'C ${strings.touchEditorProjectile}';
        case TouchControl.function:
          return 'D ${strings.touchEditorDash}';
        default:
          break;
      }
    }
    return switch (this) {
      TouchControl.focus =>
        gameId == 'th01' ? strings.touchEditorSlide : strings.touchEditorFocus,
      TouchControl.fire => strings.touchEditorFire,
      TouchControl.bomb => 'BOMB',
      TouchControl.joystick => strings.touchEditorStick,
      TouchControl.escape => 'ESC',
      TouchControl.restart => 'R',
      TouchControl.function =>
        const {'th03', 'th19'}.contains(gameId)
            ? strings.touchEditorCharge
            : strings.touchEditorSpecial,
      TouchControl.thpracTab => 'Tab',
      TouchControl.thpracMenu => strings.touchEditorCheatMenu,
    };
  }
}

class TouchLayoutPreview extends StatelessWidget {
  const TouchLayoutPreview({
    super.key,
    required this.game,
    required this.template,
    required this.profile,
    required this.controls,
    required this.selected,
    required this.opacity,
    required this.adjustingViewport,
    required this.sensitivity,
    required this.onSelect,
    required this.onMove,
    required this.onResize,
    required this.onViewport,
  });

  final Game game;
  final TouchTemplate template;
  final TouchProfile profile;
  final List<TouchControl> controls;
  final TouchControl selected;
  final double opacity;
  final bool adjustingViewport;
  final double? sensitivity;
  final ValueChanged<TouchControl> onSelect;
  final void Function(TouchControl, Offset) onMove;
  final void Function(TouchControl, double) onResize;
  final ValueChanged<double> onViewport;

  @override
  Widget build(BuildContext context) => LayoutBuilder(
    builder: (context, constraints) {
      final reference = template.referenceSize;
      final factor = math.min(
        constraints.maxWidth / reference.width,
        constraints.maxHeight / reference.height,
      );
      final size = reference * factor;
      if (size.width < 32 || size.height < 28) {
        return const SizedBox.shrink();
      }
      final strings = context.strings;
      final rects = <TouchControl, Rect>{
        for (final control in controls)
          control: _controlRect(control, size, factor),
      };
      final sorted = [...controls]
        ..sort(
          (a, b) => profile.controls[a]!.priority.compareTo(
            profile.controls[b]!.priority,
          ),
        );
      final colors = Theme.of(context).colorScheme;
      final viewportWidth = math.min(size.width, size.height * .84);
      final viewportHeight = viewportWidth * .75;
      return Center(
        child: SizedBox.fromSize(
          size: size,
          child: DecoratedBox(
            decoration: BoxDecoration(
              color: colors.surfaceContainerLowest,
              border: Border.all(color: colors.outlineVariant),
              borderRadius: BorderRadius.circular(12),
            ),
            child: ClipRRect(
              borderRadius: BorderRadius.circular(12),
              child: Stack(
                fit: StackFit.expand,
                children: [
                  Positioned(
                    left:
                        (size.width - viewportWidth) / 2 +
                        profile.viewportX * size.width,
                    top: (size.height - viewportHeight) / 2,
                    width: viewportWidth,
                    height: viewportHeight,
                    child: DecoratedBox(
                      decoration: BoxDecoration(
                        color: colors.surfaceContainer,
                        border: Border.all(color: colors.outlineVariant),
                      ),
                      child: Align(
                        alignment: sensitivity == null
                            ? Alignment.center
                            : Alignment.topCenter,
                        child: Padding(
                          padding: const EdgeInsets.fromLTRB(4, 4, 4, 0),
                          child: Text(
                            strings.touchEditorViewport,
                            maxLines: 1,
                            overflow: TextOverflow.ellipsis,
                          ),
                        ),
                      ),
                    ),
                  ),
                  if (sensitivity case final gain? when !adjustingViewport)
                    _SensitivityPreview(
                      key: ObjectKey(template),
                      size: size,
                      gain: gain,
                    ),
                  if (adjustingViewport)
                    GestureDetector(
                      key: const ValueKey('touch-viewport-gesture'),
                      behavior: HitTestBehavior.opaque,
                      dragStartBehavior: DragStartBehavior.down,
                      onHorizontalDragUpdate: (event) =>
                          onViewport(event.delta.dx / size.width),
                      child: Align(
                        alignment: Alignment.bottomCenter,
                        child: Padding(
                          padding: const EdgeInsets.all(8),
                          child: Text(
                            strings.touchEditorDragViewport,
                            textAlign: TextAlign.center,
                            maxLines: 2,
                            overflow: TextOverflow.ellipsis,
                          ),
                        ),
                      ),
                    )
                  else
                    for (final control in sorted)
                      Positioned.fromRect(
                        key: ValueKey(control),
                        rect: rects[control]!,
                        child: _button(context, control, rects, size),
                      ),
                ],
              ),
            ),
          ),
        ),
      );
    },
  );

  Rect _controlRect(TouchControl control, Size area, double factor) {
    final placement = profile.controls[control]!;
    final measured =
        template.controlSizes[control]! * (factor * placement.scale);
    final width = measured.width.clamp(32.0, math.max(32.0, area.width - 8));
    final height = measured.height.clamp(28.0, math.max(28.0, area.height - 8));
    final rect = Rect.fromCenter(
      center: Offset(
        (placement.position.dx * area.width).clamp(
          width / 2,
          area.width - width / 2,
        ),
        (placement.position.dy * area.height).clamp(
          height / 2,
          area.height - height / 2,
        ),
      ),
      width: width.toDouble(),
      height: height.toDouble(),
    );
    return rect;
  }

  Widget _button(
    BuildContext context,
    TouchControl control,
    Map<TouchControl, Rect> rects,
    Size size,
  ) {
    final selected = this.selected == control;
    final rect = rects[control]!;
    final colors = Theme.of(context).colorScheme;
    final strings = context.strings;
    final label = control.labelFor(game.id, strings);
    final handleSize = math.min(23.0, rect.shortestSide * .45);
    final collision = rects.entries.any((entry) {
      if (entry.key == control || !rect.overlaps(entry.value)) return false;
      final overlap = rect.intersect(entry.value);
      return overlap.width * overlap.height >=
          .28 *
              math.min(
                rect.width * rect.height,
                entry.value.width * entry.value.height,
              );
    });
    return Semantics(
      button: true,
      selected: selected,
      label: label,
      onTap: () => onSelect(control),
      child: _PreviewDrag(
        key: ValueKey('touch-preview-${control.name}'),
        onTap: () => onSelect(control),
        onStart: (_) {
          onSelect(control);
          return _PreviewPointerDrag(
            (delta) => onMove(
              control,
              Offset(delta.dx / size.width, delta.dy / size.height),
            ),
          );
        },
        child: Opacity(
          opacity: selected ? 1 : opacity,
          child: DecoratedBox(
            decoration: BoxDecoration(
              color: selected
                  ? colors.primaryContainer
                  : colors.surfaceContainerHigh,
              borderRadius: BorderRadius.circular(
                control == TouchControl.joystick ? 80 : 9,
              ),
              border: Border.all(
                color: collision
                    ? colors.error
                    : selected
                    ? colors.primary
                    : colors.outlineVariant,
              ),
            ),
            child: Stack(
              children: [
                Positioned.fill(
                  child: Center(
                    child: Padding(
                      padding: const EdgeInsets.symmetric(horizontal: 4),
                      child: ExcludeSemantics(
                        child: FittedBox(
                          fit: BoxFit.scaleDown,
                          child: Text(
                            label,
                            maxLines: 1,
                            softWrap: false,
                            textAlign: TextAlign.center,
                            style: Theme.of(context).textTheme.labelSmall,
                          ),
                        ),
                      ),
                    ),
                  ),
                ),
                if (selected)
                  Positioned(
                    right: 0,
                    bottom: 0,
                    child: Tooltip(
                      message: strings.touchEditorResize,
                      child: _PreviewDrag(
                        key: const ValueKey('touch-preview-resize'),
                        onStart: (_) => _PreviewPointerDrag(
                          (delta) =>
                              onResize(control, (delta.dx + delta.dy) / 100),
                        ),
                        child: SizedBox.square(
                          dimension: handleSize,
                          child: Align(
                            alignment: Alignment.bottomRight,
                            child: Icon(
                              Icons.open_in_full,
                              size: math.min(13.0, handleSize),
                            ),
                          ),
                        ),
                      ),
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

class _SensitivityPreview extends StatefulWidget {
  const _SensitivityPreview({
    super.key,
    required this.size,
    required this.gain,
  });

  final Size size;
  final double gain;

  @override
  State<_SensitivityPreview> createState() => _SensitivityPreviewState();
}

class _SensitivityPreviewState extends State<_SensitivityPreview> {
  Offset? _delta;

  Drag? _start(Offset _) {
    if (_delta != null) return null;
    setState(() => _delta = Offset.zero);
    return _PreviewPointerDrag(
      (delta) => setState(() => _delta = _delta! + delta),
      onEnd: () => setState(() => _delta = null),
    );
  }

  @override
  Widget build(BuildContext context) {
    final colors = Theme.of(context).colorScheme;
    final center = widget.size.center(Offset.zero);
    final target = center + (_delta ?? Offset.zero) * widget.gain;
    final position = Offset(
      target.dx.clamp(0.0, widget.size.width),
      target.dy.clamp(0.0, widget.size.height),
    );
    return _PreviewDrag(
      key: const ValueKey('touch-sensitivity-background'),
      onStart: _start,
      child: Stack(
        children: [
          Positioned(
            left: position.dx - 14,
            top: position.dy - 14,
            child: Container(
              key: const ValueKey('touch-sensitivity-target'),
              width: 28,
              height: 28,
              decoration: BoxDecoration(
                shape: BoxShape.circle,
                color: _delta == null
                    ? colors.surfaceContainerHigh
                    : colors.primaryContainer,
                border: Border.all(
                  width: 2,
                  color: _delta == null ? colors.onSurface : colors.primary,
                ),
              ),
              alignment: Alignment.center,
              child: DecoratedBox(
                decoration: BoxDecoration(
                  shape: BoxShape.circle,
                  color: colors.onSurface,
                ),
                child: const SizedBox.square(dimension: 5),
              ),
            ),
          ),
        ],
      ),
    );
  }
}

class _PreviewDrag extends StatelessWidget {
  const _PreviewDrag({
    super.key,
    this.onTap,
    required this.onStart,
    required this.child,
  });

  final VoidCallback? onTap;
  final GestureMultiDragStartCallback onStart;
  final Widget child;

  @override
  Widget build(BuildContext context) => RawGestureDetector(
    behavior: HitTestBehavior.opaque,
    excludeFromSemantics: true,
    gestures: {
      if (onTap != null)
        TapGestureRecognizer:
            GestureRecognizerFactoryWithHandlers<TapGestureRecognizer>(
              TapGestureRecognizer.new,
              (recognizer) => recognizer.onTap = onTap,
            ),
      // Preview gestures can claim either axis before the outer view scrolls.
      ImmediateMultiDragGestureRecognizer:
          GestureRecognizerFactoryWithHandlers<
            ImmediateMultiDragGestureRecognizer
          >(
            ImmediateMultiDragGestureRecognizer.new,
            (recognizer) => recognizer.onStart = onStart,
          ),
    },
    child: child,
  );
}

class _PreviewPointerDrag extends Drag {
  _PreviewPointerDrag(this.onUpdate, {this.onEnd});

  final ValueChanged<Offset> onUpdate;
  final VoidCallback? onEnd;

  @override
  void update(DragUpdateDetails details) => onUpdate(details.delta);

  @override
  void end(DragEndDetails details) => onEnd?.call();

  @override
  void cancel() => onEnd?.call();
}
