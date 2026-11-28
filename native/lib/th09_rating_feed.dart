import 'dart:async';

import 'package:material_ui/material_ui.dart';

import 'account.dart';
import 'th09_ratings.dart';

typedef _Query = ({String roster, String? viewer});

class Th09RatingFeed extends StatefulWidget {
  const Th09RatingFeed({
    super.key,
    required this.session,
    required this.roster,
    required this.builder,
  });

  final AccountSession session;
  final Iterable<String>? roster;
  final Widget Function(BuildContext, Th09LobbyRatings, Th09LobbyRating?)
  builder;

  @override
  State<Th09RatingFeed> createState() => _Th09RatingFeedState();
}

class _Th09RatingFeedState extends State<Th09RatingFeed>
    with WidgetsBindingObserver {
  bool _foreground = true;
  bool _visible = false;
  bool _loading = false;
  Timer? _timer;
  _Query? _query;
  AccountSession? _source;
  String? _owner;
  Th09LobbyRatings _data = const Th09LobbyRatings();

  String? get _viewer => widget.session.identity?.username.toLowerCase();

  @override
  void initState() {
    super.initState();
    final binding = WidgetsBinding.instance;
    binding.addObserver(this);
    final lifecycle = binding.lifecycleState;
    _foreground = lifecycle == null || lifecycle == AppLifecycleState.resumed;
    widget.session.addListener(_accountChanged);
  }

  @override
  void didChangeDependencies() {
    super.didChangeDependencies();
    final ticking = TickerMode.valuesOf(context).enabled;
    final current = ModalRoute.isCurrentOf(context);
    _visible = ticking && current != false;
    _sync();
  }

  @override
  void didUpdateWidget(Th09RatingFeed oldWidget) {
    super.didUpdateWidget(oldWidget);
    if (!identical(oldWidget.session, widget.session)) {
      oldWidget.session.removeListener(_accountChanged);
      widget.session.addListener(_accountChanged);
      _query = null;
    }
    _sync();
  }

  @override
  void didChangeAppLifecycleState(AppLifecycleState state) {
    _foreground = state == AppLifecycleState.resumed;
    _sync();
  }

  void _accountChanged() {
    setState(() {});
    _sync();
  }

  void _sync() {
    final roster = widget.roster;
    final active = _foreground && _visible && roster != null;
    if (!active) {
      _query = null;
      _timer?.cancel();
      _timer = null;
      return;
    }
    final names = roster.map((name) => name.toLowerCase()).toSet().toList()
      ..sort();
    final next = (roster: names.take(400).join(','), viewer: _viewer);
    if (_query == next) return;
    _query = next;
    _timer ??= Timer.periodic(th09RatingsRefreshPeriod, (_) => _refresh());
    unawaited(_refresh());
  }

  Future<void> _refresh() async {
    final query = _query;
    if (query == null || _loading) return;
    final source = widget.session;
    _loading = true;
    try {
      final result = await source.api.th09LobbyRatings(
        query.roster.isEmpty ? const [] : query.roster.split(','),
        authenticated: query.viewer != null,
      );
      if (mounted && query == _query && identical(source, widget.session)) {
        setState(() {
          _data = result;
          _source = source;
          _owner = query.viewer;
        });
      }
    } on Object {
      // Keep the last confirmed data while the next foreground tick retries.
    } finally {
      _loading = false;
      if (mounted && (query != _query || !identical(source, widget.session))) {
        unawaited(_refresh());
      }
    }
  }

  @override
  void dispose() {
    _timer?.cancel();
    WidgetsBinding.instance.removeObserver(this);
    widget.session.removeListener(_accountChanged);
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    final viewer = _viewer;
    final data = identical(_source, widget.session) && _owner == viewer
        ? _data
        : const Th09LobbyRatings();
    return widget.builder(context, data, data.players[viewer]);
  }
}
