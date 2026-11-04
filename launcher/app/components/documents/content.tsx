import {createElement, type ReactNode} from 'react';
import {parseFragment, type DefaultTreeAdapterMap} from 'parse5';
import type {TOCItemType} from 'fumadocs-core/toc';
import {contentElements, contentClasses} from '../../../src/launcher/content-fragment.mts';
import {withUiLanguage, type UiLocale} from '../../../src/launcher/i18n.mts';

type Node = DefaultTreeAdapterMap['node'];
const plainText = (node:Node,depth=0):string => depth>64?'':node.nodeName==='#text' ? (node as DefaultTreeAdapterMap['textNode']).value : 'childNodes' in node ? node.childNodes.map(child=>plainText(child,depth+1)).join('') : '';

/** Render the shared authored vocabulary as React nodes, with matching TOC anchors. */
export function prepareHelpDocument(html:string, base:string, locale:UiLocale) {
  const toc:TOCItemType[]=[];
  const origin='https://eagler.invalid';
  const safeHref=(value:string,mail=false)=>{
    let url:URL; try{url=new URL(value,origin+base);}catch{return undefined;}
    if(!['http:','https:',...(mail?['mailto:']:[])].includes(url.protocol))return undefined;
    if(url.origin!==origin)return url.href;
    if(value.startsWith('#'))return value;
    if(/\/compatibility(?:\.(?:en|ja))?\.html$/.test(url.pathname))url.pathname=url.pathname.replace(/compatibility(?:\.(?:en|ja))?\.html$/,`compatibility${locale==='zh-CN'?'':'.'+locale}.html`);
    const path=url.pathname+url.search+url.hash;
    return /\/(?:faq|about|guide|multiplayer|lobby|profile|login)\.html$/.test(url.pathname)?withUiLanguage(path,locale):path;
  };
  const render=(node:Node,key:number,depth=0):ReactNode=>{
    if(depth>64)return null;
    if(node.nodeName==='#text')return (node as DefaultTreeAdapterMap['textNode']).value;
    if(!('tagName' in node)||!contentElements.has(node.tagName)||node.namespaceURI!=='http://www.w3.org/1999/xhtml')return null;
    const tag=node.tagName;
    if(tag==='h1')return null;
    const source=Object.fromEntries(node.attrs.map(attribute=>[attribute.name,attribute.value]));
    const props:Record<string,unknown>={key};
    if(source.title)props.title=source.title;
    const classes=source.class?.split(/\s+/).filter(name=>contentClasses.has(name));
    if(classes?.length)props.className=classes.join(' ');
    if(/^h[2-6]$/.test(tag)){
      const id=`section-${toc.length+1}`;
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
    return createElement(tag,props,...(tag==='img'||tag==='br'||tag==='hr'?[]:node.childNodes.map((child,index)=>render(child,index,depth+1))));
  };
  const body=parseFragment(html).childNodes.map((node,index)=>render(node,index));
  return {body,toc};
}
