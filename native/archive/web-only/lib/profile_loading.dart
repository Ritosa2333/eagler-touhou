import 'dart:math' as math;
import 'dart:ui' show SemanticsRole;

import 'package:material_ui/material_ui.dart';
import 'package:skeletonizer/skeletonizer.dart';

/// The shared, static loading panel used by the profile and its settings.
class ProfilePanelLoading extends StatelessWidget {
  const ProfilePanelLoading({super.key, required this.label});

  final String label;

  @override
  Widget build(BuildContext context) => Semantics(
    container: true,
    role: SemanticsRole.status,
    label: label,
    child: ExcludeSemantics(
      child: ConstrainedBox(
        key: const ValueKey('profile-panel-loading'),
        constraints: const BoxConstraints(minHeight: 128),
        child: Card.outlined(
          color: Theme.of(context).colorScheme.surface,
          margin: EdgeInsets.zero,
          child: Padding(
            padding: const EdgeInsets.all(20),
            child: LayoutBuilder(
              builder: (context, space) => Skeletonizer.zone(
                effect: SolidColorEffect(
                  color: Theme.of(context).colorScheme.surfaceContainerHighest,
                ),
                child: Column(
                  mainAxisSize: MainAxisSize.min,
                  crossAxisAlignment: CrossAxisAlignment.start,
                  spacing: 16,
                  children: [
                    Bone(
                      width: math.min(150, space.maxWidth * .55),
                      height: 12,
                      uniRadius: 4,
                    ),
                    Bone(width: space.maxWidth * .8, height: 12, uniRadius: 4),
                  ],
                ),
              ),
            ),
          ),
        ),
      ),
    ),
  );
}
