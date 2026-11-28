import 'dart:convert';

import 'package:flutter_markdown_plus/flutter_markdown_plus.dart';
import 'package:go_router/go_router.dart';
import 'package:material_ui/material_ui.dart';
import 'package:url_launcher/url_launcher.dart';

import 'account_policy.dart';
import 'l10n/strings.dart';
import 'profile_social.dart';

const faqPath = '/faq.html';
const firstUsePath = '/guide.html';
const multiplayerGuidePath = '/docs/multiplayer';
const aboutPath = '/about.html';
const termsPath = '/terms.html';
const privacyPath = '/privacy.html';
const uptimePath = '/uptime';
const _progressPath = '/faq.html?view=progress';

const documentRoutes = [
  faqPath,
  firstUsePath,
  aboutPath,
  termsPath,
  privacyPath,
  '/multiplayer.html',
];
const _cooperative = ['coop', 'th06', 'th07', 'th08', 'th10', 'th11', 'th15'];
const _versus = ['th09', 'th19'];
final _guidePages = [
  multiplayerGuidePath,
  for (final topic in [..._cooperative, ..._versus])
    '$multiplayerGuidePath/$topic',
];
final _documentOrder = [firstUsePath, faqPath, ..._guidePages];
const _policyAssets = {
  termsPath: AccountDocument.terms,
  privacyPath: AccountDocument.privacy,
};

Uri _documentAddress(Uri address) {
  final path = address.path;
  if (path == '/multiplayer.html') {
    return address.replace(path: multiplayerGuidePath);
  }
  return path.length > 1 && path.endsWith('/')
      ? address.replace(path: path.substring(0, path.length - 1))
      : address;
}

Locale? sitePageLocale(Uri address) =>
    switch (address.queryParameters['lang']) {
      'en' => const Locale('en'),
      'ja' => const Locale('ja'),
      'zh-CN' => const Locale('zh'),
      _ => null,
    };

Uri _withDocumentLocale(Uri target, Locale? locale) =>
    locale == null || target.queryParameters.containsKey('lang')
    ? target
    : target.replace(
        queryParameters: {
          ...target.queryParametersAll,
          'lang': bundledLanguage(locale),
        },
      );

bool _isDocument(Uri url) {
  if (documentRoutes.contains(url.path) || url.path == multiplayerGuidePath) {
    return true;
  }
  final topic = url.pathSegments.lastOrNull;
  return (_cooperative.contains(topic) || _versus.contains(topic)) &&
      url.path == '$multiplayerGuidePath/$topic';
}

Future<void> openSiteLink(
  BuildContext context,
  String? href, {
  String from = '/',
}) async {
  if (href == null || href.trim().isEmpty) return;
  final link = Uri.tryParse(href);
  if (link == null) return;
  final origin = Uri.https('touhouonline.com').resolve(from);
  final url = origin.resolveUri(link);
  if (url.userInfo.isNotEmpty ||
      !['http', 'https', 'mailto'].contains(url.scheme)) {
    return;
  }
  if (url.host == 'touhouonline.com') {
    if (url.path == '/assets/donation.webp') {
      await showSupportDialog(context);
      return;
    }
    final address = _documentAddress(url);
    final path = address.path == '$uptimePath/index.html'
        ? uptimePath
        : address.path;
    if (_isDocument(address) || path == uptimePath) {
      final internal = _withDocumentLocale(
        Uri(
          path: path,
          query: address.hasQuery ? address.query : null,
          fragment: address.hasFragment ? address.fragment : null,
        ),
        sitePageLocale(origin),
      ).toString();
      await context.push(internal);
      return;
    }
    if (url.path == '/' || url.path == '/lobby.html') {
      context.go(url.path == '/' ? '/' : '/lobby');
      return;
    }
  }
  try {
    if (await launchUrl(url, mode: LaunchMode.externalApplication)) return;
  } on Exception {
    // Some devices do not have an application for the link scheme.
  }
  if (context.mounted) {
    ScaffoldMessenger.of(context)
        .showSnackBar(SnackBar(content: Text(context.strings.cannotOpenLink)));
  }
}

Future<void> showSupportDialog(BuildContext context) => showDialog<void>(
  context: context,
  builder: (dialog) => AlertDialog(
    title: Text(dialog.strings.donationTitle),
    content: SingleChildScrollView(
      child: Column(
        mainAxisSize: MainAxisSize.min,
        spacing: 12,
        children: [
          Text(dialog.strings.donationScan),
          Image.asset(
            'assets/donation.webp',
            width: 280,
            fit: BoxFit.contain,
            semanticLabel: dialog.strings.donationImage,
          ),
        ],
      ),
    ),
    actions: [
      TextButton(
        onPressed: () => Navigator.of(dialog).pop(),
        child: Text(dialog.strings.close),
      ),
    ],
  ),
);

MarkdownStyleSheet siteMarkdownStyle(BuildContext context) {
  final theme = Theme.of(context);
  final colors = theme.colorScheme;
  final text = theme.textTheme;
  final body = (text.bodyMedium ?? const TextStyle(fontSize: 14)).copyWith(
    color: colors.onSurface,
    height: 1.55,
  );
  return MarkdownStyleSheet(
    p: body,
    a: body.copyWith(
      color: colors.primary,
      decoration: TextDecoration.underline,
    ),
    h1: text.headlineSmall?.copyWith(color: colors.onSurface),
    h2: text.titleLarge?.copyWith(color: colors.onSurface),
    h3: text.titleMedium?.copyWith(color: colors.onSurface),
    h4: text.titleSmall?.copyWith(color: colors.onSurface),
    h5: text.bodyLarge?.copyWith(color: colors.onSurface),
    h6: text.bodyLarge?.copyWith(color: colors.onSurface),
    code: body.copyWith(
      fontFamily: 'monospace',
      backgroundColor: colors.surfaceContainerHighest,
    ),
    strong: const TextStyle(fontWeight: FontWeight.w700),
    em: const TextStyle(fontStyle: FontStyle.italic),
    listBullet: body,
    listIndent: 17,
    blockSpacing: 12,
    blockquote: body.copyWith(color: colors.onSurfaceVariant),
    blockquoteDecoration: BoxDecoration(
      color: colors.surfaceContainerHighest,
      border: BorderDirectional(
        start: BorderSide(color: colors.primary, width: 3),
      ),
    ),
    tableHead: body.copyWith(fontWeight: FontWeight.w700),
    tableBody: body,
    tableHeadAlign: TextAlign.start,
    tableBorder: TableBorder.all(color: colors.outlineVariant),
    tableHeadCellsDecoration: BoxDecoration(
      color: colors.surfaceContainerHighest,
    ),
    tableCellsPadding: const EdgeInsets.symmetric(horizontal: 10, vertical: 8),
    tableHeadCellsPadding: const EdgeInsets.symmetric(
      horizontal: 10,
      vertical: 8,
    ),
    horizontalRuleDecoration: BoxDecoration(
      border: Border(top: BorderSide(color: colors.outlineVariant)),
    ),
  );
}

typedef _Block = ({
  String text,
  int columns,
  List<List<String>> rows,
  String? notice,
});
typedef _Section = ({String title, String id, int depth, List<_Block> blocks});
typedef _Revision = ({String author, String commit, DateTime date});
typedef _Document = ({
  String title,
  List<_Section> sections,
  _Revision? revision,
  bool pending,
});

class _Documents {
  const _Documents(this.entries);
  final Map<String, _Document> entries;

  static Future<_Documents> read(
    AssetBundle bundle,
    Uri address,
    Locale locale,
    LauncherStrings strings,
  ) async {
    final json = jsonDecode(
      await bundle.loadString(
        'assets/documents/site.${bundledLanguage(locale)}.json',
      ),
    ) as Map<String, dynamic>;
    final docs = <String, _Document>{
      for (final MapEntry(key: path, value: data) in json.entries)
        path: _parse(data as Map<String, dynamic>),
    };
    final policy = _policyAssets[address.path];
    if (policy != null) {
      final text = await policy.load(bundle, locale);
      final base = docs[address.path];
      docs[address.path] = (
        title: policy.title(strings),
        revision: base?.revision,
        pending: base?.pending ?? false,
        sections: [
          (
            title: '',
            id: '',
            depth: 2,
            blocks: [(text: text, columns: 0, rows: const [], notice: null)],
          ),
        ],
      );
    }
    return _Documents(docs);
  }

  static _Revision? _revision(Map<String, dynamic>? value) {
    final date = DateTime.tryParse(value?['date'] as String? ?? '');
    final commit = value?['commit'] as String?;
    final author = value?['author'] as String?;
    if (date == null ||
        commit == null ||
        author == null ||
        !RegExp(r'^[a-f0-9]{40}$').hasMatch(commit)) {
      return null;
    }
    return (author: author, commit: commit, date: date);
  }

  static _Document _parse(Map<String, dynamic> data) => (
    title: data['title'] as String,
    revision: _revision(data['revision'] as Map<String, dynamic>?),
    pending: (data['revision'] as Map<String, dynamic>?)?['pending'] == true,
    sections: [
      for (final section
          in (data['sections'] as List).cast<Map<String, dynamic>>())
        (
          title: section['title'] as String,
          id: section['id'] as String,
          depth: section['depth'] as int,
          blocks: [
            for (final block
                in (section['blocks'] as List).cast<Map<String, dynamic>>())
              (
                text: block['text'] as String? ?? '',
                columns: block['columns'] as int,
                notice: block['notice'] as String?,
                rows: [
                  for (final row in block['rows'] as List? ?? const [])
                    (row as List).cast<String>(),
                ],
              ),
          ],
        ),
    ],
  );
}

class SiteDocumentPage extends StatefulWidget {
  const SiteDocumentPage({super.key, required this.address});
  final Uri address;

  @override
  State<SiteDocumentPage> createState() => _SiteDocumentPageState();
}

class _SiteDocumentPageState extends State<SiteDocumentPage> {
  Map<String, GlobalKey> _sections = {};
  late Future<_Documents> _document;
  (AssetBundle, Locale)? _source;
  late final _scroll = ScrollController();
  bool _initialAnchor = true;

  Uri get _address => _documentAddress(widget.address);
  String get _current =>
      _address.path == faqPath && _address.queryParameters['view'] == 'progress'
      ? _progressPath
      : _address.path;
  bool get _guideOpen => _current.startsWith(multiplayerGuidePath);

  Locale get _locale =>
      sitePageLocale(_address) ?? Localizations.localeOf(context);

  LauncherStrings get _strings => lookupLauncherStrings(_locale);

  Future<_Documents> _load() => _Documents.read(
    DefaultAssetBundle.of(context),
    _address,
    _locale,
    _strings,
  );

  @override
  void didChangeDependencies() {
    super.didChangeDependencies();
    final source = (DefaultAssetBundle.of(context), _locale);
    if (source == _source) return;
    _source = source;
    _document = _load();
  }

  @override
  void dispose() {
    _scroll.dispose();
    super.dispose();
  }

  void _jump(String id, {bool instant = false}) {
    final target = _sections[id]?.currentContext;
    if (target == null) return;
    Scrollable.ensureVisible(
      target,
      duration: instant || MediaQuery.disableAnimationsOf(context)
          ? Duration.zero
          : const Duration(milliseconds: 180),
      curve: Curves.easeOut,
    );
  }

  void _link(String? href) {
    final path = href == null ? null : Uri.tryParse(href);
    if (path != null && path.path.isEmpty && path.hasFragment) {
      _jump(path.fragment);
      return;
    }
    openSiteLink(context, href, from: _address.toString());
  }

  Future<void> _navigate(String path) async {
    if (path == _current) return;
    final target = _withDocumentLocale(
      Uri.parse(path),
      sitePageLocale(_address),
    );
    await context.push(target.toString());
  }

  @override
  Widget build(BuildContext context) => FutureBuilder<_Documents>(
    future: _document,
    builder: (context, state) {
      final catalog = state.data;
      final document = catalog?.entries[_current];
      final outline =
          document?.sections
              .where((section) => section.title.isNotEmpty)
              .toList() ??
          const <_Section>[];
      final labels = Theme.of(context).textTheme;
      final wide = MediaQuery.sizeOf(context).width >= 1000;
      return Scaffold(
        appBar: AppBar(
          leading: BackButton(onPressed: () => profileBack(context, '/')),
          title: Text(
            document?.title ?? _strings.help,
            maxLines: 1,
            overflow: TextOverflow.ellipsis,
          ),
          actions: [
            if (!wide && _guideOpen && catalog != null)
              PopupMenuButton<String>(
                tooltip: _strings.guideNavigation,
                icon: const Icon(Icons.library_books_outlined),
                onSelected: _navigate,
                itemBuilder: (_) => [
                  for (final path in _guidePages)
                    PopupMenuItem(
                      value: path,
                      child: Text(catalog.entries[path]!.title),
                    ),
                ],
              ),
            if (outline.isNotEmpty)
              PopupMenuButton<String>(
                tooltip: _strings.documentToc,
                icon: const Icon(Icons.toc),
                onSelected: _jump,
                itemBuilder: (_) => [
                  for (final section in outline)
                    PopupMenuItem(
                      value: section.id,
                      child: Text(section.title),
                    ),
                ],
              ),
          ],
        ),
        body: SafeArea(
          child: state.hasError
              ? Center(
                  child: OutlinedButton(
                    onPressed: () => setState(() {
                      _document = _load();
                    }),
                    child: Text(_strings.retryDocument),
                  ),
                )
              : state.connectionState != ConnectionState.done
              ? const Center(child: CircularProgressIndicator())
              : document == null
              ? Center(child: Text(_strings.documentMissing))
              : Row(
                  children: [
                    if (wide && catalog != null) ...[
                      SizedBox(
                        width: 248,
                        child: _DocumentNavigation(
                          catalog: catalog,
                          current: _current,
                          strings: _strings,
                          open: _navigate,
                        ),
                      ),
                      const VerticalDivider(width: 1),
                    ],
                    Expanded(child: _article(document, catalog!, labels)),
                  ],
                ),
        ),
      );
    },
  );

  Widget _article(_Document document, _Documents catalog, TextTheme labels) {
    _sections = {
      for (final section in document.sections)
        section.id: _sections[section.id] ?? GlobalKey(),
    };
    if (_initialAnchor && _address.hasFragment) {
      _initialAnchor = false;
      WidgetsBinding.instance.addPostFrameCallback((_) {
        if (mounted) _jump(_address.fragment, instant: true);
      });
    }
    return SingleChildScrollView(
      controller: _scroll,
      padding: const EdgeInsets.fromLTRB(16, 22, 16, 32),
      child: Center(
        child: ConstrainedBox(
          constraints: const BoxConstraints(maxWidth: 780),
          child: Column(
            crossAxisAlignment: CrossAxisAlignment.stretch,
            spacing: 18,
            children: [
              for (final section in document.sections)
                Column(
                  key: _sections[section.id],
                  crossAxisAlignment: CrossAxisAlignment.stretch,
                  spacing: 10,
                  children: [
                    if (section.title.isNotEmpty)
                      Semantics(
                        header: true,
                        child: Text(
                          section.title,
                          style: section.depth > 2
                              ? labels.titleMedium
                              : labels.titleLarge,
                        ),
                      ),
                    for (final block in section.blocks) _markdown(block),
                  ],
                ),
              if (document.pending)
                Text(_strings.documentUncommitted)
              else if (document.revision case final _Revision revision)
                _documentRevision(revision),
              if (_documentOrder.contains(_current))
                _documentNeighbours(catalog),
            ],
          ),
        ),
      ),
    );
  }

  Widget _documentRevision(_Revision revision) {
    final day = revision.date.toUtc().add(const Duration(hours: 8));
    final formatted = day.toIso8601String().split('T').first;
    final link =
        'https://github.com/YomotsuHisami/eagler-touhou/commit/${revision.commit}';
    return Wrap(
      crossAxisAlignment: WrapCrossAlignment.center,
      spacing: 4,
      children: [
        Text(_strings.documentModified),
        InkWell(
          onTap: () => _link(link),
          child: Text(
            revision.author,
            style: TextStyle(
              decoration: TextDecoration.underline,
              color: Theme.of(context).colorScheme.primary,
            ),
          ),
        ),
        Text('| $formatted'),
      ],
    );
  }

  Widget _documentNeighbours(_Documents catalog) {
    final index = _documentOrder.indexOf(_current);
    final links = [
      if (index > 0)
        (
          _documentOrder[index - 1],
          _strings.documentPrevious,
          Icons.arrow_back,
        ),
      if (index + 1 < _documentOrder.length)
        (_documentOrder[index + 1], _strings.documentNext, Icons.arrow_forward),
    ];
    return LayoutBuilder(
      builder: (context, constraints) {
        final stacked = constraints.maxWidth < 540;
        return Flex(
          direction: stacked ? Axis.vertical : Axis.horizontal,
          crossAxisAlignment: stacked
              ? CrossAxisAlignment.stretch
              : CrossAxisAlignment.start,
          spacing: 8,
          children: [
            for (final (path, label, icon) in links)
              Flexible(
                flex: stacked ? 0 : 1,
                child: Card(
                  margin: EdgeInsets.zero,
                  child: TextButton.icon(
                    onPressed: () => _navigate(path),
                    icon: Icon(icon),
                    label: Column(
                      mainAxisSize: MainAxisSize.min,
                      crossAxisAlignment: CrossAxisAlignment.start,
                      children: [
                        Text(
                          label,
                          style: Theme.of(context).textTheme.labelSmall,
                        ),
                        Text(catalog.entries[path]!.title),
                      ],
                    ),
                  ),
                ),
              ),
          ],
        );
      },
    );
  }

  Widget _markdown(_Block block) {
    if (block.rows.isNotEmpty) return _compactTable(block.rows);
    final markdown = MarkdownBody(
      data: block.text,
      selectable: true,
      styleSheet: siteMarkdownStyle(context),
      onTapLink: (_, href, _) => _link(href),
    );
    if (block.notice != null) {
      final warning = block.notice == 'WARNING';
      final colors = Theme.of(context).colorScheme;
      return DecoratedBox(
        decoration: BoxDecoration(
          color: warning ? colors.errorContainer : colors.surfaceContainer,
          border: Border.all(
            color: warning ? colors.error : colors.outlineVariant,
          ),
          borderRadius: BorderRadius.circular(8),
        ),
        child: Padding(
          padding: const EdgeInsets.all(12),
          child: Row(
            crossAxisAlignment: CrossAxisAlignment.start,
            spacing: 8,
            children: [
              Icon(
                warning ? Icons.warning_amber : Icons.info_outline,
                semanticLabel: warning
                    ? _strings.documentWarning
                    : _strings.documentNote,
                color: warning ? colors.onErrorContainer : colors.primary,
                size: 18,
              ),
              Expanded(child: markdown),
            ],
          ),
        ),
      );
    }
    if (block.columns < 3) return markdown;
    return LayoutBuilder(
      builder: (context, box) => SingleChildScrollView(
        primary: false,
        scrollDirection: Axis.horizontal,
        child: SizedBox(
          width: (block.columns * 124.0).clamp(box.maxWidth, double.infinity),
          child: markdown,
        ),
      ),
    );
  }

  Widget _compactTable(List<List<String>> rows) {
    final scheme = Theme.of(context).colorScheme;
    final style = siteMarkdownStyle(context);
    return Table(
      border: TableBorder.all(color: scheme.outlineVariant),
      columnWidths: const {0: FractionColumnWidth(.32), 1: FlexColumnWidth()},
      defaultVerticalAlignment: TableCellVerticalAlignment.middle,
      children: [
        for (final (index, row) in rows.indexed)
          TableRow(
            decoration: index == 0
                ? BoxDecoration(color: scheme.surfaceContainerHighest)
                : null,
            children: [
              for (final cell in row)
                Padding(
                  padding: const EdgeInsets.symmetric(
                    horizontal: 10,
                    vertical: 8,
                  ),
                  child: index == 0
                      ? Text(cell, style: style.tableHead)
                      : MarkdownBody(
                          data: cell,
                          selectable: true,
                          styleSheet: style,
                          onTapLink: (_, href, _) => _link(href),
                        ),
                ),
            ],
          ),
      ],
    );
  }
}

class _DocumentNavigation extends StatelessWidget {
  const _DocumentNavigation({
    required this.catalog,
    required this.current,
    required this.strings,
    required this.open,
  });
  final _Documents catalog;
  final String current;
  final LauncherStrings strings;
  final ValueChanged<String> open;

  @override
  Widget build(BuildContext context) {
    final titles = Theme.of(context).textTheme;
    Widget heading(String label) => Padding(
      padding: const EdgeInsets.fromLTRB(12, 14, 12, 4),
      child: Text(label, style: titles.titleSmall),
    );
    Widget item(String path, [String? name, bool indent = false]) => ListTile(
      contentPadding: EdgeInsetsDirectional.only(
        start: indent ? 22 : 12,
        end: 12,
      ),
      title: Text(
        name ?? catalog.entries[path]!.title,
        maxLines: 1,
        overflow: TextOverflow.ellipsis,
      ),
      dense: true,
      selected: path == current,
      onTap: () => open(path),
    );
    return ListView(
      padding: const EdgeInsets.symmetric(horizontal: 8, vertical: 16),
      children: [
        heading(strings.help),
        item(firstUsePath),
        item(faqPath),
        item(multiplayerGuidePath),
        for (final topic in [..._cooperative, ..._versus])
          item('$multiplayerGuidePath/$topic', null, true),
        heading(strings.about),
        item(aboutPath),
        item(termsPath, strings.terms),
        item(privacyPath, strings.privacy),
      ],
    );
  }
}
