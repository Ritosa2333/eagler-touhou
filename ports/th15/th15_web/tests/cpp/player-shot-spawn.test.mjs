import test from "node:test";import {verifyPlayerShots} from "./player-shot-oracle.mjs";
test("TH15 four real SHT shot sets preserve firing, callbacks, pools and ANM submission",()=>verifyPlayerShots(false));
