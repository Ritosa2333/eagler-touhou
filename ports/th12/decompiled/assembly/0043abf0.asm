
// block 0043abf0
0043abf0  mov        eax, dword ptr [0x4b4514]
0043abf5  cmp        dword ptr [eax + 0x8980], edx
0043abfb  jne        0x43ac0e

// block 0043abfd
0043abfd  mov        dword ptr [eax + 0x8980], 0
0043ac07  mov        byte ptr [eax + 0x8984], 0

// block 0043ac0e
0043ac0e  add        eax, 0xaac
0043ac13  mov        ecx, 0x100

// block 0043ac18
0043ac18  cmp        dword ptr [eax], edx
0043ac1a  jne        0x43ac22

// block 0043ac1c
0043ac1c  mov        dword ptr [eax], 0

// block 0043ac22
0043ac22  add        eax, 0x78
0043ac25  sub        ecx, 1
0043ac28  jne        0x43ac18

// block 0043ac2a
0043ac2a  ret        
