FLAGS =
EXEC_NAME  = exec.exe
SOURCE_DIR = source
BUILD_DIR = build
INC_DIR = include

SOURCE_FILES = $(wildcard $(SOURCE_DIR)/*.cpp)
OBJ_FILES    = $(patsubst $(SOURCE_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SOURCE_FILES))

$(EXEC_NAME): $(OBJ_FILES)
	g++ $^ $(FLAGS) -o $@


$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.cpp
	g++ -c $(FLAGS) -I$(INC_DIR)/ $< -o $@

run: $(EXEC_NAME)
	./$(EXEC_NAME)

clean:
	rm -f $(OBJ_FILES) $(EXEC_NAME)
