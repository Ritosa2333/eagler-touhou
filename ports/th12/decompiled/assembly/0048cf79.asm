
// block 0048cf79
0048cf79  push       0x10
0048cf7b  push       0x4ab0c0
0048cf80  call       0x46fcec

// block 0048cf85
0048cf85  xor        esi, esi
0048cf87  mov        dword ptr [ebp - 0x20], esi
0048cf8a  xor        eax, eax
0048cf8c  cmp        dword ptr [ebp + 8], esi
0048cf8f  setne      al
0048cf92  cmp        eax, esi
0048cf94  jne        0x48cfba

// block 0048cf96
0048cf96  call       0x46e982

// block 0048cf9b
0048cf9b  mov        dword ptr [eax], esi
0048cf9d  call       0x46e96f

// block 0048cfa2
0048cfa2  mov        dword ptr [eax], 0x16
0048cfa8  push       esi
0048cfa9  push       esi
0048cfaa  push       esi
0048cfab  push       esi
0048cfac  push       esi
0048cfad  call       0x470f2d

// block 0048cfb2
0048cfb2  add        esp, 0x14
0048cfb5  or         eax, 0xffffffff
0048cfb8  jmp        0x48d009

// block 0048cfba
0048cfba  push       dword ptr [ebp + 8]
0048cfbd  call       0x48eb61

// block 0048cfc2
0048cfc2  mov        dword ptr [ebp - 0x1c], eax
0048cfc5  push       3
0048cfc7  call       0x46ec9a

// block 0048cfcc
0048cfcc  pop        ecx
0048cfcd  pop        ecx
0048cfce  mov        dword ptr [ebp - 4], esi

// block 0048cfd1
0048cfd1  mov        eax, dword ptr [ebp - 0x1c]
0048cfd4  dec        dword ptr [ebp - 0x1c]
0048cfd7  test       eax, eax
0048cfd9  je         0x48cffa

// block 0048cfdb
0048cfdb  mov        eax, dword ptr [ebp + 8]
0048cfde  movzx      eax, word ptr [eax]
0048cfe1  add        dword ptr [ebp + 8], 2
0048cfe5  push       eax
0048cfe6  call       0x48ceb4

// block 0048cfeb
0048cfeb  pop        ecx
0048cfec  mov        ecx, 0xffff
0048cff1  cmp        ax, cx
0048cff4  jne        0x48cfd1

// block 0048cff6
0048cff6  or         dword ptr [ebp - 0x20], 0xffffffff

// block 0048cffa
0048cffa  mov        dword ptr [ebp - 4], 0xfffffffe
0048d001  call       0x48d00f

// block 0048d006
0048d006  mov        eax, dword ptr [ebp - 0x20]

// block 0048d009
0048d009  call       0x46fd31

// block 0048d00e
0048d00e  ret        
