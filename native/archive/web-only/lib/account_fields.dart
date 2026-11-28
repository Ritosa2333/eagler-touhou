import 'package:flutter/services.dart';
import 'package:material_ui/material_ui.dart';

import 'l10n/strings.dart';

final _accountEmailPattern = RegExp(r'^[^\s@]+@[^\s@]+\.[^\s@]+$');

bool validAccountEmail(String? value) =>
    _accountEmailPattern.hasMatch(value?.trim() ?? '');

class AccountField extends StatefulWidget {
  const AccountField({
    super.key,
    required this.label,
    required this.controller,
    required this.enabled,
    required this.validator,
    this.focusNode,
    this.password = false,
    this.help,
    this.hint,
    this.autofill,
    this.keyboard,
    this.formatters,
    this.onSubmitted,
    this.onChanged,
    this.accessory,
    this.inputAccessory,
    this.labelWidth,
    this.inputKey,
    this.maxLength,
    this.minLines,
    this.maxLines = 1,
    this.helperText,
    this.showCounter = false,
  });

  final String label;
  final TextEditingController controller;
  final bool enabled;
  final FormFieldValidator<String> validator;
  final FocusNode? focusNode;
  final bool password;
  final String? help;
  final String? hint;
  final String? autofill;
  final TextInputType? keyboard;
  final List<TextInputFormatter>? formatters;
  final ValueChanged<String>? onSubmitted;
  final ValueChanged<String>? onChanged;
  final Widget? accessory;
  final Widget? inputAccessory;
  final double? labelWidth;
  final Key? inputKey;
  final int? maxLength;
  final int? minLines;
  final int maxLines;
  final String? helperText;
  final bool showCounter;

  @override
  State<AccountField> createState() => _AccountFieldState();
}

class _AccountFieldState extends State<AccountField> {
  bool _showPassword = false;

  @override
  Widget build(BuildContext context) {
    final inline =
        widget.labelWidth != null &&
        MediaQuery.textScalerOf(context).scale(14) <= 18.2;
    return Flex(
      direction: inline ? Axis.horizontal : Axis.vertical,
      crossAxisAlignment: inline
          ? CrossAxisAlignment.start
          : CrossAxisAlignment.stretch,
      spacing: inline ? 20 : 8,
      children: [
        SizedBox(
          width: inline ? widget.labelWidth : null,
          child: Padding(
            padding: EdgeInsets.only(top: inline ? 10 : 0),
            child: Row(
              children: [
                Flexible(
                  flex: 2,
                  fit: widget.accessory == null ? FlexFit.loose : FlexFit.tight,
                  child: Text(
                    widget.label,
                    style: widget.labelWidth == null
                        ? null
                        : Theme.of(context).textTheme.bodyMedium?.copyWith(
                            color: Theme.of(context)
                                .colorScheme
                                .onSurfaceVariant,
                          ),
                  ),
                ),
                if (widget.help case final String help)
                  IconButton(
                    tooltip: help,
                    padding: EdgeInsets.zero,
                    constraints: const BoxConstraints.tightFor(
                      width: 40,
                      height: 32,
                    ),
                    icon: const Icon(Icons.help_outline, size: 16),
                    onPressed: () => showDialog<void>(
                      context: context,
                      builder: (dialog) => AlertDialog(
                        title: Text(widget.label),
                        content: Text(help),
                        actions: [
                          TextButton(
                            onPressed: () => Navigator.of(dialog).pop(),
                            child: Text(dialog.strings.close),
                          ),
                        ],
                      ),
                    ),
                  ),
                if (widget.accessory case final Widget action)
                  Expanded(
                    flex: 3,
                    child: Align(
                      alignment: AlignmentDirectional.centerEnd,
                      widthFactor: 1,
                      child: action,
                    ),
                  ),
              ],
            ),
          ),
        ),
        Flexible(
          flex: inline ? 1 : 0,
          fit: inline ? FlexFit.tight : FlexFit.loose,
          child: widget.inputAccessory == null
              ? _input()
              : LayoutBuilder(
                  builder: (context, constraints) => Row(
                    crossAxisAlignment: CrossAxisAlignment.start,
                    spacing: 8,
                    children: [
                      Expanded(child: _input()),
                      ConstrainedBox(
                        constraints: BoxConstraints(
                          maxWidth: constraints.maxWidth * .45,
                        ),
                        child: widget.inputAccessory!,
                      ),
                    ],
                  ),
                ),
        ),
      ],
    );
  }

  Widget _input() => Semantics(
    label: widget.label,
    child: TextFormField(
      key: widget.inputKey,
      controller: widget.controller,
      enabled: widget.enabled,
      focusNode: widget.focusNode,
      keyboardType: widget.keyboard,
      autofillHints: widget.autofill == null ? null : [widget.autofill!],
      inputFormatters: widget.formatters,
      validator: widget.validator,
      obscureText: widget.password && !_showPassword,
      enableSuggestions: !widget.password,
      autocorrect: false,
      maxLength: widget.password ? 128 : widget.maxLength,
      minLines: widget.minLines,
      maxLines: widget.maxLines,
      textInputAction: widget.onSubmitted == null
          ? widget.maxLines > 1
                ? TextInputAction.newline
                : TextInputAction.next
          : TextInputAction.done,
      onFieldSubmitted: widget.onSubmitted,
      onChanged: widget.onChanged,
      decoration: InputDecoration(
        border: const OutlineInputBorder(),
        isDense: true,
        contentPadding: const EdgeInsets.symmetric(
          horizontal: 14,
          vertical: 10,
        ),
        constraints: const BoxConstraints(minHeight: 44),
        suffixIconConstraints: const BoxConstraints(
          minHeight: 44,
          minWidth: 44,
        ),
        hintText: widget.hint,
        helperText: widget.helperText,
        errorMaxLines: 3,
        counterText: widget.showCounter ? null : '',
        suffixIcon: widget.password
            ? IconButton(
                tooltip: _showPassword
                    ? context.strings.hidePassword
                    : context.strings.showPassword,
                constraints: const BoxConstraints.tightFor(
                  width: 44,
                  height: 44,
                ),
                padding: EdgeInsets.zero,
                onPressed: widget.enabled
                    ? () => setState(() => _showPassword = !_showPassword)
                    : null,
                icon: Icon(
                  _showPassword
                      ? Icons.visibility_off_outlined
                      : Icons.visibility_outlined,
                ),
              )
            : null,
      ),
    ),
  );
}
