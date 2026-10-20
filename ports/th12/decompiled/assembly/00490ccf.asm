
// block 00490ccf
00490ccf  mov        edi, edi
00490cd1  push       ebp
00490cd2  mov        ebp, esp
00490cd4  push       esi
00490cd5  mov        esi, dword ptr [0x4b3d80]
00490cdb  jmp        0x490cfe

// block 00490cdd
00490cdd  push       edi
00490cde  push       eax
00490cdf  push       dword ptr [ebp + 8]
00490ce2  call       0x48e329

// block 00490ce7
00490ce7  add        esp, 0xc
00490cea  test       eax, eax
00490cec  jne        0x490cfb

// block 00490cee
00490cee  mov        eax, dword ptr [esi]
00490cf0  mov        al, byte ptr [edi + eax]
00490cf3  cmp        al, 0x3d
00490cf5  je         0x490d14

// block 00490cf7
00490cf7  test       al, al
00490cf9  je         0x490d14

// block 00490cfb
00490cfb  add        esi, 4

// block 00490cfe
00490cfe  mov        eax, dword ptr [esi]
00490d00  test       eax, eax
00490d02  jne        0x490cdd

// block 00490d04
00490d04  mov        eax, esi
00490d06  sub        eax, dword ptr [0x4b3d80]
00490d0c  sar        eax, 2
00490d0f  neg        eax

// block 00490d11
00490d11  pop        esi
00490d12  pop        ebp
00490d13  ret        

// block 00490d14
00490d14  mov        eax, esi
00490d16  sub        eax, dword ptr [0x4b3d80]
00490d1c  sar        eax, 2
00490d1f  jmp        0x490d11
