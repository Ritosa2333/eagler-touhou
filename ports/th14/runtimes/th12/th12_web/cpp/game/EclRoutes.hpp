#pragma once
#include "PortTitle.hpp"
#include "Types.hpp"
namespace th11 {
// The titles renumber parts of the ECL command space, and a number that means one thing in TH12 can
// mean another in the next title. The port's handlers are shared, so a command the two titles spell
// with different numbers needs one row naming both, and the row needs evidence that the two records
// really are the same command - not a guess from a nearby number.
//
// The first row is the named enemy spawn. TH12 gives it 0x118 and TH13 gives it 0x141:
//
//   - TH13's own dispatcher sends 0x141 to handler 0x41b2ec (`tools/th-ecl-dispatch.py` reads the
//     two tables at 0x41f9d8 and 0x41f760 through the formula at 0x41b2ae..0x41b2d6). That handler
//     is `push edi; call 0x41fcc0` and 0x41fcc0 reads the first argument as a byte length and the
//     name that follows it (`0x41fcf2 mov eax,[ebx+0x10]`, then the floats from the word after the
//     padded name) - the shape of the TH12 body in EnemyManager.cpp, not of a position set.
//   - The records are byte for byte the same. In TH12's stage05.ecl the instruction at 0x158a0 is
//     `0x118`, length 52, six parameters, arguments `0xc "MapleEnemy" 0 0 0x64 0x3e8 0`; in
//     TH13's st05.ecl the instruction at 0x60c4 is `0x141` with the same length, the same
//     parameter count and the same argument bytes (`tools/th-ecl-instructions.py --at`). The first
//     argument is the padded name length in both, and it is the same name.
//
// So TH13's 0x141 runs TH12's 0x118 body, and the "relative" arm of that body is the one TH12
// uses for it.
//
// TH16 gives the same command the same number, and its own records are the reading: the eight
// 0x141 records of its st06bs.ecl are length 52 with six parameters and the arguments
// `0xc "MapleEnemy3" 0 0 0x64 0x3e8 0`, and its st07.ecl has the shape twice (`MapleEnemy`,
// `MapleEnemy2`) - the same record with the name the stage spawns, which is what TH13's st06.ecl
// and TH12's 0x118 carry too (`tools/th-ecl-instructions.py`). The census agrees: TH16 spells this 0x141
// 16 times and 0x118 not once, TH13 7 and TH14 7, and TH12 0x118 four times
// (`py -3 tools/th-ecl-wait-census.py --words 280,321`).
inline constexpr bool ecl_named_spawn_relative(u16 op) {
    if (op == 0x118) return true;
    return (kPortTitle==13||kPortTitle==14||kPortTitle==16||kPortTitle==17||kPortTitle==18)
        && op == 0x141;
}

// The dispatcher chain in EnemyScriptServices::command asks whether an opcode belongs to the manager's
// command space; the alias has to be admitted there too, or the manager never sees it.
inline constexpr bool ecl_manager_route(u16 op) {
    if(kPortTitle==16&&(op==502||op==503||op==512||op==518||op==520))return true;
    return ((op >= 0x100 && op <= 0x10f && op != 0x102 && op != 0x103 && op != 0x106 &&
             op != 0x107 && op != 0x108 && op != 0x10d) ||
            op == 0x118 || op == 0x150 || op == 0x159 || op == 0x173 || op == 0x1c2 ||
            op == 0x1a0) || ecl_named_spawn_relative(op);
}

// The bytecode numbers differ between titles as well, and TH13's own VM switch says which one moved.
// The switch is right there in the interpreter loop (th13.exe):
//
//   0x4772e4  movsx ecx, word ptr [eax + 4]      ; the record's opcode word
//   0x4772e8  cmp   ecx, 0x5D                    ; 94 bytecode cases, 0..93
//   0x4772f1  movzx edx, byte ptr [ecx + 0x478D44]   ; opcode -> case group
//   0x4772f8  jmp   dword ptr [edx*4 + 0x478C38]      ; case group -> body
//   0x478b9d  (the out-of-range path)             ; [[ctx+0x1014]]->vtable[0]() = the title's
//                                                 ; own 0x1xx command dispatcher
//
// and the case for 23 is 0x47759e:
//
//   0x47759e  fld   dword ptr [ebx]              ; the thread's clock
//   0x4775a8  call  0x478e60                      ; resolve argument 0
//   0x4775b5  fsubr qword ptr [esp + 0x18]
//   0x4775b9  fstp  dword ptr [ebx]              ; the clock moves by that many frames
//   0x4775bb  jmp   0x478bbf                      ; the shared advance tail
//
// so 23 is the frames-counted wait that TH12 spells 83. The records agree in shape and in values:
// TH13's 0x17 records are length 20 with one parameter, arguments 0x28, 0x64, 0x190 and 0x3e8
// (tools/th-ecl-instructions.py); TH12's 0x53 records are the same shape with 0x28, 0x1e and
// 0x3d. The census is the other half: TH13 uses 23 1468 times and 83 not once, TH12 uses 83 1890
// times and 23 not once (tools/th-ecl-opcode-census.py). Since the port's interpreter switches on
// one enum, the title's number is translated to the enum's first.
inline constexpr u16 ecl_vm_opcode(u16 op) {
    if(kPortTitle==16){
        // Own retail VM473a4c/473b58. ECL93 remains outside this proof.
        switch(op){case 22:case 30:case 31:return 0;case 23:return 83;case 24:return 0xff00;
        case 83:return 84;case 84:return 85;case 85:return 86;case 86:return 0xff01;case 91:case 92:return 0xff03;default:break;}
    }
    if(kPortTitle==14){
        // Retail VM table 483d00/483e0c, including the new floating wait.
        switch(op){case 22:case 30:case 31:return 0;case 23:return 83;case 24:return 0xff00;
        case 83:return 84;case 84:return 85;case 85:return 86;case 86:return 0xff01;case 93:return 0xff02;default:break;}
    }
    // The wait moves for every title after TH12, not just TH13: TH16's own scripts spell it 0x17
    // 1591 times and 0x53 not once, TH17 1514 and TH18 1510 the same way, while TH12 spells 0x53
    // 1890 times and 0x17 never (tools/tmp/vm-wait-census.py). The corpus fingerprints and the
    // per-stage alignment agree (tools/th-ecl-opcode-map.py: TH16 0x17 -> TH12 0x53 in 981 of its
    // records; tools/th-ecl-script-align.py: 0x017 -> 0x053 in 10 aligned pairs of stage one).
    if ((kPortTitle==13||kPortTitle==14||kPortTitle==16||kPortTitle==17||kPortTitle==18)&&op==23)
        return 83;
    return op;
}

// TH13 also spells the dialogue pair differently: 0x206 starts a dialogue, 0x207 waits for it, while
// TH12 spells them 0x1a2 and 0x1a3. Three measurements agree, and they are of different kinds:
//
//   by alignment. TH13's st05.ecl and TH12's stage05.ecl are the same stage, and aligning the
//   two record sequences (tools/th-ecl-script-align.py, ignoring the opcode words and matching on
//   records) pairs 0x206 with 0x1a2 three times and 0x207 with 0x1a3 four times. The same run
//   pairs 0x141 with 0x118 and 0x017 with 0x053, i.e. both rows measured last round - the
//   instrument reproduces what independent work had already established.
//
//   by the records. TH13's 0x207 records are length 16 with no parameter, and its 0x206
//   records are length 20 with one (arguments 0 and 1 and -1); TH12's 0x1a3 is the parameterless
//   16-byte word and 0x1a2 is the one-argument reader of FUN_0041a900.
//
//   by the bodies. TH12's 0x1a2 at 0x418813 resolves argument 0 (`xor ecx,ecx; call 0x41a900`),
//   calls the dialogue starter with it, then `xor edi,edi; xor ebx,ebx` before the teardown tail.
//   TH13's 0x206 at 0x41e3da resolves argument 0 (`xor ecx,ecx; call 0x421b10`), calls a manager
//   on it, and reaches the same `xor edi,edi; xor ebx,ebx` before its own teardown tail.
//
// The pair belongs to the whole generation, not to TH13 alone. The census is the first half:
// 518 and 519 are 0 in TH12's whole ECL set and are 50/59, 49/69, 56/68, 55/59 and 58/92 in
// TH13/14/16/17/18 (tools/th-ecl-wait-census.py --words 518,519). The records are the second:
// 0x206 is 20 bytes with one parameter and 0x207 is 16 with none in each of those titles, which
// is the shape TH12 spells 0x1a2 and 0x1a3, and the ones in the boss-opening stage carry the same
// times and the same single argument value across the four titles read. The alignment that named the
// pair for TH13 reproduces it on TH13 and pairs TH16's 0x207 with 0x1a3 three times. And the
// bodies are the third: every title resolves the pair to handlers of its own (13 0x41e3da/0x41e407,
// 14 0x428819/0x428847, 16 0x4216b3/0x4216e0, 17 0x42506e/0x4250af, 18 0x434dbf/0x434e2b,
// tools/th-ecl-dispatch.py), and each 0x207 side is TH13's own three steps - read the manager, test
// its field and leave through the shared tail if it is not done, else answer minus one:
//
//   16 0x216e0  mov eax,[0x4a6dcc] ; mov eax,[eax+0x1c8] ; test eax,eax ; je 0x22a9b
//               cmp dword ptr [eax+0x18c],0 ; jne 0x22a9b ; or eax,0xffffffff ; jmp 0x22a9d
//
// (0x22a9b is the tail the title's other cases leave through as well, tools/th-code-window.py).
//
// The port's state handler already owns 0x1a2 and 0x1a3, so the row is the whole difference.
inline constexpr u16 ecl_state_opcode(u16 op) {
    if (kPortTitle != 12 && (op == 0x206 || op == 0x207)) return u16(op - 0x64);
    return op;
}
}
