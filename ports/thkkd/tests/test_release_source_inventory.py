"""检查源码包保留核心调度代码，并排除编译后的网页程序。"""
import importlib.util
from pathlib import Path
import tempfile
import unittest
import subprocess
from unittest.mock import patch


spec = importlib.util.spec_from_file_location(
    'thkkd_release_web', Path(__file__).resolve().parents[1] / 'scripts/release-web.py')
release = importlib.util.module_from_spec(spec)
spec.loader.exec_module(release)


class SourceInventoryTest(unittest.TestCase):
    def test_gitignore_keeps_core_runtime(self):
        names = [
            'ports/thkkd/src/core/runtime/CombinedGameHost.hpp',
            'ports/thkkd/runtime/thkkd.wasm',
            'ports/thkkd/assets/dat/title.png',
            'ports/thkkd/content/original.zip',
        ]
        result = subprocess.run(
            ['git', 'check-ignore', '--no-index', '-z', '--stdin'], cwd=release.ROOT,
            input='\0'.join(names).encode(), capture_output=True, check=False)
        self.assertEqual(result.returncode, 0)
        self.assertEqual(result.stdout.decode().rstrip('\0').split('\0'), names[1:])

    def test_core_runtime_is_included_and_build_output_is_excluded(self):
        names = [
            'ports/thkkd/src/core/runtime/CombinedGameHost.hpp',
            'ports/thkkd/web/shell.mjs',
            'launcher/public/assets/favicon.png',
            'launcher/third_party/thkkd-video/ffmpeg-core.wasm',
            'ports/thkkd/runtime/thkkd.wasm',
            'ports/thkkd/build/web/thkkd.wasm',
            'ports/thkkd/userdata/config.txt',
            'ports/thkkd/assets/dat/title.png',
            'ports/thkkd/content/original.zip',
        ]
        with tempfile.TemporaryDirectory() as folder:
            root = Path(folder)
            for name in names:
                path = root / name
                path.parent.mkdir(parents=True, exist_ok=True)
                path.write_bytes(b'test fixture')
            with patch.object(release, 'ROOT', root), patch.object(
                    release.subprocess, 'check_output', return_value='\0'.join(names).encode()):
                selected = {path.relative_to(root).as_posix() for path in release.source_files()}
        self.assertEqual(selected, set(names[:4]))


if __name__ == '__main__':
    unittest.main()
