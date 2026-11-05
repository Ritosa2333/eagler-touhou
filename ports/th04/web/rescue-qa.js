// Manual emulator integration fixture, enabled only via ?qa=rescue.
// It drives the real death handler and native rescue code; it never draws.
export function install(api){
  const area=document.createElement('div');area.className='toolbar';
  document.getElementById('tests').before(area);
  const report=text=>{document.getElementById('tests').textContent=text;};
  async function memory(){
    if(!api.snapshot())throw Error('请先开始游戏并进入关卡');
    const patch=await(await fetch('patch.json',{cache:'no-store'})).json();
    const m=api.getMailbox(),cs=m-patch.mailbox_cs_offset;
    const heap=api.getEmulator().module.HEAPU8,v=new DataView(heap.buffer);
    const ds=cs+(patch.data_segment-patch.code_segment)*16;
    return {heap,v,m,ds,state:cs+patch.rescue_cs_offset};
  }
  function pause(){api.getEmulator().pause();document.getElementById('pause').textContent='继续模拟器';}
  function resume(){api.getEmulator().run();document.getElementById('pause').textContent='暂停模拟器';}
  function button(label,action){
    const b=document.createElement('button');b.textContent=label;area.append(b);
    b.onclick=async()=>{b.disabled=true;api.setTesting(true);try{await action();}
      catch(e){report(`测试失败：${e.message}`);api.nativeKey('ShiftLeft',false);}
      finally{b.disabled=false;api.setTesting(false);api.releaseTest();}};
  }
  button('测试：导出内存现场',async()=>{
    pause();const {heap,v,m,ds}=await memory();
    const base=ds-v.getUint16(m+20,true)*16;
    const saved=await fetch('/__qa_memory',{method:'POST',body:heap.slice(base,base+0x100000)});
    if(!saved.ok)throw Error('内存导出需要 tools/serve_qa.py 专用本地测试服务器');
    report(`内存已导出：DS=${v.getUint16(m+20,true).toString(16)}，vsync=${v.getUint16(ds+0x2ab4,true)}，heap=${[0x77e,0x2abc,0x2abe,0x2ac0,0x2ac2].map(at=>v.getUint16(ds+at,true).toString(16)).join('/')}，sprites=${v.getUint16(ds+0x7a8,true)}。`);
  });
  button('测试：生成 P2 幽灵',async()=>{
    const {heap,v,m,ds}=await memory();
    api.nativeKey('ShiftLeft',false);
    // Protect P1 and put P2 on the final life, then let the original death
    // handler consume it. The native code chooses the ghost anchor.
    const resident=ds+(v.getUint16(ds+0xba88,true)*16+v.getUint16(ds+0xba86,true)-v.getUint16(m+20,true)*16);
    heap[m+609+4]=6;heap[resident+11]=6;
    v.setUint16(ds+0x4662,255,true);
    heap[m+609+16+4]=1;v.setUint16(m+66,0,true);
    heap[m+71]=1;heap[m+72]=0;
    resume();
    await api.inputForFrames(0,85);
    pause();
    if(!api.snapshot().players[1].out)throw Error('P2 尚未进入幽灵状态');
    report('通过：原版最后一命受击后，P2 在初始位置变成原生幽灵。模拟器已暂停，可保存画面。');
  });
  button('测试：借命至 50%',async()=>{
    const {v,ds,state}=await memory();
    if(!api.snapshot().players[1].out)throw Error('请先生成 P2 幽灵');
    v.setUint16(ds+0x464e,240*16,true);v.setUint16(ds+0x4652,240*16,true);
    v.setUint16(ds+0x4650,320*16,true);v.setUint16(ds+0x4654,320*16,true);
    v.setUint16(ds+0x4662,255,true);
    api.nativeKey('ShiftLeft',true);resume();
    await api.inputForFrames(0,45);pause();
    const n=api.getEmulator().module.HEAPU8[state];
    if(n<40||n>=90)throw Error(`读条异常：${n}/90`);
    report(`通过：原生进度 ${Math.floor(n*100/90)}%，尚未扣命。已暂停，可保存画面。`);
  });
  button('测试：完成借命',async()=>{
    const before=api.snapshot();resume();await api.inputForFrames(0,60);pause();
    const after=api.snapshot();api.nativeKey('ShiftLeft',false);
    if(after.players[1].out||after.players[0].lives!==before.players[0].lives-1)throw Error('扣命或复活结果异常');
    report('通过：P1 只扣一条备用命，P2 复活；原生进度达到 100%。');
  });
  button('测试：进入下一关',async()=>{
    const {heap,v,m,ds}=await memory(),before=api.snapshot();
    const resident=ds+v.getUint16(ds+0xba88,true)*16+v.getUint16(ds+0xba86,true)-v.getUint16(m+20,true)*16;
    const stage=heap[ds+0x5394];
    if(stage>=5)throw Error('此按钮只验证普通关卡间切换');
    heap[resident+17]=stage+1;heap[resident+19]=48+stage+1;
    heap[ds+0x5392]=2;resume();
    report(`正在执行原版 ${stage+1} → ${stage+2} 面清理和加载…`);
    const deadline=performance.now()+15000;
    while(performance.now()<deadline){
      const s=api.snapshot();
      if(s?.stage>before.stage&&s.ticks>before.ticks+5){
        pause();report(`通过：进入第 ${heap[ds+0x5394]+1} 面，关卡代数 ${s.stage}，游戏帧 ${s.ticks}。`);return;
      }
      await new Promise(resolve=>setTimeout(resolve,50));
    }
    pause();report(`切换停滞：关卡=${heap[ds+0x5394]+1}，代数=${api.snapshot()?.stage}，quit=${heap[ds+0x5392]}，已暂停保留现场。`);
  });
}
