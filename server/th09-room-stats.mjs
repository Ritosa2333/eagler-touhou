// Room-scoped panel statistics. The host reports confirmed match boundaries;
// no gameplay packets, state snapshots or per-frame server work are involved.
export function th09PlayerStats(room, clientId) {
  if (!room.id.startsWith('th09mp-')) return undefined;
  const value = room.th09Stats?.get(clientId);
  const frames = value?.frames || 0;
  return {wins:value?.wins || 0, losses:value?.losses || 0, minutes:Math.floor(frames/3600)};
}

export function acceptTh09MatchStats(room, run, clientId, message) {
  if (!room.id.startsWith('th09mp-') || !run || run.ended ||
      run.lobbyClientIds?.[0] !== clientId || run.lobbyClientIds.length !== 2 ||
      message.serial !== room.lobby.startSerial) return false;
  const {start, frames, winner} = message;
  if (!Number.isInteger(start) || start < 0 || start > 0xffffffff ||
      !Number.isInteger(frames) || frames < 0 || frames > 60*60*60 ||
      start + frames > 0xffffffff || (winner !== undefined && winner !== 0 && winner !== 1)) return false;
  let cursor = run.th09StatsCursor;
  if (cursor && (start < cursor.start ||
      (start > cursor.start && start < cursor.start + cursor.frames))) return false;
  const newMatch = !cursor || start > cursor.start;
  if (newMatch) cursor = run.th09StatsCursor = {start,frames:0,ended:false};
  if (cursor.ended || frames < cursor.frames || (winner !== undefined && frames === 0)) return false;
  const delta = frames - cursor.frames;
  if (!delta && winner === undefined && !newMatch) return false;
  room.th09Stats ||= new Map();
  for (const [index,id] of run.lobbyClientIds.entries()) {
    let value = room.th09Stats.get(id);
    if (!value) {value={wins:0,losses:0};room.th09Stats.set(id,value);}
    value.frames = (value.frames || 0) + delta;
    if (winner !== undefined) value[index === winner ? 'wins' : 'losses']++;
  }
  cursor.frames = frames; cursor.ended = winner !== undefined;
  room.th09CurrentMatch = {serial:message.serial,start,frames,clientIds:[...run.lobbyClientIds]};
  // Bound departed-member records while retaining the current two players.
  for (const id of room.th09Stats.keys()) {
    if (room.th09Stats.size <= 64) break;
    if (!run.lobbyClientIds.includes(id)) room.th09Stats.delete(id);
  }
  return true;
}
