#!/usr/bin/env python3
"""Isolate multi-second delivery blackouts from all game/WASM/rollback work.

Two blank browser pages send binary messages to a local echo server at 60 Hz.
The server forwards immediately and emits independent heartbeat/byte counters.
This never changes system proxy settings or the shared production relay.
"""
from __future__ import annotations
import argparse
import json
import socket
import subprocess
import tempfile
import time
from pathlib import Path
from playwright.sync_api import sync_playwright

ROOT = Path(__file__).resolve().parents[1]
SERVER = r"""
const {WebSocketServer} = require('ws');
const {performance} = require('node:perf_hooks');
const http = require('node:http').createServer((req,res)=>{
  res.writeHead(200,{'content-type':'text/html'});res.end('<!doctype html><title>Local socket health fixture</title>');
});
const server = new WebSocketServer({server:http,perMessageDeflate:false});
let n=0,last=performance.now();
const peers=new Map();
server.on('connection',ws=>{
  const state={messages:0,bytes:0,last:performance.now(),maxGap:0};
  peers.set(ws,state);
  ws.on('message',(bytes,binary)=>{
    const now=performance.now();state.maxGap=Math.max(state.maxGap,now-state.last);state.last=now;
    state.messages++;state.bytes+=bytes.length;n++;ws.send(bytes,{binary});
  });
  ws.on('error',e=>console.log(JSON.stringify({error:String(e)})));
});
http.listen(Number(process.argv[1]),'127.0.0.1',()=>console.log('READY'));
setInterval(()=>{const now=performance.now();console.log(JSON.stringify({tickMs:now-last,n,
  peers:[...peers.values()].map(p=>({...p,age:now-p.last}))}));last=now;},1000).unref();
"""

def main() -> int:
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--seconds',type=int,default=45)
    parser.add_argument('--cpu-rate',type=float,default=1.5)
    parser.add_argument('--direct',action='store_true',help='Only this test browser bypasses proxy settings')
    parser.add_argument('--output',type=Path,required=True)
    args=parser.parse_args()
    if not 5<=args.seconds<=120 or not 1<=args.cpu_rate<=8:
        parser.error('invalid bounded duration/CPU settings')
    with socket.socket() as sock:
        sock.bind(('127.0.0.1',0));port=sock.getsockname()[1]
    report={'settings':{**vars(args),'output':str(args.output)},'peers':[]}
    with tempfile.TemporaryFile(mode='w+t') as log:
        relay=subprocess.Popen(['node','-e',SERVER,str(port)],cwd=ROOT.parent/'eagler-touhou',stdout=log,stderr=log)
        browsers=[]
        try:
            end=time.monotonic()+10
            while time.monotonic()<end:
                try:
                    with socket.create_connection(('127.0.0.1',port),timeout=.2):break
                except OSError:time.sleep(.1)
            else:raise RuntimeError('echo server did not start')
            with sync_playwright() as pw:
                try:
                    pages=[]
                    for i in range(2):
                        flags=['--disable-background-timer-throttling','--disable-renderer-backgrounding','--disable-backgrounding-occluded-windows']
                        if args.direct:flags.append('--no-proxy-server')
                        browser=pw.chromium.launch(headless=True,channel='msedge',args=flags);browsers.append(browser)
                        page=browser.new_page();pages.append(page)
                        # Same loopback origin as gameplay, not an opaque blank
                        # origin subject to different local-network permission.
                        page.goto(f'http://127.0.0.1:{port}/',wait_until='load')
                        if i:page.context.new_cdp_session(page).send('Emulation.setCPUThrottlingRate',{'rate':args.cpu_rate})
                        page.evaluate(r"""port => {
                          const p=globalThis.health={sent:0,received:0,maxReceiveGap:0,maxSendGap:0,
                            maxRtt:0,rtts:[],lastSend:0,lastReceive:0,error:null,buffered:0};
                          const ws=new WebSocket('ws://127.0.0.1:'+port);ws.binaryType='arraybuffer';
                          ws.onmessage=e=>{const now=performance.now();const d=new DataView(e.data);
                            const rtt=now-d.getFloat64(8,true);p.rtts.push(rtt);p.maxRtt=Math.max(p.maxRtt,rtt);
                            if(p.lastReceive)p.maxReceiveGap=Math.max(p.maxReceiveGap,now-p.lastReceive);
                            p.lastReceive=now;p.received++;
                          };
                          ws.onerror=()=>p.error='WebSocket error';
                          ws.onopen=()=>{p.timer=setInterval(()=>{
                            const now=performance.now();const a=new ArrayBuffer(96);const d=new DataView(a);
                            d.setUint32(0,p.sent,true);d.setFloat64(8,now,true);ws.send(a);
                            if(p.lastSend)p.maxSendGap=Math.max(p.maxSendGap,now-p.lastSend);
                            p.lastSend=now;p.sent++;p.buffered=Math.max(p.buffered,ws.bufferedAmount);
                          },1000/60);};
                          globalThis.stopHealth=()=>clearInterval(p.timer);
                        }""",port)
                    for page in pages:
                        page.wait_for_function('health.sent > 0 || health.error',timeout=10000)
                        if page.evaluate('health.error'):raise RuntimeError('local echo socket failed before sampling')
                    deadline=time.monotonic()+args.seconds
                    while time.monotonic()<deadline:pages[0].wait_for_timeout(250)
                    for page in pages:page.evaluate('stopHealth()')
                    pages[0].wait_for_timeout(1000)
                    for page in pages:
                        data=page.evaluate('({...health,endTime:performance.now()})');samples=sorted(data.pop('rtts'));data.pop('timer',None)
                        data['outstanding']=data['sent']-data['received']
                        data['receiveAgeMs']=max(0,data['endTime']-data['lastReceive'])
                        data['rttP99']=samples[min(len(samples)-1,int(len(samples)*.99))] if samples else None
                        report['peers'].append(data)
                    report['status']='PASS' if all(p['received'] and not p['error'] and p['outstanding']==0 for p in report['peers']) else 'FAIL'
                finally:
                    for browser in browsers:browser.close()
        except Exception as error:report.update(status='FAIL',error=str(error))
        finally:
            relay.terminate()
            try:relay.wait(timeout=5)
            except subprocess.TimeoutExpired:relay.kill();relay.wait(timeout=5)
            log.seek(0);report['server_log']=log.read()[-100000:]
    args.output.parent.mkdir(parents=True,exist_ok=True)
    args.output.write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8')
    print(json.dumps({k:v for k,v in report.items() if k!='server_log'}),flush=True)
    return 0 if report['status']=='PASS' else 1

if __name__=='__main__':raise SystemExit(main())
