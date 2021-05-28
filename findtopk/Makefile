compile: compile1 compile2 compile3

compile1: create-directory
	gcc -Wall -o temp/findtopk.out findtopk.c

compile2: create-directory
	gcc -Wall -o temp/findtopk_m.out findtopk_mqueue.c -lrt

compile3: create-directory
	gcc -Wall -o temp/findtopk_t.out findtopk_thread.c -lpthread

create-directory:
	if [ ! -d ./temp/ ]; then \
		mkdir temp; \
	fi
	if [ ! -d ./output_folder/ ]; then \
		mkdir output_folder; \
	fi

test: test1 test2 test3

test1: compile1
	temp/findtopk.out 1 2 input_folder/input1.txt input_folder/input2.txt output_folder/out1.txt

test2: compile2
	temp/findtopk_m.out 1 2 input_folder/input1.txt input_folder/input2.txt output_folder/out2.txt

test3: compile3
	temp/findtopk_t.out 1 2 input_folder/input1.txt input_folder/input2.txt output_folder/out3.txt

measure: measure1 measure2 measure3

measure1: compile1
	time temp/findtopk.out 1000 3 input_folder/rand1.txt input_folder/rand2.txt input_folder/rand3.txt output_folder/out1.txt

measure2: compile2
	time temp/findtopk_m.out 1000 3 input_folder/rand1.txt input_folder/rand2.txt input_folder/rand3.txt output_folder/out2.txt

measure3: compile3
	time temp/findtopk_t.out 1000 3 input_folder/rand1.txt input_folder/rand2.txt input_folder/rand3.txt output_folder/out3.txt

rebuild: clean compile

clean:
	rm -rf temp/
