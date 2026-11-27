#!/usr/bin/env python3
"""th16 试玩服务器：http.server 禁缓存版（no-store），模块与 wasm 每次取新。
用法：python3 serve.py [端口]（默认 8446），在 ports/th16/web 目录下运行。"""
import sys
from http.server import SimpleHTTPRequestHandler, ThreadingHTTPServer


class NoCacheHandler(SimpleHTTPRequestHandler):
    def end_headers(self):
        self.send_header("Cache-Control", "no-store, must-revalidate")
        self.send_header("Pragma", "no-cache")
        self.send_header("Expires", "0")
        super().end_headers()

    def log_message(self, fmt, *args):
        sys.stderr.write("[%s] %s\n" % (self.log_date_time_string(), fmt % args))


def main():
    port = int(sys.argv[1]) if len(sys.argv) > 1 else 8446
    server = ThreadingHTTPServer(("0.0.0.0", port), NoCacheHandler)
    print(f"th16 playtest server on http://127.0.0.1:{port}/sdl-runtime/th16-dev.html (no-store)")
    server.serve_forever()


if __name__ == "__main__":
    main()
