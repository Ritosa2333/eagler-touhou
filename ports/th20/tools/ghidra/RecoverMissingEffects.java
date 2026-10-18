// Supplemental decompiler evidence only; generated text is not buildable source.
// @category Reconstruction
import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.*;
import ghidra.program.model.listing.Function;
import java.nio.charset.StandardCharsets;
import java.nio.file.*;
public class RecoverMissingEffects extends GhidraScript {
    @Override public void run() throws Exception {
        Path out=Paths.get(getScriptArgs()[0]);Files.createDirectories(out);
        DecompInterface d=new DecompInterface();DecompileOptions o=new DecompileOptions();
        o.setMaxInstructions(300000);d.setOptions(o);d.toggleCCode(true);d.toggleSyntaxTree(false);
        if(!d.openProgram(currentProgram))throw new IllegalStateException(d.getLastMessage());
        for(long address:new long[]{0x45c360L,0x467430L}) {
            Function f=getFunctionAt(toAddr(address));
            if(f==null){disassemble(toAddr(address));f=createFunction(toAddr(address),"effect_"+Long.toHexString(address));}
            if(f==null)throw new IllegalStateException("Cannot create "+Long.toHexString(address));
            DecompileResults r=d.decompileFunction(f,120,monitor);
            if(!r.decompileCompleted()||r.getDecompiledFunction()==null)throw new IllegalStateException(r.getErrorMessage());
            Files.writeString(out.resolve(String.format("%08x.c",address)),r.getDecompiledFunction().getC(),StandardCharsets.UTF_8);
            println("SUPPLEMENTAL "+Long.toHexString(address)+" exported");
        }
        d.dispose();
    }
}
