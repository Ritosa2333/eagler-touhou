
// block 0041d020
0041d020  push       ebx
0041d021  push       ebp
0041d022  mov        ebp, dword ptr [0x4b4514]
0041d028  push       esi
0041d029  push       edi
0041d02a  mov        edi, 8
0041d02f  or         dword ptr [ebp + 0xc414], edi
0041d035  lea        esi, [ebp + 0x8314]
0041d03b  jmp        0x41d040

// block 0041d040
0041d040  mov        eax, dword ptr [esi - 4]
0041d043  push       eax
0041d044  mov        ebx, 3
0041d049  call       0x461970

// block 0041d04e
0041d04e  mov        ecx, dword ptr [esi]
0041d050  push       ecx
0041d051  call       0x461970

// block 0041d056
0041d056  add        esi, 0xe4
0041d05c  sub        edi, 1
0041d05f  jne        0x41d040

// block 0041d061
0041d061  mov        dword ptr [ebp + 0xc418], edi
0041d067  pop        edi
0041d068  pop        esi
0041d069  pop        ebp
0041d06a  pop        ebx
0041d06b  ret        
