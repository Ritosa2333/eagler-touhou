
// block 00477d65
00477d65  mov        edi, edi
00477d67  push       ebp
00477d68  mov        ebp, esp
00477d6a  push       ebx

// block 00477d6b
00477d6b  mov        edx, dword ptr [ebp + 8]
00477d6e  inc        dword ptr [esi]
00477d70  call       0x477d3c

// block 00477d75
00477d75  mov        ebx, eax
00477d77  cmp        ebx, -1
00477d7a  je         0x477d8a

// block 00477d7c
00477d7c  movzx      eax, bl
00477d7f  push       eax
00477d80  call       0x48bb76

// block 00477d85
00477d85  pop        ecx
00477d86  test       eax, eax
00477d88  jne        0x477d6b

// block 00477d8a
00477d8a  mov        eax, ebx
00477d8c  pop        ebx
00477d8d  pop        ebp
00477d8e  ret        
