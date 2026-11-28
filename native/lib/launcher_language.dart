import 'package:material_ui/material_ui.dart';

import 'catalog.dart';
import 'l10n/strings.dart';
import 'launcher_select.dart';

Future<void> showLauncherLanguage(
  BuildContext context,
  LauncherPreferences preferences,
) => showDialog<void>(
  context: context,
  animationStyle: LauncherControl.overlayMotion(context),
  builder: (dialog) => AlertDialog(
    title: Text(dialog.strings.language),
    content: Align(
      alignment: AlignmentDirectional.centerStart,
      heightFactor: 1,
      child: _LanguageSelector(preferences: preferences),
    ),
    actions: [
      TextButton(
        onPressed: () => Navigator.of(dialog).pop(),
        child: Text(dialog.strings.close),
      ),
    ],
  ),
);

class LauncherLanguageRow extends StatelessWidget {
  const LauncherLanguageRow({
    super.key,
    required this.preferences,
    this.contentPadding,
  });

  final LauncherPreferences preferences;
  final EdgeInsetsGeometry? contentPadding;

  @override
  Widget build(BuildContext context) => LauncherSettingRow(
    contentPadding: contentPadding,
    label: context.strings.language,
    controlWidth:
        LauncherControl.labelWidth(context, preferences.language.label) + 44,
    control: _LanguageSelector(preferences: preferences),
  );
}

class _LanguageSelector extends StatefulWidget {
  const _LanguageSelector({required this.preferences});

  final LauncherPreferences preferences;

  @override
  State<_LanguageSelector> createState() => _LanguageSelectorState();
}

class _LanguageSelectorState extends State<_LanguageSelector> {
  bool _saving = false;

  Future<void> _select(LauncherLanguage? language) async {
    if (_saving ||
        language == null ||
        language == widget.preferences.language) {
      return;
    }
    setState(() => _saving = true);
    try {
      await widget.preferences.setLanguage(language);
    } on Exception {
      if (!mounted) return;
      ScaffoldMessenger.of(
        context,
      ).showSnackBar(SnackBar(content: Text(context.strings.selectionFailed)));
    } finally {
      if (mounted) setState(() => _saving = false);
    }
  }

  @override
  Widget build(BuildContext context) => ListenableBuilder(
    listenable: widget.preferences,
    builder: (context, _) => LauncherSelect(
      label: context.strings.language,
      value: widget.preferences.language,
      onChanged: _saving ? null : _select,
      labels: {
        for (final language in LauncherLanguage.values)
          language: language.label,
      },
    ),
  );
}
