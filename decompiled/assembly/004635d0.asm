
// block 004635d0
004635d0  sub        esp, 0x104
004635d6  mov        eax, dword ptr [0x4ad138]
004635db  xor        eax, esp
004635dd  mov        dword ptr [esp + 0x100], eax
004635e4  push       esi
004635e5  xor        esi, esi
004635e7  cmp        dword ptr [0x4cf3fc], esi
004635ed  je         0x4638ec

// block 004635f3
004635f3  test       dword ptr [0x4cee78], 0x400
004635fd  jne        0x46375d

// block 00463603
00463603  lea        eax, [esp + 4]
00463607  push       eax
00463608  call       dword ptr [0x498244]

// block 0046360e
0046360e  movzx      esi, byte ptr [esp + 0x56]
00463613  movzx      ecx, byte ptr [esp + 0x48]
00463618  movzx      edx, byte ptr [esp + 0x11]
0046361d  movzx      eax, byte ptr [esp + 0x54]
00463622  and        esi, 0x80
00463628  and        ecx, 0x80
0046362e  add        esi, esi
00463630  or         esi, ecx
00463632  movzx      ecx, byte ptr [esp + 0x28]
00463637  or         eax, ecx
00463639  movzx      ecx, byte ptr [esp + 0x15]
0046363e  and        eax, 0x80
00463643  add        esi, esi
00463645  and        edx, 0x80
0046364b  or         esi, edx
0046364d  movzx      edx, byte ptr [esp + 0x57]
00463652  add        esi, esi
00463654  or         esi, eax
00463656  movzx      eax, byte ptr [esp + 0x55]
0046365b  and        eax, 0x80
00463660  add        esi, esi
00463662  and        edx, 0x80
00463668  or         esi, edx
0046366a  movzx      edx, byte ptr [esp + 0x1f]
0046366f  and        ecx, 0x80
00463675  add        esi, esi
00463677  or         esi, eax
00463679  movzx      eax, byte ptr [esp + 0x5c]
0046367e  and        eax, 0x80
00463683  shl        esi, 7
00463686  or         esi, ecx
00463688  movzx      ecx, byte ptr [esp + 0x5e]
0046368d  shr        ecx, 1
0046368f  or         eax, ecx
00463691  movzx      ecx, byte ptr [esp + 0x6c]
00463696  and        edx, 0x80
0046369c  shr        eax, 2
0046369f  add        esi, esi
004636a1  or         esi, edx
004636a3  movzx      edx, byte ptr [esp + 0x14]
004636a8  and        edx, 0x80
004636ae  or         eax, edx
004636b0  movzx      edx, byte ptr [esp + 0x2a]
004636b5  or         ecx, edx
004636b7  movzx      edx, byte ptr [esp + 0x2c]
004636bc  shr        eax, 1
004636be  and        ecx, 0x80
004636c4  or         eax, ecx
004636c6  movzx      ecx, byte ptr [esp + 0x66]
004636cb  or         ecx, edx
004636cd  movzx      edx, byte ptr [esp + 0x29]
004636d2  shr        eax, 1
004636d4  and        ecx, 0x80
004636da  or         eax, ecx
004636dc  movzx      ecx, byte ptr [esp + 0x68]
004636e1  or         ecx, edx
004636e3  movzx      edx, byte ptr [esp + 0x67]
004636e8  shr        eax, 1
004636ea  and        ecx, 0x80
004636f0  or         eax, ecx
004636f2  movzx      ecx, byte ptr [esp + 0x2b]
004636f7  shr        eax, 1
004636f9  add        esi, esi
004636fb  or         esi, eax
004636fd  movzx      eax, byte ptr [esp + 0x6a]
00463702  or         eax, ecx
00463704  mov        cl, byte ptr [esp + 0x6d]
00463708  and        eax, 0x80
0046370d  or         esi, eax
0046370f  and        dl, 0x80
00463712  movzx      eax, dl
00463715  neg        eax
00463717  sbb        eax, eax
00463719  and        cl, 0x80
0046371c  and        eax, 0xa0
00463721  movzx      edx, cl
00463724  or         esi, eax
00463726  neg        edx
00463728  mov        al, byte ptr [esp + 0x65]
0046372c  sbb        edx, edx
0046372e  and        al, 0x80
00463730  and        edx, 0x90
00463736  movzx      ecx, al
00463739  or         esi, edx
0046373b  movzx      edx, byte ptr [esp + 0x6b]
00463740  neg        ecx
00463742  sbb        ecx, ecx
00463744  and        dl, 0x80
00463747  and        ecx, 0x60
0046374a  movzx      eax, dl
0046374d  or         esi, ecx
0046374f  neg        eax
00463751  sbb        eax, eax
00463753  and        eax, 0x50
00463756  or         esi, eax
00463758  jmp        0x4638ec

// block 0046375d
0046375d  mov        eax, dword ptr [0x4ce908]
00463762  mov        ecx, dword ptr [eax]
00463764  lea        edx, [esp + 4]
00463768  push       edx
00463769  push       0x100
0046376e  push       eax
0046376f  mov        eax, dword ptr [ecx + 0x24]
00463772  call       eax

// block 00463774
00463774  xor        esi, esi
00463776  cmp        eax, 0x8007001e
0046377b  je         0x463781

// block 0046377d
0046377d  test       eax, eax
0046377f  je         0x463793

// block 00463781
00463781  mov        eax, dword ptr [0x4ce908]
00463786  mov        ecx, dword ptr [eax]
00463788  mov        edx, dword ptr [ecx + 0x1c]
0046378b  push       eax
0046378c  call       edx

// block 0046378e
0046378e  jmp        0x4638ec

// block 00463793
00463793  movzx      esi, byte ptr [esp + 0x17]
00463798  movzx      eax, byte ptr [esp + 0x24]
0046379d  movzx      ecx, byte ptr [esp + 0x20]
004637a2  movzx      edx, byte ptr [esp + 0x1d]
004637a7  and        esi, 0x80
004637ad  and        eax, 0x80
004637b2  add        esi, esi
004637b4  or         esi, eax
004637b6  movzx      eax, byte ptr [esp + 0xcb]
004637be  or         edx, eax
004637c0  movzx      eax, byte ptr [esp + 0x21]
004637c5  and        edx, 0x80
004637cb  add        esi, esi
004637cd  and        ecx, 0x80
004637d3  or         esi, ecx
004637d5  movzx      ecx, byte ptr [esp + 0x23]
004637da  add        esi, esi
004637dc  or         esi, edx
004637de  movzx      edx, byte ptr [esp + 0x14]
004637e3  and        edx, 0x80
004637e9  and        ecx, 0x80
004637ef  add        esi, esi
004637f1  or         esi, ecx
004637f3  movzx      ecx, byte ptr [esp + 5]
004637f8  and        eax, 0x80
004637fd  add        esi, esi
004637ff  or         esi, edx
00463801  movzx      edx, byte ptr [esp + 0x31]
00463806  and        edx, 0x80
0046380c  shl        esi, 7
0046380f  or         esi, eax
00463811  movzx      eax, byte ptr [esp + 0x30]
00463816  and        ecx, 0x80
0046381c  shr        eax, 1
0046381e  or         edx, eax
00463820  movzx      eax, byte ptr [esp + 0x4c]
00463825  shr        edx, 2
00463828  add        esi, esi
0046382a  or         esi, ecx
0046382c  movzx      ecx, byte ptr [esp + 0x2e]
00463831  and        ecx, 0x80
00463837  or         edx, ecx
00463839  movzx      ecx, byte ptr [esp + 0xcc]
00463841  or         eax, ecx
00463843  movzx      ecx, byte ptr [esp + 0x54]
00463848  shr        edx, 1
0046384a  and        eax, 0x80
0046384f  or         edx, eax
00463851  movzx      eax, byte ptr [esp + 0xd4]
00463859  or         eax, ecx
0046385b  movzx      ecx, byte ptr [esp + 0xcf]
00463863  shr        edx, 1
00463865  and        eax, 0x80
0046386a  or         edx, eax
0046386c  movzx      eax, byte ptr [esp + 0x4f]
00463871  or         eax, ecx
00463873  movzx      ecx, byte ptr [esp + 0x4d]
00463878  shr        edx, 1
0046387a  and        eax, 0x80
0046387f  or         edx, eax
00463881  movzx      eax, byte ptr [esp + 0xd1]
00463889  shr        edx, 1
0046388b  add        esi, esi
0046388d  or         esi, edx
0046388f  movzx      edx, byte ptr [esp + 0x51]
00463894  or         edx, eax
00463896  mov        al, byte ptr [esp + 0x53]
0046389a  and        edx, 0x80
004638a0  and        cl, 0x80
004638a3  or         esi, edx
004638a5  movzx      edx, cl
004638a8  neg        edx
004638aa  sbb        edx, edx
004638ac  and        al, 0x80
004638ae  and        edx, 0x90
004638b4  movzx      ecx, al
004638b7  or         esi, edx
004638b9  neg        ecx
004638bb  sbb        ecx, ecx
004638bd  and        ecx, 0x60
004638c0  or         esi, ecx
004638c2  mov        dl, byte ptr [esp + 0x4b]
004638c6  movzx      ecx, byte ptr [esp + 0x55]
004638cb  and        dl, 0x80
004638ce  movzx      eax, dl
004638d1  neg        eax
004638d3  sbb        eax, eax
004638d5  and        cl, 0x80
004638d8  and        eax, 0x50
004638db  movzx      edx, cl
004638de  or         esi, eax
004638e0  neg        edx
004638e2  sbb        edx, edx
004638e4  and        edx, 0xa0
004638ea  or         esi, edx

// block 004638ec
004638ec  mov        ecx, esi
004638ee  call       0x462a80

// block 004638f3
004638f3  mov        ecx, dword ptr [esp + 0x104]
004638fa  pop        esi
004638fb  xor        ecx, esp
004638fd  call       0x46c932

// block 00463902
00463902  add        esp, 0x104
00463908  ret        
