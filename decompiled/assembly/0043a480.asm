
// block 0043a480
0043a480  push       ebp
0043a481  mov        ebp, esp
0043a483  and        esp, 0xfffffff8
0043a486  sub        esp, 0x18
0043a489  push       ebx
0043a48a  push       ebp
0043a48b  push       esi
0043a48c  mov        esi, edx
0043a48e  cmp        dword ptr [esi + 0x48], 2
0043a492  push       edi
0043a493  je         0x43a66a

// block 0043a499
0043a499  mov        ebp, dword ptr [0x4b43dc]
0043a49f  xor        ebx, ebx
0043a4a1  cmp        ebp, ebx
0043a4a3  jne        0x43a4aa

// block 0043a4a5
0043a4a5  mov        dword ptr [esi + 0x54], ebx
0043a4a8  jmp        0x43a4e7

// block 0043a4aa
0043a4aa  cmp        dword ptr [esi + 0x54], ebx
0043a4ad  jne        0x43a4e7

// block 0043a4af
0043a4af  mov        ecx, dword ptr [esi + 0x18]
0043a4b2  fld        dword ptr [0x4a3fc8]
0043a4b8  mov        eax, dword ptr [esi + 0x14]
0043a4bb  mov        edx, dword ptr [esi + 0x1c]
0043a4be  push       ecx
0043a4bf  lea        edi, [esp + 0x20]
0043a4c3  fstp       dword ptr [esp]
0043a4c6  mov        dword ptr [esp + 0x20], eax
0043a4ca  mov        dword ptr [esp + 0x24], ecx
0043a4ce  mov        dword ptr [esp + 0x28], edx
0043a4d2  call       0x41a9f0

// block 0043a4d7
0043a4d7  mov        dword ptr [esi + 0x54], eax
0043a4da  cmp        eax, ebx
0043a4dc  je         0x43a4e7

// block 0043a4de
0043a4de  mov        eax, dword ptr [eax + 0x27b4]
0043a4e4  mov        dword ptr [esi + 0x64], eax

// block 0043a4e7
0043a4e7  mov        edx, dword ptr [esi + 0x54]
0043a4ea  cmp        edx, ebx
0043a4ec  je         0x43a517

// block 0043a4ee
0043a4ee  mov        ecx, dword ptr [esi + 0x64]
0043a4f1  cmp        ecx, ebx
0043a4f3  je         0x43a511

// block 0043a4f5
0043a4f5  mov        eax, dword ptr [ebp + 0x68]
0043a4f8  cmp        eax, ebx
0043a4fa  je         0x43a511

// block 0043a4fc
0043a4fc  lea        esp, [esp]

// block 0043a500
0043a500  mov        edi, dword ptr [eax]
0043a502  cmp        dword ptr [edi + 0x27b4], ecx
0043a508  je         0x43a54e

// block 0043a50a
0043a50a  mov        eax, dword ptr [eax + 4]
0043a50d  cmp        eax, ebx
0043a50f  jne        0x43a500

// block 0043a511
0043a511  mov        dword ptr [esi + 0x54], ebx
0043a514  mov        dword ptr [esi + 0x64], ebx

// block 0043a517
0043a517  fld        dword ptr [esi + 0x2c]
0043a51a  fadd       qword ptr [0x4a3e28]
0043a520  fstp       dword ptr [esp + 0x14]
0043a524  fld        dword ptr [0x4a3e68]
0043a52a  fcom       dword ptr [esp + 0x14]
0043a52e  fnstsw     ax
0043a530  test       ah, 5
0043a533  jp         0x43a661

// block 0043a539
0043a539  fstp       dword ptr [esp + 0x14]
0043a53d  xor        eax, eax
0043a53f  fld        dword ptr [esp + 0x14]
0043a543  fstp       dword ptr [esi + 0x2c]
0043a546  pop        edi
0043a547  pop        esi
0043a548  pop        ebp
0043a549  pop        ebx
0043a54a  mov        esp, ebp
0043a54c  pop        ebp
0043a54d  ret        

// block 0043a54e
0043a54e  mov        eax, dword ptr [edx + 0x26f8]
0043a554  test       al, 0x21
0043a556  jne        0x43a55f

// block 0043a558
0043a558  test       eax, 0x6000000
0043a55d  je         0x43a564

// block 0043a55f
0043a55f  mov        dword ptr [esi + 0x54], ebx
0043a562  jmp        0x43a517

// block 0043a564
0043a564  fld        dword ptr [esi + 0x30]
0043a567  lea        edi, [esi + 0x14]
0043a56a  sub        esp, 8
0043a56d  lea        eax, [edx + 0x1074]
0043a573  fstp       dword ptr [esp + 4]
0043a577  mov        ecx, edi
0043a579  call       0x43ad50

// block 0043a57e
0043a57e  fstp       dword ptr [esp]
0043a581  call       0x43ace0

// block 0043a586
0043a586  fstp       dword ptr [esp + 0x18]
0043a58a  cmp        dword ptr [esi + 4], 0x3c
0043a58e  fld        dword ptr [esi + 0x2c]
0043a591  fstp       dword ptr [esp + 0x14]
0043a595  jge        0x43a64a

// block 0043a59b
0043a59b  fld        dword ptr [esp + 0x18]
0043a59f  fld        st(0)
0043a5a1  fabs       
0043a5a3  fstp       dword ptr [esp + 0x18]
0043a5a7  fld        dword ptr [esp + 0x18]
0043a5ab  fcom       qword ptr [0x4a3fc0]
0043a5b1  fnstsw     ax
0043a5b3  test       ah, 1
0043a5b6  jne        0x43a5df

// block 0043a5b8
0043a5b8  fstp       st(0)
0043a5ba  fld        dword ptr [esp + 0x14]
0043a5be  fsub       qword ptr [0x4a3fb8]
0043a5c4  fstp       dword ptr [esp + 0x14]
0043a5c8  fld        dword ptr [0x4a3fb0]
0043a5ce  fcom       dword ptr [esp + 0x14]
0043a5d2  fnstsw     ax
0043a5d4  test       ah, 0x41
0043a5d7  jne        0x43a60b

// block 0043a5d9
0043a5d9  fstp       dword ptr [esp + 0x14]
0043a5dd  jmp        0x43a60d

// block 0043a5df
0043a5df  fcomp      dword ptr [0x4a3ee8]
0043a5e5  fnstsw     ax
0043a5e7  test       ah, 5
0043a5ea  jp         0x43a60d

// block 0043a5ec
0043a5ec  fld        dword ptr [esp + 0x14]
0043a5f0  fadd       qword ptr [0x4a3fb8]
0043a5f6  fstp       dword ptr [esp + 0x14]
0043a5fa  fld        dword ptr [0x4a3e68]
0043a600  fcom       dword ptr [esp + 0x14]
0043a604  fnstsw     ax
0043a606  test       ah, 5
0043a609  jnp        0x43a5d9

// block 0043a60b
0043a60b  fstp       st(0)

// block 0043a60d
0043a60d  fmul       qword ptr [0x4a3fa8]
0043a613  push       ecx
0043a614  fstp       dword ptr [esp + 0x1c]
0043a618  fld        dword ptr [esp + 0x1c]
0043a61c  fadd       dword ptr [esi + 0x30]
0043a61f  fstp       dword ptr [esp + 0x1c]
0043a623  fld        dword ptr [esp + 0x1c]
0043a627  fstp       dword ptr [esp]
0043a62a  call       0x4646e0

// block 0043a62f
0043a62f  push       ecx
0043a630  fstp       dword ptr [esp]
0043a633  push       edi
0043a634  call       0x408910

// block 0043a639
0043a639  fld        dword ptr [esp + 0x14]
0043a63d  fstp       dword ptr [esi + 0x2c]
0043a640  xor        eax, eax
0043a642  pop        edi
0043a643  pop        esi
0043a644  pop        ebp
0043a645  pop        ebx
0043a646  mov        esp, ebp
0043a648  pop        ebp
0043a649  ret        

// block 0043a64a
0043a64a  fld        dword ptr [esp + 0x14]
0043a64e  xor        eax, eax
0043a650  fadd       qword ptr [0x4a3fb8]
0043a656  fstp       dword ptr [esi + 0x2c]
0043a659  pop        edi
0043a65a  pop        esi
0043a65b  pop        ebp
0043a65c  pop        ebx
0043a65d  mov        esp, ebp
0043a65f  pop        ebp
0043a660  ret        

// block 0043a661
0043a661  fstp       st(0)
0043a663  fld        dword ptr [esp + 0x14]
0043a667  fstp       dword ptr [esi + 0x2c]

// block 0043a66a
0043a66a  pop        edi
0043a66b  xor        eax, eax
0043a66d  pop        esi
0043a66e  pop        ebp
0043a66f  pop        ebx
0043a670  mov        esp, ebp
0043a672  pop        ebp
0043a673  ret        
