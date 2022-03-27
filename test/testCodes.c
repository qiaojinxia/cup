int testCode(){
    assert(-35,({ 10 - 3  * 4  + 5 * 4 / 3  + 3 - 4  * 2 * 6 +2 * 7 - 7 / 4 *3 - 2 - 1 *2 -2+1+2/3;}));
    assert(1,({ 10 - 3  * 4  + 5 - 4 / 3  + 3 - 4 ;}));
    assert(2,({ 10 + 3  - 4  + 5 - 4 / 3  + 3 % 2 - 4 * 3 ;}));
    assert(11,({2 + 9;}));
    assert(2,({4 / 2;}));
    assert(0,({4 < 2;}));
    assert(1,({4 >= 2;}));
    assert(1,({7 % 2;}));
    assert(7,({i = 7;i;}));
    assert(7,({int i = 7;if(i == 7){i;}else{0;}}));
    assert(-8,({ -+10 + 2;}));
    assert(12,({ --10 + 2;})); //support annotation
    assert(3,({ int x = 3;*&x;}));

    return 0;
}