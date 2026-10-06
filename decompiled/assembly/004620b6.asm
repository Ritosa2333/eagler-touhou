
// block 004620b6
004620b6  push       esi
004620b7  mov        esi, eax
004620b9  mov        eax, dword ptr [esi]
004620bb  push       eax
004620bc  call       0x461920

// block 004620c1
004620c1  test       eax, eax
004620c3  jne        0x4620c7

// block 004620c5
004620c5  mov        dword ptr [esi], eax

// block 004620c7
004620c7  mov        edx, dword ptr [esp + 8]
004620cb  mov        esi, eax
004620cd  call       0x462020

// block 004620d2
004620d2  pop        esi
004620d3  ret        4
