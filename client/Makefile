AWSBUCKET = albertsgamemachine
CFDISTID = E1TUM5W71MF59M

JAVAC=javac
JAVAJAR=jar

CLASSDIR=classes

JCMD=${JAVAC} -d ${CLASSDIR} -classpath . -Xlint:unchecked -Xlint:deprecation

SUBDIRS = TheGameMachine NodeInterfacePackage Utilities MySwing DOMUtilities Voronoi org/json
SUBJAVA = $(SUBDIRS:%=%/*.java)
JAVASOURCES := $(wildcard $(SUBJAVA))

.PHONY: all jar install

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

install: Client.jar
	aws s3 cp Client.jar s3://${AWSBUCKET}/
	aws cloudfront create-invalidation --distribution-id ${CFDISTID} --paths "/*"