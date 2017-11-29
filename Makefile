


mpu9250_simple: mpu9250_simple.c
	gcc -O2 -Wall $^ -o $@


.PHONY: clean
clean:
	rm -f mpu9250_simple

