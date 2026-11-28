import 'dart:convert';
import 'dart:io';

const th09ReplayLimit = 1024 * 1024;
const th09ReplayMetadataLimit = 16 * 1024;

typedef Th09ReplayMatch = ({
  List<String> names,
  DateTime startedAt,
  int frames,
  int winner,
});

String th09ReplayMetadataPath(String replay) =>
    '${replay.substring(0, replay.lastIndexOf('.'))}.json';

Future<Th09ReplayMatch?> readTh09ReplayMatch(File file) async {
  try {
    if (!await file.exists() || await file.length() > th09ReplayMetadataLimit) {
      return null;
    }
    final input = await file.open();
    try {
      final bytes = await input.read(th09ReplayMetadataLimit + 1);
      if (bytes.length > th09ReplayMetadataLimit) return null;
      final json = jsonDecode(utf8.decode(bytes));
      if (json
          case {
            'schema': 'eagler-touhou/th09-match-replay/1',
            'names': [final String left, final String right],
            'startedAt': final int startedAt,
            'frames': final int frames,
            'winner': final int winner,
          }
          when left.length <= 64 &&
              right.length <= 64 &&
              startedAt >= 0 &&
              startedAt <= 8640000000000000 &&
              frames >= 30 &&
              frames <= 9007199254740991 &&
              (winner == 0 || winner == 1)) {
        return (
          names: [left.isEmpty ? 'P1' : left, right.isEmpty ? 'P2' : right],
          startedAt: DateTime.fromMillisecondsSinceEpoch(
            startedAt,
            isUtc: true,
          ),
          frames: frames,
          winner: winner,
        );
      }
    } finally {
      await input.close();
    }
  } on FormatException {
    return null;
  } on FileSystemException {
    return null;
  }
  return null;
}
