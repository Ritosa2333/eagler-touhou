import 'package:material_ui/material_ui.dart';

import 'l10n/strings.dart';
import 'launcher_select.dart';

Map<String, String> _inputActionLabels(LauncherStrings strings) => {
  'up': strings.inputUp,
  'down': strings.inputDown,
  'left': strings.inputLeft,
  'right': strings.inputRight,
  'shoot': strings.inputShoot,
  'bomb': strings.inputBomb,
  'focus': strings.inputFocus,
  'action': strings.inputSpecial,
  'pause': strings.inputPause,
  'confirm': strings.inputConfirm,
  'skip': strings.inputSkip,
  'restart': strings.inputRestart,
};

Map<String, String> localDuelActions(LauncherStrings strings) {
  final inputActionLabels = _inputActionLabels(strings);
  return {
    'shoot': inputActionLabels['shoot']!,
    'bomb': strings.inputQuick,
    'focus': strings.inputDuelFocus,
    'pause': strings.inputDuelPause,
    for (final action in const [
      'up',
      'down',
      'left',
      'right',
      'skip',
      'confirm',
      'restart',
    ])
      action: inputActionLabels[action]!,
  };
}

Map<String, String> inputActions(
  LauncherStrings strings,
  String scope, {
  bool keyboard = false,
}) {
  final inputActionLabels = _inputActionLabels(strings);
  if (scope == 'th19') {
    return {
      for (final action in const [
        'up',
        'down',
        'left',
        'right',
        'shoot',
        'action',
        'bomb',
        'focus',
        'pause',
        'confirm',
        'skip',
      ])
        action: action == 'action'
            ? strings.inputCharge
            : inputActionLabels[action]!,
    };
  }
  if (scope != 'th075' || !keyboard) return inputActionLabels;
  return {
    ...inputActionLabels,
    'shoot': strings.inputLightAttack,
    'bomb': strings.inputHeavyAttack,
    'action': strings.inputDanmaku,
    'focus': strings.inputDash,
  }..remove('confirm');
}

class InputBindingEntry {
  const InputBindingEntry({
    required this.bindingKey,
    required this.action,
    required this.keys,
    required this.onBind,
    required this.onReset,
    required this.resetLabel,
    this.active = false,
  });

  final String bindingKey;
  final String action;
  final String keys;
  final VoidCallback? onBind;
  final VoidCallback? onReset;
  final String resetLabel;
  final bool active;

  List<Widget> _cells(BuildContext context) => [
    Text(action, style: LauncherControl.label(context)),
    Semantics(
      label: context.strings.inputChangeAction(action),
      child: TextButton(
        key: ValueKey(bindingKey),
        style: TextButton.styleFrom(
          alignment: AlignmentDirectional.centerStart,
          textStyle: LauncherControl.label(context),
          backgroundColor: active
              ? Theme.of(context).colorScheme.primaryContainer
              : null,
          foregroundColor: active
              ? Theme.of(context).colorScheme.onPrimaryContainer
              : null,
        ),
        onPressed: onBind,
        child: Text(keys.isEmpty ? context.strings.inputUnbound : keys),
      ),
    ),
    IconButton(
      tooltip: '$resetLabel | $action',
      onPressed: onReset,
      icon: const Icon(Icons.restore),
    ),
  ];
}

class InputBindingTable extends StatelessWidget {
  const InputBindingTable({
    super.key,
    required this.entries,
    this.showHeading = false,
  });

  final List<InputBindingEntry> entries;
  final bool showHeading;

  @override
  Widget build(BuildContext context) => LayoutBuilder(
    builder: (context, bounds) {
      final labelLimit =
          ((bounds.maxWidth - kMinInteractiveDimension - 16) * .55).clamp(
            0.0,
            double.infinity,
          );
      TableRow row(List<Widget> cells) => TableRow(
        children: [
          for (final (index, cell) in cells.indexed)
            Padding(
              padding: EdgeInsetsDirectional.fromSTEB(
                0,
                4,
                index == 2 ? 0 : 8,
                4,
              ),
              child: index == 0
                  ? ConstrainedBox(
                      constraints: BoxConstraints(
                        minWidth: labelLimit.clamp(0.0, 82.0),
                        maxWidth: labelLimit,
                      ),
                      child: cell,
                    )
                  : cell,
            ),
        ],
      );
      return Table(
        columnWidths: const {
          0: IntrinsicColumnWidth(),
          1: FlexColumnWidth(),
          2: FixedColumnWidth(kMinInteractiveDimension),
        },
        defaultVerticalAlignment: TableCellVerticalAlignment.middle,
        children: [
          if (showHeading)
            row([
              Text(
                context.strings.inputActionColumn,
                style: LauncherControl.label(context),
              ),
              Text(
                context.strings.inputBindingColumn,
                style: LauncherControl.label(context),
              ),
              const SizedBox.shrink(),
            ]),
          for (final entry in entries) row(entry._cells(context)),
        ],
      );
    },
  );
}
