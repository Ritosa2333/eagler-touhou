import importlib.util
from pathlib import Path
import struct
import unittest

spec = importlib.util.spec_from_file_location('config', Path(__file__).with_name('windows-retail-config.py'))
config = importlib.util.module_from_spec(spec)
spec.loader.exec_module(config)


def fixture(game):
    data = bytearray(56)
    struct.pack_into('<I', data, 20, 0x102 if game == 6 else 0x70002)
    data[30 if game == 6 else 34] = 1
    return data


class WindowedConfigTest(unittest.TestCase):
    def test_valid_windowed(self):
        for game in (6, 7):
            config.validate_config(game, fixture(game))

    def test_fullscreen_is_rejected(self):
        for game in (6, 7):
            data = fixture(game)
            data[30 if game == 6 else 34] = 0
            with self.assertRaises(ValueError):
                config.validate_config(game, data)

    def test_corrupt_other_option_is_rejected_even_when_windowed(self):
        for game in (6, 7):
            data = fixture(game)
            data[24 if game == 6 else 28] = 5
            with self.assertRaises(ValueError):
                config.validate_config(game, data)

    def test_wrong_version_or_truncated_config_is_rejected(self):
        for data in (b'', fixture(7), fixture(6)[:-1]):
            with self.assertRaises(ValueError):
                config.validate_config(6, data)

    def test_filename_uses_windows_ansi_interpretation(self):
        self.assertEqual(config.config_name(6, 932), '東方紅魔郷.cfg')
        self.assertNotEqual(config.config_name(6, 65001), '東方紅魔郷.cfg')
        self.assertEqual(config.config_name(7, 65001), 'th07.cfg')


if __name__ == '__main__':
    unittest.main()
