
// block 00412314
00412314  push       esi
00412315  call       0x477420

// block 0041231a
0041231a  fld        dword ptr [0x4a3d78]
00412320  fst        dword ptr [esi + 0x54]
00412323  add        esp, 0xc
00412326  fstp       dword ptr [esi + 0x50]
00412329  mov        dword ptr [esi], 0
0041232f  ret        
