import 'package:flutter/widgets.dart';

void restoreModalTriggerFocus(BuildContext context, FocusNode? trigger) {
  if (trigger == null) return;
  WidgetsBinding.instance.addPostFrameCallback((_) {
    if (context.mounted &&
        ModalRoute.isCurrentOf(context) != false &&
        trigger.context?.mounted == true) {
      trigger.requestFocus();
    }
  });
}

class ModalKeyboardDismiss extends StatelessWidget {
  const ModalKeyboardDismiss({super.key, required this.child});

  final Widget child;

  @override
  Widget build(BuildContext context) => Actions(
    actions: {
      DismissIntent: CallbackAction<DismissIntent>(
        onInvoke: (_) => Navigator.of(context).maybePop(),
      ),
    },
    child: child,
  );
}
