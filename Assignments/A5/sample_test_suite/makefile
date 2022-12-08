all:
	gcc -g -O0 ./code/*.c

1:
	./a.out -s 10 -f < ./in/test1.in > ./student_out/test1-a.out && diff ./student_out/test1-a.out ./out/test1.out
2:
	./a.out -s 10 -f < ./in/test2.in > ./student_out/test2-a.out && diff ./student_out/test2-a.out ./out/test2.out
3:
	./a.out -s 10 -f < ./in/test3.in > ./student_out/test3-a.out && diff ./student_out/test3-a.out ./out/test3.out
4:
	./a.out -s 10 -f < ./in/test4.in > ./student_out/test4-a.out && diff ./student_out/test4-a.out ./out/test4.out
5:
	./a.out -s 10 -f < ./in/test5.in > ./student_out/test5-a.out && diff ./student_out/test5-a.out ./out/test5.out
6:
	./a.out -s 10 -f < ./in/test6.in > ./student_out/test6-a.out && diff ./student_out/test6-a.out ./out/test6.out
7:
	./a.out -s 5 -b < ./in/test7.in > ./student_out/test7-a.out && diff ./student_out/test7-a.out ./out/test7.out
8:
	./a.out -s 5 -w < ./in/test8.in > ./student_out/test8-a.out && diff ./student_out/test8-a.out ./out/test8.out
9:
	./a.out -s 5 -f < ./in/test9.in > ./student_out/test9-a.out && diff ./student_out/test9-a.out ./out/test9.out
10:
	./a.out -s 103 -f	 < ./in/test10.in > ./student_out/test10.out && diff ./student_out/test10.out ./out/test10.out

clean:
	rm -rf ./a.out 
	rm -rf student_out/*

cp:

