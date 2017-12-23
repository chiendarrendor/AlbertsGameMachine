INSTALLTARGET=/c/apache/htdocs/client

JAVADIR=C:/Program\ Files/Java/jdk1.6.0_25

JAVAC=${JAVADIR}/bin/javac
JAVAJAR=${JAVADIR}/bin/jar

JCMD=${JAVAC} -classpath . -Xlint:unchecked -Xlint:deprecation

SUBDIRS = OpenZorz NodeInterfacePackage Utilities MySwing DOMUtilities Voronoi
SUBCLASS = $(SUBDIRS:%=%/*.class)
SUBJAVA = $(SUBDIRS:%=%/*.java)

all:
	- rm ${SUBCLASS}
	${JCMD} OpenZorz/GameClient.java
	${JCMD} OpenZorz/GUITester.java
	${JCMD} OpenZorz/ClientApplet.java
	${JCMD} MySwing/*.java
	${JCMD} Voronoi/*.java

jar: all
	${JAVAJAR} cf Client.jar ${SUBCLASS}
	

install: jar
	cp Client.jar ${INSTALLTARGET}