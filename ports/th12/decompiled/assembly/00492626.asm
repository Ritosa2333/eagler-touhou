
// block 00492626
00492626  mov        edi, edi
00492628  push       ebp
00492629  mov        ebp, esp
0049262b  push       esi
0049262c  mov        esi, dword ptr [ebp + 0xc]
0049262f  push       edi
00492630  test       esi, esi
00492632  jne        0x492639

// block 00492634
00492634  call       0x472b94

// block 00492639
00492639  mov        esi, dword ptr [esi]
0049263b  test       esi, esi
0049263d  jne        0x492644

// block 0049263f
0049263f  call       0x472b94

// block 00492644
00492644  cmp        dword ptr [esi], 0xe06d7363
0049264a  jne        0x49266a

// block 0049264c
0049264c  cmp        dword ptr [esi + 0x10], 3
00492650  jne        0x49266a

// block 00492652
00492652  mov        eax, dword ptr [esi + 0x14]
00492655  cmp        eax, 0x19930520
0049265a  je         0x49266f

// block 0049265c
0049265c  cmp        eax, 0x19930521
00492661  je         0x49266f

// block 00492663
00492663  cmp        eax, 0x19930522
00492668  je         0x49266f

// block 0049266a
0049266a  call       0x472b94

// block 0049266f
0049266f  mov        eax, dword ptr [esi + 0x1c]
00492672  mov        eax, dword ptr [eax + 0xc]
00492675  mov        edi, dword ptr [eax]
00492677  lea        esi, [eax + 4]
0049267a  jmp        0x49269d

// block 0049267c
0049267c  mov        eax, dword ptr [esi]
0049267e  mov        eax, dword ptr [eax + 4]
00492681  mov        ecx, dword ptr [ebp + 8]
00492684  add        eax, 8
00492687  push       eax
00492688  call       0x46ce35

// block 0049268d
0049268d  push       eax
0049268e  call       0x472ac0

// block 00492693
00492693  pop        ecx
00492694  pop        ecx
00492695  test       eax, eax
00492697  je         0x4926a7

// block 00492699
00492699  dec        edi
0049269a  add        esi, 4

// block 0049269d
0049269d  test       edi, edi
0049269f  jg         0x49267c

// block 004926a1
004926a1  xor        eax, eax

// block 004926a3
004926a3  pop        edi
004926a4  pop        esi
004926a5  pop        ebp
004926a6  ret        

// block 004926a7
004926a7  xor        eax, eax
004926a9  inc        eax
004926aa  jmp        0x4926a3
