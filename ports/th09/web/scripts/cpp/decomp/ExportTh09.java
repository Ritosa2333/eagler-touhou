// Export analysis evidence. Decompiled text is a candidate, not a verified implementation.
// @category TH09
import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.*;
import com.google.gson.*;
import java.nio.file.*;
import java.nio.charset.StandardCharsets;
import java.util.*;

public class ExportTh09 extends GhidraScript {
    public void run() throws Exception {
        Path output = Paths.get(getScriptArgs()[0]);
        Files.createDirectories(output.resolve("functions"));
        Set<String> selected = new HashSet<>();
        for (int i = 1; i < getScriptArgs().length; i++) {
            var address = toAddr(getScriptArgs()[i]);
            if (getFunctionAt(address) == null) {
                disassemble(address);
                createFunction(address, null);
            }
            selected.add(address.toString());
        }
        Gson gson = new GsonBuilder().setPrettyPrinting().create();
        DecompInterface decompiler = new DecompInterface();
        DecompileOptions options = new DecompileOptions();
        options.grabFromProgram(currentProgram);
        decompiler.setOptions(options);
        decompiler.toggleCCode(true);
        decompiler.toggleSyntaxTree(true);
        decompiler.openProgram(currentProgram);
        JsonArray functions = new JsonArray();
        int count = 0, failed = 0;
        try {
            FunctionIterator iterator = currentProgram.getFunctionManager().getFunctions(true);
            while (iterator.hasNext() && !monitor.isCancelled()) {
                Function function = iterator.next();
                if (function.isExternal()) continue;
                String address = function.getEntryPoint().toString();
                if (!selected.isEmpty() && !selected.contains(address)) continue;
                JsonObject entry = new JsonObject();
                entry.addProperty("address", "0x" + address);
                entry.addProperty("name", function.getName(true));
                entry.addProperty("signature", function.getPrototypeString(true, true));
                entry.addProperty("callingConvention", function.getCallingConventionName());
                entry.addProperty("bodyBytes", function.getBody().getNumAddresses());
                entry.addProperty("thunk", function.isThunk());
                JsonArray calls = new JsonArray();
                for (Function called : function.getCalledFunctions(monitor)) {
                    JsonObject call = new JsonObject();
                    call.addProperty("address", "0x" + called.getEntryPoint().toString());
                    call.addProperty("name", called.getName(true));
                    calls.add(call);
                }
                entry.add("calls", calls);
                DecompileResults result = decompiler.decompileFunction(function, 60, monitor);
                entry.addProperty("decompiled", result.decompileCompleted());
                entry.addProperty("message", result.getErrorMessage());
                if (result.decompileCompleted()) {
                    String code = "// Analysis candidate: " + address + " " + function.getName(true) + "\n" + result.getDecompiledFunction().getC();
                    Files.writeString(output.resolve("functions").resolve(address + ".c"), code, StandardCharsets.UTF_8);
                } else failed++;
                StringBuilder assembly = new StringBuilder();
                InstructionIterator instructions = currentProgram.getListing().getInstructions(function.getBody(), true);
                while (instructions.hasNext()) {
                    Instruction ins = instructions.next();
                    assembly.append(ins.getAddress()).append(" ").append(ins.toString()).append("\n");
                }
                Files.writeString(output.resolve("functions").resolve(address + ".asm"), assembly.toString(), StandardCharsets.UTF_8);
                functions.add(entry);
                if (++count % 100 == 0) System.out.println("TH09 functions exported: " + count + ", failures: " + failed);
            }
        } finally { decompiler.dispose(); }
        JsonObject report = new JsonObject();
        report.addProperty("program", currentProgram.getName());
        report.addProperty("sha256", currentProgram.getExecutableSHA256());
        report.addProperty("language", currentProgram.getLanguageID().toString());
        report.addProperty("functionCount", count);
        report.addProperty("failedCount", failed);
        report.add("functions", functions);
        Files.writeString(output.resolve(selected.isEmpty() ? "functions.json" : "selected-functions.json"), gson.toJson(report), StandardCharsets.UTF_8);
        System.out.println("TH09 export complete: " + count + " functions, " + failed + " decompiler failures");
    }
}
