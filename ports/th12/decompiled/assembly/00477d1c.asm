
// block 00477d1c
00477d1c  mov        edi, edi
00477d1e  push       ebp
00477d1f  mov        ebp, esp
00477d21  movzx      eax, byte ptr [ebp + 8]
00477d25  push       eax
00477d26  call       0x48ba71

// block 00477d2b
00477d2b  test       eax, eax
00477d2d  movsx      eax, byte ptr [ebp + 8]
00477d31  pop        ecx
00477d32  jne        0x477d3a

// block 00477d34
00477d34  and        eax, 0xffffffdf
00477d37  sub        eax, 7

// block 00477d3a
00477d3a  pop        ebp
00477d3b  ret        
