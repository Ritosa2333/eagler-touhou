
// block 00491611
00491611  mov        edi, edi
00491613  push       ebp
00491614  mov        ebp, esp
00491616  push       esi
00491617  mov        esi, ecx
00491619  mov        dword ptr [esi], 0x49f39c
0049161f  call       0x49155a

// block 00491624
00491624  test       byte ptr [ebp + 8], 1
00491628  je         0x491631

// block 0049162a
0049162a  push       esi
0049162b  call       0x46ca4f

// block 00491630
00491630  pop        ecx

// block 00491631
00491631  mov        eax, esi
00491633  pop        esi
00491634  pop        ebp
00491635  ret        4
