all: debug

format:
	python3 format.py --verbose --write test.lum

wip:
	./lumina test.lum

valwip:
	valgrind ./lumina test.lum

demo:
	./lumina examples/demo/demo.lum

report:
	./lumina report

debug:
	./build_lumina.sh debug

release:
	./build_lumina.sh release

release_native:
	./build_lumina.sh release-native

nightly:
	./build_lumina.sh nightly
