/** Focus the hosted page's keyboard owner and retain the focus evidence around the correction. */
export async function focusRestartKeyboardHost({bringToFront,focusHostBody,readFocus}={}){
  if(typeof bringToFront!=='function'||typeof focusHostBody!=='function'||typeof readFocus!=='function')
    throw new TypeError('Restart keyboard focus needs bringToFront(), focusHostBody() and readFocus()');
  const before=await readFocus();
  await bringToFront();
  await focusHostBody();
  const after=await readFocus();
  const missing=[];
  if(after?.documentHasFocus!==true)missing.push('host document focus');
  if(after?.visibilityState!=='visible'||after?.hidden!==false)missing.push('visible host document');
  if(after?.activeElementTag!=='BODY'||after?.activeElementIsBody!==true)
    missing.push('host body active element');
  if(after?.activeElementIsRuntimeFrame!==false)missing.push('Runtime iframe is not the active host element');
  return{status:missing.length?'incomplete':'ready',before,after,missing};
}

/** Require the exact host-forwarded physical key sequence in this Runtime epoch. */
export function assessRestartKeyboardForwarding({events,epoch,expectedCodes}={}){
  if(!Array.isArray(events)||!Number.isSafeInteger(epoch)||!Array.isArray(expectedCodes)||
    expectedCodes.some(code=>typeof code!=='string'||!code))
    throw new TypeError('Restart keyboard evidence needs captured events, an epoch and expected key codes');
  const current=events.filter(event=>event?.epoch===epoch);
  const expected=expectedCodes.flatMap(code=>[{code,down:true},{code,down:false}]);
  const actual=current.map(({code,down})=>({code,down}));
  if(JSON.stringify(actual)!==JSON.stringify(expected))
    return{status:'failed',reason:'Host-forwarded physical keyboard events do not match the registered title-flow inputs. '+
      `Expected ${JSON.stringify(expected)}; observed ${JSON.stringify(actual)}.`,expected,actual,events:current};
  const rejected=current.filter(event=>event.accepted!==true);
  if(rejected.length)
    return{status:'failed',reason:'A physical keyboard event was not accepted by the Runtime input bridge.',
      expected,actual,events:current,rejected};
  return{status:'passed',expected,actual,events:current};
}
