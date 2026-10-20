
// block 00401000
00401000  lea        ecx, [esi + 0x14]
00401003  mov        dword ptr [esi], 0x49f4b8
00401009  call       0x4027e0

// block 0040100e
0040100e  lea        ecx, [esi + 0x4c8]
00401014  call       0x4027e0

// block 00401019
00401019  push       0x18fc4
0040101e  push       0
00401020  push       esi
00401021  call       0x477420

// block 00401026
00401026  fld1       
00401028  or         dword ptr [esi + 4], 2
0040102c  fst        dword ptr [esi + 0x18f84]
00401032  mov        eax, 1
00401037  fstp       dword ptr [esi + 0x18f88]
0040103d  mov        dword ptr [esi + 0x18fa4], eax
00401043  mov        dword ptr [esi + 0x18fa8], eax
00401049  add        esp, 0xc
0040104c  mov        dword ptr [0x4b43b8], esi
00401052  mov        dword ptr [esi + 0x18f80], 0xffffffff
0040105c  mov        dword ptr [esi + 0x18f90], 0
00401066  mov        dword ptr [esi + 0x18fac], 9
00401070  mov        eax, esi
00401072  ret        
