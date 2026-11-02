import {Input, Textarea} from '../../ui/input';
import {MainSelect} from './MainSelect';
import {Check, Button as UiButton} from '../../ui/controls';
import {useCallback, useEffect, useRef, useState, type PointerEvent as ReactPointerEvent} from 'react';
import {createTrackerRenderer, trackerLayout, trackerDisplayName} from './tracker-renderer';
import {blankRecord, cellLabel, DEFAULT_OPTIONS, TRACKER_CELLS, TRACKER_GAMES, validCareerRecords, validOptions,
  type CareerRecords, type TrackerOptions, type TrackerRecord} from './tracker-records';
import {readBookPreference, writeBookPreference} from './book-preferences';
import {useLocale, type UiMessageKey} from '../../i18n';
import {ConfirmationDialog} from '../ConfirmationDialog';
import './career-showcase.css';
import '../../ui/document-body.css';

const careerButtonClass = 'max-[780px]:min-w-0 max-[780px]:h-auto max-[780px]:min-h-11 max-[780px]:whitespace-normal max-[780px]:px-1 max-[780px]:text-xs';

type Cell = {key: string; x: number; y: number};
type View = {scale: number; x: number; y: number};
const chartHeight = (options: TrackerOptions) => trackerLayout(options).height;
const completed = (records: CareerRecords) => Object.values(records).filter(record => record.done).length;
function saveFile(name: string, url: string) {const link = document.createElement('a'); link.download = name; link.href = url; link.click();}

function Chart({records, options, onCells}: {records: CareerRecords; options: TrackerOptions; onCells?: (cells: Cell[]) => void}) {
  const {t} = useLocale();
  const own = useRef<HTMLCanvasElement>(null);
  useEffect(() => {
    const canvas = own.current;
    if (!canvas) return;
    const renderer = createTrackerRenderer(canvas);
    let active = true;
    const draw = () => {if (active) {const cells = renderer.draw(records, options, document.documentElement.dataset.bookTheme !== 'light', t); onCells?.(cells);}};
    const observer = new MutationObserver(draw);
    observer.observe(document.documentElement, {attributes: true, attributeFilter: ['data-book-theme']});
    draw();
    void document.fonts.ready.then(draw);
    return () => {active = false; observer.disconnect();};
  }, [records, options, onCells, t]);
  return <canvas ref={own} width={800} height={chartHeight(options)} aria-label={t('career.chartAria')}/>;
}

export function CareerSummary({records}: {records: CareerRecords}) {
  const {t} = useLocale();
  const [expanded, setExpanded] = useState(false);
  const previewHeight = trackerLayout(DEFAULT_OPTIONS).sections[0].rows[1].y - 12;
  return <section className="profile-panel career-overview"><div className="profile-section-title"><h2>{t('career.title')}</h2><span className="career-total"><b>{completed(records)}</b>{t('career.completedUnit')}</span><UiButton variant="quiet" type="button" className={`${careerButtonClass} career-overview-toggle`} aria-expanded={expanded} onClick={() => setExpanded(value => !value)}>{expanded ? t('career.collapse') : t('career.expand')}</UiButton></div>
    <UiButton variant="unstyled" type="button" className="career-thumbnail" style={expanded ? undefined : {aspectRatio: "800 / " + previewHeight}} data-expanded={expanded} aria-expanded={expanded} onClick={() => setExpanded(value => !value)} aria-label={expanded ? t('career.collapseAria') : t('career.expandAria')}><Chart records={records} options={DEFAULT_OPTIONS}/></UiButton></section>;
}

export function CareerShowcase({records, onSave, initiallyOpen = false, readOnly = false}: {records: CareerRecords; onSave: (next: CareerRecords) => void; initiallyOpen?: boolean; readOnly?: boolean}) {
  const {t} = useLocale();
  const [options, setOptions] = useState(DEFAULT_OPTIONS);
  const [helpOpen, setHelpOpen] = useState(false);
  const [open, setOpen] = useState(initiallyOpen), [editRequested, setEditing] = useState(false), [draft, setDraft] = useState<CareerRecords>({});
  const editing = editRequested && !readOnly;
  const writable = useRef(!readOnly); writable.current = !readOnly;
  const [selection, setSelection] = useState<string | null>(null), [brush, setBrush] = useState('details');
  const [cells, setCells] = useState<Cell[]>([]), [view, setView] = useState<View>({scale: 1, x: 0, y: 0});
  const [confirmClose, setConfirmClose] = useState(false), [message, setMessage] = useState<UiMessageKey | null>(null);
  const dialog = useRef<HTMLDialogElement>(null), viewport = useRef<HTMLDivElement>(null);
  const returnFocus = useRef<HTMLElement | null>(null), input = useRef<HTMLInputElement>(null), fitted = useRef(false);
  const pointers = useRef(new Map<number, {x: number; y: number}>()), gesture = useRef({moved: false, startX: 0, startY: 0});
  const shown = editing ? draft : records;
  const height = chartHeight(options);
  const dirty = editing && JSON.stringify(draft) !== JSON.stringify(records);
  useEffect(() => {setOptions(readBookPreference('career-chart-options-v1', DEFAULT_OPTIONS, validOptions));}, []);
  const changeOptions = (next: TrackerOptions) => {setOptions(next); if (!readOnly) writeBookPreference('career-chart-options-v1', next); fitted.current = false;};
  const fitMap = (whole = false) => {
    const area = viewport.current; if (!area) return;
    const scale = Math.max(.15, Math.min((area.clientWidth - 48) / 800, whole ? (area.clientHeight - 48) / height : 6));
    setView({scale, x: (area.clientWidth - 800 * scale) / 2, y: whole ? (area.clientHeight - height * scale) / 2 : 24}); fitted.current = true;
  };
  const fitRef = useRef(fitMap); fitRef.current = fitMap;
  useEffect(() => {
    if (!open) return;
    const openedDialog = dialog.current; openedDialog?.showModal();
    const overflow = document.body.style.overflow; document.body.style.overflow = 'hidden';
    if (!fitted.current) fitRef.current();
    const observer = new ResizeObserver(() => {if (!fitted.current) fitRef.current();});
    if (viewport.current) observer.observe(viewport.current);
    const resize = () => fitRef.current(); window.addEventListener('resize', resize);
    return () => {observer.disconnect(); window.removeEventListener('resize', resize); document.body.style.overflow = overflow; openedDialog?.close(); pointers.current.clear(); returnFocus.current?.focus({preventScroll:true});};
  }, [open]);
  // biome-ignore lint/correctness/useExhaustiveDependencies: Refit the current viewport when its requested height changes.
  useEffect(() => {if (open) fitRef.current();}, [height, open]);
  const zoom = useCallback((factor: number, x?: number, y?: number) => {
    const area = viewport.current; if (!area) return;
    const ax = x ?? area.clientWidth / 2, ay = y ?? area.clientHeight / 2;
    setView(previous => {const scale = Math.max(.15, Math.min(6, previous.scale * factor)); const ratio = scale / previous.scale;
      return {scale, x: ax - (ax - previous.x) * ratio, y: ay - (ay - previous.y) * ratio};});
  }, []);
  useEffect(() => {
    const area = viewport.current; if (!open || !area) return;
    const wheel = (event: WheelEvent) => {event.preventDefault(); const rect = area.getBoundingClientRect(); zoom(Math.exp(-event.deltaY * .0015), event.clientX - rect.left, event.clientY - rect.top);};
    area.addEventListener('wheel', wheel, {passive: false});
    return () => area.removeEventListener('wheel', wheel);
  }, [open, zoom]);
  const launch = (edit = false) => {returnFocus.current = document.activeElement as HTMLElement; setOpen(true); setConfirmClose(false); if (edit && !readOnly) {setDraft({...records}); setEditing(true);} };
  const close = () => {if (dirty) {setConfirmClose(true); return;} setOpen(false); setEditing(false); setSelection(null);};
  const update = (key: string, record?: TrackerRecord) => setDraft(current => {const next = {...current}; if (record) next[key] = record; else delete next[key]; return next;});
  const select = (key: string) => {
    setSelection(key);
    if (!editing || brush === 'details') return;
    if (brush === 'erase') update(key);
    else update(key, {...(draft[key] ?? blankRecord(key)), done: true, continued: false,
      misses: brush === 'nm' || brush === 'nn' ? '0' : null, bombs: brush === 'nb' || brush === 'nn' ? '0' : null});
  };
  const pointerDown = (event: ReactPointerEvent<HTMLDivElement>) => {
    if (event.button !== 0) return;
    event.currentTarget.setPointerCapture(event.pointerId);
    pointers.current.set(event.pointerId, {x: event.clientX, y: event.clientY});
    if (pointers.current.size === 1) gesture.current = {moved: false, startX: event.clientX, startY: event.clientY};
    else gesture.current.moved = true;
  };
  const pointerMove = (event: ReactPointerEvent<HTMLDivElement>) => {
    const previous = pointers.current.get(event.pointerId); if (!previous) return;
    const before = [...pointers.current.values()];
    pointers.current.set(event.pointerId, {x: event.clientX, y: event.clientY});
    const after = [...pointers.current.values()];
    if (Math.hypot(event.clientX - gesture.current.startX, event.clientY - gesture.current.startY) > 5) gesture.current.moved = true;
    if (before.length === 2) {
      const distance = (points: typeof before) => Math.hypot(points[0].x - points[1].x, points[0].y - points[1].y);
      const oldDistance = distance(before); if (oldDistance < 1) return;
      const rect = event.currentTarget.getBoundingClientRect();
      const oldX = (before[0].x + before[1].x) / 2 - rect.left, oldY = (before[0].y + before[1].y) / 2 - rect.top;
      const newX = (after[0].x + after[1].x) / 2 - rect.left, newY = (after[0].y + after[1].y) / 2 - rect.top;
      setView(view => {const scale = Math.max(.15, Math.min(6, view.scale * distance(after) / oldDistance)); const ratio = scale / view.scale;
        return {scale, x: newX - (oldX - view.x) * ratio, y: newY - (oldY - view.y) * ratio};});
    } else if (before.length === 1 && gesture.current.moved) setView(view => ({...view, x: view.x + event.clientX - previous.x, y: view.y + event.clientY - previous.y}));
  };
  const pointerUp = (event: ReactPointerEvent<HTMLDivElement>) => {
    if (!pointers.current.has(event.pointerId)) return;
    pointers.current.delete(event.pointerId);
    if (gesture.current.moved || event.type === 'pointercancel') return;
    const rect = event.currentTarget.getBoundingClientRect();
    const x = (event.clientX - rect.left - view.x) / view.scale, y = (event.clientY - rect.top - view.y) / view.scale;
    const cell = cells.find(cell => x >= cell.x && x < cell.x + 17 && y >= cell.y && y < cell.y + 17);
    if (cell) select(cell.key);
  };
  const exportPng = async () => {
    const canvas = document.createElement('canvas'); createTrackerRenderer(canvas).draw(shown, options, document.documentElement.dataset.bookTheme !== 'light', t);
    saveFile('touhou-1cc-chart.png', canvas.toDataURL('image/png'));
  };
  const exportJson = () => {const url = URL.createObjectURL(new Blob([JSON.stringify({version: 2, records: shown, options}, null, 2)], {type: 'application/json'})); saveFile('touhou-career.json', url); setTimeout(() => URL.revokeObjectURL(url), 1000);};
  return <section className="profile-panel career-showcase" aria-label={t('career.sectionAria')}>
    <div className="profile-section-title career-heading"><h2>{t('career.title')}</h2><span className="career-total"><b>{completed(records)}</b>{t('career.completedUnit')}</span></div>
    <UiButton variant="unstyled" type="button" className="career-thumbnail" onClick={() => launch()} aria-label={t('career.fullscreen')}><Chart records={records} options={options}/></UiButton>
    <div className="career-card-footer"><div>{!readOnly && <UiButton className={careerButtonClass} type="button" onClick={() => launch(true)}>{t('career.edit')}</UiButton>}<UiButton className={careerButtonClass} type="button" onClick={exportPng}>{t('career.exportPng')}</UiButton></div></div>

    {open && <dialog ref={dialog} className="career-viewer" aria-labelledby="career-viewer-title" onCancel={event => {event.preventDefault(); close();}}>
      <header className="career-viewer-header"><div><h2 id="career-viewer-title">{editing ? t('career.editing') : t('career.title')}</h2><span className="career-viewer-count">{completed(shown)} / {TRACKER_CELLS.length}</span></div><div className="career-viewer-actions">
        {editing ? <><UiButton className={careerButtonClass} type="button" onClick={() => {if (dirty) setConfirmClose(true); else {setEditing(false); setSelection(null);}}}>{t('career.cancelEdit')}</UiButton><UiButton variant="primary" type="button" className={careerButtonClass} onClick={() => {if (!writable.current) return; onSave(draft); setEditing(false); setSelection(null); setConfirmClose(false); setMessage('career.saved');}}>{t('career.save')}</UiButton></> : !readOnly && <UiButton className={careerButtonClass} type="button" onClick={() => {setDraft({...records}); setEditing(true);}}>{t('career.edit')}</UiButton>}
        <UiButton type="button" className={`${careerButtonClass} career-help-trigger`} onClick={() => setHelpOpen(true)}><span aria-hidden="true">?</span>{t('career.help')}</UiButton><UiButton className={careerButtonClass} type="button" onClick={exportPng}>{t('career.exportPng')}</UiButton><UiButton className={careerButtonClass} variant="icon" type="button" aria-label={t('career.close')} onClick={close}>×</UiButton></div></header>
      <div className="career-viewer-body"><div className="career-map-column">
        <div className="career-map-tools"><div><UiButton className={careerButtonClass} type="button" aria-label={t('career.zoomOut')} onClick={() => zoom(1 / 1.25)}>−</UiButton><output aria-label={t('career.zoomLevel')}>{Math.round(view.scale * 100)}%</output><UiButton className={careerButtonClass} type="button" aria-label={t('career.zoomIn')} onClick={() => zoom(1.25)}>＋</UiButton><UiButton className={careerButtonClass} type="button" onClick={() => fitMap(true)}>{t('career.fit')}</UiButton><UiButton className={careerButtonClass} type="button" onClick={() => zoom(1 / view.scale)}>{t('career.actualSize')}</UiButton></div><span>{editing ? t('career.selectHint') : t('career.gestureHint')}</span></div>
        <div className="career-map-viewport" ref={viewport} onPointerDown={pointerDown} onPointerMove={pointerMove} onPointerUp={pointerUp} onPointerCancel={pointerUp} onLostPointerCapture={event => pointers.current.delete(event.pointerId)}>
          <div className="career-map-sheet" style={{width: 800, height, transform: `translate(${view.x}px, ${view.y}px) scale(${view.scale})`}}>
            <Chart records={shown} options={options} onCells={setCells}/>
            {cells.map(cell => <UiButton variant="unstyled" type="button" key={cell.key} tabIndex={selection === cell.key ? 0 : -1} data-cell-key={cell.key} className={`career-map-cell${selection === cell.key ? ' is-selected' : ''}`} style={{left: cell.x, top: cell.y}} aria-label={`${cellLabel(cell.key, t)} | ${shown[cell.key]?.done ? t('career.done') : t('career.notDone')}`} aria-pressed={!!shown[cell.key]?.done} title={cellLabel(cell.key, t)} onClick={event => {if (event.detail === 0) select(cell.key);}} onFocus={event => {if (event.currentTarget.matches(':focus-visible')) {setSelection(cell.key); setView(view => ({...view, x: (viewport.current?.clientWidth ?? 800) / 2 - cell.x * view.scale, y: (viewport.current?.clientHeight ?? 600) / 2 - cell.y * view.scale}));}}}/>)}
          </div>
        </div>
        <footer className="career-map-footer"><span>{t('career.totals', {clears: completed(shown), cells: cells.length})}</span></footer>
      </div>
      <aside className="career-map-sidebar">
        {editing && <section className="career-tool-section"><h3>{t('career.tools')}</h3><div className="career-brushes">{[['details', t('career.detailsBrush')], ['1cc', '1CC'], ['nm', 'NM'], ['nb', 'NB'], ['nn', 'NN'], ['erase', t('career.erase')]].map(([id, label]) => <UiButton className={careerButtonClass} type="button" key={id} aria-pressed={brush === id} onClick={() => setBrush(id)}>{label}</UiButton>)}</div></section>}
        <CellFinder visibleKeys={cells.map(cell => cell.key)} selection={selection} onLocate={key => {
          setSelection(key);
          const cell = cells.find(cell => cell.key === key);
          if (cell) setView({scale: 2, x: (viewport.current?.clientWidth ?? 800) / 2 - cell.x * 2, y: (viewport.current?.clientHeight ?? 600) / 2 - cell.y * 2});
        }}/>
        <section className="career-tool-section"><div className="career-detail-title"><h3>{selection ? cellLabel(selection, t) : t('career.details')}</h3><UiButton type="button" className={`${careerButtonClass} career-detail-help`} aria-label={t('career.detailsHelp')} onClick={() => setHelpOpen(true)}>?</UiButton></div>
          {selection ? <CellEditor key={selection + JSON.stringify(shown[selection])} record={shown[selection] ?? blankRecord(selection)} editing={editing && brush === 'details'} onApply={record => update(selection, record)}/>
          : <p>{t('career.selectCell')}</p>}

        </section>
        <section className="career-tool-section"><h3>{t('career.display')}</h3>{([['easy', t('career.showEasy')], ['extra', t('career.showExtras')], ['transparent', t('career.transparent')]] as const).map(([key, label]) => <label className="career-check" key={key}><Check checked={options[key]} onCheckedChange={event => changeOptions({...options, [key]: event})}/>{label}</label>)}</section>
        <section className="career-tool-section"><h3>{t('career.backup')}</h3><div className="career-backup-actions"><UiButton className={careerButtonClass} type="button" onClick={exportJson}>{t('career.exportJson')}</UiButton>{!readOnly && <UiButton className={careerButtonClass} type="button" onClick={() => input.current?.click()}>{t('career.importJson')}</UiButton>}</div>{!readOnly && <input hidden ref={input} type="file" accept="application/json,.json" onChange={async event => {
          const file = event.target.files?.[0]; event.target.value = ''; if (!file || !writable.current) return;
          try {if (file.size > 2_000_000) throw new Error(); const data = JSON.parse(await file.text()); if (data.version !== 2 || !validCareerRecords(data.records)) throw new Error();
            if (!writable.current) return; setDraft(data.records); setEditing(true); setSelection(null); setMessage('career.imported'); if (validOptions(data.options)) changeOptions(data.options);
          } catch {setMessage('career.importError');}
        }}/>}{!readOnly && <p>{t('career.importHint')}</p>}</section>
      </aside></div>{message && <div className="career-viewer-status" role="status">{t(message)}</div>}
    {helpOpen && <CareerHelp onClose={() => setHelpOpen(false)}/>}
    </dialog>}
    {confirmClose && <ConfirmationDialog idPrefix="careerDecision" open title={t('career.cancelEdit')} message={t('career.unsaved')} cancelText={t('career.continue')} confirmText={t('career.discard')} tone="danger" onCancel={() => setConfirmClose(false)} onConfirm={() => {setConfirmClose(false); setEditing(false); setOpen(false); setSelection(null);}}/>}
  </section>;
}

const recordCounts = Array.from({length: 10}, (_, value) => value);

function CellFinder({visibleKeys, selection, onLocate}: {visibleKeys: string[]; selection: string | null; onLocate(key: string): void}) {
  const {t} = useLocale();
  const [gameName, setGameName] = useState('EOSD'), [difficulty, setDifficulty] = useState(''), [query, setQuery] = useState('');
  const selected = TRACKER_CELLS.find(cell => cell.key === selection);
  useEffect(() => {if (selected) {setGameName(selected.game.name); setDifficulty(selected.difficulty); setQuery('');}}, [selected]);
  const visible = new Set(visibleKeys);
  const available = TRACKER_CELLS.filter(cell => visible.has(cell.key));
  const games = TRACKER_GAMES.filter(game => available.some(cell => cell.game.name === game.name));
  const game = games.find(game => game.name === gameName) ?? games[0];
  const gameCells = available.filter(cell => cell.game.name === game?.name);
  const difficulties = game?.difficulties.filter(value => gameCells.some(cell => cell.difficulty === value)) ?? [];
  const activeDifficulty = difficulties.includes(difficulty) ? difficulty : difficulties[0];
  const results = gameCells.filter(cell => cell.difficulty === activeDifficulty && cell.character.toLowerCase().includes(query.trim().toLowerCase()));
  return <section className="career-tool-section career-cell-finder" aria-label={t('career.findAria')}><h3>{t('career.find')}</h3>
    <label>{t('career.game')}<MainSelect aria-label={t('career.findGame')} value={game?.name ?? ''} onChange={event => {setGameName(event.target.value); setDifficulty(''); setQuery('');}}>{games.map(game => <option value={game.name} key={game.name}>{trackerDisplayName(game)} | {t(game.titleKey)}</option>)}</MainSelect></label>
    <fieldset className="career-finder-difficulties" aria-label={t('career.findDifficulty')}>{difficulties.map(value => <UiButton size="icon" className={careerButtonClass} type="button" key={value} aria-pressed={activeDifficulty === value} onClick={() => setDifficulty(value)}>{value}</UiButton>)}</fieldset>
    {results.length > 12 || query ? <Input type="search" aria-label={t('career.filterAria')} placeholder={t('career.filter')} value={query} onChange={event => setQuery(event.target.value)}/> : null}
    <fieldset className="career-finder-results" aria-label={t('career.locate')}>{results.map(cell => <UiButton className={careerButtonClass} type="button" key={cell.key} aria-pressed={selection === cell.key} data-locate-cell={cell.key} onClick={() => onLocate(cell.key)}>{cell.character}</UiButton>)}</fieldset>
    {!results.length && <p>{t('career.noMatches')}</p>}
  </section>;
}

function CellEditor({record, editing, onApply}: {record: TrackerRecord; editing: boolean; onApply: (record?: TrackerRecord) => void}) {
  const {t} = useLocale();
  if (!editing) return <div className="career-record-description"><strong className={record.done ? 'is-complete' : ''}>{record.done ? t('career.clear1cc') : record.continued ? t('career.continued') : t('career.noRecord')}</strong><dl className="career-record-counts">{[['Miss', record.misses], ['Bomb', record.bombs], [t('career.lives'), record.lives]].map(([label, value]) => <div key={label}><dt>{label}</dt><dd>{value ?? '—'}</dd></div>)}</dl><div className="career-record-tags">{[record.vertical && 'NV', record.focus && 'NF', record.pacifist && 'P', record.unique && 'U', record.challenge].filter(Boolean).map(tag => <span key={String(tag)}>{tag}</span>)}</div>{record.note && <p className="career-record-note">{record.note}</p>}</div>;
  return <form className="career-cell-form" onSubmit={event => {
    event.preventDefault(); const data = new FormData(event.currentTarget);
    const next = {...record};
    for (const key of ['done', 'vertical', 'focus', 'pacifist', 'unique'] as const) next[key] = data.has(key);
    for (const key of ['misses', 'bombs', 'lives'] as const) next[key] = String(data.get(key) ?? '') || null;
    next.continued = data.has('continued');
    next.cellColour = '#ffffff'; next.textColour = '#000000';
    next.note = String(data.get('note') ?? '').trim(); next.challenge = String(data.get('challenge') ?? '').trim();
    onApply(next);
  }}>
    <label className="career-check"><Check name="done" defaultChecked={record.done}/>{t('career.mark1cc')}</label>
    <label className="career-check"><Check name="continued" defaultChecked={record.continued}/>{t('career.markContinued')}</label>
    <div className="career-count-fields">{([['misses', 'Miss'], ['bombs', 'Bomb'], ['lives', t('career.lives')]] as const).map(([key, label]) => <label key={key}>{label}<MainSelect name={key} aria-label={label} defaultValue={record[key] ?? ''}><option value="">{t('career.unknown')}</option>{recordCounts.map(n => <option key={n} value={n}>{n}</option>)}</MainSelect></label>)}</div>
    <fieldset><legend>{t('career.restrictions')}</legend>{([['vertical', t('career.nvOption')], ['focus', t('career.nfOption')], ['pacifist', t('career.pOption')], ['unique', t('career.uOption')]] as const).map(([key, label]) => <label className="career-check" key={key}><Check name={key} defaultChecked={record[key]}/>{label}</label>)}</fieldset>
    <label>{t('career.custom')}<Input name="challenge" maxLength={24} defaultValue={record.challenge}/></label><label>{t('career.note')}<Textarea name="note" rows={2} maxLength={240} defaultValue={record.note}/></label>
    <div className="career-backup-actions"><UiButton className={careerButtonClass} type="button" onClick={() => onApply()}>{t('career.remove')}</UiButton><UiButton variant="primary" className={careerButtonClass} type="submit">{t('career.apply')}</UiButton></div>
  </form>;
}

function CareerHelp({onClose}: {onClose: () => void}) {
  const {t} = useLocale();
  const dialog = useRef<HTMLDialogElement>(null);
  useEffect(() => {
    const opened = dialog.current;
    const previous = document.activeElement as HTMLElement | null;
    opened?.showModal();
    return () => {opened?.close(); previous?.focus({preventScroll:true});};
  }, []);
  return <dialog ref={dialog} className="career-help-dialog" aria-labelledby="career-help-title" onCancel={event => {event.preventDefault(); event.stopPropagation(); onClose();}}>
    <header><h2 id="career-help-title">{t('career.helpTitle')}</h2><UiButton className={careerButtonClass} variant="icon" type="button" aria-label={t('career.closeHelp')} onClick={onClose}>×</UiButton></header>
    <div className="career-help-content ui-document prose">
      <section><h3>{t('career.readCell')}</h3><div className="career-help-example"><div className="career-example-cell" role="img" aria-label={t('career.exampleAria')}><span>2</span><span>0</span><span>3</span><b>F</b></div><p>{t('career.cellGuide')}</p></div></section>
      <section><h3>{t('career.difficultyClears')}</h3><dl className="career-help-definitions"><div><dt>E / N / H / L</dt><dd>Easy / Normal / Hard / Lunatic</dd></div><div><dt>X / P</dt><dd>Extra / Phantasm</dd></div><div><dt>1CC</dt><dd>{t('career.oneCreditGuide')}</dd></div><div><dt>NM / NB / NN</dt><dd>{t('career.noMissBombGuide')}</dd></div></dl><p>{t('career.countGuide')}</p></section>
      <section><h3>{t('career.restrictions')}</h3><dl className="career-help-definitions"><div><dt>{t('career.nvNotation')}</dt><dd>{t('career.nv')}</dd></div><div><dt>F / NF</dt><dd>{t('career.nf')}</dd></div><div><dt>P</dt><dd>{t('career.pacifist')}</dd></div><div><dt>U</dt><dd>{t('career.uniqueGuide')}</dd></div></dl></section>
      <section><h3>{t('career.routes')}</h3><dl className="career-help-definitions"><div><dt>{t('career.inRoutes')}</dt><dd>{t('career.inFinals')}</dd></div><div><dt>{t('career.lolkModes')}</dt><dd>{t('career.lolkModesGuide')}</dd></div><div><dt>{t('career.hsifsSeasons')}</dt><dd>{t('career.seasons')}</dd></div><div><dt>e1 / e2</dt><dd>{t('career.endings')}</dd></div></dl><p>{t('career.photoGuide')}</p></section>
      <section><h3>{t('career.viewEdit')}</h3><p>{t('career.gesturesGuide')}</p><p>{t('career.editGuide')}</p><p>{t('career.exportGuide')}</p></section>
      <section><h3>{t('career.gameNumbers')}</h3><dl className="career-help-games">{TRACKER_GAMES.map(game => <div key={game.name}><dt><i style={{background: game.colour}}/>{trackerDisplayName(game)}</dt><dd>{t(game.titleKey)}</dd></div>)}</dl></section>
    </div>
  </dialog>;
}
