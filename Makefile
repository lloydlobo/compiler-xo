all: debug

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
