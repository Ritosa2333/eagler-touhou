function externalResourceTarget(requestUrl, targetUrl) {
  const request = new URL(requestUrl);
  const target = new URL(targetUrl, request);
  if (target.protocol !== "https:" || target.origin === request.origin) {
    throw new Error("external resource target must use a different HTTPS Origin");
  }
  if (target.pathname !== request.pathname || target.search !== request.search) {
    throw new Error("external resource target must preserve the Package path and query");
  }
  return target;
}

export function validateExternalResourceRedirect(requestUrl, { status, location } = {}) {
  if (status !== 307) throw new Error(`HTTP ${status}, expected 307 redirect`);
  if (typeof location !== "string" || !location) throw new Error("external redirect Location is missing");
  return externalResourceTarget(requestUrl, location);
}

export function validateExternalResourceFinalUrl(requestUrl, finalUrl) {
  return externalResourceTarget(requestUrl, finalUrl);
}
