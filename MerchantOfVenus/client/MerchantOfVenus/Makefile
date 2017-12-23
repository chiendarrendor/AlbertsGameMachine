JAVADIR=/c/Program\ Files/Java/jdk1.7.0_01
COMMON=../../client
JAVAC=${JAVADIR}/bin/javac -classpath $(COMMON) -sourcepath .
JAR=${JAVADIR}/bin/jar
JARSIGNER=${JAVADIR}/bin/jarsigner
INSITUCLASSES=$(JAVACLASSES:%=MerchantOfVenus/%)
JAVAMAIN=MerchantOfVenus/VoronoiApplet.java
JARNAME=VoronoiApplet.jar
MAPFILE=MerchantOfVenusMap.xml

jar:
	-rm *.class
	(cd .. ; $(JAVAC) $(JAVAMAIN))
	(cd .. ; $(JAR) -cf $(JARNAME) MerchantOfVenus/*.class)
	mv ../$(JARNAME) .

# an unsigned applet will run in 'sandbox' mode
# a signed applet will, be default run in 'all-permissions'
# and will fail in Browsers unless the Publisher is
# recognized.  Since I don't need this code
# to run in all-permissions, signing is unnecessary
#	$(JARSIGNER) -storepass foobar $(JARNAME) mykey
