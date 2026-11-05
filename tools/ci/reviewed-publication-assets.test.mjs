import test from 'node:test';
import assert from 'node:assert/strict';
import {mkdtemp,mkdir,writeFile,rm} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {join} from 'node:path';
import {createHash} from 'node:crypto';
import {loadReviewedPublicationAssets} from '../../launcher/lib/reviewed-publication-assets.mjs';
test('review binds exact asset and notice bytes and cannot admit game data or covers',async()=>{
 const root=await mkdtemp(join(tmpdir(),'eagler-asset-review-')),asset='assets/ui.svg',license='LICENSE.txt',manifest=join(root,'review.json');
 await mkdir(join(root,'assets'));await writeFile(join(root,asset),'<svg/>\r\n');await writeFile(join(root,license),'permission\n');
 const identity=(path,text)=>({path,hashMode:'lf-text',sha256:createHash('sha256').update(text).digest('hex')});
 const entry={...identity(asset,'<svg/>\n'),evidence:[identity(license,'permission\n')]};
 const save=entries=>writeFile(manifest,JSON.stringify({schema:'eagler-touhou/reviewed-source-assets/1',entries}));
 await save([entry]);let reviewed=await loadReviewedPublicationAssets(root,manifest);
 assert.equal(await reviewed(asset),true);assert.equal(await reviewed('assets/unknown.svg'),false);
 await writeFile(join(root,asset),'<svg>changed</svg>\n');assert.equal(await reviewed(asset),false);
 await writeFile(join(root,asset),'<svg/>\n');await writeFile(join(root,license),'changed permission\n');assert.equal(await reviewed(asset),false);
 await writeFile(join(root,license),'permission\n');await rm(join(root,license));assert.equal(await reviewed(asset),false);
 await writeFile(join(root,license),'permission\n');await save([{...entry,evidence:[]}]);reviewed=await loadReviewedPublicationAssets(root,manifest);
 assert.equal(await reviewed(asset),false);
 await save([{...entry,evidence:[{...entry.evidence[0],sha256:'0'.repeat(64)}]}]);reviewed=await loadReviewedPublicationAssets(root,manifest);
 assert.equal(await reviewed(asset),false);
 for(const path of ['../escape.svg','assets/private.dat','assets/th11-card.png']){await save([{...entry,path}]);await assert.rejects(loadReviewedPublicationAssets(root,manifest));}
});
