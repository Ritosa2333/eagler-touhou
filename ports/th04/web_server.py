"""Shared static delivery for the local and LAN launchers."""
from collections import OrderedDict
from datetime import timezone
from email.utils import parsedate_to_datetime
import gzip
from http.server import SimpleHTTPRequestHandler
from io import BytesIO
import os
from pathlib import Path
import threading
from urllib.parse import urlsplit


class AssetHandler(SimpleHTTPRequestHandler):
    # Compress once per file revision, then share the result across players.
    # Keep memory bounded even when several engine variants are requested.
    _compressed = OrderedDict()
    _compression_lock = threading.Lock()
    _cache_limit = 32 * 1024 * 1024
    _compressible = {'.wasm', '.js', '.mjs', '.css', '.json', '.html'}

    def end_headers(self):
        path = urlsplit(self.path).path
        dynamic = path.startswith('/api/') or path == '/relay'
        # Revalidate static resources on every visit so updates take effect.
        self.send_header('Cache-Control', 'no-store' if dynamic else 'no-cache')
        super().end_headers()

    def accepts_gzip(self):
        encodings = {}
        for item in self.headers.get('Accept-Encoding', '').split(','):
            name, *params = item.strip().lower().split(';')
            quality = 1.0
            for param in params:
                key, _, value = param.strip().partition('=')
                if key == 'q':
                    try:
                        quality = float(value)
                    except ValueError:
                        quality = 0.0
            encodings[name] = quality
        return encodings.get('gzip', encodings.get('*', 0)) > 0

    def compressed_asset(self, path, revision, source):
        with self._compression_lock:
            cached = self._compressed.get(path)
            if cached is not None and cached[0] == revision:
                self._compressed.move_to_end(path)
                return cached[1]
            data = gzip.compress(source.read(), compresslevel=3, mtime=0)
            self._compressed[path] = (revision, data)
            self._compressed.move_to_end(path)
            while sum(len(entry[1]) for entry in self._compressed.values()) > self._cache_limit:
                self._compressed.popitem(last=False)
            return data

    def send_head(self):
        path = self.translate_path(self.path)
        if not os.path.isfile(path):
            return super().send_head()
        try:
            source = open(path, 'rb')
        except OSError:
            self.send_error(404, 'File not found')
            return None
        try:
            stat = os.fstat(source.fileno())
            compressible = Path(path).suffix.lower() in self._compressible and stat.st_size >= 1024
            encoded = compressible and self.accepts_gzip()
            revision = (stat.st_mtime_ns, stat.st_size)
            etag = f'W/"{stat.st_mtime_ns:x}-{stat.st_size:x}-{"gzip" if encoded else "identity"}"'
            match = self.headers.get('If-None-Match')
            unchanged = match is not None and any(
                tag.strip().removeprefix('W/') in ('*', etag.removeprefix('W/'))
                for tag in match.split(','))
            if match is None and self.headers.get('If-Modified-Since'):
                try:
                    since = parsedate_to_datetime(self.headers['If-Modified-Since'])
                    if since.tzinfo is None:
                        since = since.replace(tzinfo=timezone.utc)
                    unchanged = int(stat.st_mtime) <= since.timestamp()
                except (TypeError, ValueError, OverflowError):
                    pass
            body = source
            if encoded and not unchanged:
                body = BytesIO(self.compressed_asset(path, revision, source))
                source.close()
            # A cache hit needs only metadata, without reading/compressing WASM.
            self.send_response(304 if unchanged else 200)
            self.send_header('ETag', etag)
            self.send_header('Last-Modified', self.date_time_string(stat.st_mtime))
            if compressible:
                self.send_header('Vary', 'Accept-Encoding')
            if unchanged:
                self.end_headers()
                source.close()
                return None
            if encoded:
                self.send_header('Content-Encoding', 'gzip')
            self.send_header('Content-Type', self.guess_type(path))
            self.send_header('Content-Length', str(body.getbuffer().nbytes if encoded else stat.st_size))
            self.end_headers()
            return body
        except BaseException:
            source.close()
            raise
