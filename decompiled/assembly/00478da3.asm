
// block 00478da3
00478da3  mov        edi, edi
00478da5  push       ebp
00478da6  mov        ebp, esp
00478da8  movzx      eax, byte ptr [ebp + 8]
00478dac  push       eax
00478dad  call       0x48ba71

// block 00478db2
00478db2  test       eax, eax
00478db4  movsx      eax, byte ptr [ebp + 8]
00478db8  pop        ecx
00478db9  jne        0x478dc1

// block 00478dbb
00478dbb  and        eax, 0xffffffdf
00478dbe  sub        eax, 7

// block 00478dc1
00478dc1  pop        ebp
00478dc2  ret        
