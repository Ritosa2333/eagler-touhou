// Room-scoped panel statistics. The host reports confirmed match boundaries;
// no gameplay packets, state snapshots or per-frame server work are involved.
export function th19PlayerStats(room, clientId) {
  if (!/^th19mp-/.test(room.id)) return undefined;
  const value = room.th19Stats?.get(clientId);
  const frames = value?.frames || 0;
  return {wins:value?.wins || 0, losses:value?.losses || 0, minutes:Math.floor(frames/3600)};
}

export function acceptTh19MatchStats(room, run, clientId, message) {
  if (!/^th19mp-/.test(room.id) || !run || run.ended ||
      run.lobbyClientIds?.[0] !== clientId || run.lobbyClientIds.length !== 2 ||
      message.serial !== room.lobby.startSerial) return false;
  const {start, frames, winner} = message;
  if (start !== 0 ||
      !Number.isInteger(frames) || frames < 0 || frames > 60*60*60 ||
      (winner !== undefined && (frames === 0 || (winner !== 0 && winner !== 1)))) return false;
  // Each lobby run contains one match whose confirmed frame counter starts at zero.
  const cursor = run.th19StatsCursor;
  const delta = frames - (cursor?.frames ?? 0);
  if (cursor?.ended || delta < 0 || (cursor && delta === 0 && winner === undefined)) return false;
  const stats = room.th19Stats ??= new Map();
  for (const [index,id] of run.lobbyClientIds.entries()) {
    let value = stats.get(id);
    if (!value) {value={wins:0,losses:0};stats.set(id,value);}
    value.frames = (value.frames || 0) + delta;
    if (winner !== undefined) value[index === winner ? 'wins' : 'losses']++;
  }
  run.th19StatsCursor = {frames,ended:winner !== undefined};
  // Bound departed-member records while retaining the current two players.
  for (const id of stats.keys()) {
    if (stats.size <= 64) break;
    if (!run.lobbyClientIds.includes(id)) stats.delete(id);
  }
  return true;
}
