
// block 0048e41a
0048e41a  mov        edi, edi
0048e41c  push       ebp
0048e41d  mov        ebp, esp
0048e41f  xor        eax, eax
0048e421  cmp        dword ptr [ebp + 0xc], eax
0048e424  jbe        0x48e435

// block 0048e426
0048e426  mov        ecx, dword ptr [ebp + 8]

// block 0048e429
0048e429  cmp        byte ptr [ecx], 0
0048e42c  je         0x48e435

// block 0048e42e
0048e42e  inc        eax
0048e42f  inc        ecx
0048e430  cmp        eax, dword ptr [ebp + 0xc]
0048e433  jb         0x48e429

// block 0048e435
0048e435  pop        ebp
0048e436  ret        
