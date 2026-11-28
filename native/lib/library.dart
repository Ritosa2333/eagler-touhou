import 'package:flutter/foundation.dart';
import 'package:flutter_animate/flutter_animate.dart';
import 'package:lucide_icons_flutter/lucide_icons.dart';
import 'package:material_ui/material_ui.dart';
import 'package:go_router/go_router.dart';
import 'package:prism_ui/prism_ui.dart' as prism;

import 'catalog.dart';
import 'l10n/strings.dart';
import 'lobby_directory.dart';
import 'launcher_navigation.dart';
import 'launcher_background.dart';
import 'launcher_select.dart';

class GameArtwork extends StatelessWidget {
  const GameArtwork({
    super.key,
    required this.game,
    this.size,
    this.borderRadius = const BorderRadius.all(Radius.circular(12)),
  });
  final Game game;
  final double? size;
  final BorderRadius borderRadius;

  @override
  Widget build(BuildContext context) {
    final fallback = ColoredBox(
      color: Theme.of(context).colorScheme.surfaceContainerHigh,
      child: const Center(child: Icon(Icons.sports_esports_outlined)),
    );
    final artwork = game.artwork;
    return ClipRRect(
      borderRadius: borderRadius,
      child: SizedBox.square(
        dimension: size,
        child: artwork == null
            ? fallback
            : Image.asset(
                'assets/book-covers/$artwork',
                excludeFromSemantics: true,
                fit: BoxFit.cover,
                errorBuilder: (_, _, _) => fallback,
                frameBuilder: (_, child, frame, _) =>
                    frame == null ? fallback : child,
              ),
      ),
    );
  }
}

class LibrarySearch extends StatelessWidget {
  const LibrarySearch({super.key, required this.book, this.toolbar = false});
  final BookState book;
  final bool toolbar;

  @override
  Widget build(BuildContext context) {
    final palette = Theme.of(context).colorScheme;
    final search = SearchBar(
      controller: book.searchController,
      elevation: const WidgetStatePropertyAll(0),
      constraints: BoxConstraints.tightFor(height: toolbar ? 48 : 44),
      shape: toolbar
          ? null
          : WidgetStatePropertyAll(
              RoundedRectangleBorder(
                borderRadius: BorderRadius.circular(8),
                side: BorderSide(color: palette.outlineVariant),
              ),
            ),
      backgroundColor: toolbar
          ? const WidgetStatePropertyAll(Colors.transparent)
          : WidgetStatePropertyAll(palette.surface),
      overlayColor: toolbar
          ? const WidgetStatePropertyAll(Colors.transparent)
          : null,
      padding: toolbar
          ? const WidgetStatePropertyAll(
              EdgeInsetsDirectional.only(start: 12, end: 4),
            )
          : const WidgetStatePropertyAll(
              EdgeInsetsDirectional.only(start: 16, end: 4),
            ),
      textStyle: toolbar
          ? WidgetStatePropertyAll(LauncherControl.label(context))
          : WidgetStatePropertyAll(Theme.of(context).textTheme.bodyMedium),
      hintStyle: WidgetStatePropertyAll(
        (toolbar
                ? LauncherControl.label(context)
                : Theme.of(context).textTheme.bodyMedium!)
            .copyWith(
              color: palette.onSurfaceVariant,
              fontWeight: FontWeight.w400,
            ),
      ),
      leading: toolbar
          ? Icon(LucideIcons.search, size: 16, color: palette.onSurfaceVariant)
          : Icon(LucideIcons.search, size: 20, color: palette.onSurfaceVariant),
      hintText: MediaQuery.sizeOf(context).width <= 360
          ? context.strings.searchGamesShort
          : context.strings.searchGames,
      onChanged: book.searchChanged,
      trailing: [
        if (book.searchController.text.isNotEmpty)
          IconButton(
            tooltip: context.strings.clearSearch,
            icon: const Icon(Icons.close),
            onPressed: () {
              book.searchController.clear();
              book.searchChanged('');
            },
          ),
      ],
    );
    if (!toolbar) return search;
    return Stack(
      alignment: Alignment.center,
      children: [
        PositionedDirectional(
          start: 0,
          end: 0,
          height: 34,
          child: DecoratedBox(
            key: const ValueKey('mobile search fill'),
            decoration: BoxDecoration(
              color: palette.surfaceContainerHigh,
              borderRadius: BorderRadius.circular(17),
            ),
          ),
        ),
        search,
      ],
    );
  }
}

const _bookMotionDuration = Duration(milliseconds: 180);

class _BookOrderMotion extends StatefulWidget {
  const _BookOrderMotion({
    required this.layout,
    required this.slots,
    required this.builder,
  });

  final Object layout;
  final Map<Object, Offset> slots;
  final Widget Function(
    BuildContext context,
    Animation<Offset> Function(Object) movement,
  )
  builder;

  @override
  State<_BookOrderMotion> createState() => _BookOrderMotionState();
}

class _BookOrderMotionState extends State<_BookOrderMotion>
    with SingleTickerProviderStateMixin {
  late final _controller = AnimationController(
    vsync: this,
    duration: _bookMotionDuration,
    value: 1,
  );
  final _deltas = <Object, Offset>{};
  bool _reduced = false;

  @override
  void didChangeDependencies() {
    super.didChangeDependencies();
    _reduced = MediaQuery.disableAnimationsOf(context);
    if (_reduced) _snap();
  }

  void _snap() {
    _deltas.clear();
    if (_controller.value != 1) _controller.value = 1;
  }

  @override
  void didUpdateWidget(_BookOrderMotion oldWidget) {
    super.didUpdateWidget(oldWidget);
    if (_reduced || oldWidget.layout != widget.layout) {
      _snap();
      return;
    }
    if (mapEquals(oldWidget.slots, widget.slots)) return;

    final remaining = 1 - Curves.easeOutCubic.transform(_controller.value);
    final deltas = <Object, Offset>{};
    for (final MapEntry(key: key, value: target) in widget.slots.entries) {
      final previous = oldWidget.slots[key];
      if (previous == null) continue;
      final delta =
          previous - target + (_deltas[key] ?? Offset.zero) * remaining;
      if (delta.distance < .001) continue;
      // Keep items arriving from well outside the viewport near their new slot.
      deltas[key] = delta.distance > 4 ? delta / delta.distance * 4 : delta;
    }
    _deltas
      ..clear()
      ..addAll(deltas);
    if (_deltas.isEmpty) {
      _snap();
    } else {
      _controller.forward(from: 0);
    }
  }

  Animation<Offset> _movement(Object key) {
    final delta = _deltas[key];
    if (delta == null) return const AlwaysStoppedAnimation(Offset.zero);
    return _controller
        .drive(CurveTween(curve: Curves.easeOutCubic))
        .drive(Tween(begin: delta, end: Offset.zero));
  }

  @override
  Widget build(BuildContext context) => widget.builder(context, _movement);

  @override
  void dispose() {
    _controller.dispose();
    super.dispose();
  }
}

class _BookTileHover extends StatefulWidget {
  const _BookTileHover({required this.builder});

  final Widget Function(BuildContext, bool) builder;

  @override
  State<_BookTileHover> createState() => _BookTileHoverState();
}

class _BookTileHoverState extends State<_BookTileHover> {
  bool _hovered = false;

  @override
  Widget build(BuildContext context) => MouseRegion(
    onEnter: (_) => setState(() => _hovered = true),
    onExit: (_) => setState(() => _hovered = false),
    child: widget.builder(context, _hovered),
  );
}

class FavoriteButton extends StatelessWidget {
  const FavoriteButton({
    super.key,
    required this.book,
    required this.game,
    this.tileHovered = false,
    this.compact = false,
    this.outlined = false,
  });
  final BookState book;
  final Game game;
  final bool tileHovered;
  final bool compact;
  final bool outlined;

  @override
  Widget build(BuildContext context) => ListenableBuilder(
    listenable: book.favorites,
    builder: (context, _) {
      final active = book.isFavorite(game);
      final button = outlined ? IconButton.outlined : IconButton.new;
      return button(
        tooltip: active
            ? context.strings.removeFavorite(game.title)
            : context.strings.favoriteGame(game.title),
        isSelected: active,
        style: outlined
            ? LauncherControl.style(context)
            : compact
            ? IconButton.styleFrom(
                minimumSize: const Size.square(36),
                fixedSize: const Size.square(36),
                padding: EdgeInsets.zero,
                tapTargetSize: MaterialTapTargetSize.shrinkWrap,
              )
            : null,
        onPressed: () => book.toggleFavorite(game),
        icon: _icon(context, Icons.star_outline_rounded),
        selectedIcon: _icon(
          context,
          Icons.star_rounded,
          outlined ? const Color(0xFFF5D278) : const Color(0xFFD3993D),
        ),
      );
    },
  );

  Widget _icon(BuildContext context, IconData icon, [Color? color]) {
    final child = Icon(icon, color: color, size: compact ? 16 : null);
    if (MediaQuery.disableAnimationsOf(context)) return child;
    return child
        .animate(autoPlay: false, target: tileHovered ? 1 : 0)
        .scaleXY(
          end: 1.08,
          duration: _bookMotionDuration,
          curve: Curves.easeOutCubic,
        );
  }
}

class _GameTitleLine extends StatelessWidget {
  const _GameTitleLine(this.game, {this.style});
  final Game game;
  final TextStyle? style;

  @override
  Widget build(BuildContext context) => Semantics(
    label: '${game.title} ${game.displayCode}',
    excludeSemantics: true,
    child: Row(
      mainAxisSize: MainAxisSize.min,
      crossAxisAlignment: CrossAxisAlignment.baseline,
      textBaseline: TextBaseline.alphabetic,
      spacing: 4,
      children: [
        Flexible(
          child: Text(
            game.title,
            maxLines: 1,
            overflow: TextOverflow.ellipsis,
            style: style,
          ),
        ),
        Text(
          game.displayCode,
          style: Theme.of(context).textTheme.labelSmall?.copyWith(
            fontSize: 11,
            fontWeight: FontWeight.w400,
            color: Theme.of(context).colorScheme.onSurfaceVariant,
            letterSpacing: 0,
          ),
        ),
      ],
    ),
  );
}

class GameRow extends StatelessWidget {
  const GameRow({
    super.key,
    required this.book,
    required this.game,
    required this.open,
    this.selected = false,
    this.dense = false,
    this.toggleSelection,
    this.movement = const AlwaysStoppedAnimation(Offset.zero),
  });
  final BookState book;
  final Game game;
  final VoidCallback open;
  final bool selected;
  final bool dense;
  final VoidCallback? toggleSelection;
  final Animation<Offset> movement;

  @override
  Widget build(BuildContext context) => SlideTransition(
    position: movement,
    textDirection: Directionality.of(context),
    child: RepaintBoundary(child: _BookTileHover(builder: _tile)),
  );

  Widget _tile(BuildContext context, bool hovered) => Stack(
    clipBehavior: Clip.none,
    children: [
      _BookRowSurface(
        selected: selected,
        hovered: hovered,
        onPressed: open,
        child: prism.ListRow(
          minHeight: dense ? 0 : 60,
          minVerticalPadding: dense ? 4 : 8,
          horizontalTitleGap: 10,
          padding: const EdgeInsetsDirectional.fromSTEB(10, 0, 10, 0),
          leading: GameArtwork(
            game: game,
            size: dense ? 32 : 40,
            borderRadius: prism.AppRadius.xs,
          ),
          title: Padding(
            padding: const EdgeInsetsDirectional.only(end: 36),
            child: _GameTitleLine(
              game,
              style: dense
                  ? const TextStyle(
                      fontSize: 12,
                      fontWeight: FontWeight.w500,
                      height: 1.25,
                    )
                  : null,
            ),
          ),
          subtitle: Text(
            game.subtitle,
            style: prism.AppTextStyle.footnote
                .of(context)
                .copyWith(
                  color: prism.Theme.of(context).colorScheme.onSurfaceVariant,
                  fontSize: dense ? 11 : null,
                  height: dense ? 1.25 : null,
                ),
            maxLines: 1,
            overflow: TextOverflow.ellipsis,
          ),
        ),
      ),
      PositionedDirectional(
        top: 0,
        end: 1,
        child: FavoriteButton(
          book: book,
          game: game,
          tileHovered: hovered,
          compact: true,
        ),
      ),
      if (toggleSelection case final VoidCallback toggle)
        PositionedDirectional(
          start: 2,
          top: 0,
          bottom: 0,
          child: Center(
            widthFactor: 1,
            child: Transform.translate(
              offset: Offset(0, 5 - (dense ? 32 : 40) / 2),
              child: GameSelectionMarker(
                game: game,
                selected: selected,
                onPressed: toggle,
              ),
            ),
          ),
        ),
    ],
  );
}

class _BookRowSurface extends StatelessWidget {
  const _BookRowSurface({
    super.key,
    required this.selected,
    this.hovered = false,
    required this.onPressed,
    required this.child,
  });

  final bool selected;
  final bool hovered;
  final VoidCallback onPressed;
  final Widget child;

  @override
  Widget build(BuildContext context) {
    final colors = prism.Theme.of(context).colorScheme;
    return prism.PressSurface(
      selected: selected,
      fill: LauncherBackdrop.tint(
        context,
        BackgroundLayer.category,
        selected
            ? colors.surfaceContainerHighest
            : hovered
            ? colors.surfaceContainerHigh
            : Colors.transparent,
      ),
      shape: prism.AppShape.sm,
      onPressed: onPressed,
      showsHover: !hovered && !selected,
      child: child,
    );
  }
}

class GameSelectionMarker extends StatelessWidget {
  const GameSelectionMarker({
    super.key,
    required this.game,
    required this.selected,
    required this.onPressed,
  });

  final Game game;
  final bool selected;
  final VoidCallback onPressed;

  @override
  Widget build(BuildContext context) {
    final colors = Theme.of(context).colorScheme;
    return Semantics(
      checked: selected,
      child: IconButton(
        tooltip: context.strings.filterMultiplayerGame(game.title),
        onPressed: onPressed,
        style: IconButton.styleFrom(
          fixedSize: const Size.square(26),
          minimumSize: const Size.square(26),
          padding: EdgeInsets.zero,
          tapTargetSize: MaterialTapTargetSize.shrinkWrap,
        ),
        icon: CircleAvatar(
          radius: 7,
          backgroundColor: colors.surface,
          foregroundColor: selected ? colors.primary : colors.onSurfaceVariant,
          child: Icon(
            selected ? Icons.check_circle : Icons.radio_button_unchecked,
            size: 14,
          ),
        ),
      ),
    );
  }
}

double bookSidebarWidth(BuildContext context) =>
    (MediaQuery.sizeOf(context).width * .24).clamp(248.0, 296.0);

class BookSidebar extends StatelessWidget {
  const BookSidebar({
    super.key,
    required this.book,
    required this.multiplayer,
    this.selection,
    this.available,
    this.selectedGame,
    this.width,
  });
  final BookState book;
  final bool multiplayer;
  final LobbySelection? selection;
  final Set<String>? available;
  final String? selectedGame;
  final double? width;

  @override
  Widget build(BuildContext context) => ListenableBuilder(
    listenable: book,
    builder: (context, _) => _list(context),
  );

  Widget _list(BuildContext context) {
    final strings = context.strings;
    final groups = multiplayer
        ? BookCategory.multiplayerGroups
        : BookCategory.values;
    final visible = {
      for (final group in groups)
        group: book
            .filter(group, multiplayer: multiplayer, applySearch: true)
            .where((game) => available?.contains(game.id) ?? true)
            .toList(),
    };
    final slots = <Object, Offset>{
      for (final group in groups)
        if (book.expanded.contains(group))
          for (final (index, game) in visible[group]!.indexed)
            (group, game.id): Offset(0, index.toDouble()),
    };
    return _BookOrderMotion(
      layout: (multiplayer, book.searchController.text),
      slots: slots,
      builder: (context, movement) => Material(
        color: LauncherBackdrop.tint(
          context,
          BackgroundLayer.category,
          Theme.of(context).colorScheme.surfaceContainerLow,
        ),
        child: SizedBox(
          width: width ?? bookSidebarWidth(context),
          child: ListView(
            key: PageStorageKey(('game sidebar', multiplayer)),
            primary: false,
            padding: const EdgeInsets.fromLTRB(16, 16, 16, 16),
            children: [
              Padding(
                padding: const EdgeInsets.only(bottom: 14),
                child: LibrarySearch(book: book),
              ),
              for (final group in groups)
                _BookGroup(
                  key: ValueKey(('sidebar group', group)),
                  expanded: book.expanded.contains(group),
                  headerBuilder: (context, unfold) => _BookRowSurface(
                    key: ValueKey(group),
                    selected: selection == null
                        ? book.category == group
                        : group == BookCategory.all && selection!.value.isEmpty,
                    onPressed: () {
                      if (selection case final LobbySelection selected) {
                        if (group == BookCategory.all) {
                          selected.clear();
                        } else {
                          book.toggleExpanded(group);
                        }
                        return;
                      }
                      book.select(group);
                      final root = multiplayer ? '/lobby' : '/';
                      if (GoRouterState.of(context).uri.path != root) {
                        context.go(root);
                      }
                    },
                    child: prism.ListRow(
                      minHeight: 38,
                      minVerticalPadding: 0,
                      horizontalTitleGap: 8,
                      padding: const EdgeInsetsDirectional.fromSTEB(
                        10,
                        0,
                        4,
                        0,
                      ),
                      leading: Icon(group.icon, size: 16),
                      title: Text(
                        group.label(strings, multiplayer: multiplayer),
                        maxLines: 1,
                        overflow: TextOverflow.ellipsis,
                        style: const TextStyle(fontSize: 13),
                      ),
                      trailing: Row(
                        mainAxisSize: MainAxisSize.min,
                        children: [
                          Text(
                            '${visible[group]!.length}',
                            key: const ValueKey('category count'),
                            style: const TextStyle(
                              fontSize: 11,
                              fontWeight: FontWeight.w500,
                              fontFeatures: [FontFeature.tabularFigures()],
                            ),
                          ),
                          const SizedBox(width: 4),
                          SizedBox.square(
                            dimension: 34,
                            child: group == BookCategory.all
                                ? null
                                : IconButton(
                                    tooltip: book.expanded.contains(group)
                                        ? strings.collapseGroup(
                                            group.label(strings),
                                          )
                                        : strings.expandGroup(
                                            group.label(strings),
                                          ),
                                    padding: EdgeInsets.zero,
                                    icon: RotationTransition(
                                      turns: unfold.drive(
                                        prism.UnfoldMotion.chevronTurns,
                                      ),
                                      child: const Icon(
                                        Icons.expand_more,
                                        size: 18,
                                      ),
                                    ),
                                    onPressed: () => book.toggleExpanded(group),
                                  ),
                          ),
                        ],
                      ),
                    ),
                  ),
                  child: Column(
                    children: [
                      for (final game in visible[group]!)
                        Padding(
                          padding: const EdgeInsetsDirectional.only(start: 18),
                          child: GameRow(
                            key: ValueKey((group, game.id)),
                            dense: true,
                            movement: movement((group, game.id)),
                            book: book,
                            game: game,
                            selected:
                                selection?.value.contains(game.id) ??
                                selectedGame == game.id,
                            toggleSelection: selection == null
                                ? null
                                : () => selection!.toggle(game),
                            open: () {
                              if (selection
                                  case final LobbySelection selected) {
                                selected.showOnly(game);
                              } else {
                                openLibraryGame(context, game.id);
                              }
                            },
                          ),
                        ),
                    ],
                  ),
                ),
            ],
          ),
        ),
      ),
    );
  }
}

class _BookGroup extends StatefulWidget {
  const _BookGroup({
    super.key,
    required this.expanded,
    required this.headerBuilder,
    required this.child,
  });

  final bool expanded;
  final Widget Function(BuildContext, Animation<double>) headerBuilder;
  final Widget child;

  @override
  State<_BookGroup> createState() => _BookGroupState();
}

class _BookGroupState extends State<_BookGroup>
    with SingleTickerProviderStateMixin {
  late final _controller = AnimationController(
    vsync: this,
    value: widget.expanded ? 1 : 0,
  );
  bool _reduced = false;

  @override
  void didChangeDependencies() {
    super.didChangeDependencies();
    _reduced = MediaQuery.disableAnimationsOf(context);
    if (_reduced) _controller.value = widget.expanded ? 1 : 0;
  }

  @override
  void didUpdateWidget(_BookGroup oldWidget) {
    super.didUpdateWidget(oldWidget);
    if (oldWidget.expanded == widget.expanded) return;
    prism.UnfoldMotion.run(
      _controller,
      open: widget.expanded,
      animate: !_reduced,
    );
  }

  @override
  Widget build(BuildContext context) => AnimatedBuilder(
    animation: _controller,
    child: RepaintBoundary(child: widget.child),
    builder: (context, child) => Column(
      crossAxisAlignment: CrossAxisAlignment.stretch,
      children: [
        widget.headerBuilder(context, _controller),
        Offstage(
          offstage: _controller.isDismissed,
          child: IgnorePointer(
            ignoring: !widget.expanded,
            child: ExcludeSemantics(
              excluding: !widget.expanded,
              child: ExcludeFocus(
                excluding: !widget.expanded,
                child: TickerMode(
                  enabled: !_controller.isDismissed,
                  child: SizeTransition(
                    sizeFactor: _controller,
                    alignment: Alignment.bottomCenter,
                    child: child,
                  ),
                ),
              ),
            ),
          ),
        ),
      ],
    ),
  );

  @override
  void dispose() {
    _controller.dispose();
    super.dispose();
  }
}

class CategoryRail extends StatelessWidget {
  const CategoryRail({super.key, required this.book, this.multiplayer = false});
  final BookState book;
  final bool multiplayer;

  @override
  Widget build(BuildContext context) => SingleChildScrollView(
    key: const PageStorageKey('library category rail'),
    primary: false,
    scrollDirection: Axis.horizontal,
    child: Row(
      spacing: 8,
      children: [
        for (final group in BookCategory.values)
          if (group == BookCategory.all ||
              group == BookCategory.favorites ||
              book.filter(group, multiplayer: multiplayer).isNotEmpty)
            _category(context, group),
      ],
    ),
  );

  Widget _category(BuildContext context, BookCategory group) {
    final colors = Theme.of(context).colorScheme;
    final selected = book.category == group;
    return ChoiceChip(
      avatar: Icon(
        group.icon,
        size: LauncherControl.iconSize,
        color: selected ? colors.primary : colors.onSurfaceVariant,
      ),
      labelStyle: LauncherControl.label(
        context,
      ).copyWith(color: selected ? colors.onSurface : colors.onSurfaceVariant),
      selectedColor: colors.surfaceContainerHighest,
      backgroundColor: colors.surfaceContainerLow,
      side: BorderSide.none,
      showCheckmark: false,
      label: Text.rich(
        TextSpan(
          text: group.label(context.strings),
          children: [
            if (selected)
              TextSpan(
                text:
                    ' ${book.filter(group, applySearch: true, multiplayer: multiplayer).length}',
                style: TextStyle(
                  color: colors.onSurfaceVariant,
                  fontWeight: FontWeight.w400,
                ),
              ),
          ],
        ),
      ),
      selected: selected,
      onSelected: (_) => book.select(group),
    );
  }
}

class LibraryPage extends StatelessWidget {
  const LibraryPage({super.key, required this.book});
  final BookState book;

  @override
  Widget build(BuildContext context) => ListenableBuilder(
    listenable: Listenable.merge([book, book.history]),
    builder: (context, _) => LayoutBuilder(
      builder: (context, constraints) {
        final compact = MediaQuery.sizeOf(context).width <= 780;
        final items = book.filter(book.category, applySearch: true);
        final recent = book.recentGames;
        final padding = EdgeInsets.symmetric(
          horizontal: compact
              ? 16
              : (MediaQuery.sizeOf(context).width * .039).clamp(20.0, 56.0),
        );
        final columnGap = compact ? 10.0 : 16.0;
        final rowGap = compact ? 16.0 : 20.0;
        final columns = compact
            ? 2
            : (constraints.maxWidth / 250).floor().clamp(2, 7).toInt();
        final tileWidth =
            ((constraints.maxWidth -
                        padding.horizontal -
                        (columns - 1) * columnGap) /
                    columns)
                .clamp(1.0, double.infinity);
        final captionHeight = compact
            ? _mobileBookCaptionHeight(context, items)
            : _bookCardCaptionHeight(context);
        final tileHeight = tileWidth + captionHeight;
        final indices = {
          for (final (index, game) in items.indexed) game.id: index,
        };
        int? findIndex(Key key) =>
            key is ValueKey<String> ? indices[key.value] : null;
        return _BookOrderMotion(
          layout: (
            book.category,
            book.covers,
            book.searchController.text,
            columns,
            tileWidth,
          ),
          slots: {
            for (final MapEntry(key: id, value: index) in indices.entries)
              id: book.covers
                  ? Offset(
                      (index % columns) * (1 + columnGap / tileWidth),
                      (index ~/ columns) * (1 + rowGap / tileHeight),
                    )
                  : Offset(0, index.toDouble()),
          },
          builder: (context, movement) => CustomScrollView(
            key: const PageStorageKey('library main content'),
            slivers: [
              if (recent.isNotEmpty)
                SliverPadding(
                  padding: padding.add(
                    EdgeInsets.only(top: 16, bottom: compact ? 10 : 18),
                  ),
                  sliver: SliverToBoxAdapter(
                    child: _RecentGames(games: recent, compact: compact),
                  ),
                ),
              SliverPadding(
                padding: padding.add(const EdgeInsets.symmetric(vertical: 16)),
                sliver: SliverToBoxAdapter(
                  child: _LibraryHeader(
                    book: book,
                    compact: compact,
                    count: items.length,
                  ),
                ),
              ),
              if (items.isEmpty)
                SliverFillRemaining(
                  hasScrollBody: false,
                  child: Center(
                    child: Text(
                      book.category == BookCategory.favorites &&
                              book.searchController.text.isEmpty
                          ? context.strings.emptyFavorites
                          : context.strings.emptySearch,
                    ),
                  ),
                )
              else if (book.covers)
                SliverPadding(
                  padding: padding.add(const EdgeInsets.only(bottom: 16)),
                  sliver: SliverGrid.builder(
                    itemCount: items.length,
                    findChildIndexCallback: findIndex,
                    gridDelegate: SliverGridDelegateWithFixedCrossAxisCount(
                      crossAxisCount: columns,
                      crossAxisSpacing: columnGap,
                      mainAxisSpacing: rowGap,
                      mainAxisExtent: tileHeight,
                    ),
                    itemBuilder: (context, index) {
                      final game = items[index];
                      return _GameCard(
                        key: ValueKey(game.id),
                        movement: movement(game.id),
                        book: book,
                        game: game,
                        compact: compact,
                        captionHeight: captionHeight,
                      );
                    },
                  ),
                )
              else
                SliverPadding(
                  padding: padding.add(const EdgeInsets.only(bottom: 16)),
                  sliver: SliverList.builder(
                    itemCount: items.length,
                    findChildIndexCallback: findIndex,
                    itemBuilder: (context, index) {
                      final game = items[index];
                      return GameRow(
                        key: ValueKey(game.id),
                        movement: movement(game.id),
                        book: book,
                        game: game,
                        open: () => openLibraryGame(context, game.id),
                      );
                    },
                  ),
                ),
            ],
          ),
        );
      },
    ),
  );
}

class _BookSectionTitle extends StatelessWidget {
  const _BookSectionTitle(this.label);
  final String label;

  @override
  Widget build(BuildContext context) => Text(
    label,
    style: Theme.of(context).textTheme.titleMedium?.copyWith(
      fontSize: MediaQuery.sizeOf(context).width <= launcherCompactWidth
          ? 17
          : 20,
      fontWeight: FontWeight.w600,
      letterSpacing: 0,
      height: 1.3,
    ),
  );
}

class _RecentGames extends StatelessWidget {
  const _RecentGames({required this.games, required this.compact});

  final List<Game> games;
  final bool compact;

  @override
  Widget build(BuildContext context) => Column(
    crossAxisAlignment: CrossAxisAlignment.start,
    spacing: compact ? 16 : 14,
    children: [
      Semantics(
        header: true,
        child: _BookSectionTitle(context.strings.recentGames),
      ),
      LayoutBuilder(
        builder: (context, constraints) {
          final width = compact
              ? ((constraints.maxWidth - 36) / 4).clamp(1.0, double.infinity)
              : 230.0;
          return SizedBox(
            height: compact ? width : _recentBookHeight(context, games),
            child: ListView.separated(
              key: const PageStorageKey('recently played games'),
              primary: false,
              scrollDirection: Axis.horizontal,
              itemCount: games.length,
              separatorBuilder: (_, _) => SizedBox(width: compact ? 12 : 8),
              itemBuilder: (context, index) {
                final game = games[index];
                final cover = GameArtwork(
                  game: game,
                  size: compact ? null : 56,
                  borderRadius: prism.AppRadius.xs,
                );
                return SizedBox(
                  key: ValueKey(game.id),
                  width: width,
                  child: Material(
                    type: MaterialType.transparency,
                    borderRadius: BorderRadius.circular(8),
                    clipBehavior: Clip.antiAlias,
                    child: InkWell(
                      onTap: () => openLibraryGame(context, game.id),
                      child: Semantics(
                        button: true,
                        child: compact
                            ? Semantics(
                                label: '${game.title} ${game.displayCode}',
                                child: cover,
                              )
                            : Padding(
                                padding: const EdgeInsets.all(6),
                                child: Row(
                                  spacing: 10,
                                  children: [
                                    cover,
                                    Expanded(
                                      child: Column(
                                        mainAxisAlignment:
                                            MainAxisAlignment.center,
                                        crossAxisAlignment:
                                            CrossAxisAlignment.start,
                                        children: [
                                          _GameTitleLine(
                                            game,
                                            style: const TextStyle(
                                              fontSize: 12,
                                              fontWeight: FontWeight.w500,
                                              height: 1.25,
                                            ),
                                          ),
                                          Text(
                                            game.subtitle,
                                            style: Theme.of(context)
                                                .textTheme
                                                .bodySmall
                                                ?.copyWith(
                                                  fontSize: 11,
                                                  height: 1.45,
                                                ),
                                            maxLines: 1,
                                            overflow: TextOverflow.ellipsis,
                                          ),
                                        ],
                                      ),
                                    ),
                                  ],
                                ),
                              ),
                      ),
                    ),
                  ),
                );
              },
            ),
          );
        },
      ),
    ],
  );
}

class _LibraryHeader extends StatelessWidget {
  const _LibraryHeader({
    required this.book,
    required this.compact,
    required this.count,
  });
  final BookState book;
  final bool compact;
  final int count;

  @override
  Widget build(BuildContext context) {
    final theme = Theme.of(context);
    final strings = context.strings;
    final title = Row(
      spacing: 8,
      crossAxisAlignment: CrossAxisAlignment.baseline,
      textBaseline: TextBaseline.alphabetic,
      children: [
        Flexible(child: _BookSectionTitle(book.category.label(strings))),
        Text(
          strings.gameCount(count),
          style: theme.textTheme.labelSmall?.copyWith(
            letterSpacing: 0,
            color: theme.colorScheme.onSurfaceVariant,
          ),
        ),
      ],
    );
    LauncherSegments<bool> mode(bool showLabels) => LauncherSegments<bool>(
      key: const ValueKey('library view control'),
      segments: [
        ButtonSegment(
          value: true,
          icon: const Icon(LucideIcons.layoutGrid),
          label: showLabels ? Text(strings.coverView) : null,
          tooltip: showLabels ? null : strings.coverView,
        ),
        ButtonSegment(
          value: false,
          icon: const Icon(LucideIcons.list),
          label: showLabels ? Text(strings.listView) : null,
          tooltip: showLabels ? null : strings.listView,
        ),
      ],
      value: book.covers,
      onChanged: book.changeView,
    );
    final sort = LauncherSelect<GameOrder>(
      key: const ValueKey('library sort control'),
      label: book.order.label(strings),
      value: book.order,
      expand: true,
      labels: {
        for (final order in GameOrder.values) order: order.label(strings),
      },
      onChanged: (value) {
        if (value != null) book.changeOrder(value);
      },
    );
    return LayoutBuilder(
      builder: (context, constraints) {
        double measure(String label) =>
            LauncherControl.labelWidth(context, label);

        final coverWidth = measure(strings.coverView);
        final listWidth = measure(strings.listView);
        final viewWidth =
            2 * (coverWidth > listWidth ? coverWidth : listWidth) + 92;
        final sortLabelWidth =
            GameOrder.values
                .map((order) => measure(order.label(strings)))
                .reduce((a, b) => a > b ? a : b) +
            44;

        final width = constraints.maxWidth;
        if (compact) {
          return Column(
            crossAxisAlignment: CrossAxisAlignment.start,
            spacing: 10,
            children: [
              title,
              CategoryRail(book: book),
              Row(
                spacing: 8,
                children: [
                  mode(width >= viewWidth + sortLabelWidth + 8),
                  Expanded(child: sort),
                ],
              ),
            ],
          );
        }

        final sortWidth = sortLabelWidth.clamp(
          120.0,
          (width * .4).clamp(120.0, 224.0),
        );
        return Row(
          spacing: 12,
          children: [
            Expanded(child: title),
            mode(width - sortWidth >= viewWidth + 132),
            SizedBox(width: sortWidth, child: sort),
          ],
        );
      },
    );
  }
}

class _GameCard extends StatelessWidget {
  const _GameCard({
    super.key,
    required this.book,
    required this.game,
    required this.movement,
    required this.compact,
    required this.captionHeight,
  });
  final BookState book;
  final Game game;
  final Animation<Offset> movement;
  final bool compact;
  final double captionHeight;

  @override
  Widget build(BuildContext context) => SlideTransition(
    position: movement,
    textDirection: Directionality.of(context),
    child: _BookTileHover(builder: _tile),
  );

  Widget _tile(BuildContext context, bool hovered) =>
      compact ? _mobileTile(context, hovered) : _desktopTile(context, hovered);

  Widget _desktopTile(BuildContext context, bool hovered) {
    final theme = Theme.of(context);
    return Material(
      color: LauncherBackdrop.tint(
        context,
        BackgroundLayer.card,
        theme.colorScheme.surfaceContainerLow,
      ),
      shape: prism.AppShape.sm.copyWith(
        side: BorderSide(color: theme.colorScheme.outlineVariant),
      ),
      clipBehavior: Clip.antiAlias,
      child: Stack(
        fit: StackFit.expand,
        children: [
          InkWell(
            onTap: () => openLibraryGame(context, game.id),
            child: Semantics(
              button: true,
              child: Column(
                crossAxisAlignment: CrossAxisAlignment.stretch,
                children: [
                  AspectRatio(
                    aspectRatio: 1,
                    child: GameArtwork(
                      game: game,
                      borderRadius: BorderRadius.zero,
                    ),
                  ),
                  Expanded(
                    child: Padding(
                      padding: const EdgeInsets.all(12),
                      child: Column(
                        crossAxisAlignment: CrossAxisAlignment.start,
                        mainAxisAlignment: MainAxisAlignment.center,
                        spacing: 5,
                        children: [
                          DefaultTextStyle.merge(
                            style: theme.textTheme.bodyMedium?.copyWith(
                              fontSize: 14,
                              fontWeight: FontWeight.w600,
                              height: 1.4,
                            ),
                            child: _GameTitleLine(game),
                          ),
                          Text(
                            game.subtitle,
                            maxLines: 1,
                            overflow: TextOverflow.ellipsis,
                            style: theme.textTheme.bodySmall?.copyWith(
                              fontSize: 11,
                              height: 1.45,
                              color: theme.colorScheme.onSurfaceVariant,
                            ),
                          ),
                        ],
                      ),
                    ),
                  ),
                ],
              ),
            ),
          ),
          PositionedDirectional(
            top: 8,
            end: 8,
            child: Material(
              color: theme.colorScheme.surfaceContainerLow,
              shape: const CircleBorder(),
              child: FavoriteButton(
                book: book,
                game: game,
                tileHovered: hovered,
                compact: true,
              ),
            ),
          ),
        ],
      ),
    );
  }

  Widget _mobileTile(BuildContext context, bool hovered) => Column(
    children: [
      AspectRatio(
        aspectRatio: 1,
        child: Material(
          shape: prism.AppShape.sm,
          clipBehavior: Clip.antiAlias,
          child: InkWell(
            onTap: () => openLibraryGame(context, game.id),
            child: Semantics(
              button: true,
              label: '${game.title} ${game.displayCode}',
              child: GameArtwork(game: game, borderRadius: BorderRadius.zero),
            ),
          ),
        ),
      ),
      DecoratedBox(
        decoration: BoxDecoration(
          color: LauncherBackdrop.active(context)
              ? LauncherBackdrop.tint(
                  context,
                  BackgroundLayer.card,
                  Theme.of(context).colorScheme.surfaceContainerLow,
                )
              : Colors.transparent,
          borderRadius: const BorderRadius.vertical(bottom: Radius.circular(8)),
        ),
        child: SizedBox(
          height: captionHeight,
          child: Row(
            children: [
              Expanded(
                child: Padding(
                  padding: const EdgeInsetsDirectional.only(end: 4),
                  child: Wrap(
                    spacing: 4,
                    crossAxisAlignment: WrapCrossAlignment.center,
                    children: [
                      Text(
                        game.title,
                        maxLines: 1,
                        overflow: TextOverflow.ellipsis,
                        style: LauncherControl.label(context),
                      ),
                      Text(
                        game.displayCode,
                        style: Theme.of(context).textTheme.labelSmall?.copyWith(
                          letterSpacing: 0,
                          color: Theme.of(context).colorScheme.onSurfaceVariant,
                        ),
                      ),
                    ],
                  ),
                ),
              ),
              FavoriteButton(book: book, game: game, tileHovered: hovered),
            ],
          ),
        ),
      ),
    ],
  );
}

double _bookCardCaptionHeight(BuildContext context) {
  final scaler = MediaQuery.textScalerOf(context);
  return (scaler.scale(14) * 1.4).ceilToDouble() +
      (scaler.scale(11) * 1.45).ceilToDouble() +
      29;
}

double _bookTextHeight(BuildContext context, InlineSpan content) {
  final painter = TextPainter(
    text: TextSpan(
      style: DefaultTextStyle.of(context).style,
      children: [content],
    ),
    textDirection: Directionality.of(context),
    textScaler: MediaQuery.textScalerOf(context),
    maxLines: 1,
  )..layout();
  final height = painter.height.ceilToDouble();
  painter.dispose();
  return height;
}

double _mobileBookCaptionHeight(BuildContext context, List<Game> games) {
  final codeStyle = Theme.of(context).textTheme.labelSmall
      ?.copyWith(letterSpacing: 0);
  return games.fold(44.0, (height, game) {
    // The number may wrap below the single-line title at narrow widths.
    final required =
        _bookTextHeight(
          context,
          TextSpan(text: game.title, style: LauncherControl.label(context)),
        ) +
        _bookTextHeight(
          context,
          TextSpan(text: game.displayCode, style: codeStyle),
        ) +
        8;
    return required > height ? required : height;
  });
}

double _recentBookHeight(BuildContext context, List<Game> games) {
  final theme = Theme.of(context);
  return games.fold(68.0, (height, game) {
    final title = _bookTextHeight(
      context,
      TextSpan(
        text: game.title,
        style: const TextStyle(
          fontSize: 12,
          fontWeight: FontWeight.w500,
          height: 1.25,
        ),
        children: [
          TextSpan(
            text: game.displayCode,
            style: theme.textTheme.labelSmall?.copyWith(
              fontSize: 11,
              fontWeight: FontWeight.w400,
              letterSpacing: 0,
            ),
          ),
        ],
      ),
    );
    final subtitle = _bookTextHeight(
      context,
      TextSpan(
        text: game.subtitle,
        style: theme.textTheme.bodySmall?.copyWith(fontSize: 11, height: 1.45),
      ),
    );
    final required = title + subtitle + 12;
    return required > height ? required : height;
  });
}
