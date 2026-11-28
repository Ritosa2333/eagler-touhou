import 'package:material_ui/material_ui.dart';

import 'account_api.dart';
import 'catalog.dart';
import 'game_launcher.dart';
import 'l10n/strings.dart';
import 'replay_manager.dart';
import 'th09_ratings.dart';

class Th09DuelReplayList extends StatefulWidget {
  const Th09DuelReplayList({
    super.key,
    required this.api,
    required this.duel,
    required this.count,
    required this.game,
    required this.launcher,
  });

  final AccountApi api;
  final String duel;
  final int count;
  final Game game;
  final GameLaunchService launcher;

  @override
  State<Th09DuelReplayList> createState() => _Th09DuelReplayListState();
}

class _Th09DuelReplayListState extends State<Th09DuelReplayList> {
  final _detailsStorage = PageStorageBucket();
  Future<List<Th09DuelReplay>>? _replays;
  bool _busy = false;

  void _load() {
    final request = Future<List<Th09DuelReplay>>.sync(
      () => widget.api.th09DuelReplays(widget.duel),
    );
    request.ignore(); // The FutureBuilder subscribes on the next frame.
    setState(() {
      _replays = request;
    });
  }

  Future<void> _access(Th09DuelReplay round, {required bool play}) async {
    if (_busy) return;
    setState(() => _busy = true);
    try {
      final archive = await widget.api.downloadTh09Replay(widget.duel, round);
      if (!mounted) return;
      final status = await widget.launcher.withArchivedReplay(
        widget.game,
        archive.filename,
        archive.bytes,
        (file, export) async {
          if (!mounted) return null;
          if (play) {
            return widget.launcher.start(
              widget.game,
              multiplayer: true,
              isActive: () => mounted,
              replayFile: file,
            );
          }
          final saved = await export();
          if (mounted && saved) {
            ScaffoldMessenger.of(context).showSnackBar(
              SnackBar(content: Text(context.strings.gameFileSaved)),
            );
          }
          return null;
        },
      );
      if (!mounted || status != GameLaunchStatus.nativeRuntimeUnavailable) {
        return;
      }
      await showReplayUnavailable(
        context,
        name: round.name ?? _characters(round),
      );
    } catch (_) {
      if (mounted) {
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(content: Text(context.strings.replayFileFailed)),
        );
      }
    } finally {
      if (mounted) setState(() => _busy = false);
    }
  }

  String _characters(Th09DuelReplay round) => round.characters
      .map((id) => th09CharacterLabel(id, context.strings.localeName))
      .join(' | ');

  @override
  Widget build(BuildContext context) => PageStorage(
    bucket: _detailsStorage,
    child: ExpansionTile(
      tilePadding: EdgeInsets.zero,
      leading: const Icon(Icons.movie_outlined),
      title: Text(context.strings.profileMatchReplays(widget.count)),
      onExpansionChanged: (expanded) {
        if (expanded && _replays == null) _load();
      },
      children: [
        if (_replays case final Future<List<Th09DuelReplay>> future)
          FutureBuilder(
            future: future,
            builder: (context, state) {
              if (state.hasError) {
                return TextButton(
                  onPressed: _load,
                  child: Text(context.strings.replayRead),
                );
              }
              final rounds = state.data;
              if (rounds == null) return const LinearProgressIndicator();
              if (rounds.isEmpty) return Text(context.strings.gameNoReplay);
              return Column(
                children: [
                  for (final (index, round) in rounds.indexed)
                    ListTile(
                      key: ValueKey(round.id),
                      contentPadding: const EdgeInsetsDirectional.only(
                        start: 6,
                      ),
                      title: Text(_characters(round)),
                      subtitle: Text(
                        [
                          context.strings.th09ReplayRound(index + 1),
                          if (round.frames case final int frames)
                            context.strings.formatPlaytime(frames / 60),
                        ].join(' | '),
                      ),
                      trailing: Row(
                        mainAxisSize: MainAxisSize.min,
                        children: [
                          IconButton(
                            onPressed: _busy
                                ? null
                                : () => _access(round, play: true),
                            tooltip: context.strings.th09ReplayPlayRound(
                              index + 1,
                            ),
                            icon: const Icon(Icons.play_arrow_outlined),
                          ),
                          IconButton(
                            onPressed: _busy
                                ? null
                                : () => _access(round, play: false),
                            tooltip: context.strings.th09ReplayDownloadRound(
                              index + 1,
                            ),
                            icon: const Icon(Icons.download_outlined),
                          ),
                        ],
                      ),
                    ),
                  if (_busy) const LinearProgressIndicator(),
                ],
              );
            },
          ),
      ],
    ),
  );
}
