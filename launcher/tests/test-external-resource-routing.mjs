import assert from "node:assert/strict";
import {
  validateExternalResourceFinalUrl,
  validateExternalResourceRedirect,
} from "../lib/external-resource-routing.mjs";

const request = "https://play.example.com/games/th08/th08.data?v=revision";
assert.equal(
  validateExternalResourceRedirect(request, {
    status: 307,
    location: "https://asset.example.com/games/th08/th08.data?v=revision",
  }).href,
  "https://asset.example.com/games/th08/th08.data?v=revision",
);
assert.equal(
  validateExternalResourceFinalUrl(request, "https://asset.example.com/games/th08/th08.data?v=revision").href,
  "https://asset.example.com/games/th08/th08.data?v=revision",
);

assert.throws(() => validateExternalResourceRedirect(request, {
  status: 308,
  location: "https://asset.example.com/games/th08/th08.data?v=revision",
}), /expected 307/);
assert.throws(() => validateExternalResourceRedirect(request, {
  status: 307,
  location: "https://asset.example.com/games/th07/th07.data?v=revision",
}), /preserve the Package path and query/);
assert.throws(() => validateExternalResourceRedirect(request, {
  status: 307,
  location: "https://asset.example.com/games/th08/th08.data?v=other",
}), /preserve the Package path and query/);
assert.throws(() => validateExternalResourceRedirect(request, {
  status: 307,
  location: "https://play.example.com/games/th08/th08.data?v=revision",
}), /different HTTPS Origin/);
assert.throws(() => validateExternalResourceFinalUrl(
  request,
  "http://asset.example.com/games/th08/th08.data?v=revision",
), /different HTTPS Origin/);

console.log("External resource routing contract: PASS");
