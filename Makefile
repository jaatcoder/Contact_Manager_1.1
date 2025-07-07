all:
	mkdir -p build
	cd build && cmake .. && make
	cp build/contact_manager ./contact_manager

clean:
	rm -rf build
	rm -f contact_manager main

run:
	./contact_manager

terminal:
	./contact_manager --terminal