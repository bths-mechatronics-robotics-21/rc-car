PORT ?=

all:
	arduino-cli compile --fqbn arduino:avr:uno ./main

clean:
	rm -vrf ./main/build

flash: all
ifndef PORT
	$(error variable: [PORT] (not defined))
endif
	arduino-cli upload --verify --port $(PORT) --fqbn arduino:avr:uno ./main

.PHONY: clean flash
