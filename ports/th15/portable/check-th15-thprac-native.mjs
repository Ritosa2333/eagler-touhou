// Read-only mapping proof for the local original executable. No retail data
// is copied to build output. Fail closed when the expected instruction changes.
import {readFileSync} from 'node:fs';
import {createHash} from 'node:crypto';
const exe=readFileSync(process.argv[2]);
const pe=exe.readUInt32LE(0x3c);
if(exe.toString('ascii',pe,pe+4)!=='PE\0\0'||exe.readUInt16LE(pe+4)!==0x14c)throw Error('TH15 PE32 executable required');
const optional=pe+24,base=exe.readUInt32LE(optional+28),table=optional+exe.readUInt16LE(pe+20);
const sections=Array.from({length:exe.readUInt16LE(pe+6)},(_,i)=>{const at=table+i*40;return {rva:exe.readUInt32LE(at+12),size:exe.readUInt32LE(at+16),raw:exe.readUInt32LE(at+20)};});
function bytes(va,n){const rva=va-base,s=sections.find(s=>rva>=s.rva&&rva+n<=s.rva+s.size);if(!s)throw Error('Unmapped instruction '+va.toString(16));return exe.subarray(s.raw+rva-s.rva,s.raw+rva-s.rva+n);}
const sites=[
 [0x42b275,'6a008bcfe8d2500000f30f59c0f30f118774400000','ECL 526 evaluated float squared into bullet minimum distance'],
 [0x42bd23,'8b8f74400000894844','bullet emission consumes enemy minimum-distance gate'],
 [0x41fdf5,'7c4b','bonus decay branch'],
 [0x428b56,'8d8ebc020000e83f00feff','enemy +2bc age timer tick'],
 [0x408c08,'c20400','patched timer target RET 4'],
 [0x43d0b5,'741e83f82b','reward-only skip entry'],
 [0x43d0d5,'8b87b000000085c0780b','checkpoint branch preserved'],
 [0x43dd49,'3943087407c7431000000000894308','chapter comparison precedes override'],
 [0x43dd58,'8d939c000000','chapter override site'],
 [0x43dece,'c20400','Extra chapter bonus after snapshot return'],
 [0x456392,'a150744e0048a350744e00','life hook before decrement'],
 [0x43d5c1,'6a00e818fe0000','entrance BGM slot selection'],
 [0x42b259,'f30f11876c3f0000e941290000','range hook after authored height store'],
 [0x420290,'c7814092010002000000','history coordinate-space immediate'],
 [0x420343,'83f9647c19','MASTER threshold branch'],
 [0x420368,'83f8647c1a','99+ threshold branch'],
 [0x44d3e3,'f605cc794e00107413','BGM helper conditional reset flag'],
 [0x44d3fa,'e8119b0200','BGM helper reset Other caller'],
 [0x44d40e,'e8fd9a0200','BGM Play caller'],
 [0x43cb3b,'e8d0a30300','BGM Stop caller'],
 [0x450ef8,'e813600200','BGM Pause caller'],
 [0x452a5e,'e8ad440200','BGM Resume caller'],
 [0x48b4de,'ff75108b4b0c6a00e8a50d0000','Stars non-intro PCM position caller'],
 [0x48c294,'8bf1','Stars stream seek entry'],
 [0x48c33d,'8b7d0c3b7a1c','Stars PCM byte position argument and loop-end comparison'],
 [0x43404a,'c783c0010000ffffffff','HUD init lock-counter reset'],
 [0x42b938,'8bf0','boss-mode lock-counter reset'],
 [0x42c738,'8944242c','next-pattern lock-counter reset'],
 [0x4301e8,'8981c0010000','Boss countdown lock-counter observation'],
 [0x43d99d,'e8fe380100','all-clear original practice finish call'],
 [0x43daac,'83b8b400000000','all-clear stage-six post-award hook'],
 [0x43dcb5,'83b8b400000000','all-clear non-final stage hook'],
 [0x43dbd3,'83b8b400000000','Extra post-award branch remains unhooked'],
];
for(const [address,expected,name] of sites)if(bytes(address,expected.length/2).toString('hex')!==expected)throw Error('Native mapping drift: '+name);
// Purple replaces the first relative-call byte 3f->a7; prove that this is
// precisely the no-op timer return, rather than a tick with rate zero.
const call=bytes(0x428b5c,5);const patched=Buffer.from(call);patched[1]=0xa7;
if(0x428b61+patched.readInt32LE(1)!==0x408c08)throw Error('Lock-time call target mismatch');
console.log(JSON.stringify({passed:true,kind:'read-only original instruction ownership',sites:sites.length,sha256:createHash('sha256').update(exe).digest('hex')}));
