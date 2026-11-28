import 'dart:convert';
import 'dart:io';
import 'package:integration_test/integration_test_driver.dart';

Future<void> main() async {
  await integrationDriver(
    writeResponseOnFailure: true,
    responseDataCallback: (data) async {
      final path = Platform.environment['TOUHOU_PERF_REPORT']!;
      await File(path).writeAsString(const JsonEncoder.withIndent('  ').convert(data));
    },
  );
}
