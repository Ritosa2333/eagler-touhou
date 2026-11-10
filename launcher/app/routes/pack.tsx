import {useEffect, useRef, useState} from 'react';
import {useHref, useLocation} from 'react-router';
import {MainSelect} from '../components/launcher/MainSelect';
import {translate, useLocale} from '../i18n';
import {sitePageTitle} from '../navigation/document-locale';
import {previewFeaturesEnabled} from '../release-features';
import {createMetadataService} from '../services/metadata';
import {createPackageAcquisition, preparationErrorText} from '../services/package-acquisition';
import {StandalonePageShell} from '../standalone-pages';
import {Button} from '../ui/button';
import {PRODUCT_GAMES, type GameId} from '../../src/contracts/product-catalog.mts';
import {isUiLocale, isUiMessageKey, withUiLanguage} from '../../src/launcher/i18n.mts';
import {retailRequiredFiles, supportsRetailImport} from '../../src/launcher/retail-package.mjs';
import {testFeaturesEnabled} from '../../src/launcher/test-features.mts';
import './pack.css';

const games = (Object.keys(PRODUCT_GAMES) as GameId[]).filter(supportsRetailImport);
export const meta = () => [{title: sitePageTitle(translate('zh-CN', 'package.originalTitle'))}];

export default function PackRoute() {
  return <StandalonePageShell titleKey="package.originalTitle" mainClassName="retail-pack-page"><PackForm/></StandalonePageShell>;
}

function PackForm() {
  const {t, locale} = useLocale();
  const base = useHref('/');
  const {search} = useLocation();
  const [enabled, setEnabled] = useState(false);
  const [game, setGame] = useState<GameId>(() => games.find(id => id === new URLSearchParams(search).get('game')) ?? games[0]);
  const [archives, setArchives] = useState<File[]>([]);
  const [fonts, setFonts] = useState<File[]>([]);
  const [music, setMusic] = useState<File | null>(null);
  const [busy, setBusy] = useState(false);
  const [status, setStatus] = useState('');
  const [error, setError] = useState('');
  const [installed, setInstalled] = useState(false);
  const [download, setDownload] = useState<{url: string; name: string} | null>(null);
  const archiveInput = useRef<HTMLInputElement>(null);
  const folderInput = useRef<HTMLInputElement>(null);
  const musicInput = useRef<HTMLInputElement>(null);
  const fontInput = useRef<HTMLInputElement>(null);
  const conversion = useRef<AbortController | null>(null);
  const downloadUrl = useRef<string | null>(null);
  const metadata = useRef<ReturnType<typeof createMetadataService> | null>(null);

  useEffect(() => {
    setEnabled(previewFeaturesEnabled && (new URLSearchParams(search).has('test') || testFeaturesEnabled(window)));
  }, [search]);

  useEffect(() => {
    if (!enabled) return;
    const service = createMetadataService({
      baseUrl: new URL(base, window.location.href).href,
      storage: null,
      translate: (language, key, params) => isUiMessageKey(key) ? translate(isUiLocale(language) ? language : 'zh-CN', key, params) : key,
      webAudioAvailable: false, webMidiAvailable: false, mobile: false, testBuild: true,
    });
    metadata.current = service;
    void service.initialize();
    return () => {
      conversion.current?.abort();
      metadata.current = null;
      service.dispose();
    };
  }, [base, enabled]);

  useEffect(() => () => {
    if (downloadUrl.current) URL.revokeObjectURL(downloadUrl.current);
  }, []);

  function resetResult() {
    if (downloadUrl.current) URL.revokeObjectURL(downloadUrl.current);
    downloadUrl.current = null;
    setDownload(null);
    setInstalled(false);
    setStatus('');
    setError('');
  }

  function selectArchives(files: FileList | null, folder: boolean) {
    if (!files?.length) return;
    setArchives(Array.from(files));
    if (folder) {
      setFonts([]);
      setMusic(null);
    }
    resetResult();
  }

  async function generate(action: 'import' | 'download') {
    if (!enabled || !archives.length || conversion.current || !metadata.current) return;
    const controller = new AbortController();
    conversion.current = controller;
    resetResult();
    setBusy(true);
    setStatus(t('package.originalConverting'));
    const acquisition = createPackageAcquisition({
      metadata: metadata.current,
      baseUrl: new URL(base, window.location.href).href,
      translate: t,
      requestPersistence: async () => navigator.storage?.persist?.(),
    });
    const inputs = [...archives, ...fonts, ...(music ? [music] : [])];
    const options = {
      signal: controller.signal,
      onProgress: ({message}: {message: string}) => {
        if (!controller.signal.aborted) setStatus(message);
      },
    };
    try {
      if (action === 'import') {
        await acquisition.importOriginal(game, inputs, options);
        controller.signal.throwIfAborted();
        setInstalled(true);
        setStatus(t('package.importedReady'));
      } else {
        const zip = await acquisition.exportOriginal(game, inputs, options);
        controller.signal.throwIfAborted();
        const url = URL.createObjectURL(zip);
        const name = game + '-local.zip';
        downloadUrl.current = url;
        setDownload({url, name});
        setStatus('');
        const link = document.createElement('a');
        link.href = url;
        link.download = name;
        document.body.append(link);
        link.click();
        link.remove();
      }
    } catch (failure) {
      setStatus('');
      if (!controller.signal.aborted) setError(preparationErrorText(failure));
    } finally {
      if (conversion.current === controller) {
        conversion.current = null;
        setBusy(false);
      }
    }
  }

  const files = retailRequiredFiles(game);
  const selected = [...archives, ...fonts, ...(music ? [music] : [])];
  const gameHref = withUiLanguage(base + '?test=1&game=' + game, locale);
  return <>
    <h1>{t('package.originalTitle')}</h1>
    {!enabled ? <p>{t('package.originalPreview')}</p> : <>
      <p>{t('package.originalPrivacy')}</p>
      <section className="retail-pack-panel" aria-busy={busy}>
        <div className="retail-pack-field">
          <label id="retail-pack-game-label" htmlFor="retail-pack-game">{t('package.originalGame')}</label>
          <MainSelect id="retail-pack-game" aria-labelledby="retail-pack-game-label" value={game} disabled={busy}
            onChange={event => {
              const next = games.find(id => id === event.target.value);
              if (!next || next === game) return;
              setGame(next);
              setArchives([]);
              setFonts([]);
              setMusic(null);
              resetResult();
            }}>
            {games.map(id => <option key={id} value={id}>{id.toUpperCase()} · {PRODUCT_GAMES[id].title}</option>)}
          </MainSelect>
        </div>
        <p>{t('package.originalRequired', {files: files.join('、')})}</p>
        <p>{t('package.originalExtras')}</p>
        <div className="retail-pack-actions">
          <Button variant="outline" disabled={busy} onClick={() => folderInput.current?.click()}>{t('package.originalFolder')}</Button>
          <Button variant="outline" disabled={busy} onClick={() => archiveInput.current?.click()}>{t('package.originalPick')}</Button>
          {files.length === 1 && <Button variant="outline" disabled={busy} onClick={() => musicInput.current?.click()}>{t('package.originalMusic')}</Button>}
          <Button variant="outline" disabled={busy} onClick={() => fontInput.current?.click()}>{t('package.originalFonts')}</Button>
        </div>
        <input ref={folderInput} type="file" multiple hidden {...{webkitdirectory: ''}} onChange={event => {
          selectArchives(event.currentTarget.files, true);
          event.currentTarget.value = '';
        }}/>
        <input ref={archiveInput} type="file" multiple hidden accept=".dat,.zip" onChange={event => {
          selectArchives(event.currentTarget.files, false);
          event.currentTarget.value = '';
        }}/>
        <input ref={musicInput} type="file" hidden accept=".dat" onChange={event => {
          const file = event.currentTarget.files?.[0];
          if (file) {setMusic(file); resetResult();}
          event.currentTarget.value = '';
        }}/>
        <input ref={fontInput} type="file" multiple hidden accept=".ttc,.otf" onChange={event => {
          const files = event.currentTarget.files;
          if (files?.length) {setFonts(Array.from(files)); resetResult();}
          event.currentTarget.value = '';
        }}/>
        {selected.length > 0 && <p>{t('package.originalSelected', {count: selected.length})} · {selected.slice(0, 3).map(file => file.name).join('、')}{selected.length > 3 ? '…' : ''}</p>}
        <div className="retail-pack-actions retail-pack-submit">
          <Button disabled={busy || !archives.length} onClick={() => void generate('import')}>{t('package.originalImport')}</Button>
          <Button variant="outline" disabled={busy || !archives.length} onClick={() => void generate('download')}>{t('package.originalExport')}</Button>
          {busy && <Button variant="ghost" onClick={() => conversion.current?.abort()}>{t('action.cancel')}</Button>}
        </div>
        {status && <p role="status">{status}</p>}
        {error && <p role="alert">{error}</p>}
        {download && <a className="retail-pack-link" href={download.url} download={download.name}>{download.name}</a>}
        {installed && <a className="retail-pack-link" href={gameHref}>{t('nav.single')}</a>}
      </section>
    </>}
  </>;
}
