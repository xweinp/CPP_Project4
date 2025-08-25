CXX := clang++
CXXFLAGS := -Wall -Wextra -std=c++20 -O2

.PHONY: all clean tests

TESTS := tests/concept_test \
         tests/operator_test \
         tests/poly_simple_tests \
         tests/poly_test_1 \
         tests/tests_PUBLIC

all: poly.o

poly.o: poly.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TESTS): %: %.cpp poly.h
	$(CXX) $(CXXFLAGS) $< -o $@

# Build and run all tests
tests: $(TESTS)
	@set -e; \
	for t in $(TESTS); do \
	  echo "[RUN] $$t"; \
	  ./$$t; \
	done

clean:
	rm -f $(TESTS) poly.o
