/*
 * Compatibility owner for retiring the former /eagler-touhou/ Service Worker
 * scope. Nginx serves this file only at the historical worker script URL.
 * Once activated it unregisters that scope and moves controlled windows to the
 * deployment root. Remove this file and the matching server route only after
 * the published old-worker population has aged out.
 */
self.addEventListener("install", event => {
  event.waitUntil(self.skipWaiting());
});

self.addEventListener("activate", event => {
  event.waitUntil((async () => {
    const windows = await self.clients.matchAll({ type: "window", includeUncontrolled: true });
    await self.registration.unregister();
    await Promise.all(windows
      .filter(client => new URL(client.url).pathname.startsWith("/eagler-touhou"))
      .map(client => client.navigate(new URL("/", self.location.origin).href)));
  })());
});

self.addEventListener("fetch", event => {
  if (event.request.mode === "navigate") {
    event.respondWith(Response.redirect(new URL("/", self.location.origin), 302));
    return;
  }
  event.respondWith(fetch(event.request));
});
