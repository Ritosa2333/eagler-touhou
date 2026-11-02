import {useState} from 'react';
import {Search, ArrowUpRight} from 'lucide-react';
import {catalogPages} from './page-catalog';
import {Button} from './button';
import {ChoiceGroup, ChoiceItem} from './choice-group';
import {InputGroup, InputGroupInput} from './input-group';

export function PageInventory() {
  const [query, setQuery] = useState(''), [area, setArea] = useState('全部');
  const pages = catalogPages.filter(page => (area === '全部' || page.area === area) && JSON.stringify(page).toLowerCase().includes(query.trim().toLowerCase()));
  return <><div className="catalog-module-tools"><InputGroup><Search aria-hidden="true"/><InputGroupInput aria-label="查找页面或部件" placeholder="查找房间、隐私、按键…" value={query} onChange={event => setQuery(event.target.value)}/></InputGroup>
    <ChoiceGroup type="single" value={area} onValueChange={value => value && setArea(value)} aria-label="页面分类" className="catalog-choice-group">{['全部', '浏览', '游玩', '联机', '账户', '支持'].map(value => <ChoiceItem key={value} value={value}>{value}</ChoiceItem>)}</ChoiceGroup></div>
    <p className="catalog-result-count" role="status">{pages.length} 个页面与工作区</p><div className="catalog-page-inventory">{pages.map(page => <details key={page.id} id={`page-${page.id}`}>
      <summary><span>{page.name}</span><small>{page.area} | {page.views.join(' | ')}</small></summary><div className="catalog-page-body">
        <a className="catalog-page-entry" href={page.href} target="_blank" rel="noopener noreferrer">打开实际界面<ArrowUpRight aria-hidden="true"/></a>
        {page.entryPath && <p className="catalog-muted">进入路径：{page.entryPath}</p>}
        {page.regions.map(region => <div className="catalog-page-region" key={region.name}><h4>{region.name}</h4><p>{region.parts}</p><p className="catalog-muted">{region.states}</p>
          <a href={region.sample}>{region.sample === '#room-components' ? '可操作生产部件' : region.sample.startsWith('#') ? '通用组合参考' : '打开实际页面'}<ArrowUpRight aria-hidden="true"/></a></div>)}
      </div></details>)}</div>{!pages.length && <div className="catalog-no-results"><p>没有匹配的页面或部件</p><Button variant="outline" onClick={() => {setQuery('');setArea('全部');}}>清除筛选</Button></div>}</>;
}
