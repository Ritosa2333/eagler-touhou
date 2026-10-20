
// block 00423020
00423020  push       ecx
00423021  push       esi
00423022  mov        dword ptr [0x4cf508], 0xffffffff
0042302c  mov        esi, 0x4d0e70

// block 00423031
00423031  mov        eax, dword ptr [esi]
00423033  mov        dword ptr [esi + 0x14], 0
0042303a  test       eax, eax
0042303c  je         0x42305f

// block 0042303e
0042303e  mov        ecx, dword ptr [eax]
00423040  lea        edx, [esp + 4]
00423044  push       edx
00423045  push       eax
00423046  mov        eax, dword ptr [ecx + 0x24]
00423049  call       eax

// block 0042304b
0042304b  mov        ecx, dword ptr [esp + 4]
0042304f  mov        eax, dword ptr [esi]
00423051  and        ecx, 1
00423054  mov        dword ptr [esi + 0x14], ecx
00423057  mov        edx, dword ptr [eax]
00423059  push       eax
0042305a  mov        eax, dword ptr [edx + 0x48]
0042305d  call       eax

// block 0042305f
0042305f  add        esi, 0x18
00423062  cmp        esi, 0x4d1410
00423068  jl         0x423031

// block 0042306a
0042306a  pop        esi
0042306b  pop        ecx
0042306c  ret        
