CXX           = g++
PLATFORM      = -DSKY_LINUX -DSKY_X11_CONTEXT -DSKY_FMODEX_SYNTH
CXXFLAGS      = -g -w -pedantic -std=c++11 -ffast-math -ffunction-sections -fgcse -I../include -I../skyoralis/include -DGL_GLEXT_PROTOTYPES -DSKY_DEBUG $(PLATFORM)
LDFLAGS       = -L../skyoralis/build/lib -L../skyoralis/lib -lskyoralis -lGL -lX11 -lfmodex
EXEC_DIR_PATH = ./bin
RELEASE       = evoke2013_64k
EXEC          = $(RELEASE).bin
PACKER        = $(RELEASE)
COMPRESS_LVL  = 6
OBJ           = \
								main.o\
								\
								fsm.cube_room.o\
								fsm.init.o\

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
	@echo "-- Compressing"
	@xz -$(COMPRESS_LVL)ck $(EXEC_DIR_PATH)/$(EXEC) >> $(EXEC_DIR_PATH)/$(PACKER)
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
