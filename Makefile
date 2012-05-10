all:
	scons

check:
	scons test=True && ./amalgam-test --gtest_color=yes

clean:
	find . -name "*.o" -exec rm {} \;
	rm amalgam
