INSTALLTARGET=/c/apache/htdocs/client

AWSBUCKET = albertsgamemachine

JAVAC=javac
JAVAJAR=jar

CLASSDIR=classes

JCMD=${JAVAC} -d ${CLASSDIR} -classpath . -Xlint:unchecked -Xlint:deprecation

SUBDIRS = TheGameMachine NodeInterfacePackage Utilities MySwing DOMUtilities Voronoi
SUBJAVA = $(SUBDIRS:%=%/*.java)

all:
	- rm -rf ${CLASSDIR}
	- mkdir -p ${CLASSDIR}
	${JCMD} TheGameMachine/GameClient.java
	${JCMD} TheGameMachine/GUITester.java
	${JCMD} TheGameMachine/ClientApplet.java
	${JCMD} TheGameMachine/ClientApplication.java
	${JCMD} TheGameMachine/TestResource.java
	${JCMD} MySwing/*.java
	${JCMD} Voronoi/*.java

jar: all
	${JAVAJAR} cf Client.jar -C ${CLASSDIR} .


install: jar
	cp Client.jar ${INSTALLTARGET}

awsinstall: jar
	aws s3 cp Client.jar s3://${AWSBUCKET}/