"use strict";
// Runtime-only registration. It never controls/preloads Launcher pages, DATA,
// fonts from shared/ or games/, or mutable save/replay content.
const programMount = new URL('./', self.location.href);
const programCache = createRuntimeCache({scopeUrl: programMount, catalog: {
  schema: EaglerRuntimeGenerations.RUNTIME_MANIFEST_SCHEMA,
  protocol: EaglerRuntimeGenerations.RUNTIME_PROTOCOL, groups: [],
}});
self.addEventListener('install', event => {event.waitUntil(self.skipWaiting());});
self.addEventListener('fetch', event => {
  const response = programCache.handle(event);
  if (response) event.respondWith(response);
});
self.addEventListener('message', event => {
  const port = event.ports?.[0], C = EaglerRuntimeGenerations;
  if (![C.RUNTIME_CAPABILITIES, C.RUNTIME_PREPARE, C.RUNTIME_IMPORT].includes(event.data?.type)) return;
  const source = event.source?.url && new URL(event.source.url);
  if (!source || source.origin !== programMount.origin || !source.pathname.startsWith(programMount.pathname)) return;
  event.waitUntil((async () => {
    try {
      if (event.data.type === C.RUNTIME_CAPABILITIES) {
        port?.postMessage({ok:true, protocol:C.RUNTIME_CACHE_PROTOCOL, localOnly:true, importFiles:true});return;
      }
      if (event.data.type === C.RUNTIME_IMPORT) {
        const request = C.validateRuntimeImportRequest(event.data);
        port?.postMessage(await programCache.importLaunch(request.root, request.descriptor, request.files));return;
      }
      const request = C.validateRuntimePrepareRequest(event.data);
      port?.postMessage(await programCache.prepareLaunch(request.entry, {
        exclude:request.exclude, ...(request.catalog ? {catalog:request.catalog} : {}),
        ...(request.localOnly ? {localOnly:true} : {}),
        ...(request.progress ? {onProgress: progress => port?.postMessage(progress)} : {}),
      }));
    } catch (error) {port?.postMessage({ok:false,error:error instanceof Error ? error.message : String(error)});}
  })());
});
