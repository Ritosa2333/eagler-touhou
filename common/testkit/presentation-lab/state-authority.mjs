export const STATE_AUTHORITY_SCHEMA='presentation-lab/state-authority/1';

// Every presentation adapter maps its native snapshot groups onto these
// title-neutral ownership domains. A digest group can support more than one
// domain, but every domain and every group must have an explicit source map.
export const REQUIRED_STATE_AUTHORITY_DOMAINS=Object.freeze([
  'simulation-world',
  'gameplay-session',
  'practice-state',
  'replay-state',
  'input-transients',
  'user-payload-buffers',
  'persistent-storage',
  'synchronous-audio',
  'host-cadence',
  'presentation-owners',
]);

// Build the adapter's map from its digest-group source fields. Missing domain
// or group mappings remain empty and are rejected by validateStateAuthority.
export function defineStateAuthority({groupFields={},domains={}}={}){
  const ids=[...REQUIRED_STATE_AUTHORITY_DOMAINS,...Object.keys(domains).filter(id=>!REQUIRED_STATE_AUTHORITY_DOMAINS.includes(id))];
  return Object.freeze({schema:STATE_AUTHORITY_SCHEMA,domains:Object.freeze(ids.map(id=>{
    const groups=Array.isArray(domains[id])?[...domains[id]]:[];
    const sourceFields=[...new Set(groups.flatMap(group=>Array.isArray(groupFields[group])?groupFields[group]:[]))];
    return Object.freeze({id,groups:Object.freeze(groups),sourceFields:Object.freeze(sourceFields)});
  }))});
}

export function validateStateAuthority(evidence){
  const groups=Array.isArray(evidence?.groups)?evidence.groups:[];
  const groupIds=new Set(groups.map(group=>group?.id).filter(id=>typeof id==='string'&&id));
  const fieldsByGroup=new Map();
  for(const group of groups){
    const fields=Array.isArray(group?.includedFields)?group.includedFields:[];
    if(!fields.length||fields.some(field=>typeof field!=='string'||!field.trim())||new Set(fields).size!==fields.length)
      fieldsByGroup.set(group?.id, null);
    else fieldsByGroup.set(group.id,new Set(fields));
  }
  const authority=evidence?.authority;
  if(authority?.schema!==STATE_AUTHORITY_SCHEMA||!Array.isArray(authority.domains))
    return {complete:false,missingDomains:[...REQUIRED_STATE_AUTHORITY_DOMAINS],unmappedGroups:[...groupIds],reason:'missing authority map'};
  const seen=new Set(),missingDomains=[],invalidDomains=[],mappedGroups=new Set(),mappedFields=new Set();
  for(const domain of authority.domains){
    if(!domain||typeof domain.id!=='string'||!REQUIRED_STATE_AUTHORITY_DOMAINS.includes(domain.id)||seen.has(domain.id)){
      invalidDomains.push(domain?.id??null);continue;
    }
    seen.add(domain.id);
    const domainGroups=Array.isArray(domain.groups)?domain.groups:[];
    const sourceFields=Array.isArray(domain.sourceFields)?domain.sourceFields:[];
    if(!domainGroups.length||new Set(domainGroups).size!==domainGroups.length||!sourceFields.length||new Set(sourceFields).size!==sourceFields.length||sourceFields.some(field=>typeof field!=='string'||!field.trim()))invalidDomains.push(domain.id);
    for(const id of domainGroups){
      if(!groupIds.has(id)){invalidDomains.push(domain.id);continue;}
      mappedGroups.add(id);
      const groupFields=fieldsByGroup.get(id);
      if(!groupFields){invalidDomains.push(domain.id);continue;}
      for(const field of sourceFields){
        if(groupFields.has(field))mappedFields.add(`${id}\0${field}`);
      }
    }
    for(const field of sourceFields){
      if(!domainGroups.some(id=>fieldsByGroup.get(id)?.has(field)))invalidDomains.push(domain.id);
    }
  }
  for(const id of REQUIRED_STATE_AUTHORITY_DOMAINS)if(!seen.has(id))missingDomains.push(id);
  const unmappedGroups=[...groupIds].filter(id=>!mappedGroups.has(id));
  const uncoveredFields=[];
  for(const [id,fields] of fieldsByGroup){
    if(!fields){uncoveredFields.push(`${id}:missing-included-fields`);continue;}
    for(const field of fields)if(!mappedFields.has(`${id}\0${field}`))uncoveredFields.push(`${id}:${field}`);
  }
  const complete=!missingDomains.length&&!invalidDomains.length&&!unmappedGroups.length&&!uncoveredFields.length&&groupIds.size===groups.length;
  return {complete,missingDomains,invalidDomains,unmappedGroups,uncoveredFields,reason:complete?null:'authority map is incomplete'};
}
