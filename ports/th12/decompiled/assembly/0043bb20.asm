
// block 0043bb20
0043bb20  sub        esp, 0x10
0043bb23  cmp        dword ptr [0x4b44e8], 0
0043bb2a  je         0x43bbd3

// block 0043bb30
0043bb30  mov        eax, dword ptr [ecx + 0x10]
0043bb33  test       eax, eax
0043bb35  je         0x43bbd3

// block 0043bb3b
0043bb3b  cmp        eax, 1
0043bb3e  jne        0x43bbd3

// block 0043bb44
0043bb44  fld        dword ptr [0x4a3f94]
0043bb4a  movzx      eax, byte ptr [ecx + 0x1cc]
0043bb51  fstp       dword ptr [esp + 4]
0043bb55  mov        dword ptr [esp], eax
0043bb58  fld        dword ptr [0x4a3f90]
0043bb5e  fstp       dword ptr [esp + 8]
0043bb62  fldz       
0043bb64  fstp       dword ptr [esp + 0xc]
0043bb68  fild       dword ptr [esp]
0043bb6b  fld        qword ptr [0x4a3e78]
0043bb71  fcomp      st(1)
0043bb73  fnstsw     ax
0043bb75  test       ah, 0x41
0043bb78  jne        0x43bb83

// block 0043bb7a
0043bb7a  fstp       st(0)
0043bb7c  mov        eax, 0xff5050ff
0043bb81  jmp        0x43bb9a

// block 0043bb83
0043bb83  fcomp      qword ptr [0x4a3f88]
0043bb89  fnstsw     ax
0043bb8b  test       ah, 5
0043bb8e  jp         0x43bb97

// block 0043bb90
0043bb90  mov        eax, 0xffa0a0ff
0043bb95  jmp        0x43bb9a

// block 0043bb97
0043bb97  or         eax, 0xffffffff

// block 0043bb9a
0043bb9a  push       ebx
0043bb9b  push       esi
0043bb9c  mov        esi, dword ptr [0x4b43b8]
0043bba2  mov        dword ptr [esi + 0x18f80], eax
0043bba8  movzx      ecx, byte ptr [ecx + 0x1cc]
0043bbaf  push       ecx
0043bbb0  push       0x4a1298
0043bbb5  lea        ebx, [esp + 0x14]
0043bbb9  call       0x4015c0

// block 0043bbbe
0043bbbe  mov        edx, dword ptr [0x4b43b8]
0043bbc4  add        esp, 8
0043bbc7  pop        esi
0043bbc8  mov        dword ptr [edx + 0x18f80], 0xffffffff
0043bbd2  pop        ebx

// block 0043bbd3
0043bbd3  mov        eax, 1
0043bbd8  add        esp, 0x10
0043bbdb  ret        
