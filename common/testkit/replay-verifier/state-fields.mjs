/** Validate title-declared raw fields before converting values into state digests. */
export function assertReplayStateFields(row,{uint32=[],vectors={},byteArrays={}}={}){
  if(!row||typeof row!=='object'||Array.isArray(row))throw new TypeError('Replay state row must be an object');
  for(const key of uint32)if(!Number.isSafeInteger(row[key])||row[key]<0||row[key]>0xffffffff)
    throw new TypeError(`Replay state ${key} must be an unsigned 32-bit integer`);
  for(const [key,length]of Object.entries(vectors)){
    const value=row[key];
    if(!Array.isArray(value)||value.length!==length||!Array.from(value).every(Number.isFinite))
      throw new TypeError(`Replay state ${key} must contain ${length} finite coordinates`);
  }
  for(const [key,length]of Object.entries(byteArrays)){
    const value=row[key];
    if(!Array.isArray(value)||value.length!==length||!Array.from(value).every(byte=>Number.isInteger(byte)&&byte>=0&&byte<=255))
      throw new TypeError(`Replay state ${key} must contain exactly ${length} bytes`);
  }
  return row;
}
