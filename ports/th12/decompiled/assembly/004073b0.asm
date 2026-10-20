
// block 004073b0
004073b0  mov        edx, dword ptr [ecx + 0x18]
004073b3  sub        esp, 0x20
004073b6  cmp        edx, dword ptr [ecx + 0x14]
004073b9  je         0x4073ca

// block 004073bb
004073bb  and        edx, 0x80000003
004073c1  jns        0x4073c8

// block 004073c3
004073c3  dec        edx
004073c4  or         edx, 0xfffffffc
004073c7  inc        edx

// block 004073c8
004073c8  je         0x4073d0

// block 004073ca
004073ca  xor        eax, eax
004073cc  add        esp, 0x20
004073cf  ret        

// block 004073d0
004073d0  fld        dword ptr [esi]
004073d2  xor        edx, edx
004073d4  fld        qword ptr [0x4a3cf8]
004073da  fmul       st(1), st(0)
004073dc  fld        dword ptr [eax]
004073de  fsub       st(2)
004073e0  fstp       dword ptr [esp + 0x10]
004073e4  fmul       dword ptr [esi + 4]
004073e7  fld        dword ptr [eax + 4]
004073ea  fsub       st(1)
004073ec  fstp       dword ptr [esp + 0x14]
004073f0  fld        dword ptr [eax]
004073f2  faddp      st(2)
004073f4  fxch       st(1)
004073f6  fstp       dword ptr [esp + 0x18]
004073fa  fadd       dword ptr [eax + 4]
004073fd  fstp       dword ptr [esp + 0x1c]
00407401  fld        dword ptr [ecx + 0x508]
00407407  fld        qword ptr [0x4a3d68]
0040740d  fsub       st(1), st(0)
0040740f  fxch       st(1)
00407411  fstp       dword ptr [esp]
00407414  fadd       dword ptr [ecx + 0x508]
0040741a  fstp       dword ptr [esp + 8]
0040741e  fld        dword ptr [ecx + 0x50c]
00407424  fsub       qword ptr [0x4a3d10]
0040742a  fstp       dword ptr [esp + 4]
0040742e  fld        dword ptr [ecx + 0x50c]
00407434  fld        qword ptr [0x4a3d70]
0040743a  fadd       st(1), st(0)
0040743c  fxch       st(1)
0040743e  fstp       dword ptr [esp + 0xc]
00407442  fld        dword ptr [esp]
00407445  fld        dword ptr [esp + 0x18]
00407449  fcom       st(1)
0040744b  fnstsw     ax
0040744d  fstp       st(1)
0040744f  fld        dword ptr [esp + 0x14]
00407453  fld        dword ptr [esp + 0x10]
00407457  fld        dword ptr [esp + 0x1c]
0040745b  test       ah, 5
0040745e  jnp        0x40748c

// block 00407460
00407460  fld        dword ptr [esp + 4]
00407464  fcomp      st(1)
00407466  fnstsw     ax
00407468  test       ah, 0x41
0040746b  je         0x40748c

// block 0040746d
0040746d  fld        dword ptr [esp + 8]
00407471  fcomp      st(2)
00407473  fnstsw     ax
00407475  test       ah, 5
00407478  jnp        0x40748c

// block 0040747a
0040747a  fld        dword ptr [esp + 0xc]
0040747e  fcomp      st(3)
00407480  fnstsw     ax
00407482  test       ah, 5
00407485  jnp        0x40748c

// block 00407487
00407487  mov        edx, 0x1e

// block 0040748c
0040748c  fld        dword ptr [ecx + 0x508]
00407492  fld        qword ptr [0x4a3e88]
00407498  fsub       st(1), st(0)
0040749a  fxch       st(1)
0040749c  fstp       dword ptr [esp]
0040749f  fadd       dword ptr [ecx + 0x508]
004074a5  fstp       dword ptr [esp + 8]
004074a9  fld        dword ptr [ecx + 0x50c]
004074af  fsub       qword ptr [0x4a3d50]
004074b5  fstp       dword ptr [esp + 4]
004074b9  fld        dword ptr [ecx + 0x50c]
004074bf  fstp       dword ptr [esp + 0xc]
004074c3  fld        dword ptr [esp]
004074c6  fcomp      st(4)
004074c8  fnstsw     ax
004074ca  test       ah, 0x41
004074cd  je         0x4074f9

// block 004074cf
004074cf  fld        dword ptr [esp + 4]
004074d3  fcomp      st(1)
004074d5  fnstsw     ax
004074d7  test       ah, 0x41
004074da  je         0x4074f9

// block 004074dc
004074dc  fld        dword ptr [esp + 8]
004074e0  fcomp      st(2)
004074e2  fnstsw     ax
004074e4  test       ah, 5
004074e7  jnp        0x4074f9

// block 004074e9
004074e9  fld        dword ptr [esp + 0xc]
004074ed  fcomp      st(3)
004074ef  fnstsw     ax
004074f1  test       ah, 5
004074f4  jnp        0x4074f9

// block 004074f6
004074f6  add        edx, 0x14

// block 004074f9
004074f9  fld        dword ptr [ecx + 0x508]
004074ff  fld        qword ptr [0x4a3d48]
00407505  fsub       st(1), st(0)
00407507  fxch       st(1)
00407509  fstp       dword ptr [esp]
0040750c  fadd       dword ptr [ecx + 0x508]
00407512  fstp       dword ptr [esp + 8]
00407516  fld        dword ptr [ecx + 0x50c]
0040751c  fsub       qword ptr [0x4a3e80]
00407522  fstp       dword ptr [esp + 4]
00407526  fld        dword ptr [ecx + 0x50c]
0040752c  fsubrp     st(5)
0040752e  fxch       st(4)
00407530  fstp       dword ptr [esp + 0xc]
00407534  fld        dword ptr [esp]
00407537  fcomp      st(3)
00407539  fnstsw     ax
0040753b  fstp       st(2)
0040753d  test       ah, 0x41
00407540  je         0x407574

// block 00407542
00407542  fld        dword ptr [esp + 4]
00407546  fcomp      st(3)
00407548  fnstsw     ax
0040754a  fstp       st(2)
0040754c  test       ah, 0x41
0040754f  je         0x407576

// block 00407551
00407551  fld        dword ptr [esp + 8]
00407555  fcompp     
00407557  fnstsw     ax
00407559  test       ah, 5
0040755c  jnp        0x407578

// block 0040755e
0040755e  fld        dword ptr [esp + 0xc]
00407562  fcompp     
00407564  fnstsw     ax
00407566  test       ah, 5
00407569  jnp        0x40757a

// block 0040756b
0040756b  add        edx, 0x14
0040756e  mov        eax, edx
00407570  add        esp, 0x20
00407573  ret        

// block 00407574
00407574  fstp       st(2)

// block 00407576
00407576  fstp       st(0)

// block 00407578
00407578  fstp       st(0)

// block 0040757a
0040757a  mov        eax, edx
0040757c  add        esp, 0x20
0040757f  ret        
