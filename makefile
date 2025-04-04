PROJECT_NAME = UAV
BUILD_DIR = build
EXECUTABLE = $(BUILD_DIR)/$(PROJECT_NAME)

.PHONY: gl configure build clean run

# This will build the project
UAV: configure build

# Configure and build the project
configure:
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && cmake ..

build:
	@cmake --build $(BUILD_DIR)

# This will run the executable
run: $(EXECUTABLE)
	@./$(EXECUTABLE)

clean:
	@rm -rf $(BUILD_DIR)
