import type {RuntimeCommandPayloads, RuntimeConfigureOptions} from '../contracts/runtime-protocol.mts';
import type {InstalledPackageGeneration} from '../contracts/package-read-models.mts';
import type {MusicMode} from './game-preferences.mts';
import {requireCompleteTh128Package} from './th128-managed-package.mjs';

export const TH128_INSTALLED_LANGUAGE = 'lang_zh-hans';
export function th128LanguageAvailable(language: string) {
  return [TH128_INSTALLED_LANGUAGE, 'zh', 'zh-hans', 'zh-CN'].includes(language);
}

export function th128RequestedMusic(requested: MusicMode, webAudio: boolean): 'ogg-full' | 'none' {
  return webAudio && requested !== 'none' ? 'ogg-full' : 'none';
}

export function th128InstalledMusic(generation: InstalledPackageGeneration | null, requested: MusicMode, webAudio: boolean): 'ogg-full' | 'none' {
  // TH128 mounts all tracks from one installed generation. The Launcher
  // availability hint is for the settings UI and can lag behind Package Store.
  requireCompleteTh128Package(generation);
  return th128RequestedMusic(requested, webAudio);
}

export function th128InstalledConfigure(language: string, music: MusicMode, options: RuntimeConfigureOptions,
  installedLanguage = TH128_INSTALLED_LANGUAGE) {
  if (music !== 'ogg-stream' && music !== 'ogg-full' && music !== 'none') {
    throw new Error('TH128 supports OGG or no BGM');
  }
  if (options.alwaysHitbox) throw new Error('TH128 does not support an always-visible hitbox');
  const runtimeLanguage = installedLanguage === 'lang_en' ? 'en' : installedLanguage === 'ja' ? 'ja' : 'zh';
  const configure: RuntimeCommandPayloads['configure'] = {
    language: runtimeLanguage, music: music === 'none' ? 'none' : 'ogg', runtimePack: null,
    options: {...options, oggDecodeMode: 'full'},
  };
  return {language: installedLanguage, music: music === 'none' ? 'none' as const : 'ogg-full' as const,
    languageFallback: language !== installedLanguage && language !== runtimeLanguage &&
      !(runtimeLanguage === 'zh' && th128LanguageAvailable(language)), configure};
}
