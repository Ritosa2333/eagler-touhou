import 'dart:convert';
import 'dart:io';

Future<Map<String, dynamic>> readSiteManifest(
  Uri origin,
  HttpClient client,
  Duration timeout,
) async {
  final request = await client
      .getUrl(origin.resolve('/site-manifest.json'))
      .timeout(timeout);
  request.followRedirects = false;
  request.headers.set(HttpHeaders.acceptHeader, 'application/json');
  final response = await request.close().timeout(timeout);
  if (response.statusCode != HttpStatus.ok) {
    await response.drain<void>().timeout(timeout);
    throw const FormatException('Site manifest unavailable');
  }
  var byteCount = 0;
  final raw = await utf8.decoder
      .bind(
        response.map((chunk) {
          byteCount += chunk.length;
          if (byteCount > 1024 * 1024) {
            throw const FormatException('Site manifest is too large');
          }
          return chunk;
        }),
      )
      .join()
      .timeout(timeout);
  final data = jsonDecode(raw);
  if (data is! Map<String, dynamic> ||
      data['schema'] != 'eagler-touhou/site-manifest/1' ||
      data['shared'] is! Map<String, dynamic> ||
      data['games'] is! Map<String, dynamic>) {
    throw const FormatException('Invalid site manifest');
  }
  return data;
}
