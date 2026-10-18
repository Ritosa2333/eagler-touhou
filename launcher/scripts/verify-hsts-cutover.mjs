const [httpArg, httpsArg] = process.argv.slice(2);
if (!httpArg || !httpsArg) {
  throw new Error("Usage: node scripts/verify-hsts-cutover.mjs http://host/ https://host/");
}

function normalizeBase(value, protocol) {
  const url = new URL(value);
  if (url.protocol !== protocol) throw new Error(`expected ${protocol} URL, got ${url.href}`);
  if (!url.pathname.endsWith("/")) url.pathname += "/";
  url.search = "";
  url.hash = "";
  return url;
}

const httpBase = normalizeBase(httpArg, "http:");
const httpsBase = normalizeBase(httpsArg, "https:");
if (httpBase.hostname !== httpsBase.hostname) {
  throw new Error(`cutover origins must use the same hostname: ${httpBase.hostname} != ${httpsBase.hostname}`);
}

const redirect = await fetch(httpBase, { cache: "no-store", redirect: "manual" });
if (![301, 302, 307, 308].includes(redirect.status)) {
  throw new Error(`plain HTTP entry must redirect to HTTPS, got ${redirect.status}`);
}
const location = redirect.headers.get("location");
if (!location || new URL(location, httpBase).href !== httpsBase.href) {
  throw new Error(`plain HTTP entry must redirect cleanly to ${httpsBase.href}, got ${location || "missing Location"}`);
}

const secure = await fetch(httpsBase, { cache: "no-store", redirect: "manual" });
if (!secure.ok) throw new Error(`HTTPS Launcher entry failed: ${secure.status}`);
const hsts = secure.headers.get("strict-transport-security") || "";
const maxAge = Number.parseInt(/(?:^|;)\s*max-age=(\d+)/i.exec(hsts)?.[1] || "", 10);
if (!Number.isSafeInteger(maxAge) || maxAge < 31_536_000) {
  throw new Error(`final HTTPS site must publish at least one year of HSTS, got ${hsts || "missing"}`);
}

const manifestUrl = new URL("site-manifest.json", httpsBase);
const manifestResponse = await fetch(manifestUrl, { cache: "no-store" });
if (!manifestResponse.ok) throw new Error(`Site Manifest failed: ${manifestResponse.status}`);
const manifest = await manifestResponse.json();
if (manifest?.shared?.originMigration != null) {
  throw new Error("final HSTS cutover must remove shared.originMigration from the Site Manifest");
}

console.log(JSON.stringify({
  valid: true,
  hostname: httpsBase.hostname,
  redirect: redirect.status,
  hstsMaxAge: maxAge,
  migrationCapability: false,
}));
