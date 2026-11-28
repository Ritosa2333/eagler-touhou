import 'package:material_ui/material_ui.dart';

import 'account.dart';
import 'catalog.dart';
import 'library.dart';
import 'l10n/strings.dart';
import 'lobby_directory.dart';
import 'th09_player_card.dart';
import 'th09_ratings.dart';

class LobbyRoomCard extends StatelessWidget {
  const LobbyRoomCard({
    super.key,
    required this.room,
    required this.session,
    required this.clock,
    required this.onEnter,
    this.ratings = const Th09LobbyRatings(),
    this.ownRating,
  });

  final LobbyRoom room;
  final AccountSession session;
  final ValueNotifier<DateTime> clock;
  final VoidCallback? onEnter;
  final Th09LobbyRatings ratings;
  final Th09LobbyRating? ownRating;

  @override
  Widget build(BuildContext context) {
    final theme = Theme.of(context);
    final colors = theme.colorScheme;
    final strings = context.strings;
    final status = room.playing
        ? strings.roomPlaying
        : room.watchEntry
        ? strings.roomLobbyFull
        : strings.roomLobbyOpen;
    final facts = Column(
      crossAxisAlignment: CrossAxisAlignment.start,
      spacing: 5,
      children: [
        Wrap(
          spacing: 8,
          runSpacing: 3,
          crossAxisAlignment: WrapCrossAlignment.center,
          children: [
            Text(room.game.title, style: theme.textTheme.titleSmall),
            if (room.difficulty case final String difficulty)
              DecoratedBox(
                decoration: BoxDecoration(
                  color: colors.surfaceContainerHighest,
                  borderRadius: BorderRadius.circular(4),
                ),
                child: Padding(
                  padding: const EdgeInsets.symmetric(
                    horizontal: 5,
                    vertical: 2,
                  ),
                  child: Text(difficulty, style: theme.textTheme.labelSmall),
                ),
              ),
          ],
        ),
        Text(
          '#${room.code} | $status',
          style: theme.textTheme.labelMedium?.copyWith(
            color: room.playing ? colors.primary : colors.tertiary,
            fontFeatures: const [FontFeature.tabularFigures()],
          ),
        ),
        if (room.track case final String track)
          Text(
            track,
            maxLines: 1,
            overflow: TextOverflow.ellipsis,
            style: theme.textTheme.bodySmall,
          ),
      ],
    );
    final metrics = ValueListenableBuilder(
      valueListenable: clock,
      builder: (_, now, _) {
        final since = room.playing ? room.startedAt : room.createdAt;
        final seconds = since == null
            ? null
            : now.difference(since).inSeconds.clamp(0, 86400);
        final time = seconds == null
            ? null
            : '${seconds ~/ 60}:${(seconds % 60).toString().padLeft(2, '0')}';
        return Text(
          [
            strings.roomSpectatorCount(room.spectators),
            if (time != null)
              room.playing
                  ? strings.roomElapsed(time)
                  : strings.roomWaitTime(time),
          ].join(' | '),
          style: theme.textTheme.bodySmall?.copyWith(
            fontFeatures: const [FontFeature.tabularFigures()],
          ),
        );
      },
    );
    final identity = Row(
      spacing: 10,
      children: [
        GameArtwork(game: room.game, size: 48),
        Expanded(child: facts),
      ],
    );
    final rules = Text(
      room.rules.isEmpty && room.ballCount == null
          ? strings.roomDefaultRules
          : [
              for (final rule in room.rules) rule.label(strings),
              if (room.ballCount case final int count)
                strings.roomBallCount(count),
            ].join(' | '),
      style: theme.textTheme.bodySmall,
    );
    final seats = _Seats(
      room: room,
      session: session,
      ratings: ratings,
      ownRating: ownRating,
    );
    final button = OutlinedButton(
      onPressed: onEnter,
      child: Text(room.watchEntry ? strings.roomJoinSpectator : strings.join),
    );
    return Padding(
      padding: const EdgeInsets.symmetric(horizontal: 12, vertical: 14),
      child: LayoutBuilder(
        builder: (_, constraints) => constraints.maxWidth >= 670
            ? Row(
                spacing: 12,
                children: [
                  Expanded(
                    flex: 3,
                    child: Column(
                      crossAxisAlignment: CrossAxisAlignment.start,
                      spacing: 5,
                      children: [
                        identity,
                        Padding(
                          padding: const EdgeInsetsDirectional.only(start: 58),
                          child: metrics,
                        ),
                      ],
                    ),
                  ),
                  SizedBox(width: 105, child: rules),
                  Expanded(flex: 5, child: seats),
                  SizedBox(width: 106, child: button),
                ],
              )
            : Column(
                crossAxisAlignment: CrossAxisAlignment.stretch,
                spacing: 10,
                children: [
                  identity,
                  rules,
                  seats,
                  Row(
                    spacing: 8,
                    children: [
                      Expanded(child: metrics),
                      button,
                    ],
                  ),
                ],
              ),
      ),
    );
  }
}

class _Seats extends StatelessWidget {
  const _Seats({
    required this.room,
    required this.session,
    required this.ratings,
    required this.ownRating,
  });

  final LobbyRoom room;
  final AccountSession session;
  final Th09LobbyRatings ratings;
  final Th09LobbyRating? ownRating;

  @override
  Widget build(BuildContext context) {
    final duel =
        room.game.category == BookCategory.versus && room.seats.length == 2;
    final first = room.seats.first;
    final second = room.seats.last;
    final th09 = room.game.id == 'th09';
    final seats = Row(
      spacing: 6,
      children: [
        for (final (index, seat) in room.seats.indexed) ...[
          if (duel && index == 1)
            Expanded(
              child: Text(
                first == null || second == null
                    ? 'VS'
                    : '${first.wins} : ${second.wins}',
                textAlign: TextAlign.center,
                maxLines: 1,
                overflow: TextOverflow.ellipsis,
                style: Theme.of(context).textTheme.titleLarge,
              ),
            ),
          Expanded(
            flex: duel && th09 ? 2 : 1,
            child: _SeatView(
              seat: seat,
              session: session,
              ratings: th09 ? ratings : null,
            ),
          ),
        ],
      ],
    );
    if (!duel || !th09) return seats;
    return Column(
      mainAxisSize: MainAxisSize.min,
      children: [
        Row(
          spacing: 6,
          children: [
            Expanded(flex: 2, child: _seatRating(context, first)),
            const Expanded(child: SizedBox()),
            Expanded(flex: 2, child: _seatRating(context, second)),
          ],
        ),
        seats,
      ],
    );
  }

  Widget _seatRating(BuildContext context, LobbySeat? seat) => seat == null
      ? const SizedBox()
      : Th09RatingText(
          rating: ratings.players[seat.account?.toLowerCase()],
          viewer: ownRating,
          games: seat.ratingGames,
        );
}

class _SeatView extends StatelessWidget {
  const _SeatView({required this.seat, required this.session, this.ratings});

  final LobbySeat? seat;
  final AccountSession session;
  final Th09LobbyRatings? ratings;

  @override
  Widget build(BuildContext context) {
    final theme = Theme.of(context);
    final strings = context.strings;
    final point = seat;
    if (point == null) {
      return Row(
        spacing: 5,
        children: [
          const CircleAvatar(
            radius: 12,
            child: Icon(Icons.person_outline, size: 14),
          ),
          Flexible(
            child: Text(
              strings.roomSeatEmpty,
              maxLines: 1,
              overflow: TextOverflow.ellipsis,
              style: theme.textTheme.bodySmall,
            ),
          ),
        ],
      );
    }
    final color = !point.online
        ? theme.colorScheme.outline
        : point.ready
        ? theme.colorScheme.tertiary
        : theme.colorScheme.primary;
    final control = point.control?.label(strings);
    final account = point.account?.toLowerCase();
    final canOpenCard = ratings != null && account != null;
    final avatar = Stack(
      clipBehavior: Clip.none,
      children: [
        if (point.account case final String name)
          AccountAvatar(api: session.api, username: name, size: 26)
        else
          CircleAvatar(
            radius: 13,
            child: Text(
              String.fromCharCode(point.name.runes.first),
              style: theme.textTheme.labelSmall,
            ),
          ),
        PositionedDirectional(
          end: 0,
          bottom: 0,
          child: DecoratedBox(
            decoration: BoxDecoration(
              color: color,
              shape: BoxShape.circle,
              border: Border.all(color: theme.colorScheme.surface),
            ),
            child: const SizedBox.square(dimension: 8),
          ),
        ),
      ],
    );
    final caption = [
      point.name,
      ?control,
      point.online
          ? (point.ready ? strings.roomSeatReady : strings.roomOnline)
          : strings.roomOffline,
    ].join(' | ');
    final body = Row(
      spacing: 5,
      children: [
        if (canOpenCard)
          Th09PlayerCardButton(
            api: session.api,
            username: account,
            name: point.name,
            scores: ratings?.cards[account],
            child: avatar,
          )
        else
          avatar,
        Flexible(
          child: Column(
            crossAxisAlignment: CrossAxisAlignment.start,
            children: [
              Text(point.name, maxLines: 1, overflow: TextOverflow.ellipsis),
              if (control != null)
                Text(
                  control,
                  maxLines: 1,
                  overflow: TextOverflow.ellipsis,
                  style: theme.textTheme.labelSmall,
                ),
            ],
          ),
        ),
      ],
    );
    return canOpenCard
        ? Semantics(label: caption, child: body)
        : Tooltip(message: caption, child: body);
  }
}
