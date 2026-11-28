import 'dart:io';

import 'package:material_ui/material_ui.dart';

import 'game_file_archive.dart';
import 'game_file_feedback.dart';
import 'game_launcher.dart';
import 'game_user_files.dart';
import 'l10n/strings.dart';
import 'launcher_select.dart';
import 'modal_keyboard_dismiss.dart';

Future<void> showReplayUnavailable(BuildContext context, {String? name}) =>
    showDialog<void>(
      context: context,
      animationStyle: LauncherControl.overlayMotion(context),
      builder: (dialog) => AlertDialog(
        title: Text(
          dialog.strings.gameReplayUnavailable,
          style: Theme.of(dialog).textTheme.titleMedium,
        ),
        content: name == null ? null : Text(name),
        actions: [
          TextButton(
            onPressed: () => Navigator.of(dialog).pop(),
            child: Text(dialog.strings.gameReturnToReplays),
          ),
        ],
      ),
    );

class ReplayManagerDialog extends StatefulWidget {
  const ReplayManagerDialog({
    super.key,
    required this.workspace,
    required this.play,
  });

  final GameFileWorkspace workspace;
  final Future<GameLaunchStatus?> Function(BuildContext, File) play;

  @override
  State<ReplayManagerDialog> createState() => _ReplayManagerDialogState();
}

class _ReplayManagerDialogState extends State<ReplayManagerDialog> {
  late Future<List<ReplayEntry>> _rows = widget.workspace.replays();
  final _folderBack = FocusNode();
  final _folderLink = FocusNode();
  final _importButton = FocusNode();
  String? _folder;
  String? _returnFolder;
  bool _busy = false;
  String? _feedback;

  @override
  void dispose() {
    _folderBack.dispose();
    _folderLink.dispose();
    _importButton.dispose();
    super.dispose();
  }

  Future<void> _run(
    Future<void> Function() action, {
    String? success,
    bool refresh = true,
  }) async {
    if (_busy) return;
    setState(() {
      _busy = true;
      _feedback = null;
    });
    try {
      await action();
      if (!mounted) return;
      setState(() {
        _feedback = success;
        if (refresh) _rows = widget.workspace.replays();
      });
    } on GameFileError catch (error) {
      if (mounted) setState(() => _feedback = error.message(context.strings));
    } on Exception {
      if (mounted) setState(() => _feedback = context.strings.replayFileFailed);
    } finally {
      if (mounted) setState(() => _busy = false);
    }
  }

  Future<void> _import() async {
    if (_busy) return;
    final restore = _importButton.hasFocus;
    await _run(() async {
      final file = await widget.workspace.pickReplayForViewing();
      if (!mounted || file == null) return;
      if (widget.workspace.supportsMatchArchives &&
          isTh09MatchArchive(file.name)) {
        await widget.workspace.withImportedMatchReplay(file, _play);
        return;
      }
      await widget.workspace.importFile(GameUserFileKind.replay, file);
      if (mounted) {
        setState(() {
          _rows = widget.workspace.replays();
        });
      }
    }, refresh: false);
    if (mounted && restore) restoreModalTriggerFocus(context, _importButton);
  }

  Future<void> _play(File file) async {
    if (!mounted) return;
    final status = await widget.play(context, file);
    if (mounted && status == GameLaunchStatus.nativeRuntimeUnavailable) {
      await showReplayUnavailable(context, name: file.uri.pathSegments.last);
    }
  }

  Future<void> _remove(ReplayEntry entry) async {
    final confirm = await showDialog<bool>(
      context: context,
      animationStyle: LauncherControl.overlayMotion(context),
      builder: (context) => AlertDialog(
        title: Text(context.strings.replayDeleteTitle),
        content: Text(gameFileName(entry.path)),
        actions: [
          TextButton(
            onPressed: () => Navigator.of(context).pop(false),
            child: Text(context.strings.cancel),
          ),
          FilledButton(
            onPressed: () => Navigator.of(context).pop(true),
            child: Text(context.strings.gameFileDelete),
          ),
        ],
      ),
    );
    if (confirm == true && mounted) {
      await _run(() => widget.workspace.deleteReplay(entry.path));
    }
  }

  Future<void> _rename(ReplayEntry entry) async {
    final name = await _askName(
      title: context.strings.replayRenameTitle,
      label: context.strings.replayFileName,
      initial: gameFileName(entry.path),
    );
    if (name != null && mounted) {
      await _run(() => widget.workspace.renameReplay(entry.path, name));
    }
  }

  Future<String?> _askName({
    required String title,
    required String label,
    String initial = '',
  }) {
    var value = initial;
    return showDialog<String>(
      context: context,
      animationStyle: LauncherControl.overlayMotion(context),
      builder: (context) => AlertDialog(
        title: Text(title),
        content: TextFormField(
          initialValue: initial,
          autofocus: true,
          decoration: InputDecoration(labelText: label),
          onChanged: (next) => value = next,
          onFieldSubmitted: (_) => Navigator.of(context).pop(value),
        ),
        actions: [
          TextButton(
            onPressed: () => Navigator.of(context).pop(),
            child: Text(context.strings.cancel),
          ),
          FilledButton(
            onPressed: () => Navigator.of(context).pop(value),
            child: Text(context.strings.replayConfirm),
          ),
        ],
      ),
    );
  }

  Future<void> _move(ReplayEntry entry, Iterable<String> folders) async {
    var destination = 'replay';
    const create = 'new';
    final result = await showDialog<String>(
      context: context,
      animationStyle: LauncherControl.overlayMotion(context),
      builder: (context) => StatefulBuilder(
        builder: (context, update) => AlertDialog(
          title: Text(context.strings.replayMoveTitle),
          content: DropdownButton<String>(
            value: destination,
            isExpanded: true,
            items: [
              DropdownMenuItem(
                value: 'replay',
                child: Text(context.strings.replayRoot),
              ),
              for (final folder in folders)
                DropdownMenuItem(
                  value: folder,
                  child: Text(
                    gameFileName(folder),
                    overflow: TextOverflow.ellipsis,
                  ),
                ),
              DropdownMenuItem(
                value: create,
                child: Text('${context.strings.replayNewFolder}…'),
              ),
            ],
            onChanged: (next) => update(() => destination = next!),
          ),
          actions: [
            TextButton(
              onPressed: () => Navigator.of(context).pop(),
              child: Text(context.strings.cancel),
            ),
            FilledButton(
              onPressed: () => Navigator.of(context).pop(destination),
              child: Text(context.strings.replayMove),
            ),
          ],
        ),
      ),
    );
    if (!mounted || result == null) return;
    String? target = result;
    if (target == create) {
      final name = await _askName(
        title: context.strings.replayNewFolder,
        label: context.strings.replayFolderName,
      );
      target = name == null ? null : 'replay/${name.trim()}';
    }
    if (target != null && mounted) {
      await _run(() => widget.workspace.moveReplay(entry.path, target!));
    }
  }

  void _close() {
    if (_busy) return;
    if (_folder != null) {
      setState(() => _folder = null);
      WidgetsBinding.instance.addPostFrameCallback((_) {
        if (mounted && _folder == null && _folderLink.context != null) {
          _folderLink.requestFocus();
        }
      });
    } else {
      Navigator.of(context).pop();
    }
  }

  void _openFolder(String path) {
    setState(() {
      _folder = path;
      _returnFolder = path;
    });
    WidgetsBinding.instance.addPostFrameCallback((_) {
      if (mounted && _folder == path) _folderBack.requestFocus();
    });
  }

  @override
  Widget build(BuildContext context) => PopScope(
    canPop: !_busy && _folder == null,
    onPopInvokedWithResult: (didPop, _) {
      if (!didPop) _close();
    },
    child: ModalKeyboardDismiss(
      child: Dialog(
        insetPadding: const EdgeInsets.symmetric(horizontal: 12, vertical: 18),
        child: SizedBox(
          width: 640,
          height: 560,
          child: Column(
            crossAxisAlignment: CrossAxisAlignment.stretch,
            children: [
              Padding(
                padding: const EdgeInsets.fromLTRB(16, 8, 8, 8),
                child: Row(
                  children: [
                    if (_folder != null)
                      IconButton(
                        focusNode: _folderBack,
                        tooltip: MaterialLocalizations.of(context)
                            .backButtonTooltip,
                        onPressed: _busy ? null : _close,
                        icon: const BackButtonIcon(),
                      ),
                    Expanded(
                      child: Text(
                        _folder == null
                            ? context.strings.replayManager
                            : gameFileName(_folder!),
                        overflow: TextOverflow.ellipsis,
                        style: Theme.of(context).textTheme.titleLarge,
                      ),
                    ),
                    IconButton(
                      autofocus: true,
                      tooltip: context.strings.close,
                      onPressed: _busy
                          ? null
                          : () => Navigator.of(context).pop(),
                      icon: const Icon(Icons.close),
                    ),
                  ],
                ),
              ),
              if (_busy && ModalRoute.isCurrentOf(context) != false)
                const LinearProgressIndicator(minHeight: 2)
              else
                const Divider(height: 2),
              if (_feedback case final feedback?)
                Padding(
                  padding: const EdgeInsets.all(12),
                  child: Semantics(liveRegion: true, child: Text(feedback)),
                ),
              Expanded(
                child: FutureBuilder<List<ReplayEntry>>(
                  future: _rows,
                  builder: (context, snapshot) {
                    if (snapshot.hasError) {
                      return Center(
                        child: OutlinedButton.icon(
                          icon: const Icon(Icons.refresh),
                          label: Text(context.strings.replayRead),
                          onPressed: () => setState(() {
                            _rows = widget.workspace.replays();
                          }),
                        ),
                      );
                    }
                    final rows = snapshot.data;
                    if (rows == null) {
                      return const Center(child: CircularProgressIndicator());
                    }
                    return _list(rows);
                  },
                ),
              ),
              Padding(
                padding: const EdgeInsets.fromLTRB(12, 4, 12, 12),
                child: Row(
                  mainAxisAlignment: MainAxisAlignment.end,
                  spacing: 8,
                  children: [
                    if (_folder != null)
                      IconButton(
                        tooltip: context.strings.replayDownloadFolder,
                        icon: const Icon(Icons.download),
                        onPressed: _busy
                            ? null
                            : () => _run(() async {
                                await widget.workspace.exportReplays(
                                  folder: _folder,
                                );
                              }, refresh: false),
                      ),
                    Flexible(
                      child: FilledButton.tonalIcon(
                        focusNode: _importButton,
                        onPressed: _busy ? null : _import,
                        icon: const Icon(Icons.file_upload_outlined),
                        label: Text(context.strings.replayImport),
                      ),
                    ),
                  ],
                ),
              ),
            ],
          ),
        ),
      ),
    ),
  );

  Widget _list(List<ReplayEntry> rows) {
    final folders = <String, int>{};
    final visible = <ReplayEntry>[];
    for (final row in rows) {
      final folder = row.path.substring(0, row.path.lastIndexOf('/'));
      if (folder != 'replay') {
        folders.update(folder, (count) => count + 1, ifAbsent: () => 1);
      }
      if (folder == (_folder ?? 'replay')) visible.add(row);
    }
    if (visible.isEmpty && (_folder != null || folders.isEmpty)) {
      return Center(child: Text(context.strings.gameNoReplay));
    }
    return ListView(
      key: PageStorageKey(_folder ?? 'replay'),
      padding: const EdgeInsets.symmetric(vertical: 8),
      children: [
        if (_folder == null)
          for (final folder in folders.entries)
            ListTile(
              focusNode: _returnFolder == folder.key ? _folderLink : null,
              leading: const Icon(Icons.folder_outlined),
              title: Text(
                gameFileName(folder.key),
                overflow: TextOverflow.ellipsis,
              ),
              subtitle: Text(context.strings.replayFolderCount(folder.value)),
              trailing: const Icon(Icons.chevron_right),
              onTap: _busy ? null : () => _openFolder(folder.key),
            ),
        for (final row in visible)
          ListTile(
            title: Text(
              gameFileName(row.path),
              maxLines: 1,
              overflow: TextOverflow.ellipsis,
            ),
            subtitle: _subtitle(row),
            trailing: PopupMenuButton<String>(
              tooltip: context.strings.replayActions,
              enabled: !_busy,
              onSelected: (action) {
                switch (action) {
                  case 'watch':
                    _run(() async {
                      await _play(await widget.workspace.replayFile(row.path));
                    }, refresh: false);
                  case 'download':
                    _run(() async {
                      await widget.workspace.exportReplay(row.path);
                    }, refresh: false);
                  case 'rename':
                    _rename(row);
                  case 'move':
                    _move(row, folders.keys);
                  case 'delete':
                    _remove(row);
                }
              },
              itemBuilder: (_) => [
                if (widget.workspace.supportsFolders)
                  PopupMenuItem(
                    value: 'watch',
                    child: Text(context.strings.replayPlay),
                  ),
                PopupMenuItem(
                  value: 'download',
                  child: Text(context.strings.gameDownload),
                ),
                PopupMenuItem(
                  value: 'rename',
                  child: Text(context.strings.replayRename),
                ),
                if (widget.workspace.supportsFolders)
                  PopupMenuItem(
                    value: 'move',
                    child: Text(context.strings.replayMove),
                  ),
                PopupMenuItem(
                  value: 'delete',
                  child: Text(context.strings.gameFileDelete),
                ),
              ],
            ),
          ),
      ],
    );
  }

  Widget _subtitle(ReplayEntry row) {
    final size = row.size < 1024
        ? '${row.size} B'
        : '${(row.size / 1024).toStringAsFixed(1)} KiB';
    final match = row.match;
    if (match == null) return Text(size);
    final strings = context.strings;
    final date = MaterialLocalizations.of(context)
        .formatCompactDate(match.startedAt.toLocal());
    return Padding(
      padding: const EdgeInsets.only(top: 2),
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.start,
        mainAxisSize: MainAxisSize.min,
        spacing: 2,
        children: [
          Text(
            match.names.join(' vs '),
            maxLines: 2,
            overflow: TextOverflow.ellipsis,
          ),
          Text(
            strings.th09ReplayWinner(match.names[match.winner]),
            maxLines: 1,
            overflow: TextOverflow.ellipsis,
          ),
          Text('$date | ${strings.formatPlaytime(match.frames ~/ 60)} | $size'),
        ],
      ),
    );
  }
}
