
// block 004646e0
004646e0  fld        dword ptr [esp + 4]
004646e4  xor        ecx, ecx
004646e6  fld        qword ptr [0x4a3cd8]
004646ec  fcom       st(1)
004646ee  fnstsw     ax
004646f0  fld        qword ptr [0x4a3cd0]
004646f6  test       ah, 5
004646f9  jnp        0x464701

// block 004646fb
004646fb  fstp       st(1)
004646fd  jmp        0x464722

// block 004646ff
004646ff  fxch       st(2)

// block 00464701
00464701  fsub       st(2), st(0)
00464703  mov        eax, ecx
00464705  fxch       st(2)
00464707  inc        ecx
00464708  cmp        eax, 0x20
0046470b  fstp       dword ptr [esp + 4]
0046470f  jg         0x464738

// block 00464711
00464711  fld        dword ptr [esp + 4]
00464715  fcom       st(1)
00464717  fnstsw     ax
00464719  test       ah, 0x41
0046471c  je         0x4646ff

// block 0046471e
0046471e  fstp       st(1)

// block 00464720
00464720  fxch       st(1)

// block 00464722
00464722  fld        qword ptr [0x4a3d08]
00464728  fcom       st(2)
0046472a  fnstsw     ax
0046472c  test       ah, 0x41
0046472f  je         0x464742

// block 00464731
00464731  fstp       st(0)
00464733  fstp       st(0)
00464735  ret        4

// block 00464738
00464738  fstp       st(0)
0046473a  fld        dword ptr [esp + 4]
0046473e  jmp        0x464720

// block 00464740
00464740  fxch       st(2)

// block 00464742
00464742  fxch       st(2)
00464744  mov        edx, ecx
00464746  fadd       st(1)
00464748  inc        ecx
00464749  cmp        edx, 0x20
0046474c  fstp       dword ptr [esp + 4]
00464750  jg         0x464766

// block 00464752
00464752  fld        dword ptr [esp + 4]
00464756  fcom       st(2)
00464758  fnstsw     ax
0046475a  test       ah, 5
0046475d  jnp        0x464740

// block 0046475f
0046475f  fstp       st(2)
00464761  fstp       st(0)
00464763  ret        4

// block 00464766
00464766  fstp       st(1)
00464768  fstp       st(0)
0046476a  fld        dword ptr [esp + 4]
0046476e  ret        4
