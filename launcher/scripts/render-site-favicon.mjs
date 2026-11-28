import {readFile} from 'node:fs/promises';
import puppeteer from 'puppeteer-core';
import {findChromiumExecutable} from '../lib/chromium-executable.mjs';

const svg = await readFile(new URL('./assets/lily-white-favicon.svg', import.meta.url), 'utf8');
const browser = await puppeteer.launch({headless:true, executablePath:await findChromiumExecutable()});
try {
  const page = await browser.newPage();
  await page.setViewport({width:512, height:512});
  await page.setContent(`<!doctype html><style>body{margin:0}svg{display:block;width:100vw;height:100vh}</style>${svg}`);
  process.stdout.write(await page.screenshot({omitBackground:true}));
} finally {
  await browser.close();
}
