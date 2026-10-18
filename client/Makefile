AWSBUCKET = albertsgamemachine
CFDISTID = E1TUM5W71MF59M

JAVAC=javac
JAVAJAR=jar

CLASSDIR=classes

JCMD=${JAVAC} -d ${CLASSDIR} -classpath . -Xlint:unchecked -Xlint:deprecation

SUBDIRS = TheGameMachine NodeInterfacePackage Utilities MySwing DOMUtilities Voronoi org/json org/doghouse/jsonpath
SUBJAVA = $(SUBDIRS:%=%/*.java)
JAVASOURCES := $(wildcard $(SUBJAVA))

.PHONY: all jar install test

# No test framework (JUnit or otherwise) is vendored anywhere in this
# project's Java client -- each class below is a plain main()-driven runner
# (see org/doghouse/jsonpath/JsonPathTest.java), printing per-check results
# and exiting non-zero on any failure. Hand-maintained, same spirit as the
# explicit TESTOBJS lists the C++ side's Makefiles already use.
TESTCLASSES = org.doghouse.jsonpath.JsonPathTest NodeInterfacePackage.GuiPacketQueryTest NodeInterfacePackage.GuiPacketParserTest

# javac's own transitive reachability analysis (compile a handful of 'root'
# files and let it pull in whatever they reference) is not reliable as a way
# to find every file that needs compiling -- it only finds what's reachable
# from the given roots, so a file nothing happens to reference yet silently
# never gets compiled. Compiling the real, complete file list instead avoids
# that; depending on that same list as prerequisites (rather than a bare
# always-runs recipe) is what lets make skip this entirely when nothing
# changed, instead of re-doing it on every invocation.
${CLASSDIR}/.compiled: ${JAVASOURCES}
	- rm -rf ${CLASSDIR}
	mkdir -p ${CLASSDIR}
	${JCMD} ${SUBJAVA}
	touch $@

all: ${CLASSDIR}/.compiled

Client.jar: ${CLASSDIR}/.compiled
	${JAVAJAR} cf Client.jar -C ${CLASSDIR} .

jar: Client.jar

test: ${CLASSDIR}/.compiled
	@for t in ${TESTCLASSES}; do \
	  echo "--- $$t ---"; \
	  java -classpath ${CLASSDIR} $$t || exit 1; \
	done

install: Client.jar
	aws s3 cp Client.jar s3://${AWSBUCKET}/
	aws cloudfront create-invalidation --distribution-id ${CFDISTID} --paths "/*"