import {afterEach,expect,it,vi} from 'vitest';

afterEach(()=>{vi.unstubAllGlobals();vi.unstubAllEnvs();vi.resetModules();});

function snapshot(game,archive){
  return {product:game,revision:3,uploadedAt:'2026-11-27T00:00:00.000Z',
    files:[{name:'score.dat',bytes:2807,sha256:'a'.repeat(64)}],
    statistics:{game,parserVersion:1,format:'test',complete:true,
      playtimeSeconds:null,applicationTimeSeconds:null,playCount:null,clearCount:null,
      loadouts:[],progress:[],warnings:[],...(archive?{spellArchive:archive}:{})}};
}

// CATK parser regression sample: card 206 is Last Word, practice 19/1.
const archive={scope:'loadout',cards:[{number:206,loadouts:[{
  character:2,shot:0,difficulty:'last-word',activity:'practice',attempts:19,captures:1,
}]}]};

async function client(value){
  vi.stubEnv('VITE_ACCOUNTS_ENABLED','true');
  vi.stubGlobal('location',{origin:'https://touhouonline.com'});
  vi.stubGlobal('fetch',vi.fn(async input=>Response.json(String(input)==='/api/ready'?{ready:true}:value)));
  return import('../../app/services/cloud-saves.ts');
}

function listing(saves){return {products:[],saves,pending:[],limits:{fileBytes:4194304,totalBytes:8388608,files:6}};}

it('reads mixed saves with th08 Last Word and preserves practice counters',async()=>{
  const saves=[snapshot('th07'),snapshot('th08',archive)];
  const api=await client(listing(saves));
  const result=await api.listCloudSaves();
  expect(result.saves).toHaveLength(2);
  expect(result.saves[1].statistics.spellArchive).toEqual(archive);
  expect(result.saves[1].files).toEqual(saves[1].files);
});

it.each(['new-difficulty',null])('isolates unsupported optional archive data (%s)',async difficulty=>{
  const unsupported=structuredClone(archive);
  unsupported.cards[0].loadouts[0].difficulty=difficulty;
  const saves=[snapshot('th07'),snapshot('th08',unsupported)];
  const api=await client(listing(saves));
  const result=await api.listCloudSaves();
  expect(result.saves).toHaveLength(2);
  expect(result.saves[1]).toMatchObject({revision:3,files:saves[1].files});
  expect(result.saves[1].statistics.spellArchive).toBeUndefined();
  expect(result.saves[1].statistics.warnings).toContain('spell-archive-unavailable');
  expect(result.saves[0].statistics.warnings).toEqual([]);
});

it('also accepts Last Word in preview, upload and public profile responses',async()=>{
  const save=snapshot('th08',archive);
  const api=await client(save);
  expect((await api.previewCloudSave('th08',[])).statistics.spellArchive).toEqual(archive);
  expect((await api.uploadCloudSave('th08',2,[])).statistics.spellArchive).toEqual(archive);
  vi.stubGlobal('fetch',async()=>Response.json({career:true,records:true,playtimeSeconds:null,games:[save]}));
  expect((await api.getProfileStatistics('test')).games[0].statistics.spellArchive).toEqual(archive);
});

it('continues to reject invalid revision and file integrity metadata',async()=>{
  const save=snapshot('th08',archive);save.revision=-1;save.files[0].sha256='invalid';
  const api=await client(listing([save]));
  await expect(api.listCloudSaves()).rejects.toMatchObject({code:'ACCOUNT_RESPONSE_INVALID'});
});

it('accepts older archives without activity',async()=>{
  const legacy=structuredClone(archive);delete legacy.cards[0].loadouts[0].activity;
  legacy.cards[0].loadouts[0].difficulty='normal';
  const api=await client(listing([snapshot('th07',legacy)]));
  expect((await api.listCloudSaves()).saves[0].statistics.spellArchive).toEqual(legacy);
});
