import 'dart:io';

import 'package:file_picker/file_picker.dart';
import 'package:file_saver/file_saver.dart';

typedef GameFileInput = ({
  String name,
  Future<int?> Function() length,
  Stream<List<int>> Function() read,
});

Future<GameFileInput?> pickGameFile(List<String> extensions) async {
  // Proprietary game formats are often absent from Android's MIME registry.
  final file = await FilePicker.pickFile(
    type: Platform.isAndroid ? FileType.any : FileType.custom,
    allowedExtensions: Platform.isAndroid ? null : extensions,
  );
  if (file == null) return null;
  return (name: file.name, length: file.length, read: file.readAsByteStream);
}

Future<List<GameFileInput>> pickGameFiles(List<String> extensions) async {
  final files = await FilePicker.pickFiles(
    type: Platform.isAndroid ? FileType.any : FileType.custom,
    allowedExtensions: Platform.isAndroid ? null : extensions,
  );
  return files
      .map(
        (file) =>
            (name: file.name, length: file.length, read: file.readAsByteStream),
      )
      .toList(growable: false);
}

Future<bool> exportGameFile(String name, File file) async =>
    await FileSaver.instance.saveAs(
      name: name,
      file: file,
      includeExtension: false,
      mimeType: name.endsWith('.zip') ? MimeType.zip : MimeType.other,
    ) !=
    null;
