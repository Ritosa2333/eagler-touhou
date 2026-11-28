import 'dart:async';

import 'package:go_router/go_router.dart';
import 'package:lucide_icons_flutter/lucide_icons.dart';
import 'package:intl/intl.dart';
import 'package:material_ui/material_ui.dart';
import 'package:prism_ui/prism_ui.dart' as prism;
import 'package:uuid/uuid.dart';

import 'account.dart';
import 'account_api.dart';
import 'inbox.dart';
import 'l10n/strings.dart';
import 'launcher_background.dart';
import 'launcher_navigation.dart';
import 'message_highlights.dart';

enum MessageSection {
  announcements('/announcements', LucideIcons.megaphone),
  inbox('/messages', LucideIcons.mail);

  const MessageSection(this.path, this.icon);
  final String path;
  final IconData icon;

  int unread(MessageHighlights highlights) => switch (this) {
    announcements => highlights.announcementUnread,
    inbox => highlights.inboxUnread ?? 0,
  };

  String label(LauncherStrings strings) => switch (this) {
    announcements => strings.announcements,
    inbox => strings.privateMessages,
  };
}

class MessageActions extends StatelessWidget {
  const MessageActions({
    super.key,
    required this.highlights,
    this.showPreviews = false,
    this.onOpenSection,
  });

  final MessageHighlights highlights;
  final bool showPreviews;
  final ValueChanged<MessageSection>? onOpenSection;

  @override
  Widget build(BuildContext context) => ListenableBuilder(
    listenable: highlights,
    builder: (context, _) => Row(
      mainAxisSize: MainAxisSize.min,
      children: [
        for (final section in MessageSection.values)
          _MessageEntryButton(
            section: section,
            highlights: highlights,
            showPreview: showPreviews,
            onOpenSection: onOpenSection,
          ),
      ],
    ),
  );
}

class _MessageEntryButton extends StatefulWidget {
  const _MessageEntryButton({
    required this.section,
    required this.highlights,
    required this.showPreview,
    this.onOpenSection,
  });

  final MessageSection section;
  final MessageHighlights highlights;
  final bool showPreview;
  final ValueChanged<MessageSection>? onOpenSection;

  @override
  State<_MessageEntryButton> createState() => _MessageEntryButtonState();
}

class _MessageEntryButtonState extends State<_MessageEntryButton> {
  final _focus = FocusNode();
  final _menu = MenuController();

  @override
  void dispose() {
    _focus.dispose();
    super.dispose();
  }

  Widget _button(BuildContext context, VoidCallback open) {
    final strings = context.strings;
    final section = widget.section;
    final label = section.label(strings);
    final count = section.unread(widget.highlights);
    final colors = Theme.of(context).colorScheme;
    return Semantics(
      label: count > 0 ? strings.messageUnread(count) : null,
      child: IconButton(
        focusNode: _focus,
        tooltip: label,
        onPressed: open,
        icon: ExcludeSemantics(
          child: Badge.count(
            key: ValueKey((section.path, 'unread')),
            count: count,
            isLabelVisible: count > 0,
            backgroundColor: colors.primary,
            textColor: colors.onPrimary,
            smallSize: 6,
            largeSize: 14,
            textStyle: const TextStyle(fontSize: 9),
            offset: const Offset(7, -4),
            child: Icon(section.icon, size: 18),
          ),
        ),
      ),
    );
  }

  @override
  Widget build(BuildContext context) {
    final section = widget.section;
    void openPage() {
      final open = widget.onOpenSection;
      if (open == null) {
        context.push(section.path);
      } else {
        open(section);
      }
    }

    if (!widget.showPreview) return _button(context, openPage);

    final colors = Theme.of(context).colorScheme;
    const previewWidth = 392.0;
    final size = MediaQuery.sizeOf(context);
    return MenuAnchor(
      controller: _menu,
      childFocusNode: _focus,
      useRootOverlay: true,
      crossAxisUnconstrained: false,
      animated: !MediaQuery.disableAnimationsOf(context),
      alignmentOffset: const Offset(-previewWidth, 7),
      reservedPadding: const EdgeInsets.all(12),
      clipBehavior: Clip.antiAlias,
      style: MenuStyle(
        alignment: AlignmentDirectional.bottomEnd,
        minimumSize: const WidgetStatePropertyAll(Size(previewWidth, 0)),
        maximumSize: WidgetStatePropertyAll(
          Size(previewWidth, (size.height - 60).clamp(80, 500).toDouble()),
        ),
        padding: const WidgetStatePropertyAll(EdgeInsets.zero),
        backgroundColor: WidgetStatePropertyAll(colors.surfaceContainer),
        elevation: const WidgetStatePropertyAll(3),
        surfaceTintColor: const WidgetStatePropertyAll(Colors.transparent),
        shape: WidgetStatePropertyAll(
          RoundedRectangleBorder(
            borderRadius: BorderRadius.circular(10),
            side: BorderSide(color: colors.outlineVariant),
          ),
        ),
      ),
      menuChildren: [
        SizedBox(
          key: ValueKey((section.path, 'preview')),
          width: previewWidth,
          child: _MessagePreview(
            section: section,
            highlights: widget.highlights,
            isOpen: () => _menu.isOpen,
            viewAll: () {
              _menu.close();
              openPage();
            },
          ),
        ),
      ],
      builder: (context, menu, _) =>
          _button(context, () => menu.isOpen ? menu.close() : menu.open()),
    );
  }
}

class _MessagePreview extends StatefulWidget {
  const _MessagePreview({
    required this.section,
    required this.highlights,
    required this.isOpen,
    required this.viewAll,
  });

  final MessageSection section;
  final MessageHighlights highlights;
  final bool Function() isOpen;
  final VoidCallback viewAll;

  @override
  State<_MessagePreview> createState() => _MessagePreviewState();
}

class _MessagePreviewState extends State<_MessagePreview> {
  Future<PublicAnnouncement?>? _latest;

  @override
  void initState() {
    super.initState();
    if (widget.section == MessageSection.announcements) {
      _latest = _loadLatest();
    } else {
      widget.highlights.refreshInbox();
    }
  }

  Future<PublicAnnouncement?> _loadLatest() async {
    void markSeen(PublicAnnouncement? entry) {
      if (entry != null && mounted && widget.isOpen()) {
        unawaited(widget.highlights.markAnnouncementsSeen([entry]));
      }
    }

    markSeen(widget.highlights.latestAnnouncement);
    final current = await widget.highlights.announcements(refresh: true);
    final entry = current.items.firstOrNull;
    markSeen(entry);
    return entry;
  }

  @override
  Widget build(BuildContext context) {
    final strings = context.strings;
    final theme = Theme.of(context);
    final announcements = widget.section == MessageSection.announcements;

    return Padding(
      padding: const EdgeInsets.fromLTRB(16, 12, 16, 16),
      child: Column(
        mainAxisSize: MainAxisSize.min,
        crossAxisAlignment: CrossAxisAlignment.start,
        spacing: 12,
        children: [
          Row(
            spacing: 8,
            children: [
              Expanded(
                child: Semantics(
                  header: true,
                  child: Text(
                    announcements
                        ? strings.latestAnnouncement
                        : strings.privateMessages,
                    style: theme.textTheme.titleMedium?.copyWith(
                      fontWeight: FontWeight.w700,
                    ),
                  ),
                ),
              ),
              TextButton(
                onPressed: widget.viewAll,
                style: TextButton.styleFrom(
                  minimumSize: const Size(0, 31),
                  padding: const EdgeInsets.symmetric(horizontal: 8),
                  tapTargetSize: MaterialTapTargetSize.shrinkWrap,
                  foregroundColor: theme.colorScheme.onSurfaceVariant,
                ),
                child: Text(strings.viewAllMessages),
              ),
            ],
          ),
          if (announcements)
            FutureBuilder<PublicAnnouncement?>(
              initialData: widget.highlights.latestAnnouncement,
              future: _latest,
              builder: (context, snapshot) {
                if (snapshot.connectionState != ConnectionState.done &&
                    !snapshot.hasData) {
                  return const LinearProgressIndicator();
                }
                if (snapshot.hasError) {
                  return TextButton(
                    onPressed: () {
                      final request = _loadLatest();
                      setState(() {
                        _latest = request;
                      });
                    },
                    child: Text(strings.messageLoadRetry),
                  );
                }
                final entry = snapshot.data;
                if (entry == null) return Text(strings.noAnnouncements);
                return _Announcement(
                  key: ValueKey(entry.id),
                  api: widget.highlights.session.api,
                  entry: entry,
                  preview: true,
                );
              },
            )
          else
            ListenableBuilder(
              listenable: widget.highlights,
              builder: (context, _) {
                final unread = widget.highlights.inboxUnread;
                if (unread == null && widget.highlights.inboxFailed) {
                  return TextButton(
                    onPressed: widget.highlights.refreshInbox,
                    child: Text(strings.messageLoadRetry),
                  );
                }
                return Text(
                  unread == null
                      ? strings.openInbox
                      : strings.messageUnread(unread),
                  style: theme.textTheme.bodySmall?.copyWith(
                    color: theme.colorScheme.onSurfaceVariant,
                  ),
                );
              },
            ),
        ],
      ),
    );
  }
}

enum MessageEntryOrigin { inbox }

class MessagesPage extends StatelessWidget {
  const MessagesPage({
    super.key,
    required this.highlights,
    required this.section,
    this.peer,
    this.returnToInbox = false,
  });

  final MessageHighlights highlights;
  final MessageSection section;
  final String? peer;
  final bool returnToInbox;

  void back(BuildContext context) {
    if (returnToInbox && peer != null) {
      context.replace(MessageSection.inbox.path);
    } else {
      closeLauncherPage(context, '/');
    }
  }

  @override
  Widget build(BuildContext context) {
    final compact = MediaQuery.sizeOf(context).width <= launcherCompactWidth;
    final conversation = peer;
    return PopScope(
      canPop: !(returnToInbox && conversation != null),
      onPopInvokedWithResult: (didPop, _) {
        if (!didPop && returnToInbox && conversation != null) {
          back(context);
        }
      },
      child: compact && conversation != null
          ? ConversationPageView(
              key: ValueKey(conversation),
              username: conversation,
              highlights: highlights,
              onBack: returnToInbox ? () => back(context) : null,
            )
          : DefaultTabController(
              length: MessageSection.values.length,
              initialIndex: section.index,
              animationDuration:
                  compact && !MediaQuery.disableAnimationsOf(context)
                  ? const Duration(milliseconds: 180)
                  : Duration.zero,
              child: _MessagesLayout(page: this, compact: compact),
            ),
    );
  }
}

class _MessagesLayout extends StatefulWidget {
  const _MessagesLayout({required this.page, required this.compact});
  final MessagesPage page;
  final bool compact;

  @override
  State<_MessagesLayout> createState() => _MessagesLayoutState();
}

class _MessagesLayoutState extends State<_MessagesLayout> {
  TabController? _tabs;

  @override
  void didChangeDependencies() {
    super.didChangeDependencies();
    final next = DefaultTabController.of(context);
    if (_tabs == next) return;
    _tabs?.removeListener(_setLocation);
    _tabs = next;
    next.addListener(_setLocation);
  }

  @override
  void didUpdateWidget(_MessagesLayout oldWidget) {
    super.didUpdateWidget(oldWidget);
    final tab = _tabs!;
    final section = widget.page.section;
    if (oldWidget.page.section != section && tab.index != section.index) {
      tab.index = section.index;
    }
  }

  void _setLocation() {
    final tab = _tabs!;
    if (tab.indexIsChanging || ModalRoute.of(context)?.isCurrent == false) {
      return;
    }
    final section = MessageSection.values[tab.index];
    if (section != widget.page.section) context.replace(section.path);
  }

  @override
  void dispose() {
    _tabs?.removeListener(_setLocation);
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    final page = widget.page;
    final session = page.highlights.session;
    final strings = context.strings;
    final theme = Theme.of(context);
    final compact = widget.compact;
    final content = TabBarView(
      physics: compact ? null : const NeverScrollableScrollPhysics(),
      children: [
        ListenableBuilder(
          listenable: session,
          builder: (context, _) => _AnnouncementList(
            key: ValueKey(session.identity?.username),
            highlights: page.highlights,
            padding: compact ? const EdgeInsets.all(16) : EdgeInsets.zero,
          ),
        ),
        ListenableBuilder(
          listenable: session,
          builder: (context, _) {
            final member = session.identity;
            return member == null
                ? MessageSignIn(
                    padding: compact
                        ? const EdgeInsets.all(16)
                        : EdgeInsets.zero,
                  )
                : compact
                ? Inbox(
                    key: ValueKey(member.username),
                    highlights: page.highlights,
                    padding: const EdgeInsets.all(16),
                  )
                : InboxWorkspace(
                    key: ValueKey(member.username),
                    highlights: page.highlights,
                    username: page.peer,
                    onOpen: (username) {
                      if (page.peer == username) return;
                      FocusManager.instance.primaryFocus?.unfocus();
                      context.replace(
                        messageThreadPath(username),
                        extra: MessageEntryOrigin.inbox,
                      );
                    },
                    onClose: () => context.replace(MessageSection.inbox.path),
                  );
          },
        ),
      ],
    );

    return Scaffold(
      backgroundColor: compact ? null : LauncherBackdrop.page(context),
      appBar: compact
          ? AppBar(
              toolbarHeight: 52,
              backgroundColor: theme.colorScheme.surface,
              titleSpacing: 8,
              title: Text(
                strings.messageCenter,
                style: theme.textTheme.titleMedium,
              ),
              leading: BackButton(
                onPressed: () => closeLauncherPage(context, '/'),
              ),
              bottom: TabBar(
                labelPadding: const EdgeInsets.symmetric(horizontal: 6),
                labelColor: theme.colorScheme.primary,
                unselectedLabelColor: theme.colorScheme.onSurfaceVariant,
                dividerColor: theme.colorScheme.outlineVariant,
                indicatorSize: TabBarIndicatorSize.tab,
                indicatorPadding: EdgeInsets.symmetric(
                  horizontal: (MediaQuery.sizeOf(context).width / 4 - 19).clamp(
                    0.0,
                    double.infinity,
                  ),
                ),
                indicator: UnderlineTabIndicator(
                  borderSide: BorderSide(
                    width: 2,
                    color: theme.colorScheme.primary,
                  ),
                ),
                tabs: [
                  for (final section in MessageSection.values)
                    Tab(
                      height: 42,
                      child: Row(
                        mainAxisSize: MainAxisSize.min,
                        spacing: 8,
                        children: [
                          Icon(section.icon, size: 17),
                          Flexible(
                            child: Text(
                              section.label(strings),
                              maxLines: 1,
                              overflow: TextOverflow.ellipsis,
                            ),
                          ),
                          ListenableBuilder(
                            listenable: page.highlights,
                            builder: (context, _) => _SectionBadge(
                              count: section.unread(page.highlights),
                            ),
                          ),
                        ],
                      ),
                    ),
                ],
              ),
            )
          : null,
      body: compact
          ? content
          : Align(
              alignment: Alignment.topCenter,
              child: Padding(
                padding: const EdgeInsets.fromLTRB(24, 58, 24, 40),
                child: ConstrainedBox(
                  constraints: const BoxConstraints(maxWidth: 1030),
                  child: Row(
                    key: const ValueKey('message center body'),
                    children: [
                      SizedBox(
                        width: 220,
                        child: _MessageSidebar(
                          tabs: _tabs!,
                          highlights: page.highlights,
                          onBack: () => page.back(context),
                        ),
                      ),
                      const SizedBox(width: 40),
                      Expanded(
                        child: Padding(
                          key: const ValueKey('message center content'),
                          padding: const EdgeInsets.only(top: 13),
                          child: content,
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

class _MessageSidebar extends StatelessWidget {
  const _MessageSidebar({
    required this.tabs,
    required this.highlights,
    required this.onBack,
  });

  final TabController tabs;
  final MessageHighlights highlights;
  final VoidCallback onBack;

  @override
  Widget build(BuildContext context) {
    final strings = context.strings;
    final theme = Theme.of(context);
    final palette = prism.Theme.of(context).colorScheme;
    return DecoratedBox(
      decoration: BoxDecoration(
        border: BorderDirectional(
          end: BorderSide(color: palette.outlineVariant),
        ),
      ),
      child: Padding(
        padding: const EdgeInsetsDirectional.only(end: 29),
        child: ListView(
          primary: false,
          padding: EdgeInsets.zero,
          children: [
            Padding(
              padding: const EdgeInsetsDirectional.only(start: 10, bottom: 12),
              child: ConstrainedBox(
                constraints: const BoxConstraints(minHeight: 35),
                child: Row(
                  children: [
                    Expanded(
                      child: Semantics(
                        header: true,
                        child: Text(
                          strings.messageCenter,
                          style: theme.textTheme.titleLarge?.copyWith(
                            fontSize: 21,
                            fontWeight: FontWeight.w600,
                          ),
                        ),
                      ),
                    ),
                    IconButton(
                      iconSize: 17,
                      tooltip: MaterialLocalizations.of(context)
                          .backButtonTooltip,
                      visualDensity: VisualDensity.compact,
                      onPressed: onBack,
                      icon: const Icon(Icons.arrow_back),
                    ),
                  ],
                ),
              ),
            ),
            ListenableBuilder(
              listenable: Listenable.merge([tabs, highlights]),
              builder: (context, _) => Column(
                spacing: 4,
                children: [
                  for (final section in MessageSection.values)
                    _MessageSidebarRow(
                      section: section,
                      selected: section.index == tabs.index,
                      unread: section.unread(highlights),
                      onPressed: () => tabs.index = section.index,
                    ),
                ],
              ),
            ),
          ],
        ),
      ),
    );
  }
}

class _MessageSidebarRow extends StatelessWidget {
  const _MessageSidebarRow({
    required this.section,
    required this.selected,
    required this.unread,
    required this.onPressed,
  });

  final MessageSection section;
  final bool selected;
  final int unread;
  final VoidCallback onPressed;

  @override
  Widget build(BuildContext context) {
    final palette = prism.Theme.of(context).colorScheme;
    final color = selected ? palette.primary : palette.onSurfaceVariant;
    final strings = context.strings;
    final label = section.label(strings);
    return Tooltip(
      message: label,
      excludeFromSemantics: true,
      child: prism.PressSurface(
        selected: selected,
        key: ValueKey((section.path, 'sidebar')),
        onPressed: onPressed,
        fill: selected ? palette.surfaceContainerHigh : Colors.transparent,
        shape: prism.AppShape.sm,
        showsHover: !selected,
        child: Padding(
          padding: const EdgeInsets.symmetric(horizontal: 12),
          child: ConstrainedBox(
            constraints: const BoxConstraints(minHeight: 42),
            child: Row(
              spacing: 8,
              children: [
                Icon(section.icon, size: 17, color: color),
                Expanded(
                  child: Text(
                    label,
                    overflow: TextOverflow.ellipsis,
                    maxLines: 1,
                    style: TextStyle(
                      color: color,
                      fontWeight: selected ? FontWeight.w600 : FontWeight.w400,
                    ),
                  ),
                ),
                _SectionBadge(count: unread),
              ],
            ),
          ),
        ),
      ),
    );
  }
}

class _SectionBadge extends StatelessWidget {
  const _SectionBadge({required this.count});

  final int count;

  @override
  Widget build(BuildContext context) {
    if (count <= 0) return const SizedBox.shrink();
    final primary = prism.Theme.of(context).colorScheme.primary;
    return Padding(
      padding: const EdgeInsetsDirectional.only(start: 6),
      child: Semantics(
        label: context.strings.messageUnread(count),
        excludeSemantics: true,
        child: Badge.count(
          count: count,
          backgroundColor: primary.withValues(alpha: .16),
          textColor: primary,
        ),
      ),
    );
  }
}

class _AnnouncementList extends StatefulWidget {
  const _AnnouncementList({
    super.key,
    required this.highlights,
    required this.padding,
  });

  final EdgeInsets padding;

  final MessageHighlights highlights;
  AccountSession get session => highlights.session;

  @override
  State<_AnnouncementList> createState() => _AnnouncementListState();
}

class _AnnouncementListState extends State<_AnnouncementList>
    with AutomaticKeepAliveClientMixin {
  final draft = TextEditingController();
  var items = <PublicAnnouncement>[];
  bool canPublish = false;
  bool publishing = false;
  bool publishFailed = false;
  bool published = false;
  ({String id, String body})? pending;
  int? nextBefore;
  bool loading = false;
  bool failed = false;
  TabController? _tabs;

  void _markVisible() {
    final tabs = _tabs;
    if (tabs != null && tabs.index == 0 && !tabs.indexIsChanging) {
      widget.highlights.markAnnouncementsSeen(items);
    }
  }

  @override
  void didChangeDependencies() {
    super.didChangeDependencies();
    final tabs = DefaultTabController.of(context);
    if (_tabs == tabs) return;
    _tabs?.removeListener(_markVisible);
    _tabs = tabs;
    tabs.addListener(_markVisible);
    _markVisible();
  }

  @override
  bool get wantKeepAlive => true;

  @override
  void initState() {
    super.initState();
    load();
  }

  Future<void> load({
    bool more = false,
    bool refresh = false,
    bool published = false,
  }) async {
    if (loading) return;
    setState(() {
      loading = true;
      failed = false;
    });
    try {
      final page = published
          ? await widget.highlights.publishedAnnouncement()
          : await widget.highlights.announcements(
              before: more ? nextBefore : null,
              refresh: refresh,
            );
      if (!mounted) return;
      setState(() {
        items = more ? [...items, ...page.items] : page.items;
        nextBefore = page.nextBefore;
        canPublish = page.canPublish;
        loading = false;
      });
      _markVisible();
    } catch (error) {
      if (!mounted) return;
      widget.session.expireIfUnauthorized(error);
      setState(() {
        loading = false;
        failed = true;
      });
    }
  }

  Future<void> publish() async {
    final body = draft.text.trim();
    if (!canPublish || publishing || loading || body.isEmpty) return;
    final id = pending?.body == body ? pending!.id : const Uuid().v4();
    pending = (id: id, body: body);
    setState(() {
      publishing = true;
      published = false;
      publishFailed = false;
    });
    try {
      await widget.session.api.publishAnnouncement(body, id);
      if (!mounted) return;
      pending = null;
      draft.clear();
      await load(published: true);
      if (mounted) setState(() => published = true);
    } catch (error) {
      if (!mounted) return;
      widget.session.expireIfUnauthorized(error);
      setState(() {
        publishFailed = true;
      });
    } finally {
      if (mounted) setState(() => publishing = false);
    }
  }

  @override
  void dispose() {
    _tabs?.removeListener(_markVisible);
    draft.dispose();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    super.build(context);
    return RefreshIndicator(
      onRefresh: () => load(refresh: true),
      child: ListView(
        physics: const AlwaysScrollableScrollPhysics(),
        padding: widget.padding,
        children: [
          if (canPublish)
            Align(
              alignment: AlignmentDirectional.centerStart,
              child: ConstrainedBox(
                constraints: const BoxConstraints(maxWidth: 740),
                child: Column(
                  crossAxisAlignment: CrossAxisAlignment.end,
                  spacing: 8,
                  children: [
                    TextField(
                      controller: draft,
                      enabled: !publishing,
                      minLines: 3,
                      maxLines: 8,
                      maxLength: 4000,
                      onChanged: (_) => setState(() {}),
                      decoration: InputDecoration(
                        border: OutlineInputBorder(),
                        hintText: context.strings.announcementInput,
                      ),
                    ),
                    if (publishFailed)
                      Semantics(
                        liveRegion: true,
                        child: Text(context.strings.announcementPublishRetry),
                      ),
                    FilledButton.icon(
                      onPressed:
                          publishing || loading || draft.text.trim().isEmpty
                          ? null
                          : publish,
                      icon: const Icon(Icons.send),
                      label: Text(context.strings.publishAnnouncement),
                    ),
                  ],
                ),
              ),
            ),
          if (published)
            Padding(
              padding: const EdgeInsets.symmetric(vertical: 8),
              child: Semantics(
                liveRegion: true,
                child: Text(context.strings.announcementPublishedNotice),
              ),
            ),
          if (items.isEmpty && !loading && !failed)
            Center(child: Text(context.strings.noAnnouncements)),
          for (final (index, entry) in items.indexed)
            Align(
              alignment: AlignmentDirectional.centerStart,
              child: ConstrainedBox(
                constraints: const BoxConstraints(maxWidth: 740),
                child: _Announcement(
                  key: ValueKey(entry.id),
                  api: widget.session.api,
                  entry: entry,
                  connectAbove: index > 0,
                  connectBelow: index + 1 < items.length,
                ),
              ),
            ),
          if (loading) const Center(child: CircularProgressIndicator()),
          if (!loading && (failed || nextBefore != null))
            Center(
              child: TextButton(
                onPressed: () =>
                    load(more: nextBefore != null, refresh: failed),
                child: Text(
                  failed
                      ? context.strings.messageLoadRetry
                      : context.strings.loadMoreMessages,
                ),
              ),
            ),
        ],
      ),
    );
  }
}

class _Announcement extends StatefulWidget {
  const _Announcement({
    super.key,
    required this.api,
    required this.entry,
    this.preview = false,
    this.connectAbove = false,
    this.connectBelow = false,
  });

  final AccountApi api;
  final PublicAnnouncement entry;
  final bool preview;
  final bool connectAbove;
  final bool connectBelow;

  @override
  State<_Announcement> createState() => _AnnouncementState();
}

class _AnnouncementState extends State<_Announcement> {
  bool expanded = false;

  @override
  Widget build(BuildContext context) {
    final entry = widget.entry;
    final username = entry.authorUsername;
    final theme = Theme.of(context);
    final strings = context.strings;
    final age = DateTime.now().difference(entry.publishedAt);
    final relativeTime = switch (age) {
      Duration(inDays: final days) when days > 0 => strings.announcementDaysAgo(
        days,
      ),
      Duration(inHours: final hours) when hours > 0 =>
        strings.announcementHoursAgo(hours),
      Duration(inMinutes: final minutes) when minutes > 0 =>
        strings.announcementMinutesAgo(minutes),
      _ => strings.announcementJustNow,
    };
    final timestamp = DateFormat('yyyy.MM.dd HH:mm:ss')
        .format(entry.publishedAt.toUtc().add(const Duration(hours: 8)));
    final byline = strings.announcementByline(
      entry.author,
      '$timestamp (UTC+8)',
    );
    final metadataStyle = theme.textTheme.bodySmall?.copyWith(
      color: theme.colorScheme.onSurfaceVariant,
    );
    final preview = widget.preview;
    final compact = MediaQuery.sizeOf(context).width <= launcherCompactWidth;
    final avatarSize = preview ? 32.0 : 35.0;
    final limit = preview ? 6 : 10;
    final row = Padding(
      padding: EdgeInsets.only(bottom: widget.connectBelow ? 27 : 0),
      child: Row(
        spacing: preview ? 10 : (compact ? 8 : 16),
        crossAxisAlignment: CrossAxisAlignment.start,
        children: [
          AccountAvatar(api: widget.api, username: username, size: avatarSize),
          Expanded(
            child: Column(
              crossAxisAlignment: CrossAxisAlignment.start,
              spacing: 4,
              children: [
                Text(relativeTime, style: metadataStyle),
                LayoutBuilder(
                  builder: (context, constraints) {
                    final style = switch ((preview, compact)) {
                      (true, _) => theme.textTheme.bodyMedium!.copyWith(
                        height: 22 / 14,
                      ),
                      (false, true) => theme.textTheme.bodyLarge!.copyWith(
                        fontSize: 15,
                        height: 22 / 15,
                      ),
                      _ => theme.textTheme.bodyLarge!.copyWith(height: 23 / 16),
                    };
                    final painter = TextPainter(
                      text: TextSpan(text: entry.body, style: style),
                      textDirection: Directionality.of(context),
                      textScaler: MediaQuery.textScalerOf(context),
                      maxLines: limit,
                    )..layout(maxWidth: constraints.maxWidth);
                    final overflow = painter.didExceedMaxLines;
                    painter.dispose();
                    return Column(
                      crossAxisAlignment: CrossAxisAlignment.start,
                      children: [
                        Text(
                          entry.body,
                          style: style,
                          maxLines: expanded ? null : limit,
                          overflow: expanded ? null : TextOverflow.ellipsis,
                        ),
                        if (overflow)
                          TextButton(
                            onPressed: () =>
                                setState(() => expanded = !expanded),
                            child: Text(
                              expanded
                                  ? context.strings.announcementLess
                                  : context.strings.announcementMore,
                            ),
                          ),
                      ],
                    );
                  },
                ),
                Padding(
                  padding: const EdgeInsets.only(top: 4),
                  child: Text(
                    byline,
                    style: metadataStyle?.copyWith(fontSize: 11),
                  ),
                ),
              ],
            ),
          ),
        ],
      ),
    );
    if (!widget.connectAbove && !widget.connectBelow) return row;
    final colors = theme.colorScheme;
    return Stack(
      children: [
        PositionedDirectional(
          start: (avatarSize - 1) / 2,
          width: 1,
          top: widget.connectAbove ? 0 : avatarSize / 2,
          bottom: widget.connectBelow ? 0 : null,
          height: widget.connectBelow ? null : avatarSize / 2,
          child: ColoredBox(
            key: const ValueKey('announcement timeline divider'),
            color: Color.alphaBlend(
              colors.primary.withValues(alpha: .42),
              colors.outlineVariant,
            ),
          ),
        ),
        row,
      ],
    );
  }
}
