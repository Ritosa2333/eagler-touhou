import {execFileSync} from 'node:child_process';
import {existsSync, readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {PRODUCT_GAMES} from '../common/contracts/product-catalog.mjs';
import {WORKSPACE_REPOSITORIES} from '../launcher/lib/workspace-layout.mjs';
import {isNodeTest} from '../tools/testing/discovery.mjs';
import {classifyTouchSourceOwner,productTouchMatrix,requireTouchSourceCoverage,sourceOwners} from '../tools/ports/touch-native-invariants.mjs';
import {parseWindowsCompilerHelpers} from '../tools/testing/compiler-helpers.mjs';
import {buildAdaptationPlan} from './adaptation/catalog.mjs';

export {parseWindowsCompilerHelpers} from '../tools/testing/compiler-helpers.mjs';

export const repository = resolve(import.meta.dirname, '..');
export function runtimeBrowserStgGames(products=PRODUCT_GAMES) {
  return Object.entries(products).filter(([,product])=>product.inputProfile!=='fighting').map(([game])=>game);
}
export function touchBombFixtureOwners({root=repository,products=PRODUCT_GAMES,repositories=WORKSPACE_REPOSITORIES}={}) {
  const owners=sourceOwners(productTouchMatrix(products,repositories)).map(owner=>{
    const directory=repositories[owner.sourceOwner];
    if(!directory)throw new Error(`Touch source owner is absent from the workspace catalog: ${owner.sourceOwner}`);
    const sourceRoot=resolve(root,directory);
    return {...classifyTouchSourceOwner(owner,{
      controllerExists:existsSync(resolve(sourceRoot,'portable/input/TouchController.hpp')),
      motionExists:existsSync(resolve(sourceRoot,'portable/input/MotionTrack.hpp')),
    }),sourceRoot};
  });
  requireTouchSourceCoverage(owners);
  for(const owner of owners.filter(item=>item.status==='production-fixture')){
    const missing=['portable/input/touch-bomb-check.cpp','portable/input/touch-bomb-check.mjs']
      .filter(file=>!existsSync(resolve(owner.sourceRoot,file)));
    if(missing.length)throw new Error(`Production touch-bomb owner ${owner.sourceOwner} is missing: ${missing.join(', ')}`);
  }
  return owners;
}
const node = (id, args, cwd=repository, extra={}) => ({id, command:process.execPath, args, cwd,
  timeoutMs:300000, ...extra});

function touchBombModuleTasks(root,output) {
  return touchBombFixtureOwners({root}).filter(owner=>owner.status==='production-fixture').map(owner=>{
    const id=`touch-bomb-${owner.sourceOwner}`;
    return node(id,[resolve(owner.sourceRoot,'portable/input/touch-bomb-check.mjs')],root,{
      timeoutMs:240000,
      environment:{
        EAGLER_TOUCH_BOMB_TASK_ID:id,
        EAGLER_TOUCH_BOMB_SOURCE_OWNER:owner.sourceOwner,
        EAGLER_TOUCH_BOMB_VARIANTS:owner.variants.join(','),
        EAGLER_TOUCH_BOMB_REPORT_DIRECTORY:resolve(output,'touch-bomb-reports',owner.sourceOwner),
      },
    });
  });
}

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

const portModuleUnitTests=Object.freeze([
  'ports/th075-mp/tests/eagler/content-bundle.test.mjs',
  'ports/th075-mp/tests/eagler/multiplayer-options.test.mjs',
  'ports/th075/tests/eagler/content-bundle.test.mjs',
  'ports/th08-mp/portable/multiplayer/analyze-motion-cadence.test.mjs',
  'ports/th08-mp/portable/multiplayer/host-options.test.mjs',
  'ports/th08-mp/portable/package-eagler.test.mjs',
  'ports/th08-mp/portable/presentation-lab/test-analyzer.mjs',
  'ports/th08-mp/portable/test-eagler-host.mjs',
  'ports/th08-mp/portable/test-outside-touch.mjs',
  'ports/th08-mp/portable/test-th08-shell-touch.mjs',
  'ports/th08-mp/tests/multiplayer-build-isolation.test.mjs',
  'ports/th08-mp/tools/replay-verifier/legacy-oracle-adapter.test.mjs',
  'ports/th08/portable/test-eagler-host.mjs',
  'ports/th08/portable/test-outside-touch.mjs',
  'ports/th08/tools/replay-verifier/legacy-oracle-adapter.test.mjs',
  'ports/th09/web/tests/multiplayer/auto-replay.test.mjs',
  'ports/th09/web/tests/multiplayer/callback-cost-probe.test.mjs',
  'ports/th09/web/tests/multiplayer/chrome-scheduler-finish.test.mjs',
  'ports/th09/web/tests/multiplayer/chrome-scheduler-trace.test.mjs',
  'ports/th09/web/tests/multiplayer/gpu-vsync-control.test.mjs',
  'ports/th09/web/tests/multiplayer/raf-anomaly-probe.test.mjs',
  'ports/th09/web/tests/multiplayer/relay-transport-probe.test.mjs',
  'ports/th09/web/tests/multiplayer/startup-transport-probe.test.mjs',
  'ports/th09/web/tests/multiplayer/touch-input-probe.test.mjs',
  'ports/th10-mp/portable/browser/replay-file-policy.test.mjs',
  'ports/th10-mp/portable/multiplayer/build-isolation.test.mjs',
  'ports/th10-mp/portable/multiplayer/replay-robot.test.mjs',
  'ports/th10-mp/portable/package-eagler.test.mjs',
  'ports/th10-mp/portable/practice-loader.test.mjs',
  'ports/th10-mp/portable/response-file.test.mjs',
  'ports/th10-mp/portable/test-eagler-host.mjs',
  'ports/th10-mp/portable/test-outside-touch.mjs',
  'ports/th10-mp/portable/test-th10-shell-touch.mjs',
  'ports/th10-mp/tests/live-motion-analysis.test.mjs',
  'ports/th10-mp/tests/make-dependencies.test.mjs',
  'ports/th10-mp/tests/multiplayer-build-isolation.test.mjs',
  'ports/th10-mp/tools/replay-verifier/playback-validation.test.mjs',
  'ports/th10/portable/test-eagler-host.mjs',
  'ports/th10/portable/test-outside-touch.mjs',
  'ports/th10/tests/make-dependencies.test.mjs',
  'ports/th10/tests/adaptation-state.test.mjs',
  'ports/th10/tools/replay-verifier/playback-validation.test.mjs',
]);
const portPythonUnitTests=Object.freeze([
  'ports/th08-mp/portable/multiplayer/test-resource-trace-comparison.py',
  'ports/th10-mp/portable/multiplayer/test-fixed-work-rate-calibration.py',
  'ports/th10-mp/portable/multiplayer/test-performance-summary.py',
  'ports/th10-mp/tools/replay-verifier/test_capture_current_demos.py',
]);

export function suites({root=repository, output=resolve(root, 'artifacts/validation/tests'), environment=process.env,
  platform=process.platform}={}) {
  const files = trackedFiles(root), launcher = resolve(root, 'launcher');
  const adaptationTestFiles=testFiles(files,'tests/adaptation',isNodeTest);
  const presentationEvidenceTests=adaptationTestFiles.filter(file=>file==='tests/adaptation/adapters/presentation-evidence.test.mjs');
  if(presentationEvidenceTests.length!==1)throw new Error('Missing shared presentation evidence regression');
  const explicitPortTests=[...portModuleUnitTests,...portPythonUnitTests,'ports/th10-mp/tests/high-refresh-contract.test.mjs'];
  if(new Set(explicitPortTests).size!==explicitPortTests.length)throw new Error('Duplicate explicit per-title test path');
  const missingPortTests=explicitPortTests.filter(file=>!files.includes(file));
  if(missingPortTests.length)throw new Error(`Explicit per-title test path is missing: ${missingPortTests.join(', ')}`);
  const workspace = JSON.parse(readFileSync(resolve(root, 'launcher/config/workspace.json')));
  const nativeBuild = resolve(output, 'common-native-build');
  const oracleTargets=Object.freeze(files
    .map(file=>file.replaceAll('\\','/'))
    .filter(file=>/^ports\/[^/]+\/web\/tests\/cpp\/oracle-suite\.json$/.test(file))
    .map(manifest=>{
      const owner=manifest.match(/^ports\/([^/]+)\/web\/tests\/cpp\/oracle-suite\.json$/)[1];
      return Object.freeze({owner,manifest,root:resolve(root,'ports',owner,'web')});
    }).sort((left,right)=>left.owner.localeCompare(right.owner)));
  const compilerHelpers=parseWindowsCompilerHelpers(environment,platform);
  const compilerHelperCleanup=compilerHelpers.length?{cleanupImages:compilerHelpers}:{};
  const commonBuildEnvironment={MSBUILDDISABLENODEREUSE:'1'};
  const testPython=environment.TH_PYTHON??environment.PYTHON??(platform==='win32'?'python':'python3');
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
      scope:'Build tools, test execution, source ownership and repository policy', tier:'quick',coverageKind:'module-unit',
      tasks:()=>[
        node('tooling', ['--test', '--test-timeout=30000', '--test-concurrency=2', ...testFiles(files, 'tools', isNodeTest),
          ...adaptationTestFiles.filter(file=>!presentationEvidenceTests.includes(file))], root,
          {timeoutMs:180000, resources:['source-tree'],...compilerHelperCleanup}),
        node('presentation-evidence', ['--test', '--test-timeout=30000', '--test-concurrency=1', ...presentationEvidenceTests], root,
          {timeoutMs:180000, resources:['source-tree']}),
      ],
    },
    'launcher-core': {
      scope:'Launcher modules, types, syntax, publication and generated content', tier:'quick',coverageKind:'production-component',
      tasks:()=>[node('launcher-core', ['scripts/check.mjs',`--output=${resolve(output,'launcher-core-details')}`], launcher,
        {timeoutMs:600000, resources:['source-tree','launcher-build']})],
    },
    'launcher-ui': {
      scope:'Mounted React and DOM behavior with fixed historical inputs', tier:'integration',coverageKind:'production-component', concurrency:2,
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
      scope:'HTTP, browser helpers, temporary packages and current keyboard entry', tier:'integration',coverageKind:'production-component',
      tasks:()=>[node('launcher-support', ['--test', '--test-concurrency=2',
        ...testFiles(files, 'launcher/tests/support', /^[^/]+\.test\.mjs$/).map(file=>resolve(root, file))], launcher,
      {timeoutMs:300000, resources:['source-tree','launcher-build']})],
    },
    'launcher-workspace': {
      scope:'Cross-component source contracts and explicitly prepared local integrations', tier:'acceptance',coverageKind:'source-protocol',
      tasks:()=>[node('launcher-workspace', ['scripts/check.mjs', '--lane=workspace',`--output=${resolve(output,'launcher-workspace-details')}`], launcher,
        {timeoutMs:600000, resources:['source-tree','launcher-build']})],
    },
    'ports-source': {
      scope:'Current source and module checks for every registered product variant', tier:'integration',coverageKind:'source-protocol',
      tasks:()=>[
        node('ports-source', ['tools/ports/check.mjs', '--all', '--check-status',
          `--output=${resolve(output, 'ports-source')}`], root,
        {timeoutMs:600000, resources:['source-tree','launcher-build']}),
        node('th10-mp-high-refresh-source-contract',['--test','ports/th10-mp/tests/high-refresh-contract.test.mjs'],root,
          {resources:['source-tree']}),
      ],
    },
    'ports-unit': {
      scope:'Shared components and deterministic per-title module/VM fixtures; title fixtures use synthetic inputs and do not claim packaged gameplay',tier:'integration',coverageKind:'production-component',concurrency:2,
      tasks:()=>[
        node('per-title-module-unit-fixtures',['--experimental-vm-modules','--test','--test-concurrency=2',...portModuleUnitTests],root,
          {timeoutMs:600000,resources:['source-tree'],environment:{TH_PYTHON:testPython},...compilerHelperCleanup}),
        ...portPythonUnitTests.map(file=>({id:`${file.split('/').at(-1).replace(/[^a-z0-9]+/gi,'-').toLowerCase()}-module-unit`,
          command:testPython,
          args:[resolve(root,file)],cwd:root,timeoutMs:60000})),
        node('presentation-lab-components',['--test',...testFiles(files,'common/testkit/presentation-lab',isNodeTest),
          'ports/th08/portable/presentation-lab/test-analyzer.mjs']),
        node('golden-integrity',['--test','--test-concurrency=2',...['th08','th08-mp','th10','th10-mp'].map(owner=>`ports/${owner}/tools/replay-verifier/golden.test.mjs`)]),
        node('th06-th07-replay-source-contract',['launcher/tests/test-replay-determinism-contract.mjs'],root,compilerHelperCleanup),
        ...['th06','th07'].map(owner=>({id:`${owner}-cooperative-rules`,
          command:environment.TH_PYTHON??environment.PYTHON??(process.platform==='win32'?'python':'python3'),
          args:[resolve(root,`ports/${owner}/tests/cooperative-rules-test.py`)],cwd:root,timeoutMs:60000,
          ...compilerHelperCleanup})),
        packageTest('th09-server','ports/th09/web','test:server'),
        packageTest('th09-timeline','ports/th09/web','test:spectator-timeline'),
        packageTest('th11-browser','ports/th11/web','test:browser-contracts'),
        packageTest('th11-mp-browser','ports/th11-mp/web','test:browser-contracts'),
        packageTest('th11-mp-runtime','ports/th11-mp/web','test:multiplayer-runtime'),
        node('th15-progress',['--test','ports/th15/tools/replay-verifier/original-progress.test.mjs']),
        node('th15-mp-progress',['--test','ports/th15-mp/tools/replay-verifier/original-progress.test.mjs']),
        node('th15-mp-disconnect',['--test','ports/th15-mp/web/tests/multiplayer/disconnect.test.mjs']),
        node('th20-modules',['--test','ports/th20/portable/test-outside-touch.mjs','ports/th20/portable/test-eagler-host.mjs','ports/th20/tools/replay-verifier/adapter.test.mjs']),
      ],
    },
    'ports-touch-wasi': {
      scope:'Production WASI fixtures | catalog-derived touch controllers, recorded-input lifecycle and th08 visual endpoints; th06/th07 retain reference-only touch anchors',tier:'integration',coverageKind:'production-component',concurrency:2,
      tasks:()=>[...touchBombModuleTasks(root,output),
        node('th08-pending-input-clear',['tools/ports/th08-pending-input-clear-check.mjs'],root),
        node('th08-visual-endpoints',['ports/th08/portable/presentation-lab/test-visual.mjs'],root)],
    },
    'ports-oracle': {
      scope:'Frozen original/native comparison suites against current compiled fixture cores and private reference inputs',tier:'acceptance',coverageKind:'original-comparison',concurrency:1,
      targets:oracleTargets,
      tasks:()=>oracleTargets.map(({owner,root:webRoot})=>{
        for (const file of ['artifacts/cpp/game-core-test.wasm','artifacts/cpp/game-core-test-build.json'])
          if (!existsSync(resolve(webRoot,file))) throw new Error(`Required oracle build input: ${owner}/web/${file}`);
        return node(`${owner}-oracle`,['tools/testing/oracle-cli.mjs',`--root=${webRoot}`,'--name=local-oracle'],root,{timeoutMs:7500000});
      }),
    },
    'common-native': {
      scope:'Compiled common C++ consumers and browser bridge modules', tier:'integration',coverageKind:'production-component', sequential:true,
      tasks:()=>[
        {id:'common-configure', command:environment.EAGLER_TEST_CMAKE ?? 'cmake', cwd:root, args:['-S', resolve(root, 'common'), '-B', nativeBuild,
          '-DCMAKE_BUILD_TYPE=Release', '-DEAGLER_COMMON_BUILD_TESTS=ON'], timeoutMs:120000,
          environment:commonBuildEnvironment,...compilerHelperCleanup},
        {id:'common-build', command:environment.EAGLER_TEST_CMAKE ?? 'cmake', cwd:root, args:['--build', nativeBuild, '--config', 'Release', '--parallel', '4'], timeoutMs:300000,
          environment:commonBuildEnvironment,...compilerHelperCleanup},
        {id:'common-test', command:environment.EAGLER_TEST_CTEST ?? 'ctest', cwd:root, args:['--test-dir', nativeBuild, '-C', 'Release', '--no-tests=error', '--output-on-failure'], timeoutMs:180000,
          environment:commonBuildEnvironment,...compilerHelperCleanup},
      ],
    },
    relay: {
      scope:'Standalone Relay protocol, room isolation and lifecycle behavior', tier:'quick',coverageKind:'module-unit',
      tasks:()=>[node('relay', ['scripts/check.mjs'], resolve(root, workspace.repositories.relay), {timeoutMs:180000})],
    },
    telemetry: {
      scope:'Standalone telemetry requests, validation and persistence', tier:'quick',coverageKind:'module-unit',
      tasks:()=>[node('telemetry', ['--test', ...testFiles(files, workspace.repositories.telemetry + '/tests', isNodeTest)
        .map(file=>resolve(root, file))], resolve(root, workspace.repositories.telemetry))],
    },
    turn: {
      scope:'TURN service configuration and generated deployment values', tier:'quick',coverageKind:'module-unit',
      tasks:()=>[node('turn', ['scripts/check.mjs'], resolve(root, workspace.repositories.turn))],
    },
    'browser-shell': {
      scope:'STG native input, shared touch invariants, real WebAudio queue output and cataloged STG Runtime Shell wiring with negative controls', tier:'integration',coverageKind:'browser', sequential:true,failFast:false,
      tasks:()=>[
        node('runtime-protocol', ['launcher/tests/test-runtime-protocol-contract.mjs'], root),
        node('native-gamepad', ['launcher/tests/test-gamepad-contract.mjs',`--games=${runtimeBrowserStgGames().join(',')}`], root, compilerHelperCleanup),
        node('sfx-queue-output', ['tools/ports/sfx-queue-invariants.mjs',
          `--output=${resolve(output,'sfx-queue-output')}`], root, {timeoutMs:180000,resources:['browser']}),
        node('touch-native-invariants', ['tools/ports/touch-native-invariants.mjs',
          `--output=${resolve(output,'touch-native-invariants')}`], root, {timeoutMs:180000,...compilerHelperCleanup}),
        {id:'browser-shell',command:environment.TH_PYTHON ?? 'python',cwd:root,
          args:['launcher/tests/test-keyboard-mapping-browser.py', `--workspace=${root}`,'--variants',...runtimeBrowserStgGames().flatMap(game=>[`${game}-normal`,...(PRODUCT_GAMES[game].multiplayer?[`${game}-multiplayer`]:[])])],timeoutMs:180000,resources:['browser']},
        {id:'browser-shell-negative',command:environment.TH_PYTHON ?? 'python',cwd:root,
          args:['launcher/tests/test-keyboard-mapping-browser.py', `--workspace=${root}`, '--negative-control'],timeoutMs:180000,resources:['browser']},
      ],
    },
    'runtime-browser': {
      scope:'Packaged ordinary STG WASM, private DATA, native input, persistence and restart', tier:'acceptance',coverageKind:'browser-runtime',
      tasks:()=>[node('runtime-browser', ['tools/ports/runtime-browser.mjs',
        `--manifest=${requireInput('EAGLER_TEST_RUNTIME_MANIFEST')}`, '--gamepad', '--variant=normal',
        `--game=${runtimeBrowserStgGames().join(',')}`,
        `--output=${resolve(output, 'runtime-browser')}`], root,
      {timeoutMs:1800000, resources:['browser','runtime']})],
    },
    'runtime-desktop': {
      scope:'Current ordinary WASM | Shell keyboard paths, remapping, simulated touch and actual WebAudio output', tier:'acceptance',coverageKind:'browser-runtime',
      tasks:()=>[node('runtime-desktop', ['tools/ports/runtime-desktop-probes.mjs',
        `--manifest=${requireInput('EAGLER_TEST_RUNTIME_MANIFEST')}`, '--variant=normal',
        `--game=${Object.keys(PRODUCT_GAMES).join(',')}`,
        `--output=${resolve(output, 'runtime-desktop')}`], root,
      {timeoutMs:1800000, resources:['browser','runtime']})],
    },
    'runtime-practice': {
      scope:'Catalog-derived ordinary Practice acceptance | shared adaptation plan, native lifecycle and real Launcher settings', tier:'acceptance',coverageKind:'browser-runtime',
      tasks:()=>[node('runtime-practice', ['tests/run.mjs','--profile=adaptation','--suite=practice','--variant=normal',
        `--output=${resolve(output, 'runtime-practice-adaptation')}`], root,
      {timeoutMs:1800000, resources:['browser','runtime'],environment:{
        EAGLER_TEST_RUNTIME_MANIFEST:environment.EAGLER_TEST_RUNTIME_MANIFEST??'',
        EAGLER_TEST_PACKAGES:environment.EAGLER_TEST_PACKAGES??'',
        EAGLER_TEST_ARTWORK:environment.EAGLER_TEST_ARTWORK??'',
      }})],
    },
    'runtime-replay': {
      scope:'Current ordinary WASM | official Shell Replay import, 120 rate-1 native frames and native termination signal | full input consumption and original-format compatibility reported separately; MP requires explicit selection', tier:'acceptance',coverageKind:'browser-runtime',
      tasks:()=>[node('runtime-replay', ['tools/ports/replay-browser.mjs',
        `--manifest=${requireInput('EAGLER_TEST_RUNTIME_MANIFEST')}`, '--variant=normal',
        `--game=${Object.keys(PRODUCT_GAMES).join(',')}`,
        `--output=${resolve(output, 'runtime-replay')}`], root,
      {timeoutMs:3600000, resources:['browser','runtime']})],
    },
    'runtime-music': {
      scope:'Catalog-derived ordinary music acceptance | shared adaptation plan, Runtime PCM and native identity; missing interfaces or inputs remain blocked', tier:'acceptance',coverageKind:'browser-runtime',
      tasks:()=>[node('runtime-music', ['tests/run.mjs','--profile=adaptation','--suite=music','--variant=normal',
        `--output=${resolve(output, 'runtime-music-adaptation')}`], root,
      {timeoutMs:1800000, resources:['browser','runtime'],environment:{
        EAGLER_TEST_RUNTIME_MANIFEST:environment.EAGLER_TEST_RUNTIME_MANIFEST??'',
        EAGLER_TEST_MUSIC_MANIFEST:environment.EAGLER_TEST_MUSIC_MANIFEST??'',
        EAGLER_TEST_PACKAGES:environment.EAGLER_TEST_PACKAGES??'',
        EAGLER_TEST_ARTWORK:environment.EAGLER_TEST_ARTWORK??'',
      }})],
    },
    'runtime-offline': {
      scope:'Packaged Launcher | verified local ZIP, offline normal Runtime restart and file persistence', tier:'acceptance',coverageKind:'browser-runtime',
      tasks:()=>Object.keys(PRODUCT_GAMES).map(game=>node(`offline-${game}`, ['tools/ports/offline-runtime-browser.mjs',
        `--manifest=${requireInput('EAGLER_TEST_RUNTIME_MANIFEST')}`,
        `--packages=${requireInput('EAGLER_TEST_PACKAGES')}`,
        `--artwork-dir=${requireInput('EAGLER_TEST_ARTWORK')}`, `--game=${game}`,
        `--output=${resolve(output, `runtime-offline-${game}`)}`], root,
      {timeoutMs:600000, resources:['browser','runtime','launcher-build']})),
    },
    'runtime-offline-update': {
      scope:'Packaged Launcher | incomplete local update retention, complete second-generation commit and offline recovery', tier:'acceptance',coverageKind:'browser-runtime',
      tasks:()=>{
        const updateManifestPath=requireInput('EAGLER_TEST_UPDATE_PACKAGES');
        const updateManifest=JSON.parse(readFileSync(updateManifestPath,'utf8'));
        const games=Object.keys(updateManifest.games??{});
        if(!games.length||games.some(game=>!PRODUCT_GAMES[game]))throw new Error('EAGLER_TEST_UPDATE_PACKAGES must select supported games');
        return games.map(game=>node(`offline-update-${game}`, ['tools/ports/offline-runtime-browser.mjs',
          `--manifest=${requireInput('EAGLER_TEST_RUNTIME_MANIFEST')}`,
          `--packages=${requireInput('EAGLER_TEST_PACKAGES')}`,
          `--artwork-dir=${requireInput('EAGLER_TEST_ARTWORK')}`,
          `--update-manifest=${updateManifestPath}`,`--game=${game}`,
          `--output=${resolve(output, `runtime-offline-update-${game}`)}`], root,
        {timeoutMs:900000, resources:['browser','runtime','launcher-build']}));
      },
    },
    'runtime-spectator': {
      scope:'Real Launcher 2P Relay rooms | current MP spectator join, native playback and input isolation', tier:'acceptance',coverageKind:'browser-runtime',
      tasks:()=>[node('runtime-spectator', ['tools/ports/room-browser.mjs',
        `--manifest=${requireInput('EAGLER_TEST_RUNTIME_MANIFEST')}`,
        `--url=${requireInput('EAGLER_TEST_LAUNCHER_URL')}`, '--route=relay', '--players=2', '--spectator',
        `--game=${Object.entries(PRODUCT_GAMES).filter(([,product])=>product.multiplayer?.playerCounts.includes(2)).map(([game])=>game).join(',')}`,
        `--output=${resolve(output, 'runtime-spectator')}`,
        ...(environment.EAGLER_TEST_PACKAGES ? [`--packages=${environment.EAGLER_TEST_PACKAGES}`] : [])], root,
      {timeoutMs:1800000, resources:['browser','runtime']})],
    },
    'runtime-rooms': {
      scope:'Packaged MP WASM through real Launcher rooms, RTC/Relay and peer departure', tier:'acceptance',coverageKind:'browser-runtime',
      tasks:()=>[2,3].flatMap(players=>['rtc','relay'].map(route=>{
        const games = Object.entries(PRODUCT_GAMES).filter(([, product])=>product.multiplayer?.playerCounts.includes(players)).map(([game])=>game);
        return node(`rooms-${players}p-${route}`, ['tools/ports/room-browser.mjs',
          `--manifest=${requireInput('EAGLER_TEST_RUNTIME_MANIFEST')}`,
          `--url=${requireInput('EAGLER_TEST_LAUNCHER_URL')}`, `--route=${route}`, `--players=${players}`,
          `--game=${games.join(',')}`,
          '--disconnect', '--gamepad', '--lifecycle', `--output=${resolve(output, `rooms-${players}p-${route}`)}`,
          ...(environment.EAGLER_TEST_PACKAGES ? [`--packages=${environment.EAGLER_TEST_PACKAGES}`] : [])], root,
        {timeoutMs:1800000, resources:['browser','runtime']});
      })),
    },
    adaptation: {
      scope:'Product and variant acceptance scenarios with per-case inputs, coverage mapping and explicit blocked work',tier:'acceptance',coverageKind:'adaptation-scenario',
      plan:options=>buildAdaptationPlan({root,output,environment,...options}),
    },
  };
}

export const profiles = Object.freeze({
  quick:['tooling','launcher-core','relay','telemetry','turn'],
  integration:['tooling','launcher-core','launcher-ui','launcher-support','browser-shell','ports-source','ports-unit','ports-touch-wasi','common-native','relay','telemetry','turn'],
  acceptance:['launcher-workspace','runtime-browser','runtime-desktop','runtime-practice','runtime-replay','runtime-music','runtime-offline','runtime-rooms','runtime-spectator'],
  adaptation:['adaptation'],
  oracle:['ports-oracle'],
});
