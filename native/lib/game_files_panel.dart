import 'dart:io';
import 'dart:math' as math;

import 'package:material_ui/material_ui.dart';

import 'game_file_archive.dart';
import 'game_file_feedback.dart';
import 'game_launcher.dart';
import 'game_user_files.dart';
import 'l10n/strings.dart';
import 'launcher_select.dart';
import 'modal_keyboard_dismiss.dart';
import 'replay_manager.dart';

typedef _FileAction = (IconData, String, VoidCallback);

class GameFilesPanel extends StatefulWidget {
  const GameFilesPanel({
    super.key,
    required this.workspace,
    required this.play,
    this.padding = const EdgeInsets.fromLTRB(16, 0, 16, 12),
  });

  final GameFileWorkspace workspace;
  final EdgeInsetsGeometry padding;
  final Future<GameLaunchStatus?> Function(BuildContext, File) play;

  @override
  State<GameFilesPanel> createState() => _GameFilesPanelState();
}

class _GameFilesPanelState extends State<GameFilesPanel> {
  bool _busy = false;
  bool _transferring = false;
  String? _feedback;

  Future<void> _perform(Future<String?> Function() action) async {
    if (_busy) return;
    final focused = FocusManager.instance.primaryFocus;
    final trigger =
        focused?.context?.findAncestorStateOfType<_GameFilesPanelState>() ==
            this
        ? focused
        : null;
    setState(() => _busy = true);
    try {
      final note = await action();
      if (mounted && note != null) setState(() => _feedback = note);
    } on GameFileError catch (error) {
      if (mounted) setState(() => _feedback = error.message(context.strings));
    } on Exception {
      if (mounted) setState(() => _feedback = context.strings.gameFileFailed);
    } finally {
      if (mounted) {
        setState(() => _busy = false);
        restoreModalTriggerFocus(context, trigger);
      }
    }
  }

  Future<bool> _confirm(
    String title,
    String summary,
    String action, {
    bool destructive = false,
  }) async {
    final colors = Theme.of(context).colorScheme;
    final result = await showDialog<bool>(
      context: context,
      animationStyle: LauncherControl.overlayMotion(context),
      builder: (context) => ModalKeyboardDismiss(
        child: AlertDialog(
          scrollable: true,
          insetPadding: const EdgeInsets.symmetric(
            horizontal: 16,
            vertical: 24,
          ),
          title: Text(title),
          content: Text(summary),
          actions: [
            TextButton(
              autofocus: true,
              onPressed: () => Navigator.of(context).pop(false),
              child: Text(context.strings.cancel),
            ),
            FilledButton(
              style: destructive
                  ? FilledButton.styleFrom(
                      backgroundColor: colors.error,
                      foregroundColor: colors.onError,
                    )
                  : null,
              onPressed: () => Navigator.of(context).pop(true),
              child: Text(action),
            ),
          ],
        ),
      ),
    );
    return result == true;
  }

  Future<String?> _import(GameUserFileKind kind) async {
    final strings = context.strings;
    if (kind == GameUserFileKind.save &&
        !await _confirm(
          strings.gameImportSaveTitle,
          strings.gameSaveOverwrite,
          strings.gameContinueImport,
          destructive: true,
        )) {
      return null;
    }
    return _chooseAndImport(kind);
  }

  Future<String?> _chooseAndImport(GameUserFileKind kind) async {
    if (!mounted) return null;
    final workspace = widget.workspace;
    final selected = await workspace.pick(kind);
    if (!mounted || selected == null) return null;
    final files = await _transfer(() => workspace.importFile(kind, selected));
    return mounted ? context.strings.gameFilesImported(files.length) : null;
  }

  Future<String?> _export(GameUserFileKind kind) async {
    final result = await _transfer(
      kind == GameUserFileKind.save
          ? widget.workspace.exportSave
          : widget.workspace.exportReplays,
    );
    if (!mounted) return null;
    final strings = context.strings;
    if (result != null) return result ? strings.gameFileSaved : null;
    final title = kind == GameUserFileKind.save
        ? strings.gameNoSave
        : strings.gameNoReplay;
    return await _confirm(title, strings.gamePickLocal, strings.gameSelectFile)
        ? _chooseAndImport(kind)
        : null;
  }

  Future<T> _transfer<T>(Future<T> Function() action) async {
    setState(() => _transferring = true);
    try {
      return await action();
    } finally {
      if (mounted) setState(() => _transferring = false);
    }
  }

  Future<String?> _removeHint() async {
    final strings = context.strings;
    final confirmed = await _confirm(
      strings.gameDeleteHint,
      strings.gameDeleteHintNote,
      strings.gameFileDelete,
      destructive: true,
    );
    if (!mounted || !confirmed) return null;
    await _transfer(widget.workspace.deleteHints);
    return strings.gameHintDeleted;
  }

  void _manage() => showDialog<void>(
    context: context,
    animationStyle: LauncherControl.overlayMotion(context),
    barrierDismissible: false,
    builder: (dialog) =>
        ReplayManagerDialog(workspace: widget.workspace, play: widget.play),
  );

  @override
  Widget build(BuildContext context) {
    final strings = context.strings;
    final rows = <(GameUserFileKind, String, List<_FileAction>)>[
      (
        GameUserFileKind.save,
        strings.gameSave,
        [
          (
            Icons.download,
            strings.gameDownload,
            () => _perform(() => _export(GameUserFileKind.save)),
          ),
          (
            Icons.upload_outlined,
            strings.gameUpload,
            () => _perform(() => _import(GameUserFileKind.save)),
          ),
        ],
      ),
      (
        GameUserFileKind.replay,
        strings.gameReplay,
        [
          (
            Icons.download,
            strings.gameDownload,
            () => _perform(() => _export(GameUserFileKind.replay)),
          ),
          (Icons.folder_open_outlined, strings.gameManage, _manage),
        ],
      ),
      if (widget.workspace.catalog.hints.isNotEmpty)
        (
          GameUserFileKind.hint,
          strings.gameHint,
          [
            (
              Icons.file_upload_outlined,
              strings.gameFileImport,
              () => _perform(() => _import(GameUserFileKind.hint)),
            ),
            (
              Icons.delete_outline,
              strings.gameFileDelete,
              () => _perform(_removeHint),
            ),
          ],
        ),
    ];
    final labelWidth = rows
        .map((row) => LauncherControl.labelWidth(context, row.$2))
        .reduce(math.max);
    final actionWidth = rows
        .expand((row) => row.$3)
        .map((action) => LauncherControl.iconButtonWidth(context, action.$2))
        .reduce(math.max);
    return Padding(
      padding: widget.padding,
      child: LayoutBuilder(
        builder: (context, constraints) {
          final withLabels =
              constraints.maxWidth >= labelWidth + 2 * actionWidth + 16;
          return Column(
            spacing: 8,
            crossAxisAlignment: CrossAxisAlignment.stretch,
            children: [
              for (final (kind, label, actions) in rows)
                _row(kind, label, actions, withLabels ? actionWidth : null),
              SizedBox(
                height: 2,
                child: _transferring ? const LinearProgressIndicator() : null,
              ),
              if (_feedback case final note?)
                Semantics(liveRegion: true, child: Text(note)),
            ],
          );
        },
      ),
    );
  }

  Widget _row(
    GameUserFileKind kind,
    String label,
    List<_FileAction> actions,
    double? textButtonWidth,
  ) {
    final style = LauncherControl.style(context);
    return Row(
      spacing: 8,
      children: [
        Expanded(child: Text(label, style: LauncherControl.label(context))),
        for (final (index, (icon, text, action)) in actions.indexed)
          SizedBox(
            width: textButtonWidth ?? LauncherControl.height,
            child: textButtonWidth == null
                ? IconButton.outlined(
                    key: ValueKey((kind, index)),
                    tooltip: '$label | $text',
                    style: style,
                    onPressed: _busy ? null : action,
                    icon: Icon(icon),
                  )
                : Semantics(
                    label: label,
                    child: OutlinedButton.icon(
                      key: ValueKey((kind, index)),
                      style: style,
                      onPressed: _busy ? null : action,
                      icon: Icon(icon),
                      label: Text(
                        text,
                        maxLines: 1,
                        overflow: TextOverflow.ellipsis,
                      ),
                    ),
                  ),
          ),
      ],
    );
  }
}
