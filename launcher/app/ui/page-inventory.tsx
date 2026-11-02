import { ArrowUpRight, Search } from 'lucide-react';
import { type ReactNode, useCallback, useEffect, useState } from 'react';
import { Link, useLocation } from 'react-router';
import { Button } from './button';
import { ChoiceGroup, ChoiceItem } from './choice-group';
import { InputGroup, InputGroupInput } from './input-group';
import { type CatalogPage, type CatalogRegion, catalogAnchor, catalogPages, catalogTarget } from './page-catalog';
import { PageExample } from './page-examples';
import { RoomExample } from './room-example';

export function PageInventory({ announce = () => {} }: { announce?(message: string): void }) {
  const location = useLocation();
  const [query, setQuery] = useState(''),
    [area, setArea] = useState('全部');
  const [openPage, setOpenPage] = useState<string | null>(() => catalogTarget(location.hash)?.page.id ?? null);
  const [request, setRequest] = useState<ReturnType<typeof catalogTarget>>(null);
  const reveal = useCallback((hash: string) => {
    const target = catalogTarget(hash);
    if (!target) return;
    setOpenPage(target.page.id);
    setQuery('');
    setArea('全部');
    setRequest(target);
  }, []);
  // Router links and POP both expose targets within initially closed pages.
  useEffect(() => {
    reveal(location.hash);
  }, [location, reveal]);
  useEffect(() => {
    if (!request || openPage !== request.page.id) return;
    const frame = requestAnimationFrame(() => {
      document.getElementById(request.id)?.scrollIntoView({ block: 'start' });
      setRequest(null);
    });
    return () => cancelAnimationFrame(frame);
  }, [request, openPage]);
  const pages = catalogPages.filter(
    (page) =>
      (area === '全部' || page.area === area) &&
      JSON.stringify(page).toLowerCase().includes(query.trim().toLowerCase()),
  );
  const jump = (id: string, label: ReactNode, className?: string) => (
    <Link key={id} className={className} to={`#${id}`} onClick={() => reveal(id)}>
      {label}
    </Link>
  );
  const renderRegion = (page: CatalogPage, region: CatalogRegion, sample: ReactNode) => (
    <section
      className="catalog-page-region"
      id={catalogAnchor(page.id, region.id)}
      key={region.id}
      aria-labelledby={`${catalogAnchor(page.id, region.id)}-title`}
    >
      <header>
        <h4 id={`${catalogAnchor(page.id, region.id)}-title`}>
          {jump(catalogAnchor(page.id, region.id), region.name)}
        </h4>
        {jump(
          `example-${page.id}-${region.id}`,
          <>
            检查本区
            <ArrowUpRight aria-hidden="true" />
          </>,
          'catalog-sample-link',
        )}
      </header>
      <ul className="catalog-parts">
        {region.parts.map((part) => (
          <li key={part.id} id={catalogAnchor(page.id, region.id, part.id)}>
            <h5>{jump(catalogAnchor(page.id, region.id, part.id), part.name)}</h5>
            <ul className="catalog-part-states" aria-label={`${part.name}的检查状态`}>
              {part.states.map((state, index) => (
                <li key={state} id={catalogAnchor(page.id, region.id, part.id, index)}>
                  {jump(catalogAnchor(page.id, region.id, part.id, index), state)}
                </li>
              ))}
            </ul>
          </li>
        ))}
      </ul>
      <div className="catalog-region-example" id={`example-${page.id}-${region.id}`}>
        {sample}
      </div>
    </section>
  );
  return (
    <>
      <div className="catalog-module-tools">
        <InputGroup>
          <Search aria-hidden="true" />
          <InputGroupInput
            aria-label="查找页面或部件"
            placeholder="查找房间、隐私、按键…"
            value={query}
            onChange={(event) => setQuery(event.target.value)}
          />
        </InputGroup>
        <ChoiceGroup
          type="single"
          value={area}
          onValueChange={(value) => value && setArea(value)}
          aria-label="页面分类"
          className="catalog-choice-group"
        >
          {['全部', '浏览', '游玩', '联机', '账户', '支持'].map((value) => (
            <ChoiceItem key={value} value={value}>
              {value}
            </ChoiceItem>
          ))}
        </ChoiceGroup>
      </div>
      <p className="catalog-result-count" role="status">
        {pages.length} 个页面与工作区
      </p>
      <nav className="catalog-page-index" aria-label="按页面查看部件">
        {pages.map((page) => jump(catalogAnchor(page.id), page.name))}
      </nav>
      <div className="catalog-page-inventory">
        {pages.map((page) => (
          <details key={page.id} id={catalogAnchor(page.id)} open={openPage === page.id}>
            {/* biome-ignore lint/a11y/noStaticElementInteractions: Native summary supplies disclosure keyboard behavior; this click synchronizes controlled open state. */}
            <summary
              onClick={(event) => {
                event.preventDefault();
                setOpenPage(openPage === page.id ? null : page.id);
              }}
            >
              <span>{page.name}</span>
              <small>
                {page.area} | {page.views.join(' | ')}
              </small>
            </summary>
            <div className="catalog-page-body">
              <a className="catalog-page-entry" href={page.href} target="_blank" rel="noopener noreferrer">
                打开实际界面
                <ArrowUpRight aria-hidden="true" />
              </a>
              {page.entryPath && <p className="catalog-muted">进入路径：{page.entryPath}</p>}
              <nav className="catalog-region-index" aria-label={`${page.name}区域`}>
                {page.regions.map((region) => jump(catalogAnchor(page.id, region.id), region.name))}
              </nav>
              {openPage === page.id &&
                (page.id === 'room' ? (
                  <RoomExample
                    announce={announce}
                    renderRegion={(id, children) => {
                      const region = page.regions.find((region) => region.id === id);
                      return region ? renderRegion(page, region, children) : null;
                    }}
                  />
                ) : (
                  page.regions.map((region) =>
                    renderRegion(page, region, <PageExample page={page} region={region} announce={announce} />),
                  )
                ))}
            </div>
          </details>
        ))}
      </div>
      {!pages.length && (
        <div className="catalog-no-results">
          <p>没有匹配的页面或部件</p>
          <Button
            variant="outline"
            onClick={() => {
              setQuery('');
              setArea('全部');
            }}
          >
            清除筛选
          </Button>
        </div>
      )}
    </>
  );
}
