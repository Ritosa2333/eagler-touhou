export function expectedDisconnectConsole(entry,disconnect) {
  const native=disconnect?.after?.[entry.seat];
  return disconnect?.observed===true&&Number.isFinite(disconnect.requestedCheckerMs)&&
    entry.checkerMs>=disconnect.requestedCheckerMs&&entry.kind==='console-error'&&native?.disconnected===true&&
    /^RTC peer P[1-3] input channel closed$/.test(native.error??'')&&
    entry.error?.split('\n')[0]===`Error: ${native.error}`;
}
