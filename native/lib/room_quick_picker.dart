import 'package:material_ui/material_ui.dart';

import 'catalog.dart';
import 'l10n/strings.dart';
import 'launcher_select.dart';
import 'room_phrases.dart';

class RoomQuickPicker extends StatefulWidget {
  const RoomQuickPicker({super.key, required this.preferences});

  final LauncherPreferences preferences;

  @override
  State<RoomQuickPicker> createState() => _RoomQuickPickerState();
}

class _RoomQuickPickerState extends State<RoomQuickPicker> {
  bool _all = false;
  bool _editing = false;
  int? _slot;
  bool _saving = false;
  bool _saveFailed = false;

  void _back() {
    if (_saving) return;
    setState(() {
      if (_slot != null) {
        _slot = null;
      } else {
        _editing = false;
      }
      _saveFailed = false;
    });
  }

  Future<void> _pick(int index, String id) async {
    if (_saving) return;
    final slot = _slot;
    if (!_editing) {
      Navigator.of(context).pop(id);
    } else if (slot == null) {
      setState(() => _slot = index);
    } else {
      setState(() {
        _saving = true;
        _saveFailed = false;
      });
      try {
        await widget.preferences.setQuickChatSlot(slot, id);
        if (mounted) setState(() => _slot = null);
      } on Exception {
        if (mounted) setState(() => _saveFailed = true);
      } finally {
        if (mounted) setState(() => _saving = false);
      }
    }
  }

  @override
  Widget build(BuildContext context) {
    final strings = context.strings;
    final language = bundledLanguage(Localizations.localeOf(context));
    final style = Theme.of(context).textTheme;
    final slot = _slot;
    final inSlots = _slot == null && (_editing || !_all);
    final ids = inSlots
        ? widget.preferences.quickChatSlots
        : roomPhrases.keys.toList();
    return PopScope<String>(
      canPop: !_editing,
      onPopInvokedWithResult: (popped, _) {
        if (!popped) _back();
      },
      child: CallbackShortcuts(
        bindings: {
          if (inSlots)
            for (final (index, id) in ids.indexed)
              CharacterActivator('${index + 1}'): () => _pick(index, id),
        },
        child: Focus(
          autofocus: true,
          child: FractionallySizedBox(
            heightFactor: .78,
            child: SafeArea(
              top: false,
              child: Column(
                children: [
                  Padding(
                    padding: const EdgeInsetsDirectional.fromSTEB(16, 0, 8, 6),
                    child: Row(
                      children: [
                        if (_editing)
                          BackButton(onPressed: _saving ? null : _back),
                        Expanded(
                          child: Text(
                            slot != null
                                ? strings.roomQuickChatChooseSlot(slot + 1)
                                : _editing
                                ? strings.roomQuickChatEdit
                                : strings.roomQuickChat,
                            style: style.titleMedium,
                          ),
                        ),
                        if (!_editing)
                          IconButton(
                            tooltip: strings.roomQuickChatEdit,
                            icon: const Icon(Icons.settings_outlined),
                            onPressed: () => setState(() => _editing = true),
                          ),
                      ],
                    ),
                  ),
                  if (!_editing)
                    Padding(
                      padding: const EdgeInsets.fromLTRB(16, 0, 16, 8),
                      child: LauncherSegments<bool>(
                        value: _all,
                        onChanged: (all) => setState(() => _all = all),
                        segments: [
                          ButtonSegment(
                            value: false,
                            label: Text(strings.roomQuickChatFavorites),
                          ),
                          ButtonSegment(
                            value: true,
                            label: Text(strings.roomQuickChatAll),
                          ),
                        ],
                      ),
                    ),
                  if (_saveFailed)
                    Semantics(
                      liveRegion: true,
                      child: Text(strings.gameSaveSettingsFailed),
                    ),
                  Expanded(
                    child: ListView.builder(
                      key: ValueKey((_editing, _slot, _all)),
                      primary: false,
                      padding: const EdgeInsets.fromLTRB(16, 0, 16, 10),
                      itemCount: ids.length,
                      itemBuilder: (_, index) => ListTile(
                        contentPadding: const EdgeInsets.symmetric(
                          horizontal: 8,
                        ),
                        leading: inSlots
                            ? SizedBox(
                                width: 16,
                                child: Text(
                                  '${index + 1}',
                                  style: style.labelMedium,
                                ),
                              )
                            : null,
                        title: Text(
                          roomPhraseLabel(roomPhrases[ids[index]]!, language),
                        ),
                        trailing: _editing && _slot == null
                            ? const Icon(Icons.chevron_right, size: 18)
                            : null,
                        selected:
                            slot != null &&
                            widget.preferences.quickChatSlots[slot] ==
                                ids[index],
                        enabled: !_saving,
                        onTap: () => _pick(index, ids[index]),
                      ),
                    ),
                  ),
                ],
              ),
            ),
          ),
        ),
      ),
    );
  }
}
