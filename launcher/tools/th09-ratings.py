"""Build a th09mp WHR board for the launcher development preview."""

import argparse
from collections import Counter, defaultdict
import json
import math
from pathlib import Path
import re
from tempfile import TemporaryDirectory

import numpy as np


DAY = 86_400_000
CENTER = 1500
SCALE = 400 / math.log(10)
INITIAL_RD = 350
VARIANCE_PER_DAY = 1000
ESTABLISHED_RD = 110
INACTIVE_DAYS = 90
DEFAULT_MIN_GAMES = 10
PUBLIC_USERNAME = re.compile(r'[A-Za-z0-9._]{3,12}')
AVATAR_THUMBNAIL = re.compile(r'data:image/(?:webp|jpeg);base64,[A-Za-z0-9+/]+={0,2}')


def estimate(matches, as_of):
    played_on = defaultdict(set)
    last_at = {}
    opponents = defaultdict(set)
    counts = Counter()
    wins = Counter()
    for first, second, at, winner in matches:
        for player, opponent in ((first, second), (second, first)):
            played_on[player].add(at // DAY)
            last_at[player] = max(last_at.get(player, 0), at)
            opponents[player].add(opponent)
            counts[player] += 1
        wins[(first, second)[winner]] += 1
    if not played_on:
        return []

    days = {player: sorted(played_on[player]) for player in sorted(played_on)}
    nodes = [(player, day) for player in days for day in days[player]]
    index = {node: i for i, node in enumerate(nodes)}
    prior = np.zeros((len(nodes), len(nodes)))
    for player, history in days.items():
        initial = index[player, history[0]]
        prior[initial, initial] = (SCALE / INITIAL_RD) ** 2
        for previous, current in zip(history, history[1:]):
            i, j = index[player, previous], index[player, current]
            precision = SCALE ** 2 / (VARIANCE_PER_DAY * (current - previous))
            prior[i, i] += precision
            prior[j, j] += precision
            prior[i, j] -= precision
            prior[j, i] -= precision

    design = np.zeros((len(matches), len(nodes)))
    outcomes = np.array([1 - winner for _, _, _, winner in matches])
    for i, (first, second, at, _) in enumerate(matches):
        day = at // DAY
        design[i, index[first, day]] = 1
        design[i, index[second, day]] = -1

    def loss(scores):
        margin = design @ scores
        return (np.logaddexp(0, margin) - outcomes * margin).sum() + scores @ prior @ scores / 2

    scores = np.zeros(len(nodes))
    for _ in range(60):
        probabilities = 1 / (1 + np.exp(-np.clip(design @ scores, -700, 700)))
        gradient = design.T @ (probabilities - outcomes) + prior @ scores
        curvature = probabilities * (1 - probabilities)
        hessian = design.T @ (curvature[:, None] * design) + prior
        if np.max(np.abs(gradient)) < 1e-8:
            break
        step = np.linalg.solve(hessian, gradient)
        old_loss = loss(scores)
        for backoff in range(25):
            candidate = scores - step / (2 ** backoff)
            if loss(candidate) <= old_loss + 1e-12:
                scores = candidate
                break
        else:
            raise RuntimeError('WHR line search did not converge')
    else:
        raise RuntimeError('WHR did not converge')

    groups = connected_groups(opponents)
    rows = []
    for player, history in days.items():
        indices = [index[player, day] for day in history]
        # WHR estimates this player's deviation with opponent ratings fixed.
        precision = hessian[np.ix_(indices, indices)]
        variance = np.linalg.inv(precision)[-1, -1] * SCALE ** 2
        age = (as_of - last_at[player]) / DAY
        rd = min(INITIAL_RD, math.sqrt(variance + VARIANCE_PER_DAY * age))
        state = 'inactive' if age >= INACTIVE_DAYS else 'active' if rd < ESTABLISHED_RD else 'provisional'
        rows.append({
            'player': player,
            'rating': CENTER + scores[indices[-1]] * SCALE,
            'interval_halfwidth': round(2 * rd, 1),
            'status': state,
            'games': counts[player],
            'wins': wins[player],
            'losses': counts[player] - wins[player],
            'opponents': len(opponents[player]),
            'connected_group': groups[player],
            'last_at': last_at[player],
        })

    rows.sort(key=lambda row: (-row['rating'], row['player']))
    for row in rows:
        row['rating'] = round(row['rating'], 1)
    return rows


def connected_groups(opponents):
    remaining = set(opponents)
    groups = []
    while remaining:
        pending = [min(remaining)]
        group = set()
        while pending:
            player = pending.pop()
            if player in remaining:
                remaining.remove(player)
                group.add(player)
                pending.extend(opponents[player] & remaining)
        groups.append(group)
    groups.sort(key=lambda group: (-len(group), min(group)))
    return {player: i for i, group in enumerate(groups, 1) for player in group}


def build_board(snapshot):
    as_of, games = snapshot['as_of'], snapshot['games']
    if type(as_of) is not int or as_of <= 0 or not isinstance(games, list):
        raise ValueError('expected a snapshot timestamp and a list of matches')
    matches = []
    character_counts = defaultdict(Counter)
    for game in games:
        if not isinstance(game, list) or len(game) not in (4, 6):
            raise ValueError('expected a four-field match with an optional pair of characters')
        first, second, at, winner = game[:4]
        if (not isinstance(first, str) or not isinstance(second, str) or not first or not second
                or first == second or type(at) is not int or not 0 < at <= as_of
                or type(winner) is not int or winner not in (0, 1)):
            raise ValueError('expected two distinct player IDs, a match timestamp, and winning seat 0 or 1')
        matches.append((first, second, at, winner))
        for player, character in zip((first, second), game[4:]):
            if character is not None:
                if type(character) is not int or not 0 <= character <= 15:
                    raise ValueError('expected a character ID from 0 to 15 or null')
                character_counts[player][character] += 1

    profiles = snapshot.get('profiles')
    if 'profiles' in snapshot and not isinstance(profiles, dict):
        raise ValueError('expected an object of public profiles keyed by player ID')
    rows = []
    rank = 0
    for row in estimate(matches, as_of):
        player = row['player']
        if profiles is not None:
            profile = profiles.get(player)
            if profile is None:
                continue
            username, nickname = profile['username'], profile['nickname']
            if not isinstance(username, str) or not PUBLIC_USERNAME.fullmatch(username) or not isinstance(nickname, str) or not nickname:
                raise ValueError('expected a public username and nickname')
            avatar = profile.get('avatar')
            row['profile'] = {'username': username, 'nickname': nickname}
            if isinstance(avatar, str) and len(avatar) <= 11_000 and AVATAR_THUMBNAIL.fullmatch(avatar):
                row['profile']['avatar'] = avatar
        most_used = sorted(character_counts[player].items(), key=lambda pair: (-pair[1], pair[0]))[:3]
        row['top_characters'] = [{'character': character, 'games': uses} for character, uses in most_used]
        if row['status'] == 'active':
            rank += 1
            row['rank'] = rank
        else:
            row['rank'] = None
        rows.append(row)
    return {
        'as_of': as_of,
        'rated_games': len(matches),
        'unique_pairs': len({tuple(sorted((first, second))) for first, second, _, _ in matches}),
        'default_min_games': DEFAULT_MIN_GAMES,
        'board': rows,
    }


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('snapshot', type=Path)
    parser.add_argument('--output', type=Path, default=Path(__file__).resolve().parents[1] / '.cache/th09-ratings.json')
    args = parser.parse_args()
    try:
        snapshot = json.loads(args.snapshot.read_text(encoding='utf-8-sig'))
        board = build_board(snapshot)
    except (TypeError, KeyError, ValueError) as error:
        parser.error(str(error))
    args.output.parent.mkdir(parents=True, exist_ok=True)
    with TemporaryDirectory(dir=args.output.parent) as staging:
        prepared = Path(staging) / args.output.name
        prepared.write_text(json.dumps(board, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')
        prepared.replace(args.output)
    print(f'{board["rated_games"]} matches, {len(board["board"])} players | {args.output}')


if __name__ == '__main__':
    main()
