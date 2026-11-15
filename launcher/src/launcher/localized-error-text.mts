import errors from '../../messages/errors.json' with {type: 'json'};
import type {UiMessageKey} from './i18n.mjs';

type ErrorTranslate = (key: UiMessageKey, params?: Readonly<Record<string, string>>) => string;
const patterns = errors.map(({key, source}) => {
  const names: string[] = [];
  const parts = source.split(/(\{[A-Za-z0-9_]+\})/).map(part => {
    if (/^\{[A-Za-z0-9_]+\}$/.test(part)) {names.push(part.slice(1, -1)); return '(.*?)';}
    return part.replace(/[.*+?^${}()|[\]\\]/g, '\\$&');
  });
  return {key: key as UiMessageKey, pattern: new RegExp(`^${parts.join('')}$`, 's'), names};
});

/** Translate known presentation text; callers retain the original error object. */
export function localizeKnownError(text: string, translate: ErrorTranslate): string {
  for (const {key, pattern, names} of patterns) {
    const match = pattern.exec(text);
    if (match && names.every((name, index) => match[index + 1] === match[names.indexOf(name) + 1])) {
      return translate(key, Object.fromEntries(names.map((name, index) => [name, match[index + 1] ?? ''])));
    }
  }
  return text;
}
