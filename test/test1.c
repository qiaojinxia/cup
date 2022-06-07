
 //Created by qiaojinxia on 2022/5/11.


//int testFloatCmp(){
//    float a = 1.0;
//    int b = 2;
//    _Bool x = a <= b;
//    assert(1,(int)x);
//}
//
//
//int testFuncCall(){
//    int* m = malloc (sizeof (int) * 100);
//    char j = 0;
//    for(int i =0;i<100;i++){
//        m[j] = 77;
//        printf("memory:%p value:%d\n",&m[j],m[j]);
//        j ++;
//    }
//    assert(77,m[99]);
//    free(m);
//}
//
//int testString(){
//    char m[30] = "hello,world!";
//    print_s(&m);
//}
//
//int testTypeDef(){
//    float caomaofloat;
//    int x = 3.0;
//    int y = 4.0;
//    int z = x + y;
//    assert(7,z);
//    typedef struct User
//    {
//        char *  name;
//        int  age;
//        int  sex;
//        int height;
//        int width;
//    } user ;
//
//    user a = {
//            "caomaoboy",
//            10,
//            1,
//            190,
//            140
//    };
//    assert(140,({a.width;}));
//    print_s(a.name);
//}
//
//
//
//int testStruct(){
//    struct Books
//    {
//        unsigned long  name;
//        int  price;
//        char  index;
//        int   book_id;
//        int * a[4];
//    } book = {7, 2, 97, 32,{0x0001,0b111,3434,123}};
//    int  a[3] = {1,2,3};
//    assert(3,a[2]);
//
//    struct Books a;
//    a = book;
//    a.price = 22;
//    struct Books *bk = &a;
//    assert(2,({book.price;}));
//    float total = (float)bk->price + (float)book.price;
//    assert_char(97,a.index);
//    assert_f(24.0,total);
//    char * x = "hello,world!";
//    print_s(x);
//    memset(book,0x1,sizeof book);
//    assert_char(1,book.index);
//}
//

int main(){
    testFuncCall();
}