
// block 00465de0
00465de0  push       ecx
00465de1  push       ebx
00465de2  push       ebp
00465de3  xor        ebp, ebp
00465de5  push       edi
00465de6  xor        edi, edi
00465de8  mov        dword ptr [esi + 0x30], ebp
00465deb  cmp        dword ptr [esi + 0x10], ebp
00465dee  jbe        0x465e11

// block 00465df0
00465df0  mov        eax, dword ptr [esi + 4]
00465df3  cmp        dword ptr [eax + edi*4], ebp
00465df6  lea        eax, [eax + edi*4]
00465df9  je         0x465e0b

// block 00465dfb
00465dfb  mov        eax, dword ptr [eax]
00465dfd  mov        ecx, dword ptr [eax]
00465dff  mov        edx, dword ptr [ecx + 8]
00465e02  push       eax
00465e03  call       edx

// block 00465e05
00465e05  mov        eax, dword ptr [esi + 4]
00465e08  mov        dword ptr [eax + edi*4], ebp

// block 00465e0b
00465e0b  inc        edi
00465e0c  cmp        edi, dword ptr [esi + 0x10]
00465e0f  jb         0x465df0

// block 00465e11
00465e11  mov        eax, dword ptr [esi + 4]
00465e14  cmp        eax, ebp
00465e16  je         0x465e24

// block 00465e18
00465e18  push       eax
00465e19  call       0x46ca4f

// block 00465e1e
00465e1e  add        esp, 4
00465e21  mov        dword ptr [esi + 4], ebp

// block 00465e24
00465e24  mov        eax, dword ptr [esi + 0x10]
00465e27  xor        ecx, ecx
00465e29  mov        edx, 4
00465e2e  mul        edx
00465e30  seto       cl
00465e33  mov        dword ptr [esp + 0xc], ebp
00465e37  neg        ecx
00465e39  or         ecx, eax
00465e3b  push       ecx
00465e3c  call       0x46c9ea

// block 00465e41
00465e41  add        esp, 4
00465e44  xor        ebx, ebx
00465e46  mov        dword ptr [esi + 4], eax
00465e49  cmp        dword ptr [esi + 0x10], ebp
00465e4c  jbe        0x465f09

// block 00465e52
00465e52  lea        ebp, [esi + 0x38]

// block 00465e55
00465e55  mov        edx, dword ptr [esi + 4]
00465e58  mov        eax, dword ptr [esi + 0x5c]
00465e5b  mov        eax, dword ptr [eax]
00465e5d  mov        ecx, dword ptr [eax]
00465e5f  push       0
00465e61  lea        edi, [ebx*4]
00465e68  add        edx, edi

// block 00465f09
00465f09  pop        edi
00465f0a  pop        ebp
00465f0b  xor        eax, eax
00465f0d  pop        ebx
00465f0e  pop        ecx
00465f0f  ret        
