import 'AVM';

contract CountdownStepFunction is AVMStepValidator {
    function validateStep(uint256[] readAccesses, uint256[] writeAccesses) external returns (bool) {
        if (readAccesses.length == 2 && readAccesses[0] == 1) {
            // Read OK
            if (writeAccesses.length == 2 && writeAccesses[0] == 1) {
                // Write address ok
                if (readAccesses[1] > 0 && writeAccesses[1] == readAccesses[1] - 1) {
                    // Correctly applied
                    return true;
                }
            } else if (readAccesses[1] == 0 && writeAccesses.length == 0) {
                // No write if we hit zero
                return true;
            }
        }
        return false;
    }
    
    function getStateSizes() returns (uint, uint) {
        return (2, 4);
    }
}

contract AVMTestSuite {
    AVMDisputeProcess public process;
    
    function AVMTestSuite() {
        process = new AVMDisputeProcess();
    }
    
    function calculateEphemeralMemory(uint step) internal returns (bytes32) {
        return sha3(sha3((bytes32) (0xdeadbeef), (uint) (1000000 - step)), sha3((uint) (0), (uint) (0)));
    }
    
    event TestState(AVMDisputeProcess.DisputeState);
    function prepareDisputeValid() returns (uint) {
        bytes32 zeroStore = sha3((uint) (0), (uint) (0));
        zeroStore = sha3(zeroStore, zeroStore);
        zeroStore = sha3(zeroStore, zeroStore);
        zeroStore = sha3(zeroStore, zeroStore);
        
        uint id = process.openDispute(
            new CountdownStepFunction(),
            this,
            this,
            sha3(calculateEphemeralMemory(0), zeroStore),
            sha3(calculateEphemeralMemory(1000000), zeroStore),
            1000000,
            300,
            15
        );
        
        TestState(process.getDisputeState(id));
        
        process.doConfirmComplaint(id, process.getNextAntiReplayTag(id), false);
        
        TestState(process.getDisputeState(id));
        
        bytes32[] memory states = new bytes32[](15);
        uint step = 62500;
        uint number = 62500;
        for (uint i = 0; i < 15; i++) {
            states[i] = sha3(calculateEphemeralMemory(number), zeroStore);
            number += step;
        }
        
        process.doProvideStateRoots(id, process.getNextAntiReplayTag(id), states);
        
        TestState(process.getDisputeState(id));
        
        process.doSelectDisputedStateRoot(id, process.getNextAntiReplayTag(id), 5);
        
        TestState(process.getDisputeState(id));
        
        number = 316406;
        step = 320312 - number;
        for (i = 0; i < 15; i++) {
            states[i] = sha3(calculateEphemeralMemory(number), zeroStore);
            number += step;
        }
        
        process.doProvideStateRoots(id, process.getNextAntiReplayTag(id), states);
        
        TestState(process.getDisputeState(id));
        
        process.doSelectDisputedStateRoot(id, process.getNextAntiReplayTag(id), 15);
        
        TestState(process.getDisputeState(id));
        
        number = 371334;
        step = 371578 - number;
        for (i = 0; i < 15; i++) {
            states[i] = sha3(calculateEphemeralMemory(number), zeroStore);
            number += step;
        }
        
        process.doProvideStateRoots(id, process.getNextAntiReplayTag(id), states);
        
        TestState(process.getDisputeState(id));
        
        process.doSelectDisputedStateRoot(id, process.getNextAntiReplayTag(id), 0);
        
        TestState(process.getDisputeState(id));
        
        number = 371105;
        step = 371120 - number;
        for (i = 0; i < 15; i++) {
            states[i] = sha3(calculateEphemeralMemory(number), zeroStore);
            number += step;
        }
        
        process.doProvideStateRoots(id, process.getNextAntiReplayTag(id), states);
        
        TestState(process.getDisputeState(id));
        
        process.doSelectDisputedStateRoot(id, process.getNextAntiReplayTag(id), 0);
        
        TestState(process.getDisputeState(id));
        
        number = 371091;
        step = 1;
        states = new bytes32[](14);
        for (i = 0; i < 14; i++) {
            states[i] = sha3(calculateEphemeralMemory(number), zeroStore);
            number += step;
        }
        
        process.doProvideStateRoots(id, process.getNextAntiReplayTag(id), states);
        
        TestState(process.getDisputeState(id));
        
        process.doSelectDisputedStateRoot(id, process.getNextAntiReplayTag(id), 6);
        
        TestState(process.getDisputeState(id));
        
        return id;
    }
    
    function testDisputedWriteValid(uint id) returns (bool) {
        bytes32 zeroStore = sha3((uint) (0), (uint) (0));
        zeroStore = sha3(zeroStore, zeroStore);
        zeroStore = sha3(zeroStore, zeroStore);
        zeroStore = sha3(zeroStore, zeroStore);
        
        uint[] memory reads = new uint[](2);
        uint[] memory writes = new uint[](4);
        
        reads[0] = 1;
        reads[1] = 1000000 - 371096;
        
        writes[0] = 1;
        writes[1] = 1000000 - 371096;
        writes[2] = 1000000 - 371097;
        writes[3] = (uint) (sha3(calculateEphemeralMemory(371097), zeroStore));
        
        process.doProvideMemoryAccesses(id, process.getNextAntiReplayTag(id), reads, writes);
        
        TestState(process.getDisputeState(id));
        
        process.doDisputeMemoryWrite(id, process.getNextAntiReplayTag(id), 0);
        
        TestState(process.getDisputeState(id));
        
        bytes32[] memory proof = new bytes32[](3);
        proof[0] = (bytes32) (0xdeadbeef);
        proof[1] = sha3((uint) (0), (uint) (0));
        proof[2] = zeroStore;
        
        process.doProveMemoryWrite(id, process.getNextAntiReplayTag(id), proof);
        
        TestState(process.getDisputeState(id));
        
        return process.isResolvedForDefendant(id);
    }
    
    function testDisputedWriteInvalid(uint id) returns (bool) {
        uint[] memory reads = new uint[](2);
        uint[] memory writes = new uint[](4);
        
        reads[0] = 1;
        reads[1] = 1000000 - 371096;
        
        writes[0] = 1;
        writes[1] = 1000000 - 371096;
        writes[2] = 1000000 - 371094;
        writes[3] = (uint) (sha3(calculateEphemeralMemory(371097), zeroStore));
        
        process.doProvideMemoryAccesses(id, process.getNextAntiReplayTag(id), reads, writes);
        
        TestState(process.getDisputeState(id));
        
        bytes32 zeroStore = sha3((uint) (0), (uint) (0));
        zeroStore = sha3(zeroStore, zeroStore);
        zeroStore = sha3(zeroStore, zeroStore);
        zeroStore = sha3(zeroStore, zeroStore);
        
        process.doDisputeMemoryWrite(id, process.getNextAntiReplayTag(id), 0);
        
        TestState(process.getDisputeState(id));
        
        bytes32[] memory proof = new bytes32[](3);
        proof[0] = (bytes32) (0xdeadbeef);
        proof[1] = sha3((uint) (0), (uint) (0));
        proof[2] = zeroStore;
        
        process.doProveMemoryWrite(id, process.getNextAntiReplayTag(id), proof);
        
        TestState(process.getDisputeState(id));
        
        return process.isResolvedForComplainant(id);
    }
}