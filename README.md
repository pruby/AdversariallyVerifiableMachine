# Adversarially Verifiable Machine

## Purpose

An Adversarially Verifiable Machine executes a computation in a way so that the result is proveable to any party
disputing that the computation was carried out correctly. Furthermore, while both parties must be in posession of
the full information acted upon by the computation, only the parts in dispute need be published to resolve the
dispute.

This repository implements the process of determining on the Ethereum chain whether or not a calculation was carried
out correctly as a two-party game between a complainant and the party who made the calculation (the defendant).

The verification game implemented in this repository is based on the concept published by Dr. Christian Reitwiessner at http://chriseth.github.io/notes/talks/truebit/#/ .

## Dispute Process

When the outcome of a computation is contested, a contract may invoke the dispute process to resolve the difference.

The party who originally carried out the computation, and claims it was executed correctly, is the defendant.
The party who invokes the dispute, and claims that the computation was executed incorrectly, is the complainant.

We convert the process of deciding which party is correct in to a series of rounds, in which the defendant must provide information
about the intermediate states of execution, and the complainant must specify which of those intermediate states introduces the
difference between outcomes.

Once we reduce the dispute to a single, small step, we ask the complainant whether the issue is the initial values of memory,
the process of the computation step, or the process of updating the Merkle tree as a result of any resulting writes to memory.

If a calculation was performed correctly, the defendant is now able to prove that such a small piece was performed correctly to the
Ethereum contract. If they cannot prove that they calculated this tiny piece correctly, we find in favour of the complainant.

The internal state machine and transitions are illustrated here (defendant actions in blue, complainant actions in red, automatic in gray).

![Alt text](http://g.gravizo.com/g?
  digraph G {
    AWAIT_STEP_ROOTS;
    AWAIT_DISPUTED_ROOT;
    narrowed_to_one [label="Narrowed to one step?",color=gray11];
    AWAIT_MEMORY_ACCESSES;
    AWAIT_COMPLAINANT_CHOICE;
    DISPUTED_READ;
    DISPUTED_WRITE;
    proof_valid [label="Proof Valid?",color=gray11];
    access_valid [label="Ask step validator - is access sequence valid?",color=gray11];
    RESOLVED_FOR_DEFENDANT;
    RESOLVED_FOR_COMPLAINANT;
    AWAIT_STEP_ROOTS -> AWAIT_DISPUTED_ROOT [label="doProvideStateRoots",color=blue];
    AWAIT_DISPUTED_ROOT -> narrowed_to_one [label="doSelectDisputedStateRoot",color=red];
    narrowed_to_one -> AWAIT_STEP_ROOTS [label="no"];
    narrowed_to_one -> AWAIT_MEMORY_ACCESSES [label="yes"];
    AWAIT_MEMORY_ACCESSES -> AWAIT_COMPLAINANT_CHOICE [label="doProvideMemoryAccesses",color=blue];
    AWAIT_COMPLAINANT_CHOICE -> DISPUTED_READ [label="doDisputeMemoryRead",color=red];
    AWAIT_COMPLAINANT_CHOICE -> DISPUTED_WRITE [label="doDisputeMemoryWrite",color=red];
    AWAIT_COMPLAINANT_CHOICE -> access_valid [label="doDisputeMemoryAccessSequence",color=red];
    access_valid -> RESOLVED_FOR_DEFENDANT [label="yes",color=gray11];
    access_valid -> RESOLVED_FOR_COMPLAINANT [label="no",color=gray11];
    DISPUTED_READ -> proof_valid [label="doProveMemoryRead",color=blue];
    DISPUTED_WRITE -> proof_valid [label="doProveMemoryWrite",color=blue];
    proof_valid -> RESOLVED_FOR_DEFENDANT [label="yes",color=gray11];
    proof_valid -> RESOLVED_FOR_COMPLAINANT [label="no",color=gray11];
    AWAIT_STEP_ROOTS -> RESOLVED_FOR_COMPLAINANT [label="timeout",color=gray];
    AWAIT_MEMORY_ACCESSES -> RESOLVED_FOR_COMPLAINANT [label="timeout",color=gray];
    DISPUTED_READ -> RESOLVED_FOR_COMPLAINANT [label="timeout",color=gray];
    DISPUTED_WRITE -> RESOLVED_FOR_COMPLAINANT [label="timeout",color=gray];
    AWAIT_DISPUTED_ROOT -> RESOLVED_FOR_DEFENDANT [label="timeout",color=gray];
    AWAIT_COMPLAINANT_CHOICE -> RESOLVED_FOR_DEFENDANT [label="timeout",color=gray];
  }
)

This contract does not itself handle any form of bond, or rights of any party to invoke disputes. It is intended to be called
as a utility by other contracts under circumstances where one party may dispute the other's computation, and the contracts using
this utility need to ensure that it is not abused. In particular, participating in this process may be relatively intensive in
terms of both computation, gas and complexity for the defendant, and it's recommended that the complainant be required to make
a matching committment, or put down a bond that repays this effort in the event of invalid complaints.

While I considered an off-chain version of this, in which parties sent each other signed committments, the fact that a dispute
is being invoked implies that one of the parties has either been dishonest or acted in error, and in either case felt that it's
best to keep the dispute resolution on record despite the cost.

## Test Suite

The current test suite in AVMTestSuite.sol runs the entire dispute process in one transaction. This is implemented in Solidity, not using a framework like Truffle, due to issues with obtaining the return value of an asynchronous transaction call (easily supported within Solidity, not so much within ether pudding). This can be run in Browser Solidity, but consumes a lot of gas, so be sure to increase the default gas to at least 2,000,000.

All functions with names beginning "test" are expected to return a true value. I've personally had some issues with Chrome crashing (aw... snap) when running multiple tests - just reload and try that test again. I think this is thanks to the new tracing utilities piling up a load of memory, but can't be sure.

The step function for this simply decrements the number at memory address 1, stopping when it hits zero. The trace of this behaviour is simple enough to validate strictly by hand.

## Demo Stack Machine

AVMDemoStackMachine.sol is in development as a demonstration of what this approach is capable of. It simulates a virtual machine operating on 32-bit values using a custom bytecode, and is likely to be the basis of a future test suite. It may be adjusted in the near future to use EVM bytecode (except external foreign call instructions, which will fail).

The memory access routines in AVMMemoryContext32.sol are used to allow us to write the step process as we normally would in a bytecode interpreter, and records if any invalid memory access is made. This technique allows us to express complex step logic in a normal, readable way, without worrying about the details of comparing individual reads and writes to the step trace provided.

Note that this does not validate the order of reads made - only that the read list includes all addresses needed. If we write to an address, this updates future reads to read the written value, which is what we would normally expect when writing an interpreter.
