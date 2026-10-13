
AWSBUCKET=albertsgamemachine
CFDISTID=E1TUM5W71MF59M

#JAVAC=javac
JAVAC=javac -Xlint:unchecked
JAR=jar cvf

CLASSDIR=classes

JAVAS=RoomPage.java LineFeedPanel.java

JAVALOCS=$(JAVAS:%=ROOMGUI/%)

.PHONY: java install

${CLASSDIR}/.compiled: ${JAVALOCS}
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

