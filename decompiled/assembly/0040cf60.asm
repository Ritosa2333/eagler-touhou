
// block 0040cf60
0040cf60  sub        esp, 0x48
0040cf63  mov        eax, dword ptr [0x4b43cc]
0040cf68  test       byte ptr [eax + 0x7c], 1
0040cf6c  push       ebx
0040cf6d  push       ebp
0040cf6e  push       esi
0040cf6f  mov        esi, dword ptr [0x4b43c8]
0040cf75  push       edi
0040cf76  mov        dword ptr [esp + 0x14], esi
0040cf7a  lea        edi, [esi + 0x64]
0040cf7d  je         0x40cf90

// block 0040cf7f
0040cf7f  mov        eax, dword ptr [eax + 0x78]
0040cf82  cmp        eax, 0x60
0040cf85  jl         0x40cf90

// block 0040cf87
0040cf87  cmp        eax, 0x63
0040cf8a  jle        0x40d221

// block 0040cf90
0040cf90  fld        dword ptr [esi + 0x50]
0040cf93  mov        dword ptr [esp + 0x10], 0x7d0
0040cf9b  fld        qword ptr [0x4a3cf8]
0040cfa1  fmul       st(1), st(0)
0040cfa3  fxch       st(1)
0040cfa5  fstp       dword ptr [esp + 0x1c]
0040cfa9  fld        dword ptr [esi + 0x54]
0040cfac  fmul       st(1)
0040cfae  fstp       dword ptr [esp + 0x20]
0040cfb2  fld        dword ptr [esi + 0x44]
0040cfb5  fld        dword ptr [esp + 0x1c]
0040cfb9  fld        st(0)
0040cfbb  fsubp      st(2)
0040cfbd  fxch       st(1)
0040cfbf  fstp       dword ptr [esp + 0x34]
0040cfc3  fld        dword ptr [esi + 0x48]
0040cfc6  fld        dword ptr [esp + 0x20]
0040cfca  fld        st(0)
0040cfcc  fsubp      st(2)
0040cfce  fxch       st(1)
0040cfd0  fstp       dword ptr [esp + 0x38]
0040cfd4  fld        dword ptr [esi + 0x44]
0040cfd7  faddp      st(2)
0040cfd9  fxch       st(1)
0040cfdb  fstp       dword ptr [esp + 0x4c]
0040cfdf  fadd       dword ptr [esi + 0x48]
0040cfe2  fstp       dword ptr [esp + 0x50]

// block 0040cfe6
0040cfe6  movzx      eax, word ptr [edi + 0x532]
0040cfed  test       ax, ax
0040cff0  je         0x40d20e

// block 0040cff6
0040cff6  cmp        ax, 3
0040cffa  je         0x40d20e

// block 0040d000
0040d000  fld        dword ptr [edi + 0x4dc]
0040d006  lea        ebp, [edi + 0x4bc]
0040d00c  fmul       st(1)
0040d00e  fld        dword ptr [ebp]
0040d011  fsub       st(1)
0040d013  fstp       dword ptr [esp + 0x40]
0040d017  fld        dword ptr [edi + 0x4e0]
0040d01d  fmul       st(2)
0040d01f  fld        dword ptr [edi + 0x4c0]
0040d025  fsub       st(1)
0040d027  fstp       dword ptr [esp + 0x44]
0040d02b  fld        dword ptr [ebp]
0040d02e  faddp      st(2)
0040d030  fxch       st(1)
0040d032  fstp       dword ptr [esp + 0x28]
0040d036  fadd       dword ptr [edi + 0x4c0]
0040d03c  fstp       dword ptr [esp + 0x2c]
0040d040  fld        dword ptr [esp + 0x28]
0040d044  fld        dword ptr [esp + 0x34]
0040d048  fcompp     
0040d04a  fnstsw     ax
0040d04c  test       ah, 0x41
0040d04f  je         0x40d20e

// block 0040d055
0040d055  fld        dword ptr [esp + 0x40]
0040d059  fld        dword ptr [esp + 0x4c]
0040d05d  fcompp     
0040d05f  fnstsw     ax
0040d061  test       ah, 5
0040d064  jnp        0x40d20e

// block 0040d06a
0040d06a  fld        dword ptr [esp + 0x2c]
0040d06e  fld        dword ptr [esp + 0x38]
0040d072  fcompp     
0040d074  fnstsw     ax
0040d076  test       ah, 0x41
0040d079  je         0x40d20e

// block 0040d07f
0040d07f  fld        dword ptr [esp + 0x44]
0040d083  fld        dword ptr [esp + 0x50]
0040d087  fcompp     
0040d089  fnstsw     ax
0040d08b  test       ah, 5
0040d08e  jnp        0x40d20e

// block 0040d094
0040d094  or         dword ptr [edi], 8
0040d097  fstp       st(0)
0040d099  cmp        dword ptr [esp + 0x5c], 0
0040d09e  je         0x40d0be

// block 0040d0a0
0040d0a0  fld        dword ptr [0x4a41f4]
0040d0a6  sub        esp, 8
0040d0a9  fstp       dword ptr [esp + 4]
0040d0ad  fld        dword ptr [0x4a4060]
0040d0b3  fstp       dword ptr [esp]
0040d0b6  push       ebp
0040d0b7  push       9
0040d0b9  call       0x4273f0

// block 0040d0be
0040d0be  test       dword ptr [0x4cee78], 0x8000
0040d0c8  mov        ebx, dword ptr [esi + 0x4debdc]
0040d0ce  je         0x40d0e1

// block 0040d0d0
0040d0d0  push       0x4cf1d0
0040d0d5  call       dword ptr [0x498088]

// block 0040d0db
0040d0db  inc        byte ptr [0x4cf221]

// block 0040d0e1
0040d0e1  inc        dword ptr [ebx + 0x130]
0040d0e7  call       0x4621c0

// block 0040d0ec
0040d0ec  mov        esi, eax
0040d0ee  or         dword ptr [esi + 0x480], 1
0040d0f5  mov        dword ptr [esi + 0x20], 0x17
0040d0fc  test       ebp, ebp
0040d0fe  jne        0x40d12e

// block 0040d100
0040d100  fldz       
0040d102  fst        dword ptr [esp + 0x1c]
0040d106  mov        eax, dword ptr [esp + 0x1c]
0040d10a  fst        dword ptr [esp + 0x20]
0040d10e  mov        ecx, dword ptr [esp + 0x20]
0040d112  fstp       dword ptr [esp + 0x24]
0040d116  mov        edx, dword ptr [esp + 0x24]
0040d11a  mov        dword ptr [esi + 0x430], eax
0040d120  mov        dword ptr [esi + 0x434], ecx
0040d126  mov        dword ptr [esi + 0x438], edx
0040d12c  jmp        0x40d161

// block 0040d12e
0040d12e  fld        dword ptr [ebp]
0040d131  fadd       qword ptr [0x4a3d70]
0040d137  fadd       qword ptr [0x4a3d28]
0040d13d  fstp       dword ptr [esi + 0x430]
0040d143  fld        dword ptr [edi + 0x4c0]
0040d149  fadd       qword ptr [0x4a3d68]
0040d14f  fstp       dword ptr [esi + 0x434]
0040d155  fld        dword ptr [edi + 0x4c4]
0040d15b  fstp       dword ptr [esi + 0x438]

// block 0040d161
0040d161  push       0x60
0040d163  push       esi
0040d164  mov        ecx, ebx
0040d166  call       0x454d10

// block 0040d16b
0040d16b  mov        ebx, esi
0040d16d  lea        eax, [esp + 0x18]
0040d171  call       0x461250

// block 0040d176
0040d176  test       dword ptr [0x4cee78], 0x8000
0040d180  mov        esi, dword ptr [eax]
0040d182  je         0x40d195

// block 0040d184
0040d184  push       0x4cf1d0
0040d189  call       dword ptr [0x49808c]

// block 0040d18f
0040d18f  dec        byte ptr [0x4cf221]

// block 0040d195
0040d195  mov        edx, dword ptr [0x4ce8cc]
0040d19b  push       esi
0040d19c  call       0x461920

// block 0040d1a1
0040d1a1  mov        ecx, dword ptr [edi + 0x3fc]
0040d1a7  mov        edx, eax
0040d1a9  test       ecx, ecx
0040d1ab  je         0x40d1fa

// block 0040d1ad
0040d1ad  fld        dword ptr [0x4a3e68]
0040d1b3  fcomp      dword ptr [ecx + 0x38]
0040d1b6  fnstsw     ax
0040d1b8  test       ah, 1
0040d1bb  jne        0x40d1cd

// block 0040d1bd
0040d1bd  movsx      eax, word ptr [edi + 0x9f6]
0040d1c4  mov        ecx, dword ptr [eax*4 + 0x4b0bd0]
0040d1cb  jmp        0x40d1f4

// block 0040d1cd
0040d1cd  fld        dword ptr [0x4a3d78]
0040d1d3  fcomp      dword ptr [ecx + 0x38]
0040d1d6  fnstsw     ax
0040d1d8  test       ah, 1
0040d1db  movsx      eax, word ptr [edi + 0x9f6]
0040d1e2  jne        0x40d1ed

// block 0040d1e4
0040d1e4  mov        ecx, dword ptr [eax*4 + 0x4b0c10]
0040d1eb  jmp        0x40d1f4

// block 0040d1ed
0040d1ed  mov        ecx, dword ptr [eax*4 + 0x4b0c30]

// block 0040d1f4
0040d1f4  mov        dword ptr [edx + 0x3bc], ecx

// block 0040d1fa
0040d1fa  fld        qword ptr [0x4a3cf8]
0040d200  mov        esi, dword ptr [esp + 0x14]
0040d204  mov        dword ptr [edi + 0x53c], 0

// block 0040d20e
0040d20e  add        edi, 0x9f8
0040d214  sub        dword ptr [esp + 0x10], 1
0040d219  jne        0x40cfe6

// block 0040d21f
0040d21f  fstp       st(0)

// block 0040d221
0040d221  pop        edi
0040d222  pop        esi
0040d223  pop        ebp
0040d224  xor        eax, eax
0040d226  pop        ebx
0040d227  add        esp, 0x48
0040d22a  ret        4
