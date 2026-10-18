import test from "node:test";import {verifyPlayerShots} from "./player-shot-oracle.mjs";
test("TH15 real player shots preserve updates, retirement and animation lifecycles",()=>verifyPlayerShots(true));
