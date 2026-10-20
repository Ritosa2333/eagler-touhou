
// block 0045b930
0045b930  mov        eax, dword ptr [ebx + 0x47c]
0045b936  sub        esp, 0x44
0045b939  push       ebp
0045b93a  push       esi
0045b93b  push       edi
0045b93c  test       eax, 0x8000
0045b941  jne        0x45ba07

// block 0045b947
0045b947  test       al, 0xc
0045b949  je         0x45ba07

// block 0045b94f
0045b94f  fld        dword ptr [ebx + 0x40]
0045b952  lea        ebp, [ebx + 0x33c]
0045b958  lea        esi, [ebx + 0x2fc]
0045b95e  mov        ecx, 0x10
0045b963  mov        edi, ebp

// block 0045b965
0045b965  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 0045b967
0045b967  fmul       dword ptr [ebp]
0045b96a  fstp       dword ptr [ebp]
0045b96d  fld        dword ptr [ebx + 0x44]
0045b970  fmul       dword ptr [ebx + 0x350]
0045b976  fstp       dword ptr [ebx + 0x350]
0045b97c  fldz       
0045b97e  fcomp      dword ptr [ebx + 0x24]
0045b981  and        eax, 0xfffffff7
0045b984  mov        dword ptr [ebx + 0x47c], eax
0045b98a  fnstsw     ax
0045b98c  test       ah, 0x44
0045b98f  jnp        0x45b9ae

// block 0045b991
0045b991  fld        dword ptr [ebx + 0x24]
0045b994  push       ecx
0045b995  lea        eax, [esp + 0x14]
0045b999  fstp       dword ptr [esp]
0045b99c  push       eax
0045b99d  call       0x46c6f2

// block 0045b9a2
0045b9a2  lea        ecx, [esp + 0x10]
0045b9a6  push       ecx
0045b9a7  push       ebp
0045b9a8  push       ebp
0045b9a9  call       0x46c6f8

// block 0045b9ae
0045b9ae  fldz       
0045b9b0  fcomp      dword ptr [ebx + 0x28]
0045b9b3  fnstsw     ax
0045b9b5  test       ah, 0x44
0045b9b8  jnp        0x45b9d7

// block 0045b9ba
0045b9ba  fld        dword ptr [ebx + 0x28]
0045b9bd  push       ecx
0045b9be  lea        edx, [esp + 0x14]
0045b9c2  fstp       dword ptr [esp]
0045b9c5  push       edx
0045b9c6  call       0x46c6fe

// block 0045b9cb
0045b9cb  lea        eax, [esp + 0x10]
0045b9cf  push       eax
0045b9d0  push       ebp
0045b9d1  push       ebp
0045b9d2  call       0x46c6f8

// block 0045b9d7
0045b9d7  fldz       
0045b9d9  fcomp      dword ptr [ebx + 0x2c]
0045b9dc  fnstsw     ax
0045b9de  test       ah, 0x44
0045b9e1  jnp        0x45ba00

// block 0045b9e3
0045b9e3  fld        dword ptr [ebx + 0x2c]
0045b9e6  push       ecx
0045b9e7  lea        ecx, [esp + 0x14]
0045b9eb  fstp       dword ptr [esp]
0045b9ee  push       ecx
0045b9ef  call       0x46c704

// block 0045b9f4
0045b9f4  lea        edx, [esp + 0x10]
0045b9f8  push       edx
0045b9f9  push       ebp
0045b9fa  push       ebp
0045b9fb  call       0x46c6f8

// block 0045ba00
0045ba00  and        dword ptr [ebx + 0x47c], 0xfffffffb

// block 0045ba07
0045ba07  fld        dword ptr [ebx + 0x430]
0045ba0d  lea        esi, [ebx + 0x33c]
0045ba13  fadd       dword ptr [ebx + 0x424]
0045ba19  mov        ecx, 0x10
0045ba1e  lea        edi, [esp + 0x10]

// block 0045ba22
0045ba22  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 0045ba24
0045ba24  fadd       dword ptr [ebx + 0x43c]
0045ba2a  fadd       dword ptr [esp + 0x40]
0045ba2e  fstp       dword ptr [esp + 0x40]
0045ba32  fld        dword ptr [ebx + 0x434]
0045ba38  fadd       dword ptr [ebx + 0x428]
0045ba3e  mov        edi, dword ptr [esp + 0x54]
0045ba42  fadd       dword ptr [ebx + 0x440]
0045ba48  add        edi, 0x4b5140
0045ba4e  mov        ecx, 0x10
0045ba53  lea        esi, [esp + 0x10]
0045ba57  fadd       dword ptr [esp + 0x44]
0045ba5b  xor        eax, eax
0045ba5d  fstp       dword ptr [esp + 0x44]
0045ba61  fld        dword ptr [ebx + 0x438]
0045ba67  fadd       dword ptr [ebx + 0x42c]
0045ba6d  fadd       dword ptr [ebx + 0x444]
0045ba73  fstp       dword ptr [esp + 0x48]

// block 0045ba77
0045ba77  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 0045ba79
0045ba79  pop        edi
0045ba7a  pop        esi
0045ba7b  pop        ebp
0045ba7c  add        esp, 0x44
0045ba7f  ret        4
