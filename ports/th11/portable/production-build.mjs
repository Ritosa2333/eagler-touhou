// Validate the real binary before the production packager writes any files.
export function validateProductionBuild(build,bytes,profile='sdl3'){
  if(!['sdl3','multiplayer'].includes(profile)||build.profile!==profile||build.diagnostic!==false)
    throw Error('Production packaging rejects diagnostic and unclassified builds');
  const actual=WebAssembly.Module.exports(bytes instanceof WebAssembly.Module?bytes:new WebAssembly.Module(bytes)).map(entry=>entry.name).sort();
  const declared=(build.exports||[]).map(entry=>entry.name).sort();
  if(JSON.stringify(actual)!==JSON.stringify(declared))
    throw Error('Build export attestation does not match the actual WASM');
  for(const name of actual)if(/^(presentation_lab_|audit_|th11_probe_|th11_presentation_)/.test(name))
    throw Error('Production binary contains diagnostic export '+name);
  return new Set(actual);
}
