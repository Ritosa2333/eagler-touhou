
// block 0040ce20
0040ce20  sub        esp, 0x10
0040ce23  push       ebp
0040ce24  push       esi
0040ce25  mov        esi, eax
0040ce27  mov        eax, dword ptr [0x4b43cc]
0040ce2c  add        esi, 0x64
0040ce2f  test       byte ptr [eax + 0x7c], 1
0040ce33  push       edi
0040ce34  je         0x40ce51

// block 0040ce36
0040ce36  mov        eax, dword ptr [eax + 0x78]
0040ce39  cmp        eax, 0x60
0040ce3c  jl         0x40ce51

// block 0040ce3e
0040ce3e  cmp        eax, 0x63
0040ce41  jg         0x40ce51

// block 0040ce43
0040ce43  fld        dword ptr [esp + 0x20]
0040ce47  fdiv       qword ptr [0x4a3fd8]
0040ce4d  fstp       dword ptr [esp + 0x20]

// block 0040ce51
0040ce51  mov        ebp, 0x7d0
0040ce56  jmp        0x40ce60

// block 0040ce60
0040ce60  movzx      eax, word ptr [esi + 0x532]
0040ce67  test       ax, ax
0040ce6a  je         0x40cf31

// block 0040ce70
0040ce70  cmp        ax, 3
0040ce74  je         0x40cf31

// block 0040ce7a
0040ce7a  cmp        dword ptr [esp + 0x28], 0
0040ce7f  je         0x40ce8b

// block 0040ce81
0040ce81  cmp        dword ptr [esi + 4], 0
0040ce85  jne        0x40cf31

// block 0040ce8b
0040ce8b  fld        dword ptr [esi + 0x4bc]
0040ce91  lea        edi, [esi + 0x4bc]
0040ce97  fsub       dword ptr [ebx]
0040ce99  fstp       dword ptr [esp + 0x10]
0040ce9d  fld        dword ptr [esi + 0x4c0]
0040cea3  fsub       dword ptr [ebx + 4]
0040cea6  fstp       dword ptr [esp + 0x14]
0040ceaa  fld        dword ptr [esi + 0x4dc]
0040ceb0  fmul       qword ptr [0x4a3cf8]
0040ceb6  fadd       dword ptr [esp + 0x20]
0040ceba  fstp       dword ptr [esp + 0xc]
0040cebe  fld        dword ptr [esp + 0x14]
0040cec2  fld        dword ptr [esp + 0x10]
0040cec6  fld        dword ptr [esp + 0xc]
0040ceca  fld        st(1)
0040cecc  fmulp      st(2)
0040cece  fld        st(2)
0040ced0  fmulp      st(3)
0040ced2  fxch       st(1)
0040ced4  faddp      st(2)
0040ced6  fxch       st(1)
0040ced8  fstp       dword ptr [esp + 0xc]
0040cedc  fld        dword ptr [esp + 0xc]
0040cee0  fld        st(1)
0040cee2  fmulp      st(2)
0040cee4  fcompp     
0040cee6  fnstsw     ax
0040cee8  test       ah, 0x41
0040ceeb  je         0x40cf31

// block 0040ceed
0040ceed  call       0x40c8b0

// block 0040cef2
0040cef2  fld        dword ptr [0x4a4014]
0040cef8  sub        esp, 8
0040cefb  fst        dword ptr [esp + 4]
0040ceff  mov        ecx, edi
0040cf01  fstp       dword ptr [esp]
0040cf04  call       0x40d560

// block 0040cf09
0040cf09  test       eax, eax
0040cf0b  jne        0x40cf31

// block 0040cf0d
0040cf0d  cmp        dword ptr [esp + 0x24], eax
0040cf11  je         0x40cf31

// block 0040cf13
0040cf13  fld        dword ptr [0x4a41f4]
0040cf19  sub        esp, 8
0040cf1c  fstp       dword ptr [esp + 4]
0040cf20  fld        dword ptr [0x4a4060]
0040cf26  fstp       dword ptr [esp]
0040cf29  push       edi
0040cf2a  push       9
0040cf2c  call       0x4273f0

// block 0040cf31
0040cf31  add        esi, 0x9f8
0040cf37  sub        ebp, 1
0040cf3a  jne        0x40ce60

// block 0040cf40
0040cf40  mov        eax, dword ptr [esp + 0x24]
0040cf44  fld        dword ptr [esp + 0x20]
0040cf48  push       eax
0040cf49  push       ecx
0040cf4a  fstp       dword ptr [esp]
0040cf4d  push       ebx
0040cf4e  call       0x415340

// block 0040cf53
0040cf53  pop        edi
0040cf54  pop        esi
0040cf55  xor        eax, eax
0040cf57  pop        ebp
0040cf58  add        esp, 0x10
0040cf5b  ret        0xc
