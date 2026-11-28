import 'dart:async';

import 'package:intl/intl.dart';
import 'package:material_ui/material_ui.dart';

import 'account.dart';
import 'account_api.dart';
import 'l10n/strings.dart';
import 'launcher_select.dart';
import 'site_documents.dart';
import 'profile_data.dart';
import 'profile_identity.dart';
import 'th09_rating_chart.dart';
import 'th09_ratings.dart';

class Th09PlayerCardButton extends StatefulWidget {
  const Th09PlayerCardButton({
    super.key,
    required this.api,
    required this.username,
    required this.name,
    required this.scores,
    required this.child,
    this.profileCharacter,
  });

  final AccountApi api;
  final String username;
  final String name;
  final Th09PlayerScoreCard? scores;
  final Widget child;
  final int? profileCharacter;

  @override
  State<Th09PlayerCardButton> createState() => _Th09PlayerCardButtonState();
}

class _Th09PlayerCardButtonState extends State<Th09PlayerCardButton> {
  final _menu = MenuController();
  Timer? _hover;
  bool _pinned = false;

  @override
  void dispose() {
    _hover?.cancel();
    super.dispose();
  }

  void _enter() {
    _hover?.cancel();
    if (_menu.isOpen) return;
    _hover = Timer(const Duration(milliseconds: 180), () {
      if (mounted && !_menu.isOpen) _menu.open();
    });
  }

  void _leave() {
    _hover?.cancel();
    _hover = Timer(const Duration(milliseconds: 220), () {
      if (mounted && !_pinned && _menu.isOpen) _menu.close();
    });
  }

  Widget _trigger(BuildContext context, VoidCallback onTap) => Semantics(
    label: widget.scores == null
        ? '${context.strings.viewProfile} | ${widget.name}'
        : context.strings.th09PlayerCardLabel(widget.name),
    child: IconButton(
      key: ValueKey('th09-card-${widget.username}'),
      onPressed: onTap,
      padding: EdgeInsets.zero,
      constraints: const BoxConstraints.tightFor(width: 44, height: 44),
      icon: widget.child,
    ),
  );

  Widget _card(VoidCallback onProfile) => _PlayerScoreCard(
    key: ValueKey(widget.username),
    api: widget.api,
    username: widget.username,
    name: widget.name,
    scores: widget.scores,
    onProfile: onProfile,
  );

  @override
  Widget build(BuildContext context) {
    final path = Uri(
      path: profilePath(widget.username),
      queryParameters: widget.profileCharacter == null
          ? null
          : {'character': '${widget.profileCharacter}'},
    ).toString();
    if (MediaQuery.sizeOf(context).width <= 780) {
      _hover?.cancel();
      return _trigger(context, () {
        showModalBottomSheet<void>(
          context: context,
          sheetAnimationStyle: LauncherControl.overlayMotion(context),
          useRootNavigator: true,
          showDragHandle: true,
          useSafeArea: true,
          isScrollControlled: true,
          builder: (dialog) => SafeArea(
            top: false,
            child: SingleChildScrollView(
              child: _card(() {
                Navigator.of(dialog).pop();
                openSiteLink(context, path);
              }),
            ),
          ),
        );
      });
    }
    return MenuAnchor(
      animated: !MediaQuery.disableAnimationsOf(context),
      controller: _menu,
      alignmentOffset: const Offset(0, 4),
      style: const MenuStyle(
        padding: WidgetStatePropertyAll(EdgeInsets.zero),
        maximumSize: WidgetStatePropertyAll(Size(320, 420)),
      ),
      onClose: () => _pinned = false,
      menuChildren: [
        MouseRegion(
          onEnter: (_) => _hover?.cancel(),
          onExit: (_) => _leave(),
          child: SizedBox(
            width: 300,
            child: _card(() {
              _menu.close();
              openSiteLink(context, path);
            }),
          ),
        ),
      ],
      builder: (_, controller, _) => MouseRegion(
        onEnter: (_) => _enter(),
        onExit: (_) => _leave(),
        child: _trigger(context, () {
          _hover?.cancel();
          if (_pinned) {
            controller.close();
          } else {
            _pinned = true;
            if (!controller.isOpen) controller.open();
          }
        }),
      ),
    );
  }
}

class Th09RatingText extends StatelessWidget {
  const Th09RatingText({
    super.key,
    this.rating,
    this.viewer,
    this.games,
    this.alignment = TextAlign.start,
  });

  final Th09LobbyRating? rating;
  final Th09LobbyRating? viewer;
  final int? games;
  final TextAlign alignment;

  @override
  Widget build(BuildContext context) {
    final theme = Theme.of(context);
    final strings = context.strings;
    final player = rating;
    final own = viewer;
    final delta = player != null && own != null && player.group == own.group
        ? player.score - own.score
        : 0;
    final intensity = ((delta.abs() - (player?.interval ?? 0)) / 300).clamp(
      0.0,
      1.0,
    );
    final count = games ?? player?.games;
    final number = NumberFormat.decimalPattern(strings.localeName);
    final relative = delta > 0
        ? theme.colorScheme.error
        : theme.brightness == Brightness.dark
        ? Colors.green.shade300
        : Colors.green.shade800;
    return Text.rich(
      TextSpan(
        style: theme.textTheme.labelSmall,
        children: [
          TextSpan(
            text: player == null ? '—' : number.format(player.score),
            style: TextStyle(
              fontWeight: FontWeight.w600,
              color: Color.lerp(
                theme.colorScheme.onSurface,
                relative,
                intensity,
              ),
            ),
          ),
          if (count != null)
            TextSpan(text: ' ${strings.th09RatingGames(number.format(count))}'),
        ],
      ),
      maxLines: 1,
      overflow: TextOverflow.ellipsis,
      textAlign: alignment,
    );
  }
}

class _PlayerScoreCard extends StatefulWidget {
  const _PlayerScoreCard({
    super.key,
    required this.api,
    required this.username,
    required this.name,
    required this.scores,
    required this.onProfile,
  });

  final AccountApi api;
  final String username;
  final String name;
  final Th09PlayerScoreCard? scores;
  final VoidCallback onProfile;

  @override
  State<_PlayerScoreCard> createState() => _PlayerScoreCardState();
}

class _PlayerScoreCardState extends State<_PlayerScoreCard> {
  late final _profile = widget.api.profile(widget.username);

  @override
  Widget build(BuildContext context) => FutureBuilder<PublicProfile>(
    future: _profile,
    builder: (context, snapshot) {
      final profile = snapshot.data;
      final scores = profile?.showVersus == true ? widget.scores : null;
      final total = profile?.showRecords == true ? scores?.score : null;
      final characters = scores?.characters ?? const <Th09CharacterScore>[];
      final strings = context.strings;
      final theme = Theme.of(context);
      final numbers = NumberFormat.decimalPattern(strings.localeName);
      return Column(
        key: const ValueKey('th09-player-card'),
        mainAxisSize: MainAxisSize.min,
        crossAxisAlignment: CrossAxisAlignment.stretch,
        children: [
          if (profile?.cover case final Uri cover)
            Image.network(
              cover.toString(),
              height: 68,
              fit: BoxFit.cover,
              excludeFromSemantics: true,
              errorBuilder: (_, _, _) => const SizedBox(height: 68),
            ),
          Padding(
            padding: const EdgeInsets.fromLTRB(16, 12, 16, 12),
            child: Column(
              crossAxisAlignment: CrossAxisAlignment.stretch,
              mainAxisSize: MainAxisSize.min,
              children: [
                Row(
                  spacing: 10,
                  children: [
                    AccountAvatar(
                      api: widget.api,
                      username: widget.username,
                      address: profile?.avatar,
                    ),
                    Expanded(
                      child: Column(
                        crossAxisAlignment: CrossAxisAlignment.start,
                        children: [
                          Text(
                            profile?.nickname ?? widget.name,
                            maxLines: 1,
                            overflow: TextOverflow.ellipsis,
                            style: theme.textTheme.titleSmall,
                          ),
                          Text(
                            '@${widget.username}',
                            style: theme.textTheme.bodySmall,
                          ),
                        ],
                      ),
                    ),
                  ],
                ),
                if (total != null || characters.isNotEmpty) ...[
                  const Divider(height: 22),
                  Row(
                    children: [
                      Expanded(
                        child: Text(
                          strings.th09RatingsTitle,
                          style: theme.textTheme.titleSmall,
                        ),
                      ),
                      if (total != null)
                        Text(
                          numbers.format(total),
                          style: theme.textTheme.titleSmall,
                        ),
                    ],
                  ),
                  const SizedBox(height: 6),
                  for (final row in characters)
                    Padding(
                      padding: const EdgeInsets.symmetric(vertical: 3),
                      child: Row(
                        spacing: 8,
                        children: [
                          Th09CharacterPortrait(row.character, size: 24),
                          Expanded(
                            child: Text(
                              th09CharacterLabel(
                                row.character,
                                Localizations.localeOf(context).languageCode,
                              ),
                            ),
                          ),
                          Text(numbers.format(row.score)),
                        ],
                      ),
                    ),
                ],
                Align(
                  alignment: AlignmentDirectional.centerEnd,
                  child: TextButton.icon(
                    onPressed: widget.onProfile,
                    icon: const Icon(Icons.north_east, size: 14),
                    label: Text(strings.viewProfile),
                  ),
                ),
              ],
            ),
          ),
        ],
      );
    },
  );
}
