
// block 004dac60
004dac60  pop        ecx
004dac61  stosd      dword ptr es:[edi], eax
004dac62  mov        byte ptr [0x45e292d1], al
004dac67  jle        0x4daca5

// block 004dac69
004dac69  push       edi
004dac6a  aaa        
004dac6b  int        0x69

// block 004daca5
004daca5  jb         0x4dad17

// block 004daca7
004daca7  cmpsd      dword ptr [esi], dword ptr es:[edi]
004daca8  dec        edi
004daca9  pushal     
004dacaa  ljmp       0x975b:0x26cbb9ec

// block 004dad17
004dad17  push       ss
004dad18  stosd      dword ptr es:[edi], eax
004dad19  and        al, 0xbe
004dad1b  sub        esp, ecx
004dad1d  test       eax, 0x96e4a6a0
004dad22  mov        cl, 0x86
004dad24  mov        edx, 0xa829f3f
004dad29  xchg       ebp, eax
