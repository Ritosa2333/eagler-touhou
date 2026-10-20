
// block 0040c230
0040c230  push       ebp
0040c231  mov        ebp, esp
0040c233  and        esp, 0xfffffff8
0040c236  sub        esp, 0x2c
0040c239  push       ebx
0040c23a  push       esi
0040c23b  mov        esi, eax
0040c23d  mov        eax, dword ptr [esi + 0x8d8]
0040c243  cmp        eax, dword ptr [esi + 0x8fc]
0040c249  push       edi
0040c24a  jl         0x40c2bd

// block 0040c24c
0040c24c  fld        dword ptr [esi + 0x8e8]
0040c252  mov        ecx, dword ptr [esi + 0x8f0]
0040c258  mov        edx, dword ptr [esi + 0x8f4]
0040c25e  fstp       dword ptr [esp + 0x10]
0040c262  fld        dword ptr [esp + 0x10]
0040c266  mov        eax, dword ptr [esi + 0x8f8]
0040c26c  and        dword ptr [esi + 0x528], 0xfdffffff
0040c276  fst        dword ptr [esi + 0x4d4]
0040c27c  sub        esp, 8
0040c27f  fstp       dword ptr [esp + 4]
0040c283  mov        dword ptr [esi + 0x4bc], ecx
0040c289  fld        dword ptr [esi + 0x4d8]
0040c28f  mov        dword ptr [esi + 0x4c0], edx
0040c295  lea        ecx, [esi + 0x4c8]
0040c29b  fstp       dword ptr [esp]
0040c29e  mov        dword ptr [esi + 0x4c4], eax
0040c2a4  call       0x40d640

// block 0040c2a9
0040c2a9  fldz       
0040c2ab  fstp       dword ptr [esi + 0x4d0]
0040c2b1  mov        eax, 1
0040c2b6  pop        edi
0040c2b7  pop        esi
0040c2b8  pop        ebx
0040c2b9  mov        esp, ebp
0040c2bb  pop        ebp
0040c2bc  ret        

// block 0040c2bd
0040c2bd  mov        ecx, dword ptr [esi + 0x4bc]
0040c2c3  mov        edx, dword ptr [esi + 0x4c0]
0040c2c9  mov        eax, dword ptr [esi + 0x4c4]
0040c2cf  lea        edi, [esi + 0x9a8]
0040c2d5  lea        ebx, [esp + 0x2c]
0040c2d9  mov        dword ptr [esp + 0x14], ecx
0040c2dd  mov        dword ptr [esp + 0x18], edx
0040c2e1  mov        dword ptr [esp + 0x1c], eax
0040c2e5  call       0x405900

// block 0040c2ea
0040c2ea  fld        dword ptr [eax]
0040c2ec  fsub       dword ptr [esp + 0x14]
0040c2f0  fstp       dword ptr [esp + 0x20]
0040c2f4  mov        ecx, dword ptr [esp + 0x20]
0040c2f8  fld        dword ptr [eax + 4]
0040c2fb  fsub       dword ptr [esp + 0x18]
0040c2ff  fstp       dword ptr [esp + 0x24]
0040c303  mov        edx, dword ptr [esp + 0x24]
0040c307  fld        dword ptr [eax + 8]
0040c30a  mov        dword ptr [esi + 0x4c8], ecx
0040c310  fsub       dword ptr [esp + 0x1c]
0040c314  mov        dword ptr [esi + 0x4cc], edx
0040c31a  fstp       dword ptr [esp + 0x28]
0040c31e  mov        eax, dword ptr [esp + 0x28]
0040c322  mov        dword ptr [esi + 0x4d0], eax
0040c328  fld        dword ptr [esi + 0x4c8]
0040c32e  fabs       
0040c330  fstp       dword ptr [esp + 0x10]
0040c334  fld        dword ptr [esp + 0x10]
0040c338  fld        qword ptr [0x4a3ff0]
0040c33e  fcom       st(1)
0040c340  fnstsw     ax
0040c342  fstp       st(1)
0040c344  test       ah, 5
0040c347  jnp        0x40c364

// block 0040c349
0040c349  fld        dword ptr [esi + 0x4cc]
0040c34f  fabs       
0040c351  fstp       dword ptr [esp + 0x10]
0040c355  fld        dword ptr [esp + 0x10]
0040c359  fcompp     
0040c35b  fnstsw     ax
0040c35d  test       ah, 0x41
0040c360  jne        0x40c385

// block 0040c362
0040c362  jmp        0x40c366

// block 0040c364
0040c364  fstp       st(0)

// block 0040c366
0040c366  fld        dword ptr [esi + 0x4cc]
0040c36c  fld        dword ptr [esi + 0x4c8]
0040c372  call       0x4937aa

// block 0040c377
0040c377  fstp       dword ptr [esp + 0x10]
0040c37b  fld        dword ptr [esp + 0x10]
0040c37f  fstp       dword ptr [esi + 0x4d8]

// block 0040c385
0040c385  fldz       
0040c387  add        esi, 0x8d4
0040c38d  fstp       dword ptr [esi - 0x404]
0040c393  call       0x464a80

// block 0040c398
0040c398  pop        edi
0040c399  pop        esi
0040c39a  xor        eax, eax
0040c39c  pop        ebx
0040c39d  mov        esp, ebp
0040c39f  pop        ebp
0040c3a0  ret        
