import 'package:flutter/services.dart';
import 'package:lucide_icons_flutter/lucide_icons.dart';
import 'package:material_ui/material_ui.dart';
import 'package:prism_ui/prism_ui.dart' as prism;

class LauncherControl {
  static AnimationStyle? overlayMotion(BuildContext context) =>
      MediaQuery.disableAnimationsOf(context)
      ? AnimationStyle.noAnimation
      : null;

  static const height = 44.0;
  static const iconSize = 16.0;
  static const shape = prism.AppShape.sm;
  static const geometry = ButtonStyle(
    iconSize: WidgetStatePropertyAll(iconSize),
    minimumSize: WidgetStatePropertyAll(Size(height, height)),
    padding: WidgetStatePropertyAll(EdgeInsets.symmetric(horizontal: 10)),
    tapTargetSize: MaterialTapTargetSize.shrinkWrap,
    shape: WidgetStatePropertyAll(shape),
  );

  static TextStyle label(BuildContext context) =>
      (Theme.of(context).textTheme.labelMedium ?? const TextStyle()).copyWith(
        fontSize: 12,
        fontWeight: FontWeight.w500,
        letterSpacing: 0,
        height: 1.2,
        leadingDistribution: TextLeadingDistribution.even,
      );

  static double labelWidth(
    BuildContext context,
    String text, {
    FontWeight weight = FontWeight.w500,
  }) => TextPainter.computeWidth(
    text: TextSpan(
      text: text,
      style: label(context).copyWith(fontWeight: weight),
    ),
    textDirection: Directionality.of(context),
    textScaler: MediaQuery.textScalerOf(context),
    maxLines: 1,
  );

  static double iconButtonWidth(BuildContext context, String text) =>
      labelWidth(context, text) + 44;

  static ButtonStyle style(BuildContext context) {
    final colors = Theme.of(context).colorScheme;
    return geometry.copyWith(
      textStyle: WidgetStatePropertyAll(label(context)),
      animationDuration: MediaQuery.disableAnimationsOf(context)
          ? Duration.zero
          : const Duration(milliseconds: 150),
      side: WidgetStatePropertyAll(BorderSide(color: colors.outlineVariant)),
      foregroundColor: WidgetStateProperty.resolveWith(
        (states) => states.contains(WidgetState.disabled)
            ? colors.onSurface.withValues(alpha: .38)
            : states.contains(WidgetState.selected)
            ? colors.primary
            : colors.onSurfaceVariant,
      ),
      backgroundColor: WidgetStateProperty.resolveWith(
        (states) => states.contains(WidgetState.selected)
            ? colors.surfaceContainerHighest
            : colors.surfaceContainerLow,
      ),
    );
  }
}

/// Keeps a setting label readable when a fixed control would squeeze its text.
class LauncherSettingRow extends StatelessWidget {
  const LauncherSettingRow({
    super.key,
    required this.label,
    required this.control,
    required this.controlWidth,
    this.description,
    this.contentPadding,
  });

  final String label;
  final String? description;
  final Widget control;
  final double controlWidth;
  final EdgeInsetsGeometry? contentPadding;

  @override
  Widget build(BuildContext context) {
    final theme = Theme.of(context);
    final tile = ListTileTheme.of(context);
    final padding =
        contentPadding ??
        tile.contentPadding ??
        const EdgeInsetsDirectional.only(start: 16, end: 24);
    final titleStyle = (tile.titleTextStyle ?? theme.textTheme.bodyLarge!)
        .copyWith(
          color: theme.colorScheme.onSurface,
          fontSize: tile.dense == true ? 13 : null,
        );
    final subtitleStyle =
        (tile.subtitleTextStyle ?? theme.textTheme.bodyMedium!).copyWith(
          color: theme.colorScheme.onSurfaceVariant,
          fontSize: tile.dense == true ? 12 : null,
        );
    return LayoutBuilder(
      builder: (context, constraints) {
        final scaler = MediaQuery.textScalerOf(context);
        final largeText =
            scaler.scale(titleStyle.fontSize!) > titleStyle.fontSize! * 1.3;
        final titleWidth = !largeText
            ? 0.0
            : TextPainter.computeWidth(
                text: TextSpan(text: label, style: titleStyle),
                textDirection: Directionality.of(context),
                textScaler: scaler,
                maxLines: 1,
              );
        final available = constraints.maxWidth - padding.horizontal;
        if (largeText && titleWidth + controlWidth + 16 > available) {
          return Padding(
            padding: padding.add(const EdgeInsets.symmetric(vertical: 10)),
            child: Column(
              crossAxisAlignment: CrossAxisAlignment.stretch,
              spacing: 8,
              children: [
                Text(label, style: titleStyle),
                if (description case final value?)
                  Text(value, style: subtitleStyle),
                Align(
                  alignment: AlignmentDirectional.centerStart,
                  child: control,
                ),
              ],
            ),
          );
        }
        return ListTile(
          contentPadding: contentPadding,
          title: Text(label),
          subtitle: description == null ? null : Text(description!),
          trailing: SizedBox(width: controlWidth, child: control),
        );
      },
    );
  }
}

class LauncherSegments<T> extends StatelessWidget {
  const LauncherSegments({
    super.key,
    required this.segments,
    required this.value,
    required this.onChanged,
  });

  final List<ButtonSegment<T>> segments;
  final T value;
  final ValueChanged<T>? onChanged;

  @override
  Widget build(BuildContext context) {
    final colors = Theme.of(context).colorScheme;
    final style = LauncherControl.style(context);
    return DecoratedBox(
      decoration: ShapeDecoration(
        color: colors.surfaceContainerLow,
        shape: LauncherControl.shape.copyWith(
          side: BorderSide(color: colors.outlineVariant),
        ),
      ),
      child: SizedBox(
        height: LauncherControl.height,
        child: SegmentedButton<T>(
          segments: [
            for (final segment in segments)
              ButtonSegment(
                value: segment.value,
                enabled: segment.enabled,
                tooltip: segment.tooltip,
                label: Padding(
                  padding: const EdgeInsets.all(3),
                  child: AnimatedContainer(
                    duration: style.animationDuration!,
                    curve: Curves.easeOutCubic,
                    height: LauncherControl.height - 6,
                    padding: const EdgeInsets.symmetric(horizontal: 9),
                    decoration: BoxDecoration(
                      color: segment.value == value
                          ? colors.primary.withValues(
                              alpha: segment.enabled && onChanged != null
                                  ? .13
                                  : .05,
                            )
                          : Colors.transparent,
                      borderRadius: BorderRadius.circular(6),
                    ),
                    child: Row(
                      mainAxisAlignment: MainAxisAlignment.center,
                      spacing: 6,
                      children: [
                        ?segment.icon,
                        if (segment.label case final Widget label)
                          Flexible(child: label),
                      ],
                    ),
                  ),
                ),
              ),
          ],
          selected: {value},
          onSelectionChanged: onChanged == null
              ? null
              : (values) => onChanged!(values.single),
          showSelectedIcon: false,
          style: style.copyWith(
            side: const WidgetStatePropertyAll(BorderSide.none),
            padding: const WidgetStatePropertyAll(EdgeInsets.zero),
            backgroundColor: const WidgetStatePropertyAll(Colors.transparent),
          ),
        ),
      ),
    );
  }
}

class LauncherSelect<T> extends StatefulWidget {
  const LauncherSelect({
    super.key,
    required this.label,
    required this.value,
    required this.labels,
    required this.onChanged,
    this.expand = false,
    this.disabledValues = const {},
  });

  final String label;
  final T value;
  final Map<T, String> labels;
  final ValueChanged<T?>? onChanged;
  final bool expand;
  final Set<T> disabledValues;

  @override
  State<LauncherSelect<T>> createState() => _LauncherSelectState<T>();
}

class _LauncherSelectState<T> extends State<LauncherSelect<T>> {
  final _controller = MenuController();
  final _focus = FocusNode();
  // Keep the menu scroll offset separate from its host's saved expansion state.
  final _menuStorage = PageStorageBucket();

  @override
  void dispose() {
    _focus.dispose();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    final colors = Theme.of(context).colorScheme;
    final active = widget.onChanged;
    final choices = widget.labels.keys.where(
      (value) => !widget.disabledValues.contains(value),
    );
    final enabled = active != null && choices.isNotEmpty;
    final focusValue = choices.contains(widget.value)
        ? widget.value
        : choices.firstOrNull;
    final selected = widget.labels[widget.value] ?? widget.label;
    final style = LauncherControl.style(context);
    return PageStorage(
      bucket: _menuStorage,
      child: MenuAnchor(
        controller: _controller,
        useRootOverlay: true,
        childFocusNode: _focus,
        animated: !MediaQuery.disableAnimationsOf(context),
        alignmentOffset: const Offset(0, 4),
        style: MenuStyle(
          backgroundColor: WidgetStatePropertyAll(colors.surfaceContainer),
          surfaceTintColor: const WidgetStatePropertyAll(Colors.transparent),
          padding: const WidgetStatePropertyAll(
            EdgeInsets.symmetric(vertical: 4),
          ),
          shape: WidgetStatePropertyAll(LauncherControl.shape),
          elevation: const WidgetStatePropertyAll(3),
        ),
        menuChildren: [
          for (final MapEntry(:key, :value) in widget.labels.entries)
            MenuItemButton(
              autofocus: key == focusValue,
              style: ButtonStyle(
                textStyle: style.textStyle,
                iconSize: style.iconSize,
              ),
              leadingIcon: key == widget.value
                  ? Icon(LucideIcons.check, color: colors.primary)
                  : const SizedBox.square(dimension: LauncherControl.iconSize),
              onPressed: active == null || widget.disabledValues.contains(key)
                  ? null
                  : () => active(key),
              child: Semantics(
                selected: key == widget.value,
                child: Text(value),
              ),
            ),
        ],
        builder: (context, controller, _) => Semantics(
          label: widget.label == selected ? null : widget.label,
          expanded: controller.isOpen,
          child: CallbackShortcuts(
            bindings: enabled && !controller.isOpen
                ? {
                    const SingleActivator(LogicalKeyboardKey.arrowDown): () =>
                        controller.open(),
                    const SingleActivator(LogicalKeyboardKey.arrowUp): () =>
                        controller.open(),
                  }
                : const {},
            child: SizedBox(
              height: LauncherControl.height,
              child: TextButton(
                focusNode: _focus,
                style: style,
                onPressed: !enabled
                    ? null
                    : () {
                        if (controller.isOpen) {
                          controller.close();
                        } else {
                          controller.open();
                        }
                      },
                child: Row(
                  mainAxisSize: widget.expand
                      ? MainAxisSize.max
                      : MainAxisSize.min,
                  spacing: 8,
                  children: [
                    Flexible(
                      fit: widget.expand ? FlexFit.tight : FlexFit.loose,
                      child: Text(
                        selected,
                        maxLines: 1,
                        overflow: TextOverflow.ellipsis,
                      ),
                    ),
                    const Icon(
                      LucideIcons.chevronDown,
                      size: LauncherControl.iconSize,
                    ),
                  ],
                ),
              ),
            ),
          ),
        ),
      ),
    );
  }
}
