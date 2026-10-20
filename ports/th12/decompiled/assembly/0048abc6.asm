
// block 0048abc6
0048abc6  mov        edi, edi
0048abc8  push       ebp
0048abc9  mov        ebp, esp
0048abcb  mov        eax, dword ptr [ebp + 0x14]
0048abce  cmp        eax, 0x65
0048abd1  je         0x48ac32

// block 0048abd3
0048abd3  cmp        eax, 0x45
0048abd6  je         0x48ac32

// block 0048abd8
0048abd8  cmp        eax, 0x66
0048abdb  jne        0x48abf6

// block 0048abdd
0048abdd  push       dword ptr [ebp + 0x20]
0048abe0  push       dword ptr [ebp + 0x18]
0048abe3  push       dword ptr [ebp + 0x10]
0048abe6  push       dword ptr [ebp + 0xc]
0048abe9  push       dword ptr [ebp + 8]
0048abec  call       0x48a9d4

// block 0048abf1
0048abf1  add        esp, 0x14
0048abf4  pop        ebp
0048abf5  ret        

// block 0048abf6
0048abf6  cmp        eax, 0x61
0048abf9  je         0x48ac19

// block 0048abfb
0048abfb  cmp        eax, 0x41
0048abfe  je         0x48ac19

// block 0048ac00
0048ac00  push       dword ptr [ebp + 0x20]
0048ac03  push       dword ptr [ebp + 0x1c]
0048ac06  push       dword ptr [ebp + 0x18]
0048ac09  push       dword ptr [ebp + 0x10]
0048ac0c  push       dword ptr [ebp + 0xc]
0048ac0f  push       dword ptr [ebp + 8]
0048ac12  call       0x48aaac

// block 0048ac17
0048ac17  jmp        0x48ac49

// block 0048ac19
0048ac19  push       dword ptr [ebp + 0x20]
0048ac1c  push       dword ptr [ebp + 0x1c]
0048ac1f  push       dword ptr [ebp + 0x18]
0048ac22  push       dword ptr [ebp + 0x10]
0048ac25  push       dword ptr [ebp + 0xc]
0048ac28  push       dword ptr [ebp + 8]
0048ac2b  call       0x48a54a

// block 0048ac30
0048ac30  jmp        0x48ac49

// block 0048ac32
0048ac32  push       dword ptr [ebp + 0x20]
0048ac35  push       dword ptr [ebp + 0x1c]
0048ac38  push       dword ptr [ebp + 0x18]
0048ac3b  push       dword ptr [ebp + 0x10]
0048ac3e  push       dword ptr [ebp + 0xc]
0048ac41  push       dword ptr [ebp + 8]
0048ac44  call       0x48a45a

// block 0048ac49
0048ac49  add        esp, 0x18
0048ac4c  pop        ebp
0048ac4d  ret        
