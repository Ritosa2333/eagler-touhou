import { useEffect, useState } from 'react';
import { useLocale } from '../../i18n';
import { linkAbortSignals } from '../../services/abort-signals';
import { Button } from '../ui/button';

type Range = '24h' | '7d' | '90d';
interface Point {
  at: string;
  samples: number;
  availability: number | null;
  latencyMs: number | null;
}
interface Service {
  id: string;
  label: string;
  group: string;
  status: string;
  checkedAt: string | null;
  latencyMs: number | null;
  availability: number | null;
  points: Point[];
}
interface Summary {
  schema: string;
  range: Range;
  updatedAt: string;
  services: Service[];
}
const ranges = ['24h', '7d', '90d'] as const;
const percentage = (value: number | null) => (value === null ? '—' : `${(value * 100).toFixed(2)}%`);
const latency = (value: number | null) => (value === null ? '—' : `${Math.round(value)} ms`);

export function UptimePage({ endpoint }: { endpoint: string }) {
  const { locale, t } = useLocale();
  const [range, setRange] = useState<Range>('24h');
  const [data, setData] = useState<Summary | null>(null),
    [failed, setFailed] = useState(false);
  useEffect(() => {
    setData(null);
    setFailed(false);
    let disposed = false,
      busy = false;
    const controller = new AbortController();
    async function refresh() {
      if (busy || document.hidden) return;
      busy = true;
      const timeout = new AbortController(),
        linked = linkAbortSignals(controller.signal, timeout.signal);
      const deadline = setTimeout(() => timeout.abort(), 10000);
      try {
        const url = new URL(endpoint);
        url.searchParams.set('range', range);
        const response = await fetch(url, { cache: 'no-store', signal: linked.signal });
        if (!response.ok) throw Error('unavailable');
        const value = (await response.json()) as Summary;
        if (
          value.schema !== 'eagler-uptime/1' ||
          value.range !== range ||
          !Array.isArray(value.services) ||
          value.services.length > 24 ||
          value.services.some(
            (s) =>
              typeof s.id !== 'string' ||
              typeof s.label !== 'string' ||
              !Array.isArray(s.points) ||
              s.points.length > 90,
          )
        )
          throw Error('invalid summary');
        if (!disposed) {
          setData(value);
          setFailed(false);
        }
      } catch {
        if (!disposed) setFailed(true);
      } finally {
        clearTimeout(deadline);
        linked.dispose();
        busy = false;
      }
    }
    void refresh();
    const timer = setInterval(() => void refresh(), 30000);
    const visible = () => {
      if (!document.hidden) void refresh();
    };
    document.addEventListener('visibilitychange', visible);
    return () => {
      disposed = true;
      controller.abort();
      clearInterval(timer);
      document.removeEventListener('visibilitychange', visible);
    };
  }, [range, endpoint]);
  const state = (value: string) =>
    ({
      operational: t('uptime.normal'),
      outage: t('uptime.outage'),
      stale: t('uptime.unknown'),
      unknown: t('uptime.unknown'),
    })[value] ?? t('uptime.unknown');
  const labels: Record<string, string> = {
    site: t('uptime.site'),
    download: t('uptime.download'),
    netplay: t('uptime.netplay'),
    turn: t('uptime.turn'),
  };
  return (
    <>
      <div className="uptime-heading">
        <h1>{t('nav.serviceStatus')}</h1>
        <div className="uptime-ranges" role="group" aria-label={t('uptime.historyRange')}>
          {ranges.map((r, i) => (
            <Button
              variant="legacy"
              size="legacy"
              key={r}
              type="button"
              aria-pressed={range === r}
              onClick={() => setRange(r)}
            >
              {[t('uptime.range24h'), t('uptime.range7d'), t('uptime.range90d')][i]}
            </Button>
          ))}
        </div>
      </div>
      {failed && (
        <p role="alert" className="uptime-message">
          {t('uptime.failed')}
        </p>
      )}
      {!data && !failed && (
        <p role="status" className="uptime-message">
          {t('uptime.loading')}
        </p>
      )}
      {data && (
        <div className="uptime-list" aria-label={t('uptime.services')} aria-busy={data.range !== range}>
          <div className="uptime-columns" aria-hidden="true">
            <span>{t('uptime.status')}</span>
            <span>{t('uptime.service')}</span>
            <span>{t('uptime.history', { range: data.range })}</span>
            <span>{t('uptime.response')}</span>
            <span>{t('uptime.availability')}</span>
          </div>
          {Object.entries(labels).map(([group, label]) => {
            const services = data.services.filter((s) => s.group === group);
            return (
              services.length > 0 && (
                <section className="uptime-group" key={group} aria-label={label}>
                  <h2>{label}</h2>
                  {services.map((s) => {
                    const stale = failed || !s.checkedAt || Date.now() - Date.parse(s.checkedAt) > 90000;
                    const status = stale ? 'unknown' : s.status;
                    return (
                      <div className="uptime-row" key={s.id} data-status={status}>
                        <span className="uptime-status">{state(status)}</span>
                        <span className="uptime-service">{s.label}</span>
                        <div
                          className="uptime-history"
                          role="img"
                          aria-label={`${s.label} | ${data.range} | ${percentage(s.availability)}`}
                        >
                          {s.points.map((p) => (
                            <i
                              key={p.at}
                              data-state={
                                p.availability === null
                                  ? 'unknown'
                                  : p.availability === 1
                                    ? 'up'
                                    : p.availability === 0
                                      ? 'down'
                                      : 'partial'
                              }
                              title={`${new Date(p.at).toLocaleString(locale)} | ${percentage(p.availability)} | ${latency(p.latencyMs)}`}
                            />
                          ))}
                        </div>
                        <span className="uptime-latency">{latency(stale ? null : s.latencyMs)}</span>
                        <span className="uptime-availability" title={t('uptime.availabilityHint')}>
                          {percentage(s.availability)}
                        </span>
                      </div>
                    );
                  })}
                </section>
              )
            );
          })}
          {!data.services.length && <p className="uptime-message">{t('uptime.empty')}</p>}
        </div>
      )}
    </>
  );
}
