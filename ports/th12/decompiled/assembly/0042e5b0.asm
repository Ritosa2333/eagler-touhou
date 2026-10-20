
// block 0042e5b0
0042e5b0  push       ebp
0042e5b1  mov        ebp, esp
0042e5b3  and        esp, 0xfffffff8
0042e5b6  sub        esp, 0x24
0042e5b9  mov        eax, dword ptr [ebp + 8]
0042e5bc  fld        dword ptr [eax]
0042e5be  push       esi
0042e5bf  mov        esi, ecx
0042e5c1  fsub       dword ptr [esi + 0x50]
0042e5c4  fstp       dword ptr [esp + 0x10]
0042e5c8  fld        dword ptr [eax + 4]
0042e5cb  fsub       dword ptr [esi + 0x54]
0042e5ce  fstp       dword ptr [esp + 0x14]
0042e5d2  fld        dword ptr [esi + 0x68]
0042e5d5  fchs       
0042e5d7  fstp       dword ptr [esp + 4]
0042e5db  fld        dword ptr [esp + 4]
0042e5df  call       0x4938c0

// block 0042e5e4
0042e5e4  fstp       dword ptr [esp + 8]
0042e5e8  fld        dword ptr [esp + 8]
0042e5ec  fstp       dword ptr [esp + 0xc]
0042e5f0  fld        dword ptr [esp + 4]
0042e5f4  call       0x4939f0

// block 0042e5f9
0042e5f9  fstp       dword ptr [esp + 8]
0042e5fd  fld        dword ptr [esp + 8]
0042e601  fstp       dword ptr [esp + 8]
0042e605  fld        dword ptr [esp + 8]
0042e609  fld        st(0)
0042e60b  fld        dword ptr [esp + 0x10]
0042e60f  fld        st(0)
0042e611  fmulp      st(2)
0042e613  fld        dword ptr [esp + 0xc]
0042e617  fld        st(0)
0042e619  fld        dword ptr [esp + 0x14]
0042e61d  fld        st(0)
0042e61f  fmulp      st(2)
0042e621  fxch       st(4)
0042e623  fsubrp     st(1)
0042e625  fstp       dword ptr [esp + 0x10]
0042e629  fmulp      st(1)
0042e62b  fxch       st(2)
0042e62d  fmulp      st(1)
0042e62f  faddp      st(1)
0042e631  fstp       dword ptr [esp + 0x14]
0042e635  fld        dword ptr [esp + 0x10]
0042e639  fld        st(0)
0042e63b  fld        dword ptr [ebp + 0xc]
0042e63e  fld        st(0)
0042e640  fsubp      st(2)
0042e642  fxch       st(1)
0042e644  fstp       dword ptr [esp + 0x1c]
0042e648  fld        dword ptr [esp + 0x14]
0042e64c  fld        st(0)
0042e64e  fsub       st(2)
0042e650  fstp       dword ptr [esp + 0x20]
0042e654  fld        st(1)
0042e656  faddp      st(3)
0042e658  fxch       st(2)
0042e65a  fstp       dword ptr [esp + 0x10]
0042e65e  faddp      st(1)
0042e660  fstp       dword ptr [esp + 0x14]
0042e664  fld        dword ptr [esp + 0x1c]
0042e668  fld        dword ptr [esi + 0x6c]
0042e66b  fcompp     
0042e66d  fnstsw     ax
0042e66f  test       ah, 5
0042e672  jnp        0x42e6bd

// block 0042e674
0042e674  fld        dword ptr [esp + 0x20]
0042e678  fld        dword ptr [esi + 0x70]
0042e67b  fld        qword ptr [0x4a3cf8]
0042e681  fmul       st(1), st(0)
0042e683  fxch       st(2)
0042e685  fcompp     
0042e687  fnstsw     ax
0042e689  test       ah, 0x41
0042e68c  je         0x42e6bb

// block 0042e68e
0042e68e  fldz       
0042e690  fcomp      dword ptr [esp + 0x10]
0042e694  fnstsw     ax
0042e696  test       ah, 0x41
0042e699  je         0x42e6bb

// block 0042e69b
0042e69b  fld        dword ptr [esp + 0x14]
0042e69f  fld        dword ptr [esi + 0x70]
0042e6a2  fchs       
0042e6a4  fmulp      st(2)
0042e6a6  fcompp     
0042e6a8  fnstsw     ax
0042e6aa  test       ah, 5
0042e6ad  jnp        0x42e6bd

// block 0042e6af
0042e6af  mov        eax, 2
0042e6b4  pop        esi
0042e6b5  mov        esp, ebp
0042e6b7  pop        ebp
0042e6b8  ret        8

// block 0042e6bb
0042e6bb  fstp       st(0)

// block 0042e6bd
0042e6bd  xor        eax, eax
0042e6bf  pop        esi
0042e6c0  mov        esp, ebp
0042e6c2  pop        ebp
0042e6c3  ret        8
