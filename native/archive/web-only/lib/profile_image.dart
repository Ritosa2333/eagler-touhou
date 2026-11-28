import 'dart:convert';
import 'dart:isolate';
import 'dart:math' as math;
import 'dart:typed_data';
import 'dart:ui' show Rect;

import 'package:file_picker/file_picker.dart';
import 'package:image/image.dart' as image;

const profileImageInputLimit = 20 * 1024 * 1024;
const profileImageLimit = 150000;
const profileThumbnailLimit = 8192;

enum ProfileImageKind {
  avatar(1),
  cover(3);

  const ProfileImageKind(this.aspect);
  final double aspect;
}

typedef ProfileCropSource = ({Uint8List bytes, int width, int height});
typedef PreparedProfileImage = ({String data, String? thumbnail});

class ProfileImageException implements Exception {
  const ProfileImageException(this.message);
  final String message;
}

Future<ProfileCropSource?> pickProfileImage() async {
  final chosen = await FilePicker.pickFile(type: FileType.image);
  if (chosen == null) return null;
  final known = await chosen.length();
  if (known != null && known > profileImageInputLimit) {
    throw const ProfileImageException('图片不能超过 20 MiB');
  }
  final bytes = BytesBuilder(copy: false);
  await for (final chunk in chosen.readAsByteStream()) {
    if (bytes.length + chunk.length > profileImageInputLimit) {
      throw const ProfileImageException('图片不能超过 20 MiB');
    }
    bytes.add(chunk);
  }
  return prepareProfileSource(bytes.takeBytes());
}

Future<ProfileCropSource> prepareProfileSource(Uint8List bytes) =>
    Isolate.run(() => _prepareSource(bytes));

ProfileCropSource _prepareSource(Uint8List bytes) {
  if (bytes.length > profileImageInputLimit) {
    throw const ProfileImageException('图片不能超过 20 MiB');
  }
  final original = image.bakeOrientation(_decode(bytes));
  final scale = math.min(1, 2500 / math.max(original.width, original.height));
  final resized = scale == 1
      ? original
      : image.copyResize(
          original,
          width: math.max(1, (original.width * scale).round()),
          height: math.max(1, (original.height * scale).round()),
          interpolation: image.Interpolation.average,
        );
  return (
    bytes: image.encodePng(resized, singleFrame: true, level: 2),
    width: resized.width,
    height: resized.height,
  );
}

image.Image _decode(Uint8List bytes) {
  try {
    final decoder = image.findDecoderForData(bytes);
    if (decoder is! image.JpegDecoder &&
        decoder is! image.PngDecoder &&
        decoder is! image.WebPDecoder) {
      throw const ProfileImageException('请选择 PNG、JPEG 或 WebP 图片');
    }
    final info = decoder!.startDecode(bytes);
    if (info == null || info.width < 1 || info.height < 1) {
      throw const ProfileImageException('无法读取图片');
    }
    if (info.width * info.height > 60000000) {
      throw const ProfileImageException('图片像素过多，请先缩小图片');
    }
    return decoder.decodeFrame(0) ??
        (throw const ProfileImageException('无法读取图片'));
  } on ProfileImageException {
    rethrow;
  } catch (_) {
    throw const ProfileImageException('无法读取图片');
  }
}

Rect fitProfileCrop(ProfileCropSource source, ProfileImageKind kind) {
  final height = math.min(source.height.toDouble(), source.width / kind.aspect);
  final width = height * kind.aspect;
  return Rect.fromLTWH(
    (source.width - width) / 2,
    (source.height - height) / 2,
    width,
    height,
  );
}

Future<PreparedProfileImage> encodeProfileCrop(
  ProfileCropSource source,
  ProfileImageKind kind,
  Rect crop,
) => Isolate.run(() {
  final avatar = kind == ProfileImageKind.avatar;
  final result = _cropPixels(source, kind, crop, avatar ? 512 : 266);
  return (
    data: _encodeSizeLimited(result, profileImageLimit),
    thumbnail: avatar ? _encodeThumbnail(result) : null,
  );
});

Future<Uint8List> renderProfileCropPreview(
  ProfileCropSource source,
  ProfileImageKind kind,
  Rect crop,
) => Isolate.run(() {
  final avatar = kind == ProfileImageKind.avatar;
  final result = _cropPixels(source, kind, crop, avatar ? 512 : 108);
  return avatar
      ? UriData.parse(_encodeThumbnail(result)).contentAsBytes()
      : image.encodePng(result, singleFrame: true, level: 2);
});

image.Image _cropPixels(
  ProfileCropSource source,
  ProfileImageKind kind,
  Rect crop,
  int maxHeight,
) {
  final original = _decode(source.bytes);
  if (!crop.isFinite ||
      crop.isEmpty ||
      original.width != source.width ||
      original.height != source.height) {
    throw const ProfileImageException('裁剪范围无效');
  }
  final height = math.min(
    crop.height,
    math.min(source.height.toDouble(), source.width / kind.aspect),
  );
  final width = height * kind.aspect;
  final left = (crop.center.dx - width / 2).clamp(0.0, original.width - width);
  final top = (crop.center.dy - height / 2).clamp(
    0.0,
    original.height - height,
  );

  final avatar = kind == ProfileImageKind.avatar;
  final ratio = avatar ? 1 : 6;
  final outputHeight = math.min(maxHeight, math.max(1, height.floor()));
  final outputWidth = outputHeight * ratio;
  final firstX = avatar ? left : left - width / 2;
  final scale = height / outputHeight;
  final result = image.Image(
    width: outputWidth,
    height: outputHeight,
    numChannels: original.hasAlpha ? 4 : 3,
  );
  for (var y = 0; y < result.height; y++) {
    for (var x = 0; x < result.width; x++) {
      final column = firstX + (x + .5) * scale;
      // Match the web cover convention: the central half is the selected 3:1 crop.
      final mirrored = column < 0
          ? -column
          : column >= original.width
          ? original.width * 2 - column - 1
          : column;
      result.setPixel(
        x,
        y,
        original.getPixelInterpolate(
          mirrored.clamp(0, original.width - 1),
          (top + (y + .5) * scale).clamp(0, original.height - 1),
          interpolation: image.Interpolation.linear,
        ),
      );
    }
  }

  return result;
}

Future<String> encodeStoredProfileThumbnail(Uint8List bytes) => Isolate.run(() {
  final source = image.bakeOrientation(_decode(bytes));
  final edge = math.min(source.width, source.height);
  final square = image.copyCrop(
    source,
    x: (source.width - edge) ~/ 2,
    y: (source.height - edge) ~/ 2,
    width: edge,
    height: edge,
  );
  return _encodeThumbnail(square);
});

String _encodeThumbnail(image.Image square) => _encodeSizeLimited(
  image.copyResize(
    square,
    width: math.min(64, square.width),
    interpolation: image.Interpolation.average,
  ),
  profileThumbnailLimit,
);

String _encodeSizeLimited(image.Image source, int limit) {
  var candidate = source;
  // Compression and any resize run off the UI isolate.
  for (var attempt = 0; attempt < 3; attempt++) {
    for (final quality in [82, 62, 42]) {
      final bytes = image.encodeWebP(
        candidate,
        lossless: false,
        quality: quality,
        method: 2,
        singleFrame: true,
        alphaQuality: quality,
      );
      if (bytes.isNotEmpty && bytes.length <= limit) {
        return 'data:image/webp;base64,${base64Encode(bytes)}';
      }
    }
    final height = math.max(1, (candidate.height * .72).floor());
    candidate = image.copyResize(
      candidate,
      width: math.max(1, (height * source.width / source.height).round()),
      height: height,
      interpolation: image.Interpolation.average,
    );
  }
  throw const ProfileImageException('图片压缩后仍超出大小限制，请选另一张');
}

Uint8List? profileImageBytes(String data) {
  if (data.isEmpty || data.length > 1000050) return null;
  try {
    final uri = UriData.parse(data);
    return uri.isBase64 &&
            const [
              'image/png',
              'image/jpeg',
              'image/webp',
            ].contains(uri.mimeType)
        ? uri.contentAsBytes()
        : null;
  } on FormatException {
    return null;
  }
}
