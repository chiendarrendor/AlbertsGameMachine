---
name: build-environment
description: "SUPERSEDED 2026-09-16: The Game Machine's C++ build was intentionally dual-environment (Windows/MSYS local dev + Linux/EC2 deploy); Albert has now decided to drop MSYS support and develop Linux-only going forward, since all future releases come out of AWS"
metadata: 
  node_type: memory
  type: feedback
  originSessionId: 416ca727-2f1a-4082-9558-d306f19709ba
  modified: 2026-09-16T00:00:00.000Z
---

**UPDATE 2026-09-16:** Albert has decided to stop supporting MSYS/Windows builds — all future Game
Machine development happens on Linux (this EC2 box), since all releases come out of AWS anyway. The
dual-environment history below is kept for context (e.g. if old MSYS-era Makefile branches are seen
elsewhere in the repo, this explains why they exist), but is no longer the guidance to follow: do
**not** preserve/restore `OSTYPE`-conditioned dual-path Makefile branches going forward, and it's
fine to simplify a Makefile to a single Linux path when touched. No project-wide sweep to strip
existing MSYS branches has been done yet — that's an opportunistic cleanup as files get touched, not
a dedicated pass, unless Albert asks for one.

---

Historical context (no longer current guidance): Albert used to have a general professional dislike of legacy projects carrying build/deploy support for environments other than the single final deployment target — but explicitly prided himself that **this particular personal project was dual-environment-capable on purpose**: it built both locally on Windows/MSYS and on the Linux EC2 deployment target (see [[aws-deployment]]).

The mechanism (found in `server/Makefile`) is an `OSTYPE`-conditioned `BOOSTINC`:
```makefile
ifeq (${OSTYPE},msys)
BOOSTINC=/c/Boost/include/boost-1_46
endif
ifeq (${OSTYPE},linux)
BOOSTINC=/usr/include
endif
```

**Discovered 2026-09-14, during a MerchantOfVenus Boost build-error investigation:** during Albert's recent Makefile refresh for EC2 deployment (server, generic client, Outpost backend+frontend), `Outpost/tca/Makefile` was simplified to hard-code `BOOSTINC=/usr/include` — dropping the `OSTYPE` conditional entirely, i.e. it silently lost local-Windows buildability. `MerchantOfVenus/tca/Makefile` was left out of the refresh altogether and never had the conditional at all (predates it, or lost it), which is the immediate cause of MoV's build error (`BOOSTINC` resolves empty). Also: no Boost install of any kind currently exists on this Windows machine — the `msys` branch's path (`/c/Boost/include/boost-1_46`) doesn't exist, so even a syntactically-fixed Makefile can't build MoV locally until Boost 1.46 (MinGW-compatible) is (re)installed.

**Gotcha confirmed 2026-09-14 — `OSTYPE` must be explicitly exported:** bash sets `$OSTYPE` as a built-in shell parameter, but does NOT export it to child processes by default — `make` (a separate process) sees it as completely undefined unless something does `export OSTYPE=...` first, which silently defeats every `ifeq (${OSTYPE},...)` branch (leaves `BOOSTINC` fully unset, not just empty). Albert's fix: `export OSTYPE=msys` is now in `~/.bash_profile` on this MSYS/MinGW machine (and `export OSTYPE=linux` in `.bashrc` on his Linux/EC2 host). **Tool-specific trap:** each Bash-tool invocation in this Claude Code session starts a fresh shell — exported vars from one Bash call do NOT persist to the next one (only the working directory does) — so `source ~/.bash_profile` must be chained in the *same* command as any `make` invocation (e.g. `source ~/.bash_profile && make`), not run as a separate prior step.

**Also confirmed 2026-09-14:** the installed local MinGW compiler is GCC 6.3.0, while the Boost libs the Makefiles reference are named for `mgw45` (GCC 4.5) — even after fixing `BOOSTINC`, old prebuilt Boost 1.46 `.a` files won't ABI-match this compiler. No Boost install of any kind currently exists on this machine at all. Albert has said sourcing/building a GCC-6.3-compatible Boost locally is his own task, not something to hand to Claude.

**How to apply:** When fixing any per-game `tca/Makefile`, restore/preserve the `OSTYPE`-conditioned dual-path pattern from `server/Makefile` — do not follow `Outpost/tca/Makefile`'s simplified single-path version as the template, since that itself is a (likely unintentional) regression against Albert's stated intent. Worth proactively flagging to Albert if `Outpost/tca/Makefile`'s dropped Windows-buildability comes up, since he may want it restored too.

---

**Client-side GUI XML is spliced together with the C preprocessor at build time — discovered
2026-09-27, reading `Outpost/tca/Makefile` together with Albert while walking the client
architecture (see [[component-java-client]] for the GUI side of this).** The source files under a
game's `client/` directory (e.g. `Outpost/client/OutpostClient.xml`) are not the files actually
served to the Java client — they're templates using literal, non-standard `#include "otherfile.xml"`
directives to split one logical `<gameboard>` document (event schema, actions, windows, tabs) across
several physical files (`events.xml`/`actions.xml`/`Options.xml`/`Players.xml`/`ResourceRules.xml`
for Outpost). `Outpost/tca/Makefile` has a rule that runs the real C preprocessor over the top file
and writes the fully-spliced, well-formed result into `tca/`:
```makefile
CPP=cpp
OutpostClient.xml: ../client/OutpostClient.xml 
	${CPP} -I../client -P ../client/OutpostClient.xml > OutpostClient.xml
```
(`-P` suppresses `cpp`'s line-marker output, which isn't meaningful in a non-C file anyway.) The
*generated* `tca/OutpostClient.xml` is what `clientinstall` actually `aws s3 cp`'s up to CloudFront
(see [[aws-deployment]]) — i.e. the real, deployed, client-facing asset lives only as a build
artifact, never checked into git as such. Confirmed as a project-wide convention, not an
Outpost-only quirk: `#include` shows up the same way in `AOR/client/AORClient.xml`,
`AOR/client/gameboard.xml`, and even a *server*-side file, `MerchantOfVenus/MerchantOfVenusServer.xml`.

**Why this is worth flagging as non-standard, not just a curiosity:** XML has two real, spec-native
ways to include one document's tree inside another — general external entities (DTD-based,
`<!ENTITY foo SYSTEM "foo.xml">` + `&foo;`, also the classic XXE attack vector) and XInclude
(`xi:include`, which the parser must opt into via `setXIncludeAware(true)` on a
namespace-aware `DocumentBuilderFactory`). `GUIUnit.ParseXML`'s `DocumentBuilderFactory` uses
neither. Splicing with `cpp` instead means the include mechanism has zero awareness of XML
structure — it's pure text substitution before any parser ever runs, so nothing enforces that an
included fragment is well-formed in isolation or checks include-depth, unlike either real XML
mechanism. Same underlying trick (reusing `cpp` as a generic macro/include tool well outside its
usual C/C++ role) as the `GAME_SERVER_NAME()` function-like-macro pattern from Phase 1B of
[[server-game-interface-spec]] — worth remembering as a recurring pattern in this codebase, not a
one-off.

**How to apply:** when investigating or editing any per-game client GUI XML, always check the
game's `tca/Makefile` for a `${CPP} ... -P` rule before assuming a `client/*.xml` file is the real,
final document — the physical file under `client/` may be one fragment of a larger spliced whole,
and the actually-deployed content only exists post-build, in `tca/`.

---

**Java client builds now output to a separate `classes/` directory, not next to source — fixed
2026-09-28 (Albert: finding `.class` files sitting next to `.java` files in his IDE was "an
irritant").** Previously, none of the four client-facing Makefiles (`client/Makefile`,
`Outpost/tca/Makefile`, `MerchantOfVenus/tca/Makefile`, `roomgui/Makefile`) used `javac -d` correctly
— `client/Makefile` and `roomgui/Makefile` didn't use `-d` at all (defaulting to in-situ output), and
the two per-game `tca/Makefile`s used `-d ${CLIENTJAVADIR}` (`../client`), which pointed *back into*
the source tree rather than away from it. Fixed by adding a `CLASSDIR=classes` (or `CLIENTCLASSDIR`)
variable to each and pointing `-d` at it — following the same "build artifacts belong in a dedicated
directory, not mixed with source" principle `tca/` itself already embodies for C++/generated output
(see the `cpp`-splicing note above), just extended to Java. Packaging steps (`jar cf`, `clientinstall`'s
`cp` of `.class` files) were updated to read from the new location.

**Real, non-obvious dependency this uncovered:** `Outpost/tca/Makefile` and `MerchantOfVenus/tca/Makefile`
compile their per-game client classes against `-classpath ../../client` — i.e. they depend on
`client/`'s shared framework classes (`NodeInterfacePackage`, etc.) already being compiled and sitting
*in-situ* there. Once `client/Makefile`'s own build stopped putting `.class` files there (writing to
`client/classes/` instead), that classpath silently pointed at a now-empty location. Fixed by changing
both to `-classpath ../../client/classes` (and `roomgui/Makefile`'s equivalent `-classpath ../client` to
`../client/classes`) — but this is a real build-order dependency worth remembering: `client/`'s own
build must run first (producing `client/classes/`) before any per-game `clientjava`/roomgui `java`
target can succeed from a clean checkout.

**A second, unrelated pre-existing issue found while verifying this (flagged, not fixed — same
"don't drive-by-fix unrelated issues" policy as elsewhere in this file):** `MerchantOfVenus/tca/Makefile`
still hardcodes a Windows JDK path (`JAVADIR=/c/Program\ Files/Java/jdk-19`, `JAVAC=${JAVADIR}/bin/javac`)
— a leftover from before the MSYS→Linux migration that was never updated for this specific Makefile,
same class of gap as `Outpost/tca/Makefile`'s old `BOOSTINC`/library-suffix issues elsewhere in this
file. `clientjava`/`voronoi` currently only work on this Linux box via `make JAVAC=javac ...`
overriding the default. Worth fixing the same way the other per-Makefile Linux-migration gaps were —
just not done as part of this `classes/` fix, since it's a separate concern.

**Also flagged, not yet acted on (Albert, 2026-09-28):** whether each Makefile's `clientinstall`/
`awsinstall` targets actually deploy the resulting jars/XML to the *correct* place in S3 for CloudFront
to serve — see [[aws-deployment]]'s new note. This `classes/` fix only touched *local* file-path
correctness, not the S3-side install logic.
