
// block 004067a0
004067a0  mov        ecx, dword ptr [eax + 0x10]
004067a3  mov        edx, dword ptr [esp + 4]
004067a7  test       cl, 1
004067aa  jne        0x4067cb

// block 004067ac
004067ac  fldz       
004067ae  or         ecx, 1
004067b1  fstp       dword ptr [eax + 8]
004067b4  mov        dword ptr [eax + 4], 0
004067bb  mov        dword ptr [eax], 0xfff0bdc1
004067c1  mov        dword ptr [eax + 0xc], 0x4b2ed0
004067c8  mov        dword ptr [eax + 0x10], ecx

// block 004067cb
004067cb  fild       dword ptr [esp + 4]
004067cf  mov        dword ptr [eax + 4], edx
004067d2  dec        edx
004067d3  mov        dword ptr [eax], edx
004067d5  fstp       dword ptr [eax + 8]
004067d8  ret        4
