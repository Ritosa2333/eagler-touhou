
// block 0041b5f0
0041b5f0  sub        esp, 0xc
0041b5f3  push       ebx
0041b5f4  push       ebp
0041b5f5  push       esi
0041b5f6  mov        esi, dword ptr [0x4b43b8]
0041b5fc  push       edi
0041b5fd  mov        edi, 1
0041b602  mov        dword ptr [esi + 0x18f98], 5
0041b60c  xor        ebp, ebp
0041b60e  cmp        dword ptr [ecx + 0x23c], edi
0041b614  jne        0x41b682

// block 0041b616
0041b616  mov        eax, dword ptr [0x4b43dc]
0041b61b  mov        edx, dword ptr [eax + 0x1c]
0041b61e  mov        edx, dword ptr [edx + 0x1288]
0041b624  mov        dword ptr [ecx + 0x240], edx
0041b62a  mov        edx, dword ptr [eax + 0x1c]
0041b62d  add        dword ptr [edx + 0x1288], edi
0041b633  mov        edx, dword ptr [eax + 0x1c]
0041b636  cmp        dword ptr [edx + 0x1288], 0xf
0041b63d  jl         0x41b647

// block 0041b63f
0041b63f  mov        eax, edx
0041b641  mov        dword ptr [eax + 0x1288], ebp

// block 0041b647
0041b647  mov        edx, dword ptr [ecx + 0x240]
0041b64d  mov        edx, dword ptr [edx*4 + 0x4b2f48]
0041b654  mov        eax, 0x66666667
0041b659  imul       edx
0041b65b  mov        eax, dword ptr [0x4b0c44]
0041b660  sar        edx, 2
0041b663  mov        ebx, edx
0041b665  shr        ebx, 0x1f
0041b668  add        ebx, edx
0041b66a  add        eax, ebx
0041b66c  cmp        eax, 0x3b9aca00
0041b671  mov        dword ptr [0x4b0c44], eax
0041b676  jl         0x41b682

// block 0041b678
0041b678  mov        dword ptr [0x4b0c44], 0x3b9ac9ff

// block 0041b682
0041b682  fld        dword ptr [ecx + 0x34]
0041b685  lea        ebx, [esp + 0x10]
0041b689  fadd       qword ptr [0x4a3d70]
0041b68f  fadd       qword ptr [0x4a3d28]
0041b695  fstp       dword ptr [esp + 0x10]
0041b699  fld        dword ptr [ecx + 0x38]
0041b69c  fadd       qword ptr [0x4a3d68]
0041b6a2  fstp       dword ptr [esp + 0x14]
0041b6a6  fld        dword ptr [ecx + 0x3c]
0041b6a9  mov        dword ptr [esi + 0x18f80], 0xd0ffffff
0041b6b3  fstp       dword ptr [esp + 0x18]
0041b6b7  mov        dword ptr [esi + 0x18fa4], ebp
0041b6bd  fld        dword ptr [0x4a4014]
0041b6c3  mov        dword ptr [esi + 0x18fa8], ebp
0041b6c9  fst        dword ptr [esi + 0x18f84]
0041b6cf  mov        dword ptr [esi + 0x18f9c], edi
0041b6d5  fstp       dword ptr [esi + 0x18f88]
0041b6db  mov        eax, dword ptr [ecx + 0x240]
0041b6e1  mov        ecx, dword ptr [eax*4 + 0x4b2f48]
0041b6e8  push       ecx
0041b6e9  push       0x49f484
0041b6ee  call       0x4015c0

// block 0041b6f3
0041b6f3  fld1       
0041b6f5  mov        eax, dword ptr [0x4b43b8]
0041b6fa  fst        dword ptr [eax + 0x18f84]
0041b700  add        esp, 8
0041b703  fstp       dword ptr [eax + 0x18f88]
0041b709  mov        dword ptr [eax + 0x18fa4], edi
0041b70f  mov        dword ptr [eax + 0x18fa8], edi
0041b715  pop        edi
0041b716  pop        esi
0041b717  mov        dword ptr [eax + 0x18f98], ebp
0041b71d  mov        dword ptr [eax + 0x18f9c], ebp
0041b723  pop        ebp
0041b724  mov        dword ptr [eax + 0x18f80], 0xffffffff
0041b72e  xor        eax, eax
0041b730  pop        ebx
0041b731  add        esp, 0xc
0041b734  ret        
