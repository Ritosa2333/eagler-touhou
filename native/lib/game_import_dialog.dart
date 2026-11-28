import 'package:lucide_icons_flutter/lucide_icons.dart';
import 'package:material_ui/material_ui.dart';

import 'catalog.dart';
import 'game_resources.dart';
import 'l10n/strings.dart';
import 'launcher_select.dart';
import 'modal_keyboard_dismiss.dart';
import 'site_documents.dart';

class GameDataImportDialog extends StatefulWidget {
  const GameDataImportDialog({
    super.key,
    required this.game,
    required this.resources,
    required this.installed,
  });

  final Game game;
  final GameResources resources;
  final bool installed;

  @override
  State<GameDataImportDialog> createState() => _GameDataImportDialogState();
}

class _GameDataImportDialogState extends State<GameDataImportDialog> {
  bool _busy = false;
  DataImportProgress? _progress;
  String? _error;
  Future<GamePackageReference?>? _reference;
  final _referenceButton = FocusNode();
  final _dismissButton = FocusNode();
  final _pickButton = FocusNode();

  @override
  void dispose() {
    _referenceButton.dispose();
    _dismissButton.dispose();
    _pickButton.dispose();
    super.dispose();
  }

  Future<void> _pick() async {
    if (_busy) return;
    setState(() {
      _busy = true;
      _progress = null;
      _error = null;
    });
    try {
      final selection = await widget.resources.pick();
      if (!mounted || selection == null) return;
      await widget.resources.importData(
        widget.game,
        selection,
        progress: (next) {
          if (mounted) setState(() => _progress = next);
        },
      );
      if (mounted) Navigator.of(context).pop(true);
    } on GameDataImportException catch (error) {
      if (!mounted) return;
      final strings = context.strings;
      setState(
        () => _error = switch (error.reason) {
          DataImportError.filename => strings.importPackageIncorrectName(
            widget.game.data!.name,
          ),
          DataImportError.size => strings.importPackageSizeMismatch,
          DataImportError.checksum => strings.importPackageDigestMismatch,
          DataImportError.archive => strings.importPackageUnreadable,
          DataImportError.wrongGame => strings.importPackageWrongGame,
        },
      );
    } on Exception {
      if (mounted) {
        setState(() => _error = context.strings.importPackageIoError);
      }
    } finally {
      if (mounted) {
        setState(() {
          _busy = false;
          _progress = null;
        });
        restoreModalTriggerFocus(context, _pickButton);
      }
    }
  }

  void _backToImport() {
    setState(() => _reference = null);
    WidgetsBinding.instance.addPostFrameCallback((_) {
      if (mounted && _reference == null) _referenceButton.requestFocus();
    });
  }

  void _openReference() {
    final reference = Future.sync(widget.resources.reference);
    // The builder subscribes on the next frame.
    reference.ignore();
    setState(() {
      _reference = reference;
    });
    _dismissButton.requestFocus();
  }

  Widget _referenceBody(Future<GamePackageReference?> future) =>
      FutureBuilder<GamePackageReference?>(
        future: future,
        builder: (context, snapshot) {
          final strings = context.strings;
          if (snapshot.connectionState == ConnectionState.waiting) {
            return LinearProgressIndicator(
              semanticsLabel: strings.importPackageReferenceLoading,
            );
          }
          if (snapshot.hasError) {
            return Column(
              crossAxisAlignment: CrossAxisAlignment.start,
              mainAxisSize: MainAxisSize.min,
              spacing: 8,
              children: [
                Semantics(
                  liveRegion: true,
                  child: Text(strings.importPackageReferenceFailed),
                ),
                OutlinedButton(
                  style: LauncherControl.style(context),
                  onPressed: _openReference,
                  child: Text(strings.importPackageRetryReference),
                ),
              ],
            );
          }
          final link = snapshot.data;
          if (link == null) return Text(strings.importPackageNoReference);
          return Column(
            crossAxisAlignment: CrossAxisAlignment.start,
            mainAxisSize: MainAxisSize.min,
            spacing: 8,
            children: [
              Text(strings.importPackageAddress),
              Align(
                alignment: AlignmentDirectional.centerStart,
                child: TextButton(
                  style: TextButton.styleFrom(
                    padding: const EdgeInsets.symmetric(vertical: 6),
                    alignment: AlignmentDirectional.centerStart,
                  ),
                  onPressed: () =>
                      openSiteLink(context, link.address.toString()),
                  child: Text(link.address.toString()),
                ),
              ),
              if (link.hint case final String hint when hint.isNotEmpty) ...[
                Text(strings.importPackageHint),
                SelectableText(hint),
              ],
            ],
          );
        },
      );

  @override
  Widget build(BuildContext context) {
    final theme = Theme.of(context);
    final strings = context.strings;
    final data = widget.game.data!;
    final progress = _progress;
    final reference = _reference;
    final message =
        _error ??
        switch (progress?.phase) {
          DataImportPhase.copying => strings.importPackageCopying,
          DataImportPhase.extracting => strings.importPackageExtracting,
          DataImportPhase.verifying => strings.importPackageVerifying,
          null when widget.installed => strings.importPackageInstalled,
          _ => null,
        };
    return PopScope(
      canPop: !_busy && reference == null,
      onPopInvokedWithResult: (didPop, _) {
        if (!didPop && reference != null) _backToImport();
      },
      child: ModalKeyboardDismiss(
        child: AlertDialog(
          insetPadding: const EdgeInsets.symmetric(
            horizontal: 20,
            vertical: 20,
          ),
          shape: RoundedRectangleBorder(
            borderRadius: BorderRadius.circular(12),
          ),
          titlePadding: const EdgeInsetsDirectional.fromSTEB(16, 8, 8, 0),
          contentPadding: const EdgeInsets.fromLTRB(16, 8, 16, 16),
          scrollable: true,
          title: Row(
            spacing: 4,
            children: [
              Expanded(
                child: Text(
                  reference == null
                      ? strings.importPackageTitle
                      : strings.importPackageDownload,
                  style: theme.textTheme.titleMedium,
                ),
              ),
              IconButton(
                autofocus: true,
                focusNode: _dismissButton,
                tooltip: reference == null
                    ? strings.close
                    : MaterialLocalizations.of(context).backButtonTooltip,
                onPressed: _busy
                    ? null
                    : reference == null
                    ? () => Navigator.of(context).pop(false)
                    : _backToImport,
                icon: Icon(
                  reference == null ? LucideIcons.x : LucideIcons.arrowLeft,
                  size: LauncherControl.iconSize,
                ),
              ),
            ],
          ),
          content: SizedBox(
            width: 344,
            child: reference != null
                ? _referenceBody(reference)
                : Column(
                    mainAxisSize: MainAxisSize.min,
                    crossAxisAlignment: CrossAxisAlignment.stretch,
                    spacing: 10,
                    children: [
                      Text(strings.importPackageHelp),
                      Text(
                        '${data.name}  |  ${(data.bytes / (1024 * 1024)).toStringAsFixed(1)} MiB',
                        style: theme.textTheme.bodySmall?.copyWith(
                          color: theme.colorScheme.onSurfaceVariant,
                        ),
                      ),
                      if (message != null)
                        Semantics(liveRegion: true, child: Text(message)),
                      if (progress != null)
                        LinearProgressIndicator(
                          value: progress.percent == null
                              ? null
                              : progress.percent! / 100,
                        ),
                      const SizedBox(height: 2),
                      IntrinsicHeight(
                        child: Row(
                          crossAxisAlignment: CrossAxisAlignment.stretch,
                          spacing: 8,
                          children: [
                            Expanded(
                              child: OutlinedButton(
                                style: LauncherControl.style(context),
                                focusNode: _referenceButton,
                                onPressed: _busy ? null : _openReference,
                                child: Text(strings.importPackageOpenReference),
                              ),
                            ),
                            Expanded(
                              child: FilledButton(
                                focusNode: _pickButton,
                                style: FilledButton.styleFrom(
                                  shape: LauncherControl.shape,
                                  minimumSize: const Size(
                                    0,
                                    LauncherControl.height,
                                  ),
                                  padding: const EdgeInsets.symmetric(
                                    horizontal: 8,
                                  ),
                                  textStyle: LauncherControl.label(context),
                                ),
                                onPressed: _busy ? null : _pick,
                                child: Text(strings.importPackageChoose),
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
  }
}
