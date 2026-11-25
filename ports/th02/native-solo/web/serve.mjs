import { createReadStream, statSync } from 'node:fs';
import { createServer } from 'node:http';
import { extname, resolve, sep } from 'node:path';
import { pathToFileURL } from 'node:url';

export function previewServer(directory) {
    const root = resolve(directory);
    return createServer((request, response) => {
        let name;
        try { name = decodeURIComponent(new URL(request.url, 'http://localhost').pathname); }
        catch { response.writeHead(400).end(); return; }
        const path = resolve(root, '.' + (name === '/' ? '/index.html' : name));
        if (!path.startsWith(root + sep) || !statSync(path, {throwIfNoEntry: false})?.isFile()) {
            response.writeHead(404).end();
            return;
        }
        response.setHeader('Cache-Control', 'no-store');
        response.setHeader('Content-Type', {
            '.html': 'text/html; charset=utf-8',
            '.mjs': 'text/javascript; charset=utf-8',
            '.wasm': 'application/wasm',
            '.json': 'application/json',
        }[extname(path)] ?? 'application/octet-stream');
        createReadStream(path).pipe(response);
    });
}

if (import.meta.url === pathToFileURL(process.argv[1]).href) {
    const [directory = 'build-web', port = '18323'] = process.argv.slice(2);
    previewServer(directory).listen(Number(port), '127.0.0.1', () =>
        console.log(`http://127.0.0.1:${port}/`));
}
