
// block 004931e0
004931e0  cmp        dword ptr [0x4d52dc], 0
004931e7  je         0x493216

// block 004931e9
004931e9  push       ebp
004931ea  mov        ebp, esp
004931ec  sub        esp, 8
004931ef  and        esp, 0xfffffff8
004931f2  fstp       qword ptr [esp]
004931f5  cvttsd2si  eax, qword ptr [esp]
004931fa  leave      
004931fb  ret        

// block 00493216
00493216  push       ebp
00493217  mov        ebp, esp
00493219  sub        esp, 0x20
0049321c  and        esp, 0xfffffff0
0049321f  fld        st(0)
00493221  fst        dword ptr [esp + 0x18]
00493225  fistp      qword ptr [esp + 0x10]
00493229  fild       qword ptr [esp + 0x10]
0049322d  mov        edx, dword ptr [esp + 0x18]
00493231  mov        eax, dword ptr [esp + 0x10]
00493235  test       eax, eax
00493237  je         0x493275

// block 00493239
00493239  fsubp      st(1)
0049323b  test       edx, edx
0049323d  jns        0x49325d

// block 0049323f
0049323f  fstp       dword ptr [esp]
00493242  mov        ecx, dword ptr [esp]
00493245  xor        ecx, 0x80000000
0049324b  add        ecx, 0x7fffffff
00493251  adc        eax, 0
00493254  mov        edx, dword ptr [esp + 0x14]
00493258  adc        edx, 0
0049325b  jmp        0x493289

// block 0049325d
0049325d  fstp       dword ptr [esp]
00493260  mov        ecx, dword ptr [esp]
00493263  add        ecx, 0x7fffffff
00493269  sbb        eax, 0
0049326c  mov        edx, dword ptr [esp + 0x14]
00493270  sbb        edx, 0
00493273  jmp        0x493289

// block 00493275
00493275  mov        edx, dword ptr [esp + 0x14]
00493279  test       edx, 0x7fffffff
0049327f  jne        0x493239

// block 00493281
00493281  fstp       dword ptr [esp + 0x18]
00493285  fstp       dword ptr [esp + 0x18]

// block 00493289
00493289  leave      
0049328a  ret        
