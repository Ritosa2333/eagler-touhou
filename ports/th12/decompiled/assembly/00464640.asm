
// block 00464640
00464640  fld        dword ptr [esp + 4]
00464644  xor        ecx, ecx
00464646  fadd       dword ptr [esp + 8]
0046464a  fstp       dword ptr [esp + 4]
0046464e  fld        dword ptr [esp + 4]
00464652  fld        qword ptr [0x4a3cd8]
00464658  fcom       st(1)
0046465a  fnstsw     ax
0046465c  fld        qword ptr [0x4a3cd0]
00464662  test       ah, 5
00464665  jnp        0x46466d

// block 00464667
00464667  fstp       st(1)
00464669  jmp        0x46468e

// block 0046466b
0046466b  fxch       st(2)

// block 0046466d
0046466d  fsub       st(2), st(0)
0046466f  mov        eax, ecx
00464671  fxch       st(2)
00464673  inc        ecx
00464674  cmp        eax, 0x20
00464677  fstp       dword ptr [esp + 4]
0046467b  jg         0x4646a4

// block 0046467d
0046467d  fld        dword ptr [esp + 4]
00464681  fcom       st(1)
00464683  fnstsw     ax
00464685  test       ah, 0x41
00464688  je         0x46466b

// block 0046468a
0046468a  fstp       st(1)

// block 0046468c
0046468c  fxch       st(1)

// block 0046468e
0046468e  fld        qword ptr [0x4a3d08]
00464694  fcom       st(2)
00464696  fnstsw     ax
00464698  test       ah, 0x41
0046469b  je         0x4646ae

// block 0046469d
0046469d  fstp       st(0)
0046469f  fstp       st(0)
004646a1  ret        8

// block 004646a4
004646a4  fstp       st(0)
004646a6  fld        dword ptr [esp + 4]
004646aa  jmp        0x46468c

// block 004646ac
004646ac  fxch       st(2)

// block 004646ae
004646ae  fxch       st(2)
004646b0  mov        edx, ecx
004646b2  fadd       st(1)
004646b4  inc        ecx
004646b5  cmp        edx, 0x20
004646b8  fstp       dword ptr [esp + 4]
004646bc  jg         0x4646d2

// block 004646be
004646be  fld        dword ptr [esp + 4]
004646c2  fcom       st(2)
004646c4  fnstsw     ax
004646c6  test       ah, 5
004646c9  jnp        0x4646ac

// block 004646cb
004646cb  fstp       st(2)
004646cd  fstp       st(0)
004646cf  ret        8

// block 004646d2
004646d2  fstp       st(1)
004646d4  fstp       st(0)
004646d6  fld        dword ptr [esp + 4]
004646da  ret        8
