import importlib.util
import struct
import unittest
from pathlib import Path
from profile import prepare_config,capture,replay_identity
from window_capture import encode_png

class Memory:
    def __init__(self):self.data={}
    def put(self,address,fmt,*values):self.data[address]=struct.pack('<'+fmt,*values)
    def unpack(self,fmt,address):return struct.unpack(fmt,self.data[address])
    def read(self,address,length):return self.data[address][:length]

class ProfileTests(unittest.TestCase):
    def test_config_preserves_all_other_options(self):
        original=bytearray(60);struct.pack_into('<I',original,0,0x120001)
        original[29]=0;original[30]=2;original[40:44]=b'abcd'
        configured=prepare_config(original)
        self.assertEqual([i for i in range(60) if original[i]!=configured[i]],[29,30])
        self.assertEqual(configured[29:31],b'\1\0')
    def test_invalid_config_refuses_retail_fullscreen_fallback(self):
        for data in (bytes(60),bytes(56),struct.pack('<I',0x120001)+bytes(22)+bytes([2])+bytes(33)):
            with self.assertRaises(ValueError):prepare_config(data)
    def test_layout_keeps_raw_and_display_separate(self):
        m=Memory()
        for address,value in [(0x4b0cb0,6),(0x4b0c44,11575724),(0x4b0c78,1892030),
                (0x4b0cdc,4596),(0x4b0c48,306)]:m.put(address,'i',value)
        m.put(0x4ce568,'II',55179,195958)
        for address,ptr in [(0x4b4514,0x1000000),(0x4b43e4,0x2000000),
                (0x4b43cc,0x3000000),(0x4b4518,0x4000000),(0x4b43dc,0)]:m.put(address,'I',ptr)
        m.put(0x4000000+0x10,'i',1);m.put(0x4000000+6*36+0xbc,'i',14637)
        m.put(0x4000000+0x1d0,'i',14637)
        m.put(0x2000000+0x6cdc,'i',11575720);m.put(0x2000000+0x6ce0,'i',1)
        m.put(0x1000000+0x988,'ii',-871,48037)
        for offset,value in [(0xa28,1),(0xa34,152),(0xa48,14637),(0xc404,0)]:m.put(0x1000000+offset,'i',value)
        m.put(0x3000000+0x78,'i',90);m.put(0x3000000+0x7c,'I',1);m.put(0x3000000+0x28,'i',1933)
        row=capture(m)
        self.assertEqual(row['replayCursor'],14637)
        self.assertEqual(row['scoreStored'],11575724)
        self.assertEqual(row['displayScoreStored'],11575720)
        self.assertEqual(row['spell'],{'card':90,'flags':1,'age':1933})
        self.assertEqual(row['playerFixed'],[-871,48037])
    def test_capture_menu_without_manager_dereferences(self):
        m=Memory()
        for address in (0x4b0cb0,0x4b0c44,0x4b0c78,0x4b0cdc,0x4b0c48):m.put(address,'i',0)
        for address in (0x4b4514,0x4b43e4,0x4b43cc,0x4b4518,0x4b43dc):m.put(address,'I',0)
        m.put(0x4ce568,'II',123,0)
        self.assertIsNone(capture(m)['replayCursor'])
    def test_lossless_png_of_captured_bgrx(self):
        import zlib
        png=encode_png(2,1,b'\0\0\xff\0\0\xff\0\0')
        self.assertTrue(png.startswith(b'\x89PNG\r\n\x1a\n'))
        at=8;decoded=None
        while at<len(png):
            n=struct.unpack_from('>I',png,at)[0]
            if png[at+4:at+8]==b'IDAT':decoded=zlib.decompress(png[at+8:at+8+n])
            at+=n+12
        self.assertEqual(decoded,b'\0\xff\0\0\0\xff\0')
    def test_replay_identity_covers_header_input_and_fps_payload(self):
        import hashlib
        m=Memory();m.put(0x4b4518,'I',0x2000000)
        m.put(0x2000000+6*36+0xa8,'I',0x3000000);m.put(0x2000000+6*36+0xb8,'I',0x4000000)
        header=bytearray(160);struct.pack_into('<HHII',header,0,6,42,2,13)
        m.data[0x4000000]=bytes(header);m.data[0x3000000]=bytes(range(13))
        identity=replay_identity(m,6)
        self.assertEqual(identity,{'stage':6,'frames':2,'stageSha256':hashlib.sha256(bytes(header)+bytes(range(13))).hexdigest()})
        m.data[0x3000000]=bytes(range(12))+b'\xff'
        self.assertNotEqual(replay_identity(m,6)['stageSha256'],identity['stageSha256'])

if __name__=='__main__':unittest.main()
