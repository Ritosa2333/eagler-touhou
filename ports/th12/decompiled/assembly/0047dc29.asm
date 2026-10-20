
// block 0047dc29
0047dc29  mov        edi, edi
0047dc2b  push       ebp
0047dc2c  mov        ebp, esp
0047dc2e  push       esi
0047dc2f  push       edi
0047dc30  mov        edi, dword ptr [ebp + 8]
0047dc33  add        edi, 7
0047dc36  and        edi, 0xfffffff8
0047dc39  cmp        dword ptr [ebp + 0xc], 0
0047dc3d  mov        esi, ecx
0047dc3f  je         0x47dc47

// block 0047dc41
0047dc41  push       edi
0047dc42  call       dword ptr [esi]

// block 0047dc44
0047dc44  pop        ecx
0047dc45  jmp        0x47dcab

// block 0047dc47
0047dc47  test       edi, edi
0047dc49  ja         0x47dc4e

// block 0047dc4b
0047dc4b  push       8
0047dc4d  pop        edi

// block 0047dc4e
0047dc4e  mov        eax, dword ptr [esi + 0x10]
0047dc51  push       ebx
0047dc52  cmp        eax, edi
0047dc54  jae        0x47dc9b

// block 0047dc56
0047dc56  mov        ebx, 0x1000
0047dc5b  cmp        edi, ebx
0047dc5d  ja         0x47dc97

// block 0047dc5f
0047dc5f  push       1
0047dc61  push       0x1004
0047dc66  mov        ecx, 0x4b42f8
0047dc6b  call       0x47dc29

// block 0047dc70
0047dc70  test       eax, eax
0047dc72  je         0x47dc79

// block 0047dc74
0047dc74  and        dword ptr [eax], 0
0047dc77  jmp        0x47dc7b

// block 0047dc79
0047dc79  xor        eax, eax

// block 0047dc7b
0047dc7b  test       eax, eax
0047dc7d  je         0x47dc97

// block 0047dc7f
0047dc7f  mov        ecx, dword ptr [esi + 0xc]
0047dc82  test       ecx, ecx
0047dc84  je         0x47dc8a

// block 0047dc86
0047dc86  mov        dword ptr [ecx], eax
0047dc88  jmp        0x47dc8d

// block 0047dc8a
0047dc8a  mov        dword ptr [esi + 8], eax

// block 0047dc8d
0047dc8d  sub        ebx, edi
0047dc8f  mov        dword ptr [esi + 0xc], eax
0047dc92  mov        dword ptr [esi + 0x10], ebx
0047dc95  jmp        0x47dca0

// block 0047dc97
0047dc97  xor        eax, eax
0047dc99  jmp        0x47dcaa

// block 0047dc9b
0047dc9b  sub        eax, edi
0047dc9d  mov        dword ptr [esi + 0x10], eax

// block 0047dca0
0047dca0  mov        eax, dword ptr [esi + 0xc]
0047dca3  mov        ecx, dword ptr [esi + 0x10]
0047dca6  lea        eax, [eax + ecx + 4]

// block 0047dcaa
0047dcaa  pop        ebx

// block 0047dcab
0047dcab  pop        edi
0047dcac  pop        esi
0047dcad  pop        ebp
0047dcae  ret        8
