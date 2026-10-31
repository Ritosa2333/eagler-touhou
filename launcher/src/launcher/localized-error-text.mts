import errors from '../../messages/errors.json' with { type: 'json' };
import { isUiLocale, translateUi, type UiMessageKey, type UiMessageParams } from './i18n.mts';

const patterns = errors.map(({ key, source }) => {
  const names: string[] = [];
  const parts = source.split(/(\{[A-Za-z0-9_]+\})/).map((part) => {
    if (/^\{[A-Za-z0-9_]+\}$/.test(part)) {
      names.push(part.slice(1, -1));
      return '(.+?)';
    }
    return part.replace(/[.*+?^${}()|[\]\\]/g, '\\$&');
  });
  return { key: key as UiMessageKey, pattern: new RegExp(`^${parts.join('')}$`, 's'), names };
});

/** Validation and protocol strings stay stable; the presentation boundary translates known errors. */
export function localizeKnownError(
  text: string,
  translate?: (key: UiMessageKey, params?: UiMessageParams) => string,
): string {
  const locale = globalThis.document?.documentElement?.dataset.uiLocale;
  const t =
    translate ??
    ((key: UiMessageKey, params?: UiMessageParams) => translateUi(isUiLocale(locale) ? locale : 'zh-CN', key, params));
  for (const { key, pattern, names } of patterns) {
    const match = pattern.exec(text);
    if (match) return t(key, Object.fromEntries(names.map((name, index) => [name, match[index + 1]])));
  }
  return text;
}
