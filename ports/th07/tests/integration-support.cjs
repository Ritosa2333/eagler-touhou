const fs = require('node:fs');
const path = require('node:path');

function requireDirectoryEnv(name, description) {
  const raw = process.env[name];
  if (!raw) throw new Error(`${name} must point to ${description}`);
  const root = path.resolve(raw);
  if (!fs.existsSync(root) || !fs.statSync(root).isDirectory()) {
    throw new Error(`${name} is not a directory: ${root}`);
  }
  return root;
}

function requireHostFile(relativePath, description) {
  const root = requireDirectoryEnv(
    'TH_EAGLER_HOST_ROOT',
    'an eagler-touhou checkout with its Host integration files',
  );
  const target = path.join(root, ...relativePath.split('/'));
  if (!fs.existsSync(target) || !fs.statSync(target).isFile()) {
    throw new Error(`TH_EAGLER_HOST_ROOT is missing ${description} (${relativePath}): ${root}`);
  }
  return target;
}

function requireHostNodeModule(moduleName) {
  const root = requireDirectoryEnv(
    'TH_EAGLER_HOST_ROOT',
    'an eagler-touhou checkout with its Node dependencies installed',
  );
  let resolved;
  try { resolved = require.resolve(moduleName, { paths: [root] }); }
  catch (error) {
    throw new Error(`Unable to resolve Host Node module ${moduleName} from ${root}. Install the Host Node dependencies first. Original error: ${error.message}`);
  }
  const relative = path.relative(root, resolved);
  if (relative.startsWith(`..${path.sep}`) || path.isAbsolute(relative)) {
    throw new Error(`Host Node module ${moduleName} resolved outside TH_EAGLER_HOST_ROOT: ${resolved}`);
  }
  return require(resolved);
}

module.exports = { requireDirectoryEnv, requireHostFile, requireHostNodeModule };
