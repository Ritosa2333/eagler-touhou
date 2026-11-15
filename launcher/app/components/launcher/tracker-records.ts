import {trackerGames} from './tracker-renderer';
import {readBookPreference} from './book-preferences';
import {translateUi} from '../../../src/launcher/i18n.mts';
import type {Translate, UiMessageKey} from '../../i18n';

export type TrackerRecord = {name: string; done: boolean; misses: string | null; bombs: string | null; lives: string | null;
  vertical: boolean; focus: boolean; pacifist: boolean; unique: boolean; cellColour: string; textColour: string;
  note?: string; challenge?: string; continued?: boolean};
export type CareerRecords = Record<string, TrackerRecord>;
export type TrackerOptions = {easy: boolean; extra: boolean; legend: boolean; transparent: boolean};
export const DEFAULT_OPTIONS: TrackerOptions = {easy: true, extra: true, legend: true, transparent: false};
export const CAREER_STORAGE_KEY = 'player-career-chart-v2';
const titleKeys: Record<string, UiMessageKey> = {
  "HRTP": "career.game.HRTP",
  "SOEW": "career.game.SOEW",
  "PODD": "career.game.PODD",
  "LS": "career.game.LS",
  "MS": "career.game.MS",
  "EOSD": "career.game.EOSD",
  "STB": "career.game.STB",
  "DS": "career.game.DS",
  "ISC": "career.game.ISC",
  "VD": "career.game.VD",
  "100BM": "career.game.100BM",
  "PCB": "career.game.PCB",
  "IN": "career.game.IN",
  "POFV": "career.game.POFV",
  "MOF": "career.game.MOF",
  "SA": "career.game.SA",
  "UFO": "career.game.UFO",
  "GFW": "career.game.GFW",
  "TD": "career.game.TD",
  "DDC": "career.game.DDC",
  "LOLK": "career.game.LOLK",
  "HSIFS": "career.game.HSIFS",
  "WBAWC": "career.game.WBAWC",
  "UM": "career.game.UM",
  "UDOALG": "career.game.UDOALG",
  "IAMP": "career.game.IAMP",
  "SWR": "career.game.SWR",
  "H.SOKU": "career.game.H_SOKU",
  "HM": "career.game.HM",
  "ULIL": "career.game.ULIL",
  "AOCF": "career.game.AOCF",
  "GI": "career.game.GI",
  "TH20": "career.game.TH20"
};
export type TrackerGame = {name: string; colour: string; titleKey: UiMessageKey; characters: string[]; difficulties: string[]};
type SourceGame = {name: string; colour: string; characters: {name: string; subcharacters: string[]}[]; difficulties: string[]};
export const TRACKER_GAMES: TrackerGame[] = (trackerGames as unknown as SourceGame[]).map(game => ({
  name: game.name, colour: game.colour, titleKey: titleKeys[game.name]!,
  characters: game.characters.flatMap(character => character.subcharacters.length ? character.subcharacters.map(sub => character.name + sub) : [character.name]),
  difficulties: game.name === 'IN' ? ['X', 'B-L', 'B-H', 'B-N', 'B-E', 'A-L', 'A-H', 'A-N', 'A-E'] : [...game.difficulties, ...(game.difficulties.at(-1) === 'N' ? ['E'] : [])],
}));
export function cellAvailable(game: string, character: string, difficulty: string) {
  return !(game === 'GFW' && character === 'EX' && difficulty !== 'N') &&
    !(game === 'LOLK' && !character.endsWith('l') && difficulty === 'X') &&
    !(game === 'HSIFS' && !character.endsWith('sp') && difficulty === 'X') &&
    !(['TD', 'UM'].includes(game) && character.endsWith('2') && difficulty === 'X');
}
export const TRACKER_CELLS = TRACKER_GAMES.flatMap(game => game.characters.flatMap(character => game.difficulties
  .filter(difficulty => cellAvailable(game.name, character, difficulty)).map(difficulty => ({key: `${game.name}-${character}-${difficulty}`, game, character, difficulty}))));
const keys = new Set(TRACKER_CELLS.map(cell => cell.key));
export const blankRecord = (name: string): TrackerRecord => ({name, done: false, misses: null, bombs: null, lives: null, vertical: false, focus: false, pacifist: false, unique: false, cellColour: '#ffffff', textColour: '#000000'});
const count = (value: unknown) => value === null || (typeof value === 'string' && /^[0-9]$/.test(value));
export function validCareerRecords(value: unknown): value is CareerRecords {
  if (!value || typeof value !== 'object' || Array.isArray(value)) return false;
  return Object.entries(value).every(([key, item]) => {
    if (!keys.has(key) || !item || typeof item !== 'object' || Array.isArray(item)) return false;
    const r = item as TrackerRecord;
    return r.name === key && ['done', 'vertical', 'focus', 'pacifist', 'unique'].every(flag => typeof r[flag as keyof TrackerRecord] === 'boolean') &&
      [r.misses, r.bombs, r.lives].every(count) && /^#[\da-f]{6}$/i.test(r.cellColour) && /^#[\da-f]{6}$/i.test(r.textColour) &&
      (r.note === undefined || (typeof r.note === 'string' && r.note.length <= 240)) &&
      (r.challenge === undefined || (typeof r.challenge === 'string' && r.challenge.length <= 24)) &&
      (r.continued === undefined || typeof r.continued === 'boolean');
  });
}
export const validOptions = (value: unknown): value is TrackerOptions => !!value && typeof value === 'object' &&
  Object.keys(DEFAULT_OPTIONS).every(key => typeof (value as Record<string, unknown>)[key] === 'boolean');
const difficultyNames: Record<string, string> = {E: 'Easy', N: 'Normal', H: 'Hard', L: 'Lunatic', X: 'Extra', P: 'Phantasm'};
const cellsByKey = new Map(TRACKER_CELLS.map(cell => [cell.key, cell]));
export function cellLabel(key: string, t: Translate = key => translateUi('zh-CN', key)) {
  const cell = cellsByKey.get(key);
  if (!cell) return key;
  return `${t(cell.game.titleKey)} | ${cell.character} | ${difficultyNames[cell.difficulty] ?? cell.difficulty}`;
}
export function loadCareerRecords() {
  return readBookPreference(CAREER_STORAGE_KEY, {}, validCareerRecords);
}
