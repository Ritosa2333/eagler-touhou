
// block 004dbe85
004dbe85  pop        ebp
004dbe86  mov        ebx, 0xffffffed
004dbe8b  add        ebx, ebp
004dbe8d  sub        ebx, 0x6000
004dbe93  cmp        dword ptr [ebp + 0x488], 0
004dbe9a  mov        dword ptr [ebp + 0x488], ebx
004dbea0  jne        0x4dc271

// block 004dbea6
004dbea6  lea        eax, [ebp + 0x494]
004dbeac  push       eax
004dbead  call       dword ptr [ebp + 0xfa9]

// block 004dbeb3
004dbeb3  mov        dword ptr [ebp + 0x48c], eax
004dbeb9  mov        esi, eax
004dbebb  lea        edi, [ebp + 0x51]

// block 004dbebe
004dbebe  push       edi
004dbebf  push       esi
004dbec0  call       dword ptr [ebp + 0xfa5]

// block 004dbec6
004dbec6  stosd      dword ptr es:[edi], eax
004dbec7  mov        al, 0

// block 004dbec9
004dbec9  scasb      al, byte ptr es:[edi]
004dbeca  jne        0x4dbec9

// block 004dbecc
004dbecc  cmp        byte ptr [edi], al
004dbece  jne        0x4dbebe

// block 004dbed0
004dbed0  lea        eax, [ebp + 0x7a]
004dbed3  jmp        eax

// block 004dc271
004dc271  mov        eax, 0x14e1
004dc276  push       eax
004dc277  add        eax, dword ptr [ebp + 0x488]
004dc27d  pop        ecx
004dc27e  or         ecx, ecx
004dc280  mov        dword ptr [ebp + 0x40e], eax
004dc286  popal      
004dc287  jne        0x4dc291

// block 004dc289
004dc289  mov        eax, 1
004dc28e  ret        0xc

// block 004dc291
004dc291  push       0
004dc296  ret        
