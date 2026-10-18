import test from "node:test";import {verifyPlayerShots} from "./player-shot-oracle.mjs";
test("TH15 homing and side-turn shots preserve real enemy target selection",()=>verifyPlayerShots(true,true));
