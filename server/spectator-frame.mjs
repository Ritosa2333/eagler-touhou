// Only the room's declared game protocol may enter its spectator backlog.
export function isSpectatorFrameForRoom(roomId, payload, playerCount) {
  if (!payload || payload[6] !== playerCount) return false;
  if (roomId.startsWith('th09mp-')) return playerCount === 2 &&
    payload.length === 46 && payload[0] === 0x54 && payload[1] === 0x39 &&
    payload[2] === 0x53 && payload[3] === 0x50 && payload[4] === 1 && payload[5] === 3 &&
    payload[7] === 0;
  return (playerCount === 2 || playerCount === 3) &&
    payload.length === 24 + playerCount * 12 &&
    payload[0] === 0x45 && (payload[1] === 0x36 || payload[1] === 0x37) &&
    payload[2] === 0x4e && payload[3] === 0x50 && payload[4] === 4 &&
    payload[5] === 3 && payload[7] === 0;
}
