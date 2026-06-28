CXX ?= c++
CXXFLAGS ?= -std=c++11 -Wall -Wextra -O0 -g -I fake -I .
SRC = test_main.cpp test_lifecycle.cpp test_cv.cpp test_audio.cpp test_leds.cpp test_buttons.cpp test_meter.cpp test_euclid.cpp test_sequencer.cpp
EXISTING = $(wildcard $(SRC))
run: $(EXISTING)
	$(CXX) $(CXXFLAGS) $(EXISTING) -o run
	./run
clean:
	rm -f run
.PHONY: run clean
