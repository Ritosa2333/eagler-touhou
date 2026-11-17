import puppeteer from 'puppeteer-core';import assert from 'node:assert/strict';
const browser=await puppeteer.launch({executablePath:process.env.CHROME_PATH || 'C:/Program Files/Google/Chrome/Application/chrome.exe',headless:true});
try {
 const page=await browser.newPage();await page.setViewport({width:1000,height:800});
 await page.goto(new URL('lobby.html',process.argv[2] || 'http://127.0.0.1:5173/'),{waitUntil:'networkidle2'});
 const result=await page.evaluate(()=>{const section=document.createElement('section');section.className='mp-quick-chat';section.style.width='360px';section.innerHTML='<div class="mp-quick-chat-log">'+Array.from({length:8},(_,i)=>'<p class="room-chat-entry"><span class="chat-avatar">A</span><span class="room-chat-message"><span class="chat-sender"><strong>玩家</strong><span class="chat-role">P1</span></span><span class="chat-body">消息 '+i+'</span></span></p>').join('')+'</div>';document.body.append(section);const log=section.firstElementChild,rows=[...log.children];return {height:log.clientHeight,scroll:log.scrollHeight,five:rows[5].getBoundingClientRect().top-rows[0].getBoundingClientRect().top,count:rows.length,overflow:getComputedStyle(log).overflowY};});
 assert.ok(Math.abs(result.height-result.five)<2,JSON.stringify(result));assert.ok(result.scroll>result.height);assert.equal(result.count,8);assert.equal(result.overflow,'auto');console.log('PASS: gameplay chat viewport fits five standard entries without deleting messages');
}finally{await browser.close();}
