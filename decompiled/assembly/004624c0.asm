
// block 004624c0
004624c0  sub        esp, 8
004624c3  test       dword ptr [0x4cee78], 0x8000
004624cd  push       ebp
004624ce  mov        ebp, dword ptr [0x498088]
004624d4  push       esi
004624d5  push       edi
004624d6  je         0x4624f0

// block 004624d8
004624d8  push       0x4cf0f8
004624dd  call       ebp

// block 004624df
004624df  inc        byte ptr [0x4cf218]
004624e5  jmp        0x4624f0

// block 004624f0
004624f0  mov        eax, dword ptr [ebx + 0x18]
004624f3  mov        dword ptr [esp + 0xc], 0
004624fb  jmp        0x462500

// block 00462500
00462500  test       eax, eax
00462502  je         0x4625d5

// block 00462508
00462508  mov        esi, dword ptr [eax]
0046250a  cmp        dword ptr [esi + 8], 0
0046250e  mov        ecx, dword ptr [eax + 4]
00462511  mov        dword ptr [esp + 0x10], ecx
00462515  je         0x4625b0

// block 0046251b
0046251b  test       byte ptr [esi + 4], 2
0046251f  je         0x4625ac

// block 00462525
00462525  cmp        dword ptr [ebx + 0x48], 0
00462529  jne        0x4625a0

// block 0046252b
0046252b  test       dword ptr [0x4cee78], 0x8000
00462535  je         0x462548

// block 00462537
00462537  push       0x4cf0f8
0046253c  call       dword ptr [0x49808c]

// block 00462542
00462542  dec        byte ptr [0x4cf218]

// block 00462548
00462548  mov        ecx, dword ptr [esi + 0x20]
0046254b  mov        edx, dword ptr [esi + 8]
0046254e  call       edx

// block 00462550
00462550  test       dword ptr [0x4cee78], 0x8000
0046255a  mov        edi, eax
0046255c  je         0x46256b

// block 0046255e
0046255e  push       0x4cf0f8
00462563  call       ebp

// block 00462565
00462565  inc        byte ptr [0x4cf218]

// block 0046256b
0046256b  cmp        edi, 8
0046256e  ja         0x4625ac

// block 00462570
00462570  jmp        dword ptr [edi*4 + 0x4625fc]

// block 00462577
00462577  test       byte ptr [esi + 4], 2
0046257b  jne        0x462525

// block 0046257d
0046257d  inc        dword ptr [esp + 0xc]
00462581  mov        eax, dword ptr [esp + 0x10]
00462585  jmp        0x462500

// block 0046258a
0046258a  mov        ecx, esi
0046258c  mov        edx, ebx
0046258e  call       0x462890

// block 00462593
00462593  inc        dword ptr [esp + 0xc]
00462597  mov        eax, dword ptr [esp + 0x10]
0046259b  jmp        0x462500

// block 004625a0
004625a0  mov        eax, dword ptr [esi + 0x10]
004625a3  test       eax, eax
004625a5  je         0x4625ac

// block 004625a7
004625a7  mov        ecx, dword ptr [esi + 0x20]
004625aa  call       eax

// block 004625ac
004625ac  inc        dword ptr [esp + 0xc]

// block 004625b0
004625b0  mov        eax, dword ptr [esp + 0x10]
004625b4  jmp        0x462500

// block 004625b9
004625b9  mov        dword ptr [esp + 0xc], 0
004625c1  jmp        0x4625d5

// block 004625c3
004625c3  mov        dword ptr [esp + 0xc], 1
004625cb  jmp        0x4625d5

// block 004625cd
004625cd  mov        dword ptr [esp + 0xc], 0xffffffff

// block 004625d5
004625d5  test       dword ptr [0x4cee78], 0x8000
004625df  pop        edi
004625e0  pop        esi
004625e1  pop        ebp
004625e2  je         0x4625f5

// block 004625e4
004625e4  push       0x4cf0f8
004625e9  call       dword ptr [0x49808c]

// block 004625ef
004625ef  dec        byte ptr [0x4cf218]

// block 004625f5
004625f5  mov        eax, dword ptr [esp]
004625f8  add        esp, 8
004625fb  ret        
