
// block 0042aa60
0042aa60  push       ebp
0042aa61  mov        ebp, esp
0042aa63  and        esp, 0xfffffff8
0042aa66  sub        esp, 0x24
0042aa69  mov        eax, dword ptr [ebp + 8]
0042aa6c  fld        dword ptr [eax]
0042aa6e  push       esi
0042aa6f  mov        esi, ecx
0042aa71  fsub       dword ptr [esi + 0x50]
0042aa74  fstp       dword ptr [esp + 0x10]
0042aa78  fld        dword ptr [eax + 4]
0042aa7b  fsub       dword ptr [esi + 0x54]
0042aa7e  fstp       dword ptr [esp + 0x14]
0042aa82  fld        dword ptr [esi + 0x68]
0042aa85  fchs       
0042aa87  fstp       dword ptr [esp + 4]
0042aa8b  fld        dword ptr [esp + 4]
0042aa8f  call       0x4938c0

// block 0042aa94
0042aa94  fstp       dword ptr [esp + 8]
0042aa98  fld        dword ptr [esp + 8]
0042aa9c  fstp       dword ptr [esp + 0xc]
0042aaa0  fld        dword ptr [esp + 4]
0042aaa4  call       0x4939f0

// block 0042aaa9
0042aaa9  fstp       dword ptr [esp + 8]
0042aaad  fld        dword ptr [esp + 8]
0042aab1  fstp       dword ptr [esp + 8]
0042aab5  fld        dword ptr [esp + 8]
0042aab9  fld        st(0)
0042aabb  fld        dword ptr [esp + 0x10]
0042aabf  fld        st(0)
0042aac1  fmulp      st(2)
0042aac3  fld        dword ptr [esp + 0xc]
0042aac7  fld        st(0)
0042aac9  fld        dword ptr [esp + 0x14]
0042aacd  fld        st(0)
0042aacf  fmulp      st(2)
0042aad1  fxch       st(4)
0042aad3  fsubrp     st(1)
0042aad5  fstp       dword ptr [esp + 0x10]
0042aad9  fmulp      st(1)
0042aadb  fxch       st(2)
0042aadd  fmulp      st(1)
0042aadf  faddp      st(1)
0042aae1  fstp       dword ptr [esp + 0x14]
0042aae5  fld        dword ptr [esp + 0x10]
0042aae9  fld        st(0)
0042aaeb  fld        dword ptr [ebp + 0xc]
0042aaee  fld        st(0)
0042aaf0  fsubp      st(2)
0042aaf2  fxch       st(1)
0042aaf4  fstp       dword ptr [esp + 0x1c]
0042aaf8  fld        dword ptr [esp + 0x14]
0042aafc  fld        st(0)
0042aafe  fsub       st(2)
0042ab00  fstp       dword ptr [esp + 0x20]
0042ab04  fld        st(1)
0042ab06  faddp      st(3)
0042ab08  fxch       st(2)
0042ab0a  fstp       dword ptr [esp + 0x10]
0042ab0e  faddp      st(1)
0042ab10  fstp       dword ptr [esp + 0x14]
0042ab14  fld        dword ptr [esp + 0x1c]
0042ab18  fld        dword ptr [esi + 0x6c]
0042ab1b  fcompp     
0042ab1d  fnstsw     ax
0042ab1f  test       ah, 5
0042ab22  jnp        0x42ab6d

// block 0042ab24
0042ab24  fld        dword ptr [esp + 0x20]
0042ab28  fld        dword ptr [esi + 0x70]
0042ab2b  fld        qword ptr [0x4a3cf8]
0042ab31  fmul       st(1), st(0)
0042ab33  fxch       st(2)
0042ab35  fcompp     
0042ab37  fnstsw     ax
0042ab39  test       ah, 0x41
0042ab3c  je         0x42ab6b

// block 0042ab3e
0042ab3e  fldz       
0042ab40  fcomp      dword ptr [esp + 0x10]
0042ab44  fnstsw     ax
0042ab46  test       ah, 0x41
0042ab49  je         0x42ab6b

// block 0042ab4b
0042ab4b  fld        dword ptr [esp + 0x14]
0042ab4f  fld        dword ptr [esi + 0x70]
0042ab52  fchs       
0042ab54  fmulp      st(2)
0042ab56  fcompp     
0042ab58  fnstsw     ax
0042ab5a  test       ah, 5
0042ab5d  jnp        0x42ab6d

// block 0042ab5f
0042ab5f  mov        eax, 2
0042ab64  pop        esi
0042ab65  mov        esp, ebp
0042ab67  pop        ebp
0042ab68  ret        8

// block 0042ab6b
0042ab6b  fstp       st(0)

// block 0042ab6d
0042ab6d  xor        eax, eax
0042ab6f  pop        esi
0042ab70  mov        esp, ebp
0042ab72  pop        ebp
0042ab73  ret        8
