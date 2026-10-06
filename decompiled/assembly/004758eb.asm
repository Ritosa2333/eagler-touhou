
// block 004758eb
004758eb  mov        edi, edi
004758ed  push       ebp
004758ee  mov        ebp, esp
004758f0  sub        esp, 0x18
004758f3  push       ebx
004758f4  push       dword ptr [ebp + 0x10]
004758f7  lea        ecx, [ebp - 0x18]
004758fa  call       0x46d3b4

// block 004758ff
004758ff  mov        ebx, dword ptr [ebp + 8]
00475902  lea        eax, [ebx + 1]
00475905  cmp        eax, 0x100
0047590a  ja         0x47591b

// block 0047590c
0047590c  mov        eax, dword ptr [ebp - 0x18]
0047590f  mov        eax, dword ptr [eax + 0xc8]
00475915  movzx      eax, word ptr [eax + ebx*2]
00475919  jmp        0x475990

// block 0047591b
0047591b  mov        dword ptr [ebp + 8], ebx
0047591e  sar        dword ptr [ebp + 8], 8
00475922  lea        eax, [ebp - 0x18]
00475925  push       eax
00475926  mov        eax, dword ptr [ebp + 8]
00475929  and        eax, 0xff
0047592e  push       eax
0047592f  call       0x47c5a3

// block 00475934
00475934  pop        ecx
00475935  pop        ecx
00475936  test       eax, eax
00475938  je         0x47594c

// block 0047593a
0047593a  mov        al, byte ptr [ebp + 8]
0047593d  push       2
0047593f  mov        byte ptr [ebp - 8], al
00475942  mov        byte ptr [ebp - 7], bl
00475945  mov        byte ptr [ebp - 6], 0
00475949  pop        ecx
0047594a  jmp        0x475956

// block 0047594c
0047594c  xor        ecx, ecx
0047594e  mov        byte ptr [ebp - 8], bl
00475951  mov        byte ptr [ebp - 7], 0
00475955  inc        ecx

// block 00475956
00475956  mov        eax, dword ptr [ebp - 0x18]
00475959  push       1
0047595b  push       dword ptr [eax + 0x14]
0047595e  push       dword ptr [eax + 4]
00475961  lea        eax, [ebp - 4]
00475964  push       eax
00475965  push       ecx
00475966  lea        eax, [ebp - 8]
00475969  push       eax
0047596a  lea        eax, [ebp - 0x18]
0047596d  push       1
0047596f  push       eax
00475970  call       0x48384c

// block 00475975
00475975  add        esp, 0x20
00475978  test       eax, eax
0047597a  jne        0x47598c

// block 0047597c
0047597c  cmp        byte ptr [ebp - 0xc], al
0047597f  je         0x475988

// block 00475981
00475981  mov        eax, dword ptr [ebp - 0x10]
00475984  and        dword ptr [eax + 0x70], 0xfffffffd

// block 00475988
00475988  xor        eax, eax
0047598a  jmp        0x4759a0

// block 0047598c
0047598c  movzx      eax, word ptr [ebp - 4]

// block 00475990
00475990  and        eax, dword ptr [ebp + 0xc]
00475993  cmp        byte ptr [ebp - 0xc], 0
00475997  je         0x4759a0

// block 00475999
00475999  mov        ecx, dword ptr [ebp - 0x10]
0047599c  and        dword ptr [ecx + 0x70], 0xfffffffd

// block 004759a0
004759a0  pop        ebx
004759a1  leave      
004759a2  ret        
