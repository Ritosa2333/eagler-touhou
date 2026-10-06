
// block 0040cd00
0040cd00  push       ebx
0040cd01  push       ebp
0040cd02  mov        ebp, dword ptr [esp + 0xc]
0040cd06  push       esi
0040cd07  mov        esi, dword ptr [0x4b43c8]
0040cd0d  push       edi
0040cd0e  mov        ebx, eax
0040cd10  add        esi, 0x64
0040cd13  mov        dword ptr [esp + 0x14], 0x7d0
0040cd1b  jmp        0x40cd20

// block 0040cd20
0040cd20  movzx      eax, word ptr [esi + 0x532]
0040cd27  fld        qword ptr [0x4a3cf8]
0040cd2d  test       ax, ax
0040cd30  je         0x40cdea

// block 0040cd36
0040cd36  cmp        ax, 3
0040cd3a  je         0x40cdea

// block 0040cd40
0040cd40  cmp        dword ptr [esi + 4], 0
0040cd44  jne        0x40cdea

// block 0040cd4a
0040cd4a  fld        dword ptr [ebp]
0040cd4d  lea        edi, [esi + 0x4bc]
0040cd53  fmul       st(1)
0040cd55  fld        dword ptr [edi]
0040cd57  fld        dword ptr [ebx]
0040cd59  fsub       st(2)
0040cd5b  fcompp     
0040cd5d  fnstsw     ax
0040cd5f  test       ah, 0x41
0040cd62  je         0x40cde8

// block 0040cd68
0040cd68  fld        dword ptr [edi]
0040cd6a  fld        dword ptr [ebx]
0040cd6c  faddp      st(2)
0040cd6e  fcompp     
0040cd70  fnstsw     ax
0040cd72  test       ah, 0x41
0040cd75  je         0x40cdea

// block 0040cd77
0040cd77  fmul       dword ptr [ebp + 4]
0040cd7a  fld        dword ptr [esi + 0x4c0]
0040cd80  fld        dword ptr [ebx + 4]
0040cd83  fsub       st(2)
0040cd85  fcompp     
0040cd87  fnstsw     ax
0040cd89  test       ah, 0x41
0040cd8c  je         0x40cdea

// block 0040cd8e
0040cd8e  fld        dword ptr [esi + 0x4c0]
0040cd94  fld        dword ptr [ebx + 4]
0040cd97  faddp      st(2)
0040cd99  fcompp     
0040cd9b  fnstsw     ax
0040cd9d  test       ah, 0x41
0040cda0  je         0x40cdec

// block 0040cda2
0040cda2  call       0x40c8b0

// block 0040cda7
0040cda7  fld        dword ptr [0x4a4014]
0040cdad  sub        esp, 8
0040cdb0  fst        dword ptr [esp + 4]
0040cdb4  mov        ecx, edi
0040cdb6  fstp       dword ptr [esp]
0040cdb9  call       0x40d560

// block 0040cdbe
0040cdbe  test       eax, eax
0040cdc0  jne        0x40cdec

// block 0040cdc2
0040cdc2  cmp        dword ptr [esp + 0x18], eax
0040cdc6  je         0x40cdec

// block 0040cdc8
0040cdc8  fld        dword ptr [0x4a41f4]
0040cdce  sub        esp, 8
0040cdd1  fstp       dword ptr [esp + 4]
0040cdd5  fld        dword ptr [0x4a4060]
0040cddb  fstp       dword ptr [esp]
0040cdde  push       edi
0040cddf  push       9
0040cde1  call       0x4273f0

// block 0040cde6
0040cde6  jmp        0x40cdec

// block 0040cde8
0040cde8  fstp       st(0)

// block 0040cdea
0040cdea  fstp       st(0)

// block 0040cdec
0040cdec  add        esi, 0x9f8
0040cdf2  sub        dword ptr [esp + 0x14], 1
0040cdf7  jne        0x40cd20

// block 0040cdfd
0040cdfd  mov        eax, dword ptr [esp + 0x18]
0040ce01  push       eax
0040ce02  push       ebp
0040ce03  call       0x4151f0

// block 0040ce08
0040ce08  pop        edi
0040ce09  pop        esi
0040ce0a  pop        ebp
0040ce0b  xor        eax, eax
0040ce0d  pop        ebx
0040ce0e  ret        8
