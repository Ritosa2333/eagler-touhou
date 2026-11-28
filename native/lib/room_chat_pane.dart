import 'package:material_ui/material_ui.dart';
import 'package:prism_ui/prism_ui.dart' as prism;

import 'catalog.dart';
import 'l10n/strings.dart';
import 'launcher_select.dart';
import 'room_chat.dart';
import 'room_phrases.dart';
import 'room_session.dart';
import 'room_quick_picker.dart';

class RoomChatPane extends StatefulWidget {
  const RoomChatPane({
    super.key,
    required this.session,
    required this.preferences,
    required this.signedIn,
    required this.signIn,
    required this.avatar,
  });

  final RoomSession session;
  final LauncherPreferences preferences;
  final bool signedIn;
  final VoidCallback signIn;
  final Widget Function(RoomMessage) avatar;

  @override
  State<RoomChatPane> createState() => _RoomChatPaneState();
}

class _RoomChatPaneState extends State<RoomChatPane> {
  final _draft = TextEditingController();
  final _log = ScrollController();
  bool _newMessage = false;
  bool _sendingText = false;
  RoomMessage? _lastMessage;
  late int _ack;

  RoomChat get _chat => widget.session.chat;
  int get _serial => widget.session.snapshot?.serial ?? 0;
  bool get _joined =>
      widget.session.connected &&
      (widget.session.localSeat != null || widget.session.watching);

  @override
  void initState() {
    super.initState();
    _ack = _chat.resultRevision;
    _lastMessage = _chat.messages.lastOrNull;
    _chat.addListener(_update);
    _log.addListener(_readLog);
    WidgetsBinding.instance.addPostFrameCallback((_) => _latest());
  }

  void _readLog() {
    if (_newMessage && _log.position.extentAfter < 8) {
      setState(() => _newMessage = false);
    }
  }

  void _latest() {
    if (mounted && _log.hasClients) _log.jumpTo(_log.position.maxScrollExtent);
  }

  void _update() {
    final last = _chat.messages.lastOrNull;
    final atLatest = !_log.hasClients || _log.position.extentAfter < 8;
    final changed = last != null && !identical(_lastMessage, last);
    _lastMessage = last;
    if (_ack != _chat.resultRevision) {
      _ack = _chat.resultRevision;
      if (_sendingText && _chat.result == null) _draft.clear();
      _sendingText = false;
    }
    setState(() {
      if (changed && !atLatest) _newMessage = true;
    });
    if (changed && atLatest) {
      WidgetsBinding.instance.addPostFrameCallback((_) => _latest());
    }
  }

  Future<void> _pickPhrase() async {
    final selected = await showModalBottomSheet<String>(
      context: context,
      sheetAnimationStyle: LauncherControl.overlayMotion(context),
      isScrollControlled: true,
      constraints: const BoxConstraints(maxWidth: 460),
      showDragHandle: true,
      useSafeArea: true,
      builder: (_) => RoomQuickPicker(preferences: widget.preferences),
    );
    if (mounted && selected != null && _joined) _chat.phrase(selected, _serial);
  }

  Future<void> _report(RoomMessage row) async {
    final strings = context.strings;
    var reason = '';
    final draft = await showDialog<String>(
      context: context,
      animationStyle: LauncherControl.overlayMotion(context),
      builder: (dialog) => AlertDialog(
        scrollable: true,
        title: Text(strings.roomChatReportTitle),
        content: TextField(
          maxLength: 500,
          minLines: 2,
          maxLines: 4,
          onChanged: (value) => reason = value,
          decoration: InputDecoration(labelText: strings.roomChatReportReason),
        ),
        actions: [
          TextButton(
            onPressed: () => Navigator.of(dialog).pop(),
            child: Text(strings.cancel),
          ),
          FilledButton(
            onPressed: () => Navigator.of(dialog).pop(reason),
            child: Text(strings.roomChatReport),
          ),
        ],
      ),
    );
    if (mounted && draft != null && _joined) _chat.report(row, draft);
  }

  void _sendText() {
    _sendingText = _chat.sendText(_draft.text, _serial);
  }

  @override
  void dispose() {
    _chat.removeListener(_update);
    _log.dispose();
    _draft.dispose();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    final theme = Theme.of(context);
    final strings = context.strings;
    final language = bundledLanguage(Localizations.localeOf(context));
    final rows = _chat.messages;
    final canWrite = _joined && _chat.registered && !_chat.banned;
    final status = _chat.result?.label(strings);
    return prism.Card(
      inert: true,
      child: Padding(
        padding: const EdgeInsets.all(16),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.stretch,
          spacing: 8,
          children: [
            Text(
              strings.roomChatTitle,
              style: prism.AppTextStyle.headline.of(context),
            ),
            SizedBox(
              height: 150,
              child: rows.isEmpty
                  ? Center(
                      child: Text(
                        strings.roomChatEmpty,
                        style: theme.textTheme.bodyMedium?.copyWith(
                          color: theme.colorScheme.onSurfaceVariant,
                        ),
                      ),
                    )
                  : Stack(
                      alignment: AlignmentDirectional.bottomEnd,
                      children: [
                        Scrollbar(
                          controller: _log,
                          child: ScrollConfiguration(
                            behavior: ScrollConfiguration.of(context)
                                .copyWith(scrollbars: false),
                            child: ListView.separated(
                              controller: _log,
                              padding: const EdgeInsetsDirectional.only(end: 5),
                              itemCount: rows.length,
                              separatorBuilder: (_, _) =>
                                  const SizedBox(height: 7),
                              itemBuilder: (_, index) =>
                                  _message(rows[index], language),
                            ),
                          ),
                        ),
                        if (_newMessage)
                          IconButton.filledTonal(
                            tooltip: strings.roomChatLatest,
                            onPressed: _latest,
                            icon: const Icon(Icons.keyboard_double_arrow_down),
                          ),
                      ],
                    ),
            ),
            if (status != null)
              Semantics(liveRegion: true, child: Text(status)),
            Row(
              spacing: 6,
              children: [
                IconButton.outlined(
                  tooltip: strings.roomQuickChat,
                  onPressed: _joined ? _pickPhrase : null,
                  icon: const Icon(Icons.chat_bubble_outline),
                ),
                Expanded(
                  child: canWrite
                      ? TextField(
                          controller: _draft,
                          readOnly: _chat.busy,
                          maxLength: 500,
                          textInputAction: TextInputAction.send,
                          onSubmitted: (_) => _sendText(),
                          decoration: InputDecoration(
                            hintText: strings.roomChatMessage,
                            counterText: '',
                            isDense: true,
                          ),
                        )
                      : Text(
                          !_joined
                              ? strings.roomChatJoin
                              : _chat.banned
                              ? strings.roomChatBan
                              : widget.signedIn
                              ? strings.roomChatUnavailable
                              : strings.roomChatSignIn,
                          style: theme.textTheme.bodySmall,
                        ),
                ),
                if (canWrite)
                  IconButton.filledTonal(
                    tooltip: strings.roomChatSend,
                    onPressed: _chat.busy ? null : _sendText,
                    icon: const Icon(Icons.send_outlined),
                  )
                else if (_joined && !widget.signedIn)
                  TextButton(
                    onPressed: widget.signIn,
                    child: Text(strings.signIn),
                  ),
              ],
            ),
          ],
        ),
      ),
    );
  }

  Widget _message(RoomMessage row, String language) {
    final theme = Theme.of(context);
    final strings = context.strings;
    final name = row.name.isEmpty ? strings.roomUnnamedSpectator : row.name;
    final phrase = roomPhrases[row.phrase];
    final message = phrase == null
        ? row.text
        : roomPhraseLabel(phrase, language);
    return Row(
      crossAxisAlignment: CrossAxisAlignment.start,
      spacing: 8,
      children: [
        widget.avatar(row),
        Expanded(
          child: Column(
            crossAxisAlignment: CrossAxisAlignment.start,
            spacing: 2,
            children: [
              Text(
                row.spectator ? '$name | ${strings.roomChatSpectator}' : name,
                style: theme.textTheme.labelSmall,
                maxLines: 1,
                overflow: TextOverflow.ellipsis,
              ),
              SelectableText(message, style: theme.textTheme.bodySmall),
            ],
          ),
        ),
        PopupMenuButton<bool>(
          tooltip: strings.roomChatActions,
          iconSize: 18,
          onSelected: (report) =>
              report ? _report(row) : _chat.mute(row.client),
          itemBuilder: (_) => [
            PopupMenuItem(value: false, child: Text(strings.roomChatMute)),
            if (_chat.canReport(row))
              PopupMenuItem(
                enabled: _joined && !_chat.busy,
                value: true,
                child: Text(strings.roomChatReportTitle),
              ),
          ],
        ),
      ],
    );
  }
}
