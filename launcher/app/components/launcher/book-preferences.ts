import {bookGameKind, isPc98Product, type LibraryProduct} from './products';

export const BOOK_PREFERENCES_EVENT = 'eagler-book-preferences';
const prefix = 'eagler.book.';
const transient = new WeakMap<Window, Map<string, unknown>>();
function sessionPreferences() {
  let values = transient.get(window);
  if (!values) {values = new Map(); transient.set(window, values);}
  return values;
}

export function readBookPreference<T>(key: string, fallback: T, valid: (value: unknown) => value is T): T {
  if (typeof window === 'undefined') return fallback;
  const pending = sessionPreferences().get(key);
  if (valid(pending)) return pending;
  try {
    const value: unknown = JSON.parse(window.localStorage.getItem(prefix + key) ?? 'null');
    return valid(value) ? value : fallback;
  } catch {const value = sessionPreferences().get(key); return valid(value) ? value : fallback;}
}

export function writeBookPreference(key: string, value: unknown) {
  try {window.localStorage.setItem(prefix + key, JSON.stringify(value)); sessionPreferences().delete(key);}
  catch {sessionPreferences().set(key, value);}
  window.dispatchEvent(new CustomEvent(BOOK_PREFERENCES_EVENT));
}

export const stringArray = (value: unknown): value is string[] => Array.isArray(value) && value.every(item => typeof item === 'string');

/** Called after the real runtime acknowledges a successful ordinary launch. */
export function recordBookLaunch(productId: string) {
  const recent = readBookPreference('recent', [], stringArray);
  const next = [productId, ...recent.filter(id => id !== productId)].slice(0, 20);
  const dates = readBookPreference('last-played', {}, lastPlayedDates);
  writeBookPreference('last-played', Object.fromEntries(next.flatMap((id) =>
    id === productId ? [[id, Date.now()]] : dates[id] ? [[id, dates[id]]] : [],
  )));
  writeBookPreference('recent', next);
}

function lastPlayedDates(value: unknown): value is Record<string, number> {
  return !!value && typeof value === 'object' && !Array.isArray(value) &&
    Object.values(value).every(date => Number.isFinite(date) && date > 0 && date <= 8.64e15);
}
export function bookLastPlayed(productId: string) {
  return readBookPreference('last-played', {}, lastPlayedDates)[productId];
}

export type BookCategory = 'all' | 'favorites' | 'pc98' | 'windows' | 'versus' | 'fighting' | 'spin-off' | 'doujin';
export function bookCategory(product: LibraryProduct): BookCategory {
  if (product.gameId === 'hsob' || /^th[a-z]/i.test(product.id)) return 'doujin';
  if (isPc98Product(product)) return 'pc98';
  const kind = bookGameKind(product.gameId);
  if (kind !== 'original') return kind;
  return Number.isInteger(Number(product.number)) ? 'windows' : 'spin-off';
}

function compareBookNumbers(a: LibraryProduct, b: LibraryProduct, reverse: boolean) {
  const numeric = (product: LibraryProduct) => /^th\d+(?:\.\d+)?$/i.test(product.id) && /^\d+(?:\.\d+)?$/.test(product.number);
  const aNumeric = numeric(a), bNumeric = numeric(b);
  if (aNumeric !== bNumeric) return aNumeric ? -1 : 1;
  if (!aNumeric) return a.id.localeCompare(b.id, 'en');
  return (Number(a.number) - Number(b.number)) * (reverse ? -1 : 1);
}

export function matchesBookQuery(product: LibraryProduct, query: string) {
  const label = [product.id, 'th' + product.number, product.title, product.subtitle].join(' ');
  return label.toLocaleLowerCase().includes(query.trim().toLocaleLowerCase());
}
export function filterBookProducts(products: readonly LibraryProduct[], category: BookCategory, favorites: readonly string[], query: string, sort: string) {
  return products.filter(product => !product.hidden && !product.multiplayer)
    .filter(product => category === 'all' || (category === 'favorites' ? favorites.includes(product.id) : bookCategory(product) === category))
    .filter(product => matchesBookQuery(product, query))
    .sort((a, b) =>
      (category === 'all' ? Number(favorites.includes(b.id)) - Number(favorites.includes(a.id)) : 0) ||
      (sort === 'name' ? a.title.localeCompare(b.title, 'zh-CN') : compareBookNumbers(a, b, sort === 'reverse')));
}
