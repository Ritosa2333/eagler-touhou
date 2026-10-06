
// block 0040baf0
0040baf0  push       ecx
0040baf1  push       esi
0040baf2  mov        esi, eax
0040baf4  mov        eax, dword ptr [esi + 0x7c4]
0040bafa  cmp        dword ptr [esi + 0x7a0], eax
0040bb00  mov        dword ptr [esp + 4], eax
0040bb04  jl         0x40bbd2

// block 0040bb0a
0040bb0a  mov        edx, dword ptr [esi + 0x544]
0040bb10  test       edx, edx
0040bb12  jl         0x40bb19

// block 0040bb14
0040bb14  call       0x453d90

// block 0040bb19
0040bb19  fld        dword ptr [esi + 0x7b4]
0040bb1f  inc        dword ptr [esi + 0x7cc]
0040bb25  fstp       dword ptr [esi + 0x4d8]
0040bb2b  fld        dword ptr [esi + 0x7b0]
0040bb31  fstp       dword ptr [esp + 4]
0040bb35  fld        dword ptr [esp + 4]
0040bb39  fst        dword ptr [esi + 0x4d4]
0040bb3f  mov        eax, dword ptr [esi + 0x7ac]
0040bb45  fstp       dword ptr [esp + 4]
0040bb49  fldz       
0040bb4b  test       al, 1
0040bb4d  jne        0x40bb7c

// block 0040bb4f
0040bb4f  or         eax, 1
0040bb52  fst        dword ptr [esi + 0x7a4]
0040bb58  mov        dword ptr [esi + 0x7a0], 0
0040bb62  mov        dword ptr [esi + 0x79c], 0xfff0bdc1
0040bb6c  mov        dword ptr [esi + 0x7a8], 0x4b2ed0
0040bb76  mov        dword ptr [esi + 0x7ac], eax

// block 0040bb7c
0040bb7c  fstp       dword ptr [esi + 0x7a4]
0040bb82  mov        dword ptr [esi + 0x7a0], 0
0040bb8c  mov        dword ptr [esi + 0x79c], 0xffffffff
0040bb96  mov        eax, dword ptr [esi + 0x7cc]
0040bb9c  cmp        eax, dword ptr [esi + 0x7c8]
0040bba2  jl         0x40bbea

// block 0040bba4
0040bba4  fld        dword ptr [esp + 4]
0040bba8  sub        esp, 8
0040bbab  fstp       dword ptr [esp + 4]
0040bbaf  lea        ecx, [esi + 0x4c8]
0040bbb5  fld        dword ptr [esi + 0x4d8]
0040bbbb  fstp       dword ptr [esp]
0040bbbe  call       0x40d640

// block 0040bbc3
0040bbc3  and        dword ptr [esi + 0x528], 0xffffffbf
0040bbca  mov        eax, 1
0040bbcf  pop        esi
0040bbd0  pop        ecx
0040bbd1  ret        

// block 0040bbd2
0040bbd2  fld        dword ptr [esi + 0x4d4]
0040bbd8  fld        dword ptr [esi + 0x7a4]
0040bbde  fmul       st(1)
0040bbe0  fidiv      dword ptr [esp + 4]
0040bbe4  fsubp      st(1)
0040bbe6  fstp       dword ptr [esp + 4]

// block 0040bbea
0040bbea  fld        dword ptr [esp + 4]
0040bbee  sub        esp, 8
0040bbf1  fstp       dword ptr [esp + 4]
0040bbf5  lea        ecx, [esi + 0x4c8]
0040bbfb  fld        dword ptr [esi + 0x4d8]
0040bc01  fstp       dword ptr [esp]
0040bc04  call       0x40d640

// block 0040bc09
0040bc09  add        esi, 0x79c
0040bc0f  call       0x464a80

// block 0040bc14
0040bc14  xor        eax, eax
0040bc16  pop        esi
0040bc17  pop        ecx
0040bc18  ret        
