
#JAVAC=javac
JAVAC=/c/Program\ Files/Java/jdk1.6.0_25/bin/javac

INSTALLDIR=/c/apache/htdocs/client/ROOMGUI

JAVAS=MainPage.java RoomPage.java

JAVALOCS=$(JAVAS:%=ROOMGUI/%)


java:
	- rm ROOMGUI/*.class
	${JAVAC} -d . -sourcepath . -classpath ../client ${JAVALOCS}





install: java
	cp rooms.xml ${INSTALLDIR}
	cp ROOMGUI/*.class ${INSTALLDIR}

