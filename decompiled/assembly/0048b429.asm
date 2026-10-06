
// block 0048b429
0048b429  push       0xc
0048b42b  push       0x4aafd8
0048b430  call       0x46fcec

// block 0048b435
0048b435  and        dword ptr [ebp - 4], 0
0048b439  movapd     xmm0, xmm1
0048b43d  mov        dword ptr [ebp - 0x1c], 1
0048b444  jmp        0x48b469

// block 0048b469
0048b469  mov        dword ptr [ebp - 4], 0xfffffffe
0048b470  mov        eax, dword ptr [ebp - 0x1c]
0048b473  call       0x46fd31

// block 0048b478
0048b478  ret        
