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
