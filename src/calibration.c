/*
* @Author: AnthonyKenny98
* @Date:   2020-02-29 13:30:51
* @Last Modified by:   AnthonyKenny98
* @Last Modified time: 2020-02-29 13:45:20
*/

#include <time.h>
#include <stdio.h>

void func1() {
    int i = 0; while (i<10000000) i++;
}
void func2() {
    int i = 0; while (i<25000000) i++;
}
void func3() {
    int i = 0; while (i<50000000) i++;
}
void func4() {
    int i = 0; while (i<100000000) i++;
}
void func5() {
    int i = 0; while (i<250000000) i++;
}

int main(int argc, char *argv[]) {
    
    clock_t start, finish;
    float _1, _2, _3, _4, _5;

    start = clock();
    func1();
    finish = clock();
    _1 = (float) (finish - start) / CLOCKS_PER_SEC;

    start = clock();
    func2();
    finish = clock();
    _2 = (float) (finish - start) / CLOCKS_PER_SEC;

    start = clock();
    func3();
    finish = clock();
    _3 = (float) (finish - start) / CLOCKS_PER_SEC;

    start = clock();
    func4();
    finish = clock();
    _4 = (float) (finish - start) / CLOCKS_PER_SEC;

    start = clock();
    func5();
    finish = clock();
    _5 = (float) (finish - start) / CLOCKS_PER_SEC;


    printf("func1 = %f\nfunc2 = %f\nfunc3 = %f\nfunc4 = %f\nfunc5 = %f\n", _1, _2, _3, _4, _5);
}