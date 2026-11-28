import 'dart:async';

import 'package:crop_your_image/crop_your_image.dart';
import 'package:flutter/services.dart';
import 'package:material_ui/material_ui.dart';

import 'l10n/strings.dart';
import 'modal_keyboard_dismiss.dart';
import 'profile_image.dart';

Future<PreparedProfileImage?> cropProfileImage(
  BuildContext context,
  ProfileCropSource source,
  ProfileImageKind kind,
) => showDialog<PreparedProfileImage>(
  context: context,
  barrierDismissible: false,
  builder: (_) => ModalKeyboardDismiss(
    child: _ProfileCropDialog(source: source, kind: kind),
  ),
);

class _ProfileCropDialog extends StatefulWidget {
  const _ProfileCropDialog({required this.source, required this.kind});

  final ProfileCropSource source;
  final ProfileImageKind kind;

  @override
  State<_ProfileCropDialog> createState() => _ProfileCropDialogState();
}

class _ProfileCropDialogState extends State<_ProfileCropDialog> {
  final _controller = CropController();
  late final _center = fitProfileCrop(widget.source, widget.kind);
  late Rect _selected = _center;
  Timer? _previewTimer;
  Uint8List? _preview;
  bool _previewBusy = false;
  bool _ready = false;
  bool _encoding = false;
  bool _issue = false;

  bool get _avatar => widget.kind == ProfileImageKind.avatar;

  double get _maxZoom => _center.height.clamp(1.0, 4.0);

  @override
  void dispose() {
    _previewTimer?.cancel();
    super.dispose();
  }

  void _moveTo(Rect next) {
    _controller.area = Rect.fromLTWH(
      next.left.clamp(
        0.0,
        (widget.source.width - next.width).clamp(0.0, double.infinity),
      ),
      next.top.clamp(
        0.0,
        (widget.source.height - next.height).clamp(0.0, double.infinity),
      ),
      next.width,
      next.height,
    );
  }

  void _zoomTo(double zoom) {
    final scale = zoom.clamp(1.0, _maxZoom);
    _moveTo(
      Rect.fromCenter(
        center: _selected.center,
        width: _center.width / scale,
        height: _center.height / scale,
      ),
    );
  }

  void _cropMoved(Rect _, Rect next) {
    if (next == _selected) return;
    setState(() {
      _selected = next;
      _issue = false;
    });
    _schedulePreview();
  }

  void _schedulePreview() {
    _previewTimer?.cancel();
    if (_ready && !_encoding && !_previewBusy) {
      _previewTimer = Timer(const Duration(milliseconds: 120), _readPreview);
    }
  }

  Future<void> _readPreview() async {
    final crop = _selected;
    _previewBusy = true;
    try {
      final bytes = await renderProfileCropPreview(
        widget.source,
        widget.kind,
        crop,
      );
      if (mounted && !_encoding && crop == _selected) {
        setState(() => _preview = bytes);
      }
    } catch (_) {
      if (mounted && !_encoding && crop == _selected) {
        setState(() {
          _preview = null;
          _issue = true;
        });
      }
    } finally {
      _previewBusy = false;
      if (mounted && crop != _selected) _schedulePreview();
    }
  }

  KeyEventResult _key(FocusNode _, KeyEvent event) {
    if (!_ready || _encoding || event is KeyUpEvent) {
      return KeyEventResult.ignored;
    }
    final step = _selected.height * .04;
    final shift = switch (event.logicalKey) {
      LogicalKeyboardKey.arrowLeft => Offset(-step, 0),
      LogicalKeyboardKey.arrowRight => Offset(step, 0),
      LogicalKeyboardKey.arrowUp => Offset(0, -step),
      LogicalKeyboardKey.arrowDown => Offset(0, step),
      _ => null,
    };
    if (shift != null) {
      _moveTo(_selected.shift(shift));
      return KeyEventResult.handled;
    }
    return KeyEventResult.ignored;
  }

  Future<void> _finish() async {
    if (!_ready || _encoding) return;
    _previewTimer?.cancel();
    setState(() {
      _encoding = true;
      _issue = false;
    });
    try {
      final result = await encodeProfileCrop(
        widget.source,
        widget.kind,
        _selected,
      );
      if (mounted) Navigator.of(context).pop(result);
    } catch (_) {
      if (mounted) {
        setState(() {
          _encoding = false;
          _issue = true;
        });
        _schedulePreview();
      }
    }
  }

  @override
  Widget build(BuildContext context) {
    final viewport = MediaQuery.sizeOf(context);
    final compact = viewport.width <= 480;
    final theme = Theme.of(context);
    final strings = context.strings;
    final zoom = (_center.height / _selected.height).clamp(1.0, _maxZoom);
    final adjustable = _ready && !_encoding;
    return PopScope(
      canPop: !_encoding,
      child: AlertDialog(
        insetPadding: const EdgeInsets.symmetric(horizontal: 12, vertical: 16),
        title: Text(
          _avatar ? strings.profileCropAvatar : strings.profileCropCover,
        ),
        scrollable: true,
        content: SizedBox(
          width: 540,
          child: Column(
            mainAxisSize: MainAxisSize.min,
            crossAxisAlignment: CrossAxisAlignment.stretch,
            spacing: 8,
            children: [
              Text(
                strings.profileCropHint,
                style: theme.textTheme.bodySmall?.copyWith(
                  color: theme.colorScheme.onSurfaceVariant,
                ),
              ),
              SizedBox(
                height: (viewport.height * (compact && !_avatar ? .29 : .34))
                    .clamp(100.0, 300.0),
                child: Focus(
                  autofocus: true,
                  onKeyEvent: _key,
                  child: Semantics(
                    label: _avatar
                        ? strings.profileCropAvatar
                        : strings.profileCropCover,
                    child: Crop(
                      key: const ValueKey('profile-crop-workspace'),
                      image: widget.source.bytes,
                      controller: _controller,
                      aspectRatio: widget.kind.aspect,
                      cornerDotBuilder: (_, _) => const SizedBox.shrink(),
                      initialRectBuilder: InitialRectBuilder.withArea(_center),
                      withCircleUi: _avatar,
                      overlayBuilder: (_, _) => DecoratedBox(
                        decoration: BoxDecoration(
                          border: Border.all(color: Colors.white70),
                          shape: _avatar ? BoxShape.circle : BoxShape.rectangle,
                        ),
                      ),
                      baseColor: theme.colorScheme.surfaceContainerHighest,
                      maskColor: Colors.black54,
                      onMoved: _cropMoved,
                      onStatusChanged: (status) {
                        if (mounted) {
                          setState(() => _ready = status == CropStatus.ready);
                          _schedulePreview();
                        }
                      },
                      // Encode from the original rectangle to keep surrounding cover pixels.
                      onCropped: (_) {},
                    ),
                  ),
                ),
              ),
              Row(
                children: [
                  IconButton(
                    tooltip: strings.profileCropZoomOut,
                    onPressed: adjustable && zoom > 1
                        ? () => _zoomTo(zoom - .1)
                        : null,
                    icon: const Icon(Icons.remove),
                  ),
                  Expanded(
                    child: Semantics(
                      label: strings.profileCropZoom,
                      child: Slider(
                        value: zoom,
                        min: 1,
                        max: _maxZoom,
                        padding: const EdgeInsets.symmetric(horizontal: 10),
                        semanticFormatterCallback: (value) =>
                            '${(value * 100).round()}%',
                        onChanged: adjustable && _maxZoom > 1 ? _zoomTo : null,
                      ),
                    ),
                  ),
                  IconButton(
                    tooltip: strings.profileCropZoomIn,
                    onPressed: adjustable && zoom < _maxZoom
                        ? () => _zoomTo(zoom + .1)
                        : null,
                    icon: const Icon(Icons.add),
                  ),
                  IconButton(
                    tooltip: strings.profileCropReset,
                    onPressed: adjustable ? () => _moveTo(_center) : null,
                    icon: const Icon(Icons.restart_alt),
                  ),
                ],
              ),
              if (_avatar)
                Row(
                  spacing: 10,
                  children: [
                    ClipOval(
                      child: SizedBox.square(dimension: 32, child: _image()),
                    ),
                    Expanded(child: Text(strings.profileCropRoomPreview)),
                  ],
                )
              else if (compact) ...[
                _cover(strings.profileCropMobile, 3),
                _cover(strings.profileCropDesktop, 6),
              ] else
                Row(
                  crossAxisAlignment: CrossAxisAlignment.start,
                  spacing: 12,
                  children: [
                    Expanded(child: _cover(strings.profileCropMobile, 3)),
                    Expanded(
                      flex: 2,
                      child: _cover(strings.profileCropDesktop, 6),
                    ),
                  ],
                ),
              if (_encoding) const LinearProgressIndicator(),
              if (_issue)
                Semantics(
                  liveRegion: true,
                  child: Text(strings.profileCropError),
                ),
            ],
          ),
        ),
        actions: [
          TextButton(
            onPressed: _encoding ? null : () => Navigator.of(context).pop(),
            child: Text(strings.cancel),
          ),
          FilledButton(
            onPressed: _ready && !_encoding ? _finish : null,
            child: Text(strings.profileCropConfirm),
          ),
        ],
      ),
    );
  }

  Widget _image() => _preview == null
      ? ColoredBox(color: Theme.of(context).colorScheme.surfaceContainerHighest)
      : Image.memory(
          _preview!,
          key: const ValueKey('profile-crop-preview'),
          fit: BoxFit.cover,
          gaplessPlayback: true,
        );

  Widget _cover(String label, double aspect) => Column(
    crossAxisAlignment: CrossAxisAlignment.start,
    spacing: 4,
    children: [
      Row(
        spacing: 6,
        children: [
          Expanded(
            child: Text(label, style: Theme.of(context).textTheme.labelMedium),
          ),
          Text(
            '${aspect.toInt()}:1',
            style: Theme.of(context).textTheme.labelMedium,
          ),
        ],
      ),
      ClipRRect(
        borderRadius: BorderRadius.circular(4),
        child: AspectRatio(aspectRatio: aspect, child: _image()),
      ),
    ],
  );
}
