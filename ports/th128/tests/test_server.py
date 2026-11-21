import sys
import re
import tempfile
import unittest
from pathlib import Path
from urllib.parse import urljoin

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))

from serve import parse_range, resolve_request


class ServerTests(unittest.TestCase):
    def test_only_the_selected_prepared_bullet_profile_is_public(self):
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary).resolve()
            self.assertEqual(resolve_request('/assets/runtime/bullet-profile-8.bin', root / 'site', root / 'local'),
                             root / 'local/runtime/bullet-profile-8.bin')
            self.assertEqual(resolve_request('/assets/runtime/bullet-profile-18.bin',root/'site',root/'local'),
                             root/'local/runtime/bullet-profile-18.bin')
            self.assertEqual(resolve_request('/assets/runtime/bullet-profile-20.bin',root/'site',root/'local'),
                             root/'local/runtime/bullet-profile-20.bin')
            self.assertEqual(resolve_request('/assets/runtime/sound-bindings.json', root/'site', root/'local'),
                             root/'local/runtime/sound-bindings.json')
            for name in ('th128.exe', 'bullet-profile-8.json', 'enemy-scheduling.json', '../research/bullet-fixtures.json'):
                self.assertIsNone(resolve_request('/assets/runtime/' + name, root / 'site', root / 'local'))

    def test_runtime_routes_expose_only_compiled_core_and_game_resource_types(self):
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary).resolve()
            site, local = root / 'site', root / 'local'
            generation = 'a' * 32
            self.assertEqual(resolve_request('/play.html', site, local), site / 'play.html')
            self.assertEqual(resolve_request('/runtime/core.wasm', site, local), site / 'runtime/core.wasm')
            self.assertEqual(resolve_request('/runtime/replay.mjs', site, local), site / 'runtime/replay.mjs')
            self.assertEqual(resolve_request('/runtime/sound.mjs', site, local), site/'runtime/sound.mjs')
            self.assertEqual(resolve_request('/runtime/build.json', site, local), site / 'runtime/build.json')
            build='b'*64
            self.assertEqual(resolve_request(f'/runtime/builds/{build}/core.wasm', site, local),
                             site / 'runtime/builds' / build / 'core.wasm')
            self.assertIsNone(resolve_request('/runtime/builds/not-a-build/core.wasm', site, local))
            self.assertEqual(resolve_request(f'/assets/generations/{generation}/ja/runtime/title.anm', site, local),
                             local / 'generations' / generation / 'ja/raw/title.anm')
            for name in ('th128.exe', 'th128.dat', 'score.dat', 'th128.cfg', 'arbitrary.py'):
                self.assertIsNone(resolve_request(f'/assets/generations/{generation}/ja/runtime/{name}', site, local))

    def test_only_site_and_public_prepared_assets_are_exposed(self):
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            site, local = root / 'site', root / 'local'
            self.assertEqual(resolve_request('/', site, local), site.resolve() / 'index.html')
            self.assertEqual(resolve_request('/assets/catalog.json', site, local), local.resolve() / 'catalog.json')
            generation = 'a' * 32
            texture = f'/assets/generations/{generation}/ja/textures/title-000.png'
            self.assertEqual(resolve_request(texture, site, local), local.resolve() / texture.removeprefix('/assets/'))
            for path in ('/../README.md', '/%2e%2e/README.md', '/assets/../catalog.json',
                         '/assets/%5c..%5cth128.exe', '/tools/prepare.py', '/assets/last-error.json',
                         f'/assets/generations/{generation}/ja/raw/th128.dat',
                         '/assets/generations/not-a-generation/ja/manifest.json'):
                with self.subTest(path=path):
                    self.assertIsNone(resolve_request(path, site, local))

    def test_original_ending_consumer_routes_are_explicit_and_confined(self):
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary).resolve()
            site, local = root / 'site', root / 'local'
            generation = 'a' * 32
            for module in ('native-ending-browser-host-v4.mjs', 'native-ending-bridge-v6.mjs',
                           'native-ending-pixels.mjs', 'renderer-owned-ending-v2.mjs'):
                with self.subTest(module=module):
                    self.assertEqual(resolve_request('/runtime/' + module, site, local),
                                     site / 'runtime' / module)
            prefix = f'/assets/generations/{generation}/zh/'
            self.assertEqual(resolve_request(prefix + 'native-ending-resources.json', site, local),
                             local / 'generations' / generation / 'zh/native-ending-resources.json')
            self.assertEqual(resolve_request(prefix + 'native-ending-glyphs/e00-line000.a4r4g4b4', site, local),
                             local / 'generations' / generation / 'zh/native-ending-glyphs/e00-line000.a4r4g4b4')
            for suffix in ('native-ending-glyphs/e06-line000.a4r4g4b4',
                           'native-ending-glyphs/e00-line000.exe',
                           'native-ending-glyphs/../score.dat', 'arbitrary.json'):
                with self.subTest(suffix=suffix):
                    self.assertIsNone(resolve_request(prefix + suffix, site, local))
            self.assertIsNone(resolve_request('/runtime/unlisted-ending-module.mjs', site, local))

    def test_current_runtime_name_owner_and_progress_routes_are_explicit(self):
        project = Path(__file__).resolve().parents[1]
        for name in ('last-name-field.mjs', 'status.json'):
            self.assertEqual(resolve_request('/runtime/' + name, project / 'site', project / 'local'),
                             project / 'site/runtime' / name)
        self.assertIsNone(resolve_request('/runtime/unlisted-name-store.mjs', project / 'site', project / 'local'))

    def test_play_bootstrap_relative_imports_all_have_public_routes(self):
        project = Path(__file__).resolve().parents[1]
        pending, visited = ['/play.mjs'], set()
        while pending:
            route = pending.pop()
            if route in visited:
                continue
            visited.add(route)
            source_file = resolve_request(route, project / 'site', project / 'local')
            self.assertIsNotNone(source_file, route)
            self.assertTrue(source_file.is_file(), route)
            source = source_file.read_text(encoding='utf-8')
            dynamic = re.findall(r'import\s*\(\s*["\'](\.[^"\']+)["\']', source)
            static = re.findall(r'(?:import|export)[^;\n]*?\bfrom\s*["\'](\.[^"\']+)["\']', source)
            pending.extend(urljoin(route, name) for name in dynamic + static)
        self.assertIn('/runtime/last-name-field.mjs', visited)

    def test_range_handling_for_music_seeking(self):
        self.assertEqual(parse_range(None, 100), (0, 99, False))
        self.assertEqual(parse_range('bytes=10-20', 100), (10, 20, True))
        self.assertEqual(parse_range('bytes=90-', 100), (90, 99, True))
        self.assertEqual(parse_range('bytes=-10', 100), (90, 99, True))
        self.assertEqual(parse_range('bytes=0-999', 100), (0, 99, True))
        for header in ('bytes=100-', 'bytes=9-3', 'bytes=0-1,4-5', 'other=1-2', 'bytes=-0'):
            with self.assertRaises(ValueError):
                parse_range(header, 100)


if __name__ == '__main__':
    unittest.main()
