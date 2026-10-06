
// block 0040b6f0
0040b6f0  push       ebp
0040b6f1  mov        ebp, esp
0040b6f3  and        esp, 0xfffffff8
0040b6f6  sub        esp, 0x14
0040b6f9  push       esi
0040b6fa  mov        esi, eax
0040b6fc  mov        eax, dword ptr [esi + 0x738]
0040b702  cmp        eax, dword ptr [esi + 0x75c]
0040b708  jl         0x40b71b

// block 0040b70a
0040b70a  and        dword ptr [esi + 0x528], 0xfffffffb
0040b711  mov        eax, 1
0040b716  pop        esi
0040b717  mov        esp, ebp
0040b719  pop        ebp
0040b71a  ret        

// block 0040b71b
0040b71b  fld        dword ptr [esi + 0x748]
0040b721  fmul       dword ptr [0x4b2ed0]
0040b727  fadd       dword ptr [esi + 0x4d4]
0040b72d  fstp       dword ptr [esi + 0x4d4]
0040b733  fld        dword ptr [esi + 0x750]
0040b739  fld        dword ptr [0x4b2ed0]
0040b73f  fld        st(0)
0040b741  fmulp      st(2)
0040b743  fxch       st(1)
0040b745  fstp       dword ptr [esp + 0xc]
0040b749  fld        dword ptr [esi + 0x754]
0040b74f  fmul       st(1)
0040b751  fstp       dword ptr [esp + 0x10]
0040b755  fmul       dword ptr [esi + 0x758]
0040b75b  fstp       dword ptr [esp + 0x14]
0040b75f  fld        dword ptr [esi + 0x4c8]
0040b765  fadd       dword ptr [esp + 0xc]
0040b769  fstp       dword ptr [esi + 0x4c8]
0040b76f  fld        dword ptr [esi + 0x4cc]
0040b775  fadd       dword ptr [esp + 0x10]
0040b779  fstp       dword ptr [esi + 0x4cc]
0040b77f  fld        dword ptr [esp + 0x14]
0040b783  fadd       dword ptr [esi + 0x4d0]
0040b789  fstp       dword ptr [esi + 0x4d0]
0040b78f  fld        dword ptr [esi + 0x4c8]
0040b795  fabs       
0040b797  fstp       dword ptr [esp + 8]
0040b79b  fld        dword ptr [esp + 8]
0040b79f  fld        qword ptr [0x4a3ff0]
0040b7a5  fcom       st(1)
0040b7a7  fnstsw     ax
0040b7a9  fstp       st(1)
0040b7ab  test       ah, 5
0040b7ae  jnp        0x40b7cb

// block 0040b7b0
0040b7b0  fld        dword ptr [esi + 0x4cc]
0040b7b6  fabs       
0040b7b8  fstp       dword ptr [esp + 8]
0040b7bc  fld        dword ptr [esp + 8]
0040b7c0  fcompp     
0040b7c2  fnstsw     ax
0040b7c4  test       ah, 0x41
0040b7c7  jne        0x40b7ec

// block 0040b7c9
0040b7c9  jmp        0x40b7cd

// block 0040b7cb
0040b7cb  fstp       st(0)

// block 0040b7cd
0040b7cd  fld        dword ptr [esi + 0x4cc]
0040b7d3  fld        dword ptr [esi + 0x4c8]
0040b7d9  call       0x4937aa

// block 0040b7de
0040b7de  fstp       dword ptr [esp + 8]
0040b7e2  fld        dword ptr [esp + 8]
0040b7e6  fstp       dword ptr [esi + 0x4d8]

// block 0040b7ec
0040b7ec  add        esi, 0x734
0040b7f2  call       0x464a80

// block 0040b7f7
0040b7f7  xor        eax, eax
0040b7f9  pop        esi
0040b7fa  mov        esp, ebp
0040b7fc  pop        ebp
0040b7fd  ret        
