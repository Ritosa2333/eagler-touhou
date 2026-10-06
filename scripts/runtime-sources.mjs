import fs from 'node:fs';
import path from 'node:path';

export function runtimeSources(root) {
  return ['src/core.cpp', ...['src/game', 'src/wasm'].flatMap(folder =>
    fs.readdirSync(path.join(root, folder)).filter(name => name.endsWith('.cpp'))
      .sort().map(name => folder + '/' + name))];
}

export function runtimeSourceFiles(root, folder = 'src') {
  return fs.readdirSync(path.join(root, folder), {withFileTypes: true})
    .sort((a, b) => a.name.localeCompare(b.name)).flatMap(entry => {
      const name = folder + '/' + entry.name;
      return entry.isDirectory() ? runtimeSourceFiles(root, name)
        : /\.(cpp|hpp)$/.test(name) ? [name] : [];
    });
}
