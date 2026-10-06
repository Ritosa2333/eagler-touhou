
// block 0040a020
0040a020  push       ebp
0040a021  mov        ebp, esp
0040a023  and        esp, 0xfffffff8
0040a026  sub        esp, 8
0040a029  xor        ecx, ecx
0040a02b  push       ebx
0040a02c  lea        ebx, [esi + 0x64]
0040a02f  push       edi
0040a030  mov        dword ptr [esi + 0x5c], ecx
0040a033  mov        dword ptr [esi + 0x28], ecx
0040a036  mov        dword ptr [esi + 0x24], ecx
0040a039  mov        dword ptr [esi + 0x20], ecx
0040a03c  mov        dword ptr [esi + 0x1c], ecx
0040a03f  mov        dword ptr [esi + 0x18], ecx
0040a042  mov        dword ptr [esi + 0x14], ecx
0040a045  mov        dword ptr [esi + 0x40], ecx
0040a048  mov        dword ptr [esi + 0x3c], ecx
0040a04b  mov        dword ptr [esi + 0x38], ecx
0040a04e  mov        dword ptr [esi + 0x34], ecx
0040a051  mov        dword ptr [esi + 0x30], ecx
0040a054  mov        dword ptr [esi + 0x2c], ecx
0040a057  lea        edi, [ebx + 0x4ec]
0040a05d  mov        dword ptr [esp + 8], 0x7d0

// block 0040a065
0040a065  cmp        word ptr [edi + 0x46], cx
0040a069  je         0x40a107

// block 0040a06f
0040a06f  mov        eax, dword ptr [0x4b44e8]
0040a074  cmp        eax, ecx
0040a076  je         0x40a086

// block 0040a078
0040a078  mov        eax, dword ptr [eax + 0x60]
0040a07b  test       al, 2
0040a07d  je         0x40a086

// block 0040a07f
0040a07f  test       eax, 0x400
0040a084  jne        0x40a092

// block 0040a086
0040a086  push       ebx
0040a087  call       0x4099e0

// block 0040a08c
0040a08c  xor        ecx, ecx
0040a08e  test       eax, eax
0040a090  jne        0x40a107

// block 0040a092
0040a092  mov        eax, dword ptr [edi + 0x60]
0040a095  cmp        dword ptr [esi + eax*4 + 0x14], ecx
0040a099  je         0x40a0a7

// block 0040a09b
0040a09b  mov        eax, dword ptr [esi + eax*4 + 0x2c]
0040a09f  mov        dword ptr [eax + 0x538], ebx
0040a0a5  jmp        0x40a0ab

// block 0040a0a7
0040a0a7  mov        dword ptr [esi + eax*4 + 0x14], ebx

// block 0040a0ab
0040a0ab  mov        edx, dword ptr [edi + 0x60]
0040a0ae  mov        dword ptr [esi + edx*4 + 0x2c], ebx
0040a0b2  mov        dword ptr [edi + 0x4c], ecx
0040a0b5  inc        dword ptr [esi + 0x5c]
0040a0b8  mov        edx, dword ptr [edi - 4]
0040a0bb  mov        ecx, dword ptr [edi + 4]
0040a0be  mov        dword ptr [edi - 8], edx
0040a0c1  fld        dword ptr [ecx]
0040a0c3  fcomp      qword ptr [0x4a3db0]
0040a0c9  fnstsw     ax
0040a0cb  test       ah, 0x41
0040a0ce  jne        0x40a0ef

// block 0040a0d0
0040a0d0  fld        dword ptr [0x4a3dac]
0040a0d6  fcomp      dword ptr [ecx]
0040a0d8  fnstsw     ax
0040a0da  test       ah, 0x41
0040a0dd  jne        0x40a0ef

// block 0040a0df
0040a0df  fld        dword ptr [edi]
0040a0e1  inc        edx
0040a0e2  fadd       qword ptr [0x4a3ce0]
0040a0e8  mov        dword ptr [edi - 4], edx
0040a0eb  fstp       dword ptr [edi]
0040a0ed  jmp        0x40a105

// block 0040a0ef
0040a0ef  fld        dword ptr [edi]
0040a0f1  fadd       dword ptr [ecx]
0040a0f3  fstp       dword ptr [esp + 0xc]
0040a0f7  fld        dword ptr [esp + 0xc]
0040a0fb  fst        dword ptr [edi]
0040a0fd  call       0x4931e0

// block 0040a102
0040a102  mov        dword ptr [edi - 4], eax

// block 0040a105
0040a105  xor        ecx, ecx

// block 0040a107
0040a107  add        ebx, 0x9f8
0040a10d  add        edi, 0x9f8
0040a113  sub        dword ptr [esp + 8], 1
0040a118  jne        0x40a065

// block 0040a11e
0040a11e  pop        edi
0040a11f  mov        eax, 1
0040a124  pop        ebx
0040a125  mov        esp, ebp
0040a127  pop        ebp
0040a128  ret        
