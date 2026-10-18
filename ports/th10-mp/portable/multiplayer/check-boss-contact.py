"""Diagnostic native Boss-contact and post-combat Ending input regression."""
import argparse
import hashlib
import json
from pathlib import Path
from playwright.sync_api import sync_playwright
from rollback_testkit import Fixture, barrier

parser = argparse.ArgumentParser()
parser.add_argument('--url', default='http://127.0.0.1:8141/')
parser.add_argument('--case', choices=['contact', 'ending'], default='contact')
parser.add_argument('--output', required=True)
parser.add_argument('--seat', type=int, choices=[0, 1], default=0)
parser.add_argument('--rollback', action='store_true')
parser.add_argument('--final-death', action='store_true')
parser.add_argument('--difficulty', type=int, choices=range(4), default=1)
args = parser.parse_args()
report = {'passed': False, 'case': args.case, 'contacts': [], 'settings': vars(args), 'scope': 'diagnostic retail WASM, native contact after position setup'}
report['sources'] = {str(path): hashlib.sha256(path.read_bytes()).hexdigest() for path in [Path(__file__), Path(__file__).with_name('smoke.mjs')]}
try:
    with sync_playwright() as p:
        browser = p.chromium.launch(headless=True, args=['--enable-unsafe-swiftshader'])
        fixture = Fixture(browser, args.url)
        page = fixture.open(local=0, difficulty=args.difficulty)
        helper = fixture.open(local=1, difficulty=args.difficulty)
        barrier([page, helper])
        errors = []
        page.on('pageerror', lambda e: errors.append(str(e)))
        report['console'] = []
        page.on('console', lambda m: report['console'].append(m.text) if m.type == 'error' else None)
        report['identity'] = page.evaluate('multiplayerSmoke.identity()')
        fixture.advance(page, 59)
        state = page.evaluate('multiplayerSmoke.status()')
        assert state[3] and state[7] >= 40, state
        if args.case == 'ending':
            page.evaluate('multiplayerSmoke.ending(true)')
            page.evaluate('multiplayerSmoke.netTicks(80)')
            report['before'] = page.evaluate('multiplayerSmoke.ending()')
            page.evaluate('multiplayerSmoke.key("KeyZ",true)')
            page.evaluate('multiplayerSmoke.netTicks(1)')
            report['z'] = page.evaluate('multiplayerSmoke.ending()')
            assert report['z'][2] & 1 and report['z'][3] & 1 and report['z'][6] == 0, report
            page.evaluate('multiplayerSmoke.key("KeyZ",false)')
            page.evaluate('multiplayerSmoke.key("ControlLeft",true)')
            page.evaluate('multiplayerSmoke.netTicks(8)')
            report['ctrl'] = page.evaluate('multiplayerSmoke.ending()')
            assert report['ctrl'][2] & 256, report
            assert report['ctrl'][5] > report['z'][5], report
        else:
            page.evaluate('(x)=>{window.contactSeat=x[0];window.contactRollback=x[1];window.contactFinal=x[2];}', [args.seat, args.rollback, args.final_death])
            page.evaluate('''async()=>{
                const {allKeyboardInputs}=await import('/replay-robot.mjs');
                window.contactCount=0;window.lastContact=-1000;
                window.animationPeak=[0,0,0];
                const nativeTick=multiplayerSmoke.netTicks;
                multiplayerSmoke.netTicks=function(...args){
                    const result=nativeTick.apply(this,args),counts=this.animationCounts();
                    for(let i=0;i<3;++i)animationPeak[i]=Math.max(animationPeak[i],counts[i]);
                    return result;
                };
                window.contactBatch=()=>{
                    const contacts=[];
                    for(let n=0;n<100;++n){
                        const controls=multiplayerSmoke.replayControls();
                        const state=multiplayerSmoke.status(),hit=controls[5]&&!controls[1]&&state[7]>lastContact+300&&multiplayerSmoke.contactBoss(contactSeat,contactFinal);
                        if(hit){
                            lastContact=state[7];
                            ++contactCount;
                            contacts.push({before:multiplayerSmoke.status(),controls,spell:hit===2});
                            const frame=multiplayerSmoke.netStatus()[2];
                            multiplayerSmoke.captureLocal(frame,0);
                            if(!contactRollback)multiplayerSmoke.submitRemote(1,frame,0);
                            multiplayerSmoke.netTicks(1);
                            contacts.at(-1).after=multiplayerSmoke.status();
                            if(contacts.at(-1).after[12+contactSeat*12]!==4)throw Error('Boss fixture did not cause a native hit: '+JSON.stringify(contacts.at(-1)));
                            if(contactRollback){
                                for(let k=1;k<8;++k){multiplayerSmoke.captureLocal(frame+k,0);multiplayerSmoke.netTicks(1);}
                                for(let k=0;k<8;++k)multiplayerSmoke.submitRemote(1,frame+k,0);
                                contacts.at(-1).pending=multiplayerSmoke.netStatus();
                                multiplayerSmoke.captureLocal(frame+8,0);multiplayerSmoke.submitRemote(1,frame+8,0);
                                multiplayerSmoke.netTicks(1);
                                contacts.at(-1).corrected=multiplayerSmoke.status();
                            }
                            // Keep Bomb released through the full native death
                            // window, even when the keyboard robot could save it.
                            for(let k=0;k<(contactFinal?48:12);++k){
                                const next=multiplayerSmoke.netStatus()[2];
                                multiplayerSmoke.captureLocal(next,0);multiplayerSmoke.submitRemote(1,next,0);
                                multiplayerSmoke.netTicks(1);
                            }
                            contacts.at(-1).death=multiplayerSmoke.status();
                            if(contacts.at(-1).death[10+contactSeat*12]!== (contactFinal?-1:7))throw Error('Native death did not consume one life');
                            if(contactFinal&&contacts.at(-1).death[12+contactSeat*12]!==3)throw Error('Final death did not enter Spirit');
                        }else{
                            const inputs=allKeyboardInputs(multiplayerSmoke.replayProbe(),controls);
                            const frame=multiplayerSmoke.netStatus()[2];
                            multiplayerSmoke.captureLocal(frame,inputs[0]);multiplayerSmoke.submitRemote(1,frame,inputs[1]);
                            multiplayerSmoke.netTicks(1);
                        }
                    }
                    return {contacts,count:contactCount,state:multiplayerSmoke.status(),animationPeak};
                };
            }''')
            for batch in range(140):
                result = page.evaluate('contactBatch()')
                report['contacts'].extend(result['contacts'])
                report['last'] = result
                if batch % 10 == 0:
                    print('contact progress', batch, result['state'][7], result['count'], flush=True)
                if result['state'][6] >= 2:
                    break
            assert len(report['contacts']) >= 2, report
            assert any(c['spell'] for c in report['contacts']), 'An active spell must be covered'
        assert not errors, errors
        report['passed'] = True
        browser.close()
except BaseException as e:
    report['error'] = str(e)
    raise
finally:
    Path(args.output).parent.mkdir(parents=True, exist_ok=True)
    Path(args.output).write_text(json.dumps(report, indent=2), encoding='utf-8')
    print(json.dumps({'passed': report['passed'], 'case': args.case, 'contacts': len(report['contacts'])}))
