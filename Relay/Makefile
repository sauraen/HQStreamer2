JUCE_MODULES = juce_core juce_events
SRC = main.cpp relay.cpp
EXECUTABLE = hqs2relay

CC = g++
CFLAGS = -g -std=c++14 -c -Wall -Wno-sign-compare -DJUCE_GLOBAL_MODULE_SETTINGS_INCLUDED=1 -DJUCE_STANDALONE_APPLICATION=0 -DJUCE_USE_CURL=0 -I../JuceLibraryCode/modules
LDFLAGS = -lpthread -ldl

JUCE_MODULES_OBJECTS = $(JUCE_MODULES:%=build/%.o)
SRC_OBJECTS = $(SRC:%.cpp=build/%.o)

.PHONY: default clean

default: $(EXECUTABLE)

$(JUCE_MODULES_OBJECTS):
	@mkdir -p build
	$(CC) $(CFLAGS) -o $@ ../JuceLibraryCode/modules/$(*F)/$(*F).cpp

$(SRC_OBJECTS): build/%.o: src/%.cpp
	@mkdir -p build
	$(CC) $(CFLAGS) -o $@ $<

$(EXECUTABLE): $(JUCE_MODULES_OBJECTS) $(SRC_OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^

clean:
	rm build/*.o $(EXECUTABLE)
