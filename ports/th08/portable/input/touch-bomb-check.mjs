import {resolve} from 'node:path';
import {checkTouchBomb} from '../../../../tools/ports/touch-bomb-check.mjs';
await checkTouchBomb({sourceRoot:resolve(import.meta.dirname,'../..'),checks:['touch bomb while immobile','focus preserved','movement suppressed during hit','held movement target survives deathbomb','pulse expiry before respawn','double tap across hit','double tap during deathbomb','no menu/dialogue/replay bomb carryover','movement recovery'],before:process.argv.includes('--before')});
