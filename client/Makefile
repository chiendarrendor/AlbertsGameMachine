
AWSBUCKET=albertsgamemachine
INSTALLTARGET=/c/apache/htdocs/client

JAVADIR=C:/Program\ Files/Java/jdk1.6.0_25

JAVAC=javac
JAVAJAR=jar

JCMD=${JAVAC} -classpath . -Xlint:unchecked -Xlint:deprecation

SUBDIRS = OpenZorz NodeInterfacePackage Utilities MySwing DOMUtilities Voronoi
SUBCLASS = $(SUBDIRS:%=%/*.class)
SUBJAVA = $(SUBDIRS:%=%/*.java)

all:
	- rm ${SUBCLASS}
	${JCMD} OpenZorz/GameClient.java
	${JCMD} OpenZorz/GUITester.java
	${JCMD} OpenZorz/ClientApplet.java
	${JCMD} OpenZorz/ClientApplication.java
	${JCMD} MySwing/*.java
	${JCMD} Voronoi/*.java

jar: all
	${JAVAJAR} cf Client.jar ${SUBCLASS}
	

installaws: jar
# TODO: figure out how to add CloudFront Cache Validation here
	aws s3 cp Client.jar s3://${AWSBUCKET}/
