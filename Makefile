build_all: build_test build_src

build_test:

	$(MAKE) -C test

build_src:

	$(MAKE) -C src
