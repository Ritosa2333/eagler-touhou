import 'package:material_ui/material_ui.dart';

import 'catalog.dart';
import 'game_preferences.dart';
import 'gamepad_settings.dart';
import 'input_settings_frame.dart';
import 'keyboard_settings.dart';
import 'l10n/strings.dart';
import 'launcher_select.dart';
import 'local_duel_settings.dart';

enum GameInputDevice { keyboard, gamepad }

class GameInputDialog extends StatefulWidget {
  const GameInputDialog({
    super.key,
    required this.game,
    required this.store,
    required this.device,
    required this.allowLocalDuel,
  });

  final Game game;
  final GamePreferenceStore store;
  final GameInputDevice device;
  final bool allowLocalDuel;

  @override
  State<GameInputDialog> createState() => _GameInputDialogState();
}

class _GameInputDialogState extends State<GameInputDialog> {
  late GameInputDevice _device = widget.device;
  bool _localDuel = false;
  bool _saving = false;

  void _onSaving(bool value) => setState(() => _saving = value);

  @override
  Widget build(BuildContext context) {
    final strings = context.strings;
    return InputSettingsFrame(
      title: strings.gameInputSettings,
      saving: _saving,
      controls: Column(
        crossAxisAlignment: CrossAxisAlignment.stretch,
        spacing: 8,
        children: [
          if (widget.allowLocalDuel)
            InputSettingSelect<bool>(
              label: strings.inputContext,
              value: _localDuel,
              options: {
                false: strings.inputGeneralScope,
                true: strings.gameLocalDuel,
              },
              onChanged: _saving
                  ? null
                  : (value) => setState(() => _localDuel = value),
            ),
          if (!_localDuel)
            LauncherSegments<GameInputDevice>(
              segments: [
                ButtonSegment(
                  value: GameInputDevice.keyboard,
                  icon: const Icon(Icons.keyboard_outlined),
                  label: Text(
                    strings.inputKeyboardDevice,
                    maxLines: 1,
                    overflow: TextOverflow.ellipsis,
                  ),
                ),
                ButtonSegment(
                  value: GameInputDevice.gamepad,
                  icon: const Icon(Icons.sports_esports_outlined),
                  label: Text(
                    strings.inputGamepadDevice,
                    maxLines: 1,
                    overflow: TextOverflow.ellipsis,
                  ),
                ),
              ],
              value: _device,
              onChanged: _saving
                  ? null
                  : (device) => setState(() => _device = device),
            ),
        ],
      ),
      body: _localDuel
          ? LocalDuelSettings(
              key: const ValueKey('local-duel-input'),
              store: widget.store.localDuel,
              gamepads: widget.store.gamepad,
              onSaving: _onSaving,
            )
          : switch (_device) {
              GameInputDevice.keyboard => KeyboardSettings(
                key: const ValueKey(GameInputDevice.keyboard),
                game: widget.game,
                store: widget.store.keyboard,
                onSaving: _onSaving,
              ),
              GameInputDevice.gamepad => GamepadSettings(
                key: const ValueKey(GameInputDevice.gamepad),
                gameId: widget.game.id,
                store: widget.store.gamepad,
                onSaving: _onSaving,
              ),
            },
    );
  }
}
