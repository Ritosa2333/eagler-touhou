
// block 0041cf40
0041cf40  push       ebx
0041cf41  push       ebp
0041cf42  mov        ebp, dword ptr [esp + 0x10]
0041cf46  push       esi
0041cf47  push       edi
0041cf48  xor        edi, edi
0041cf4a  test       ebp, ebp
0041cf4c  jle        0x41cf8f

// block 0041cf4e
0041cf4e  mov        esi, dword ptr [esp + 0x14]
0041cf52  add        esi, 0x2a44
0041cf58  mov        ebx, ebp
0041cf5a  mov        edi, ebp
0041cf5c  lea        esp, [esp]

// block 0041cf60
0041cf60  mov        eax, dword ptr [esi]
0041cf62  test       eax, eax
0041cf64  je         0x41cf73

// block 0041cf66
0041cf66  lea        ecx, [esi - 0x494]
0041cf6c  mov        edx, 2
0041cf71  call       eax

// block 0041cf73
0041cf73  mov        eax, 2
0041cf78  mov        word ptr [esi - 0xd0], ax
0041cf7f  add        esi, 0x4b4
0041cf85  sub        ebx, 1
0041cf88  jne        0x41cf60

// block 0041cf8a
0041cf8a  cmp        ebp, 8
0041cf8d  jae        0x41d00e

// block 0041cf8f
0041cf8f  mov        eax, dword ptr [esp + 0x1c]
0041cf93  mov        edx, dword ptr [esp + 0x14]
0041cf97  mov        ecx, edi
0041cf99  imul       ecx, ecx, 0x4b4
0041cf9f  add        eax, 7
0041cfa2  lea        esi, [ecx + edx + 0x25b0]
0041cfa9  movzx      ebx, ax
0041cfac  mov        eax, dword ptr [esi + 0x494]
0041cfb2  test       eax, eax
0041cfb4  je         0x41cfbd

// block 0041cfb6
0041cfb6  movsx      edx, bx
0041cfb9  mov        ecx, esi
0041cfbb  call       eax

// block 0041cfbd
0041cfbd  inc        edi
0041cfbe  mov        word ptr [esi + 0x3c4], bx
0041cfc5  cmp        edi, 8
0041cfc8  jae        0x41d00e

// block 0041cfca
0041cfca  mov        edx, dword ptr [esp + 0x14]
0041cfce  mov        ecx, edi
0041cfd0  imul       ecx, ecx, 0x4b4
0041cfd6  mov        ebx, 8
0041cfdb  lea        esi, [ecx + edx + 0x2a44]
0041cfe2  sub        ebx, edi

// block 0041cfe4
0041cfe4  mov        eax, dword ptr [esi]
0041cfe6  test       eax, eax
0041cfe8  je         0x41cff7

// block 0041cfea
0041cfea  lea        ecx, [esi - 0x494]
0041cff0  mov        edx, 3
0041cff5  call       eax

// block 0041cff7
0041cff7  mov        eax, 3
0041cffc  mov        word ptr [esi - 0xd0], ax
0041d003  add        esi, 0x4b4
0041d009  sub        ebx, 1
0041d00c  jne        0x41cfe4

// block 0041d00e
0041d00e  pop        edi
0041d00f  pop        esi
0041d010  pop        ebp
0041d011  pop        ebx
0041d012  ret        0xc
