
// block 004393a0
004393a0  push       ebp
004393a1  mov        ebp, esp
004393a3  and        esp, 0xfffffff8
004393a6  sub        esp, 8
004393a9  fld        dword ptr [eax + 4]
004393ac  fld        dword ptr [eax]
004393ae  call       0x4937aa

// block 004393b3
004393b3  fstp       dword ptr [esp + 4]
004393b7  fld        dword ptr [esp + 4]
004393bb  mov        eax, esi
004393bd  fstp       dword ptr [esi]
004393bf  mov        esp, ebp
004393c1  pop        ebp
004393c2  ret        
