
// block 004548a0
004548a0  sub        esp, 0x20
004548a3  push       ebx
004548a4  mov        ebx, dword ptr [0x498254]
004548aa  push       ebp
004548ab  mov        ebp, dword ptr [0x49821c]
004548b1  push       esi
004548b2  mov        dword ptr [esp + 0xc], 0
004548ba  mov        esi, 1
004548bf  nop        

// block 004548c0
004548c0  push       0x4bf
004548c5  push       -1
004548c7  push       0
004548c9  push       0x4d4758
004548ce  push       esi
004548cf  call       ebp

// block 004548d1
004548d1  mov        ecx, dword ptr [0x4d4754]
004548d7  test       ecx, ecx
004548d9  jne        0x4548df

// block 004548db
004548db  mov        dword ptr [esp + 0xc], esi

// block 004548df
004548df  sub        eax, 0
004548e2  je         0x454916

// block 004548e4
004548e4  sub        eax, esi
004548e6  jne        0x45493b

// block 004548e8
004548e8  push       esi
004548e9  push       eax
004548ea  push       eax
004548eb  push       eax
004548ec  lea        eax, [esp + 0x20]
004548f0  push       eax
004548f1  call       ebx

// block 004548f3
004548f3  test       eax, eax
004548f5  je         0x45493b

// block 004548f7
004548f7  cmp        dword ptr [esp + 0x14], 0x12
004548fc  jne        0x454902

// block 004548fe
004548fe  mov        dword ptr [esp + 0xc], esi

// block 00454902
00454902  push       esi
00454903  push       0
00454905  push       0
00454907  push       0
00454909  lea        ecx, [esp + 0x20]
0045490d  push       ecx
0045490e  call       ebx

// block 00454910
00454910  test       eax, eax
00454912  jne        0x4548f7

// block 00454914
00454914  jmp        0x45493b

// block 00454916
00454916  test       ecx, ecx
00454918  je         0x45493b

// block 0045491a
0045491a  cmp        dword ptr [ecx + 0x30], 0
0045491e  je         0x45493b

// block 00454920
00454920  mov        dword ptr [ecx + 0x78], esi
00454923  mov        edx, dword ptr [0x4d4754]
00454929  push       edx
0045492a  call       0x4667d0

// block 0045492f
0045492f  mov        eax, dword ptr [0x4d4754]
00454934  mov        dword ptr [eax + 0x78], 0

// block 0045493b
0045493b  cmp        dword ptr [esp + 0xc], 0
00454940  je         0x4548c0

// block 00454946
00454946  pop        esi
00454947  pop        ebp
00454948  xor        eax, eax
0045494a  pop        ebx
0045494b  add        esp, 0x20
0045494e  ret        4
