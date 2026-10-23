import assert from 'node:assert/strict';

// Fixed inputs for a controlled render. Expected PCM is computed outside the browser.
export const GAIN_CALIBRATION_CASES=Object.freeze([
  {id:'balance',left:.4,right:.1,samples:[[.25,-.5],[.5,-.25]]},
  {id:'float-argument',left:.1039920172913118,right:.09440609366359665,
    samples:[[.7674566507339478,-.046875],[-.049142589,.125]]},
]);

/** Schedule before rendering so the effective sample frame is independently known. */
export async function capturePresentationGainCalibration(page){
  return page.evaluate(async cases=>{
    const sampleRate=48000,startFrame=256,rows=[];
    for(const item of cases){
      const context=new OfflineAudioContext(2,512,sampleRate),source=context.createBufferSource();
      const buffer=context.createBuffer(2,item.samples.length,sampleRate);
      for(let channel=0;channel<2;channel++)buffer.getChannelData(channel).set(item.samples.map(pair=>pair[channel]));
      source.buffer=buffer;const split=context.createChannelSplitter(2),merge=context.createChannelMerger(2);
      source.connect(split);
      for(const [channel,value] of [[0,item.left],[1,item.right]]){
        const gain=context.createGain();split.connect(gain,channel);gain.connect(merge,0,channel);
        for(const requested of [.8,value,value,value])gain.gain.setValueAtTime(requested,startFrame/sampleRate);
      }
      merge.connect(context.destination);source.start(startFrame/sampleRate);
      const rendered=await context.startRendering();
      rows.push({id:item.id,channels:[0,1].map(channel=>Array.from(rendered.getChannelData(channel)))});
    }
    return {schema:'eagler-presentation/gain-calibration/1',method:'future-frame-offline-render',sampleRate,startFrame,frames:512,rows};
  },GAIN_CALIBRATION_CASES);
}

export function assertPresentationGainCalibration(capture){
  assert.equal(capture?.schema,'eagler-presentation/gain-calibration/1');
  assert.equal(capture.method,'future-frame-offline-render');assert.equal(capture.sampleRate,48000);
  assert.equal(capture.startFrame,256);assert.equal(capture.frames,512);
  assert.equal(capture.rows?.length,GAIN_CALIBRATION_CASES.length);
  for(const [index,item] of GAIN_CALIBRATION_CASES.entries()){
    const row=capture.rows[index];assert.equal(row.id,item.id);assert.equal(row.channels?.length,2);
    for(const [channel,gain] of [[0,item.left],[1,item.right]]){
      const actual=row.channels[channel];assert.equal(actual?.length,capture.frames);
      for(let frame=0;frame<capture.frames;frame++){
        const sample=item.samples[frame-capture.startFrame]?.[channel]??0;
        const expected=Math.fround(Math.fround(sample)*Math.fround(gain));
        assert.equal(actual[frame],expected,`${item.id} channel ${channel} frame ${frame}: controlled gain PCM differs`);
      }
    }
  }
  return {status:'passed',cases:capture.rows.length,comparedFrames:capture.frames*capture.rows.length,
    extent:'controlled same-time AudioParam order and float32 gain rendering; immediate game calls use separate event/output and latency checks'};
}
