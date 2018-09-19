include ../../.config
include ../../scripts/config.mak

all: build

libvorbis.a:
	@make -C vorbis

libopus.a:
	@make -C opus

build: $(CONFIG_LIBS)
	@echo "libsrc compile complete .."

clean:
	@find ./ -name "*.o"|xargs rm -f
	@find ./ -name "*.d"|xargs rm -f
	@find ./ -name "*.a"|xargs rm -f
