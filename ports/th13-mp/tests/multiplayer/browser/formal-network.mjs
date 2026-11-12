// Link conditions affect only bytes sent by the real transport. The reliable
// repair channel stays available when input datagrams are deliberately lost.
export function installWireConditions({
  route = "rtc",
  rtt = 0,
  jitter = 0,
  loss = 0,
  seed = 0x13130001,
  duplicateEvery = 0,
  burstEvery = 0,
  burstMs = 0,
}) {
  if (route === "relay") globalThis.RTCPeerConnection = undefined;
  const stats = (globalThis.__wireConditions = {
    route,
    rtt,
    jitter,
    loss,
    seed,
    duplicateEvery,
    burstEvery,
    burstMs,
    sent: 0,
    delivered: 0,
    dropped: 0,
    duplicated: 0,
    reordered: 0,
    bursts: 0,
    records: [],
  });
  let lastDelivered = 0;
  const random = () =>
    (seed = (Math.imul(seed, 1664525) + 1013904223) >>> 0) / 4294967296;
  const wrap = (proto, name) => {
    if (!proto) return;
    const original = proto.send;
    proto.send = function (data) {
      if (
        typeof data === "string" ||
        !(data instanceof ArrayBuffer || ArrayBuffer.isView(data))
      )
        return original.call(this, data);
      const bytes =
        data instanceof ArrayBuffer
          ? new Uint8Array(data.slice(0))
          : new Uint8Array(
              data.buffer.slice(
                data.byteOffset,
                data.byteOffset + data.byteLength,
              ),
            );
      const input = name === "rtc" && /-input$/.test(this.label || "");
      let delay = Math.max(0, rtt / 2 + (random() * 2 - 1) * jitter);
      const drop = input && random() < loss;
      if (burstEvery && stats.sent && stats.sent % burstEvery === 0) {
        delay += burstMs;
        ++stats.bursts;
      }
      const n = ++stats.sent,
        record = {
          n,
          t: performance.now(),
          delay,
          drop,
          channel: this.label || this.url,
          size: bytes.length,
        };
      if (stats.records.length < 2048) stats.records.push(record);
      if (drop) {
        ++stats.dropped;
        return;
      }
      const send = () => {
        if (this.readyState === "open" || this.readyState === 1) {
          original.call(this, bytes);
          record.deliveredAt = performance.now();
          record.delivery = ++stats.delivered;
          if (n < lastDelivered) ++stats.reordered;
          lastDelivered = n;
          if (duplicateEvery && n % duplicateEvery === 0) {
            original.call(this, bytes);
            ++stats.duplicated;
            record.duplicate = true;
          }
        }
      };
      if (delay) setTimeout(send, delay);
      else send();
    };
  };
  wrap(globalThis.RTCDataChannel?.prototype, "rtc");
  wrap(globalThis.WebSocket?.prototype, "ws");
}
