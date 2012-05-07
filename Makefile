all:
	scons

clean:
	find . -name "*.o" -exec rm {} \;
	rm amalgam
