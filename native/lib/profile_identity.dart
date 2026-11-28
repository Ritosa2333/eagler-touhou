import 'package:country_flags/country_flags.dart' as flags;
import 'package:material_ui/material_ui.dart';

import 'profile_regions.dart';
import 'profile_data.dart' show ProfileList;

String profilePath(String username) =>
    '/u/${Uri.encodeComponent(username.toLowerCase())}';

String profileEditPath(String username) => '${profilePath(username)}#edit';

String profileSocialPath(String username, ProfileList tab) =>
    '${profilePath(username)}?social=${tab.name}';

class IdentityDetails extends StatelessWidget {
  const IdentityDetails({super.key, required this.username, this.region});

  final String username;
  final String? region;
  @override
  Widget build(BuildContext context) {
    final theme = Theme.of(context);
    final style = theme.textTheme.bodySmall?.copyWith(
      color: theme.colorScheme.onSurfaceVariant,
    );
    final code = region?.toUpperCase();
    return Wrap(
      spacing: 10,
      runSpacing: 3,
      crossAxisAlignment: WrapCrossAlignment.center,
      children: [
        Text('@$username', style: style, overflow: TextOverflow.ellipsis),
        if (code != null && RegExp(r'^[A-Z]{2}$').hasMatch(code))
          Row(
            mainAxisSize: MainAxisSize.min,
            spacing: 5,
            children: [
              ExcludeSemantics(
                child: flags.CountryFlag.fromCountryCode(
                  code,
                  theme: const flags.ImageTheme(
                    width: 18,
                    height: 12,
                    shape: flags.RoundedRectangle(2),
                  ),
                ),
              ),
              Flexible(
                child: Text(
                  profileRegionNames[code] ?? code,
                  style: style,
                  overflow: TextOverflow.ellipsis,
                ),
              ),
            ],
          ),
      ],
    );
  }
}
