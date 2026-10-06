
// block 0048d62f
0048d62f  mov        edi, edi
0048d631  push       ebp
0048d632  mov        ebp, esp
0048d634  sub        esp, 0xc
0048d637  mov        eax, dword ptr [0x4ad138]
0048d63c  xor        eax, ebp
0048d63e  mov        dword ptr [ebp - 4], eax
0048d641  push       6
0048d643  lea        eax, [ebp - 0xc]
0048d646  push       eax
0048d647  push       0x1004
0048d64c  push       dword ptr [ebp + 8]
0048d64f  mov        byte ptr [ebp - 6], 0
0048d653  call       dword ptr [0x4980e0]

// block 0048d659
0048d659  test       eax, eax
0048d65b  jne        0x48d662

// block 0048d65d
0048d65d  or         eax, 0xffffffff
0048d660  jmp        0x48d66c

// block 0048d662
0048d662  lea        eax, [ebp - 0xc]
0048d665  push       eax
0048d666  call       0x46d114

// block 0048d66b
0048d66b  pop        ecx

// block 0048d66c
0048d66c  mov        ecx, dword ptr [ebp - 4]
0048d66f  xor        ecx, ebp
0048d671  call       0x46c932

// block 0048d676
0048d676  leave      
0048d677  ret        
