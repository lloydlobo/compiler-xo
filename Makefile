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

lumfmt:
	python3 pkg/lumfmt/lumfmt.py --verbose --write test.lum


# DEV MODE

dev_run:
	./lumina test.lum

memcheck_dev_run:
	valgrind make dev_run

debug_memcheck_dev_run_exec:
	make debug & make memcheck_dev_run
	./out & echo $?

