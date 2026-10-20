#!/usr/bin/env node
import { createReadStream } from "node:fs";
import { createHash } from "node:crypto";
import { readFile, stat } from "node:fs/promises";
import { createServer } from "node:http";
import { resolve, sep } from "node:path";
import { pipeline } from "node:stream/promises";
import { isDeepStrictEqual } from "node:util";
import { constants as zlibConstants, createBrotliCompress, createGzip } from "node:zlib";
import {
  staticContentCacheControl,
  staticContentCompressible,
  staticContentType,
} from "../server/static-content-policy.mjs";

const root = resolve(process.argv[2] || ".");
const port = Number.parseInt(process.argv[3] || process.env.EAGLER_TOUHOU_PORT || "8130", 10);
const host = process.env.EAGLER_TOUHOU_HOST || "127.0.0.1";
if (!Number.isInteger(port) || port < 1 || port > 65535) throw new Error("invalid port");
const rootInfo = await stat(root);
if (!rootInfo.isDirectory()) throw new Error(`site root is not a directory: ${root}`);

const etag = info => `\"${createHash("sha1").update(`${info.size}:${info.mtimeMs}`).digest("base64url")}\"`;

const server = createServer(async (request, response) => {
  try {
    const url = new URL(request.url || "/", `http://${host}:${port}`);
    if (url.pathname === "/eagler-touhou" || url.pathname === "/eagler-touhou/") {
      response.writeHead(302, { Location: "/", "Cache-Control": "no-store" });
      response.end();
      return;
    }
    let pathname = decodeURIComponent(url.pathname);
    const legacyRetirementWorker = pathname === "/eagler-touhou/app-shell-sw.js";
    if (legacyRetirementWorker) {
      pathname = "/legacy-mount-retirement-sw.js";
    } else if (pathname.startsWith("/eagler-touhou/")) {
      pathname = pathname.slice("/eagler-touhou".length);
    }
    let file = resolve(root, `.${pathname}`);
    if (file !== root && !file.startsWith(root + sep)) throw new Error("path outside site root");
    let info = await stat(file);
    if (info.isDirectory() && !url.pathname.endsWith("/")) {
      response.writeHead(308, { Location: `${url.pathname}/${url.search}`, "Cache-Control": "no-store" });
      response.end();
      return;
    }
    if (info.isDirectory()) { file = resolve(file, "index.html"); info = await stat(file); }
    if (!info.isFile()) throw new Error("not a file");

    const tag = etag(info);
    const commonHeaders = {
      "Content-Type": staticContentType(file),
      "Cache-Control": legacyRetirementWorker ? "no-store" : staticContentCacheControl(file),
      ETag: tag,
      "Last-Modified": info.mtime.toUTCString(),
      Vary: "Accept-Encoding",
    };
    if (legacyRetirementWorker) commonHeaders["Service-Worker-Allowed"] = "/eagler-touhou/";
    if (request.headers["if-none-match"] === tag) {
      response.writeHead(304, commonHeaders);
      response.end();
      return;
    }
    const accepted = request.headers["accept-encoding"] || "";
    const compress = staticContentCompressible(file, info.size);
    const encoding = compress && /\bbr\b/.test(accepted) ? "br"
      : compress && /\bgzip\b/.test(accepted) ? "gzip" : "";
    const headers = { ...commonHeaders };
    if (encoding) headers["Content-Encoding"] = encoding;
    else headers["Content-Length"] = info.size;
    response.writeHead(200, headers);
    if (request.method === "HEAD") { response.end(); return; }
    const source = createReadStream(file);
    if (encoding === "br") {
      await pipeline(source, createBrotliCompress({ params: { [zlibConstants.BROTLI_PARAM_QUALITY]: 5 } }), response);
    } else if (encoding === "gzip") {
      await pipeline(source, createGzip({ level: 6 }), response);
    } else {
      await pipeline(source, response);
    }
  } catch {
    if (!response.headersSent) response.writeHead(404, { "Content-Type": "text/plain; charset=utf-8" });
    if (!response.writableEnded) response.end("Not found");
  }
});
server.once("error", async error => {
  const address = `http://${host}:${port}/`;
  if (error.code === "EADDRINUSE") {
    try {
      const expected = JSON.parse(await readFile(resolve(root, "site-manifest.json"), "utf8"));
      const response = await fetch(new URL("site-manifest.json", address), {
        signal: AbortSignal.timeout(2000), redirect: "error", cache: "no-store",
      });
      if (response.ok && expected.protocol === "eagler-touhou/1" && isDeepStrictEqual(expected, await response.json())) {
        console.log(`This Eagler Touhou site is already running: ${address}`);
        console.log("Use the existing page; no second server is needed.");
        return;
      }
    } catch {}
    console.error(`Port ${port} is already in use. Close the previous server or choose another port.`);
  } else {
    console.error(`Could not start the site: ${error.message}`);
  }
  process.exitCode = 1;
});
server.listen(port, host, () => {
  console.log(`Eagler Touhou host: http://${host}:${port}/`);
  console.log(`Serving immutable site directory: ${root}`);
});
