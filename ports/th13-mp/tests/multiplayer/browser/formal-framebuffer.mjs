// Install in the game iframe; no pumping, drawing or game state writes.
export function installFramebufferCapture() {
  globalThis.__installFramebufferCapture = (module, seatCount) => {
    const state = globalThis.__framebufferCapture = {pendingReasons: [], candidate: null,
      captures: [], attempts: [], failures: [], next: 1, stages: [], settledStages: [], ghosts: null, lasers: false,
      observeCalls: 0, observeMs: 0, lastWorkSequence: null};
    const read = name => { const p=module[name](); return p ? module.UTF8ToString(p) : ''; };
    const observe = ok => {
      const start=performance.now();
      try {
        if (!ok) return;
        const probe=module._th13_probe_state() >>> 2;
        if (module.HEAP32[probe]!==1) return;
        const stage=module.HEAP32[probe+2];
        if (!state.stages.includes(stage)) { state.stages.push(stage); state.pendingReasons.push(`stage:${stage}`); }
        if (module.HEAP32[probe+5] >= 180 && !state.settledStages.includes(stage)) {
          state.settledStages.push(stage); state.pendingReasons.push(`stage-settled:${stage}`);
        }
        const seats=Array.from({length:seatCount},(_,i)=>JSON.parse(module.UTF8ToString(module._th13_debug_mp_seat_state(i))));
        const ghosts=seats.map(row=>!!row.ghost);
        ghosts.forEach((ghost,i)=>{
          if ((state.ghosts && ghost!==state.ghosts[i]) || (!state.ghosts && ghost))
            state.pendingReasons.push(`${ghost?'ghost':'rescue'}:${i}`);
        });
        state.ghosts=ghosts;
        const entities=JSON.parse(read('_th13_debug_replay_entities'));
        const lasers=entities.lasers.some(row=>row.active);
        if (lasers && !state.lasers) state.pendingReasons.push('laser-start');
        state.lasers=lasers;
      } catch(error) { state.failures.push(`observe: ${error}`); }
      finally { ++state.observeCalls; state.observeMs+=performance.now()-start; }
    };
    const old=module.onGameFrame;
    module.onGameFrame = (...args) => {
      old?.(...args);
      if (state.candidate) {
        state.failures.push('Multiple frame callbacks before capture/work association');
        state.candidate.invalidAssociation=true;
        observe(args[0]); return;
      }
      if (!state.pendingReasons.length) { observe(args[0]); return; }
      const started=performance.now();
      try {
        const canvas=document.querySelector('canvas'), gl=canvas?.getContext('webgl2');
        if (!gl) throw Error('Game canvas WebGL2 unavailable');
        const snapshot=()=>({host:read('_th13_debug_mp_host_state'),
          core:read('_th13_debug_replay_core'), entities:read('_th13_debug_replay_entities'),
          net:read('_th13_debug_mp_net_state'), seats:Array.from({length:seatCount},(_,i)=>
            module.UTF8ToString(module._th13_debug_mp_seat_state(i)))});
        const before=snapshot(), width=gl.drawingBufferWidth, height=gl.drawingBufferHeight;
        if (!width || !height) throw Error('Empty drawing buffer');
        const readBinding=gl.getParameter(gl.READ_FRAMEBUFFER_BINDING);
        const drawBinding=gl.getParameter(gl.DRAW_FRAMEBUFFER_BINDING);
        if (drawBinding!==null) {
          // Correction callbacks can leave an offscreen draw target and submit
          // nothing. Qualify this attempt against the corresponding work row
          // before treating it as a presentation/capture error.
          state.candidate={reasons:state.pendingReasons.slice(),started,
            elapsedMs:performance.now()-started,unsubmittedTarget:true,
            readBinding:readBinding===null?'default':'non-default',drawBinding:'non-default',
            error:'Presentation draw framebuffer is not default; capture unqualified',state:before};
          observe(args[0]); return;
        }
        const pack=gl.getParameter(gl.PACK_ALIGNMENT), row=gl.getParameter(gl.PACK_ROW_LENGTH),
          skipRows=gl.getParameter(gl.PACK_SKIP_ROWS), skipPixels=gl.getParameter(gl.PACK_SKIP_PIXELS),
          pbo=gl.getParameter(gl.PIXEL_PACK_BUFFER_BINDING);
        const pixels=new Uint8Array(width*height*4);
        try {
          gl.bindFramebuffer(gl.READ_FRAMEBUFFER,null); gl.bindBuffer(gl.PIXEL_PACK_BUFFER,null);
          gl.pixelStorei(gl.PACK_ALIGNMENT,1); gl.pixelStorei(gl.PACK_ROW_LENGTH,0);
          gl.pixelStorei(gl.PACK_SKIP_ROWS,0); gl.pixelStorei(gl.PACK_SKIP_PIXELS,0);
          gl.readPixels(0,0,width,height,gl.RGBA,gl.UNSIGNED_BYTE,pixels);
        } finally {
          gl.bindFramebuffer(gl.READ_FRAMEBUFFER,readBinding); gl.bindBuffer(gl.PIXEL_PACK_BUFFER,pbo);
          gl.pixelStorei(gl.PACK_ALIGNMENT,pack); gl.pixelStorei(gl.PACK_ROW_LENGTH,row);
          gl.pixelStorei(gl.PACK_SKIP_ROWS,skipRows); gl.pixelStorei(gl.PACK_SKIP_PIXELS,skipPixels);
        }
        if (gl.getParameter(gl.READ_FRAMEBUFFER_BINDING)!==readBinding ||
            gl.getParameter(gl.DRAW_FRAMEBUFFER_BINDING)!==drawBinding ||
            gl.getParameter(gl.PIXEL_PACK_BUFFER_BINDING)!==pbo ||
            gl.getParameter(gl.PACK_ALIGNMENT)!==pack || gl.getParameter(gl.PACK_ROW_LENGTH)!==row ||
            gl.getParameter(gl.PACK_SKIP_ROWS)!==skipRows || gl.getParameter(gl.PACK_SKIP_PIXELS)!==skipPixels)
          throw Error('GL state not restored');
        const after=snapshot();
        if (JSON.stringify(before)!==JSON.stringify(after)) throw Error('Read-only game state changed during capture');
        let binary=''; for (let i=0;i<pixels.length;i+=32768) binary+=String.fromCharCode(...pixels.subarray(i,i+32768));
        state.candidate={reasons:state.pendingReasons.slice(),started,width,height,rgbaBase64:btoa(binary),
          readBinding:readBinding===null?'default':'non-default',drawBinding:drawBinding===null?'default':'non-default',
          target:'default READ_FRAMEBUFFER',state:before,elapsedMs:performance.now()-started};
      } catch(error) { state.failures.push(`capture: ${error}`); }
      observe(args[0]);
    };
    state.onWork = work => {
      const ordered=Number.isSafeInteger(work?.sequence) && work.sequence > 0 &&
        (state.lastWorkSequence===null || work.sequence===state.lastWorkSequence+1);
      if (!ordered) state.failures.push('Capture work sequence missing, duplicate or reordered');
      if (Number.isSafeInteger(work?.sequence)) state.lastWorkSequence=work.sequence;
      if (state.candidate) {
        const candidate=state.candidate;
        const matched=!candidate.invalidAssociation && ordered && Number.isFinite(work.begin) && Number.isFinite(work.end) &&
          work.end>=work.begin && candidate.started>=work.begin &&
          candidate.started+candidate.elapsedMs<=work.end &&
          ((work.presentations===0 && work.presentationAt===null) ||
           (work.presentations===1 && Number.isFinite(work.presentationAt) &&
            work.presentationAt>=work.begin && work.presentationAt<=candidate.started));
        if (!matched) state.failures.push('Capture/work callback association invalid');
        else if (candidate.unsubmittedTarget) {
          state.attempts.push({work,...candidate});
          if (work.presentations!==0 || !work.ok)
            state.failures.push(`capture: ${candidate.error}`);
        } else if (work.ok && work.presentations===1 && Number.isFinite(work.presentationAt)) {
          state.captures.push({sequence:state.next++,work,...state.candidate});
          state.pendingReasons.splice(0,state.candidate.reasons.length);
        } else if (!work.ok || work.presentations!==0) {
          state.failures.push('Capture requires exactly one successful timed presentation');
        }
        state.candidate=null; // no presentation: retain reasons and retry.
      }
    };
    state.snapshot=()=>({captures:state.captures.slice(),candidate:state.candidate,
      attempts:state.attempts.slice(),failures:state.failures.slice(),
      observeCalls:state.observeCalls,observeMs:state.observeMs,pendingReasons:state.pendingReasons.slice()});
    state.ack=(first,last,count)=>{
      if (!count || state.captures[0]?.sequence!==first || state.captures[count-1]?.sequence!==last)
        throw Error('Framebuffer capture changed before persistence acknowledgement');
      state.captures.splice(0,count);
    };
  };
}
