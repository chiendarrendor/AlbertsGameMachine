
AWSBUCKET=albertsgamemachine
CFDISTID=E1TUM5W71MF59M

#JAVAC=javac
JAVAC=javac -Xlint:unchecked
JAR=jar cvf

CLASSDIR=classes

JAVAS=RoomPage.java LineFeedPanel.java

JAVALOCS=$(JAVAS:%=ROOMGUI/%)

.PHONY: java install checkclient

# Explicit, not just "whichever rule happens to be defined first" -- that's
# exactly how this broke (2026-10-19): adding the checkclient rule below,
# textually before this file's real build rule, silently changed plain
# `make`'s default target to checkclient, which only recurses into client/
# and stops -- never reaching anything that looks at RoomWidget.java/etc. at
# all, even though it printed output that looked like a legitimate build.
.DEFAULT_GOAL := java

# roomgui compiles against ../client/classes, so it has to actually depend on
# client's own build, not just assume it's already current -- a bare
# "../client/classes/.compiled:" rule with no recipe-triggering mechanism
# doesn't work here (confirmed 2026-10-19): once that file exists, Make has
# nothing of its own to compare it against and never re-checks it, so editing
# anything under client/ would go unnoticed forever after the first build.
# Marking this recursive step .PHONY forces it to always re-enter
# client/Makefile, which then does its own (already fast, stamp-based)
# up-to-date check -- correctness over the small, fixed cost of recompiling
# roomgui's own two files every time, rather than risking silently stale output.
checkclient:
	- (cd ../client ; ${MAKE} all)

${CLASSDIR}/.compiled: ${JAVALOCS} checkclient
	- rm -f ROOMGUI/*.class
	- rm -rf ${CLASSDIR}
	mkdir -p ${CLASSDIR}
	${JAVAC} -d ${CLASSDIR} -sourcepath . -classpath ../client/classes ${JAVALOCS}
	${JAR} ROOMGUI.jar -C ${CLASSDIR} .
	touch $@

java: ${CLASSDIR}/.compiled


install: java
	aws s3 cp rooms.xml s3://${AWSBUCKET}/
	aws s3 cp ROOMGUI.jar s3://${AWSBUCKET}/
	aws cloudfront create-invalidation --distribution-id ${CFDISTID} --paths "/*"

