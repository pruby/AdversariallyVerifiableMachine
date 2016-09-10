library AVMMemoryContext32 {
    struct Context {
        uint256[] readAccesses;
        uint256[] writeAccesses;
        uint writeIdx;
        uint cachedRead;
        uint cachedReadValue;
        uint addressMask;
        bool valid;
    }
    
    event trace(string);
    function initContext(uint256[] memory readAccesses, uint256[] memory writeAccesses, uint addressBits) internal returns (Context memory) {
        Context memory ctx;
        ctx.readAccesses = readAccesses;
        ctx.writeAccesses = writeAccesses;
        ctx.writeIdx = 0;
        ctx.addressMask = (2 ** addressBits) - 1;
        ctx.cachedRead = (uint) (-1);
        ctx.cachedReadValue = 0;
        ctx.valid = true;
        return ctx;
    }
    
    function read256(Context ctx, uint addr) internal returns (uint) {
        uint v;
        addr = addr & ctx.addressMask;
        
        for (uint i = 0; i < ctx.readAccesses.length; i += 2) {
            if (ctx.readAccesses[i] == addr) {
                return ctx.readAccesses[i+1];
            }
        }
        
        ctx.valid = false;
        return 0;
    }
    
    function read32(Context ctx, uint addr) internal returns (uint32) {
        uint v = read256(ctx, addr / 8);
        
        // Shift down to chosen word
        for (uint j = (addr % 8); j < 7; j++) {
            v = v / 4294967296;
        }
        
        return (uint32) (v);
    }
    
    function write256(Context ctx, uint addr, uint value) internal {
        addr = addr & ctx.addressMask;
        
        trace("Write");
        if (ctx.writeAccesses.length < (ctx.writeIdx + 2)) {
            // Insufficient writes
            trace("Insufficient Writes");
            ctx.valid = false;
            return;
        }
        
        trace("Reading write address");
        if (ctx.writeAccesses[ctx.writeIdx++] != addr) {
            // Wrong read address
            trace("Wrong read address");
            ctx.valid = false;
            return;
        }
        
        ctx.writeIdx++;
        
        trace("Reading write value");
        if (ctx.writeAccesses[ctx.writeIdx++] != value) {
            // Wrong write value
            trace("Wrong write value");
            ctx.valid = false;
            return;
        }
        
        trace("Updating read values for write");
        for (uint i = 0; i < ctx.readAccesses.length; i += 2) {
            if (ctx.readAccesses[i] == addr) {
                ctx.readAccesses[i+1] = value;
            }
        }
    }
    
    function write32(Context ctx, uint addr, uint value) internal {
        trace("Write");
        if (ctx.writeAccesses.length < (ctx.writeIdx + 2)) {
            // Insufficient writes
            trace("Insufficient Writes");
            ctx.valid = false;
            return;
        }
        
        trace("Reading write address");
        if (ctx.writeAccesses[ctx.writeIdx++] != ((addr / 8) & ctx.addressMask)) {
            // Wrong write address
            trace("Wrong write address");
            ctx.valid = false;
            return;
        }
        
        trace("Reading write prior value");
        uint result = ctx.writeAccesses[ctx.writeIdx++];
        uint mask = 4294967295;
        value = value & mask;
        
        // Shift down to chosen word
        for (uint j = (addr % 8); j < 7; j++) {
            mask = mask * 4294967296;
            value = value * 4294967296;
        }
        
        result = (result & (~mask)) | value;
        
        trace("Reading write value");
        if (ctx.writeAccesses[ctx.writeIdx++] != result) {
            // Wrong write value
            trace("Wrong write value");
            ctx.valid = false;
            return;
        }
        
        trace("Updating future read values from write");
        for (uint i = 0; i < ctx.readAccesses.length; i += 2) {
            if (ctx.readAccesses[i] == addr) {
                ctx.readAccesses[i+1] = value;
            }
        }
    }
    
    function isValid(Context memory ctx) internal returns (bool) {
        if (ctx.writeAccesses.length != ctx.writeIdx) {
            // Excess reads
            ctx.valid = false;
        }
        return ctx.valid;
    }
}
