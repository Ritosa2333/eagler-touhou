import 'package:material_ui/material_ui.dart';

import 'l10n/strings.dart';
import 'launcher_select.dart';
import 'modal_keyboard_dismiss.dart';

class InputSettingsFrame extends StatelessWidget {
  const InputSettingsFrame({
    super.key,
    required this.title,
    required this.saving,
    required this.body,
    this.controls,
  });

  final String title;
  final bool saving;
  final Widget body;
  final Widget? controls;

  @override
  Widget build(BuildContext context) => ModalKeyboardDismiss(
    child: PopScope(
      canPop: !saving,
      child: Dialog(
        insetPadding: const EdgeInsets.symmetric(horizontal: 12, vertical: 16),
        constraints: const BoxConstraints(maxWidth: 560, maxHeight: 608),
        child: SizedBox(
          width: 560,
          height: 608,
          child: LayoutBuilder(
            builder: (context, bounds) {
              final header = <Widget>[
                Padding(
                  padding: const EdgeInsetsDirectional.fromSTEB(16, 8, 8, 8),
                  child: Row(
                    children: [
                      Expanded(
                        child: Semantics(
                          namesRoute: true,
                          header: true,
                          child: Text(
                            title,
                            style: Theme.of(context).textTheme.titleLarge,
                          ),
                        ),
                      ),
                      TextButton(
                        onPressed: saving
                            ? null
                            : () => Navigator.of(context).pop(),
                        child: Text(context.strings.inputDone),
                      ),
                    ],
                  ),
                ),
                if (controls case final Widget toolbar)
                  Padding(
                    padding: const EdgeInsets.fromLTRB(16, 0, 16, 12),
                    child: toolbar,
                  ),
                const Divider(height: 1),
              ];
              final content = Padding(
                padding: const EdgeInsets.all(16),
                child: body,
              );
              if (bounds.maxHeight < 220) {
                return SingleChildScrollView(
                  key: body.key,
                  child: Column(
                    crossAxisAlignment: CrossAxisAlignment.stretch,
                    children: [...header, content],
                  ),
                );
              }
              return Column(
                crossAxisAlignment: CrossAxisAlignment.stretch,
                children: [
                  ...header,
                  Expanded(
                    child: SingleChildScrollView(key: body.key, child: content),
                  ),
                ],
              );
            },
          ),
        ),
      ),
    ),
  );
}

class InputSettingSelect<T extends Object> extends StatelessWidget {
  const InputSettingSelect({
    super.key,
    required this.label,
    required this.value,
    required this.options,
    required this.onChanged,
  });

  final String label;
  final T value;
  final Map<T, String> options;
  final ValueChanged<T>? onChanged;

  @override
  Widget build(BuildContext context) {
    final title = Text(label, style: LauncherControl.label(context));
    final select = LauncherSelect<T>(
      label: label,
      value: value,
      labels: options,
      onChanged: onChanged == null ? null : (next) => onChanged!(next!),
      expand: true,
    );
    return LayoutBuilder(
      builder: (context, bounds) {
        final labelWidth = LauncherControl.labelWidth(
          context,
          label,
        ).ceilToDouble().clamp(92.0, double.infinity);
        if (bounds.maxWidth >= labelWidth + 140) {
          return Row(
            spacing: 8,
            children: [
              SizedBox(width: labelWidth, child: title),
              Expanded(child: select),
            ],
          );
        }
        return Column(
          crossAxisAlignment: CrossAxisAlignment.stretch,
          spacing: 4,
          children: [title, select],
        );
      },
    );
  }
}
