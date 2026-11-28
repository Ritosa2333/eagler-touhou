import 'package:go_router/go_router.dart';
import 'package:material_ui/material_ui.dart';

import 'account.dart';
import 'account_api.dart';
import 'account_settings_layout.dart';
import 'catalog.dart';
import 'cloud_save_comparison.dart';
import 'cloud_saves.dart';
import 'game_file_archive.dart';
import 'game_file_feedback.dart';
import 'game_preferences.dart';
import 'game_user_files.dart';
import 'l10n/strings.dart';
import 'site_documents.dart';
import 'launcher_select.dart';
import 'launcher_navigation.dart'
    show launcherCompactWidth, LauncherExitGuard, closeLauncherPage;
import 'library.dart';
import 'modal_keyboard_dismiss.dart';
import 'save_progress.dart';

class CloudSavePage extends StatefulWidget {
  const CloudSavePage({
    super.key,
    required this.session,
    required this.games,
    required this.files,
    required this.preferences,
    this.initialGame,
    this.exitGuard,
  });

  final AccountSession session;
  final List<Game> games;
  final GameUserFiles files;
  final GamePreferenceStore preferences;
  final String? initialGame;
  final LauncherExitGuard? exitGuard;

  @override
  State<CloudSavePage> createState() => _CloudSavePageState();
}

class _CloudSavePageState extends State<CloudSavePage> {
  VoidCallback? _unbindExit;
  bool _leaving = false, _confirming = false;
  String? _owner;
  int _view = 0, _readGeneration = 0;
  CloudSaveDirectory? _directory;
  String? _selected;
  bool _loading = false, _localLoading = false, _busy = false;
  bool _localFailed = false, _listFailed = false;
  String? _notice;
  List<CloudFilePayload> _local = const [], _draft = const [];
  SaveStatistics? _localStats;
  GameFileWorkspace? _localWorkspace;
  int? _matchingRevision;

  AccountApi get _api => widget.session.api;
  final _entry = GlobalKey();
  final _refreshButton = FocusNode();

  @override
  void initState() {
    super.initState();
    _unbindExit = widget.exitGuard?.bind(_allowLeave);
    widget.session.addListener(_accountChanged);
    _accountChanged();
  }

  @override
  void dispose() {
    _unbindExit?.call();
    widget.session.removeListener(_accountChanged);
    _refreshButton.dispose();
    super.dispose();
  }

  bool _valid(int view) =>
      mounted &&
      view == _view &&
      widget.session.identity?.username == _owner &&
      _owner != null;

  void _accountChanged() {
    final user = widget.session.identity?.username;
    if (_owner == user) return;
    ++_view;
    setState(() {
      _owner = user;
      _directory = null;
      _draft = const [];
      _clearLocal();
      _busy = _loading = _localLoading = _listFailed = _localFailed = false;
      _selected = null;
      _notice = null;
    });
    if (user != null) _refresh();
  }

  Future<void> _refresh() async {
    final view = _view;
    if (!_valid(view) || _loading) return;
    final restoreFocus = _refreshButton.hasPrimaryFocus;
    setState(() {
      _loading = true;
      _listFailed = false;
    });
    try {
      final directory = await _api.cloudSaveDirectory();
      if (!_valid(view)) return;
      final initial = _directory == null ? widget.initialGame : null;
      setState(() {
        _directory = directory;
        if (_selected != null && !directory.products.containsKey(_selected)) {
          _selected = null;
          _draft = const [];
        }
        if (initial != null && directory.products.containsKey(initial)) {
          _selected = initial;
        }
      });
      await _readLocal();
    } catch (error) {
      if (_valid(view)) {
        widget.session.expireIfUnauthorized(error);
        if (_valid(view)) setState(() => _listFailed = true);
      }
    } finally {
      if (_valid(view)) {
        setState(() => _loading = false);
        if (mounted && restoreFocus) {
          restoreModalTriggerFocus(context, _refreshButton);
        }
      }
    }
  }

  Future<GameFileWorkspace> _workspace(String id) async {
    final game = widget.games.where((game) => game.id == id).firstOrNull;
    if (game == null) throw const GameFileError(GameFileIssue.gameNotInCatalog);
    final settings = await widget.preferences.load(game, multiplayer: false);
    final result = widget.files.workspace(
      game,
      multiplayer: false,
      language: settings.values['language'] as String,
    );
    if (result == null) {
      throw const GameFileError(GameFileIssue.noLocalSaveFolder);
    }
    return result;
  }

  void _clearLocal() {
    _local = const [];
    _localStats = null;
    _localWorkspace = null;
    _matchingRevision = null;
  }

  Future<void> _readLocal() async {
    final request = ++_readGeneration;
    final view = _view;
    final data = _directory;
    final product = data?.products[_selected];
    setState(() {
      _localLoading = product != null;
      _localFailed = false;
      if (_localWorkspace?.game != product?.id) _clearLocal();
    });
    if (data == null || product == null) return;
    bool current() => _valid(view) && _readGeneration == request;
    try {
      final workspace = await _workspace(product.id);
      if (!current()) return;
      setState(() {
        if (_localWorkspace?.language != workspace.language) _clearLocal();
        _localWorkspace = workspace;
      });
      final files = await workspace.cloudLocalFiles(product, data.limits);
      if (!current()) return;
      final snapshot = data.saves[product.id];
      final comparison = await compareCloudFiles(
        files,
        snapshot?.files,
        _localStats == null ? const [] : _local,
      );
      if (!current()) return;
      setState(() {
        if (_localStats != null && !comparison.unchanged) {
          _localStats = null;
        }
        _local = files;
        _matchingRevision = comparison.matchesCloud
            ? snapshot?.record.revision
            : null;
      });
    } on Exception {
      if (current()) {
        setState(() {
          _localFailed = true;
          _matchingRevision = null;
        });
      }
    } finally {
      if (current()) setState(() => _localLoading = false);
    }
  }

  Future<bool> _confirm(String title, String detail, String confirm) async {
    if (_confirming) return false;
    _confirming = true;
    try {
      return await showDialog<bool>(
            context: context,
            animationStyle: LauncherControl.overlayMotion(context),
            builder: (context) => AlertDialog(
              title: Text(title),
              content: Text(detail),
              actions: [
                TextButton(
                  onPressed: () => Navigator.of(context).pop(false),
                  child: Text(context.strings.cancel),
                ),
                FilledButton(
                  onPressed: () => Navigator.of(context).pop(true),
                  child: Text(confirm),
                ),
              ],
            ),
          ) ==
          true;
    } finally {
      _confirming = false;
    }
  }

  Future<bool> _allowDiscard() async =>
      _draft.isEmpty ||
      await _confirm(
        context.strings.cloudDiscardTitle,
        context.strings.cloudDiscardDetail,
        context.strings.cloudDiscard,
      );

  Future<void> _select(String? id, {bool reveal = false}) async {
    final view = _view;
    if (_busy || !await _allowDiscard() || !_valid(view)) return;
    setState(() {
      _selected = id;
      _draft = const [];
      _notice = null;
    });
    if (reveal) {
      WidgetsBinding.instance.addPostFrameCallback((_) {
        if (_entry.currentContext case final BuildContext item when mounted) {
          Scrollable.ensureVisible(item);
        }
      });
    }
    await _readLocal();
  }

  Future<bool> _allowLeave() async =>
      _leaving || !_busy && !_confirming && await _allowDiscard();

  Future<void> _back([String? destination]) async {
    if (_leaving || _busy || _confirming) return;
    if (destination != null && destination != '/account/cloud-saves') {
      await openSiteLink(context, destination);
      return;
    }
    if (!await _allowLeave() || !mounted) return;
    _leaving = true;
    if (destination != null) {
      context.pushReplacement(destination);
      return;
    }
    closeLauncherPage(
      context,
      widget.initialGame == null
          ? '/home'
          : '/game/${widget.initialGame!}/settings',
    );
  }

  String _error(Object error) {
    if (error is GameFileError) return error.message(context.strings);
    if (error is AccountRequestException) {
      return switch (error.code) {
        'SAVE_CONFLICT' => context.strings.cloudConflict,
        'SAVE_BUSY' => context.strings.cloudBusy,
        'SAVE_QUOTA_EXCEEDED' => context.strings.cloudQuota,
        'SAVE_TOO_LARGE' || 'SAVE_PARSE_LIMIT' => context.strings.cloudTooLarge,
        'INVALID_SAVE' ||
        'WRONG_SAVE_GAME' ||
        'INVALID_SAVE_FILE' ||
        'MAIN_SAVE_REQUIRED' => context.strings.cloudInvalid,
        'UNSUPPORTED_SAVE_VERSION' => context.strings.cloudUnsupported,
        _ => context.strings.cloudOperationFailed,
      };
    }
    return context.strings.cloudOperationFailed;
  }

  Future<void> _run(
    Future<void> Function(bool Function()) action, {
    bool mutation = false,
  }) async {
    final view = _view;
    if (_busy || !_valid(view)) return;
    bool active() => _valid(view);
    setState(() {
      _busy = true;
      _notice = null;
    });
    try {
      await action(active);
    } catch (error) {
      if (!active()) return;
      widget.session.expireIfUnauthorized(error);
      if (!active()) return;
      final conflicted =
          error is AccountRequestException && error.status == 409;
      final ambiguous =
          mutation &&
          error is! AccountRequestException &&
          error is! GameFileError;
      setState(
        () => _notice = ambiguous
            ? context.strings.cloudCheckingVersion
            : _error(error),
      );
      if (conflicted || ambiguous) {
        await _refresh();
        if (active() && ambiguous && !_listFailed) {
          setState(() => _notice = context.strings.cloudVersionChecked);
        }
      }
    } finally {
      if (active()) setState(() => _busy = false);
    }
  }

  Future<void> _upload(CloudSaveProduct product) async {
    final view = _view;
    final draft = _draft;
    if (!product.hasRequired(draft.map((file) => file.name))) return;
    if (_directory?.saves.containsKey(product.id) == true &&
        !await _confirm(
          context.strings.cloudReplaceTitle,
          context.strings.cloudReplaceDetail,
          context.strings.cloudUploadParse,
        )) {
      return;
    }
    if (!_valid(view) || _selected != product.id) return;
    await _run((active) async {
      final snapshot = await _api.uploadCloudSave(product, draft);
      if (!active()) return;
      setState(() {
        _directory = _directory!.updated(
          product.withRevision(snapshot.record.revision),
          snapshot,
        );
        _draft = const [];
        _notice = context.strings.cloudUploaded;
      });
      widget.session.cloudSaveRevision.value++;
      await _readLocal();
    }, mutation: true);
  }

  Future<void> _delete(CloudSaveProduct product) async {
    final view = _view;
    if (!await _confirm(
          context.strings.cloudDeleteTitle,
          context.strings.cloudDeleteDetail,
          context.strings.gameFileDelete,
        ) ||
        !_valid(view) ||
        _selected != product.id) {
      return;
    }
    await _run((active) async {
      final revision = await _api.deleteCloudSave(product);
      if (!active()) return;
      setState(() {
        _directory = _directory!.updated(product.withRevision(revision), null);
        _notice = context.strings.cloudDeleted;
      });
      widget.session.cloudSaveRevision.value++;
      await _readLocal();
    }, mutation: true);
  }

  Future<void> _restore(
    CloudSaveProduct product,
    CloudSaveSnapshot snapshot,
  ) async {
    final view = _view;
    final names = snapshot.files.map((file) => file.name);
    if (!await _confirm(
          context.strings.cloudRestoreTitle,
          context.strings.cloudRestoreDetail,
          context.strings.cloudRestoreAction,
        ) ||
        !_valid(view) ||
        _selected != product.id) {
      return;
    }
    await _run((active) async {
      final workspace = await _workspace(product.id);
      if (!active()) return;
      if (!workspace.canRestoreCloudSave(product, names)) {
        throw const GameFileError(GameFileIssue.cloudMainSaveMissing);
      }
      final zip = await _api.downloadCloudSave(snapshot);
      if (!active()) return;
      await workspace.restoreCloudArchive(
        product,
        snapshot,
        zip,
        isActive: active,
      );
      if (!active()) return;
      setState(() => _notice = context.strings.cloudRestored);
      await _readLocal();
    });
  }

  @override
  Widget build(BuildContext context) {
    final data = _directory;
    final compact = MediaQuery.sizeOf(context).width <= launcherCompactWidth;
    final items = [
      if (data != null)
        for (final game in widget.games)
          if (data.products[game.id] case final CloudSaveProduct product)
            (game: game, product: product, snapshot: data.saves[game.id]),
    ];
    final visible = items.where(
      (item) => item.snapshot != null || item.game.id == _selected,
    );
    final add = items.where(
      (item) =>
          item.snapshot == null &&
          item.game.id != _selected &&
          item.product.canUpload,
    );
    return PopScope(
      canPop: _leaving || !_busy && _draft.isEmpty,
      onPopInvokedWithResult: (didPop, _) {
        if (!didPop && !_busy) _back();
      },
      child: AccountSettingsLayout(
        section: AccountSettingsSection.games,
        username: _owner,
        title: context.strings.gameSettings,
        backKey: const ValueKey('cloud-settings-back'),
        onBack: _busy ? null : _back,
        onNavigate: _back,
        busy: _busy,
        progress: _busy || _localLoading || _loading && data != null,
        mobileActions: [if (compact && _owner != null) _refreshAction()],
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.stretch,
          children: [
            Padding(
              padding: const EdgeInsets.only(bottom: 12),
              child: Row(
                children: [
                  Expanded(
                    child: Semantics(
                      header: true,
                      child: Text(
                        context.strings.cloudSaves,
                        style: Theme.of(context).textTheme.titleMedium
                            ?.copyWith(
                              fontSize: 15,
                              fontWeight: FontWeight.w600,
                            ),
                      ),
                    ),
                  ),
                  if (!compact && _owner != null) _refreshAction(),
                ],
              ),
            ),
            if (_owner == null)
              Align(
                alignment: AlignmentDirectional.centerStart,
                child: OutlinedButton(
                  onPressed: () => openSiteLink(
                    context,
                    signInFor('/profile.html#game-settings'),
                  ),
                  child: Text(context.strings.cloudSignIn),
                ),
              )
            else ...[
              if (_listFailed && !_loading)
                Align(
                  alignment: AlignmentDirectional.centerStart,
                  child: TextButton.icon(
                    style: TextButton.styleFrom(minimumSize: const Size(0, 54)),
                    onPressed: _busy || _loading ? null : _refresh,
                    icon: const Icon(Icons.refresh),
                    label: Text(context.strings.cloudReadFailed),
                  ),
                ),
              if (data == null && _loading)
                ConstrainedBox(
                  constraints: const BoxConstraints(minHeight: 54),
                  child: Align(
                    alignment: AlignmentDirectional.centerStart,
                    child: SizedBox.square(
                      dimension: 16,
                      child: CircularProgressIndicator(
                        strokeWidth: 2,
                        semanticsLabel: context.strings.cloudProgressLoading,
                      ),
                    ),
                  ),
                ),
              if (data != null && visible.isEmpty)
                ConstrainedBox(
                  constraints: const BoxConstraints(minHeight: 54),
                  child: Padding(
                    padding: const EdgeInsets.symmetric(vertical: 12),
                    child: Text(
                      context.strings.cloudNoSave,
                      style: Theme.of(context).textTheme.bodyMedium?.copyWith(
                        color: Theme.of(context).colorScheme.onSurfaceVariant,
                      ),
                    ),
                  ),
                ),
              if (visible.isNotEmpty) const Divider(height: 1),
              for (final row in visible)
                Column(
                  key: row.game.id == _selected
                      ? _entry
                      : ValueKey(row.game.id),
                  crossAxisAlignment: CrossAxisAlignment.stretch,
                  children: [
                    Material(
                      color: row.game.id == _selected
                          ? Theme.of(context)
                                .colorScheme
                                .surfaceContainerHighest
                          : Theme.of(context).colorScheme.surface,
                      borderRadius: BorderRadius.circular(6),
                      clipBehavior: Clip.antiAlias,
                      child: ListTile(
                        key: ValueKey('cloud-row-${row.game.id}'),
                        dense: true,
                        minTileHeight: 62,
                        contentPadding: const EdgeInsets.symmetric(
                          horizontal: 8,
                        ),
                        horizontalTitleGap: 12,
                        minLeadingWidth: 36,
                        leading: GameArtwork(
                          game: row.game,
                          size: 36,
                          borderRadius: BorderRadius.circular(5),
                        ),
                        title: Text(
                          row.game.title,
                          style: Theme.of(context).textTheme.bodyMedium
                              ?.copyWith(
                                fontSize: 14,
                                fontWeight: FontWeight.w600,
                              ),
                        ),
                        subtitle: Text(
                          row.snapshot == null
                              ? context.strings.cloudNoSave
                              : cloudSaveDateTime(
                                  context,
                                  row.snapshot!.record.uploadedAt,
                                ),
                          style: Theme.of(context).textTheme.bodySmall
                              ?.copyWith(
                                fontSize: 12,
                                color: Theme.of(context)
                                    .colorScheme
                                    .onSurfaceVariant,
                              ),
                        ),
                        trailing: Icon(
                          row.game.id == _selected
                              ? Icons.expand_less
                              : Icons.expand_more,
                          size: 16,
                        ),
                        onTap: _busy || _loading
                            ? null
                            : () => _select(
                                row.game.id == _selected ? null : row.game.id,
                              ),
                      ),
                    ),
                    if (row.game.id == _selected)
                      Padding(
                        padding: EdgeInsets.fromLTRB(
                          MediaQuery.sizeOf(context).width > 520 ? 56 : 4,
                          10,
                          MediaQuery.sizeOf(context).width > 520 ? 8 : 4,
                          16,
                        ),
                        child: _details(row.product, row.snapshot),
                      ),
                    const Divider(height: 1),
                  ],
                ),
              if (add.isNotEmpty)
                Padding(
                  padding: const EdgeInsets.only(top: 12),
                  child: Align(
                    alignment: AlignmentDirectional.centerStart,
                    child: ConstrainedBox(
                      constraints: BoxConstraints(
                        maxWidth: MediaQuery.sizeOf(context).width > 520
                            ? 280
                            : double.infinity,
                      ),
                      child: LauncherSelect<String>(
                        label: context.strings.cloudAdd,
                        value: '',
                        expand: true,
                        labels: {
                          for (final item in add) item.game.id: item.game.title,
                        },
                        onChanged: _busy || _loading
                            ? null
                            : (value) {
                                if (value != null) _select(value, reveal: true);
                              },
                      ),
                    ),
                  ),
                ),
              if (data != null)
                Padding(
                  padding: const EdgeInsets.only(top: 12),
                  child: ListTile(
                    contentPadding: EdgeInsets.zero,
                    title: Text(context.strings.cloudAutoSync),
                    subtitle: Text(context.strings.cloudAutoSyncPlaceholder),
                    trailing: const Switch(value: false, onChanged: null),
                  ),
                ),
            ],
          ],
        ),
      ),
    );
  }

  Widget _refreshAction() => IconButton(
    focusNode: _refreshButton,
    tooltip: context.strings.cloudRefresh,
    onPressed: _loading || _busy ? null : _refresh,
    constraints: const BoxConstraints.tightFor(width: 44, height: 44),
    padding: EdgeInsets.zero,
    icon: const Icon(Icons.refresh, size: 16),
  );

  Widget _details(CloudSaveProduct product, CloudSaveSnapshot? snapshot) {
    final controls = LauncherControl.style(context);
    final strings = context.strings;
    final locked = _busy || _loading;
    final matches =
        snapshot != null && _matchingRevision == snapshot.record.revision;
    final restoreFits = snapshot == null
        ? null
        : _localWorkspace?.canRestoreCloudSave(
            product,
            snapshot.files.map((file) => file.name),
          );
    final compact = MediaQuery.sizeOf(context).width <= 430;
    final warnings = snapshot?.record.statistics.warnings ?? const <String>[];
    return Column(
      crossAxisAlignment: CrossAxisAlignment.stretch,
      spacing: 16,
      children: [
        if (_localFailed)
          TextButton(
            onPressed: locked ? null : _readLocal,
            child: Text(strings.cloudReadLocalFailed),
          ),
        SaveVersionCards(
          key: ValueKey(product.id),
          local: _local,
          localStatistics: _localStats,
          cloud: snapshot,
          match: matches,
          localLoading: _localLoading && _local.isEmpty,
        ),
        if (warnings.contains('playtime-file-missing'))
          Text(strings.cloudPlaytimeNeedsConfig),
        if (product.id == 'th05' && warnings.contains('clear-unavailable'))
          Text(strings.cloudTh05ClearHint),
        if (warnings.contains('backup-used')) Text(strings.cloudBackupUsed),
        if (restoreFits == false)
          Semantics(
            liveRegion: true,
            child: Text(strings.fileIssueCloudMainSaveMissing),
          ),
        Wrap(
          spacing: 8,
          runSpacing: 4,
          children: [
            if (snapshot != null) ...[
              Tooltip(
                message: strings.cloudDownload,
                child: OutlinedButton.icon(
                  style: controls,
                  onPressed: locked
                      ? null
                      : () => _run((active) async {
                          final workspace = await _workspace(product.id);
                          if (!active()) return;
                          final zip = await _api.downloadCloudSave(snapshot);
                          if (!active()) return;
                          final exported = await workspace.exportCloudArchive(
                            snapshot,
                            zip,
                          );
                          if (active() && exported) {
                            setState(() => _notice = strings.cloudExported);
                          }
                        }),
                  icon: const Icon(Icons.download_outlined),
                  label: Text(
                    compact ? strings.gameDownload : strings.cloudDownload,
                  ),
                ),
              ),
              OutlinedButton(
                style: controls,
                onPressed:
                    locked || _localLoading || matches || restoreFits != true
                    ? null
                    : () => _restore(product, snapshot),
                child: Text(strings.cloudRestore),
              ),
            ],
            if (product.canUpload &&
                _local.isNotEmpty &&
                !matches &&
                _localStats == null &&
                product.hasRequired(_local.map((file) => file.name)))
              TextButton(
                onPressed: locked || _localLoading || _localFailed
                    ? null
                    : () => _run((active) async {
                        final preview = await _api.previewCloudSave(
                          product.id,
                          _local,
                        );
                        if (active()) setState(() => _localStats = preview);
                      }),
                child: Text(strings.cloudPreviewLocal),
              ),
          ],
        ),
        if (product.canUpload) ...[
          const Divider(height: 1),
          Wrap(
            spacing: 8,
            runSpacing: 4,
            children: [
              OutlinedButton(
                style: controls,
                onPressed: locked || _localLoading || _localFailed
                    ? null
                    : () => setState(() {
                        if (product.hasRequired(
                          _local.map((file) => file.name),
                        )) {
                          _draft = _local;
                          _notice = null;
                        } else {
                          _notice = strings.cloudMissingLocalSave;
                        }
                      }),
                child: Text(strings.cloudChooseLocal),
              ),
              OutlinedButton(
                style: controls,
                onPressed: locked
                    ? null
                    : () => _run((active) async {
                        final workspace = await _workspace(product.id);
                        if (!active()) return;
                        final picked = await workspace.pickCloudFiles(
                          product,
                          _directory!.limits,
                        );
                        if (active() && picked.isNotEmpty) {
                          setState(() => _draft = picked);
                        }
                      }),
                child: Text(strings.cloudChooseFiles),
              ),
            ],
          ),
          Text(
            strings.cloudMainFiles(product.requiredFiles.join(' | ')),
            style: Theme.of(context).textTheme.labelSmall,
          ),
          if (_draft.isNotEmpty) ...[
            for (final file in _draft)
              Text('${file.name} | ${saveSize(file.bytes.length)}'),
            Wrap(
              spacing: 8,
              children: [
                FilledButton.icon(
                  style: FilledButton.styleFrom(
                    textStyle: LauncherControl.label(context),
                    minimumSize: const Size(48, LauncherControl.height),
                    shape: LauncherControl.shape,
                  ),
                  onPressed:
                      locked ||
                          !product.hasRequired(_draft.map((file) => file.name))
                      ? null
                      : () => _upload(product),
                  icon: const Icon(Icons.cloud_upload_outlined),
                  label: Text(strings.cloudUploadParse),
                ),
                TextButton(
                  onPressed: locked
                      ? null
                      : () => setState(() => _draft = const []),
                  child: Text(strings.cloudClearChoice),
                ),
              ],
            ),
          ],
        ],
        if (_notice case final String notice)
          Semantics(liveRegion: true, child: Text(notice)),
        if (snapshot != null) ...[
          const Divider(height: 1),
          Align(
            alignment: AlignmentDirectional.centerStart,
            child: TextButton.icon(
              onPressed: locked ? null : () => _delete(product),
              icon: const Icon(Icons.delete_outline),
              label: Text(strings.cloudDelete),
            ),
          ),
        ],
      ],
    );
  }
}
