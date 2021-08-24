/* -*- C -*- */
/*
 * Copyright (c) 2021 Seagate Technology LLC and/or its Affiliates
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * For any questions about this software or licensing,
 * please email opensource@seagate.com or cortx-questions@seagate.com.
 *
 */


/**
   @page dtm0br-dld DLD of DTM0 basic recovery

   - @ref DTM0BR-ovw
   - @ref DTM0BR-def
   - @ref DTM0BR-req
   - @ref DTM0BR-depends
   - @ref DTM0BR-highlights
   - @subpage DTM0BR-fspec "Functional Specification"
   - @ref DTM0BR-lspec
      - @ref DTM0BR-lspec-comps
      - @ref DTM0BR-lspec-rem-fom
      - @ref DTM0BR-lspec-loc-fom
      - @ref DTM0BR-lspec-evc-fom
      - @ref DTM0BR-lspec-state
      - @ref DTM0BR-lspec-thread
      - @ref DTM0BR-lspec-numa
   - @ref DTM0BR-conformance
   - @ref DTM0BR-usecases
   - @ref DTM0BR-ref
   - @ref DTM0BR-impl-plan
--->>>
Max: [* defect *] ut and st sections are missing.
<<<---
--->>>
Max: [* style *] usecases section should go to .h file, along with the rest of
     the sections in .h in DLD template.
<<<---


   <hr>
   @section DTM0BR-ovw Overview
   <i>All specifications must start with an Overview section that
   briefly describes the document and provides any additional
   instructions or hints on how to best read the specification.</i>

   This document is intended to be a description of the basic recovery
   machine used by DTM0 services to restore consistency of the underlying
   storage as per the requirements specified in the DTM0 HLD[1].
--->>>
Max: [* defect *] This is an obvious definition, which has very small value.
     Please make an overview which would bring as much useful information to the
     reader as it can in just a few lines.
<<<---


   <hr>
   @section DTM0BR-def Definitions
   <i>Mandatory.
   The DLD shall provide definitions of the terms and concepts
   introduced by the design, as well as the relevant terms used by the
   specification but described elsewhere.  References to the
   M0 Glossary and the component's HLD are permitted and encouraged.
   Agreed upon terminology should be incorporated in the glossary.</i>

   Previously defined terms used here (defined in the DTM0 HLD[1]):
   - <b>Process states</b>
   - <b>HA EOS</b>
--->>>
Max: [* question *] Are those all the terms that are used here? If no, please
     add the rest of the terms. Please also consider adding terms that are being
     used, but which are not in DTM0 HLD.
<<<---

   New terms:
   - <b>Recovery machine</b>
   - <b>Recovery FOM</b>
   - <b>Remote recovery</b>
   - <b>Local recovery</b>
   - <b>Eviction</b>
   - <b>User service</b>
   - <b>W-request</b>
   - <b>R-request</b>
--->>>
Max: [* defect *] The terms are not defined. Please define them.
<<<---

   <hr>
   @section DTM0BR-req Requirements
   <i>Mandatory.
   The DLD shall state the requirements that it attempts to meet.</i>

   Recovery machine shall meet the requirements defined in the DTM0 HLD[1].
--->>>
Max: [* question *] All of them? If no, please put a list here. If so, please
     put the list anyway.
<<<---
   Additionally, it has the following list of low-level requirements:
--->>>
Max: [* question *] Why do we have those requirements? Please provide a way to
     track them to the top-level requirements or requriements for other DTM0
     subcomponents.
<<<---

   - @b R.DTM0BR.HA-Aligned State transitions of the recovery SM shall be
   aligned with the state transitions of Motr processes delivered by
   the HA subsystem to the recovery machine.
   - @b R.DTM0BR.Basic Recovery machine shall support only a subset of
   possible use-cases. The subset is defined in @ref DTM0BR-usecases.
--->>>
Max: [* defect *] Usecases are not defined.
<<<---
   - @b R.DTM0BR.No-Batching Recovery machine shall replay logs in
   a synchronous manner one-by-one.
--->>>
Max: [* question *] One-by-one log or one-by-one log record? In the latter case:
     what about records from logs on different nodes?
<<<---
--->>>
Max: [* question *] How those logs or records are ordered (to be able to process
     them one-by-one)?
<<<---
--->>>
Max: [* question *] What about performance: is it possible for this design to
     meet performance requirements? In any case please clearly state if it is
     and why.
<<<---

   <hr>
   @section DTM0BR-depends Dependencies
   <i>Mandatory. Identify other components on which this specification
   depends.</i>

   The basic recovery machine depends on the following components:
   - Hare. This component holds the ordered log of all HA events in the system.
--->>>
Max: [* defect *] This is not true at the moment. Please specify if it's
     possible to implement this design without this being true or what is the
     plan to deal with this.
<<<---
   - DTM0 service. The service provides access to DTM0 log and DTM0 RPC link.
   - DTM0 log. The log is used to fill in REDO messages.
   - DTM0 RPC link. The lik is used as transport for REDO messages.
--->>>
Max: [* typo *] s/lik/link/
<<<---
   - Motr HA/conf. HA states of Motr conf objects provide the information
   about state transitions of local and remote Motr processes.
   - Motr conf. The conf module provides the list of remote DTM0 services.

   <hr>
   @section DTM0BR-highlights Design Highlights
   <i>Mandatory. This section briefly summarizes the key design
   decisions that are important for understanding the functional and
   logical specifications, and enumerates topics that need special
   attention.</i>

   The recovery machine is a set of independent FOMs (one FOM per
   potential DTM0 participant). Such a FOM is called a recovery FOM.
--->>>
Max: [* defect *] It's not clear if it's an in-process set or a set of foms in
     different processes. Please clarify.
<<<---
   A recovery FOM has sub-machines dedicated to a "role" in the recovery
--->>>
Max: [* defect *] It's not clear what a "sub-machine" is. Please provide a
     reference.
<<<---
   procedures: remote recovery, local recovery, and eviction.
--->>>
Max: [* doc *] Please clarify if the fom could have several "roles" at the same
     time and how the state transition between roles happen.
<<<---
   The role is changed linearly based on the incoming HA events for the
--->>>
Max: [* defect *] Definition of "linearly" in this context is missing.
<<<---
   corresponding participant.

   <hr>
   @section DTM0BR-lspec Logical Specification
   <i>Mandatory.  This section describes the internal design of the component,
   explaining how the functional specification is met.  Sub-components and
   diagrams of their interaction should go into this section.  The section has
   mandatory subsections created using the Doxygen @@subsection command.  The
   designer should feel free to use additional sub-sectioning if needed, though
   if there is significant additional sub-sectioning, provide a table of
   contents here.</i>

   - @ref DTM0BR-lspec-comps
   - @ref DTM0BR-lspec-rem-fom
   - @ref DTM0BR-lspec-loc-fom
   - @ref DTM0BR-lspec-evc-fom
   - @ref DTM0BR-lspec-state
   - @ref DTM0BR-lspec-thread
   - @ref DTM0BR-lspec-numa

   @subsection DLD-lspec-comps Component Overview
   <i>Mandatory.
   This section describes the internal logical decomposition.
   A diagram of the interaction between internal components and
   between external consumers and the internal components is useful.</i>

   The following diagram shows connections between recovery machine and
   the other components of the system:

   @dot
	digraph {
		rankdir = LR;
		bgcolor="lightblue"
		node[shape=box];
		label = "DTM0 Basic recovery machine components and dependencies";
		subgraph cluster_recovery_sm {
			label = "Recovery SM";
			bgcolor="lightyellow"
			node[shape=box];
			rfom[label="Recovery FOM"];
			E1[shape=none  label="&#8942;" fontsize=30]
			rfom_another[label="Another Recovery FOM"];
		}

		subgraph cluster_ha_conf {
			node[shape=box];
			label ="HA and Conf";
			conf_obj[label="Conf object"];
			ha[label="Motr HA"];
		}

		drlink[label="DTM0 RPC link"];
		dlog[label="DTM0 log"];

		conf_obj -> rfom[label="HA decisions"];
		rfom -> ha [label="Process events"];
		rfom -> drlink [label="REDO msgs"];
		dlog -> rfom[label="records"];
	}
   @enddot
--->>>
Max: [* defect *] Communication with other recovery foms in the same process and
     in different processes are missing. Please add.
<<<---

   The following sequence diagram represents an example of linerisation of
   HA decisions processing:
   @msc
   ha,rfom, remote;
   ha -> rfom [label="RECOVERING"];
   rfom -> remote [label="REDO(1)"];
   ha -> rfom [label="TRANSIENT"];
   remote -> rfom [label="REDO(1).reply(-ETIMEOUT)"];
   ha -> rfom [label="RECOVERING"];
   rfom -> rfom [label="Cancel recovery"];
   rfom -> remote [label="REDO(1)"];
   remote -> rfom [label="REDO(1).reply(ok)"];
   @endmsc

   In this example the recovery machine receives a sequence of events
   (TRANSIENT, RECOVERING, TRANSIENT), and reacts on them:
   - Starts replay the log by sending REDO(1).
   - Awaits on the reply.
   - Cancels recovery.
   - Re-starts replay by sending REDO(1) again.
   - Get the reply.
--->>>
Max: [* defect *] One example is not enough to describe the algorithm. Please
     explain how it's done exactly. Maybe not in this section, but somewhere in DLD.
<<<---

   @subsection DTM0BR-lspec-rem-fom Remote recovery FOM
   <i>Such sections briefly describes the purpose and design of each
   sub-component.</i>
--->>>
Max: [* defect *] State machine is there, but what the fom actually does is
     missing. Please add.
<<<---

   When a recovery FOM detects a state transition to RECOVERING of a remote
   participant, it transits to the sub-SM called "Remote recovery FOM".
   In other words, it re-incarnates as "Remote recovery FOM" as soon as
   the previous incarnation is ready to reach its "terminal" state.

   Remote recovery FOM acts as a reactor on the following kinds of events:
   - Process state transitions (from HA subsystem);
   - DTM0 log getting new records;
   - RPC replies (from DTM0 RPC link component).

   Remote recovery FOM has the following states and transitions:

   @verbatim
   REMOTE_RECOVERY(initial) -> WAITING_ON_HA_OR_RPC
   REMOTE_RECOVERY(initial) -> WAITING_ON_HA_OR_LOG
   WAITING_ON_HA_OR_RPC     -> WAITING_ON_HA_OR_LOG
   WAITING_ON_HA_OR_LOG     -> WAITING_ON_HA_OR_RPC
   WAITING_ON_HA_OR_RPC     -> NEXT_HA_STATE (terminal)
   WAITING_ON_HA_OR_LOG     -> NEXT_HA_STATE (terminal)
   WAITING_ON_HA_OR_RPC     -> SHUTDOWN (terminal)
   WAITING_ON_HA_OR_LOG     -> SHUTDOWN (terminal)

       Where:
       REMOTE_RECOVERY is a local initial state (local to the sub-SM);
       NEXT_HA_STATE is a local terminal state;
       SHUTDOWN is a global terminal state.
   @endverbatim

   @subsection DTM0BR-lspec-loc-fom Local recovery FOM
   <i>Such sections briefly describes the purpose and design of each
   sub-component.</i>

   Local recovery is used to ensure fairness of overall recovery procedure.
--->>>
Max: [* defect *] Definition of "fairness" in this context is missing. Please
     add.
<<<---
--->>>
Max: [* defect *] Definition of the "fairness" is "ensured" is missing. Please
     add.
<<<---
   Whenever a participant learns that it got all missed log records, it sends
   M0_CONF_HA_PROCESS_RECOVERED process event to the HA subsystem.

   The point where this event has to be sent is called "recovery stop
   condition". The local participant (i.e., the one that is beign in
--->>>
Max: [* typo *] being
<<<---
   the RECOVERING state) uses the information about new incoming W-requests,
   the information about the most recent (txid-wise) log entries on the other
   participants to make a decision whether recovery shall be stopped or not.

   TODO: describe the details on when and how it should stop.
--->>>
Max: [* defect *] You're right, the details are missing.
<<<---


   @subsection DTM0BR-lspec-loc-fom Eviction FOM
   <i>Such sections briefly describes the purpose and design of each
   sub-component.</i>
--->>>
Max: [* defect *] The purpose of the eviction fom is not described.
<<<---

   A recovery FOM re-incarnates as eviction FOM (i.e., enters the initial
--->>>
Max: [* defect *] A definition of "re-incarnation" is missing.
<<<---
   of the corresponding sub-SM) when the HA subsystem notifies about
--->>>
Max: [* defect *] A definition of sub-SM is missing.
<<<---
   permanent failure (FAILED) on the corresponding participant.

   The local DTM0 log shall be scanned for any record where the FAILED
--->>>
Max: [* doc *] Please add a reference to the component which does the scanning.
<<<---
--->>>
Max: [* doc *] Please explain how to scanning is done:
     - sequentially?
     - from least recent to most recent?
     - one fom/thread/whatever or multiple? How the work is distributed?
     - locks also have to be described somewhere in DLD.
<<<---
   participant participated. Such a record shall be replayed to the
   other participants that are capable of receiving REDO messages.
--->>>
Max: [* defect *] What to do with the message if a participant is in TRANSIENT
     state is not described.
<<<---

   When the log has been replayed completely, the eviction FOM notifies
--->>>
Max: [* defect *] Criteria of "log has been replayed completely" is missing.
     Please add.
<<<---
   the HA subsystem about completion and leaves the sub-SM.
--->>>
Max: [* question *] How? Please also describe what is expected from HA.
<<<---

   TODO: GC of FAILED processes and handling of clients restart.

   @subsection DTM0BR-lspec-state State Specification
   <i>Mandatory.
   This section describes any formal state models used by the component,
   whether externally exposed or purely internal.</i>

   The states of a recovery FOM is defined as a collection of the sub-SM
   states, and a few global states.

   TODO: state diagram for overall recovery FOM.
--->>>
Max: [* doc *] Also distributed state diagram.
<<<---

   @subsection DTM0BR-lspec-thread Threading and Concurrency Model
   <i>Mandatory.
   This section describes the threading and concurrency model.
   It describes the various asynchronous threads of operation, identifies
   the critical sections and synchronization primitives used
   (such as semaphores, locks, mutexes and condition variables).</i>

   Recovery machine revolves around the following kins of threads:
--->>>
Max: [* typo *] s/kins/kinds/
<<<---
--->>>
Max: [* defect *] Definition of "revolves" is missing.
<<<---
   - Locality threads (Recovery FOMs, DTM0 log updates).
   - RPC machine thread (RPC replies).

   Each recovery FOM is beign executed using Motr coroutines library.
--->>>
Max: [* defect *] s/executed/implemented/. Motr foms are being executed by
     locality threads.
<<<---
   Within the coroutine ::m0_be_op and/or ::m0_co_op is used to await
--->>>
Max: [* question *] RE: "the coroutine": which one?
<<<---
   on external events.
--->>>
Max: [* suggestion *] A list of such events would help to understand why be/co
     ops are needed.
<<<---

   DTM0 log is locked using a mutex (TBD: the log mutex or a separate mutex?)
--->>>
Max: [* question *] Entire DTM0 log is locked using a mutex? If no, please add
     unambiguous sentence. Example: "<pointer to a watcher ot whatever> is
     protected with a mutex" or "a separate mutex protects concurrent access to
     <a field>".
<<<---
   whenever recovery machine sets up or cleans up its watcher.
--->>>
Max: [* defect *] The watcher description/workflow/etc. is missing. Please add.
<<<---

   Interraction with RPC machine is wrapped by Motr RPC library and DTM0
--->>>
Max: [* typo *] s/Interraction/Interaction/
<<<---
   RPC link component. It helps to unify the type of completion objects
   across the FOM code.

   TODO: describe be/co op poll/select.

   @subsection DTM0BR-lspec-numa NUMA optimizations
   <i>Mandatory for components with programmatic interfaces.
   This section describes if optimal behavior can be supported by
   associating the utilizing thread to a single processor.</i>

   There is no so much shared resources outside of DTM0 log and
   the localities. Scaling and batching is outside of the scope
--->>>
Max: [* defect *] The number of localities is equal to the number of CPU cores
in our default configuration, so whatever uses more than one locality has to
have some kind of synchronisation.
<<<---
--->>>
Max: [* defect *] It's not clear what "scaling" and "batching" mean in this
     context. Please explain and/or add a reference where they are explained.
<<<---
   of this document.
--->>>
Max: [* defect *] FOM locality assignment is missing.
<<<---

   <hr>
   @section DTM0BR-conformance Conformance
   <i>Mandatory.
   This section cites each requirement in the @ref DTM0BR-req section,
   and explains briefly how the DLD meets the requirement.</i>
--->>>
Max: [* defect *] Top-level requirements from HLD are missing.
<<<---

   - @b I.DTM0BR.HA-Aligned Recovery machine provides an event queue that
   is beign consumed orderly by the corresponding FOM.
--->>>
Max: [* defect *] The word "queue" is present in this document only here. Please
     describe the event queue and events somewhere in this DLD.
<<<---
--->>>
Max: [* defect *] It's not clear how "HA subsystem" from the requirement is met
     by the implementation.
<<<---
   - @b I.DTM0BR.Basic Recovery machine supports only basic log replay defined
   by the remote recovery FOM and its counterpart.
--->>>
Max: [* defect *] "basic" term is not defined. Please define.
<<<---
   - @b I.DTM0BR.No-Batching Recovery machine achieves it by awaiting on
   RPC replies for every REDO message sent.
--->>>
Max: [* defect *] Maybe I should wait for I.* for performance and availability
     requirements, but as of now it's not clear how DTM recovery would catch up
     with the rest of the nodes creating new DTM0 transactions in parallel with
     DTM recovery.
<<<---

   <hr>
   @section DTM0BR-usecases
   <i>Mandatory. This section describes use-cases for recovery machine.
   </i>

   TODO: Add use-cases.
--->>>
Max: [* defect *] Right, they are missing.
<<<---


   <hr>
   @section DLD-O Analysis
   <i>This section estimates the performance of the component, in terms of
   resource (memory, processor, locks, messages, etc.) consumption,
   ideally described in big-O notation.</i>
--->>>
Max: [* doc *] Please fill this section with references to the requirements.
<<<---

   <hr>
   @section DLD-ref References
   <i>Mandatory. Provide references to other documents and components that
   are cited or used in the design.
   In particular a link to the HLD for the DLD should be provided.</i>

   - [1] <a href="https://github.com/Seagate/cortx-motr/blob/documentation/doc/dev/dtm/dtm-hld.org">
   DTM0 HLD</a>

   <hr>
   @section DLD-impl-plan Implementation Plan
   <i>Mandatory.  Describe the steps that should be taken to implement this
   design.</i>

   - Develop pseudocode-based description of recovery acivities.
   - Identify and add missing concurrency primitives (select/poll for be op).
   - Use the pseudocode and new primitives to implement a skeleton of the FOMs.
   - Incorporate RECOVERING and TRANSIENT states with their
   new meaning as soon as they are available in the upstream code.
   - Populate the list of use-cases based on available tools.
   - Populate the list of system tests.
   - Improve the stop condition based on the use-cases and tests.
   - Add implementation of eviction FOM.
--->>>
Max: [* defect *] No mention of tests being done. Please clearly state when and
     how the tests are going to be done or where and when it would be possible
     to find this information.
<<<---
 */


#define M0_TRACE_SUBSYSTEM M0_TRACE_SUBSYS_DTM0
#include "lib/trace.h"
#include "dtm0/recovery.h"

/*
 * This the draft of the DLD. It is left here for refence while the DLD
 * is being formalised. It will be removed before formal review.
 *
 *
 * DTM0 basic recovery DLD
 *
 * Definition
 * ----------
 *
 *   DTM0 basic recovery defines a set of states and transitions between them
 * for each participant of the cluster considering only a limited set of
 * usecases.
 *
 *
 * Assumptions (limitations)
 * ---------------------------
 *
 *  A1: HA (Cortx-Hare or Cortx-Hare+Cortx-HA) provides EOS for each event.
 *  A2: HA provides total ordering for all events in the system.
 *  *A3: DTM0 handles HA events one-by-one without filtering.
 *  A4: DTM0 user relies on EXECUTED and STABLE events to ensure proper ordering
 *      of DTXes.
 *  A5: There may be a point on the timeline where data is not fully replicated
 *      across the cluster.
 *  A6: RPC connections to a FAILED/TRANSIENT participant are terminated as soon
 *      as HA sends the corresponding event.
 *  A7: HA epochs are not exposed to DTM0 even if HA replies on them.
 *
 *
 * HA states and HA events
 * -----------------------
 *
 *   Every participant (PA) has the following set of HA states:
 *     TRANSIENT
 *     ONLINE
 *     RECOVERING
 *     FAILED
 * States are assigned by HA: it delivers an HA event to every participant
 * whenever it decided that one of the participants has changed its state.
 *   Motr-HA subsystem provides the "receiver-side" guarantee to comply with A1.
 * It has its own log of events that is being updated whenever an HA event
 * has been fully handled.
 *
 *
 * State transitions (HA side)
 * ---------------------------
 *
 * PROCESS_STARTING: TRANSIENT -> RECOVERING
 *   HA sends "Recovering" when the corresponding
 *   participant sends PROCESS_STARTED event to HA.
 *
 * PROCESS_STARTED: RECOVERING -> ONLINE
 *   When the process finished recovering from a failure, it sends
 *   PROCESS_STARTED to HA. HA notifies everyone that this process is back
 *   ONLINE.
 *
 * <PROCESS_T_FAILED>: ONLINE -> TRANSIENT
 *   When HA detects a transient failure, it notifies everyone about it.
 *
 * <PROCESS_FAILED>: [ONLINE, RECOVERING, TRANSIENT] -> FAILED
 *   Whenever HA detects a permanent failure, it starts eviction of the
 *   corresponding participant from the cluster.
 *
 *
 * HA Events (Motr side)
 * ---------------------
 *
 *   PROCESS_STARTING is sent when Motr process is ready to start recovering
 * procedures, and it is ready to serve WRITE-like requests.
 *   PROCESS_STARTED is sent when Motr process finished its recovery procedures,
 * and it is ready to serve READ-like requests.
 *
 *
 * State transitions (DTM0 side)
 * -----------------------------
 *
 *  TRANSIENT -> RECOVERING: DTM0 service launches a recovery FOM. There are
 *    two types of recovery FOMs: local recovery FOM and remote recovery FOM.
 *    The type is based on the participant that has changed its state.
 *  RECOVERING -> ONLINE: DTM0 service stops/finalises recovery FOMs for the
 *    participant that has changed its state.
 *  ONLINE -> TRANSIENT: DTM0 service terminates DTM0 RPC links established
 *    to the participant that has changed its state (see A6).
 *  [ONLINE, RECOVERING, TRANSIENT] -> FAILED: DTM0 service launches a recovery
 *    FOM to handle eviction (remote eviction FOM).
 *
 *
 * DTM0 recovery FOMs
 * ------------------
 *
 *   Local recovery FOM. A local recovery FOM is supposed to handle recovery
 * of the process where the fom has been launched. The FOM sends
 * PROCESS_STARTING event to HA to indicate its readiness to receive REDOs.
 * When the recovery stop condition is satisfied, the FOM causes
 * PROCESS_STARTED event.
 *   Remote recovery FOM. This FOM uses the local DTM0 log to replay DTM0
 * transactions to a remote participant. The FOM launched and stopped based
 * on the HA events.
 *   Remote eviction FOM. The FOM replays every log record to every participant
 * (where this log record is not persistent) if the FAILED participant belongs
 * the list of participants of the corresponding transaction descriptor
 * (or if it is the originator of the transaction). Note, eviction happens
 * without any state transitions of the other participants.
 *
 * DTM0 recovery stop condition
 * ----------------------------
 *   The stop condition is used to stop recovery FOMs and propagate state
 * transitions to HA. Recovery stops when a participant "catches up" with
 * the new cluster state. Since we allow WRITE-like operations to be performed
 * at almost any time (the exceptions are TRANSIENT and FAILED states of
 * of the target participants), the stop condition relies on the information
 * about the difference between the DTM0 log state when recovery started (1)
 * and the point where recovery ended (2). The second point is defined using
 * so-called "recovery stop condition". Each recovery FOM has its own condition
 * described in the next sections.
 *   Note, HA epochs would let us define a quite precise boundaries for the
 * stop. However, as it is stated in A7, we cannot rely on them. Because of
 * that, we simply store the first WRITE-like operation on the participant
 * experiencing RECOVERING, so that this participant can make a decision
 * whether recovery has to be stopped or not.
 *
 *
 * Stop condition for local FOM
 * ----------------------------
 *
 *   The local recovery FOM is created before the local process sends
 * PROCESS_STARTED event the HA. Then, it stores (in volatile memory) the first
 * WRITE-like operation that arrived after the FOM was created. Since clients
 * cannot send WRITE-like operations to servers in TRANSIENT or FAILED state,
 * and leaving of such a state happens only a local recovery FOM is created,
 * it helps to avoid a race window, as it is shown below:
 *
 * @verbatim
 *   P1.HA     | P1.TRANSIENT        | P1.RECOVERING
 *   P1.Motr   | <down> | <starting> |
 *   P1.DTM    | <down>                          | <started recovery FOM>
 *   C1.HA         | P1.TRANSIENT          | P1.RECOVERING
 *   C1.Motr   | <ongoing IO to other PAs>  | <sends KVS PUT to P1>
 *             -------------------------------------------------> real time
 *                                   1    2 3    4
 *
 *   (1) HA on P1 learns about the state transition.
 *   (2) HA on P2 learns about the state transition.
 *   (3) Client sends KVS PUT (let's say it gets delivered almost instantly).
 *   (4) A recovery FOM was launched on P1. It missed the KVS PUT.
 * @endverbatim
 *
 *   The stop condition for a local recovery FOM can be broken down to the
 * following sub-conditions applied to every ONLINE-or-TRANSIENT participant
 * of the cluster:
 *   1. Caught-up: participant sends a log record that has the same TX ID as
 *   the first operation recorded by the local recovery FOM.
 *   2. End of log: participant send "end-of-the-log" message.
 * In other words, a participant leaves RECOVERING state when it got REDOs
 * from all the others until the point where new IO was started. Note, there
 * are a lot of corner cases related to interleaving and clock synchronisation
 * but they are not covered here.
 *
 *
 * Stop condition for remote FOM
 * -----------------------------
 *
 *   The next question is "when a remote process should stop sending REDOs?".
 * This question defines the stop condition for a remote recovery FOM. Since
 * there is a race window between the point where a remote process gets
 * RECOVERING event and the point where one of the clients sends a "new"
 * WRITE-like operations, remote participants are not able to use the
 * "until first WRITE" condition. Instead, they rely on HA state transitions.
 * When a participant leaves RECOVERING state, all remote recovery FOMs should
 * be stopped.
 *   When a remote recovery FOM reaches the end of the local DTM0 log, it
 * goes to sleep. It gets awaken when a new record appears in the log. As it
 * was mentioned above, this kind of FOM can be stopped only when HA notifies
 * about state transition of the participant that is being recovered.
 *
 * Stop condition for eviction FOM
 * -------------------------------
 *
 *   The final question is how to stop eviction of a FAILED participant. The
 * stop condition here is that all the records where the participant
 * participated should be replayed to the other participants.
 *   An eviction FOM stops after all the needed log records were replayed.
 *
 *
 * REDO message
 * ------------
 *
 *   REDO messages contain full updates (as opposite to PERSISTENT messages
 * that carry only partial updates). They are sent one-by-one by a
 * recover FOM (remote/eviction). A REDO message is populated using
 * a DTM0 log record. User-specific service provides necessary callbacks
 * to transmute the message if it is required.
 *   A log record can be removed only by the local log pruner daemon.
 * DTM0 log locks or the locality lock can be used to avoid such kind of
 * conflicts (up to the point where a shot-lived lock can be taken to make a
 * copy of a log record).
 *   The REDO message is just a wrapper around user-specific WRITE-like
 * operation (for example, CAS op). Aside from user request, it may contain
 * the most recent information about the states of the transaction on the
 * other participants and information that helps to identify the sender
 * DTM0 service:
 * @verbatim
 *   REDO message request {
 *      struct pa_state latest_states[];
 *      uint64_t        source;
 *      struct buf      original_request;
 *   }
 *   REDO message reply {
 *      struct buf      reply_to_original_request;
 *   }
 * @endverbatim
 *   The receiver should send back a packed reply or an empty reply. The reply
 * serves two purposes:
 *   - Establishes back-pressure. As it was described above, a remote recovery
 *   or eviction FOM sends packets one-by-one awaiting replies each time. This
 *   could be a subject to some batching policy in future.
 *   - Helps to detect failures. For example, if one of the participants
 *   reports an error (ENOMEM or ENOENT) then it may mean that HA should
 *   put the participant into TRANSIENT state (by rebooting the OS process)
 *   or even mark it as failed (for example, if a certain index does
 *   not exist there).
 *
 *
 * REDO FOM
 * --------
 *
 *   Whenever a REDO message is received, DTM0 service schedules a REDO FOM.
 * The FOM creates a user-specific FOP (for example, CAS FOP), then it creates
 * an FOM for this FOP (let's call it uFOM). DTM0 service uses fom_interpose
 * API to execute the uFOM. Upon uFOM completion, the REDO FOM sends back
 * an RPC reply with REDO message reply FOP.
 *   Additional information carried by a REDO fop may be applied separately
 * to the DTM0 log.
 *
 *
 * Recovery and HA EOS
 * -------------------
 *
 *   HA EOS is needed to replay the events sent to the local participant
 * in order to "catch up" with the most recent state of the cluster.
 * If some the events were "consumed" (committed) by DTM0 then
 * they should not be replayed.
 *   Here is a list of allowed state transitions for a participant:
 *     TRANSIENT  -> RECOVERING (1)
 *     TRANSIENT  -> FAILED     (2)
 *     RECOVERING -> ONLINE     (3)
 *     RECOVERING -> FAILED     (4)
 *     RECOVERING -> TRANSIENT  (5)
 *     ONLINE     -> TRANSIENT  (6)
 *     ONLINE     -> FAILED     (7)
 *     FAILED     -> EVICTED    (*)
 *   Note that replay if the local HA log may be required only if
 * the participant enters TRANSIENT state: all other transitions happen
 * either without losing of the volatile state of the participant or
 * with losing of its permanent state (which means its volatile state
 * cannot be recovered at all).
 *   When a participant enters TRANSIENT (5 and 6), its previous transitions
 * (1, 3, 4, 5) get "consumed".
 *   When a participant leaves RECOVERING (3, 4, 5), its previous transition
 * (1) gets "consumed".
 *   When a participant enters FAILED, its previous transitions (1, 3, 5, 6)
 *  get "consumed".
 *   When a participant leaves FAILED (*), its previous transition (7) gets
 * "consumed". Note, this transition is not well-defined yet. If the transition
 * (7) causes every participant enter RECOVERING (i.e, ONLINE -> RECOVERING
 * is allowed) then this is not required, so that any transition to FAILED
 * gets "consumed" as soon as it gets delivered.
 *
 *
 * Originators' reaction to TRANSIENT failures
 * -------------------------------------
 *
 *  When one of the participants with persistent state enters TRANSIENT,
 * an originator (a participant without persistent state) must force
 * the DTXes that contain this TRANSIENT participant to progress without
 * waiting on a reply ("EXECUTED") or on a DTM0 PERSISTENT message.
 *  Originator's DTM0 service participates in recovery process in the
 * same way as the other types of participants.
 *
 *
 * Errata and comments
 * -------------------
 *
 *  1. PROCESS_RECOVERED is be used as an indicator of completeness.
 *     The existing states (PROCESS_START{ING,ED}) keep their original
 *     meaning.
 *  2. REDO may contain extra information about the last seen TXD or about
 *     the point where the process received the corresponding * -> RECOVERING
 *     state transition event.
 *  3. "REDO messages are sent one-by-one" is only a starting point. They
 *     can be one-way messages (with DTM0 log-based re-sends) but it has
 *     to be added at the top of basic recovery.
 *  4. WRITE-like opeartion means actual PUT/DEL requrests for the very first
 *     version of DTM0 (it should support only DIX).
 *  5. Eviction is not fully covered by this document.
 */

/*
 *  Local variables:
 *  c-indentation-style: "K&R"
 *  c-basic-offset: 8
 *  tab-width: 8
 *  fill-column: 80
 *  scroll-step: 1
 *  End:
 */
/*
 * vim: tabstop=8 shiftwidth=8 noexpandtab textwidth=80 nowrap
 */
