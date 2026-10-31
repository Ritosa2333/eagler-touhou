import { describe, expect, it } from 'vitest';
import { localeHome, resolveUiLocale, translateUi, UI_LOCALES, UI_MESSAGES } from '../../src/launcher/i18n.mts';
import { localizeKnownError } from '../../src/launcher/localized-error-text.mts';
import { describeNetplayConnection } from '../../src/launcher/runtime-diagnostics-model.mts';

describe('Paraglide locale isolation and parameter handling', () => {
  it.each(UI_LOCALES)('%s provides every message', (locale) => {
    expect(Object.keys(UI_MESSAGES[locale])).toEqual(Object.keys(UI_MESSAGES['zh-CN']));
    for (const [key, message] of Object.entries(UI_MESSAGES[locale])) {
      if (UI_MESSAGES.en[key as keyof typeof UI_MESSAGES.en].trim()) expect(message.trim()).not.toBe('');
    }
  });
  it('keeps explicit calls isolated and preserves missing and zero parameters', () => {
    expect(translateUi('ja', 'status.roomCreated', { code: 0 })).toBe('部屋 0 を作成しました');
    expect(translateUi('en', 'status.roomCreated', { code: '123456' })).toBe('Room 123456 created');
    expect(translateUi('zh-CN', 'status.roomCreated', { code: '123456' })).toBe('已创建房间 123456');
    expect(translateUi('ja', 'status.roomCreated')).toBe('部屋 {code} を作成しました');
    expect(resolveUiLocale('ja-JP')).toBe('ja');
    expect(localeHome('ja')).toBe('ja.html');
  });
});

describe('known error presentation', () => {
  it.each(UI_LOCALES)('translates original validation errors without mutating unknown diagnostics (%s)', (locale) => {
    const t = (key: Parameters<typeof translateUi>[1], params?: Parameters<typeof translateUi>[2]) =>
      translateUi(locale, key, params);
    expect(localizeKnownError('本地录像存储正被占用，请重试', t)).toBe(t('error.replayBusy'));
    expect(localizeKnownError('美化版资源目录不可用：HTTP 503', t)).toBe(t('error.beautyCatalog', { status: 503 }));
    expect(localizeKnownError('th06 尚未提供美化版资源', t)).toBe(t('error.beautyMissing', { game: 'th06' }));
    expect(localizeKnownError('unexpected fixture code: 503', t)).toBe('unexpected fixture code: 503');
  });
  it.each(UI_LOCALES)('uses the active locale in connection and spectator states (%s)', (locale) => {
    const t = (key: string) => translateUi(locale, key as Parameters<typeof translateUi>[1]);
    const view = describeNetplayConnection({ peerState: {}, translate: t });
    expect(view.title).toBe(t('connection.connecting'));
    expect(view.summary).toBe(t('connection.waitingHint'));
    expect(describeNetplayConnection({ peerState: {}, spectator: true, translate: t }).title).toBe(
      t('connection.spectatorConnecting'),
    );
    expect(describeNetplayConnection({ peerState: {}, spectator: true, failed: true, translate: t }).summary).toBe(
      t('connection.spectatorFailed'),
    );
  });
});
