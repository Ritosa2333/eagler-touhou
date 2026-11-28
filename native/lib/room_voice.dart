import 'dart:async';

import 'package:audioplayers/audioplayers.dart';

import 'room_phrases.dart';

class RoomVoice {
  AudioPlayer? _player;
  Future<void> _tail = Future.value();
  int _revision = 0;
  bool _closed = false;

  void play(String phrase, int volume) {
    if (_closed || volume <= 0 || !roomPhrases.containsKey(phrase)) return;
    final revision = ++_revision;
    _tail = _tail.then((_) async {
      if (_closed || revision != _revision) return;
      try {
        await AudioPlayer.global.ensureInitialized();
        if (_closed || revision != _revision) return;
        final player = _player ??= AudioPlayer();
        await player.stop();
        if (_closed || revision != _revision) return;
        await player.play(
          AssetSource('quick-chat/$phrase.wav'),
          volume: volume.clamp(0, 100) / 100,
        );
      } on Exception {
        // Text delivery does not depend on the audio device.
      }
    });
  }

  void dispose() {
    if (_closed) return;
    _closed = true;
    unawaited(
      _tail.then((_) async {
        try {
          await _player?.dispose();
        } on Exception {
          // The audio backend can be unavailable during shutdown.
        }
      }),
    );
  }
}
