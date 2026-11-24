const encoder=new TextEncoder();
const hashPattern=/^[a-f0-9]{64}$/;
const assetPath=/^\/(?:[a-zA-Z0-9_-][a-zA-Z0-9_.-]*\/)*[a-zA-Z0-9_-][a-zA-Z0-9_.-]*$/;
const sha=async value=>Array.from(new Uint8Array(await crypto.subtle.digest('SHA-256',value)),v=>v.toString(16).padStart(2,'0')).join('');

export async function openPinnedPreviewResources(files,load){
  if(!files||typeof load!=='function')throw new TypeError('The preview requires a pinned resource manifest');
  const keys=Object.keys(files).sort();
  if(keys.length>400||!['core','wasm','dat'].every(key=>keys.includes(key)))throw new Error('Incomplete TH128 preview resources');
  const entries=new Map(),loaded=new Map();
  for(const key of keys){
    const {url,bytes,sha256}=files[key]??{};
    if(typeof url!=='string'||!assetPath.test(url))throw new Error('Preview resources must use same-origin paths');
    if(!Number.isSafeInteger(bytes)||bytes<1||bytes>64*1024*1024||typeof sha256!=='string'||!hashPattern.test(sha256))
      throw new Error(`Invalid preview pin for ${key}`);
    entries.set(key,Object.freeze({url,bytes,sha256}));
  }
  function resource(key){const entry=entries.get(key);if(!entry)throw new Error(`Unlisted preview resource: ${key}`);return entry;}
  function verified(key){
    const entry=resource(key);
    if(!loaded.has(key))loaded.set(key,(async()=>{
      const raw=await load(entry.url);
      if(!(raw instanceof Uint8Array)||raw.byteLength!==entry.bytes)throw new Error(`Preview ${key} length mismatch`);
      const value=raw.slice();
      if(await sha(value)!==entry.sha256)throw new Error(`Preview ${key} checksum mismatch`);
      return value;
    })());
    return loaded.get(key);
  }
  await Promise.all(['core','wasm','dat'].map(verified));
  const resourceSHA256=await sha(encoder.encode(JSON.stringify([...entries])));
  const build=Object.freeze({coreSHA256:resource('wasm').sha256,resourceSHA256});
  return Object.freeze({build,url:key=>resource(key).url,read:async key=>(await verified(key)).slice()});
}
