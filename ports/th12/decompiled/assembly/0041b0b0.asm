
// block 0041b0b0
0041b0b0  mov        eax, dword ptr [edi + 0x270]
0041b0b6  cdq        
0041b0b7  idiv       dword ptr [edi + 0x240]
0041b0bd  mov        ecx, dword ptr [0x4b43c8]
0041b0c3  sub        esp, 0x10
0041b0c6  push       ebx
0041b0c7  lea        ebx, [ecx + 0x64]
0041b0ca  test       edx, edx
0041b0cc  jne        0x41b188

// block 0041b0d2
0041b0d2  push       ebp
0041b0d3  push       esi
0041b0d4  lea        ebp, [ebx + 0x4c4]
0041b0da  mov        dword ptr [esp + 0xc], 0x7d0

// block 0041b0e2
0041b0e2  test       byte ptr [ebx], 1
0041b0e5  je         0x41b16f

// block 0041b0eb
0041b0eb  cmp        word ptr [ebp + 0x6e], 1
0041b0f0  jne        0x41b16f

// block 0041b0f2
0041b0f2  movsx      eax, word ptr [ebp + 0x530]
0041b0f9  imul       eax, eax, 0xd0
0041b0ff  cmp        dword ptr [eax + 0x4af280], 0xa1
0041b109  jne        0x41b16f

// block 0041b10b
0041b10b  fld        dword ptr [ebp - 8]
0041b10e  lea        esi, [edi + 0x6a0]
0041b114  fadd       dword ptr [edi + 0x1538]
0041b11a  fstp       dword ptr [esp + 0x10]
0041b11e  mov        edx, dword ptr [esp + 0x10]
0041b122  fld        dword ptr [ebp - 4]
0041b125  fadd       dword ptr [edi + 0x153c]
0041b12b  fstp       dword ptr [esp + 0x14]
0041b12f  mov        eax, dword ptr [esp + 0x14]
0041b133  fld        dword ptr [edi + 0x1540]
0041b139  fadd       dword ptr [ebp]
0041b13c  mov        dword ptr [edi + 0x6a4], edx
0041b142  mov        dword ptr [edi + 0x6a8], eax
0041b148  fstp       dword ptr [esp + 0x18]
0041b14c  mov        edx, dword ptr [esp + 0x18]
0041b150  fld        dword ptr [edi + 0x16c8]
0041b156  mov        dword ptr [edi + 0x6ac], edx
0041b15c  fstp       dword ptr [ecx + 0x60]
0041b15f  call       0x40b5e0

// block 0041b164
0041b164  fldz       
0041b166  mov        ecx, dword ptr [0x4b43c8]
0041b16c  fstp       dword ptr [ecx + 0x60]

// block 0041b16f
0041b16f  add        ebx, 0x9f8
0041b175  add        ebp, 0x9f8
0041b17b  sub        dword ptr [esp + 0xc], 1
0041b180  jne        0x41b0e2

// block 0041b186
0041b186  pop        esi
0041b187  pop        ebp

// block 0041b188
0041b188  xor        eax, eax
0041b18a  pop        ebx
0041b18b  add        esp, 0x10
0041b18e  ret        
