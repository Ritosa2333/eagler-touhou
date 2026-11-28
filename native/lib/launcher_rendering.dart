import 'dart:ui' as ui;

import 'package:flutter/painting.dart';

/// Precompiles the Skia draw operations found in the Windows detail-entry trace.
/// This runs before the first UI frame; no page or user image is preloaded.
class LauncherShaderWarmUp extends ShaderWarmUp {
  const LauncherShaderWarmUp();
  @override
  ui.Size get size => const ui.Size(512, 512);
  @override
  Future<void> warmUpOnCanvas(ui.Canvas canvas) async {
    final rec = ui.PictureRecorder();
    final source = ui.Canvas(rec);
    source.drawRect(
      const ui.Rect.fromLTWH(0, 0, 64, 64),
      ui.Paint()
        ..shader = ui.Gradient.linear(
          const ui.Offset(0, 0),
          const ui.Offset(64, 64),
          [const ui.Color(0xFFE85895), const ui.Color(0xFF172554)],
        ),
    );
    final picture = rec.endRecording();
    final image = await picture.toImage(64, 64);
    picture.dispose();
    const rect = ui.Rect.fromLTWH(8, 8, 496, 300);
    // Match the detail backdrop at desktop and compact widths.
    for (final sigma in [32.0, 80.0]) {
      canvas.saveLayer(
        rect,
        ui.Paint()
          ..imageFilter = ui.ImageFilter.blur(
            sigmaX: sigma,
            sigmaY: sigma,
            tileMode: ui.TileMode.clamp,
          ),
      );
      canvas.drawImageRect(
        image,
        const ui.Rect.fromLTWH(0, 0, 64, 64),
        rect,
        ui.Paint()
          ..colorFilter = const ui.ColorFilter.mode(
            ui.Color(0x73000000),
            ui.BlendMode.srcOver,
          )
          ..filterQuality = ui.FilterQuality.low,
      );
      canvas.restore();
    }
    canvas.drawRect(
      rect,
      ui.Paint()
        ..shader = ui.Gradient.linear(rect.topLeft, rect.bottomRight, [
          const ui.Color(0x6617131C),
          const ui.Color(0xD1151520),
        ]),
    );
    canvas.saveLayer(
      const ui.Rect.fromLTWH(0, 0, 512, 512),
      ui.Paint()..color = const ui.Color(0x88FFFFFF),
    );
    canvas.drawRRect(
      ui.RRect.fromRectAndRadius(
        const ui.Rect.fromLTWH(12, 320, 220, 40),
        const ui.Radius.circular(6),
      ),
      ui.Paint()..color = const ui.Color(0xFF95C796),
    );
    canvas.drawRRect(
      ui.RRect.fromRectAndRadius(
        const ui.Rect.fromLTWH(240, 320, 220, 40),
        const ui.Radius.circular(8),
      ),
      ui.Paint()
        ..style = ui.PaintingStyle.stroke
        ..strokeWidth = 1
        ..color = const ui.Color(0xFF9F9F9F),
    );
    canvas.drawCircle(
      const ui.Offset(48, 400),
      18,
      ui.Paint()
        ..style = ui.PaintingStyle.stroke
        ..strokeWidth = 2,
    );
    canvas.drawPath(
      ui.Path()
        ..moveTo(110, 384)
        ..lineTo(130, 400)
        ..lineTo(110, 416)
        ..close(),
      ui.Paint(),
    );
    canvas.drawImageRect(
      image,
      const ui.Rect.fromLTWH(0, 0, 64, 64),
      const ui.Rect.fromLTWH(160, 380, 64, 64),
      ui.Paint(),
    );
    canvas.restore();
    // Shared progress cells, buttons, outlines and their opacity variants.
    for (final alpha in [0xFF, 0x80]) {
      for (final radius in [4.0, 6.0, 8.0, 12.0]) {
        final y = 320 + (radius - 4) * 12;
        final box = ui.Rect.fromLTWH(20.5, y, 160, 32);
        canvas.drawRRect(
          ui.RRect.fromRectAndRadius(box, ui.Radius.circular(radius)),
          ui.Paint()..color = ui.Color((alpha << 24) | 0x95C796),
        );
        canvas.drawRRect(
          ui.RRect.fromRectAndRadius(
            box.translate(180, 0),
            ui.Radius.circular(radius),
          ),
          ui.Paint()
            ..style = ui.PaintingStyle.stroke
            ..strokeWidth = 1
            ..color = ui.Color((alpha << 24) | 0xE1D8E1),
        );
      }
      canvas.drawCircle(
        const ui.Offset(420, 420),
        18,
        ui.Paint()..color = ui.Color((alpha << 24) | 0xE1D8E1),
      );
    }
    canvas.drawLine(
      const ui.Offset(10.5, 490.5),
      const ui.Offset(490.5, 490.5),
      ui.Paint()
        ..color = const ui.Color(0x80FFFFFF)
        ..strokeWidth = 0,
    );
    canvas.save();
    canvas.clipRRect(
      ui.RRect.fromRectAndRadius(
        const ui.Rect.fromLTWH(360, 320, 64, 64),
        const ui.Radius.circular(12),
      ),
    );
    canvas.drawImageRect(
      image,
      const ui.Rect.fromLTWH(0, 0, 64, 64),
      const ui.Rect.fromLTWH(360, 320, 64, 64),
      ui.Paint()..filterQuality = ui.FilterQuality.low,
    );
    canvas.restore();
    image.dispose();
  }
}
