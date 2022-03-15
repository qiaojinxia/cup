# boddy
a toy c compailer
- 2022-03-08
  - Compailer code `3 + 8 - 21 * 42 / 7`
- 2022-03-11
  - Code Generate Support Sample Arithmetic expression to asm
    to run with instruct `make program code="1 + 9 * 3 /2 "`
- 2022-03-12
  - Support assign express  ` make program code="ab = 2;c = ab + 3; d = c + 2 / 2; d * 3 /2;"`
- 2022-03-13
    - Support mod express  ` make program code="ab = 7 % 4;"`
- 2022-03-14 
    - Support Equal,NotEqual,Greater,GreaterEqual,Lesser,LesserEqual express  
      - ` make program code="a = 7 ; a > 1; a < 1;"`
      - ` make program code="a = 6 ; a < 7; a > 0 ;"`
      - ` make program code="a = 5 ; a == 5; a != 0;"`
      - ` make program code="a = 5 ; a != 5 ; a == 1;"`
      - ` make program code="a = 5 ; a >= 1;a <= 0;"`
      - ` make program code="a = 5 ; a <= 6; a >= 1;"`
- 2022-03-15
  - Support if Statements blockStatement `if(){}els{}` while statement `while(){}`
    - `make program code="a = 5; if (a < 7){b =3;b + 1;b * 2;}else {b  = 5;}"`
    - `make program code="a = 5;b = 0; while (a < 700){a = a + 1;b = b + 1;}b;"`