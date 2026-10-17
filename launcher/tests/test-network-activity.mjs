import assert from "node:assert/strict";
import { createNetworkActivityTracker } from "../.cache/build/browser/assets/launcher/network-activity.mjs";

const events = [];
let tick = 0;
let latestNativeResponse = null;
const delay = ms => new Promise(resolve => setTimeout(resolve, ms));
const fetchImpl = async () => (latestNativeResponse = new Response(new ReadableStream({
  start(controller) {
    controller.enqueue(new Uint8Array([1, 2, 3]));
    controller.enqueue(new Uint8Array([4, 5]));
    controller.close();
  },
}), { status: 200, headers: { "content-length": "5" } }));

const tracker = createNetworkActivityTracker({
  fetchImpl,
  clock: () => ++tick,
  onChange: value => events.push(value),
});

const response = await tracker.fetch("https://example.test/file.bin", {}, { label: "GAME DATA" });
assert.deepEqual([...new Uint8Array(await response.arrayBuffer())], [1, 2, 3, 4, 5]);
assert.equal(tracker.activeCount, 0);
assert.ok(events.some(event => event.count === 1 && event.active[0].phase === "requesting"));
assert.ok(events.some(event => event.active[0]?.total === 5));
assert.ok(events.some(event => event.active[0]?.loaded === 5));
assert.equal(events.at(-1).count, 0);

const directResponse = await tracker.fetch("https://example.test/direct-stream.bin", {}, { label: "DIRECT STREAM" });
const nativeBody = latestNativeResponse.body;
assert.equal(directResponse.body, nativeBody,
  "network tracker must expose the browser's original Response.body because Android WebView/Via can stall on a rebuilt stream");
assert.equal(tracker.activeCount, 0, "direct body consumers end generic progress ownership without wrapping the stream");
const reader = nativeBody.getReader();
const directBytes = [];
while (true) {
  const { done, value } = await reader.read();
  if (done) break;
  directBytes.push(...value);
}
assert.deepEqual(directBytes, [1, 2, 3, 4, 5]);

const directAbort = new AbortController();
const directAbortTracker = createNetworkActivityTracker({
  fetchImpl: async (_input, init) => new Response(new ReadableStream({
    start(controller) {
      init.signal.addEventListener("abort", () => controller.error(new DOMException("cancelled stream", "AbortError")), { once: true });
    },
  })),
});
const abortableResponse = await directAbortTracker.fetch(new Request("https://example.test/native-stream", { signal: directAbort.signal }));
const pendingRead = abortableResponse.body.getReader().read();
directAbort.abort();
await assert.rejects(pendingRead, error => error.name === "AbortError",
  "native body handoff must preserve caller cancellation after progress ownership ends");
assert.equal(directAbortTracker.activeCount, 0);

const preAbortedFetchController = new AbortController();
preAbortedFetchController.abort();
const preAbortedFetchTracker = createNetworkActivityTracker({
  fetchImpl: async () => {
    throw new Error("pre-aborted fetch must not start");
  },
  xhrFactory: null,
  fetchTimeoutMs: 15,
});
await assert.rejects(
  () => preAbortedFetchTracker.fetch("https://example.test/pre-cancelled.bin", {
    signal: preAbortedFetchController.signal,
  }),
  error => error?.name === "AbortError",
);
assert.equal(preAbortedFetchTracker.activeCount, 0, "pre-aborted fetch must clean the tracker");

let hangingHeaderSignal = null;
const hangingHeaderTracker = createNetworkActivityTracker({
  fetchImpl: async (_input, init) => {
    hangingHeaderSignal = init.signal;
    return new Promise(() => {});
  },
  xhrFactory: null,
  fetchTimeoutMs: 15,
});
await assert.rejects(
  () => hangingHeaderTracker.fetch("https://example.test/hanging-headers.bin"),
  error => error?.name === "TimeoutError" && /native fetch timed out/i.test(error.message),
);
assert.equal(hangingHeaderSignal?.aborted, true, "header timeout must abort the native request");
assert.equal(hangingHeaderTracker.activeCount, 0, "header timeout must clean the tracker");

let hangingBodySignal = null;
const hangingBodyTracker = createNetworkActivityTracker({
  fetchImpl: async (_input, init) => {
    hangingBodySignal = init.signal;
    return new Response(new ReadableStream({ start() {} }), {
      status: 200,
      headers: { "content-length": "5" },
    });
  },
  xhrFactory: null,
  fetchTimeoutMs: 15,
});
const hangingBodyResponse = await hangingBodyTracker.fetch("https://example.test/hanging-body.bin");
await assert.rejects(
  () => hangingBodyResponse.arrayBuffer(),
  error => error?.name === "TimeoutError" && /native fetch timed out/i.test(error.message),
);
assert.equal(hangingBodySignal?.aborted, true, "body timeout must abort the native request");
assert.equal(hangingBodyTracker.activeCount, 0, "body timeout must clean the tracker");

const manual = tracker.begin({ label: "DESCRIPTOR" });
assert.equal(tracker.activeCount, 1);
tracker.update(manual, { loaded: 2, total: 4 });
assert.equal(tracker.snapshot().loaded, 2);
tracker.finish(manual);
assert.equal(tracker.activeCount, 0);

const xhrEvents = [];
class FakeXhr {
  open() {}
  setRequestHeader() {}
  getResponseHeader(name) { return name.toLowerCase() === "content-length" ? "5" : null; }
  getAllResponseHeaders() { return "content-length: 5\r\ncontent-type: application/octet-stream\r\n"; }
  send() {
    this.status = 200;
    this.statusText = "OK";
    this.response = new Blob([new Uint8Array([1, 2, 3, 4, 5])]);
    this.onprogress?.({ loaded: 2, total: 5, lengthComputable: true });
    this.onprogress?.({ loaded: 5, total: 5, lengthComputable: true });
    this.onload?.();
  }
  abort() { this.onabort?.(); }
}

let hangingXhrAbortCalls = 0;
class HangingXhr extends FakeXhr {
  send() {}
  abort() {
    hangingXhrAbortCalls++;
    this.onabort?.();
  }
}
const hangingXhrTracker = createNetworkActivityTracker({
  fetchImpl: async () => {
    throw new Error("XHR inactivity must not fall back to fetch");
  },
  xhrFactory: () => new HangingXhr(),
  xhrTimeoutMs: 15,
});
await assert.rejects(
  () => hangingXhrTracker.xhrFetch("https://example.test/hanging-xhr.bin"),
  error => error?.name === "TimeoutError" && /no progress/i.test(error.message),
);
assert.equal(hangingXhrAbortCalls, 1, "XHR inactivity timeout must stop the request");
assert.equal(hangingXhrTracker.activeCount, 0, "XHR inactivity timeout must clean the tracker");

let slowXhr = null;
const slowXhrEvents = [];
class SlowProgressXhr extends FakeXhr {
  send() {
    this.status = 200;
    this.statusText = "OK";
    this.response = new Blob([new Uint8Array([1, 2, 3])]);
    setTimeout(() => this.onprogress?.({ loaded: 1, total: 3, lengthComputable: true }), 5);
    setTimeout(() => this.onprogress?.({ loaded: 2, total: 3, lengthComputable: true }), 20);
    setTimeout(() => this.onprogress?.({ loaded: 3, total: 3, lengthComputable: true }), 35);
    setTimeout(() => this.onload?.(), 65);
  }
}
const slowXhrTracker = createNetworkActivityTracker({
  fetchImpl,
  xhrFactory: () => (slowXhr = new SlowProgressXhr()),
  xhrTimeoutMs: 50,
  onChange: value => slowXhrEvents.push(value),
});
const slowXhrResponse = await slowXhrTracker.xhrFetch("https://example.test/slow-xhr.bin");
assert.deepEqual([...new Uint8Array(await slowXhrResponse.arrayBuffer())], [1, 2, 3]);
assert.ok(slowXhrEvents.some(event => event.active[0]?.loaded === 1));
assert.ok(slowXhrEvents.some(event => event.active[0]?.loaded === 2));
assert.ok(slowXhrEvents.some(event => event.active[0]?.loaded === 3));
assert.equal(slowXhrTracker.activeCount, 0, "progress must reset the inactivity timeout");
assert.equal(slowXhr?.onprogress, null, "completed XHR must release its progress listener");

let cancelledFetchSignal = null;
const cancelledFetchController = new AbortController();
const cancelledFetchTracker = createNetworkActivityTracker({
  fetchImpl: async (_input, init) => {
    cancelledFetchSignal = init.signal;
    return new Promise(() => {});
  },
  xhrFactory: null,
  fetchTimeoutMs: 100,
});
const cancelledFetch = cancelledFetchTracker.fetch("https://example.test/cancelled-fetch.bin", {
  signal: cancelledFetchController.signal,
});
await delay(1);
cancelledFetchController.abort();
await assert.rejects(cancelledFetch, error => error?.name === "AbortError");
assert.equal(cancelledFetchSignal?.aborted, true, "user cancellation must abort the native request");
assert.equal(cancelledFetchTracker.activeCount, 0, "user cancellation must clean the native tracker");

let cancelledXhr = null;
const cancelledXhrController = new AbortController();
const cancelledXhrTracker = createNetworkActivityTracker({
  fetchImpl,
  xhrFactory: () => (cancelledXhr = new HangingXhr()),
  xhrTimeoutMs: 15,
});
const cancelledXhrPromise = cancelledXhrTracker.xhrFetch("https://example.test/cancelled-xhr.bin", {
  signal: cancelledXhrController.signal,
});
await delay(1);
cancelledXhrController.abort();
await assert.rejects(cancelledXhrPromise, error => error?.name === "AbortError");
assert.equal(hangingXhrAbortCalls, 2, "user cancellation must stop the XHR once");
assert.equal(cancelledXhrTracker.activeCount, 0, "user cancellation must clean the XHR tracker");
await delay(25);
assert.equal(cancelledXhrTracker.activeCount, 0, "cancelled XHR timeout must remain cleared");
assert.equal(cancelledXhr?.onprogress, null, "cancelled XHR must release its progress listener");
const xhrTracker = createNetworkActivityTracker({
  fetchImpl,
  xhrFactory: () => new FakeXhr(),
  onChange: value => xhrEvents.push(value),
});
const xhrResponse = await xhrTracker.xhrFetch("https://example.test/file.bin", {}, { label: "PACKAGE" });
assert.deepEqual([...new Uint8Array(await xhrResponse.arrayBuffer())], [1, 2, 3, 4, 5]);
assert.ok(xhrEvents.some(event => event.active[0]?.loaded === 2 && event.active[0]?.total === 5),
  "XHR-backed Package transfers must expose live byte progress without wrapping a fetch stream");
assert.equal(xhrTracker.activeCount, 0);

for (const status of [204, 205, 304]) {
  class EmptyXhr extends FakeXhr {
    send() { this.status = status; this.response = new Blob([]); this.onload(); }
  }
  const emptyTracker = createNetworkActivityTracker({ fetchImpl, xhrFactory: () => new EmptyXhr() });
  const emptyResponse = await emptyTracker.xhrFetch("https://example.test/empty");
  assert.equal(emptyResponse.status, status);
  assert.equal(emptyResponse.body, null, "bodyless HTTP responses must settle rather than throw inside onload");
  assert.equal(emptyTracker.activeCount, 0);
}

let fallbackFetchCalls = 0;
const fallbackFetch = async () => {
  fallbackFetchCalls++;
  return new Response(new Uint8Array([9, 8, 7]), {
    status: 200,
    headers: { "content-length": "3" },
  });
};
class NetworkErrorXhr extends FakeXhr {
  send() { this.onerror?.(); }
}
const fallbackTracker = createNetworkActivityTracker({
  fetchImpl: fallbackFetch,
  xhrFactory: () => new NetworkErrorXhr(),
});
const fallbackResponse = await fallbackTracker.xhrFetch("https://example.test/large.bin", {}, { label: "PACKAGE FALLBACK" });
assert.deepEqual([...new Uint8Array(await fallbackResponse.arrayBuffer())], [9, 8, 7]);
assert.equal(fallbackFetchCalls, 1,
  "an XHR transport failure must retry once through native fetch");
assert.equal(fallbackTracker.activeCount, 0);

let fallbackHangingBodySignal = null;
const fallbackHangingBodyTracker = createNetworkActivityTracker({
  fetchImpl: async (_input, init) => {
    fallbackHangingBodySignal = init.signal;
    return new Response(new ReadableStream({ start() {} }), {
      status: 200,
      headers: { "content-length": "5" },
    });
  },
  xhrFactory: () => new NetworkErrorXhr(),
  fetchTimeoutMs: 15,
});
const fallbackHangingBodyResponse = await fallbackHangingBodyTracker.xhrFetch(
  "https://example.test/fallback-hanging-body.bin",
);
await assert.rejects(
  () => fallbackHangingBodyResponse.arrayBuffer(),
  error => error?.name === "TimeoutError",
);
assert.equal(fallbackHangingBodySignal?.aborted, true, "fallback body timeout must abort native fetch");
assert.equal(fallbackHangingBodyTracker.activeCount, 0, "fallback body timeout must clean the tracker");

let statusZeroFetchCalls = 0;
class StatusZeroXhr extends FakeXhr {
  send() {
    this.status = 0;
    this.statusText = "";
    this.response = new Blob([]);
    this.onload?.();
  }
}
const statusZeroTracker = createNetworkActivityTracker({
  fetchImpl: async () => {
    statusZeroFetchCalls++;
    return new Response(new Uint8Array([6, 5, 4]), {
      status: 200,
      headers: { "content-length": "3" },
    });
  },
  xhrFactory: () => new StatusZeroXhr(),
});
const statusZeroResponse = await statusZeroTracker.xhrFetch("https://example.test/status-zero.bin");
assert.deepEqual([...new Uint8Array(await statusZeroResponse.arrayBuffer())], [6, 5, 4]);
assert.equal(statusZeroFetchCalls, 1,
  "XHR onload with status 0 must retry once through native fetch");
assert.equal(statusZeroTracker.activeCount, 0);

let abortedFallbackFetchCalls = 0;
const abortedController = new AbortController();
abortedController.abort();
const abortedTracker = createNetworkActivityTracker({
  fetchImpl: async () => {
    abortedFallbackFetchCalls++;
    return new Response();
  },
  xhrFactory: () => new NetworkErrorXhr(),
});
await assert.rejects(
  () => abortedTracker.xhrFetch("https://example.test/cancelled.bin", { signal: abortedController.signal }),
  error => error?.name === "AbortError",
);
assert.equal(abortedFallbackFetchCalls, 0,
  "a user-aborted Package transfer must never be retried through fetch");
assert.equal(abortedTracker.activeCount, 0);

const cancelledRequest = new Request("https://example.test/cancelled-request.bin", { signal: abortedController.signal });
for (const method of ["fetch", "xhrFetch"]) {
  await assert.rejects(() => abortedTracker[method](cancelledRequest), error => error?.name === "AbortError",
    "the Request object's signal must survive the transport's timeout controller");
}
assert.equal(abortedFallbackFetchCalls, 0);

console.log("Network activity contract: PASS");
