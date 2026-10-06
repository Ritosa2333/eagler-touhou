
// block 00453f10
00453f10  push       ecx
00453f11  push       esi
00453f12  mov        esi, eax
00453f14  test       esi, esi
00453f16  jge        0x453f5c

// block 00453f18
00453f18  mov        esi, 0x4d0e70
00453f1d  lea        ecx, [ecx]

// block 00453f20
00453f20  mov        eax, dword ptr [esi]
00453f22  mov        dword ptr [esi + 0x14], 0
00453f29  test       eax, eax
00453f2b  je         0x453f4e

// block 00453f2d
00453f2d  mov        ecx, dword ptr [eax]
00453f2f  lea        edx, [esp + 4]
00453f33  push       edx
00453f34  push       eax
00453f35  mov        eax, dword ptr [ecx + 0x24]
00453f38  call       eax

// block 00453f3a
00453f3a  mov        ecx, dword ptr [esp + 4]
00453f3e  mov        eax, dword ptr [esi]
00453f40  and        ecx, 1
00453f43  mov        dword ptr [esi + 0x14], ecx
00453f46  mov        edx, dword ptr [eax]
00453f48  push       eax
00453f49  mov        eax, dword ptr [edx + 0x48]
00453f4c  call       eax

// block 00453f4e
00453f4e  add        esi, 0x18
00453f51  cmp        esi, 0x4d1410
00453f57  jl         0x453f20

// block 00453f59
00453f59  pop        esi
00453f5a  pop        ecx
00453f5b  ret        

// block 00453f5c
00453f5c  xor        edx, edx
00453f5e  mov        eax, 0x4cf508

// block 00453f63
00453f63  mov        ecx, dword ptr [eax]
00453f65  test       ecx, ecx
00453f67  jl         0x453f7b

// block 00453f69
00453f69  cmp        ecx, esi
00453f6b  je         0x453f87

// block 00453f6d
00453f6d  add        eax, 4
00453f70  inc        edx
00453f71  cmp        eax, 0x4cf538
00453f76  jl         0x453f63

// block 00453f78
00453f78  pop        esi
00453f79  pop        ecx
00453f7a  ret        

// block 00453f7b
00453f7b  cmp        edx, 0xc
00453f7e  jge        0x453f92

// block 00453f80
00453f80  mov        dword ptr [edx*4 + 0x4cf508], esi

// block 00453f87
00453f87  mov        dword ptr [edx*4 + 0x4cf538], 0xffffffff

// block 00453f92
00453f92  pop        esi
00453f93  pop        ecx
00453f94  ret        
