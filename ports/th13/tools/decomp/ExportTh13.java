import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.*;
import ghidra.program.model.address.Address;
import ghidra.program.model.block.CodeBlock;
import ghidra.program.model.block.CodeBlockIterator;
import ghidra.program.model.block.CodeBlockReference;
import ghidra.program.model.block.CodeBlockReferenceIterator;
import ghidra.program.model.block.SimpleBlockModel;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.*;
import com.google.gson.*;
import java.nio.file.*;
import java.nio.charset.StandardCharsets;
import java.util.*;

public class ExportTh13 extends GhidraScript {
    private static String hex(byte[] bytes) {
        StringBuilder text = new StringBuilder(bytes.length * 2);
        for (byte value : bytes) text.append(String.format("%02x", value & 0xff));
        return text.toString();
    }

    public void run() throws Exception {
        Path output = Paths.get(getScriptArgs()[0]);
        Path functionsDirectory = output.resolve("functions");
        Files.createDirectories(functionsDirectory);
        Set<String> selected = new HashSet<>();
        for (int i = 1; i < getScriptArgs().length; i++) {
            Address address = toAddr(getScriptArgs()[i]);
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
        SimpleBlockModel blockModel = new SimpleBlockModel(currentProgram);

        JsonArray functions = new JsonArray();
        JsonArray blocks = new JsonArray();
        int count = 0, failed = 0;
        try {
            FunctionIterator iterator = currentProgram.getFunctionManager().getFunctions(true);
            while (iterator.hasNext() && !monitor.isCancelled()) {
                Function function = iterator.next();
                if (function.isExternal()) continue;
                String address = function.getEntryPoint().toString();
                if (!selected.isEmpty() && !selected.contains(address)) continue;
                String label = "0x" + address + " " + function.getName(true);

                JsonObject entry = new JsonObject();
                entry.addProperty("address", "0x" + address);
                entry.addProperty("name", function.getName(true));
                entry.addProperty("signature", function.getPrototypeString(true, true));
                entry.addProperty("callingConvention", function.getCallingConventionName());
                entry.addProperty("bodyBytes", function.getBody().getNumAddresses());
                entry.addProperty("thunk", function.isThunk());
                entry.add("calls", calledFunctions(function.getCalledFunctions(monitor)));
                entry.add("callers", calledFunctions(function.getCallingFunctions(monitor)));

                DecompileResults result = decompiler.decompileFunction(function, 60, monitor);
                entry.addProperty("decompiled", result.decompileCompleted());
                entry.addProperty("message", result.getErrorMessage());
                if (result.decompileCompleted()) {
                    String code = result.getDecompiledFunction().getC();
                    JsonObject artifacts = new JsonObject();
                    artifacts.addProperty("unassigned", code.contains("unaff_"));
                    artifacts.addProperty("registerArgument", code.contains("in_E"));
                    artifacts.addProperty("warning", code.contains("// WARNING"));
                    entry.add("artifacts", artifacts);
                    Files.writeString(functionsDirectory.resolve(address + ".c"),
                        "// Analysis candidate: " + address + " " + function.getName(true) + "\n" + code,
                        StandardCharsets.UTF_8);
                } else failed++;
                StringBuilder assembly = new StringBuilder();
                InstructionIterator instructions = currentProgram.getListing().getInstructions(function.getBody(), true);
                while (instructions.hasNext()) {
                    Instruction instruction = instructions.next();
                    assembly.append(instruction.getAddress()).append(' ')
                        .append(hex(instruction.getBytes())).append(' ')
                        .append(instruction.toString()).append('\n');
                }
                Files.writeString(functionsDirectory.resolve(address + ".asm"), assembly.toString(),
                    StandardCharsets.UTF_8);
                functions.add(entry);

                JsonObject entryBlocks = new JsonObject();
                entryBlocks.addProperty("function", "0x" + address);
                JsonArray ranges = new JsonArray();
                CodeBlockIterator codeBlocks = blockModel.getCodeBlocksContaining(function.getBody(), monitor);
                while (codeBlocks.hasNext()) {
                    CodeBlock block = codeBlocks.next();
                    JsonObject range = new JsonObject();
                    range.addProperty("entry", block.getFirstStartAddress() == null ? null
                        : "0x" + block.getFirstStartAddress().toString());
                    range.addProperty("min", "0x" + block.getMinAddress().toString());
                    range.addProperty("max", "0x" + block.getMaxAddress().toString());
                    JsonArray destinations = new JsonArray();
                    CodeBlockReferenceIterator flows = block.getDestinations(monitor);
                    while (flows.hasNext()) {
                        CodeBlockReference flow = flows.next();
                        JsonObject destination = new JsonObject();
                        destination.addProperty("address", "0x" + flow.getDestinationAddress().toString());
                        destination.addProperty("flow", flow.getFlowType().getName());
                        destinations.add(destination);
                    }
                    range.add("destinations", destinations);
                    ranges.add(range);
                }
                entryBlocks.add("blocks", ranges);
                blocks.add(entryBlocks);

                if (++count % 100 == 0) System.out.println("TH13 functions exported: " + count + ", failures: " + failed);
            }
        } finally {
            decompiler.dispose();
        }

        JsonArray imports = new JsonArray();
        FunctionIterator externals = currentProgram.getFunctionManager().getExternalFunctions();
        while (externals.hasNext()) {
            Function external = externals.next();
            JsonObject entry = new JsonObject();
            entry.addProperty("address", "0x" + external.getEntryPoint().toString());
            entry.addProperty("name", external.getName(true));
            entry.addProperty("library", external.getParentNamespace().getName(true));
            entry.add("callers", calledFunctions(external.getCallingFunctions(monitor)));
            imports.add(entry);
        }
        JsonArray strings = new JsonArray();
        DataIterator data = currentProgram.getListing().getDefinedData(true);
        while (data.hasNext()) {
            Data item = data.next();
            if (!item.hasStringValue()) continue;
            JsonObject entry = new JsonObject();
            entry.addProperty("address", "0x" + item.getAddress().toString());
            entry.addProperty("value", item.getDefaultValueRepresentation());
            strings.add(entry);
        }

        JsonObject report = new JsonObject();
        report.addProperty("program", currentProgram.getName());
        report.addProperty("sha256", currentProgram.getExecutableSHA256());
        report.addProperty("language", currentProgram.getLanguageID().toString());
        report.addProperty("compiler", currentProgram.getCompilerSpec().getCompilerSpecID().getIdAsString());
        report.addProperty("functionCount", count);
        report.addProperty("failedCount", failed);
        report.add("functions", functions);
        String suffix = selected.isEmpty() ? "" : "-selected";
        Files.writeString(output.resolve("functions" + suffix + ".json"), gson.toJson(report), StandardCharsets.UTF_8);
        Files.writeString(output.resolve("blocks" + suffix + ".json"), gson.toJson(blocks), StandardCharsets.UTF_8);
        Files.writeString(output.resolve("imports" + suffix + ".json"), gson.toJson(imports), StandardCharsets.UTF_8);
        Files.writeString(output.resolve("strings" + suffix + ".json"), gson.toJson(strings), StandardCharsets.UTF_8);
        System.out.println("TH13 export complete: " + count + " functions, " + failed + " decompiler failures");
    }

    private JsonArray calledFunctions(Set<Function> functions) throws Exception {
        JsonArray calls = new JsonArray();
        for (Function function : functions) {
            JsonObject call = new JsonObject();
            call.addProperty("address", "0x" + function.getEntryPoint().toString());
            call.addProperty("name", function.getName(true));
            calls.add(call);
        }
        return calls;
    }
}
