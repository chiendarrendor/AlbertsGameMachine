COMMON=../../client/classes
JAVAC=javac -classpath $(COMMON) -sourcepath .
JAR=jar
INSITUCLASSES=$(JAVACLASSES:%=MerchantOfVenus/%)
JAVAMAIN=MerchantOfVenus/VoronoiPanel.java
JARNAME=VoronoiPanel.jar
MAPFILE=MerchantOfVenusMap.xml

.PHONY: jar

$(JARNAME): ../$(JAVAMAIN)
	-rm *.class
	(cd .. ; $(JAVAC) $(JAVAMAIN))
	(cd .. ; $(JAR) -cf $(JARNAME) MerchantOfVenus/*.class)
	mv ../$(JARNAME) .

jar: $(JARNAME)
