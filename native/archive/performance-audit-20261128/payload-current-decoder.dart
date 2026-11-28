import 'dart:convert';
import 'dart:isolate';
import 'dart:typed_data';
Future<Object?> decodeAccountJson(Uint8List bytes) async {
  if (bytes.length < 64 * 1024) return jsonDecode(utf8.decode(bytes));
  // Pass bytes alone, avoiding capture of the HTTP client or account session.
  return Isolate.run(() => jsonDecode(utf8.decode(bytes)));
}
