// Retry the single ECL dispatcher that exceeded the full-export time limit.
// @category Reconstruction
import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.*;
import ghidra.program.model.listing.Function;
import java.nio.charset.StandardCharsets;
import java.nio.file.*;
import java.util.*;
import java.io.*;
public class RetryEcl extends GhidraScript {
    @Override public void run() throws Exception {
        Path out=Paths.get(getScriptArgs()[0]);
        Function f=currentProgram.getFunctionManager().getFunctionAt(toAddr(0x0048c010L));
        if(f==null) throw new IllegalStateException("ECL function missing");
        DecompInterface decompiler=new DecompInterface();
        DecompileOptions options=new DecompileOptions();
        options.setDefaultTimeout(300);
        options.setMaxInstructions(300000);
        options.setMaxJumpTableEntries(4096);
        decompiler.setOptions(options);
        decompiler.toggleCCode(true);decompiler.toggleSyntaxTree(false);
        if(!decompiler.openProgram(currentProgram)) throw new IOException(decompiler.getLastMessage());
        long start=System.currentTimeMillis();boolean success=false;String error="";
        println("ECL RETRY START 0048c010, 300-second upper limit");
        try {
            DecompileResults result=decompiler.decompileFunction(f,300,monitor);
            success=result.decompileCompleted() && result.getDecompiledFunction()!=null;
            error=result.getErrorMessage();
            if(success) {
                String code="/* Entry: 0x0048c010; symbol: "+f.getName()+"; Ghidra-listed body bytes: "+f.getBody().getNumAddresses()+" */\n/* Retried with 300-second timeout after the original 45-second timeout. */\n"+result.getDecompiledFunction().getC();
                Files.writeString(out.resolve("pseudocode/0048c010.c"),code,StandardCharsets.UTF_8);
            }
        } catch(Exception e) { error=e.toString(); }
        finally { decompiler.dispose(); }
        try(PrintWriter p=new PrintWriter(Files.newBufferedWriter(out.resolve("ecl_retry.properties"),StandardCharsets.UTF_8))) {
            p.println("entry_va=0x0048c010");p.println("success="+success);
            p.println("timeout_seconds=300");p.println("elapsed_ms="+(System.currentTimeMillis()-start));
            p.println("error="+error.replace('\n',' ').replace('\r',' '));
            p.println("pseudocode_file="+(success?"pseudocode/0048c010.c":""));
        }
        println("ECL RETRY END success="+success+", elapsed_ms="+(System.currentTimeMillis()-start)+", error="+error);
    }
}
