import test from "node:test";import {verifyPlayerShots} from "./player-shot-oracle.mjs";
test("TH15 all shot hit callbacks preserve damage, explosions and actual effects",()=>verifyPlayerShots(true,true,true));
