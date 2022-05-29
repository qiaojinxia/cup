//
// Created by qiaojinxia on 2022/5/29.
//
typedef int (* PrintLove)();
extern int put(int);
int printLove(){
    for (float y = 1.5; y > -1.5; y = y - 0.1) {
        for (float x = -1.5; x < 1.5; x = x + 0.05) {
            float a = x * x + y * y - 1;
            put(a * a * a - x * x * y * y * y <= 0.0 ? '*' : ' ');
        }
        put('\n');
    }
}
int main() {
    PrintLove lv = 0;
    lv = printLove;
    lv();
}