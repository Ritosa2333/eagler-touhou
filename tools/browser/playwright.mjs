import {execFileSync} from 'node:child_process';
import {createRequire} from 'node:module';
import {pathToFileURL} from 'node:url';

/** Reuse either the JS install or Python Playwright's existing bundled driver.
 * This helper never downloads a browser or adds a project dependency. */
export function resolvePlaywrightModule({preferPython = false} = {}) {
  try {if (!preferPython) {
    const require = createRequire(new URL('../../launcher/package.json',import.meta.url));
    return pathToFileURL(require.resolve('playwright')).href;
  }}
  catch (error) {if (!['ERR_MODULE_NOT_FOUND','MODULE_NOT_FOUND'].includes(error.code)) throw error;}
  const module = execFileSync(process.env.PYTHON || 'python', ['-c',
    "import pathlib,playwright; print(pathlib.Path(playwright.__file__).parent/'driver'/'package'/'index.mjs')"], {encoding:'utf8',windowsHide:true}).trim();
  return pathToFileURL(module).href;
}

export async function loadPlaywright(options = {}) { return import(resolvePlaywrightModule(options)); }

/** Identify the interpreter, driver and browser used by Python capture scripts without launching a browser. */
export function identifyPythonPlaywright({environment=process.env}={}){
  const command=environment.TH_PYTHON??environment.PYTHON??(process.platform==='win32'?'python':'python3');
  const query=[
    'import importlib.metadata as metadata,json,pathlib,re,sys,playwright',
    'from playwright.sync_api import sync_playwright',
    'from playwright._impl._driver import compute_driver_executable',
    'def dependencies(root):',
    ' pending=[root]; seen={};',
    ' while pending:',
    '  requested=pending.pop(); dist=metadata.distribution(requested); name=dist.metadata.get("Name",requested); key=re.sub(r"[-_.]+","-",name).lower()',
    '  if key in seen: continue',
    '  files=dist.files',
    '  if files is None: raise RuntimeError("Python distribution has no file inventory: "+name)',
    '  paths=set()',
    '  for item in files:',
    '   relative=pathlib.PurePosixPath(str(item))',
    '   if "__pycache__" in relative.parts or relative.suffix==".pyc": continue',
    '   path=pathlib.Path(dist.locate_file(item)).resolve()',
    '   if not path.is_file(): raise RuntimeError("Python distribution file is missing: "+str(path))',
    '   paths.add(str(path))',
    '  if not paths: raise RuntimeError("Python distribution has no hashable files: "+name)',
    '  seen[key]={"name":name,"version":dist.version,"files":sorted(paths)}',
    '  for requirement in dist.requires or []:',
    '   parts=requirement.split(";",1)',
    '   if len(parts)>1 and "extra" in parts[1]: continue',
    '   match=re.match(r"\\s*([A-Za-z0-9][A-Za-z0-9._-]*)",parts[0])',
    '   if match:',
    '    try: metadata.distribution(match.group(1))',
    '    except metadata.PackageNotFoundError: continue',
    '    pending.append(match.group(1))',
    ' return sorted(seen.values(),key=lambda item:item["name"].lower())',
    'with sync_playwright() as driver:',
    ' print(json.dumps({"interpreter":str(pathlib.Path(sys.executable).resolve()),"pythonVersion":sys.version,"packageRoot":str(pathlib.Path(playwright.__file__).parent.resolve()),"playwrightVersion":metadata.version("playwright"),"dependencies":dependencies("playwright"),"executable":driver.chromium.executable_path,"driverExecutable":str(compute_driver_executable()[0])}))',
  ].join('\n');
  return JSON.parse(execFileSync(command,['-c',query],{encoding:'utf8',windowsHide:true,
    env:environment,timeout:30000,maxBuffer:1024*1024}).trim());
}
