
// block 00490d21
00490d21  mov        edi, edi
00490d23  push       ebp
00490d24  mov        ebp, esp
00490d26  push       ecx
00490d27  push       edi
00490d28  mov        edi, eax
00490d2a  xor        eax, eax
00490d2c  mov        ecx, edi
00490d2e  test       edi, edi
00490d30  je         0x490d78

// block 00490d32
00490d32  cmp        dword ptr [edi], eax
00490d34  je         0x490d3f

// block 00490d36
00490d36  add        ecx, 4
00490d39  inc        eax
00490d3a  cmp        dword ptr [ecx], 0
00490d3d  jne        0x490d36

// block 00490d3f
00490d3f  push       esi
00490d40  inc        eax
00490d41  push       4
00490d43  push       eax
00490d44  call       0x477813

// block 00490d49
00490d49  mov        esi, eax
00490d4b  pop        ecx
00490d4c  pop        ecx
00490d4d  mov        dword ptr [ebp - 4], esi
00490d50  test       esi, esi
00490d52  jne        0x490d6c

// block 00490d54
00490d54  push       9
00490d56  call       0x472c0d

// block 00490d5b
00490d5b  jmp        0x490d6b

// block 00490d5d
00490d5d  push       eax
00490d5e  call       0x49132c

// block 00490d63
00490d63  mov        dword ptr [esi], eax
00490d65  add        esi, 4
00490d68  add        edi, 4

// block 00490d6b
00490d6b  pop        ecx

// block 00490d6c
00490d6c  mov        eax, dword ptr [edi]
00490d6e  test       eax, eax
00490d70  jne        0x490d5d

// block 00490d72
00490d72  and        dword ptr [esi], eax
00490d74  mov        eax, dword ptr [ebp - 4]
00490d77  pop        esi

// block 00490d78
00490d78  pop        edi
00490d79  leave      
00490d7a  ret        
