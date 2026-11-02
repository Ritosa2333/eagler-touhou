/** Read-only browser checks shared by the screenshot sweep and manual audits. */
export function inspectUiAppearance() {
  const errors = [], metrics = {}, root = document.documentElement;
  const style = element => getComputedStyle(element);
  const visible = element => element.getBoundingClientRect().height > 0 && !element.closest('[hidden],[aria-hidden="true"]');
  const rgb = value => {
    if (value.startsWith('#')) return value.slice(1).match(/../g).slice(0,3).map(n=>+('0x'+n));
    const numbers=(value.match(/[\d.]+/g) || []).slice(0,3).map(n=>+n);
    return value.startsWith('color(srgb ')?numbers.map(n=>n*255):numbers;
  };
  const luminance = value => rgb(value).map(n=>n/255).map(n=>n<=.04045?n/12.92:((n+.055)/1.055)**2.4).reduce((sum,n,i)=>sum+n*[.2126,.7152,.0722][i],0);
  const surfaceColor = element => {
    const surfaces=[];
    for(let parent=element;parent;parent=parent.parentElement)surfaces.push(style(parent).backgroundColor);
    let result=light?[255,255,255]:[23,23,23];
    for(const color of surfaces.reverse()){
      const numbers=(color.match(/[\d.]+/g)||[]).map(Number),alpha=color==='transparent'?0:numbers[3]??1;
      const channels=rgb(color);
      if(channels.length===3)result=result.map((channel,index)=>channels[index]*alpha+channel*(1-alpha));
    }
    return `rgb(${result.join(',')})`;
  };
  const contrast = (a,b) => {const x=luminance(a),y=luminance(b);return (Math.max(x,y)+.05)/(Math.min(x,y)+.05);};
  const light = root.dataset.bookTheme === 'light';
  if (root.hasAttribute('data-darkreader-mode') || matchMedia('(forced-colors: active)').matches) errors.push('browser-colors-modified');
  if (!style(root).colorScheme.includes(light?'light':'dark')) errors.push('document-color-scheme');
  if (root.scrollWidth > innerWidth) errors.push('horizontal-overflow');
  const masthead=document.querySelector('.book-masthead');
  if(root.hasAttribute('data-ui-app')&&masthead&&visible(masthead)){
    const box=masthead.getBoundingClientRect(),page=root.getBoundingClientRect();
    if(Math.abs(box.left-page.left)>.5||Math.abs(box.right-page.right)>.5)errors.push('masthead-does-not-fill-page');
  }
  const input = document.querySelector('.book-search input');
  if (input && visible(input) && +style(input).borderTopWidth.replace('px','') > 0) errors.push('nested-search-border');
  if (input && visible(input) && input.getBoundingClientRect().height > (input.dataset.slot==='input-group-control'?44:28)) errors.push('nested-search-height');
  for (const control of document.querySelectorAll('[data-slot="input-group"] input')) {
    if (!visible(control)) continue;
    if (+style(control).borderTopWidth.replace('px','') > 0) errors.push('nested-input-border');
    if (document.activeElement === control && style(control).outlineStyle !== 'none') errors.push('nested-input-focus-outline');
  }
  const category = document.querySelector('.book-sidebar nav button');
  if (category && visible(category) && innerWidth > 1050 && style(category).display !== 'grid') errors.push('category-layout');
  for(const selected of document.querySelectorAll('.book-sidebar button:is([aria-pressed="true"],[aria-checked="true"]),.book-view-switch button:is([aria-pressed="true"],[aria-checked="true"])')){
    if(visible(selected) && ['transparent','rgba(0, 0, 0, 0)'].includes(style(selected).backgroundColor))errors.push('selected-state-missing');
  }
  for(const tab of document.querySelectorAll('[role="tab"][data-state="active"]')){
    if(!visible(tab))continue;
    const other=[...tab.closest('[role="tablist"]').querySelectorAll('[role="tab"]')].find(node=>node!==tab&&visible(node));
    if(other&&['backgroundColor','color','borderBottomColor','boxShadow','fontWeight'].every(property=>style(tab)[property]===style(other)[property]))errors.push('tab-selected-state-missing');
  }
  for(const group of document.querySelectorAll('.input-settings-entries,.touch-sensitivity-presets,[data-slot="button-group"]')){
    const buttons=[...group.querySelectorAll('button')].filter(visible);
    if(buttons.length<2)continue;
    const heights=buttons.map(button=>button.getBoundingClientRect().height);
    if(Math.max(...heights)-Math.min(...heights)>.75)errors.push('parallel-control-height');
  }
  for(const control of document.querySelectorAll('.ui-kit-button,.ui-select-trigger,.ui-tabs-trigger')){
    if(visible(control)&&control.textContent.trim()&&+style(control).fontSize.replace('px','')<11)errors.push('control-text-size:'+control.textContent.trim().slice(0,30));
  }
  const avatar = document.querySelector('.profile-avatar svg');
  if (avatar && visible(avatar)) {metrics.avatarWidth=avatar.getBoundingClientRect().width;if(metrics.avatarWidth<32) errors.push('avatar-glyph-size');}
  for (const trigger of document.querySelectorAll('.ui-select-trigger,.ui-kit-select-trigger')) {
    if (!visible(trigger) || trigger.closest('[data-ui-surface="game"]')) continue;
    if (light && luminance(surfaceColor(trigger)) < .5) errors.push('light-select-dark-surface');
  }
  for (const selector of ['dialog[open] #decisionMessage','.replay-workspace-idle p','.replay-workspace-idle small']) {
    const text=document.querySelector(selector);if(!text || !visible(text)) continue;
    const surface=text.closest('dialog,.replay-workspace-idle');
    const ratio=contrast(style(text).color,style(surface).backgroundColor);
    metrics[selector]=ratio;if(ratio<4.5) errors.push('text-contrast:'+selector);
  }
  const fieldText=[
    '.ui-field-label','.ui-field-description','.ui-tabs-trigger','.touch-layout-setting-row>label',
    '.touch-layout-editor-copy strong','.touch-layout-editor-copy em','.touch-sensitivity-custom>span',
    '.input-binding-table th','.input-axis-threshold>span','.replay-file-summary>span','.replay-current p',
    '.mp-room-setting-title','.lobby-field>span','#mpRoomTitle','.mp-panel-head h3','.room-settings-group h3',
    '.room-setting-description','.mp-network-metric strong','.mp-network-result span','.mp-network-result output',
    '.mp-network-group h3','.game-data-import-window p','.game-data-link-row','.apple-refresh-faq-item p',
    '.donation-scan-hint','.transfer-line span','.transfer-meta span','.startup-error-detail',
    '.guide-info-lead','.guide-info-note','.orientation-steps','.guide-key-row','.room-seat-status','.room-seat-network-label','.room-seat-network-value',
    '.spectator-duration-slot','.spectator-playback-meta','.spectator-round-mark',
  ].join(',');
  const fieldRatios=[...document.querySelectorAll(fieldText)].filter(visible).filter(text=>!text.closest('[data-disabled],:disabled')).map(text=>({
    name:text.textContent.trim().slice(0,40),ratio:contrast(style(text).color,surfaceColor(text)),
  }));
  metrics.fieldMinimumContrast=fieldRatios.length?Math.min(...fieldRatios.map(item=>item.ratio)):null;
  for(const item of fieldRatios)if(item.ratio<4.5)errors.push('field-text-contrast:'+item.name);
  for(const check of document.querySelectorAll('.ui-checkbox[data-state="checked"]')){
    if(visible(check)&&contrast(style(check).color,style(check).backgroundColor)<3)errors.push('checkbox-mark-contrast');
  }
  for(const menu of document.querySelectorAll('.ui-menu,.ui-select-content')){
    if(!visible(menu))continue;
    const box=menu.getBoundingClientRect();
    if(box.top<-.5||box.bottom>innerHeight+.5||box.left<-.5||box.right>innerWidth+.5)errors.push('menu-outside-viewport');
    const hit=document.elementFromPoint(Math.max(0,Math.min(innerWidth-1,box.left+box.width/2)),Math.max(0,Math.min(innerHeight-1,box.top+Math.min(box.height/2,30))));
    if(hit&&!menu.contains(hit))errors.push('menu-occluded');
  }
  const focused=document.activeElement;
  if(focused?.matches('.ui-tabs-trigger:focus-visible,.ui-checkbox:focus-visible,.ui-switch:focus-visible,.ui-select-trigger:focus-visible')){
    const actual=rgb(style(focused).outlineColor),expected=rgb(style(focused).getPropertyValue('--kit-ring').trim());
    if(actual.some((channel,index)=>Math.abs(channel-expected[index])>1))errors.push('focus-token');
  }
  const workbench=document.getElementById('touchLayoutEditor');
  if(workbench&&visible(workbench)){
    const box=workbench.getBoundingClientRect(),footer=workbench.querySelector('.touch-workbench-footer')?.getBoundingClientRect();
    metrics.workbench={width:box.width,height:box.height,bottom:box.bottom};
    if(box.left<-.5||box.right>innerWidth+.5||box.top<-.5||box.bottom>innerHeight+.5)errors.push('touch-workbench-outside-viewport');
    if(footer&&(footer.bottom>box.bottom+.5||footer.top<box.top))errors.push('touch-workbench-footer-clipped');
  }
  if (root.hasAttribute('data-ui-home')) {
    const textSelectors = '.book-identity strong,.book-identity small,.book-identity>span,.book-recent-empty,.book-current>span,.ui-kit-select-content [role="option"]';
    const ratios = [...document.querySelectorAll(textSelectors)].filter(visible).map(text=>contrast(style(text).color,surfaceColor(text)));
    metrics.homeMinimumContrast = Math.min(...ratios);
    if (metrics.homeMinimumContrast < 4.5) errors.push('home-text-contrast');
    if (innerWidth <= 780 || matchMedia('(pointer: coarse)').matches) {
      const touch = [...document.querySelectorAll('[data-book-category],[data-book-view],[data-book-favorite],.book-sort,.book-theme-toggle,.masthead-menu-toggle,.book-profile-trigger')].filter(visible);
      metrics.homeMinimumTouchTarget = Math.min(...touch.map(element=>Math.min(element.getBoundingClientRect().width,element.getBoundingClientRect().height)));
      if(metrics.homeMinimumTouchTarget<43.5)errors.push('home-touch-target');
    }
    const panel = document.querySelector('.library-layout>.tools[aria-hidden="true"]');
    if(panel&&!panel.hasAttribute('inert'))errors.push('closed-settings-focus');
  }
  return {theme:root.dataset.bookTheme,width:innerWidth,height:innerHeight,errors,metrics};
}
