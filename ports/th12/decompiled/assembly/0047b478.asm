
// block 0047b478
0047b478  push       ebx
0047b479  push       esi
0047b47a  push       edi
0047b47b  mov        edx, dword ptr [esp + 0x10]
0047b47f  mov        eax, dword ptr [esp + 0x14]
0047b483  mov        ecx, dword ptr [esp + 0x18]
0047b487  push       ebp
0047b488  push       edx
0047b489  push       eax
0047b48a  push       ecx
0047b48b  push       ecx
0047b48c  push       0x47b508
0047b491  push       dword ptr fs:[0]
0047b498  mov        eax, dword ptr [0x4ad138]
0047b49d  xor        eax, esp
0047b49f  mov        dword ptr [esp + 8], eax
0047b4a3  mov        dword ptr fs:[0], esp

// block 0047b4aa
0047b4aa  mov        eax, dword ptr [esp + 0x30]
0047b4ae  mov        ebx, dword ptr [eax + 8]
0047b4b1  mov        ecx, dword ptr [esp + 0x2c]
0047b4b5  xor        ebx, dword ptr [ecx]
0047b4b7  mov        esi, dword ptr [eax + 0xc]
0047b4ba  cmp        esi, -2
0047b4bd  je         0x47b4fa

// block 0047b4bf
0047b4bf  mov        edx, dword ptr [esp + 0x34]
0047b4c3  cmp        edx, -2
0047b4c6  je         0x47b4cc

// block 0047b4c8
0047b4c8  cmp        esi, edx
0047b4ca  jbe        0x47b4fa

// block 0047b4cc
0047b4cc  lea        esi, [esi + esi*2]
0047b4cf  lea        ebx, [ebx + esi*4 + 0x10]
0047b4d3  mov        ecx, dword ptr [ebx]
0047b4d5  mov        dword ptr [eax + 0xc], ecx
0047b4d8  cmp        dword ptr [ebx + 4], 0
0047b4dc  jne        0x47b4aa

// block 0047b4de
0047b4de  push       0x101
0047b4e3  mov        eax, dword ptr [ebx + 8]
0047b4e6  call       0x48c99d

// block 0047b4eb
0047b4eb  mov        ecx, 1
0047b4f0  mov        eax, dword ptr [ebx + 8]
0047b4f3  call       0x48c9bc

// block 0047b4f8
0047b4f8  jmp        0x47b4aa

// block 0047b4fa
0047b4fa  pop        dword ptr fs:[0]
0047b501  add        esp, 0x18
0047b504  pop        edi
0047b505  pop        esi
0047b506  pop        ebx
0047b507  ret        
