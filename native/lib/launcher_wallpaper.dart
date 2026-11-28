import 'dart:math' as math;

import 'package:material_ui/material_ui.dart';

import 'launcher_background.dart';

/// Gives a route its own background while preserving the window's wallpaper.
class LauncherPageSurface extends StatelessWidget {
  const LauncherPageSurface({super.key, required this.child});

  final Widget child;

  @override
  Widget build(BuildContext context) => Stack(
    fit: StackFit.expand,
    children: [
      ColoredBox(
        color: Theme.of(context).scaffoldBackgroundColor,
        child: LauncherBackdrop.active(context)
            ? Flow.unwrapped(
                delegate: _WallpaperRegion(
                  context,
                  LauncherBackdrop.wallpaperSizeOf(context) ??
                      MediaQuery.sizeOf(context),
                  ModalRoute.of(context),
                ),
                children: const [LauncherWallpaper()],
              )
            : null,
      ),
      Material(type: MaterialType.transparency, child: child),
    ],
  );
}

class _WallpaperRegion extends FlowDelegate {
  _WallpaperRegion(this.context, this.viewSize, ModalRoute<dynamic>? route)
    : super(
        repaint: Listenable.merge([
          route?.animation,
          route?.secondaryAnimation,
        ]),
      );

  final BuildContext context;
  final Size viewSize;

  Rect get viewport =>
      LauncherBackdrop.wallpaperViewportOf(context) ?? (Offset.zero & viewSize);

  @override
  BoxConstraints getConstraintsForChild(int i, BoxConstraints constraints) =>
      BoxConstraints.tight(viewSize);

  @override
  void paintChildren(FlowPaintingContext painting) {
    final render = context.findRenderObject()! as RenderBox;
    final offset = viewport.topLeft - render.localToGlobal(Offset.zero);
    painting.paintChild(
      0,
      transform: Matrix4.translationValues(offset.dx, offset.dy, 0),
    );
  }

  @override
  bool shouldRepaint(_WallpaperRegion oldDelegate) =>
      oldDelegate.context != context || oldDelegate.viewSize != viewSize;
}

class LauncherWallpaper extends StatelessWidget {
  const LauncherWallpaper({super.key});

  @override
  Widget build(BuildContext context) {
    final background = LauncherBackdrop.of(context);
    final mode = background?.mode ?? BackgroundMode.plain;
    if (mode == BackgroundMode.plain) return const SizedBox.shrink();
    final colors = Theme.of(context).colorScheme;
    return ExcludeSemantics(
      child: RepaintBoundary(
        child: Stack(
          fit: StackFit.expand,
          children: [
            LauncherWallpaperSample(
              mode: mode,
              background: background!,
              preview: false,
            ),
            ColoredBox(
              color: colors.surface.withValues(
                alpha: background.opacity(BackgroundLayer.shade) / 100,
              ),
            ),
          ],
        ),
      ),
    );
  }
}

class LauncherWallpaperSample extends StatelessWidget {
  const LauncherWallpaperSample({
    super.key,
    required this.mode,
    required this.background,
    this.preview = true,
  });

  final BackgroundMode mode;
  final LauncherBackground background;
  final bool preview;

  @override
  Widget build(BuildContext context) {
    final colors = Theme.of(context).colorScheme;
    final bytes = background.image;
    return ColoredBox(
      color: colors.surface,
      child: mode == BackgroundMode.image && bytes != null
          ? Image(
              image: ResizeImage(
                MemoryImage(bytes),
                width: 1920,
                height: 1440,
                policy: ResizeImagePolicy.fit,
              ),
              fit: preview || background.cover ? BoxFit.cover : BoxFit.contain,
              errorBuilder: (_, _, _) => const SizedBox.shrink(),
              excludeFromSemantics: true,
            )
          : CustomPaint(
              painter: _WallpaperPattern(mode, colors.onSurface, preview),
              child: const SizedBox.expand(),
            ),
    );
  }
}

class _WallpaperPattern extends CustomPainter {
  const _WallpaperPattern(this.mode, this.ink, this.preview);
  final BackgroundMode mode;
  final Color ink;
  final bool preview;

  @override
  void paint(Canvas canvas, Size size) {
    final paint = Paint();
    switch (mode) {
      case BackgroundMode.dots:
        final step = preview ? 14.0 : 28.0;
        paint.color = ink.withValues(alpha: preview ? .25 : .18);
        for (var y = step / 2; y < size.height; y += step) {
          for (var x = step / 2; x < size.width; x += step) {
            canvas.drawCircle(Offset(x, y), 1.1, paint);
          }
        }
      case BackgroundMode.grid:
        final step = preview ? 18.0 : 36.0;
        paint.color = ink.withValues(alpha: preview ? .12 : .08);
        for (var x = .5; x < size.width; x += step) {
          canvas.drawLine(Offset(x, 0), Offset(x, size.height), paint);
        }
        for (var y = .5; y < size.height; y += step) {
          canvas.drawLine(Offset(0, y), Offset(size.width, y), paint);
        }
      case BackgroundMode.seigaiha:
        canvas.save();
        canvas.clipRect(Offset.zero & size);
        paint
          ..style = PaintingStyle.stroke
          ..strokeWidth = preview ? 1 : 2;
        final step = preview ? 36.0 : 72.0;
        for (var row = 0; row * step / 2 < size.height + step; row++) {
          for (
            var x = row.isEven ? 0.0 : -step;
            x < size.width + step;
            x += step * 2
          ) {
            for (var ring = 0; ring < 4; ring++) {
              paint.color = ink.withValues(alpha: .28 - ring * .06);
              final rect = Rect.fromCircle(
                center: Offset(x, row * step / 2),
                radius: step * (.26 + .19 * ring),
              );
              canvas.drawArc(rect, math.pi, math.pi, false, paint);
            }
          }
        }
        canvas.restore();
      case BackgroundMode.plain:
      case BackgroundMode.image:
        break;
    }
  }

  @override
  bool shouldRepaint(_WallpaperPattern oldDelegate) =>
      oldDelegate.mode != mode ||
      oldDelegate.ink != ink ||
      oldDelegate.preview != preview;
}
