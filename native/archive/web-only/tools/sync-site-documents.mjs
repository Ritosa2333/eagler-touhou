import {copyFile, readFile, writeFile} from 'node:fs/promises';
import {createRequire} from 'node:module';
import {fileURLToPath} from 'node:url';
import {readDocumentHistory} from '../../launcher/scripts/ui-rewrite/document-history.ts';
import {
  COOP_GUIDE_GAMES, VERSUS_GUIDE_GAMES, MULTIPLAYER_GUIDE_PATH,
} from '../../launcher/lib/multiplayer-guide.mjs';

const launcher = new URL('../../launcher/', import.meta.url);
const require = createRequire(new URL('package.json', launcher));
const {marked} = require('marked');
const {parseFragment} = require('parse5');
const assets = new URL('../assets/', import.meta.url);
const guide = MULTIPLAYER_GUIDE_PATH;
const read = path => readFile(new URL(path, launcher), 'utf8');
const revisions = readDocumentHistory(fileURLToPath(launcher));

const attribute = (node, name) => node.attrs?.find(value => value.name === name)?.value;
const children = node => node.childNodes ?? [];
const text = node => node.nodeName === '#text' ? node.value : children(node).map(text).join('');
const markdownText = value => value.replace(/[\\*_\[\]~<>]/g, '\\$&');
const link = (label, destination) => '['+label+'](<'+destination+'>)';
const heading = (label, depth = 2, id = '') =>
  '#'.repeat(depth)+' '+label+(id ? ' {#'+id+'}' : '')+'\n\n';
const slug = value => value.toLowerCase().replace(/[^\p{L}\p{N}\s_-]/gu, '').replace(/\s+/g, '-');

function table(rows) {
  const columns = rows[0].length;
  const line = row => '| '+row.map(cell => cell.trim().replace(/\s*\n\s*/g, ' ')
    .replace(/(?<!\\)\|/g, '\\|')).join(' | ')+' |';
  if (rows.some(row => row.length !== columns)) throw new Error('Inconsistent document table widths');
  return [line(rows[0]), line(Array(columns).fill('---')), ...rows.slice(1).map(line)].join('\n')+'\n\n';
}

function safeAddress(value, language) {
  if (!value?.trim()) return null;
  const parsed = new URL(value, 'https://touhouonline.com/');
  if (!['http:', 'https:', 'mailto:'].includes(parsed.protocol)) return null;
  if (parsed.origin === 'https://touhouonline.com' &&
      parsed.pathname === '/compatibility.html' && language !== 'zh-CN') {
    return '/compatibility.'+language+'.html'+parsed.search+parsed.hash;
  }
  return value;
}

// Input is the repository's authored help vocabulary. Render its text and
// structure, keeping web heading identifiers available to native links.
function markdownFromHtml(nodes, language) {
  let sectionCount = 0;
  const headings = new Set();
  const render = node => {
    if (node.nodeName === '#text') return markdownText(node.value.replace(/\s+/g, ' '));
    if (node.nodeName === '#comment') return '';
    const nested = () => children(node).map(render).join('');
    switch (node.tagName) {
      case 'h1': return '';
      case 'h2': case 'h3': case 'h4': case 'h5': case 'h6': {
        sectionCount++;
        const authored = attribute(node, 'id');
        const id = /^[a-z][a-z0-9-]*$/.test(authored ?? '') &&
          !authored.startsWith('section-') && !headings.has(authored)
          ? authored : 'section-'+sectionCount;
        headings.add(id);
        return heading(text(node).trim(), Number(node.tagName[1]), id);
      }
      case 'div': case 'section': return nested()+'\n';
      case 'p': return nested().trim()+'\n\n';
      case 'a': {
        const address = safeAddress(attribute(node, 'href'), language);
        return address ? link(nested(), address) : nested();
      }
      case 'strong': return '**'+nested()+'**';
      case 'em': return '*'+nested()+'*';
      case 'del': return '~~'+nested()+'~~';
      case 'code': return '\x60'+text(node)+'\x60';
      case 'pre': return '\x60\x60\x60\n'+text(node).trimEnd()+'\n\x60\x60\x60\n\n';
      case 'br': return '  \n';
      case 'hr': return '---\n\n';
      case 'blockquote': return nested().trim().split('\n').map(line => '> '+line).join('\n')+'\n\n';
      case 'ul': case 'ol': {
        const start = Number(attribute(node,'start') ?? 1);
        return children(node).filter(item => item.tagName === 'li').map((item,index) => {
          const prefix = node.tagName === 'ul' ? '- ' : (start+index)+'. ';
          return prefix + children(item).map(render).join('').trim().replace(/\n/g, '\n    ');
        }).join('\n')+'\n\n';
      }
      case 'table': {
        const rows = children(node).flatMap(part => part.tagName === 'tr' ? [part] : children(part))
          .filter(row => row.tagName === 'tr')
          .map(row => children(row).filter(cell => ['th','td'].includes(cell.tagName))
            .map(cell => children(cell).map(render).join('')));
        return table(rows);
      }
      default: throw new Error('Unsupported authored help element: '+node.nodeName);
    }
  };
  return nodes.map(render).join('');
}

function toSections(source) {
  const sections = [{title: '', id: '', depth: 2, blocks: []}];
  let body = '';
  let section = sections[0];
  const flush = () => {
    if (body.trim()) section.blocks.push({text:body.trim(),columns:0});
    body = '';
  };
  for (const token of marked.lexer(source.replace(/\r\n/g, '\n'))) {
    const notice = token.type === 'blockquote'
      ? /^\\\[!(NOTE|WARNING)\\\]\s*/u.exec(token.text)
      : null;
    if (notice) {
      flush();
      section.blocks.push({
        text: token.text.slice(notice[0].length),
        columns: 0,
        notice: notice[1],
      });
    } else if (token.type === 'heading') {
      if (token.depth === 1) continue;
      flush();
      const authored = /\s*\{#([a-z0-9-]+)\}\s*$/u.exec(token.text);
      const title = token.text.replace(/\s*\{#[a-z0-9-]+\}\s*$/u, '');
      const canonical = authored?.[1] ?? slug(title);
      const id = sections.some(item => item.id === canonical) ? canonical+'-'+sections.length : canonical;
      section = {title,id,depth:token.depth,blocks:[]};
      sections.push(section);
    } else if (token.type === 'table') {
      flush();
      section.blocks.push(token.header.length === 2
        ? {columns:2, rows:[token.header,...token.rows].map(row => row.map(cell => cell.text))}
        : {text:token.raw.trim(),columns:token.header.length});
    } else {
      body += token.raw;
    }
  }
  flush();
  return sections.filter(item => item.title || item.blocks.length);
}

function splitRules(html, specific) {
  const sections = new Map();
  let topic;
  for (const node of parseFragment(html).childNodes) {
    if (node.tagName === 'h2') {
      topic = /^th\d+\b/i.exec(text(node).trim())?.[0].toLowerCase() ?? 'coop';
      sections.set(topic,[]);
      continue;
    }
    if (!topic || (node.nodeName === '#text' && !text(node).trim())) continue;
    if (node.tagName === 'h3' || node.tagName === 'h4') {
      if (text(node).trim() === specific) continue;
      node.tagName = node.nodeName = 'h2';
    }
    sections.get(topic).push(node);
  }
  return sections;
}

function gameGuides(t, language) {
  const md = key => t('document_multiplayer_'+key);
  const title = topic => topic === 'coop' ? md('common') : t('guide_'+topic);
  const sections = splitRules(t('content_multiplayer'), t('guide_specific'));
  const rules = [
    ['mode','challenge-mode',t('room_challengeMode'),'challengeSummary'],
    ['resources','life-transfer',md('lifeTerm')+md('lifeTransferTail'),'lifeAction'],
    ['resources','item-drops',md('powerDropsLabel'),'powerDrops'],
    ['resources','power-transfer',md('powerTransferLabel'),'powerTransfer'],
    ['survival','death-and-revival',md('deathLabel'),'deathSummary'],
    ['combat','deathbomb',md('deathbombLabel'),'deathbombSummary'],
    ['combat','boss-damage',md('bossLabel'),'bossSummary'],
    ['play','shared-controls',md('controlsLabel'),'controlsSummary'],
    ['progress','unlocks',md('unlockLabel'),'unlockSummary'],
  ];
  const overview = table([[md('category'),md('mechanism')], ...rules.map(([category,id,name,summary]) =>
    [md(category),link(name,'#'+id)+' '+md(summary)])]);
  const start = ['create','join','ready'].map((key,index) => (index+1)+'. '+md(key)).join('\n')+
    '\n\n'+md('basicFlow')+'\n\n'+heading(md('coop'))+md('coopIntro')+'\n\n'+md('coopGuide')+
    '\n\n'+link(title('coop'),guide+'/coop')+'\n\n'+heading(t('directory_group_versus'))+
    md('versusIntro')+'\n\n'+md('versusPending');
  const body = topic => {
    const nodes = sections.get(topic);
    if (!nodes) throw new Error('Missing help topic '+topic+' ('+language+')');
    return markdownFromHtml(nodes, language).trim();
  };
  return {
    [guide]: [md('title'),start],
    [guide+'/coop']: [title('coop'),overview+md('tableNote')+'\n\n'+body('coop')+
      '\n\n'+heading(md('exceptions'))+COOP_GUIDE_GAMES.filter(id => id !== 'th06').map(id =>
        '- '+link(title(id),guide+'/'+id)+md('exception_'+id)).join('\n')],
    ...Object.fromEntries(COOP_GUIDE_GAMES.map(id => {
      const rules = body(id);
      return [guide+'/'+id,[title(id),rules
        ? '> '+md('differences')+' '+link(title('coop'),guide+'/coop')+'\n\n'+rules
        : t('guide_none')]];
    })),
    ...Object.fromEntries(VERSUS_GUIDE_GAMES.map(id =>
      [guide+'/'+id,[title(id),md('versus')+'\n\n'+md(id)]])),
  };
}

// The published work progress report uses Chinese in all three locales.
const progress = (await read('../docs/TOUHOU_PROGRESS.md'))
  .replace(/<img\b[^>]*alt="([^"]+)"[^>]*>/g, '$1')
  .replace(/<br\s*\/?>/gi, ' ')
  .replace(/!\[([^\]]+)\]\[(?:done|in-progress(?:-detail)?|unassigned)\]/g, '**$1**')
  .replace(/\]\((\.\.\/ports\/[^)]+|adaptation\/README\.md)\)/g, (_, path) =>
    '](https://github.com/Ritosa2333/eagler-touhou/blob/main/'+
      new URL(path,'https://placeholder.invalid/docs/').pathname.slice(1)+')');

for (const locale of ['zh-CN','en','ja']) {
  const messages = JSON.parse(await read('messages/'+locale+'.json'));
  const t = key => {
    if (typeof messages[key] !== 'string') throw new Error('Missing site message '+key+' ('+locale+')');
    return messages[key];
  };
  const html = key => markdownFromHtml(parseFragment(t(key)).childNodes, locale);
  const entries = {
    '/guide.html': [t('firstUseNotice_title'),html('content_firstUse')],
    '/faq.html': [t('nav_faq'),html('content_faq')],
    '/about.html': [t('nav_about'),html('content_about')],
    '/faq.html?view=progress': ['东方作品进度',progress],
    '/terms.html': [t('document_terms'),''],
    '/privacy.html': [t('document_privacy'),''],
    ...gameGuides(t, locale),
  };
  const revisionKeys = {
    '/guide.html':'firstUse', '/faq.html':'faq', '/about.html':'about',
    '/terms.html':'terms', '/privacy.html':'privacy',
  };
  const documents = Object.fromEntries(Object.entries(entries).map(([path,[title,body]]) => {
    const key = path.startsWith(guide) ? 'multiplayer' : revisionKeys[path];
    const revision = key ? revisions[locale]?.[key] : undefined;
    return [path,{title,sections:toSections(body),...(revision && {revision})}];
  }));
  await writeFile(new URL('documents/site.'+locale+'.json',assets),JSON.stringify(documents,null,2)+'\n');
}
await copyFile(new URL('public/assets/donation.webp',launcher),new URL('donation.webp',assets));

