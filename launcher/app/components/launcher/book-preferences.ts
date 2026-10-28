import type {LibraryProduct} from './products';

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
  writeBookPreference('recent', [productId, ...recent.filter(id => id !== productId)].slice(0, 20));
}

/** The doujin slot intentionally has no catalogue mapping yet. It is reserved
 * for independently published titles when real support becomes available. */
export type BookCategory = 'all' | 'favorites' | 'pc98' | 'windows' | 'spin-off' | 'doujin';
export function bookCategory(product: LibraryProduct): BookCategory {
  const number = Number(product.number);
  if (!Number.isInteger(number)) return 'spin-off';
  return number <= 5 ? 'pc98' : 'windows';
}

export function filterBookProducts(products: readonly LibraryProduct[], category: BookCategory, favorites: readonly string[], query: string, sort: string) {
  const term = query.trim().toLocaleLowerCase();
  return products.filter(product => !product.hidden && !product.multiplayer)
    .filter(product => category === 'all' || (category === 'favorites' ? favorites.includes(product.id) : bookCategory(product) === category))
    .filter(product => `${product.id} TH${product.number} ${product.title} ${product.subtitle}`.toLocaleLowerCase().includes(term))
    .sort((a, b) => sort === 'name' ? a.title.localeCompare(b.title, 'zh-CN') : (Number(a.number) - Number(b.number)) * (sort === 'reverse' ? -1 : 1));
}
