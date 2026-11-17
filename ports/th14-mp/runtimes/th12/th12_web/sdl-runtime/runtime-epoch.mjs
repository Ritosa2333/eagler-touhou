// TH14 Host navigation has per-frame leases; older Hosts omit this field.
export function runtimeEpochContract(game, value) {
 const required=game==='th14',epoch=Number(value),valid=Number.isSafeInteger(epoch)&&epoch>0;
 return {
  fields:required?{epoch}: {},
  accepts(message){return !required||(valid&&message?.epoch===epoch);},
 };
}
