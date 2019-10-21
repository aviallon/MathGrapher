.PHONY: clean All

All:
	@echo "----------Building project:[ MathGrapher - Debug ]----------"
	@"$(MAKE)" -f  "MathGrapher.mk"
clean:
	@echo "----------Cleaning project:[ MathGrapher - Debug ]----------"
	@"$(MAKE)" -f  "MathGrapher.mk" clean
