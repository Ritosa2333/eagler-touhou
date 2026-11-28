import 'dart:async';

import 'package:flutter/foundation.dart';
import 'package:go_router/go_router.dart';
import 'package:material_ui/material_ui.dart';
import 'package:lucide_icons_flutter/lucide_icons.dart';

import 'account.dart';
import 'account_api.dart';
import 'account_fields.dart';
import 'account_settings_layout.dart';
import 'l10n/strings.dart';
import 'launcher_select.dart';
import 'launcher_navigation.dart';
import 'profile_crop.dart';
import 'profile_data.dart';
import 'profile_draft.dart';
import 'profile_identity.dart';
import 'profile_image.dart';
import 'profile_loading.dart';
import 'profile_social.dart';

class ProfileEditorPage extends StatelessWidget {
  const ProfileEditorPage({
    super.key,
    required this.session,
    required this.username,
    this.exitGuard,
  });

  final AccountSession session;
  final String username;
  final LauncherExitGuard? exitGuard;

  @override
  Widget build(BuildContext context) => ListenableBuilder(
    listenable: session,
    builder: (context, _) {
      final member = session.identity?.username.toLowerCase();
      if (member == username.toLowerCase()) {
        return _ProfileEditor(
          key: ValueKey(member),
          session: session,
          username: member!,
          exitGuard: exitGuard,
        );
      }
      return AccountSettingsLayout(
        section: AccountSettingsSection.profile,
        username: member,
        title: context.strings.profileEditorTitle,
        onBack: () => profileBack(context, profilePath(username)),
        child: Align(
          alignment: AlignmentDirectional.centerStart,
          child: member == null
              ? FilledButton(
                  onPressed: () => context.push('/login'),
                  child: Text(context.strings.profileEditorSignIn),
                )
              : Text(context.strings.profileEditorOwner),
        ),
      );
    },
  );
}

class _ProfileEditor extends StatefulWidget {
  const _ProfileEditor({
    super.key,
    required this.session,
    required this.username,
    this.exitGuard,
  });
  final AccountSession session;
  final String username;
  final LauncherExitGuard? exitGuard;

  @override
  State<_ProfileEditor> createState() => _ProfileEditorState();
}

class _ProfileEditorState extends State<_ProfileEditor> {
  VoidCallback? _unbindExit;
  final _form = GlobalKey<FormState>();
  final _name = TextEditingController();
  final _bio = TextEditingController();
  final _links = <_LinkEditor>[];
  late final StreamSubscription<ProfileChange> _profileChanges;
  ProfileDraft? _initial;
  late ProfileDisplay _display;
  String _avatar = '';
  String _cover = '';
  Uint8List? _avatarPreview;
  Uint8List? _coverPreview;
  String? _thumbnail;
  String? _issue;
  bool _loading = true;
  bool _busy = false;
  bool _confirming = false;
  bool _leaving = false;
  bool _saved = false;

  bool get _dirty {
    final baseline = _initial;
    if (baseline == null) return false;
    return _name.text != baseline.name ||
        _bio.text != baseline.bio ||
        _avatar != baseline.avatar ||
        _cover != baseline.cover ||
        !listEquals(_links.map((row) => row.value).toList(), baseline.links) ||
        !_display.equivalentTo(baseline.display);
  }

  @override
  void initState() {
    super.initState();
    _unbindExit = widget.exitGuard?.bind(_allowLeave);
    _profileChanges = widget.session.api.profileChanges.listen(_refreshReview);
    _load();
  }

  Future<void> _refreshReview(ProfileChange change) async {
    final baseline = _initial;
    if (change.username != widget.username ||
        !change.reviewFinished ||
        baseline == null ||
        !baseline.review.values.contains(ProfileReview.pending)) {
      return;
    }
    try {
      final latest = await widget.session.api.myProfileDraft();
      if (!mounted || !identical(_initial, baseline)) return;
      setState(() {
        _initial = baseline.saved(
          latest.review,
          avatarReady: latest.roomAvatarReady,
        );
      });
    } catch (_) {
      // A failed status refresh leaves the editor's local draft available.
    }
  }

  Future<void> _load() async {
    setState(() {
      _loading = true;
      _issue = null;
    });
    try {
      final draft = await widget.session.api.myProfileDraft();
      if (!mounted) return;
      setState(() {
        _initial = draft;
        _name.text = draft.name;
        _bio.text = draft.bio;
        _avatar = draft.avatar;
        _cover = draft.cover;
        _avatarPreview = profileImageBytes(_avatar);
        _coverPreview = profileImageBytes(_cover);
        _display = draft.display;
        _links.addAll(draft.links.map(_LinkEditor.new));
      });
    } catch (error) {
      if (!mounted) return;
      widget.session.expireIfUnauthorized(error);
      setState(() => _issue = context.strings.profileEditorLoad);
    } finally {
      if (mounted) setState(() => _loading = false);
    }
  }

  void _changed([String? _]) => setState(() {
    _saved = false;
    _issue = null;
  });

  void _removeLink(_LinkEditor row) {
    _links.remove(row);
    _changed();
    WidgetsBinding.instance.addPostFrameCallback((_) => row.dispose());
  }

  void _addLink() {
    if (_links.length >= 12) return;
    _links.add(_LinkEditor((label: '', url: '')));
    _changed();
  }

  void _showView(ProfileGames view, bool visible) {
    _display = _display.show(view, visible);
    _changed();
  }

  Future<void> _pick(ProfileImageKind kind) async {
    if (_busy) return;
    FocusScope.of(context).unfocus();
    setState(() {
      _busy = true;
      _issue = null;
    });
    try {
      final source = await pickProfileImage();
      if (!mounted || source == null) return;
      final result = await cropProfileImage(context, source, kind);
      if (!mounted || result == null) return;
      if (kind == ProfileImageKind.avatar) {
        _avatar = result.data;
        _avatarPreview = profileImageBytes(_avatar);
        _thumbnail = result.thumbnail;
      } else {
        _cover = result.data;
        _coverPreview = profileImageBytes(_cover);
      }
      _changed();
    } catch (error) {
      if (mounted) {
        setState(
          () => _issue = error is ProfileImageException
              ? switch (error.message) {
                  '图片不能超过 20 MiB' => context.strings.profileEditorImageSize,
                  '请选择 PNG、JPEG 或 WebP 图片' =>
                    context.strings.profileEditorImageFormat,
                  '图片像素过多，请先缩小图片' => context.strings.profileEditorImagePixels,
                  '裁剪范围无效' => context.strings.profileEditorImageCrop,
                  '图片压缩后仍超出大小限制，请选另一张' =>
                    context.strings.profileEditorImageCompress,
                  _ => context.strings.profileEditorImageRead,
                }
              : context.strings.profileEditorImageFailed,
        );
      }
    } finally {
      if (mounted) setState(() => _busy = false);
    }
  }

  void _removeImage(ProfileImageKind kind) {
    if (kind == ProfileImageKind.avatar) {
      _avatar = '';
      _avatarPreview = null;
      _thumbnail = null;
    } else {
      _cover = '';
      _coverPreview = null;
    }
    _changed();
  }

  Future<bool> _allowLeave() async {
    if (_leaving) return true;
    if (_busy || _confirming) return false;
    if (_dirty) {
      setState(() => _confirming = true);
      final discard = await showDialog<bool>(
        context: context,
        builder: (dialog) => AlertDialog(
          title: Text(dialog.strings.profileEditorDiscardTitle),
          actions: [
            TextButton(
              onPressed: () => Navigator.of(dialog).pop(false),
              child: Text(dialog.strings.profileEditorContinue),
            ),
            FilledButton(
              onPressed: () => Navigator.of(dialog).pop(true),
              child: Text(dialog.strings.profileEditorDiscard),
            ),
          ],
        ),
      );
      if (!mounted) return false;
      setState(() => _confirming = false);
      if (discard != true) return false;
    }
    return true;
  }

  Future<void> _back([String? destination]) async {
    if (_leaving || !await _allowLeave() || !mounted) return;
    _leaving = true;
    if (destination != null) {
      context.pushReplacement(destination);
    } else {
      profileBack(context, profilePath(widget.username));
    }
  }

  Future<void> _save() async {
    final current = _initial;
    if (current == null ||
        _busy ||
        !_dirty ||
        _form.currentState?.validate() != true) {
      return;
    }
    FocusScope.of(context).unfocus();
    final submitted = ProfileDraft(
      name: _name.text.trim(),
      bio: _bio.text,
      avatar: _avatar,
      cover: _cover,
      links: [for (final row in _links) row.trimmed],
      display: _display,
      review: current.review,
      roomAvatarReady: current.roomAvatarReady,
    );
    setState(() {
      _busy = true;
      _issue = null;
      _saved = false;
    });
    try {
      final selectedThumbnail = _thumbnail;
      var thumbnail = selectedThumbnail;
      if (thumbnail == null &&
          _avatar.isNotEmpty &&
          _avatar == current.avatar &&
          !current.roomAvatarReady &&
          _avatarPreview != null) {
        try {
          thumbnail = await encodeStoredProfileThumbnail(_avatarPreview!);
        } on ProfileImageException {
          // Older profiles can still save text when their stored avatar cannot be decoded.
        }
      }
      Map<String, ProfileReview> review;
      try {
        review = await widget.session.api.saveProfile(
          submitted,
          avatarThumbnail: thumbnail,
        );
      } on AccountRequestException catch (error) {
        if (selectedThumbnail != null ||
            thumbnail == null ||
            error.code != 'SAVE_QUOTA_EXCEEDED') {
          rethrow;
        }
        thumbnail = null;
        review = await widget.session.api.saveProfile(submitted);
      }
      if (!mounted) return;
      final ready =
          _avatar.isNotEmpty &&
          (thumbnail != null ||
              _avatar == current.avatar && current.roomAvatarReady);
      setState(() {
        _initial = submitted.saved(review, avatarReady: ready);
        _name.text = submitted.name;
        for (final row in _links) {
          row.label.text = row.label.text.trim();
          row.url.text = row.url.text.trim();
        }
        _thumbnail = null;
        _saved = true;
      });
      widget.session.profileSaved(
        widget.username,
        pending: review.values.contains(ProfileReview.pending),
      );
    } catch (error) {
      if (!mounted) return;
      widget.session.expireIfUnauthorized(error);
      final code = error is AccountRequestException ? error.code : null;
      setState(() {
        _issue = switch (code) {
          'CONTENT_REJECTED' => context.strings.profileEditorRejected,
          'REVIEW_UNAVAILABLE' =>
            context.strings.profileEditorReviewUnavailable,
          'SAVE_QUOTA_EXCEEDED' => context.strings.profileEditorQuota,
          'INVALID_PROFILE_IMAGE' => context.strings.profileEditorInvalidImage,
          _ => context.strings.profileEditorSaveFailed,
        };
      });
    } finally {
      if (mounted) setState(() => _busy = false);
    }
  }

  String? _review(String field) => switch (_initial?.review[field]) {
    ProfileReview.pending => context.strings.profileEditorPending,
    ProfileReview.rejected => context.strings.profileEditorNotApproved,
    null => null,
  };

  @override
  void dispose() {
    _unbindExit?.call();
    _profileChanges.cancel();
    _name.dispose();
    _bio.dispose();
    for (final row in _links) {
      row.dispose();
    }
    super.dispose();
  }

  @override
  Widget build(BuildContext context) => PopScope(
    canPop: _leaving || !_dirty && !_busy,
    onPopInvokedWithResult: (didPop, _) {
      if (!didPop) _back();
    },
    child: AccountSettingsLayout(
      section: AccountSettingsSection.profile,
      username: widget.username,
      title: context.strings.profileEditorTitle,
      backKey: const ValueKey('profile-editor-back'),
      onBack: _busy ? null : _back,
      onNavigate: _back,
      busy: _busy,
      progress: _busy,
      footer: _initial == null ? null : _actions(),
      child: _initial == null ? _placeholder() : _editor(),
    ),
  );

  Widget _placeholder() => _loading
      ? ProfilePanelLoading(label: context.strings.profileLoading)
      : Align(
          alignment: AlignmentDirectional.centerStart,
          child: Semantics(
            liveRegion: true,
            child: TextButton.icon(
              style: TextButton.styleFrom(minimumSize: const Size(0, 54)),
              icon: const Icon(Icons.refresh),
              label: Text(_issue ?? context.strings.profileEditorReload),
              onPressed: _load,
            ),
          ),
        );

  Widget _editor() {
    final columns = MediaQuery.sizeOf(context).width > 1000;
    return Form(
      key: _form,
      child: Flex(
        direction: columns ? Axis.horizontal : Axis.vertical,
        crossAxisAlignment: CrossAxisAlignment.start,
        spacing: columns ? 28 : 24,
        children: [
          Flexible(
            flex: columns ? 17 : 0,
            fit: columns ? FlexFit.tight : FlexFit.loose,
            child: _artwork(),
          ),
          Flexible(
            flex: columns ? 28 : 0,
            fit: columns ? FlexFit.tight : FlexFit.loose,
            child: _fields(),
          ),
        ],
      ),
    );
  }

  Widget _actions() {
    final rejected = _initial!.review.values.contains(ProfileReview.rejected);
    final pending = _initial!.review.values.contains(ProfileReview.pending);
    final message =
        _issue ??
        (rejected
            ? context.strings.profileEditorSomeRejected
            : pending
            ? context.strings.profileEditorSubmitted
            : _saved
            ? context.strings.profileEditorSaved
            : null);
    final theme = Theme.of(context);
    return Material(
      color: theme.colorScheme.surface,
      shape: Border(top: BorderSide(color: theme.colorScheme.outlineVariant)),
      child: SafeArea(
        top: false,
        minimum: EdgeInsets.fromLTRB(
          MediaQuery.sizeOf(context).width > 780 ? 0 : 20,
          8,
          MediaQuery.sizeOf(context).width > 780 ? 0 : 20,
          8,
        ),
        child: Align(
          alignment: Alignment.center,
          heightFactor: 1,
          child: ConstrainedBox(
            constraints: const BoxConstraints(maxWidth: 800),
            child: Row(
              spacing: 8,
              children: [
                Expanded(
                  child: message == null
                      ? const SizedBox.shrink()
                      : Semantics(liveRegion: true, child: Text(message)),
                ),
                FilledButton.icon(
                  style: FilledButton.styleFrom(
                    minimumSize: const Size(104, LauncherControl.height),
                  ),
                  onPressed: !_busy && _dirty ? _save : null,
                  icon: _busy
                      ? SizedBox.square(
                          dimension: 16,
                          child: CircularProgressIndicator(
                            strokeWidth: 2,
                            color: theme.colorScheme.onSurfaceVariant,
                          ),
                        )
                      : const Icon(Icons.save_outlined, size: 16),
                  label: Text(context.strings.profileEditorSave),
                ),
              ],
            ),
          ),
        ),
      ),
    );
  }

  Widget _artwork() {
    final theme = Theme.of(context);
    final cover = _coverPreview;
    final compact = MediaQuery.sizeOf(context).width <= 780;
    return Column(
      crossAxisAlignment: CrossAxisAlignment.stretch,
      children: [
        Container(
          key: const ValueKey('profile-editor-preview'),
          constraints: const BoxConstraints(minHeight: 72),
          clipBehavior: Clip.antiAlias,
          decoration: BoxDecoration(
            color: theme.colorScheme.surfaceContainer,
            border: Border.all(color: theme.colorScheme.outlineVariant),
            borderRadius: BorderRadius.circular(8),
            image: cover == null
                ? null
                : DecorationImage(
                    image: MemoryImage(cover),
                    fit: BoxFit.cover,
                    colorFilter: ColorFilter.mode(
                      theme.colorScheme.surface.withValues(alpha: .65),
                      BlendMode.srcOver,
                    ),
                  ),
          ),
          padding: const EdgeInsets.all(12),
          child: Row(
            spacing: 10,
            children: [
              AccountAvatar(
                api: widget.session.api,
                username: null,
                address: _avatarPreview == null ? null : Uri.parse(_avatar),
                size: 40,
              ),
              Expanded(
                child: Column(
                  crossAxisAlignment: CrossAxisAlignment.start,
                  children: [
                    Text(
                      _name.text,
                      maxLines: 1,
                      overflow: TextOverflow.ellipsis,
                      style: theme.textTheme.bodyMedium?.copyWith(
                        fontWeight: FontWeight.w600,
                      ),
                    ),
                    IdentityDetails(username: widget.username),
                  ],
                ),
              ),
              IconButton(
                tooltip: context.strings.profileEditorPublic,
                onPressed: _busy
                    ? null
                    : () => context.push(profilePath(widget.username)),
                icon: const Icon(Icons.open_in_new, size: 16),
              ),
            ],
          ),
        ),
        const SizedBox(height: 16),
        Flex(
          direction: compact ? Axis.horizontal : Axis.vertical,
          crossAxisAlignment: CrossAxisAlignment.start,
          spacing: compact ? 16 : 0,
          children: [
            Flexible(
              flex: compact ? 1 : 0,
              fit: compact ? FlexFit.tight : FlexFit.loose,
              child: _imageRow(ProfileImageKind.avatar, _avatar),
            ),
            Flexible(
              flex: compact ? 1 : 0,
              fit: compact ? FlexFit.tight : FlexFit.loose,
              child: _imageRow(ProfileImageKind.cover, _cover),
            ),
          ],
        ),
        const SizedBox(height: 8),
        Text(
          context.strings.profileEditorImageHint,
          style: theme.textTheme.bodySmall?.copyWith(
            fontSize: 11,
            color: theme.colorScheme.onSurfaceVariant,
          ),
        ),
      ],
    );
  }

  Widget _imageRow(ProfileImageKind kind, String current) => LayoutBuilder(
    builder: (context, box) => Padding(
      padding: const EdgeInsets.symmetric(vertical: 8),
      child: Flex(
        direction: MediaQuery.sizeOf(context).width <= 780
            ? Axis.vertical
            : Axis.horizontal,
        crossAxisAlignment: CrossAxisAlignment.start,
        spacing: 8,
        children: [
          Flexible(
            flex: MediaQuery.sizeOf(context).width <= 780 ? 0 : 1,
            fit: MediaQuery.sizeOf(context).width <= 780
                ? FlexFit.loose
                : FlexFit.tight,
            child: Column(
              crossAxisAlignment: CrossAxisAlignment.start,
              children: [
                Text(
                  kind == ProfileImageKind.avatar
                      ? context.strings.profileEditorAvatar
                      : context.strings.profileEditorCover,
                  style: Theme.of(context).textTheme.bodyMedium,
                ),
                if (_review(kind.name) case final String status) Text(status),
              ],
            ),
          ),
          Wrap(
            crossAxisAlignment: WrapCrossAlignment.center,
            children: [
              if (box.maxWidth < 260)
                IconButton(
                  key: ValueKey('choose-${kind.name}'),
                  tooltip: context.strings.profileEditorChooseImage,
                  onPressed: _busy ? null : () => _pick(kind),
                  icon: const Icon(Icons.add_photo_alternate_outlined),
                )
              else
                OutlinedButton.icon(
                  style: LauncherControl.style(context),
                  key: ValueKey('choose-${kind.name}'),
                  onPressed: _busy ? null : () => _pick(kind),
                  icon: const Icon(Icons.add_photo_alternate_outlined),
                  label: Text(context.strings.profileEditorChooseImage),
                ),
              if (current.isNotEmpty)
                IconButton(
                  tooltip: kind == ProfileImageKind.avatar
                      ? context.strings.profileEditorRemoveAvatar
                      : context.strings.profileEditorRemoveCover,
                  onPressed: _busy ? null : () => _removeImage(kind),
                  icon: const Icon(Icons.delete_outline),
                ),
            ],
          ),
        ],
      ),
    ),
  );

  Widget _fields() {
    final theme = Theme.of(context);
    return Column(
      crossAxisAlignment: CrossAxisAlignment.stretch,
      spacing: 16,
      children: [
        AccountField(
          inputKey: const ValueKey('profile-name'),
          label: context.strings.profileEditorName,
          controller: _name,
          enabled: !_busy,
          maxLength: 12,
          autofill: AutofillHints.nickname,
          onChanged: _changed,
          helperText: _review('name'),
          validator: (value) {
            final name = value?.trim() ?? '';
            return name.isEmpty || name.runes.length > 12
                ? context.strings.profileEditorNameInvalid
                : null;
          },
        ),
        AccountField(
          inputKey: const ValueKey('profile-bio'),
          label: context.strings.profileEditorBio,
          controller: _bio,
          enabled: !_busy,
          maxLength: 500,
          showCounter: true,
          minLines: 4,
          maxLines: 6,
          keyboard: TextInputType.multiline,
          onChanged: _changed,
          helperText: _review('bio'),
          validator: (value) => (value?.length ?? 0) > 500
              ? context.strings.profileEditorBioInvalid
              : null,
        ),
        Text(
          context.strings.profileEditorLinks,
          style: theme.textTheme.bodyMedium,
        ),
        if (_review('links') case final String status) Text(status),
        for (final row in _links) _linkFields(row),
        Align(
          alignment: AlignmentDirectional.centerStart,
          child: OutlinedButton.icon(
            style: LauncherControl.style(context),
            onPressed: !_busy && _links.length < 12 ? _addLink : null,
            icon: const Icon(LucideIcons.plus, size: 16),
            label: Text(context.strings.profileEditorAddLink),
          ),
        ),
        Padding(
          padding: const EdgeInsets.only(top: 8),
          child: Column(
            crossAxisAlignment: CrossAxisAlignment.stretch,
            children: [
              const Divider(height: 1),
              const SizedBox(height: 20),
              Text(
                context.strings.profileEditorDisplay,
                style: theme.textTheme.titleMedium?.copyWith(
                  fontSize: 15,
                  fontWeight: FontWeight.w600,
                ),
              ),
            ],
          ),
        ),
        Card.outlined(
          margin: EdgeInsets.zero,
          color: theme.colorScheme.surface,
          shape: RoundedRectangleBorder(
            borderRadius: BorderRadius.circular(8),
            side: BorderSide(color: theme.colorScheme.outlineVariant),
          ),
          child: Column(
            children: [
              for (final view in const [ProfileGames.all, ProfileGames.th09])
                ListTile(
                  contentPadding: const EdgeInsets.symmetric(horizontal: 12),
                  minLeadingWidth: 18,
                  horizontalTitleGap: 10,
                  leading: view == ProfileGames.all
                      ? const Icon(LucideIcons.layoutGrid, size: 18)
                      : ClipRRect(
                          borderRadius: BorderRadius.circular(4),
                          child: Image.asset(
                            'assets/book-covers/th09.webp',
                            width: 18,
                            height: 18,
                            fit: BoxFit.cover,
                          ),
                        ),
                  title: Text(
                    view == ProfileGames.th09
                        ? context.strings.th09ProfileLabel
                        : context.strings.profileGameDataAll,
                  ),
                  trailing: Row(
                    mainAxisSize: MainAxisSize.min,
                    spacing: 4,
                    children: [
                      IconButton(
                        tooltip: context.strings.profileEditorDefault,
                        isSelected: _display.selected == view,
                        icon: const Icon(Icons.star_border),
                        selectedIcon: const Icon(Icons.star),
                        onPressed: _busy || !_display.shows(view)
                            ? null
                            : () {
                                _display = _display.choose(view);
                                _changed();
                              },
                      ),
                      IconButton(
                        key: ValueKey('visibility-${view.name}'),
                        tooltip: _display.shows(view)
                            ? context.strings.profileEditorHide
                            : context.strings.profileEditorShow,
                        icon: Icon(
                          _display.shows(view)
                              ? Icons.visibility_outlined
                              : Icons.visibility_off_outlined,
                        ),
                        onPressed: _busy
                            ? null
                            : () => _showView(view, !_display.shows(view)),
                      ),
                    ],
                  ),
                ),
            ],
          ),
        ),
        SwitchListTile(
          contentPadding: EdgeInsets.zero,
          title: Text(context.strings.profileEditorSocial),
          value: _display.friends,
          onChanged: _busy
              ? null
              : (visible) {
                  _display = _display.showFriends(visible);
                  _changed();
                },
        ),
      ],
    );
  }

  Widget _linkFields(_LinkEditor row) => LayoutBuilder(
    key: ObjectKey(row),
    builder: (context, constraints) {
      InputDecoration decoration(String label) => InputDecoration(
        border: const OutlineInputBorder(),
        labelText: label,
        floatingLabelBehavior: FloatingLabelBehavior.never,
        isDense: true,
        contentPadding: const EdgeInsets.symmetric(
          horizontal: 12,
          vertical: 10,
        ),
        constraints: const BoxConstraints(minHeight: 44),
        counterText: '',
      );
      final name = TextFormField(
        enabled: !_busy,
        controller: row.label,
        maxLength: 40,
        onChanged: _changed,
        decoration: decoration(context.strings.profileEditorLinkName),
        validator: (name) =>
            name?.trim().isNotEmpty == true && name!.trim().length <= 40
            ? null
            : context.strings.profileEditorLinkNameInvalid,
      );
      final address = TextFormField(
        enabled: !_busy,
        controller: row.url,
        keyboardType: TextInputType.url,
        onChanged: _changed,
        decoration: decoration(context.strings.profileEditorLinkUrl)
            .copyWith(hintText: 'https://'),
        validator: (raw) {
          final value = raw?.trim() ?? '';
          final uri = Uri.tryParse(value);
          return value.length <= 2048 &&
                  uri != null &&
                  uri.host.isNotEmpty &&
                  uri.userInfo.isEmpty &&
                  (uri.scheme == 'https' || uri.scheme == 'http')
              ? null
              : context.strings.profileEditorLinkUrlInvalid;
        },
      );
      final remove = SizedBox.square(
        dimension: LauncherControl.height,
        child: IconButton(
          onPressed: _busy ? null : () => _removeLink(row),
          tooltip: context.strings.profileEditorDeleteLink,
          icon: const Icon(Icons.delete_outline, size: 16),
        ),
      );
      final inline =
          MediaQuery.sizeOf(context).width > 780 && constraints.maxWidth >= 400;
      return Flex(
        direction: inline ? Axis.horizontal : Axis.vertical,
        crossAxisAlignment: CrossAxisAlignment.start,
        spacing: 8,
        children: [
          Flexible(
            flex: 0,
            fit: FlexFit.loose,
            child: SizedBox(
              width: inline
                  ? (constraints.maxWidth - 16 - LauncherControl.height) / 3
                  : null,
              child: name,
            ),
          ),
          Flexible(
            flex: inline ? 2 : 0,
            fit: inline ? FlexFit.tight : FlexFit.loose,
            child: Row(
              crossAxisAlignment: CrossAxisAlignment.start,
              spacing: 8,
              children: [
                Expanded(child: address),
                remove,
              ],
            ),
          ),
        ],
      );
    },
  );
}

class _LinkEditor {
  _LinkEditor(ProfileLinkDraft initial)
    : label = TextEditingController(text: initial.label),
      url = TextEditingController(text: initial.url);

  final TextEditingController label;
  final TextEditingController url;

  ProfileLinkDraft get value => (label: label.text, url: url.text);
  ProfileLinkDraft get trimmed =>
      (label: label.text.trim(), url: url.text.trim());

  void dispose() {
    label.dispose();
    url.dispose();
  }
}
