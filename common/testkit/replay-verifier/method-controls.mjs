import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {compareTraces} from './compare.mjs';
import {validateAdapter} from './contracts.mjs';

function changedInput(value){
  if(typeof value==='number')return value===0?1:0;
  if(typeof value==='boolean')return!value;
  if(typeof value==='string')return value+'!';
  if(value===null)return 0;
  if(Array.isArray(value))return value.length?[changedInput(value[0]),...value.slice(1)]:[0];
  const keys=Object.keys(value);return keys.length?{...value,[keys[0]]:changedInput(value[keys[0]])}:{control:0};
}

/** Bounded real records exercise the comparator; an open prefix never claims a natural end. */
export async function replayMethodControls({adapter,fixture,requiredCategories=[],signal}){
  const report={schema:'eagler/replay-method-controls/v1',fixture:fixture?.id??null,replaySha256:fixture?.replaySha256??null,
    scope:'Three real reference ticks | comparator controls only',passed:false,controls:[]};
  let session;
  try{
    assert.ok(fixture,'Method controls need an available selected Replay');assert.ok(!signal?.aborted,'Method controls cancelled');
    const description=validateAdapter(adapter);assert.equal(description.features.original,true);
    assert.equal(createHash('sha256').update(fixture.replayBytes).digest('hex'),fixture.replaySha256);
    session=await adapter.openOriginal({fixture,replayBytes:Uint8Array.from(fixture.replayBytes),
      inspection:await adapter.inspectReplay(fixture.replayBytes,{fixture,signal})},{signal});
    const records=[];let ticks=0;
    for await(const record of session.records){assert.ok(!signal?.aborted,'Method controls cancelled');
      records.push(structuredClone(record));if(record.type==='tick'&&++ticks===3)break;}
    assert.equal(ticks,3,'Method controls require three actual reference ticks');
    assert.equal(records[0].comparisonIdentity.game,description.game);
    assert.equal(records[0].comparisonIdentity.replaySha256,fixture.replaySha256);
    const indices=records.flatMap((record,index)=>record.type==='tick'?[index]:[]),index=indices[1],original=records[index];
    const categories=[...new Set([...(records[0].coverage.requiredCategories??[]),...requiredCategories])];
    assert.ok(categories.length);for(const category of categories)assert.ok(original.categories[category]);
    report.window={records,mutation:{index,sequence:original.sequence,logicalTick:original.logicalTick,
      replaySampleIndex:original.replaySampleIndex,segmentId:original.segmentId,categories}};
    const compare=actual=>compareTraces(records,actual,{requiredCategories});
    const same=await compare(records);report.unchangedPrefix=same;
    assert.equal(same.status,'INCOMPLETE');assert.equal(same.reason,'missing-run-end');assert.equal(same.comparedTicks,3);
    const mutations=[
      ['input',rows=>{rows[index].appliedInput=changedInput(rows[index].appliedInput);},'DIVERGED','tick-field'],
      ...categories.map(category=>['state:'+category,rows=>{const digest=rows[index].categories[category];rows[index].categories[category]=(digest[0]==='0'?'1':'0')+digest.slice(1);},'DIVERGED','category-digest',category]),
      ...['clocks','scalars'].flatMap(group=>Object.keys(original[group]).map(key=>[group+':'+key,
        rows=>{rows[index][group][key]=changedInput(rows[index][group][key]);},'DIVERGED','tick-field'])),
      ['drop',rows=>rows.splice(index,1),'INCOMPLETE','sequence-discontinuity'],
      ['duplicate',rows=>rows.splice(index,0,structuredClone(rows[index])),'INCOMPLETE','sequence-discontinuity'],
      ['shift',rows=>{rows[index].replaySampleIndex++;},'DIVERGED','tick-field'],
    ];
    for(const [id,mutate,status,reason,category]of mutations){
      const actual=structuredClone(records);mutate(actual);const result=await compare(actual);report.controls.push({id,result});
      assert.equal(result.status,status,id);assert.equal(result.reason,reason,id);
      if(id==='drop'||id==='duplicate')assert.equal(result.side,'actual');
      else{
        assert.equal(result.firstDifference.segmentId,original.segmentId,id);
        assert.equal(result.firstDifference.logicalTick,original.logicalTick,id);
        assert.equal(result.firstDifference.replaySampleIndex,original.replaySampleIndex,id);
        if(id==='input'){assert.ok(result.firstDifference.difference.path.startsWith('appliedInput'));
          assert.notDeepEqual(original.appliedInput,actual[index].appliedInput);}
        if(id.startsWith('state:'))assert.equal(result.firstDifference.category,category);
        if(id.startsWith('clocks:')||id.startsWith('scalars:'))assert.ok(result.firstDifference.difference.path.startsWith(id.replace(':','.')));
        if(id==='shift')assert.equal(result.firstDifference.difference.path,'replaySampleIndex');
      }
    }
    report.passed=true;
  }catch(error){report.error=String(error.message??error);}
  finally{try{await session?.close();}catch(error){report.passed=false;report.closeError=String(error.message??error);}}
  return report;
}
