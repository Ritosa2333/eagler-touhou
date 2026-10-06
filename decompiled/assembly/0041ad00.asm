
// block 0041ad00
0041ad00  mov        eax, dword ptr [0x4b43dc]
0041ad05  mov        eax, dword ptr [eax + 0x68]
0041ad08  test       eax, eax
0041ad0a  je         0x41ad81

// block 0041ad0c
0041ad0c  push       ebx
0041ad0d  push       ebp
0041ad0e  push       esi
0041ad0f  push       edi
0041ad10  mov        ebx, 0xfffe7961

// block 0041ad15
0041ad15  mov        ecx, dword ptr [eax]
0041ad17  mov        ebp, dword ptr [eax + 4]
0041ad1a  add        ecx, 0x1040
0041ad20  test       dword ptr [ecx + 0x16b8], 0x400
0041ad2a  je         0x41ad77

// block 0041ad2c
0041ad2c  test       byte ptr [ecx + 0x161c], 1
0041ad33  je         0x41ad71

// block 0041ad35
0041ad35  mov        esi, dword ptr [ecx + 0x1618]
0041ad3b  add        dword ptr [ecx + 0x1614], ebx
0041ad41  mov        eax, dword ptr [ecx + 0x1614]
0041ad47  lea        edx, [esi*8]
0041ad4e  sub        edx, esi
0041ad50  sub        eax, edx
0041ad52  mov        edi, eax
0041ad54  mov        eax, 0x92492493
0041ad59  imul       edi
0041ad5b  add        edx, edi
0041ad5d  sar        edx, 2
0041ad60  mov        eax, edx
0041ad62  shr        eax, 0x1f
0041ad65  add        eax, edx
0041ad67  add        eax, esi
0041ad69  mov        dword ptr [ecx + 0x1608], eax
0041ad6f  jmp        0x41ad77

// block 0041ad71
0041ad71  add        dword ptr [ecx + 0x1608], ebx

// block 0041ad77
0041ad77  mov        eax, ebp
0041ad79  test       ebp, ebp
0041ad7b  jne        0x41ad15

// block 0041ad7d
0041ad7d  pop        edi
0041ad7e  pop        esi
0041ad7f  pop        ebp
0041ad80  pop        ebx

// block 0041ad81
0041ad81  xor        eax, eax
0041ad83  ret        
