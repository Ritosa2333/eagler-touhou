
// block 0048be53
0048be53  mov        edi, edi
0048be55  push       ebp
0048be56  mov        ebp, esp
0048be58  push       dword ptr [ebp + 0xc]
0048be5b  push       dword ptr [ebp + 8]
0048be5e  call       0x48b89c

// block 0048be63
0048be63  pop        ecx
0048be64  pop        ecx
0048be65  test       eax, eax
0048be67  jne        0x48be71

// block 0048be69
0048be69  cmp        dword ptr [ebp + 8], 0x5f
0048be6d  je         0x48be71

// block 0048be6f
0048be6f  pop        ebp
0048be70  ret        

// block 0048be71
0048be71  xor        eax, eax
0048be73  inc        eax
0048be74  pop        ebp
0048be75  ret        
