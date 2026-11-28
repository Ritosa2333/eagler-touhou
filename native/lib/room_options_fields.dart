import 'package:intl/intl.dart';
import 'package:material_ui/material_ui.dart';
import 'package:prism_ui/prism_ui.dart' as prism;

import 'catalog.dart';
import 'l10n/strings.dart';
import 'launcher_select.dart';
import 'room_session.dart';

RoomOptions initialRoomOptions(Game game) {
  final rules = game.multiplayerRules;
  return (
    players: rules?.playerCounts.first ?? 2,
    difficulty: game.id == 'th03' ? 3 : 1,
    private: false,
    limitMovement: false,
    unrated: false,
  );
}

String roomDifficulty(Game game, int index) {
  final values =
      game.multiplayerRules?.difficulties ??
      const ['Easy', 'Normal', 'Hard', 'Lunatic', 'Extra'];
  return values.isEmpty ? '—' : values[index.clamp(0, values.length - 1)];
}

class RoomOptionsFields extends StatelessWidget {
  const RoomOptionsFields({
    super.key,
    required this.game,
    required this.value,
    required this.onChanged,
  });

  final Game game;
  final RoomOptions value;
  final ValueChanged<RoomOptions> onChanged;

  @override
  Widget build(BuildContext context) {
    final strings = context.strings;
    final policy = game.multiplayerRules;
    final counts = policy?.playerCounts ?? const [2, 3];
    final difficulties =
        policy?.difficulties ??
        const ['Easy', 'Normal', 'Hard', 'Lunatic', 'Extra'];
    return Column(
      mainAxisSize: MainAxisSize.min,
      crossAxisAlignment: CrossAxisAlignment.stretch,
      spacing: 10,
      children: [
        _RoomChoice(
          label: strings.roomCapacity,
          value: value.players,
          choices: {
            for (final count in counts) count: strings.roomPlayerCount(count),
          },
          onChanged: counts.length <= 1
              ? null
              : (count) => onChanged((
                  players: count ?? value.players,
                  difficulty: value.difficulty,
                  private: value.private,
                  limitMovement: value.limitMovement,
                  unrated: value.unrated,
                )),
        ),
        if (policy?.selectInGame != true)
          _RoomChoice(
            label: strings.roomDifficultyLabel,
            value: value.difficulty.clamp(0, difficulties.length - 1),
            choices: {
              for (final (index, label) in difficulties.indexed) index: label,
            },
            onChanged: (index) => onChanged((
              players: value.players,
              difficulty: index ?? value.difficulty,
              private: value.private,
              limitMovement: value.limitMovement,
              unrated: value.unrated,
            )),
          ),
        SwitchListTile(
          contentPadding: EdgeInsets.zero,
          title: Text(strings.roomPrivateOption),
          value: value.private,
          onChanged: (selected) => onChanged((
            players: value.players,
            difficulty: value.difficulty,
            private: selected,
            limitMovement: value.limitMovement,
            unrated: value.unrated,
          )),
        ),
        SwitchListTile(
          contentPadding: EdgeInsets.zero,
          title: Text(strings.roomLimitMovement),
          value: value.limitMovement,
          onChanged: (selected) => onChanged((
            players: value.players,
            difficulty: value.difficulty,
            private: value.private,
            limitMovement: selected,
            unrated: value.unrated,
          )),
        ),
        if (game.id == 'th09')
          SwitchListTile(
            contentPadding: EdgeInsets.zero,
            title: Text(strings.roomUnrated),
            value: value.unrated,
            onChanged: (selected) => onChanged((
              players: value.players,
              difficulty: value.difficulty,
              private: value.private,
              limitMovement: value.limitMovement,
              unrated: selected,
            )),
          ),
      ],
    );
  }
}

class RoomTimingFields extends StatelessWidget {
  const RoomTimingFields({
    super.key,
    required this.policy,
    required this.value,
    required this.onChanged,
  });

  final RoomTimingPolicy policy;
  final RoomTiming value;
  final ValueChanged<RoomTiming> onChanged;

  @override
  Widget build(BuildContext context) {
    final strings = context.strings;
    final frames = value.frames;
    final adjust = !policy.measuredStartup || frames != null;
    return Column(
      crossAxisAlignment: CrossAxisAlignment.stretch,
      children: [
        const Divider(),
        Text(strings.roomTiming, style: Theme.of(context).textTheme.titleSmall),
        if (policy.rollback)
          SwitchListTile(
            contentPadding: EdgeInsets.zero,
            title: Text(strings.roomRollback),
            subtitle: Text(strings.roomRollbackHint),
            value: value.rollback,
            onChanged: (enabled) =>
                onChanged((rollback: enabled, frames: frames)),
          ),
        if (!value.rollback && !policy.runtimeTiming) ...[
          Row(
            spacing: 8,
            children: [
              Expanded(child: Text(strings.roomInputDelay)),
              prism.TonalButtonGroup(
                size: prism.TonalButtonSize.bar,
                children: [
                  prism.IconButton(
                    tooltip: strings.roomDecreaseDelay,
                    onPressed: !adjust || (frames ?? 0) == 0
                        ? null
                        : () =>
                              onChanged((rollback: false, frames: frames! - 1)),
                    icon: const Icon(Icons.remove, size: 16),
                  ),
                  if (policy.measuredStartup)
                    SizedBox(
                      width: 44,
                      child: Text(
                        frames == null ? '—' : '${frames}f',
                        textAlign: TextAlign.center,
                      ),
                    )
                  else
                    prism.TextButton(
                      onPressed: frames == null
                          ? null
                          : () => onChanged((rollback: false, frames: null)),
                      child: Tooltip(
                        message: strings.roomResetAutoDelay,
                        child: Text(
                          frames == null
                              ? strings.roomTimingAuto
                              : '${frames}f',
                        ),
                      ),
                    ),
                  prism.IconButton(
                    tooltip: strings.roomIncreaseDelay,
                    onPressed:
                        !adjust || (frames ?? 0) >= policy.manualDelayLimit
                        ? null
                        : () => onChanged((
                            rollback: false,
                            frames: (frames ?? 0) + 1,
                          )),
                    icon: const Icon(Icons.add, size: 16),
                  ),
                ],
              ),
            ],
          ),
          if (policy.measuredStartup)
            SwitchListTile(
              contentPadding: EdgeInsets.zero,
              title: Text(strings.roomInputDelayAutoToggle),
              value: frames == null,
              onChanged: (enabled) =>
                  onChanged((rollback: false, frames: enabled ? null : 0)),
            ),
        ],
      ],
    );
  }
}

class RoomRulesFields extends StatelessWidget {
  const RoomRulesFields({
    super.key,
    required this.game,
    required this.rules,
    required this.onChanged,
  });

  final Game game;
  final Map<RoomRule, Object> rules;
  final void Function(RoomRule, Object) onChanged;

  @override
  Widget build(BuildContext context) {
    final strings = context.strings;
    final challenge = rules[RoomRule.challengeMode] == true;
    final pc98Stock = game.id == 'th02' || game.id == 'th04';
    final numbers = NumberFormat.decimalPattern(strings.localeName);
    return Column(
      crossAxisAlignment: CrossAxisAlignment.stretch,
      spacing: 10,
      children: [
        const Divider(),
        Text(
          strings.roomRuleSettings,
          style: Theme.of(context).textTheme.titleSmall,
        ),
        for (final rule in RoomRule.values)
          if (rules[rule] case final Object value
              when rule != RoomRule.challengeKeepBullets || challenge)
            switch (value) {
              final bool enabled => SwitchListTile(
                key: ValueKey(rule),
                contentPadding: EdgeInsets.zero,
                title: Text(_label(strings, rule)),
                subtitle: switch (rule) {
                  RoomRule.challengeMode => Text(
                    pc98Stock
                        ? strings.roomPc98ChallengeHint
                        : strings.roomChallengeHint,
                  ),
                  RoomRule.challengeKeepBullets => Text(
                    strings.roomChallengeKeepBulletsHint,
                  ),
                  RoomRule.disableLocalChargePriority => Text(
                    strings.roomLocalChargeReplayHint,
                  ),
                  _ => null,
                },
                value: _inverted(rule) ? !enabled : enabled,
                onChanged: (selected) =>
                    onChanged(rule, _inverted(rule) ? !selected : selected),
              ),
              final int selected => _RoomChoice(
                key: ValueKey(rule),
                label: _label(strings, rule),
                value: selected,
                detail: rule == RoomRule.chapterThreshold
                    ? strings.roomTh15ChapterGraze(selected ~/ 5000)
                    : null,
                choices: {
                  for (final choice in rule.choices(game)!)
                    choice: numbers.format(choice),
                },
                onChanged:
                    pc98Stock &&
                        challenge &&
                        (rule == RoomRule.initialSpares ||
                            rule == RoomRule.initialBombs)
                    ? null
                    : (choice) {
                        if (choice != null) onChanged(rule, choice);
                      },
              ),
              _ => const SizedBox.shrink(),
            },
      ],
    );
  }

  bool _inverted(RoomRule rule) =>
      rule == RoomRule.disableLocalChargePriority ||
      (rule == RoomRule.disableFocus && game.id == 'th03');

  String _label(LauncherStrings strings, RoomRule rule) => switch (rule) {
    RoomRule.challengeMode => strings.roomChallengeMode,
    RoomRule.challengeKeepBullets => strings.roomChallengeKeepBullets,
    RoomRule.disableLocalChargePriority => strings.roomLocalChargePriority,
    RoomRule.showHitboxes => strings.roomAlwaysHitbox,
    RoomRule.disableFocus =>
      game.id == 'th03'
          ? strings.roomTh03SpecialFocus
          : strings.roomTh02DisableFocus,
    RoomRule.ballCount => strings.roomTh01BallCount,
    RoomRule.initialSpares => strings.roomTh05InitialSpares,
    RoomRule.initialBombs => strings.roomTh05InitialBombs,
    RoomRule.chapterThreshold => strings.roomTh15ChapterThreshold,
  };
}

class _RoomChoice extends StatelessWidget {
  const _RoomChoice({
    super.key,
    required this.label,
    required this.value,
    required this.choices,
    required this.onChanged,
    this.detail,
  });

  final String label;
  final int value;
  final Map<int, String> choices;
  final ValueChanged<int?>? onChanged;
  final String? detail;

  @override
  Widget build(BuildContext context) => Row(
    spacing: 12,
    children: [
      Expanded(
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          spacing: 2,
          children: [
            Text(label),
            if (detail case final String text)
              Text(text, style: Theme.of(context).textTheme.bodySmall),
          ],
        ),
      ),
      ConstrainedBox(
        constraints: const BoxConstraints(maxWidth: 120),
        child: LauncherSelect<int>(
          label: label,
          value: value,
          labels: choices,
          onChanged: onChanged,
          expand: true,
        ),
      ),
    ],
  );
}
