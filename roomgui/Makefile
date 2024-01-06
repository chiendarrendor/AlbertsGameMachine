
AWSBUCKET=albertsgamemachine

#JAVAC=javac
JAVAC=javac -Xlint:unchecked
JAR=jar cvf

INSTALLDIR=/c/apache/htdocs/client/ROOMGUI

JAVAS=MainPage.java RoomPage.java

JAVALOCS=$(JAVAS:%=ROOMGUI/%)


java:
	- rm ROOMGUI/*.class
	${JAVAC} -d . -sourcepath . -classpath ../client ${JAVALOCS}
	${JAR} ROOMGUI.jar ROOMGUI/*.class





installaws: java
	aws s3 cp rooms.xml s3://${AWSBUCKET}/
	aws s3 cp ROOMGUI.jar s3://${AWSBUCKET}/

