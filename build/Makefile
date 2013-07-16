CXX           = clang++
PLATFORM      = -DSKY_LINUX -DSKY_SDL_CONTEXT -DSKY_FMODEX_SYNTH
CXXFLAGS      = -g -W -Wall -Wextra -pedantic -std=c++11 -ffast-math -ffunction-sections -fgcse -I../include -I../skyoralis/include -DGL_GLEXT_PROTOTYPES -DSKY_DEBUG $(PLATFORM)
LDFLAGS       = -L../skyoralis/build/lib -L../skyoralis/lib -lskyoralis -lGL -lSDL -lfmodex
EXEC_DIR_PATH = ./bin
RELEASE       = evoke2013_64k
EXEC          = $(RELEASE).bin
XM            = CentralStation.xm
PACKER        = $(RELEASE)
COMPRESS_LVL  = 6
OBJ           = \
								main.o\
								\
								fsm.cave.o\
								fsm.cube_room.o\
								fsm.init.o\
								fsm.laser.o\
								fsm.liquid.o\
								fsm.slab.o\
								fsm.stairway.o

.PHONY: all, clean, mrproper

all: $(OBJ)
	@mkdir -p $(EXEC_DIR_PATH)
	@echo "-- Linking $(RELEASE)"
	@$(CXX) $(OBJ) -o $(EXEC_DIR_PATH)/$(EXEC) $(CXXFLAGS) $(LDFLAGS)
	@echo "-- Done!"

test: all
	@cd $(EXEC_DIR_PATH) && ./$(EXEC) 800 600

skyoralis:
	@cd ../skyoralis/build && git pull origin master && make

release: all
	@echo "-- Compressing release"
	@echo "-- Discarding useless symbols"
	@strip -s $(EXEC_DIR_PATH)/$(EXEC)
	@sstrip -z $(EXEC_DIR_PATH)/$(EXEC)
	@echo "-- Preparing packer"
	@cp packer $(EXEC_DIR_PATH)/$(PACKER)
	@cp $(XM) $(EXEC_DIR_PATH)
	@echo "-- Compressing"
	@cd $(EXEC_DIR_PATH) && tar -cJO $(EXEC) $(XM) >> $(PACKER)
	@chmod +x $(EXEC_DIR_PATH)/$(PACKER)
	@echo "-- Done!"

main.o: ../src/main.cpp
	@echo "-- Compiling ENTRYPOINT $@"
	@$(CXX) -c $< -o $@ $(CXXFLAGS)

fsm.%.o: ../src/fsm/%.cpp ../include/fsm/%.hpp
	@echo "-- Compiling FSM $@"
	@$(CXX) -c $< -o $@ $(CXXFLAGS)


%.o: ../src/%.cpp ../include/%.hpp
	@echo "-- Compiling $@"
	@$(CXX) -c $< -o $@ $(CXXFLAGS)

clean:
	@echo "-- Cleaning up"
	@rm -rf *.o $(EXEC_DIR_PATH)

mrproper:
	@echo "-- MrPropering"
	@rm -f *.o
