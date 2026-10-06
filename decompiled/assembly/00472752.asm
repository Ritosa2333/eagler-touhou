
// block 00472752
00472752  push       0xc
00472754  push       0x4aab40
00472759  call       0x46fcec

// block 0047275e
0047275e  push       0xe
00472760  call       0x46ec9a

// block 00472765
00472765  pop        ecx
00472766  and        dword ptr [ebp - 4], 0
0047276a  mov        esi, dword ptr [ebp + 8]
0047276d  mov        ecx, dword ptr [esi + 4]
00472770  test       ecx, ecx
00472772  je         0x4727a3

// block 00472774
00472774  mov        eax, dword ptr [0x4b3d68]
00472779  mov        edx, 0x4b3d64

// block 0047277e
0047277e  mov        dword ptr [ebp - 0x1c], eax
00472781  test       eax, eax
00472783  je         0x472796

// block 00472785
00472785  cmp        dword ptr [eax], ecx
00472787  jne        0x4727b5

// block 00472789
00472789  mov        ecx, dword ptr [eax + 4]
0047278c  mov        dword ptr [edx + 4], ecx
0047278f  push       eax
00472790  call       0x46c941

// block 00472795
00472795  pop        ecx

// block 00472796
00472796  push       dword ptr [esi + 4]
00472799  call       0x46c941

// block 0047279e
0047279e  pop        ecx
0047279f  and        dword ptr [esi + 4], 0

// block 004727a3
004727a3  mov        dword ptr [ebp - 4], 0xfffffffe
004727aa  call       0x4727b9

// block 004727af
004727af  call       0x46fd31

// block 004727b4
004727b4  ret        

// block 004727b5
004727b5  mov        edx, eax
004727b7  jmp        0x47277e
