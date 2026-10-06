
// block 004435b0
004435b0  push       ecx
004435b1  push       esi
004435b2  push       edi
004435b3  mov        edi, eax
004435b5  mov        eax, dword ptr [edi + 0x24]
004435b8  cmp        eax, 4
004435bb  ja         0x4438f8

// block 004435c1
004435c1  push       ebx
004435c2  jmp        dword ptr [eax*4 + 0x443904]

// block 004435c9
004435c9  mov        dword ptr [edi + 0x30], 7
004435d0  mov        eax, dword ptr [edi + 0x30]
004435d3  test       eax, eax
004435d5  je         0x4435e6

// block 004435d7
004435d7  jg         0x4435df

// block 004435d9
004435d9  dec        eax
004435da  mov        dword ptr [edi + 0x28], eax
004435dd  jmp        0x4435ed

// block 004435df
004435df  xor        eax, eax
004435e1  mov        dword ptr [edi + 0x28], eax
004435e4  jmp        0x4435ed

// block 004435e6
004435e6  mov        dword ptr [edi + 0x28], 0

// block 004435ed
004435ed  mov        ecx, dword ptr [edi + 0x14]
004435f0  push       0
004435f2  push       2
004435f4  lea        eax, [esp + 0x14]
004435f8  push       eax
004435f9  push       ecx
004435fa  mov        eax, 0x17
004435ff  xor        ecx, ecx
00443601  call       0x4615a0

// block 00443606
00443606  mov        edx, dword ptr [esp + 0xc]
0044360a  mov        ecx, 1
0044360f  mov        eax, edi
00443611  mov        dword ptr [edi + 0x2d0], edx
00443617  call       0x43ef40

// block 0044361c
0044361c  movzx      eax, word ptr [0x4d49ec]
00443623  mov        word ptr [edi + 0x5a68], ax
0044362a  movzx      ecx, word ptr [0x4d49ee]
00443631  mov        word ptr [edi + 0x5a6a], cx
00443638  mov        dx, word ptr [0x4d49f0]
0044363f  mov        word ptr [edi + 0x5a6c], dx
00443646  movzx      eax, word ptr [0x4d49f2]
0044364d  mov        word ptr [edi + 0x5a6e], ax
00443654  movzx      ecx, word ptr [0x4d49fc]
0044365b  mov        word ptr [edi + 0x5a70], cx
00443662  call       0x443920

// block 00443667
00443667  cmp        dword ptr [edi + 0x2b8], 6
0044366e  jle        0x4438f7

// block 00443674
00443674  mov        ecx, 2
00443679  mov        eax, edi
0044367b  call       0x43ef40

// block 00443680
00443680  mov        edx, dword ptr [edi + 0x2d0]
00443686  push       edx
00443687  mov        ebx, 3
0044368c  call       0x4619e0

// block 00443691
00443691  movzx      ebx, word ptr [edi + 0x28]
00443695  mov        eax, dword ptr [edi + 0x2d0]
0044369b  add        bx, 0x11
0044369f  push       eax
004436a0  call       0x461970

// block 004436a5
004436a5  push       edi
004436a6  call       0x444380

// block 004436ab
004436ab  pop        ebx
004436ac  pop        edi
004436ad  mov        eax, 1
004436b2  pop        esi
004436b3  pop        ecx
004436b4  ret        

// block 004436b5
004436b5  mov        ecx, dword ptr [edi + 0x28]
004436b8  lea        esi, [edi + 0x28]
004436bb  mov        al, 0x10
004436bd  mov        dword ptr [esi + 4], ecx
004436c0  test       byte ptr [0x4d48c4], al
004436c6  jne        0x4436d0

// block 004436c8
004436c8  test       byte ptr [0x4d48c0], al
004436ce  je         0x4436d9

// block 004436d0
004436d0  push       -1
004436d2  mov        eax, esi
004436d4  call       0x464970

// block 004436d9
004436d9  mov        al, 0x20
004436db  test       byte ptr [0x4d48c4], al
004436e1  jne        0x4436eb

// block 004436e3
004436e3  test       byte ptr [0x4d48c0], al
004436e9  je         0x4436f4

// block 004436eb
004436eb  push       1
004436ed  mov        eax, esi
004436ef  call       0x464970

// block 004436f4
004436f4  mov        edx, dword ptr [esi + 4]
004436f7  cmp        edx, dword ptr [esi]
004436f9  je         0x44372f

// block 004436fb
004436fb  mov        edx, 0xa
00443700  call       0x453d90

// block 00443705
00443705  mov        eax, dword ptr [edi + 0x2d0]
0044370b  push       eax
0044370c  mov        ebx, 3
00443711  call       0x4619e0

// block 00443716
00443716  movzx      ebx, word ptr [esi]
00443719  mov        ecx, dword ptr [edi + 0x2d0]
0044371f  add        bx, 7
00443723  push       ecx
00443724  call       0x461970

// block 00443729
00443729  push       edi
0044372a  call       0x444380

// block 0044372f
0044372f  call       0x462db0

// block 00443734
00443734  xor        ecx, ecx

// block 00443736
00443736  test       byte ptr [ecx + eax], 0x80
0044373a  jne        0x443744

// block 0044373c
0044373c  inc        ecx
0044373d  cmp        ecx, 0x1f
00443740  jl         0x443736

// block 00443742
00443742  jmp        0x443752

// block 00443744
00443744  mov        eax, dword ptr [esi]
00443746  cmp        eax, 4
00443749  jg         0x443752

// block 0044374b
0044374b  mov        edx, edi
0044374d  call       0x4442b0

// block 00443752
00443752  mov        eax, dword ptr [0x4d48c4]
00443757  test       eax, 0x102
0044375c  je         0x4437b3

// block 0044375e
0044375e  cmp        dword ptr [esi], 6
00443761  jne        0x4437b3

// block 00443763
00443763  movzx      edx, word ptr [0x4d49ec]
0044376a  mov        word ptr [edi + 0x5a68], dx
00443771  movzx      eax, word ptr [0x4d49ee]
00443778  mov        word ptr [edi + 0x5a6a], ax
0044377f  mov        cx, word ptr [0x4d49f0]
00443786  mov        word ptr [edi + 0x5a6c], cx
0044378d  movzx      edx, word ptr [0x4d49f2]
00443794  mov        word ptr [edi + 0x5a6e], dx
0044379b  movzx      eax, word ptr [0x4d49fc]
004437a2  mov        word ptr [edi + 0x5a70], ax
004437a9  call       0x443920

// block 004437ae
004437ae  jmp        0x44384c

// block 004437b3
004437b3  test       eax, 0x80001
004437b8  je         0x4438f7

// block 004437be
004437be  mov        esi, dword ptr [esi]
004437c0  sub        esi, 5
004437c3  je         0x44387b

// block 004437c9
004437c9  sub        esi, 1
004437cc  jne        0x4438f7

// block 004437d2
004437d2  movzx      ecx, word ptr [edi + 0x5a68]
004437d9  mov        word ptr [0x4d49ec], cx
004437e0  mov        dx, word ptr [edi + 0x5a6a]
004437e7  mov        word ptr [0x4d49ee], dx
004437ee  movzx      eax, word ptr [edi + 0x5a6c]
004437f5  mov        edx, dword ptr [0x4d49ec]
004437fb  mov        word ptr [0x4d49f0], ax
00443801  movzx      ecx, word ptr [edi + 0x5a6e]
00443808  mov        word ptr [0x4d49f2], cx
0044380f  mov        ax, word ptr [edi + 0x5a70]
00443816  mov        ecx, dword ptr [0x4d49f0]
0044381c  mov        dword ptr [0x4ceab4], edx
00443822  mov        edx, dword ptr [0x4d49f4]
00443828  mov        dword ptr [0x4ceab8], ecx
0044382e  mov        ecx, dword ptr [0x4d49f8]
00443834  mov        word ptr [0x4d49fc], ax
0044383a  mov        dword ptr [0x4ceabc], edx
00443840  mov        dword ptr [0x4ceac0], ecx
00443846  mov        word ptr [0x4ceac4], ax

// block 0044384c
0044384c  mov        edx, 9
00443851  call       0x453d90

// block 00443856
00443856  mov        edx, dword ptr [edi + 0x2d0]
0044385c  push       edx
0044385d  mov        ebx, 6
00443862  call       0x461970

// block 00443867
00443867  lea        ecx, [ebx - 2]
0044386a  mov        eax, edi
0044386c  call       0x43ef40

// block 00443871
00443871  pop        ebx
00443872  pop        edi
00443873  mov        eax, 1
00443878  pop        esi
00443879  pop        ecx
0044387a  ret        

// block 0044387b
0044387b  movzx      eax, word ptr [0x4d49ec]
00443882  mov        word ptr [edi + 0x5a68], ax
00443889  movzx      ecx, word ptr [0x4d49ee]
00443890  mov        word ptr [edi + 0x5a6a], cx
00443897  mov        dx, word ptr [0x4d49f0]
0044389e  mov        word ptr [edi + 0x5a6c], dx
004438a5  movzx      eax, word ptr [0x4d49f2]
004438ac  mov        word ptr [edi + 0x5a6e], ax
004438b3  movzx      ecx, word ptr [0x4d49fc]
004438ba  mov        word ptr [edi + 0x5a70], cx
004438c1  call       0x443920

// block 004438c6
004438c6  mov        edx, 7
004438cb  call       0x453d90

// block 004438d0
004438d0  pop        ebx
004438d1  pop        edi
004438d2  mov        eax, 1
004438d7  pop        esi
004438d8  pop        ecx
004438d9  ret        

// block 004438da
004438da  cmp        dword ptr [edi + 0x2b8], 0xa
004438e1  jl         0x4438f7

// block 004438e3
004438e3  mov        edx, 3
004438e8  mov        eax, edi
004438ea  call       0x43eee0

// block 004438ef
004438ef  lea        eax, [edi + 0x28]
004438f2  call       0x464940

// block 004438f7
004438f7  pop        ebx

// block 004438f8
004438f8  pop        edi
004438f9  mov        eax, 1
004438fe  pop        esi
004438ff  pop        ecx
00443900  ret        
