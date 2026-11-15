import {createElement, type ReactNode} from 'react';
import {parseFragment, type DefaultTreeAdapterMap} from 'parse5';
import type {TOCItemType} from 'fumadocs-core/toc';
import {Callout} from '@fumadocs/base-ui/components/callout';
import {contentElements, contentClasses} from '../../../src/launcher/content-fragment.mts';
import {withUiLanguage, type UiLocale} from '../../../src/launcher/i18n.mts';

type Node = DefaultTreeAdapterMap['node'];
const tableContainers = new Set(['table', 'thead', 'tbody', 'tr']);
const plainText = (node:Node,depth=0):string => depth>64?'':node.nodeName==='#text' ? (node as DefaultTreeAdapterMap['textNode']).value : 'childNodes' in node ? node.childNodes.map(child=>plainText(child,depth+1)).join('') : '';

/** Render the shared authored vocabulary as React nodes. Full pages also collect TOC anchors. */
export function renderHelpContent(html:string, base:URL, locale:UiLocale, toc?:TOCItemType[]) {
  return renderHelpNodes(parseFragment(html).childNodes, base, locale, toc);
}

function renderHelpNodes(nodes: Node[], base:URL, locale:UiLocale, toc?:TOCItemType[]) {
  const safeHref=(value:string,mail=false)=>{
    if(!value.trim())return undefined;
    let url:URL; try{url=new URL(value,base);}catch{return undefined;}
    if(!['http:','https:',...(mail?['mailto:']:[])].includes(url.protocol))return undefined;
    if(url.origin!==base.origin)return url.href;
    if(value.startsWith('#'))return toc?value:url.href;
    if(!toc&&url.pathname===base.pathname&&url.search===base.search&&url.hash)return url.href;
    if(/\/compatibility(?:\.(?:en|ja))?\.html$/.test(url.pathname))url.pathname=url.pathname.replace(/compatibility(?:\.(?:en|ja))?\.html$/,`compatibility${locale==='zh-CN'?'':'.'+locale}.html`);
    const path=toc?url.pathname+url.search+url.hash:url.href;
    return /\/(?:faq|about|guide|multiplayer|lobby|profile|login)\.html$/.test(url.pathname)?withUiLanguage(path,locale):path;
  };
  const render=(node:Node,key:number,depth=0):ReactNode=>{
    if(depth>64)return null;
    if(node.nodeName==='#text')return (node as DefaultTreeAdapterMap['textNode']).value;
    if(!('tagName' in node)||!contentElements.has(node.tagName)||node.namespaceURI!=='http://www.w3.org/1999/xhtml')return null;
    const tag=node.tagName;
    if(tag==='h1'&&toc)return null;
    const source=Object.fromEntries(node.attrs.map(attribute=>[attribute.name,attribute.value]));
    const props:Record<string,unknown>={key};
    if(source.title)props.title=source.title;
    const classes=source.class?.split(/\s+/).filter(name=>contentClasses.has(name));
    if(classes?.length)props.className=classes.join(' ');
    if(toc&&/^h[2-6]$/.test(tag)){
      const authoredId = /^[a-z][a-z0-9-]*$/.test(source.id || '') && !source.id.startsWith('section-') ? source.id : '';
      const id = authoredId && !toc.some(item => item.url === '#' + authoredId) ? authoredId : `section-${toc.length+1}`;
      props.id=id; toc.push({title:plainText(node),url:'#'+id,depth:Number(tag[1])});
    }
    if(tag==='a'){
      props.href=source.href?safeHref(source.href,true):undefined;
      if(source.target==='_blank'){props.target='_blank';props.rel='noopener noreferrer';}
    }
    if(tag==='img'){
      props.src=source.src?safeHref(source.src):undefined;
      if(!props.src)return null;
      props.alt=source.alt||'';props.loading='lazy';
    }
    if(tag==='ol'&&/^-?\d{1,9}$/.test(source.start||''))props.start=Number(source.start);
    if((tag==='th'||tag==='td')&&['left','center','right'].includes(source.align))props.align=source.align;
    if(tag==='img'||tag==='br'||tag==='hr')return createElement(tag,props);
    if(tag==='blockquote'){
      const paragraph = node.childNodes.find(child => 'tagName' in child);
      const marker = paragraph?.tagName === 'p' ? paragraph.childNodes[0] : undefined;
      if(marker?.nodeName==='#text'){
        const text = marker as DefaultTreeAdapterMap['textNode'];
        const alert = /^\[!(NOTE|WARNING)\](?:\r?\n|$)/.exec(text.value);
        if(alert){
          text.value = text.value.slice(alert[0].length);
          const type = alert[1] === 'WARNING' ? 'warning' : 'info';
          return createElement(Callout,{key,type,role:'note'},...node.childNodes.map((child,index)=>render(child,index,depth+1)));
        }
      }
    }
    const children = tableContainers.has(tag)
      ? node.childNodes.filter(child => child.nodeName !== '#text')
      : node.childNodes;
    if(tag==='table')return createElement('div',{key,className:'documentation-table-scroll'},createElement(tag,{},...children.map((child,index)=>render(child,index,depth+1))));
    return createElement(tag,props,...children.map((child,index)=>render(child,index,depth+1)));
  };
  return nodes.map((node,index)=>render(node,index));
}

export function prepareHelpDocument(html:string, base:string, locale:UiLocale) {
  const toc:TOCItemType[]=[];
  const body=renderHelpContent(html,new URL(base,'https://eagler.invalid'),locale,toc);
  return {body,toc};
}

/** The shared translation already groups multiplayer rules by top-level heading. */
export function prepareMultiplayerRules(html: string, base: string, locale: UiLocale, topic: string, specificLabel: string) {
  const nodes: Node[] = [], toc: TOCItemType[] = [];
  let group = '';
  for (const node of parseFragment(html).childNodes) {
    if ('tagName' in node && node.tagName === 'h2') {
      group = /^th\d+\b/i.exec(plainText(node).trim())?.[0].toLowerCase() ?? 'coop';
      continue;
    }
    if (group !== topic || (node.nodeName === '#text' && !plainText(node).trim())) continue;
    if ('tagName' in node && (node.tagName === 'h3' || node.tagName === 'h4')) {
      if (plainText(node).trim() === specificLabel) continue;
      node.tagName = node.nodeName = 'h2';
    }
    nodes.push(node);
  }
  const body = renderHelpNodes(nodes, new URL(base,'https://eagler.invalid'), locale, toc).filter(node => node != null);
  return {body,toc};
}
