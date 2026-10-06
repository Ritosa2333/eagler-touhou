
// block 0042d6e0
0042d6e0  push       ecx
0042d6e1  push       esi
0042d6e2  mov        esi, ecx
0042d6e4  mov        eax, dword ptr [esi + 0x148]
0042d6ea  cmp        dword ptr [esi + 0x124], eax
0042d6f0  mov        dword ptr [esp + 4], eax
0042d6f4  jl         0x42d7b9

// block 0042d6fa
0042d6fa  mov        edx, dword ptr [esi + 0x630]
0042d700  test       edx, edx
0042d702  jl         0x42d709

// block 0042d704
0042d704  call       0x453d90

// block 0042d709
0042d709  fld        dword ptr [esi + 0x138]
0042d70f  inc        dword ptr [esi + 0x150]
0042d715  fadd       dword ptr [esi + 0x68]
0042d718  fstp       dword ptr [esi + 0x68]
0042d71b  fld        dword ptr [esi + 0x134]
0042d721  fstp       dword ptr [esp + 4]
0042d725  fld        dword ptr [esp + 4]
0042d729  fst        dword ptr [esi + 0x74]
0042d72c  mov        eax, dword ptr [esi + 0x130]
0042d732  fstp       dword ptr [esp + 4]
0042d736  fldz       
0042d738  test       al, 1
0042d73a  jne        0x42d769

// block 0042d73c
0042d73c  or         eax, 1
0042d73f  fst        dword ptr [esi + 0x128]
0042d745  mov        dword ptr [esi + 0x124], 0
0042d74f  mov        dword ptr [esi + 0x120], 0xfff0bdc1
0042d759  mov        dword ptr [esi + 0x12c], 0x4b2ed0
0042d763  mov        dword ptr [esi + 0x130], eax

// block 0042d769
0042d769  fstp       dword ptr [esi + 0x128]
0042d76f  mov        dword ptr [esi + 0x124], 0
0042d779  mov        dword ptr [esi + 0x120], 0xffffffff
0042d783  mov        eax, dword ptr [esi + 0x150]
0042d789  cmp        eax, dword ptr [esi + 0x14c]
0042d78f  jl         0x42d7ce

// block 0042d791
0042d791  fld        dword ptr [esp + 4]
0042d795  sub        esp, 8
0042d798  fstp       dword ptr [esp + 4]
0042d79c  lea        ecx, [esi + 0x5c]
0042d79f  fld        dword ptr [esi + 0x68]
0042d7a2  fstp       dword ptr [esp]
0042d7a5  call       0x42e880

// block 0042d7aa
0042d7aa  and        dword ptr [esi + 0x430], 0xffffffef
0042d7b1  mov        eax, 1
0042d7b6  pop        esi
0042d7b7  pop        ecx
0042d7b8  ret        

// block 0042d7b9
0042d7b9  fld        dword ptr [esi + 0x74]
0042d7bc  fld        dword ptr [esi + 0x128]
0042d7c2  fmul       st(1)
0042d7c4  fidiv      dword ptr [esp + 4]
0042d7c8  fsubp      st(1)
0042d7ca  fstp       dword ptr [esp + 4]

// block 0042d7ce
0042d7ce  fld        dword ptr [esp + 4]
0042d7d2  sub        esp, 8
0042d7d5  fstp       dword ptr [esp + 4]
0042d7d9  lea        ecx, [esi + 0x5c]
0042d7dc  fld        dword ptr [esi + 0x68]
0042d7df  fstp       dword ptr [esp]
0042d7e2  call       0x42e880

// block 0042d7e7
0042d7e7  add        esi, 0x120
0042d7ed  call       0x464a80

// block 0042d7f2
0042d7f2  xor        eax, eax
0042d7f4  pop        esi
0042d7f5  pop        ecx
0042d7f6  ret        
