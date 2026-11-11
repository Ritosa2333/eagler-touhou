import {th19PlayerStats} from './th19-room-stats.mjs';
import {randomUUID} from 'node:crypto';

function currentSeats(room, ids) {
  return ids?.length === 2 && ids[0] !== ids[1] && ids.every((id, side) => id && room.lobby.seats?.[side]?.clientId === id);
}

export function reconcileTh09Duel(room) {
  if (room.th09Duel && !currentSeats(room, room.th09Duel.clientIds)) room.th09Duel = null;
}

export function bindTh09Duel(room, run, players = null) {
  if (!room.id.startsWith('th09mp-') || !currentSeats(room, run.lobbyClientIds)) return;
  reconcileTh09Duel(room);
  const duel = room.th09Duel;
  if (!duel || players?.some((player, side) => player !== duel.players?.[side])) {
    room.th09Duel = {id: randomUUID(), startedAt: Date.now(), clientIds: [...run.lobbyClientIds], players, wins: [0, 0], frames: 0};
  }
  run.th09Duel = room.th09Duel;
}

export function th09PlayerStats(room, clientId) {
  if (room.id.startsWith('th19mp-')) return th19PlayerStats(room, clientId);
  if (!room.id.startsWith('th09mp-')) return undefined;
  const duel = room.th09Duel;
  const side = duel && currentSeats(room, duel.clientIds) ? duel.clientIds.indexOf(clientId) : -1;
  return side < 0 ? {wins:0, losses:0, minutes:0} :
    {wins:duel.wins[side], losses:duel.wins[1 - side], minutes:Math.floor(duel.frames / 3600)};
}

function confirmTh09Result(run, side, message, recordRound, active) {
  const duel = run.th09Duel;
  const players = duel.players;
  if (!recordRound || !players || (players[0] && players[0] === players[1])) return;
  const {serial, start, frames, winner, characters} = message;
  const confirmations = run.th09Finals ||= [{last:-1, results:new Map()}, {last:-1, results:new Map()}];
  const own = confirmations[side];
  const peer = confirmations[1 - side].results.get(start);
  if (start <= own.last || (!active && !peer)) return;
  own.last = start;
  own.results.set(start, {frames, winner, characters});
  if (own.results.size > 16) own.results.delete(own.results.keys().next().value);
  if (peer?.frames !== frames || peer.winner !== winner) return;
  const bothHaveCharacters = characters && peer.characters;
  if (bothHaveCharacters && characters.some((character, index) => character !== peer.characters[index])) return;
  recordRound({duelId:duel.id, roundId:serial + ':' + start, startedAt:duel.startedAt, players, winner, frames,
    ...(bothHaveCharacters ? {characters} : {})});
}

// Legacy clients still supply the room's live scoreboard. Account history
// requires identical confirmed results from both admitted seats.
export function acceptTh09MatchStats(room, run, clientId, message, recordRound) {
  if (!room.id.startsWith('th09mp-') || !run?.th09Duel || message.serial !== room.lobby.startSerial) return false;
  const active = !run.ended && run.th09Duel === room.th09Duel && currentSeats(room, run.lobbyClientIds);
  const side = run.lobbyClientIds.indexOf(clientId);
  const {start, frames, winner, characters} = message;
  if (side < 0 || !Number.isInteger(start) || start < 0 || start > 0xffffffff ||
      !Number.isInteger(frames) || frames < 0 || frames > 60*60*60 ||
      start + frames > 0xffffffff || (winner !== undefined && (frames === 0 || (winner !== 0 && winner !== 1)))) return false;
  if (characters !== undefined && (winner === undefined || !Array.isArray(characters) || characters.length !== 2 ||
      characters.some(character => !Number.isInteger(character) || character < 0 || character >= 16))) return false;
  // A seat can disconnect after its report reaches the Relay. While this run
  // exists, the other bound seat may still complete that pending result.
  if (winner !== undefined) confirmTh09Result(run, side, message, recordRound, active);
  if (side !== 0 || !active) return false;
  let cursor = run.th09StatsCursor;
  if (cursor && (start < cursor.start || (start > cursor.start && start < cursor.start + cursor.frames))) return false;
  const newMatch = !cursor || start > cursor.start;
  if (newMatch) cursor = run.th09StatsCursor = {start,frames:0,ended:false};
  if (cursor.ended || frames < cursor.frames) return false;
  const delta = frames - cursor.frames;
  if (!delta && winner === undefined && !newMatch) return false;
  const duel = run.th09Duel;
  duel.frames += delta;
  cursor.frames = frames;
  cursor.ended = winner !== undefined;
  if (cursor.ended) duel.wins[winner]++;
  return true;
}
