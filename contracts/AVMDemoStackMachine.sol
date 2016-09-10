import 'AVMStepValidator.sol';
import 'AVMMemoryContext32.sol';

contract AVMDemoStackMachine is AVMStepValidator {
    event trace(string);
    event traceNum(uint);
    using AVMMemoryContext32 for AVMMemoryContext32.Context;
    
    function validateStep(uint256[] readAccesses, uint256[] writeAccesses) external returns (bool) {
        AVMMemoryContext32.Context memory ctx = AVMMemoryContext32.initContext(readAccesses, writeAccesses, getMemoryWordsLog2());
        uint ip = ctx.read32(0);
        uint sp = ctx.read32(1);
        uint op = ctx.read32((ip++) & 65535) / 16777216;
        uint v; uint w;
        if (op == 0x0) {
            // NOP
        } else if (op == 0x1) { // PUSH
            v = ctx.read32((ip++) & 65535);
            sp--;
            ctx.write32(1, sp);
            ctx.write32(sp & 65535, v);
        } else if (op == 0x2) { // POP
            sp++;
            ctx.write32(1, sp);
        } else if (op == 0x3) { // LOAD
            v = ctx.read32(sp & 65535);
            v = ctx.read32(v & 65535);
            ctx.write32(sp & 65535, v);
        } else if (op == 0x4) { // STORE
            v = ctx.read32(sp & 65535);
            w = ctx.read32((sp + 1) & 65535);
            sp += 2;
            ctx.write32(1, sp);
            ctx.write32(v & 65535, w);
        } else if (op == 0x5) { // ADD
            v = ctx.read32(sp & 65535);
            w = ctx.read32((sp + 1) & 65535);
            sp += 1;
            ctx.write32(1, sp);
            ctx.write32(sp & 65535, v + w);
        } else if (op == 0x6) { // SUB
            v = ctx.read32(sp & 65535);
            w = ctx.read32((sp + 1) & 65535);
            sp += 1;
            ctx.write32(1, sp);
            ctx.write32(sp & 65535, v - w);
        } else if (op == 0x7) { // AND
            v = ctx.read32(sp & 65535);
            w = ctx.read32((sp + 1) & 65535);
            sp += 1;
            ctx.write32(1, sp);
            ctx.write32(sp & 65535, v & w);
        } else if (op == 0x8) { // OR
            v = ctx.read32(sp & 65535);
            w = ctx.read32((sp + 1) & 65535);
            sp += 1;
            ctx.write32(1, sp);
            ctx.write32(sp & 65535, v | w);
        } else if (op == 0x9) { // NOT
            v = ctx.read32(sp & 65535);
            ctx.write32(sp & 65535, ~v);
        } else if (op == 0xa) { // JZ
            v = ctx.read32(sp & 65535);
            w = ctx.read32((sp + 1) & 65535);
            sp += 1;
            ctx.write32(1, sp);
            if (v == 0) {
                ip = w;
            }
        } else {
            // Unknown operation
            // Do not write anything, freeze here.
            return ctx.isValid();
        }
        ctx.write32(0, ip);
        return ctx.isValid();
    }
    
    function getMemoryWordsLog2() returns (uint) {
        return 13; // 16 bit addressing for 32 bit words
    }
    
    function getMaximumReadsPerStep() returns (uint) {
        return 4;
    }
    
    function getMaximumWritesPerStep() returns (uint) {
        return 3;
    }
}

