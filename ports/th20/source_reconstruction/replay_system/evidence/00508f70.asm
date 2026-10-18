00508f70 push ebx
00508f71 mov ebx, esp
00508f73 sub esp, 8
00508f76 and esp, 0xfffffff8
00508f79 add esp, 4
00508f7c push ebp
00508f7d mov ebp, dword ptr [ebx + 4]
00508f80 mov dword ptr [esp + 4], ebp
00508f84 mov ebp, esp
00508f86 sub esp, 0x250
00508f8c push esi
00508f8d push edi
00508f8e mov dword ptr [ebp - 4], ecx
00508f91 mov eax, dword ptr [ebp - 4]
00508f94 cmp dword ptr [eax + 8], 0
00508f98 je 0x508fa6
00508f9a mov ecx, dword ptr [ebp - 4]
00508f9d mov ecx, dword ptr [ecx + 8]
00508fa0 call 0x412d80
00508fa5 nop 
00508fa6 mov edx, dword ptr [ebp - 4]
00508fa9 cmp dword ptr [edx + 0x254], 0
00508fb0 je 0x508fc1
00508fb2 mov eax, dword ptr [ebp - 4]
00508fb5 mov ecx, dword ptr [eax + 0x254]
00508fbb call 0x412d80
00508fc0 nop 
00508fc1 mov ecx, dword ptr [ebp - 4]
00508fc4 cmp dword ptr [ecx + 0xc], 0
00508fc8 je 0x508fd6
00508fca mov edx, dword ptr [ebp - 4]
00508fcd mov ecx, dword ptr [edx + 0xc]
00508fd0 call 0x412d80
00508fd5 nop 
00508fd6 mov eax, 0x2c0
00508fdb imul ecx, eax, 0
00508fde add ecx, 0x5b88b0
00508fe4 call 0x5091d0
00508fe9 nop 
00508fea mov ecx, dword ptr [ebp - 4]
00508fed cmp dword ptr [ecx + 0x10], 0
00508ff1 jne 0x5090a6
00508ff7 mov edx, dword ptr [ebp - 4]
00508ffa add edx, 0x20
00508ffd mov dword ptr [ebp - 0x14], edx
00509000 mov ecx, 0x5ba568
00509005 call 0x474d60
0050900a push eax
0050900b mov ecx, dword ptr [ebp - 0x14]
0050900e call 0x414580
00509013 mov eax, dword ptr [eax]
00509015 mov dword ptr [ebp - 0xc], eax
00509018 mov ecx, 0x5ba568
0050901d call 0x474d60
00509022 push eax
00509023 mov ecx, dword ptr [ebp - 4]
00509026 call 0x509fa0
0050902b mov ecx, 0x5ba568
00509030 call 0x474d60
00509035 push eax
00509036 mov ecx, dword ptr [ebp - 4]
00509039 call 0x50a2e0
0050903e mov ecx, dword ptr [ebp - 4]
00509041 mov dword ptr [ecx + 0xe0], eax
00509047 call 0x498f40
0050904c mov edi, dword ptr [ebp - 0xc]
0050904f add edi, 0x70
00509052 mov ecx, 0x8a
00509057 mov esi, eax
00509059 rep movsd dword ptr es:[edi], dword ptr [esi]
0050905b push 0
0050905d call 0x460830
00509062 add esp, 4
00509065 mov ecx, eax
00509067 call 0x4ff690
0050906c mov edx, dword ptr [eax]
0050906e mov eax, dword ptr [eax + 4]
00509071 mov ecx, dword ptr [ebp - 0xc]
00509074 mov dword ptr [ecx + 0x10], edx
00509077 mov dword ptr [ecx + 0x14], eax
0050907a push 0
0050907c call 0x460830
00509081 add esp, 4
00509084 mov ecx, eax
00509086 call 0x4ff5e0
0050908b movzx edx, al
0050908e mov eax, dword ptr [ebp - 0xc]
00509091 mov dword ptr [eax + 0x18], edx
00509094 mov ecx, dword ptr [ebp - 4]
00509097 mov dword ptr [ecx + 0x250], 0
005090a1 jmp 0x5091b0
005090a6 mov edx, dword ptr [ebp - 4]
005090a9 cmp dword ptr [edx + 0x10], 1
005090ad jne 0x5091b0
005090b3 mov eax, dword ptr [ebp - 4]
005090b6 add eax, 0xe8
005090bb mov dword ptr [ebp - 0x18], eax
005090be mov ecx, 0x5ba568
005090c3 call 0x474d60
005090c8 push eax
005090c9 mov ecx, dword ptr [ebp - 0x18]
005090cc call 0x486e30
005090d1 mov ecx, dword ptr [eax + 0x10]
005090d4 mov dword ptr [ebp - 0x10], ecx
005090d7 mov ecx, 0x5ba568
005090dc call 0x474d60
005090e1 mov edx, dword ptr [ebp - 4]
005090e4 mov dword ptr [edx + 0x258], eax
005090ea mov esi, dword ptr [ebp - 0x10]
005090ed add esi, 0x70
005090f0 mov ecx, 0x8a
005090f5 lea edi, [ebp - 0x250]
005090fb rep movsd dword ptr es:[edi], dword ptr [esi]
005090fd call 0x498f40
00509102 mov ecx, 0x8a
00509107 lea esi, [ebp - 0x250]
0050910d mov edi, eax
0050910f rep movsd dword ptr es:[edi], dword ptr [esi]
00509111 push 0
00509113 call 0x460830
00509118 add esp, 4
0050911b mov dword ptr [ebp - 0x1c], eax
0050911e mov eax, dword ptr [ebp - 0x10]
00509121 add eax, 0x10
00509124 push eax
00509125 mov ecx, dword ptr [ebp - 0x1c]
00509128 call 0x50a430
0050912d push 0
0050912f call 0x460830
00509134 add esp, 4
00509137 mov dword ptr [ebp - 0x20], eax
0050913a mov ecx, dword ptr [ebp - 0x10]
0050913d cmp dword ptr [ecx + 0x18], 0
00509141 je 0x509149
00509143 mov byte ptr [ebp - 5], 1
00509147 jmp 0x50914d
00509149 mov byte ptr [ebp - 5], 0
0050914d movzx edx, byte ptr [ebp - 5]
00509151 push edx
00509152 mov ecx, dword ptr [ebp - 0x20]
00509155 call 0x50a410
0050915a push 0
0050915c call 0x460830
00509161 add esp, 4
00509164 mov ecx, eax
00509166 call 0x50a340
0050916b mov eax, dword ptr [ebp - 4]
0050916e add eax, 0xe8
00509173 mov dword ptr [ebp - 0x24], eax
00509176 mov ecx, 0x5ba568
0050917b call 0x474d60
00509180 push eax
00509181 mov ecx, dword ptr [ebp - 0x24]
00509184 call 0x486e30
00509189 mov ecx, eax
0050918b call 0x509da0
00509190 push 0
00509192 call 0x460830
00509197 add esp, 4
0050919a mov dword ptr [ebp - 0x28], eax
0050919d push -1
0050919f mov ecx, dword ptr [ebp - 0x28]
005091a2 call 0x4faca0
005091a7 mov ecx, dword ptr [ebp - 4]
005091aa call 0x50a3c0
005091af nop 
005091b0 mov ecx, dword ptr [ebp - 4]
005091b3 mov dword ptr [ecx + 0x250], 0
005091bd pop edi
005091be pop esi
005091bf mov esp, ebp
005091c1 pop ebp
005091c2 mov esp, ebx
005091c4 pop ebx
005091c5 ret 
005091c6 int3 
005091c7 int3 
005091c8 int3 
005091c9 int3 
005091ca int3 
005091cb int3 
005091cc int3 
005091cd int3 
005091ce int3 
005091cf int3 