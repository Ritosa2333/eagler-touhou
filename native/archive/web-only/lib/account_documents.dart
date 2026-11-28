import 'package:flutter_markdown_plus/flutter_markdown_plus.dart';
import 'package:material_ui/material_ui.dart';

import 'account_policy.dart';
import 'l10n/strings.dart';
import 'site_documents.dart';

export 'account_policy.dart';

Future<bool> openAccountDocument(
  BuildContext context,
  AccountDocument document, {
  bool consent = false,
}) async {
  final result = await showDialog<bool>(
    context: context,
    builder: (_) =>
        _AccountDocumentDialog(document: document, consent: consent),
  );
  return result == true;
}

class _AccountDocumentDialog extends StatefulWidget {
  const _AccountDocumentDialog({required this.document, required this.consent});

  final AccountDocument document;
  final bool consent;

  @override
  State<_AccountDocumentDialog> createState() => _AccountDocumentDialogState();
}

class _AccountDocumentDialogState extends State<_AccountDocumentDialog> {
  late Future<String> _document = _load();

  Future<String> _load() => widget.document.load(
    DefaultAssetBundle.of(context),
    Localizations.localeOf(context),
  );

  @override
  Widget build(BuildContext context) => AlertDialog(
    insetPadding: const EdgeInsets.symmetric(horizontal: 16, vertical: 18),
    title: Text(widget.document.title(context.strings)),
    content: SizedBox(
      width: 620,
      height: 420,
      child: FutureBuilder(
        future: _document,
        builder: (context, snapshot) {
          if (snapshot.hasError) {
            return Center(
              child: TextButton(
                onPressed: () => setState(() => _document = _load()),
                child: Text(context.strings.retryDocument),
              ),
            );
          }
          final text = snapshot.data;
          return text == null
              ? const Center(child: CircularProgressIndicator())
              : Markdown(
                  data: text,
                  padding: EdgeInsets.zero,
                  styleSheet: siteMarkdownStyle(context),
                  onTapLink: (_, href, _) => openSiteLink(context, href),
                );
        },
      ),
    ),
    actions: [
      TextButton(
        onPressed: () => Navigator.of(context).pop(false),
        child: Text(
          widget.consent ? context.strings.cancel : context.strings.close,
        ),
      ),
      if (widget.consent)
        FutureBuilder(
          future: _document,
          builder: (context, snapshot) => FilledButton(
            onPressed: snapshot.hasData
                ? () => Navigator.of(context).pop(true)
                : null,
            child: Text(context.strings.termsAgree),
          ),
        ),
    ],
  );
}

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
