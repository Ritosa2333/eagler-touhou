import { fileURLToPath } from 'node:url';
import { resolve } from 'node:path';

const [outfile, architecture] = process.argv.slice(2);
if (!outfile) throw Error('需要指定联机后台程序的输出路径');
if (process.platform !== 'win32' || !['x64', 'arm64'].includes(architecture) || process.arch !== architecture)
  throw Error('当前构建需要 Windows 上与目标架构一致的 Bun');

const addon = fileURLToPath(import.meta.resolve(`@node-datachannel/win32-${architecture}-msvc`));
const result = await Bun.build({
  entrypoints: [resolve(import.meta.dir, 'bridge.mts')],
  compile: {outfile},
  minify: true,
  plugins: [{
    name: 'bundle node-datachannel',
    setup(build) {
      // The package chooses its binding dynamically. Use the selected package
      // at build time so the executable can load it on a clean machine.
      build.onLoad({filter: /[\\/]lib[\\/]node-datachannel\.mjs$/}, () => ({
        contents: `export default require(${JSON.stringify(addon)});`,
        loader: 'js',
      }));
    },
  }],
});
for (const log of result.logs) console.error(log);
if (!result.success) process.exitCode = 1;
