import 'dart:typed_data';

import 'package:material_ui/material_ui.dart';

import 'account_api.dart';
import 'l10n/strings.dart';

Future<String?> showMailCaptcha(BuildContext context, AccountApi api) =>
    showDialog<String>(
      context: context,
      barrierDismissible: false,
      builder: (_) => _MailCaptchaDialog(api: api),
    );

class _SliderChallenge {
  _SliderChallenge(Map<String, dynamic> data)
    : id = data['id'] as String,
      background = _image(data['backgroundImage'], 'image/jpeg'),
      piece = _image(data['templateImage'], 'image/png'),
      width = _dimension(data['backgroundImageWidth']),
      height = _dimension(data['backgroundImageHeight']),
      pieceWidth = _dimension(data['templateImageWidth']),
      pieceHeight = _dimension(data['templateImageHeight']) {
    if (data['type'] != 'SLIDER' ||
        id.isEmpty ||
        id.length > 128 ||
        pieceWidth >= width ||
        pieceHeight != height) {
      throw const FormatException('Unsupported mail verification');
    }
  }

  final String id;
  final Uint8List background;
  final Uint8List piece;
  final int width;
  final int height;
  final int pieceWidth;
  final int pieceHeight;

  static Uint8List _image(Object? source, String mime) {
    if (source is! String || source.length > 2 * 1024 * 1024) {
      throw const FormatException('Invalid verification image');
    }
    final data = UriData.parse(source);
    if (!data.isBase64 || data.mimeType != mime) {
      throw const FormatException('Invalid verification image');
    }
    return data.contentAsBytes();
  }

  static int _dimension(Object? value) {
    if (value is int && value > 0 && value <= 2048) return value;
    throw const FormatException('Invalid verification image size');
  }
}

class _MailCaptchaDialog extends StatefulWidget {
  const _MailCaptchaDialog({required this.api});

  final AccountApi api;

  @override
  State<_MailCaptchaDialog> createState() => _MailCaptchaDialogState();
}

class _MailCaptchaDialogState extends State<_MailCaptchaDialog> {
  static const _handleWidth = 48.0;

  _SliderChallenge? _challenge;
  String? _issue;
  bool _pending = true;
  int? _pointer;
  Offset _origin = Offset.zero;
  double _distance = 0;
  int _started = 0;
  final _clock = Stopwatch();
  final _points = <Map<String, Object>>[];

  @override
  void initState() {
    super.initState();
    _load();
  }

  Future<void> _load({String? previousIssue}) async {
    setState(() {
      _pending = true;
      _challenge = null;
      _distance = 0;
      _issue = previousIssue;
    });
    try {
      final challenge = _SliderChallenge(
        await widget.api.mailCaptchaChallenge(),
      );
      if (mounted) setState(() => _challenge = challenge);
    } catch (_) {
      if (mounted) setState(() => _issue = context.strings.captchaLoadFailed);
    } finally {
      if (mounted) setState(() => _pending = false);
    }
  }

  Map<String, Object> _point(Offset position, String type) => {
    'x': position.dx.round(),
    'y': position.dy.round(),
    't': _clock.elapsedMilliseconds,
    'type': type,
  };

  void _down(PointerDownEvent event) {
    if (_pending || _pointer != null || event.buttons != 1) return;
    _pointer = event.pointer;
    _origin = event.position;
    _started = DateTime.now().millisecondsSinceEpoch;
    _clock
      ..reset()
      ..start();
    _points
      ..clear()
      ..add(_point(event.position, 'down'));
  }

  void _move(PointerMoveEvent event, double limit) {
    if (_pointer != event.pointer) return;
    if (_points.length < 700) {
      _points.add(_point(event.position, 'move'));
    }
    setState(
      () => _distance = (event.position.dx - _origin.dx).clamp(0.0, limit),
    );
  }

  void _cancel() {
    _pointer = null;
    _clock.stop();
    setState(() => _distance = 0);
  }

  Future<void> _release(
    PointerUpEvent event,
    _SliderChallenge challenge,
    int width,
    int height,
    double limit,
  ) async {
    if (_pointer != event.pointer) return;
    final moved = event.position.dx - _origin.dx;
    _pointer = null;
    _clock.stop();
    if (moved.abs() < 1) {
      setState(() => _distance = 0);
      return;
    }
    setState(() {
      _pending = true;
      _issue = null;
      _distance = moved.clamp(0.0, limit);
    });
    final track = [
      ..._points,
      _point(Offset(_origin.dx + _distance, event.position.dy), 'up'),
    ];
    try {
      final ticket = await widget.api.checkMailCaptcha(challenge.id, {
        'bgImageWidth': width,
        'bgImageHeight': height,
        'templateImageWidth': (width * challenge.pieceWidth / challenge.width)
            .round(),
        'templateImageHeight': height,
        'startTime': _started,
        'stopTime': _started + _clock.elapsedMilliseconds,
        'trackList': track,
      });
      if (!mounted) return;
      if (ticket != null) {
        Navigator.of(context).pop(ticket);
        return;
      }
      await _load(previousIssue: context.strings.captchaRetryAlignment);
    } catch (_) {
      if (!mounted) return;
      setState(() {
        _pending = false;
        _challenge = null;
        _issue = context.strings.captchaCheckFailed;
      });
    }
  }

  @override
  Widget build(BuildContext context) {
    final challenge = _challenge;
    return AlertDialog(
      title: Text(context.strings.captchaTitle),
      scrollable: true,
      content: SizedBox(
        width: 320,
        child: Column(
          mainAxisSize: MainAxisSize.min,
          spacing: 12,
          children: [
            if (_issue case final String message)
              Semantics(liveRegion: true, child: Text(message)),
            if (challenge != null)
              LayoutBuilder(
                builder: (_, constraints) {
                  final width = constraints.maxWidth.floor();
                  final height = (width * challenge.height / challenge.width)
                      .round();
                  final pieceWidth =
                      width * challenge.pieceWidth / challenge.width;
                  final limit =
                      width -
                      (pieceWidth > _handleWidth ? pieceWidth : _handleWidth);
                  return Column(
                    spacing: 12,
                    children: [
                      Text(context.strings.captchaInstructions),
                      SizedBox(
                        width: width.toDouble(),
                        height: height.toDouble(),
                        child: ClipRRect(
                          borderRadius: BorderRadius.circular(8),
                          child: Stack(
                            fit: StackFit.expand,
                            children: [
                              Image.memory(
                                challenge.background,
                                fit: BoxFit.fill,
                                excludeFromSemantics: true,
                              ),
                              Positioned(
                                left: _distance,
                                top: 0,
                                width: pieceWidth,
                                height: height.toDouble(),
                                child: Image.memory(
                                  challenge.piece,
                                  fit: BoxFit.fill,
                                  excludeFromSemantics: true,
                                ),
                              ),
                            ],
                          ),
                        ),
                      ),
                      SizedBox(
                        height: _handleWidth,
                        child: Stack(
                          fit: StackFit.expand,
                          children: [
                            DecoratedBox(
                              decoration: BoxDecoration(
                                color: Theme.of(context)
                                    .colorScheme
                                    .surfaceContainerHighest,
                                borderRadius: BorderRadius.circular(10),
                              ),
                            ),
                            Positioned(
                              left: _distance,
                              width: _handleWidth,
                              height: _handleWidth,
                              child: Listener(
                                key: const ValueKey('mail-captcha-handle'),
                                behavior: HitTestBehavior.opaque,
                                onPointerDown: _down,
                                onPointerMove: (event) => _move(event, limit),
                                onPointerCancel: (event) {
                                  if (_pointer == event.pointer) _cancel();
                                },
                                onPointerUp: (event) => _release(
                                  event,
                                  challenge,
                                  width,
                                  height,
                                  limit,
                                ),
                                child: Semantics(
                                  label: context.strings.captchaHandle,
                                  child: MouseRegion(
                                    cursor: SystemMouseCursors.grab,
                                    child: DecoratedBox(
                                      decoration: BoxDecoration(
                                        color: Theme.of(context)
                                            .colorScheme
                                            .primaryContainer,
                                        borderRadius: BorderRadius.circular(10),
                                      ),
                                      child: const Icon(Icons.drag_indicator),
                                    ),
                                  ),
                                ),
                              ),
                            ),
                          ],
                        ),
                      ),
                    ],
                  );
                },
              ),
            if (_pending) const LinearProgressIndicator(),
          ],
        ),
      ),
      actions: [
        TextButton(
          onPressed: () => Navigator.of(context).pop(),
          child: Text(context.strings.cancel),
        ),
        TextButton(
          onPressed: _pending || _pointer != null ? null : () => _load(),
          child: Text(context.strings.captchaNewPuzzle),
        ),
      ],
    );
  }
}
