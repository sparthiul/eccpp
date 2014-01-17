MODULES = basics mll tests tools
all:$
	@for dir in $(MODULES); do ${MAKE} $@ -C $$dir; exit_status=$$?; \
	if [ $$exit_status -ne 0 ]; then exit $$exit_status; fi; done
clean:
	cd basics && make clean
	cd mll && make clean
	cd tests && make clean
	cd tools && make clean
test:
	cd tests && ./unit_tests && cd ..
