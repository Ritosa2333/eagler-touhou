// Room-scoped panel statistics use the confirmed progress of both seats.
export function th19PlayerStats(room, clientId) {
  if (!/^th19mp-/.test(room.id)) return undefined;
  const value = room.th19Stats?.get(clientId);
  const frames = value?.frames || 0;
  return {wins:value?.wins || 0, losses:value?.losses || 0, minutes:Math.floor(frames/3600)};
}

export function acceptTh19MatchStats(room, clientId, message) {
  const run = room.th19StatsRun;
  if (!run || message.serial !== run.serial || message.serial !== room.lobby.startSerial ||
      run.clientIds.length !== 2 || run.clientIds[0] === run.clientIds[1] ||
      !run.clientIds.every((id, side) => id && room.lobby.seats[side]?.clientId === id)) return false;
  const side = run.clientIds.indexOf(clientId);
  const {start, frames, winner} = message;
  if (side < 0 || start !== 0 ||
      !Number.isInteger(frames) || frames < 0 || frames > 60*60*60 ||
      (winner !== undefined && (frames === 0 || (winner !== 0 && winner !== 1)))) return false;
  const previous = run.reports[side], peer = run.reports[1-side];
  const previousFrames = previous?.frames ?? 0;
  if (previous?.winner !== undefined || frames < previousFrames ||
      (frames === previousFrames && winner === undefined)) return false;
  run.reports[side] = {frames,winner};
  const peerFrames = peer?.frames ?? 0;
  const delta = Math.min(frames, peerFrames) - Math.min(previousFrames, peerFrames);
  const settled = winner !== undefined && winner === peer?.winner && frames === peerFrames;
  if (delta === 0 && !settled) return false;
  const stats = room.th19Stats ??= new Map();
  for (const [index,id] of run.clientIds.entries()) {
    let value = stats.get(id);
    if (!value) {value={wins:0,losses:0};stats.set(id,value);}
    value.frames = (value.frames || 0) + delta;
    if (settled) value[index === winner ? 'wins' : 'losses']++;
  }
  // Bound departed-member records while retaining the current two players.
  for (const id of stats.keys()) {
    if (stats.size <= 64) break;
    if (!run.clientIds.includes(id)) stats.delete(id);
  }
  return true;
}
