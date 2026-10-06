
// block 0048d3e9
0048d3e9  push       0x10
0048d3eb  push       0x4ab168
0048d3f0  call       0x46fcec

// block 0048d3f5
0048d3f5  call       0x47c025

// block 0048d3fa
0048d3fa  mov        esi, eax
0048d3fc  add        esi, 0x20
0048d3ff  mov        dword ptr [ebp - 0x1c], esi
0048d402  xor        eax, eax
0048d404  xor        edi, edi
0048d406  cmp        dword ptr [ebp + 0xc], edi
0048d409  setne      al
0048d40c  cmp        eax, edi
0048d40e  jne        0x48d42d

// block 0048d410
0048d410  call       0x46e96f

// block 0048d415
0048d415  mov        dword ptr [eax], 0x16
0048d41b  push       edi
0048d41c  push       edi
0048d41d  push       edi
0048d41e  push       edi
0048d41f  push       edi
0048d420  call       0x470f2d

// block 0048d425
0048d425  add        esp, 0x14
0048d428  or         eax, 0xffffffff
0048d42b  jmp        0x48d468

// block 0048d42d
0048d42d  push       esi
0048d42e  call       0x47c0fc

// block 0048d433
0048d433  pop        ecx
0048d434  mov        dword ptr [ebp - 4], edi
0048d437  push       esi
0048d438  call       0x48d319

// block 0048d43d
0048d43d  mov        edi, eax
0048d43f  push       dword ptr [ebp + 0x14]
0048d442  push       dword ptr [ebp + 0x10]
0048d445  push       dword ptr [ebp + 0xc]
0048d448  push       esi
0048d449  call       dword ptr [ebp + 8]

// block 0048d44c
0048d44c  mov        dword ptr [ebp - 0x20], eax
0048d44f  push       esi
0048d450  push       edi
0048d451  call       0x48d3b5

// block 0048d456
0048d456  add        esp, 0x1c
0048d459  mov        dword ptr [ebp - 4], 0xfffffffe
0048d460  call       0x48d471

// block 0048d465
0048d465  mov        eax, dword ptr [ebp - 0x20]

// block 0048d468
0048d468  call       0x46fd31

// block 0048d46d
0048d46d  ret        
