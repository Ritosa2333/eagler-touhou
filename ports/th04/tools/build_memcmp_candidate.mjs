// Pure byte-equivalent memcmp: use unaligned eight-byte reads only when n>=8.
// Retain both exports for direct differential tests, including boundary reads.
import fs from 'node:fs';import b from '../build/binaryen-132/package/index.js';
const input=process.argv[2],output=process.argv[3];if(!input||!output)throw Error('input output required');
const m=b.readBinary(fs.readFileSync(input));m.setFeatures(b.Features.All);
const f=m.getFunction('memcmp');if(!f)throw Error('Named memcmp absent');
const info=b.getFunctionInfo(f);if(b.expandType(info.params).join(',')!==[b.i32,b.i32,b.i32].join(',')||info.results!==b.i32)throw Error('memcmp type changed');
m.addFunction('thReferenceMemcmp',info.params,info.results,info.vars,m.copyExpression(info.body));
m.addFunctionExport('thReferenceMemcmp','thReferenceMemcmp');
const c=x=>m.i32.const(x),g=x=>m.local.get(x,b.i32),set=(x,v)=>m.local.set(x,v);
const load=x=>m.i32.load8_u(0,1,g(x),'0');
const advance=n=>[set(0,m.i32.add(g(0),c(n))),set(1,m.i32.add(g(1),c(n))),set(2,m.i32.sub(g(2),c(n)))];
const body=m.block(null,[
 m.block('words_done',[m.loop('words',m.block(null,[
  m.br_if('words_done',m.i32.lt_u(g(2),c(8))),
  m.br_if('words_done',m.i64.ne(m.i64.load(0,1,g(0),'0'),m.i64.load(0,1,g(1),'0'))),
  ...advance(8),m.br('words')
 ]))]),
 m.block('bytes_done',[m.loop('bytes',m.block(null,[
  m.br_if('bytes_done',m.i32.eqz(g(2))),
  m.if(m.i32.ne(load(0),load(1)),m.return(m.i32.sub(load(0),load(1)))),
  ...advance(1),m.br('bytes')
 ]))]),c(0)
],b.i32);
b.Function.setBody(f,body);m.addFunctionExport('memcmp','thFastMemcmp');
if(!m.validate())throw Error('Invalid memcmp candidate');
fs.writeFileSync(output,m.emitBinary());m.dispose();console.log('Built byte-equivalent word memcmp',output);
