
// block 00462740
00462740  mov        eax, dword ptr [eax + 0x18]
00462743  test       eax, eax
00462745  je         0x4627a7

// block 00462747
00462747  push       ebx
00462748  mov        ebx, dword ptr [0x49808c]
0046274e  push       ebp
0046274f  mov        ebp, dword ptr [0x498088]
00462755  push       esi
00462756  push       edi

// block 00462757
00462757  mov        esi, dword ptr [eax]
00462759  mov        edi, dword ptr [eax + 4]
0046275c  test       esi, esi
0046275e  je         0x46279d

// block 00462760
00462760  test       dword ptr [0x4cee78], 0x8000
0046276a  je         0x462779

// block 0046276c
0046276c  push       0x4cf0f8
00462771  call       ebp

// block 00462773
00462773  inc        byte ptr [0x4cf218]

// block 00462779
00462779  mov        edx, dword ptr [esp + 0x14]
0046277d  mov        ecx, esi
0046277f  call       0x462890

// block 00462784
00462784  test       dword ptr [0x4cee78], 0x8000
0046278e  je         0x46279d

// block 00462790
00462790  push       0x4cf0f8
00462795  call       ebx

// block 00462797
00462797  dec        byte ptr [0x4cf218]

// block 0046279d
0046279d  mov        eax, edi
0046279f  test       edi, edi
004627a1  jne        0x462757

// block 004627a3
004627a3  pop        edi
004627a4  pop        esi
004627a5  pop        ebp
004627a6  pop        ebx

// block 004627a7
004627a7  ret        4
