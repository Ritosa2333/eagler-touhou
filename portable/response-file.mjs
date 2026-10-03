// Emscripten expands @file with Python shlex.split (POSIX quoting), including
// on Windows. Keep the exact argument bytes and order, not shell interpolation.
export function responseFileContents(args){
 if(!Array.isArray(args)||args.some(value=>typeof value!=='string'||value.includes('\0')))
  throw new TypeError('Compiler arguments must be strings without NUL');
 return args.map(value=>'"'+value.replaceAll('\\','\\\\').replaceAll('"','\\"')+'"').join('\n')+'\n';
}
