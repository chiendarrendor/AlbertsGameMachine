INSTALLTARGET=/c/apache/htdocs/client

AWSBUCKET = albertsgamemachine

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
	${JCMD} OpenZorz/TestResource.java
	${JCMD} MySwing/*.java
	${JCMD} Voronoi/*.java

jar: all
	${JAVAJAR} cf Client.jar ${SUBCLASS}
	

install: jar
	cp Client.jar ${INSTALLTARGET}

awsinstall: jar
	aws s3 cp Client.jar s3://${AWSBUCKET}/