import {execFileSync} from 'node:child_process';
import {existsSync, readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {PRODUCT_GAMES} from '../common/contracts/product-catalog.mjs';
import {isNodeTest} from '../tools/testing/discovery.mjs';

export const repository = resolve(import.meta.dirname, '..');
const node = (id, args, cwd=repository, extra={}) => ({id, command:process.execPath, args, cwd,
  timeoutMs:300000, ...extra});

export function trackedFiles(root=repository) {
  return [...new Set(execFileSync('git', ['ls-files', '--cached', '--others', '--exclude-standard', '-z'],
    {cwd:root, encoding:'utf8', windowsHide:true, maxBuffer:32*1024*1024}).split('\0').filter(Boolean))].sort();
}

export function testFiles(files, directory, pattern) {
  const selected = files.filter(file=>file.startsWith(directory + '/') && !file.split('/').includes('fixtures') &&
    (typeof pattern === 'function' ? pattern(file.slice(directory.length + 1)) : pattern.test(file.slice(directory.length + 1))));
  if (!selected.length) throw new Error(`No test files found in ${directory}`);
  return selected;
}

export function suites({root=repository, output=resolve(root, 'artifacts/validation/tests'), environment=process.env}={}) {
  const files = trackedFiles(root), launcher = resolve(root, 'launcher');
  const workspace = JSON.parse(readFileSync(resolve(root, 'launcher/config/workspace.json')));
  const nativeBuild = resolve(output, 'common-native-build');
  const requireInput = (name, marker) => {
    const value = environment[name];
    if (!value || (marker && !existsSync(resolve(value, marker)))) throw new Error(`Required test input: ${name}${marker ? ` (${marker})` : ''}`);
    return value;
  };
  function packageTest(id,directory,script) {
    const scripts=JSON.parse(readFileSync(resolve(root,directory,'package.json'))).scripts,seen=new Set();
    let command=scripts[script];
    while (/^npm run [\w:-]+$/.test(command??'')) {
      if (seen.has(command)) throw new Error(`Cyclic test script: ${directory}/${script}`);
      seen.add(command);command=scripts[command.split(' ').at(-1)];
    }
    if (!/^node [\w./*=-]+(?: [\w./*=-]+)*$/.test(command??'')) throw new Error(`Expected a bounded Node test script: ${directory}/${script}`);
    const args=command.split(' ').slice(1).flatMap(argument=>{
      if (!argument.includes('*')) return [argument];
      const expression=new RegExp('^'+argument.split('*').map(part=>part.replace(/[.*+?^${}()|[\]\\]/g,'\\$&')).join('[^/]*')+'$');
      return testFiles(files,directory,expression).map(file=>file.slice(directory.length+1));
    });
    return node(id,args,resolve(root,directory));
  }
  return {
    tooling: {
      scope:'Build tools, test execution, source ownership and repository policy', tier:'quick',
      tasks:()=>[node('tooling', ['--test', '--test-concurrency=2', ...testFiles(files, 'tools', isNodeTest)], root,
        {timeoutMs:180000, resources:['source-tree']})],
    },
    'launcher-core': {
      scope:'Launcher modules, types, syntax, publication and generated content', tier:'quick',
      tasks:()=>[node('launcher-core', ['scripts/check.mjs',`--output=${resolve(output,'launcher-core-details')}`], launcher,
        {timeoutMs:600000, resources:['source-tree','launcher-build']})],
    },
    'launcher-ui': {
      scope:'Mounted React and DOM behavior with fixed historical inputs', tier:'integration', concurrency:2,
      prepare:()=>[node('prepare-ui', ['scripts/build-launcher.mjs','--force'], launcher)],
      tasks:()=>[
        ...testFiles(files, 'launcher/tests/react-main', /^[^/]+\.test\.mjs$/).map(file=>{
          const name=file.split('/').at(-1).replace('.test.mjs','');
          const coldBuild=['contracts-build','app-shell-build','development-server'].includes(name);
          return node(`ui-${name}`, ['--test', resolve(root,file)], launcher,
            {timeoutMs:600000, resources:coldBuild ? ['react-cold-build'] : []});
        }),
        node('launcher-site-notice', ['tests/react-main/site-notice.run.mjs'], launcher, {resources:['react-cold-build']}),
      ],
    },
    'launcher-support': {
      scope:'HTTP, browser helpers, temporary packages and current keyboard entry', tier:'integration',
      tasks:()=>[node('launcher-support', ['--test', '--test-concurrency=2',
        ...testFiles(files, 'launcher/tests/support', /^[^/]+\.test\.mjs$/).map(file=>resolve(root, file))], launcher,
      {timeoutMs:300000, resources:['source-tree','launcher-build']})],
    },
    'launcher-workspace': {
      scope:'Cross-component source contracts and explicitly prepared local integrations', tier:'acceptance',
      tasks:()=>[node('launcher-workspace', ['scripts/check.mjs', '--lane=workspace',`--output=${resolve(output,'launcher-workspace-details')}`], launcher,
        {timeoutMs:600000, resources:['source-tree','launcher-build']})],
    },
    'ports-source': {
      scope:'Current source and module checks for all 15 product variants', tier:'integration',
      tasks:()=>[node('ports-source', ['tools/ports/check.mjs', '--all', '--check-status',
        `--output=${resolve(output, 'ports-source')}`], root,
      {timeoutMs:600000, resources:['source-tree','launcher-build']})],
    },
    'ports-unit': {
      scope:'Per-title server, lifecycle, package isolation, input and replay modules with public fixtures',tier:'integration',concurrency:2,
      tasks:()=>[
        node('golden-integrity',['--test','--test-concurrency=2',...['th08','th08-mp','th10','th10-mp'].map(owner=>`ports/${owner}/tools/replay-verifier/golden.test.mjs`)]),
        packageTest('th09-server','ports/th09/web','test:server'),
        packageTest('th09-timeline','ports/th09/web','test:spectator-timeline'),
        packageTest('th11-browser','ports/th11/web','test:browser-contracts'),
        packageTest('th11-mp-browser','ports/th11-mp/web','test:browser-contracts'),
        packageTest('th11-mp-runtime','ports/th11-mp/web','test:multiplayer-runtime'),
        node('th15-progress',['--test','ports/th15/tools/replay-verifier/original-progress.test.mjs']),
        node('th15-mp-progress',['--test','ports/th15-mp/tools/replay-verifier/original-progress.test.mjs']),
        node('th20-modules',['--test','ports/th20/portable/test-outside-touch.mjs','ports/th20/portable/test-eagler-host.mjs','ports/th20/tools/replay-verifier/adapter.test.mjs']),
      ],
    },
    'ports-oracle': {
      scope:'Frozen original/native comparison suites against current compiled fixture cores and private reference inputs',tier:'acceptance',concurrency:1,
      tasks:()=>['th09','th11','th11-mp','th15','th15-mp'].map(owner=>{
        const directory=`ports/${owner}/web`;
        for (const file of ['artifacts/cpp/game-core-test.wasm','artifacts/cpp/game-core-test-build.json'])
          if (!existsSync(resolve(root,directory,file))) throw new Error(`Required oracle build input: ${directory}/${file}`);
        return node(`${owner}-oracle`,['tools/testing/oracle-cli.mjs',`--root=${resolve(root,directory)}`,'--name=local-oracle'],root,{timeoutMs:7500000});
      }),
    },
    'common-native': {
      scope:'Compiled common C++ consumers and browser bridge modules', tier:'integration', sequential:true,
      tasks:()=>[
        {id:'common-configure', command:environment.EAGLER_TEST_CMAKE ?? 'cmake', cwd:root, args:['-S', resolve(root, 'common'), '-B', nativeBuild,
          '-DCMAKE_BUILD_TYPE=Release', '-DEAGLER_COMMON_BUILD_TESTS=ON'], timeoutMs:120000},
        {id:'common-build', command:environment.EAGLER_TEST_CMAKE ?? 'cmake', cwd:root, args:['--build', nativeBuild, '--config', 'Release', '--parallel', '4'], timeoutMs:300000},
        {id:'common-test', command:environment.EAGLER_TEST_CTEST ?? 'ctest', cwd:root, args:['--test-dir', nativeBuild, '-C', 'Release', '--no-tests=error', '--output-on-failure'], timeoutMs:180000},
      ],
    },
    relay: {
      scope:'Standalone Relay protocol, room isolation and lifecycle behavior', tier:'quick',
      tasks:()=>[node('relay', ['scripts/check.mjs'], resolve(root, workspace.repositories.relay), {timeoutMs:180000})],
    },
    telemetry: {
      scope:'Standalone telemetry requests, validation and persistence', tier:'quick',
      tasks:()=>[node('telemetry', ['--test', ...testFiles(files, workspace.repositories.telemetry + '/tests', isNodeTest)
        .map(file=>resolve(root, file))], resolve(root, workspace.repositories.telemetry))],
    },
    turn: {
      scope:'TURN service configuration and generated deployment values', tier:'quick',
      tasks:()=>[node('turn', ['scripts/check.mjs'], resolve(root, workspace.repositories.turn))],
    },
    'browser-shell': {
      scope:'Chromium executes production Shell wiring for all 15 variants, with a broken-wiring negative control', tier:'integration', sequential:true,
      tasks:()=>[
        node('runtime-protocol', ['launcher/tests/test-runtime-protocol-contract.mjs'], root),
        node('native-gamepad', ['launcher/tests/test-gamepad-contract.mjs'], root),
        {id:'browser-shell',command:environment.TH_PYTHON ?? 'python',cwd:root,
          args:['launcher/tests/test-keyboard-mapping-browser.py', `--workspace=${root}`],timeoutMs:180000,resources:['browser']},
        {id:'browser-shell-negative',command:environment.TH_PYTHON ?? 'python',cwd:root,
          args:['launcher/tests/test-keyboard-mapping-browser.py', `--workspace=${root}`, '--negative-control'],timeoutMs:180000,resources:['browser']},
      ],
    },
    'runtime-browser': {
      scope:'Packaged ordinary WASM, private DATA, native input, persistence and restart', tier:'acceptance',
      tasks:()=>[node('runtime-browser', ['tools/ports/runtime-browser.mjs',
        `--manifest=${requireInput('EAGLER_TEST_RUNTIME_MANIFEST')}`, '--gamepad', '--variant=normal',
        `--game=${Object.keys(PRODUCT_GAMES).join(',')}`,
        `--output=${resolve(output, 'runtime-browser')}`], root,
      {timeoutMs:1800000, resources:['browser','runtime']})],
    },
    'runtime-rooms': {
      scope:'Packaged MP WASM through real Launcher rooms, RTC/Relay and peer departure', tier:'acceptance',
      tasks:()=>[2,3].flatMap(players=>['rtc','relay'].map(route=>{
        const games = Object.entries(PRODUCT_GAMES).filter(([, product])=>product.multiplayer?.playerCounts.includes(players)).map(([game])=>game);
        return node(`rooms-${players}p-${route}`, ['tools/ports/room-browser.mjs',
          `--manifest=${requireInput('EAGLER_TEST_RUNTIME_MANIFEST')}`,
          `--url=${requireInput('EAGLER_TEST_LAUNCHER_URL')}`, `--route=${route}`, `--players=${players}`,
          `--game=${games.join(',')}`,
          '--disconnect', '--gamepad', `--output=${resolve(output, `rooms-${players}p-${route}`)}`,
          ...(environment.EAGLER_TEST_PACKAGES ? [`--packages=${environment.EAGLER_TEST_PACKAGES}`] : [])], root,
        {timeoutMs:1800000, resources:['browser','runtime']});
      })),
    },
  };
}

export const profiles = Object.freeze({
  quick:['tooling','launcher-core','relay','telemetry','turn'],
  integration:['tooling','launcher-core','launcher-ui','launcher-support','browser-shell','ports-source','ports-unit','common-native','relay','telemetry','turn'],
  acceptance:['launcher-workspace','runtime-browser','runtime-rooms'],
  oracle:['ports-oracle'],
});
