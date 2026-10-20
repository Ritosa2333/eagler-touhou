
// block 004516a0
004516a0  sub        esp, 0x114
004516a6  mov        eax, dword ptr [0x4ad138]
004516ab  xor        eax, esp
004516ad  mov        dword ptr [esp + 0x110], eax
004516b4  push       0x105
004516b9  lea        eax, [esp + 0xc]
004516bd  push       eax
004516be  push       0
004516c0  call       dword ptr [0x4980b4]

// block 004516c6
004516c6  test       eax, eax
004516c8  je         0x45177e

// block 004516ce
004516ce  push       ebx
004516cf  push       1
004516d1  lea        ecx, [esp + 8]
004516d5  push       ecx
004516d6  lea        eax, [esp + 0x14]
004516da  xor        ebx, ebx
004516dc  call       0x463c10

// block 004516e1
004516e1  mov        ecx, eax
004516e3  mov        dword ptr [esp + 8], ecx
004516e7  test       ecx, ecx
004516e9  jne        0x451704

// block 004516eb
004516eb  or         eax, 0xffffffff
004516ee  pop        ebx
004516ef  mov        ecx, dword ptr [esp + 0x110]
004516f6  xor        ecx, esp
004516f8  call       0x46c932

// block 004516fd
004516fd  add        esp, 0x114
00451703  ret        

// block 00451704
00451704  mov        eax, dword ptr [esp + 4]
00451708  cdq        
00451709  and        edx, 3
0045170c  push       ebp
0045170d  add        eax, edx
0045170f  push       esi
00451710  sar        eax, 2
00451713  push       edi
00451714  dec        eax
00451715  xor        esi, esi
00451717  xor        edi, edi
00451719  xor        ebp, ebp
0045171b  cmp        eax, 2
0045171e  jl         0x45173d

// block 00451720
00451720  lea        edx, [eax - 2]
00451723  shr        edx, 1
00451725  inc        edx
00451726  lea        ebp, [edx + edx]
00451729  lea        esp, [esp]

// block 00451730
00451730  add        esi, dword ptr [ecx]
00451732  add        edi, dword ptr [ecx + 4]
00451735  add        ecx, 8
00451738  sub        edx, 1
0045173b  jne        0x451730

// block 0045173d
0045173d  cmp        ebp, eax
0045173f  jge        0x451743

// block 00451741
00451741  mov        ebx, dword ptr [ecx]

// block 00451743
00451743  mov        edx, dword ptr [esp + 0x14]
00451747  add        edi, esi
00451749  push       edx
0045174a  add        ebx, edi
0045174c  call       0x46c941

// block 00451751
00451751  mov        eax, dword ptr [esp + 0x14]
00451755  add        esp, 4
00451758  pop        edi
00451759  pop        esi
0045175a  mov        dword ptr [0x4cf284], eax
0045175f  pop        ebp
00451760  mov        dword ptr [0x4cf280], ebx
00451766  mov        eax, ebx
00451768  pop        ebx
00451769  mov        ecx, dword ptr [esp + 0x110]
00451770  xor        ecx, esp
00451772  call       0x46c932

// block 00451777
00451777  add        esp, 0x114
0045177d  ret        

// block 0045177e
0045177e  mov        ecx, dword ptr [esp + 0x110]
00451785  xor        ecx, esp
00451787  or         eax, 0xffffffff
0045178a  call       0x46c932

// block 0045178f
0045178f  add        esp, 0x114
00451795  ret        
