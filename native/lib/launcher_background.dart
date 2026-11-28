import 'dart:convert';
import 'dart:io';
import 'dart:typed_data';
import 'dart:ui' as ui;

import 'package:file_picker/file_picker.dart';
import 'package:flutter/widgets.dart';
import 'package:path_provider/path_provider.dart';
import 'package:shared_preferences/shared_preferences.dart';
import 'package:uuid/uuid.dart';

import 'game_file_picker.dart';

enum BackgroundMode { plain, dots, grid, seigaiha, image }

enum BackgroundLayer { shade, panel, navigation, category, card }

class LauncherBackdrop extends InheritedNotifier<LauncherBackground> {
  const LauncherBackdrop({
    super.key,
    required LauncherBackground background,
    this.wallpaperKey,
    this.wallpaperSize,
    required super.child,
  }) : super(notifier: background);

  final GlobalKey? wallpaperKey;
  final Size? wallpaperSize;

  @override
  bool updateShouldNotify(LauncherBackdrop oldWidget) =>
      super.updateShouldNotify(oldWidget) ||
      wallpaperSize != oldWidget.wallpaperSize ||
      wallpaperKey != oldWidget.wallpaperKey;

  static Size? wallpaperSizeOf(BuildContext context) =>
      context.getInheritedWidgetOfExactType<LauncherBackdrop>()?.wallpaperSize;

  static Rect? wallpaperViewportOf(BuildContext context) {
    final key = context
        .getInheritedWidgetOfExactType<LauncherBackdrop>()
        ?.wallpaperKey;
    final render = key?.currentContext?.findRenderObject();
    return render is RenderBox && render.hasSize
        ? render.localToGlobal(Offset.zero) & render.size
        : null;
  }

  static LauncherBackground? of(BuildContext context) =>
      context.dependOnInheritedWidgetOfExactType<LauncherBackdrop>()?.notifier;

  static bool active(BuildContext context) =>
      (of(context)?.mode ?? BackgroundMode.plain) != BackgroundMode.plain;

  static Color? page(BuildContext context) =>
      active(context) ? const Color(0x00000000) : null;

  static Color tint(BuildContext context, BackgroundLayer layer, Color color) {
    final background = of(context);
    return background == null || background.mode == BackgroundMode.plain
        ? color
        : color.withValues(alpha: color.a * background.opacity(layer) / 100);
  }
}

class LauncherBackground extends ChangeNotifier {
  LauncherBackground(
    this._storage, {
    Future<Directory> Function()? appDirectory,
    Future<GameFileInput?> Function()? pickImage,
  }) : _appDirectory = appDirectory ?? getApplicationSupportDirectory,
       _pickImage = pickImage ?? _selectImage;

  static const _key = 'launcher.background';
  static const _maxBytes = 10 * 1024 * 1024;
  static final _storedFilename = RegExp(
    r'^launcher-wallpaper-[0-9a-f-]{36}[.]image$',
  );

  final SharedPreferencesAsync _storage;
  final Future<Directory> Function() _appDirectory;
  final Future<GameFileInput?> Function() _pickImage;
  final _alphas = <BackgroundLayer, int>{};
  BackgroundMode _mode = BackgroundMode.plain;
  bool _cover = true;
  bool _busy = false;
  String? _filename;
  Uint8List? _image;
  Future<void> _writes = Future.value();

  Uint8List? get image => _image;
  bool get cover => _cover;
  bool get busy => _busy;
  BackgroundMode get mode => _mode == BackgroundMode.image && _image == null
      ? BackgroundMode.plain
      : _mode;
  int opacity(BackgroundLayer layer) =>
      _alphas[layer] ?? (layer == BackgroundLayer.shade ? 45 : 88);

  static Future<GameFileInput?> _selectImage() async {
    final file = await FilePicker.pickFile(type: FileType.image);
    if (file == null) return null;
    return (name: file.name, length: file.length, read: file.readAsByteStream);
  }

  Future<void> restore() async {
    try {
      final value = jsonDecode(await _storage.getString(_key) ?? '{}');
      if (value is! Map<String, dynamic>) return;
      _mode = BackgroundMode.values.firstWhere(
        (mode) => mode.name == value['mode'],
        orElse: () => BackgroundMode.plain,
      );
      _cover = value['cover'] != false;
      for (final layer in BackgroundLayer.values) {
        if (value[layer.name] case final int amount) {
          _alphas[layer] = amount.clamp(0, 100);
        }
      }
      if (value['image'] case final String filename
          when _storedFilename.hasMatch(filename)) {
        _filename = filename;
        final file = await _file(filename);
        if (await file.length() <= _maxBytes) {
          final bytes = await file.readAsBytes();
          await _validate(bytes);
          _image = bytes;
        }
      }
    } catch (_) {
      // Unavailable storage or an unreadable image falls back to the base theme.
    }
  }

  Future<File> _file(String name) async {
    final directory = await _appDirectory();
    return File.fromUri(directory.uri.resolve(name));
  }

  void _setBusy(bool busy) {
    _busy = busy;
    notifyListeners();
  }

  Future<void> select(BackgroundMode next) async {
    if (_busy ||
        next == mode ||
        next == BackgroundMode.image && _image == null) {
      return;
    }
    _mode = next;
    notifyListeners();
    await save();
  }

  Future<void> setCover(bool value) async {
    if (_busy || _cover == value) return;
    _cover = value;
    notifyListeners();
    await save();
  }

  void setOpacity(BackgroundLayer layer, int amount) {
    if (_busy || opacity(layer) == amount) return;
    _alphas[layer] = amount.clamp(0, 100);
    notifyListeners();
  }

  Future<void> save() => _save(mode: _mode, filename: _filename);

  Future<void> _save({
    required BackgroundMode mode,
    required String? filename,
  }) {
    final data = jsonEncode({
      'mode': mode.name,
      'cover': _cover,
      'image': filename,
      for (final layer in BackgroundLayer.values) layer.name: opacity(layer),
    });
    final write = _writes.then((_) => _storage.setString(_key, data));
    _writes = write.catchError((Object _) {});
    return write;
  }

  Future<void> chooseImage() async {
    if (_busy) return;
    _setBusy(true);
    File? pending;
    try {
      final selection = await _pickImage();
      if (selection == null) return;
      if ((await selection.length() ?? 0) > _maxBytes) {
        throw const FormatException();
      }
      final data = BytesBuilder(copy: false);
      await for (final chunk in selection.read()) {
        if (data.length + chunk.length > _maxBytes) {
          throw const FormatException();
        }
        data.add(chunk);
      }
      final bytes = data.takeBytes();
      await _validate(bytes);
      final filename = 'launcher-wallpaper-${const Uuid().v4()}.image';
      pending = await _file(filename);
      await pending.parent.create(recursive: true);
      await pending.writeAsBytes(bytes, flush: true);
      await _save(mode: BackgroundMode.image, filename: filename);
      final previous = _filename;
      _filename = filename;
      _image = bytes;
      _mode = BackgroundMode.image;
      pending = null;
      await _removeFile(previous);
    } finally {
      if (pending != null) await _removeFile(pending.uri.pathSegments.last);
      _setBusy(false);
    }
  }

  Future<void> removeImage() async {
    if (_busy || _filename == null) return;
    _setBusy(true);
    try {
      final previous = _filename;
      final next = _mode == BackgroundMode.image ? BackgroundMode.plain : _mode;
      await _save(mode: next, filename: null);
      _filename = null;
      _image = null;
      _mode = next;
      await _removeFile(previous);
    } finally {
      _setBusy(false);
    }
  }

  Future<void> _removeFile(String? name) async {
    if (name == null) return;
    try {
      final file = await _file(name);
      if (await file.exists()) await file.delete();
    } catch (_) {
      // Clearing the stored reference already disables the image.
    }
  }

  static Future<void> _validate(Uint8List bytes) async {
    if (bytes.isEmpty || bytes.length > _maxBytes) {
      throw const FormatException();
    }
    final buffer = await ui.ImmutableBuffer.fromUint8List(bytes);
    try {
      final descriptor = await ui.ImageDescriptor.encoded(buffer);
      try {
        if (descriptor.width * descriptor.height > 48 * 1024 * 1024) {
          throw const FormatException();
        }
        final landscape = descriptor.width >= descriptor.height;
        final codec = await descriptor.instantiateCodec(
          targetWidth: landscape ? 96 : null,
          targetHeight: landscape ? null : 96,
        );
        try {
          (await codec.getNextFrame()).image.dispose();
        } finally {
          codec.dispose();
        }
      } finally {
        descriptor.dispose();
      }
    } finally {
      buffer.dispose();
    }
  }
}
