
// block 00453cd0
00453cd0  push       ecx
00453cd1  or         eax, 0xffffffff
00453cd4  mov        dword ptr [esi + 0x20], eax
00453cd7  mov        dword ptr [esi + 0x24], eax
00453cda  mov        dword ptr [esi + 0x28], eax
00453cdd  mov        dword ptr [esi + 0x2c], eax
00453ce0  mov        dword ptr [esi + 0x30], eax
00453ce3  mov        dword ptr [esi + 0x34], eax
00453ce6  mov        dword ptr [esi + 0x38], eax
00453ce9  mov        dword ptr [esi + 0x3c], eax
00453cec  mov        dword ptr [esi + 0x40], eax
00453cef  mov        dword ptr [esi + 0x44], eax
00453cf2  mov        dword ptr [esi + 0x48], eax
00453cf5  mov        dword ptr [esi + 0x4c], eax
00453cf8  call       0x453030

// block 00453cfd
00453cfd  cmp        dword ptr [esi + 0x10], 0
00453d01  jne        0x453d08

// block 00453d03
00453d03  or         eax, 0xffffffff
00453d06  pop        ecx
00453d07  ret        

// block 00453d08
00453d08  cmp        dword ptr [esi], 0
00453d0b  je         0x453d87

// block 00453d0d
00453d0d  movsx      eax, byte ptr [0x4cead0]
00453d14  mov        dword ptr [esi + 0x5294], eax
00453d1a  movsx      eax, byte ptr [0x4cead1]
00453d21  mov        dword ptr [esi + 0x5298], eax
00453d27  test       eax, eax
00453d29  je         0x453d7d

// block 00453d2b
00453d2b  fild       dword ptr [esi + 0x5294]
00453d31  fdiv       qword ptr [0x4a3ce8]
00453d37  fstp       dword ptr [esp]
00453d3a  fld        dword ptr [esp]
00453d3d  fld1       
00453d3f  fld        st(0)
00453d41  fsubrp     st(2)
00453d43  fxch       st(1)
00453d45  fstp       dword ptr [esp]
00453d48  fld        dword ptr [esp]
00453d4b  fmul       st(0), st(0)
00453d4d  fstp       dword ptr [esp]
00453d50  fld        dword ptr [esp]
00453d53  fmul       st(0), st(0)
00453d55  fstp       dword ptr [esp]
00453d58  fsub       dword ptr [esp]
00453d5b  fstp       dword ptr [esp]
00453d5e  fld        dword ptr [esp]
00453d61  fmul       qword ptr [0x4a3dd8]
00453d67  call       0x4931e0

// block 00453d6c
00453d6c  mov        ecx, 0xffffec78
00453d71  sub        ecx, eax
00453d73  mov        dword ptr [esi + 0x529c], ecx
00453d79  xor        eax, eax
00453d7b  pop        ecx
00453d7c  ret        

// block 00453d7d
00453d7d  mov        dword ptr [esi + 0x529c], 0xffffd8f0

// block 00453d87
00453d87  xor        eax, eax
00453d89  pop        ecx
00453d8a  ret        
