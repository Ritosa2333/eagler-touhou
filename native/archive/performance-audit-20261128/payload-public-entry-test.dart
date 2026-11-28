import 'dart:convert';
import 'dart:io';
import 'package:flutter_test/flutter_test.dart';
import 'package:touhouonline/account_api.dart';

void main() {
  test('Production public responses load through the application API', () async {
    final api = AccountApi();
    addTearDown(api.close);
    final board = await api.th09Board();
    final ratings = await api.th09LobbyRatings([], authenticated: false);
    expect(board.players, isNotEmpty);
    expect(ratings.players, isNotEmpty);
    expect(board.minimumGames, greaterThan(0));
    final result = {
      'checkedAtUtc': DateTime.now().toUtc().toIso8601String(),
      'origin': 'https://touhouonline.com',
      'authentication': false,
      'entry': 'Current AccountApi.th09Board and th09LobbyRatings',
      'boardPlayers': board.players.length,
      'ratingPlayers': ratings.players.length,
      'minimumGames': board.minimumGames,
      'scope': 'Headless Flutter test using production API methods and real HTTPS. No account writes, presentation or authenticated payload acceptance.',
    };
    await File('D:/Temp/native-performance-20261128/payload-cost/public-entry.json')
        .writeAsString('${const JsonEncoder.withIndent('  ').convert(result)}\n');
  });
}
