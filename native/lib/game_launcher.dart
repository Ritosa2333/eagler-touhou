import 'dart:io';
import 'dart:typed_data';

import 'catalog.dart';
import 'game_preferences.dart';
import 'game_file_archive.dart';
import 'game_resources.dart';
import 'game_user_files.dart';
import 'gamepad_preferences.dart';
import 'local_duel_preferences.dart';
import 'touch_preferences.dart';

enum GameLaunchStatus { started, nativeRuntimeUnavailable }

abstract interface class GameLauncher {
  Future<GameLaunchStatus> launch(
    Game game, {
    required bool multiplayer,
    required GamePreferences preferences,
    required Map<String, String> keyboardAliases,
    required PadMappings gamepadMappings,
    required LocalDuelPreferences? localDuel,
    required TouchConfiguration touch,
    required String? dataPath,
    required String? userDataPath,
    required String? replayPath,
  });
}

class GameLaunchService {
  const GameLaunchService(
    this.runtime,
    this.preferences,
    this.resources,
    this.files, {
    this.history,
  });

  final GameLauncher runtime;
  final GamePreferenceStore preferences;
  final GameResources resources;
  final GameUserFiles files;
  final GamePlayHistory? history;

  Future<T> withArchivedReplay<T>(
    Game game,
    String name,
    Uint8List bytes,
    Future<T> Function(File, Future<bool> Function()) use,
  ) async {
    final options = await preferences.load(game, multiplayer: true);
    final workspace = files.workspace(
      game,
      multiplayer: true,
      language: options.values['language'] as String,
    );
    if (workspace == null) {
      throw const GameFileError(GameFileIssue.replayUnsupported);
    }
    return workspace.withArchivedReplay(name, bytes, use);
  }

  Future<GameLaunchStatus?> start(
    Game game, {
    required bool multiplayer,
    required bool Function() isActive,
    File? replayFile,
    Future<TouchOptions?> Function(TouchOptions)? prepareTouch,
  }) async {
    final options = await preferences.load(game, multiplayer: multiplayer);
    final workspace = files.workspace(
      game,
      multiplayer: multiplayer,
      language: options.values['language'] as String,
    );
    if (!isActive()) return null;
    final userData = await workspace?.directory();
    final data = await resources.installed(game.data);
    if (!isActive()) return null;
    final keyboard = await preferences.keyboard.load(game);
    final gamepad = await preferences.gamepad.load(game.id);
    final localDuel = game.id == 'th09' && !multiplayer
        ? await preferences.localDuel.load()
        : null;
    var touch = await preferences.touch.load(game.id);
    if (!isActive()) return null;
    if (prepareTouch != null) {
      final prepared = await prepareTouch(touch.options);
      if (prepared == null || !isActive()) return null;
      touch = touch.withOptions(prepared);
    }
    final status = await runtime.launch(
      game,
      multiplayer: multiplayer,
      preferences: options.forLaunch(
        externalMidiEnabled: preferences.midi.enabled,
      ),
      keyboardAliases: keyboard.active.aliases,
      gamepadMappings: gamepad,
      localDuel: localDuel,
      touch: touch,
      dataPath: data?.path,
      userDataPath: userData?.path,
      replayPath: replayFile?.path,
    );
    if (status == GameLaunchStatus.started &&
        !multiplayer &&
        replayFile == null) {
      await history?.record(game.id);
    }
    return status;
  }
}

class NativeGamePlaceholder implements GameLauncher {
  const NativeGamePlaceholder();

  @override
  Future<GameLaunchStatus> launch(
    Game game, {
    required bool multiplayer,
    required GamePreferences preferences,
    required Map<String, String> keyboardAliases,
    required PadMappings gamepadMappings,
    required LocalDuelPreferences? localDuel,
    required TouchConfiguration touch,
    required String? dataPath,
    required String? userDataPath,
    required String? replayPath,
  }) async {
    return GameLaunchStatus.nativeRuntimeUnavailable;
  }
}
