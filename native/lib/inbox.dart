import 'dart:async';

import 'package:collection/collection.dart';
import 'package:go_router/go_router.dart';
import 'package:material_ui/material_ui.dart';
import 'package:super_sliver_list/super_sliver_list.dart';
import 'package:uuid/uuid.dart';

import 'account.dart';
import 'account_api.dart';
import 'l10n/strings.dart';
import 'site_documents.dart';
import 'message_highlights.dart';

bool _messagesVisible(BuildContext context) {
  // Announcements are the first tab. Fullscreen conversations have no tabs.
  final tabs = DefaultTabController.maybeOf(context);
  return (ModalRoute.of(context)?.isCurrent ?? true) &&
      (tabs == null || tabs.index == 1);
}

class MessageSignIn extends StatelessWidget {
  const MessageSignIn({super.key, this.padding = const EdgeInsets.all(16)});

  final EdgeInsets padding;

  @override
  Widget build(BuildContext context) {
    final strings = context.strings;
    final theme = Theme.of(context);
    return SingleChildScrollView(
      padding: padding,
      child: ConstrainedBox(
        constraints: const BoxConstraints(minHeight: 240),
        child: Center(
          child: Column(
            mainAxisSize: MainAxisSize.min,
            spacing: 10,
            children: [
              Icon(
                Icons.mail_outline,
                size: 28,
                color: theme.colorScheme.onSurfaceVariant,
              ),
              Text(
                strings.loginForMessages,
                textAlign: TextAlign.center,
                style: TextStyle(color: theme.colorScheme.onSurfaceVariant),
              ),
              FilledButton(
                onPressed: () =>
                    openSiteLink(context, signInFor('/?section=messages')),
                child: Text(strings.signIn),
              ),
            ],
          ),
        ),
      ),
    );
  }
}

String messageThreadPath(String username) =>
    '/messages/${Uri.encodeComponent(username)}';

class InboxWorkspace extends StatelessWidget {
  const InboxWorkspace({
    super.key,
    required this.highlights,
    required this.username,
    required this.onOpen,
    required this.onClose,
  });

  final MessageHighlights highlights;
  final String? username;
  final ValueChanged<String> onOpen;
  final VoidCallback onClose;

  @override
  Widget build(BuildContext context) {
    final colors = Theme.of(context).colorScheme;
    final selection = username;
    return LayoutBuilder(
      builder: (context, constraints) => Material(
        key: const ValueKey('desktop mailbox'),
        color: colors.surface,
        shape: RoundedRectangleBorder(
          borderRadius: BorderRadius.circular(10),
          side: BorderSide(color: colors.outlineVariant),
        ),
        clipBehavior: Clip.antiAlias,
        child: Row(
          crossAxisAlignment: CrossAxisAlignment.stretch,
          children: [
            SizedBox(
              width: (constraints.maxWidth * .34).clamp(164.0, 250.0),
              child: Material(
                color: colors.surfaceContainerLow,
                child: Inbox(
                  key: const ValueKey('desktop threads'),
                  highlights: highlights,
                  padding: const EdgeInsets.all(8),
                  selectedUsername: selection,
                  onOpen: onOpen,
                ),
              ),
            ),
            VerticalDivider(
              width: 1,
              thickness: 1,
              color: colors.outlineVariant,
            ),
            Expanded(
              child: SizedBox(
                key: const ValueKey('desktop conversation pane'),
                child: selection == null
                    ? Center(
                        child: Padding(
                          padding: const EdgeInsets.all(16),
                          child: Text(
                            context.strings.chooseConversation,
                            textAlign: TextAlign.center,
                            style: TextStyle(color: colors.onSurfaceVariant),
                          ),
                        ),
                      )
                    : ConversationPane(
                        key: ValueKey(selection),
                        username: selection,
                        highlights: highlights,
                        onClose: onClose,
                      ),
              ),
            ),
          ],
        ),
      ),
    );
  }
}

class Inbox extends StatefulWidget {
  const Inbox({
    super.key,
    required this.highlights,
    required this.padding,
    this.selectedUsername,
    this.onOpen,
  });

  final MessageHighlights highlights;
  AccountSession get session => highlights.session;
  final EdgeInsets padding;
  final String? selectedUsername;
  final ValueChanged<String>? onOpen;

  @override
  State<Inbox> createState() => _InboxState();
}

class _InboxState extends State<Inbox> with AutomaticKeepAliveClientMixin {
  final search = TextEditingController();
  var threads = <MessageThread>[];
  bool loading = false;
  bool failed = false;
  int? nextBefore;
  bool invalidUsername = false;
  bool _refreshAfterLoad = false;
  bool _loadedOlderThreads = false;
  bool _retryOlderThreads = false;

  @override
  bool get wantKeepAlive => true;

  @override
  void initState() {
    super.initState();
    widget.highlights.inboxRefreshRevision.addListener(_refreshIfVisible);
    widget.highlights.conversationReadRevision.addListener(_refreshIfVisible);
    load();
  }

  void _refreshIfVisible() {
    if (!_messagesVisible(context)) return;
    if (loading) {
      _refreshAfterLoad = true;
    } else {
      unawaited(load());
    }
  }

  Future<void> load({bool more = false}) async {
    if (loading || (more && nextBefore == null)) return;
    setState(() {
      loading = true;
      failed = false;
      _retryOlderThreads = more;
    });
    try {
      final page = await widget.session.api.inbox(
        before: more ? nextBefore : null,
      );
      if (!mounted) return;
      setState(() {
        final byUsername = <String, MessageThread>{
          if (more || _loadedOlderThreads)
            for (final thread in threads) thread.peer.username: thread,
          for (final thread in page.threads) thread.peer.username: thread,
        };
        threads = byUsername.values.toList()
          ..sort((a, b) => b.lastMessage.id.compareTo(a.lastMessage.id));
        if (more || !_loadedOlderThreads) nextBefore = page.nextBefore;
        _loadedOlderThreads |= more;
      });
    } catch (error) {
      if (!mounted) return;
      widget.session.expireIfUnauthorized(error);
      setState(() => failed = true);
    } finally {
      if (mounted) {
        setState(() => loading = false);
        if (_refreshAfterLoad) {
          _refreshAfterLoad = false;
          unawaited(load());
        }
      }
    }
  }

  Future<void> open(String rawUsername) async {
    final username = rawUsername
        .trim()
        .replaceFirst(RegExp(r'^@'), '')
        .toLowerCase();
    if (!AccountApi.isMessageUsername(username)) {
      setState(() => invalidUsername = true);
      return;
    }
    setState(() => invalidUsername = false);
    final show = widget.onOpen;
    if (show != null) {
      show(username);
      return;
    }
    await context.push(messageThreadPath(username));
    if (mounted) await load();
  }

  @override
  void dispose() {
    widget.highlights.inboxRefreshRevision.removeListener(_refreshIfVisible);
    widget.highlights.conversationReadRevision.removeListener(
      _refreshIfVisible,
    );
    search.dispose();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    super.build(context);
    final colors = Theme.of(context).colorScheme;
    final embedded = widget.onOpen != null;
    return RefreshIndicator(
      onRefresh: load,
      child: ListView(
        physics: const AlwaysScrollableScrollPhysics(),
        padding: widget.padding,
        children: [
          if (widget.onOpen != null)
            Padding(
              padding: const EdgeInsets.fromLTRB(8, 8, 8, 12),
              child: Semantics(
                header: true,
                child: Text(
                  context.strings.recentConversations,
                  style: Theme.of(context).textTheme.titleSmall,
                ),
              ),
            ),
          TextField(
            controller: search,
            onSubmitted: open,
            autocorrect: false,
            textInputAction: TextInputAction.search,
            style: embedded ? Theme.of(context).textTheme.bodyMedium : null,
            decoration: InputDecoration(
              isDense: embedded,
              contentPadding: embedded
                  ? const EdgeInsetsDirectional.fromSTEB(10, 8, 0, 8)
                  : null,
              border: OutlineInputBorder(
                borderRadius: BorderRadius.circular(8),
              ),
              hintText: context.strings.messageUsername,
              errorText: invalidUsername
                  ? context.strings.invalidMessageUsername
                  : null,
              prefixIcon: embedded ? null : const Icon(Icons.search),
              suffixIconConstraints: embedded
                  ? const BoxConstraints.tightFor(width: 36, height: 36)
                  : null,
              suffixIcon: IconButton(
                padding: embedded ? EdgeInsets.zero : null,
                tooltip: context.strings.openConversation,
                onPressed: () => open(search.text),
                icon: const Icon(Icons.arrow_forward, size: 18),
              ),
            ),
          ),
          const SizedBox(height: 12),
          if (threads.isEmpty && !failed && !loading)
            Center(child: Text(context.strings.noConversations)),
          for (final thread in threads)
            ListTile(
              key: ValueKey(thread.peer.username),
              contentPadding: widget.onOpen == null
                  ? null
                  : const EdgeInsets.symmetric(horizontal: 8),
              shape: RoundedRectangleBorder(
                borderRadius: BorderRadius.circular(6),
              ),
              horizontalTitleGap: embedded ? 8 : null,
              selectedColor: colors.onSurface,
              selected: widget.selectedUsername == thread.peer.username,
              selectedTileColor: Theme.of(context)
                  .colorScheme
                  .surfaceContainerHigh,
              leading: AccountAvatar(
                api: widget.session.api,
                username: thread.peer.username,
                size: widget.onOpen != null ? 36 : 40,
              ),
              title: Text(
                thread.peer.nickname,
                maxLines: 1,
                overflow: TextOverflow.ellipsis,
              ),
              isThreeLine: true,
              subtitle: Column(
                crossAxisAlignment: CrossAxisAlignment.start,
                children: [
                  Text(
                    '@${thread.peer.username}',
                    maxLines: 1,
                    overflow: TextOverflow.ellipsis,
                  ),
                  Text(
                    thread.lastMessage.body,
                    maxLines: 1,
                    overflow: TextOverflow.ellipsis,
                  ),
                ],
              ),
              trailing: thread.unread > 0
                  ? Semantics(
                      label: context.strings.messageUnread(thread.unread),
                      excludeSemantics: true,
                      child: Badge.count(
                        count: thread.unread,
                        backgroundColor: colors.primary.withValues(alpha: .16),
                        textColor: colors.primary,
                      ),
                    )
                  : widget.onOpen != null
                  ? null
                  : const Icon(Icons.chevron_right),
              onTap: () => open(thread.peer.username),
            ),
          if (loading) const Center(child: CircularProgressIndicator()),
          if (!loading && (failed || nextBefore != null))
            Center(
              child: TextButton(
                onPressed: () => load(more: failed ? _retryOlderThreads : true),
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

class ConversationPageView extends StatelessWidget {
  const ConversationPageView({
    super.key,
    required this.username,
    required this.highlights,
    this.onBack,
  });

  final String username;
  final MessageHighlights highlights;
  final VoidCallback? onBack;
  AccountSession get session => highlights.session;

  void back(BuildContext context) {
    if (context.canPop()) {
      context.pop();
    } else {
      context.go('/messages');
    }
  }

  @override
  Widget build(BuildContext context) => Scaffold(
    appBar: AppBar(
      leading: BackButton(onPressed: onBack ?? () => back(context)),
      title: Text('@$username'),
    ),
    body: ListenableBuilder(
      listenable: session,
      builder: (context, _) {
        final member = session.identity;
        return member == null
            ? const MessageSignIn()
            : ConversationPane(
                key: ValueKey(member.username),
                username: username,
                highlights: highlights,
              );
      },
    ),
  );
}

class ConversationPane extends StatefulWidget {
  const ConversationPane({
    super.key,
    required this.username,
    required this.highlights,
    this.onClose,
  });

  final String username;
  final MessageHighlights highlights;
  final VoidCallback? onClose;
  AccountSession get session => highlights.session;

  @override
  State<ConversationPane> createState() => _ConversationState();
}

class _ConversationState extends State<ConversationPane> {
  static const _center = ValueKey('message history center');
  final draft = TextEditingController();
  final _history = ScrollController(keepScrollOffset: false);
  List<DirectMessage> messages = [];
  MessagePeer? peer;
  int? nextBefore;
  int? _anchorId;
  Future<void>? _loadRequest;
  bool loading = false;
  bool _olderFailed = false;
  bool failed = false;
  bool sending = false;
  bool missing = false;
  bool sendFailed = false;
  ({String id, String body})? pending;

  bool get _embedded => widget.onClose != null;

  bool get _atLatest {
    if (!_history.hasClients) return true;
    final position = _history.position;
    return (_embedded ? position.extentAfter : position.extentBefore) < 24;
  }

  @override
  void initState() {
    super.initState();
    widget.highlights.inboxRefreshRevision.addListener(_refreshIfVisible);
    load();
  }

  void _refreshIfVisible() {
    if (!sending && _messagesVisible(context)) unawaited(load());
  }

  Future<void> load({bool older = false, bool showLatest = false}) {
    final existing = _loadRequest;
    if (existing != null) return existing;
    if (older && nextBefore == null) return Future.value();
    return _loadRequest = _load(
      older: older,
      showLatest: showLatest,
    ).whenComplete(() => _loadRequest = null);
  }

  Future<void> _load({required bool older, required bool showLatest}) async {
    final username = widget.session.identity?.username;
    setState(() {
      loading = true;
      if (older) {
        _olderFailed = false;
      } else {
        failed = false;
      }
    });
    try {
      final page = await widget.session.api.conversation(
        widget.username,
        before: older ? nextBefore : null,
      );
      if (!mounted || widget.session.identity?.username != username) return;
      final previousLatest = messages.lastOrNull?.id;
      final byId = {
        for (final message in messages) message.id: message,
        for (final message in page.messages) message.id: message,
      };
      var newerPage = page;
      while (!older &&
          previousLatest != null &&
          newerPage.messages.isNotEmpty &&
          newerPage.messages.first.id > previousLatest &&
          newerPage.nextBefore != null) {
        newerPage = await widget.session.api.conversation(
          widget.username,
          before: newerPage.nextBefore,
        );
        if (!mounted || widget.session.identity?.username != username) return;
        for (final message in newerPage.messages) {
          byId[message.id] = message;
        }
      }
      final firstLoad = messages.isEmpty;
      final sorted = byId.values.toList()..sort((a, b) => a.id.compareTo(b.id));
      final atLatest = _atLatest;
      setState(() {
        missing = false;
        peer = page.peer;
        if (older || messages.isEmpty) nextBefore = page.nextBefore;
        messages = sorted;
        _anchorId ??= (_embedded ? sorted.firstOrNull : sorted.lastOrNull)?.id;
      });
      final latestChanged = previousLatest != sorted.lastOrNull?.id;
      if (!older && (showLatest || firstLoad || (atLatest && latestChanged))) {
        WidgetsBinding.instance.addPostFrameCallback((_) {
          if (!mounted || !_history.hasClients) return;
          final position = _history.position;
          _history.jumpTo(
            _embedded ? position.maxScrollExtent : position.minScrollExtent,
          );
        });
      }
      if (!older && (atLatest || showLatest) && _messagesVisible(context)) {
        await _markLatestRead();
      }
    } catch (error) {
      if (!mounted || widget.session.identity?.username != username) return;
      widget.session.expireIfUnauthorized(error);
      setState(() {
        if (older) {
          _olderFailed = true;
        } else {
          failed = true;
        }
        missing =
            peer == null &&
            error is AccountRequestException &&
            error.status == 404;
      });
    } finally {
      if (mounted) setState(() => loading = false);
    }
  }

  Future<void> _markLatestRead() async {
    final latest = messages.lastWhereOrNull((message) => !message.outgoing);
    if (latest == null) return;
    try {
      await widget.highlights.markConversationRead(widget.username, latest.id);
    } catch (error) {
      if (mounted) widget.session.expireIfUnauthorized(error);
    }
  }

  Future<void> send() async {
    final body = draft.text.trim();
    if (sending || body.isEmpty) return;
    final id = pending?.body == body ? pending!.id : const Uuid().v4();
    pending = (id: id, body: body);
    setState(() {
      sending = true;
      sendFailed = false;
    });
    try {
      await widget.session.api.sendMessage(widget.username, body, id);
      if (!mounted) return;
      pending = null;
      draft.clear();
      await _loadRequest;
      if (mounted) await load(showLatest: true);
    } catch (error) {
      if (!mounted) return;
      widget.session.expireIfUnauthorized(error);
      setState(() => sendFailed = true);
    } finally {
      if (mounted) setState(() => sending = false);
    }
  }

  @override
  void dispose() {
    widget.highlights.inboxRefreshRevision.removeListener(_refreshIfVisible);
    draft.dispose();
    _history.dispose();
    super.dispose();
  }

  Widget _messageBubble(
    DirectMessage message,
    ThemeData theme,
    MaterialLocalizations date,
  ) {
    return Align(
      key: ValueKey(message.id),
      alignment: message.outgoing
          ? AlignmentDirectional.centerEnd
          : AlignmentDirectional.centerStart,
      child: LayoutBuilder(
        builder: (context, constraints) => ConstrainedBox(
          constraints: BoxConstraints(maxWidth: constraints.maxWidth * .9),
          child: Card.filled(
            margin: const EdgeInsets.symmetric(vertical: 4),
            shape: RoundedRectangleBorder(
              borderRadius: BorderRadius.circular(10),
            ),
            color: message.outgoing
                ? Color.alphaBlend(
                    theme.colorScheme.primary.withValues(alpha: .15),
                    theme.colorScheme.surface,
                  )
                : theme.colorScheme.surfaceContainerHighest,
            child: Padding(
              padding: const EdgeInsets.symmetric(horizontal: 12, vertical: 8),
              child: Column(
                mainAxisSize: MainAxisSize.min,
                crossAxisAlignment: CrossAxisAlignment.start,
                children: [
                  SelectableText(message.body),
                  const SizedBox(height: 4),
                  Text(
                    '${date.formatShortDate(message.sentAt)} '
                    '${date.formatTimeOfDay(TimeOfDay.fromDateTime(message.sentAt))}',
                    style: theme.textTheme.bodySmall?.copyWith(
                      fontSize: 11,
                      color: theme.colorScheme.onSurfaceVariant,
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

  Widget _buildHistory(ThemeData theme, MaterialLocalizations date) {
    final anchor = _anchorId;
    final before = <DirectMessage>[];
    final after = <DirectMessage>[];
    for (final message in messages) {
      final beforeCenter =
          anchor != null &&
          (_embedded ? message.id < anchor : message.id > anchor);
      (beforeCenter ? before : after).add(message);
    }

    // Each side grows away from the same message, preserving the viewport
    // when a page is inserted on the other side.
    final leading = (_embedded ? before.reversed : before).toList();
    final trailing = (_embedded ? after : after.reversed).toList();
    final historyControl = nextBefore != null
        ? Center(
            child: TextButton(
              onPressed: loading ? null : () => load(older: true),
              child: Text(
                _olderFailed
                    ? context.strings.messageLoadRetry
                    : context.strings.olderDirectMessages,
              ),
            ),
          )
        : null;
    final control = SliverToBoxAdapter(child: historyControl);
    return NotificationListener<ScrollEndNotification>(
      onNotification: (notification) {
        if (notification.depth == 0 && _atLatest && _messagesVisible(context)) {
          unawaited(_markLatestRead());
        }
        return false;
      },
      child: CustomScrollView(
        controller: _history,
        primary: false,
        reverse: !_embedded,
        center: _center,
        semanticChildCount: messages.length,
        slivers: [
          if (_embedded) control,
          SuperSliverList(
            delegate: SliverChildBuilderDelegate(
              (_, index) => _messageBubble(leading[index], theme, date),
              childCount: leading.length,
              semanticIndexCallback: (_, index) => leading.length - 1 - index,
            ),
          ),
          SuperSliverList(
            key: _center,
            delegate: SliverChildBuilderDelegate(
              (_, index) => _messageBubble(trailing[index], theme, date),
              childCount: trailing.length,
              semanticIndexOffset: leading.length,
            ),
          ),
          if (!_embedded) control,
        ],
      ),
    );
  }

  @override
  Widget build(BuildContext context) {
    final theme = Theme.of(context);
    final date = MaterialLocalizations.of(context);
    final embedded = widget.onClose != null;
    final person = peer;
    return SafeArea(
      top: !embedded,
      bottom: !embedded,
      child: Column(
        children: [
          if (person != null || embedded)
            ListTile(
              leading: embedded
                  ? null
                  : AccountAvatar(
                      api: widget.session.api,
                      username: widget.username,
                    ),
              title: Text(person?.nickname ?? '@${widget.username}'),
              subtitle: embedded && person != null
                  ? Text('@${person.username}')
                  : null,
              trailing: embedded
                  ? IconButton(
                      tooltip: context.strings.closeConversation,
                      onPressed: widget.onClose,
                      icon: const Icon(Icons.close, size: 18),
                    )
                  : null,
            ),
          Expanded(
            child: Padding(
              padding: const EdgeInsets.all(16),
              child: peer == null || (messages.isEmpty && nextBefore == null)
                  ? Center(
                      child: loading && peer == null
                          ? const CircularProgressIndicator()
                          : missing
                          ? Semantics(
                              liveRegion: true,
                              child: Text(context.strings.messageUserMissing),
                            )
                          : !failed
                          ? Text(context.strings.noDirectMessages)
                          : null,
                    )
                  : _buildHistory(theme, date),
            ),
          ),
          if (failed && !missing)
            TextButton(
              onPressed: loading ? null : () => load(),
              child: Text(context.strings.messageLoadRetry),
            ),
          if (sendFailed)
            Semantics(
              liveRegion: true,
              child: Text(context.strings.messageSendRetry),
            ),
          if (peer != null && !missing)
            ColoredBox(
              color: embedded
                  ? theme.colorScheme.surfaceContainerLow
                  : Colors.transparent,
              child: Padding(
                padding: const EdgeInsets.fromLTRB(12, 8, 12, 12),
                child: Row(
                  spacing: 8,
                  children: [
                    Expanded(
                      child: TextField(
                        controller: draft,
                        enabled: !sending,
                        minLines: 1,
                        maxLines: 5,
                        maxLength: 2000,
                        decoration: InputDecoration(
                          border: const OutlineInputBorder(),
                          hintText: context.strings.directMessageInput,
                          counterText: '',
                        ),
                        onChanged: (_) => setState(() {}),
                      ),
                    ),
                    IconButton.filled(
                      tooltip: context.strings.sendPrivateMessage,
                      onPressed: sending || draft.text.trim().isEmpty
                          ? null
                          : send,
                      icon: const Icon(Icons.send),
                    ),
                  ],
                ),
              ),
            ),
        ],
      ),
    );
  }
}
