
// block 0040b5e0
0040b5e0  sub        esp, 8
0040b5e3  push       ebx
0040b5e4  push       ebp
0040b5e5  mov        ebp, dword ptr [0x4b43c8]
0040b5eb  push       edi
0040b5ec  lea        ecx, [esi + 4]
0040b5ef  call       0x4377a0

// block 0040b5f4
0040b5f4  fstp       dword ptr [esp + 0x10]
0040b5f8  xor        eax, eax
0040b5fa  xor        ebx, ebx
0040b5fc  cmp        ax, word ptr [esi + 0x1fa]
0040b603  jge        0x40b644

// block 0040b605
0040b605  xor        ecx, ecx
0040b607  xor        edi, edi
0040b609  cmp        cx, word ptr [esi + 0x1f8]
0040b610  jge        0x40b638

// block 0040b612
0040b612  fld        dword ptr [esp + 0x10]
0040b616  push       ecx
0040b617  fstp       dword ptr [esp]
0040b61a  push       ebx
0040b61b  push       edi
0040b61c  push       esi
0040b61d  push       ebp
0040b61e  call       0x40a250

// block 0040b623
0040b623  test       eax, eax
0040b625  je         0x40b62c

// block 0040b627
0040b627  cmp        eax, 1
0040b62a  je         0x40b644

// block 0040b62c
0040b62c  movsx      edx, word ptr [esi + 0x1f8]
0040b633  inc        edi
0040b634  cmp        edi, edx
0040b636  jl         0x40b612

// block 0040b638
0040b638  movsx      eax, word ptr [esi + 0x1fa]
0040b63f  inc        ebx
0040b640  cmp        ebx, eax
0040b642  jl         0x40b605

// block 0040b644
0040b644  test       byte ptr [esi + 0x200], 0x80
0040b64b  je         0x40b65f

// block 0040b64d
0040b64d  fld        dword ptr [esi + 4]
0040b650  mov        eax, dword ptr [esi + 0x204]
0040b656  push       ecx
0040b657  fstp       dword ptr [esp]
0040b65a  call       0x453e20

// block 0040b65f
0040b65f  pop        edi
0040b660  pop        ebp
0040b661  xor        eax, eax
0040b663  pop        ebx
0040b664  add        esp, 8
0040b667  ret        
