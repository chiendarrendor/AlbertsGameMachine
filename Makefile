
all: 
	(cd server ; make)
	(cd client ; make)
	(cd Outpost/tca ; make)

install:
	(cd client ; make install)
	(cd Outpost/tca ; make install)



