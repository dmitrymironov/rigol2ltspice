.DEFAULT_GOAL := help

SHELL := /bin/bash

IMAGE_NAME := rigol2ltspice-build:local
BUILD_DIR := build
BINARY := $(BUILD_DIR)/rigol
TEST_DIR := test
DOC_DIR := docs
DOC_BUILD_DIR := $(BUILD_DIR)/docs

.PHONY: help image build doc test clean

help:
	@echo "Targets:"
	@echo "  make help   - Show this help"
	@echo "  make image  - Build local Docker image with build-essential"
	@echo "  make build  - Build converter into build/rigol using Docker"
	@echo "  make doc    - Render PlantUML diagram into build/docs using Docker"
	@echo "  make test   - Build and run 3 fixture-based tests"
	@echo "  make clean  - Remove build artifacts"

image: Dockerfile.build
	@docker build -f Dockerfile.build -t $(IMAGE_NAME) .

$(BUILD_DIR)/.dir:
	@mkdir -p $(BUILD_DIR)
	@touch $(BUILD_DIR)/.dir

build: image | $(BUILD_DIR)/.dir
	@docker run --rm \
		-u "$$(id -u):$$(id -g)" \
		-v "$$(pwd)":/work \
		-w /work \
		$(IMAGE_NAME) \
		bash -lc "g++ -std=c++11 -O2 -Wall -Wextra rigol.cpp -o $(BINARY)"
	@echo "Built $(BINARY)"

doc: | $(DOC_BUILD_DIR)
	@docker run --rm \
		-u "$$(id -u):$$(id -g)" \
		-e HOME=/tmp \
		-e JAVA_TOOL_OPTIONS=-Duser.home=/tmp \
		-v "$$(pwd)":/work \
		-w /work \
		plantuml/plantuml:latest \
		-ttxt /work/$(DOC_DIR)/rigol_usage.puml -o /work/$(DOC_BUILD_DIR)
	@echo "Generated $(DOC_BUILD_DIR)/rigol_usage.atxt"

$(DOC_BUILD_DIR):
	@mkdir -p $(DOC_BUILD_DIR)

test: build
	@mkdir -p $(BUILD_DIR)/test
	@echo "[T1] Raw Rigol CSV auto-shift should start at zero and preserve sample count"
	@$(BINARY) $(TEST_DIR)/NewFile0.csv $(BUILD_DIR)/test/newfile0.txt >/dev/null
	@test "$$(wc -l < $(BUILD_DIR)/test/newfile0.txt)" = "16384"
	@test "$$(head -n 1 $(BUILD_DIR)/test/newfile0.txt)" = "0.000000 1.100000"
	@echo "[T2] Explicit +1ms shift should offset first timestamp to -0.006272"
	@$(BINARY) $(TEST_DIR)/NewFile0.csv $(BUILD_DIR)/test/newfile0_shift1ms.txt 1 >/dev/null
	@test "$$(head -n 1 $(BUILD_DIR)/test/newfile0_shift1ms.txt)" = "-0.006272 1.100000"
	@echo "[T3] Already-converted two-column text fixture should be ignored by CSV parser"
	@$(BINARY) $(TEST_DIR)/rigol-1.csv $(BUILD_DIR)/test/rigol1_out.txt >/dev/null
	@test "$$(wc -l < $(BUILD_DIR)/test/rigol1_out.txt)" = "0"
	@echo "All tests passed."

clean:
	@rm -rf $(BUILD_DIR)
	@rm -f rigol
	@echo "Cleaned build artifacts."
