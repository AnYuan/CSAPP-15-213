/* 
 * CS:APP Data Lab 
 * 
 * <Please put your name and userid here>
 * 
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.  
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.

 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting an integer by more
     than the word size.

EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implent floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operators (! ~ & ^ | + << >>)
     that you are allowed to use for your implementation of the function. 
     The max operator count is checked by dlc. Note that '=' is not 
     counted; you may use as many of these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce 
 *      the correct answers.
 */


#endif
/* 
 * bitAnd - x&y using only ~ and | 
 *   Example: bitAnd(6, 5) = 4
 *   Legal ops: ~ |
 *   Max ops: 8
 *   Rating: 1
 */
int bitAnd(int x, int y) {
  return ~((~x) | (~y));
}
/* 
 * getByte - Extract byte n from word x
 *   Bytes numbered from 0 (LSB) to 3 (MSB)
 *   Examples: getByte(0x12345678,1) = 0x56
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 2
 */
int getByte(int x, int n) {
  int mask = 0xff;
  return (x >> (n << 3)) & mask;
}
/* 
 * logicalShift - shift x to the right by n, using a logical shift
 *   Can assume that 0 <= n <= 31
 *   Examples: logicalShift(0x87654321,4) = 0x08765432
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 3 
 */
int logicalShift(int x, int n) {
  int a = 1 << 31;
  int mask = ~((a >> n) << 1);
  int result = (x >> n) & mask;
  return result;
}
/*
 * bitCount - returns count of number of 1's in word
 *   Examples: bitCount(5) = 2, bitCount(7) = 3
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 40
 *   Rating: 4
 */
int bitCount(int x) {
  /*can not solve it by myself...*/
  int temp_mask1 = 0x55 | (0x55 << 8); // 00000000 00000000 01010101 01010101
  int temp_mask2 = 0x33 | (0x33 << 8); // 00000000 00000000 00110011 00110011
  int temp_mask3 = 0x0f | (0x0f << 8); // 00000000 00000000 00001111 00001111
                             //support x is       11111111 11111111 11111111 11111111
  int mask1 = temp_mask1 | (temp_mask1 << 16); // 01010101 01010101 01010101 01010101 //1
  int mask2 = temp_mask2 | (temp_mask2 << 16); // 00110011 00110011 00110011 00110011 //2
  int mask3 = temp_mask3 | (temp_mask3 << 16); // 00001111 00001111 00001111 00001111 //4
  int mask4 = 0xff | (0xff << 16);             // 00000000 11111111 00000000 11111111 //8
  int mask5 = 0xff | (0xff << 8);              // 00000000 00000000 11111111 11111111 //16
  x = (x & mask1) + ((x >> 1) & mask1);        // 10101010 10101010 10101010 10101010
  x = (x & mask2) + ((x >> 2) & mask2);        // 00100010 00100010 00100010 00100010 + 00010001 00010001 00010001 00010001
  x = (x & mask3) + ((x >> 4) & mask3);        
  x = (x & mask4) + ((x >> 8) & mask4);
  x = (x & mask5) + ((x >> 16) & mask5);
  return x;
}
/* 
 * bang - Compute !x without using !
 *   Examples: bang(3) = 0, bang(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4 
 */
int bang(int x) {
  // if x is 0, t1 is overflow
  int t1 = ~x + 1;
  // if x is 0, t2 is 0, else its top most bit is 1
  int t2 = x | t1;
  // if x = 0, t3 = 0xffffffff, else t3 = 0x0
  int t3 = ~t2;
  // right shift 31
  // x = 0, mask = 0xffffffff, else mask = 0
  int mask = t3 >> 31;
  return mask & 1;
}
/* 
 * tmin - return minimum two's complement integer 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Rating: 1
 */
int tmin(void) {
  return 0x1 << 31; // 10000000
}
/* 
 * fitsBits - return 1 if x can be represented as an 
 *  n-bit, two's complement integer.
 *   1 <= n <= 32
 *   Examples: fitsBits(5,3) = 0, fitsBits(-4,3) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 2
 */
int fitsBits(int x, int n) {
  int neg_n = ~x + 1;
  int shift = 32 + neg_n;
  int t1 = x << shift; // set left most shift bits 1 to zero
  int t2 = t1 >> shift;
  int t3 = t2 ^ x;
  return !t3;
}
/* 
 * divpwr2 - Compute x/(2^n), for 0 <= n <= 30
 *  Round toward zero
 *   Examples: divpwr2(15,1) = 7, divpwr2(-33,4) = -2
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 2
 */
int divpwr2(int x, int n) {
  // what??????
    int bias=(x>>31)&((0x1<<n)+~0);
    return (x+bias)>>n;
}
/* 
 * negate - return -x 
 *   Example: negate(1) = -1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int negate(int x) {
  return ~x+1;
}
/* 
 * isPositive - return 1 if x > 0, return 0 otherwise 
 *   Example: isPositive(-1) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 8
 *   Rating: 3
 */
int isPositive(int x) {
  int mask = 0x1;
  int tmp = x >> 31;
  int positive_or_not = !(tmp & mask);
  int zero_or_not = !(x ^ 0x0);
  int result = positive_or_not & (!zero_or_not);
  return result;
}
/* 
 * isLessOrEqual - if x <= y  then return 1, else return 0 
 *   Example: isLessOrEqual(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isLessOrEqual(int x, int y) {
  /*
  1. equal
  2. different symbol
  3. x - y
  */
  int mask = 0x1;
  // determin if equal
  int isEqual = ! (x ^ y);
  int x_symbol = (x >> 31) & mask;
  int y_symbol = (y >> 31) & mask;
  int diff_symbol = x_symbol ^ y_symbol;
  // if so, when x is negative, x is less than y
  int x_l_y = diff_symbol & x_symbol;
  // x - y
  int neg_y = ~y+1;
  int temp = x + neg_y;
  int temp_symbol = (temp >> 31) & mask;
  int result = isEqual | x_l_y | ((!diff_symbol) & temp_symbol);
  return result;
}
/*
 * ilog2 - return floor(log base 2 of x), where x > 0
 *   Example: ilog2(16) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 90
 *   Rating: 4
 */
int ilog2(int x) {
  //............please kill me.....
  int ans=0;
  ans=(!!(x>>(16)))<<4;
  ans=ans+((!!(x>>(8+ans)))<<3);
  ans=ans+((!!(x>>(4+ans)))<<2);
  ans=ans+((!!(x>>(2+ans)))<<1);
  ans=ans+((!!(x>>(1+ans)))<<0);
  return ans;
}
/* 
 * float_neg - Return bit-level equivalent of expression -f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representations of
 *   single-precision floating point values.
 *   When argument is NaN, return argument.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 10
 *   Rating: 2
 */
unsigned float_neg(unsigned uf) {
  /* Other than NaN, top most bit of argument should be inversed
   * NaN reamins unchanged as a return value
   */
  /* Get the part exponent */
  unsigned exponent = (uf << 1) >> 24;
  /* Get the part fraction */
  unsigned fraction = uf << 9;
  unsigned is_exponent_full_1 = ! (exponent ^ 0xff);
  /* Determine if this value a NaN */
  unsigned is_NaN_or_not = is_exponent_full_1 && fraction;
  /* If uf a NaN, keep it unchanged, else inverse the top most bit */
  unsigned result = uf + ((! is_NaN_or_not) << 31);
  return result;
}
/* 
 * float_i2f - Return bit-level equivalent of expression (float) x
 *   Result is returned as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point values.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned float_i2f(int x) {
  /* First determine the exponent bias based on the value x
   * Then keep the symbol of x, get the alsolute value of x
   * Then determine the index of left most 1 of the abs x,
   * that index plus bias is the exponent of the single float
   * Left shifted that clears all left zero bits of abs x
   * right shifted and masked that leave 9 slots for symbol and exponent,
   * see that if the lost part equals or greater than 128,
   * two situations: greater; equal and the last bit of shifted vaule is 1,
   * both that the shifted vaule should plus 1
   * Determine if there's a bit 1 in index 23,
   * if so, fraction is 0 and exponent increases by 1
   */
  /* Determine the bias */
  unsigned exponent_bias = 0;
  if(x)
    {
      exponent_bias = 0x7f;
    }
  /* Get the symbol and abs of x */
  unsigned symbol = x & 0x80000000;
  unsigned x_absoulte = x;
  if(symbol)
    {
      x_absoulte = ~ x + 1;
    }
  /* A while loop to right shift abs x by 1 until the value is 0,
   * record the shifted setps, which is index of top most bit 1
   */
  int index = 0;
  unsigned temp = x_absoulte;
  while(temp)
    {
      temp = temp >> 1;
      index = index + (temp && 0x1);
    }
  /* Calculate exponent and left shift setps that could clear left 0 bits */
  unsigned exponent = index + exponent_bias;
  int left_shift = 31 + (~ index + 1);
  unsigned fraction = x_absoulte << left_shift;
  /* Keep the lost part */
  unsigned tail = fraction & 0xff;
  /* Leave 9 slots for symbol and exponent */
  fraction = (fraction >> 8) & 0x7fffff;
  /* Determine if it's necessary to increase fraction by 1
   * If greater than 0.5, increment;
   * equals to 0.5 and the last bit is 1, increment, round up to even
   */
  int tail_7th_bit = tail & 0x80;
  int tail_left_bits = tail & 0x7f;
  int tail_l_128 = tail_7th_bit && tail_left_bits;
  fraction = fraction + (tail_l_128 || (tail_7th_bit && (fraction & 1)));
  /* Determine if fraction has a carry after a possible increment
   * if so, fraction is 0 and exponent increases by 1
   */
  if(fraction >> 23)
    {
      fraction = 0;
      exponent = exponent + 1;
    }
  unsigned result = symbol | (exponent << 23) | fraction;
  return result;
}
/* 
 * float_twice - Return bit-level equivalent of expression 2*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned float_twice(unsigned uf) {
  /* Four special cases: NaN, infinite, zero, non-normalized,
   * return it self if uf meets first three cases,
   * for non-normalized, be careful that
   * it may become a normalized single float when doubled
   */
  /* Get the part exponent */
  unsigned exponent = (uf << 1) >> 24;
  /* Get the part fraction */
  unsigned fraction = uf & 0x7fffff;
  unsigned exponent_full_1 = ! (exponent ^ 0xff);
  /* Determine type of this value */
  unsigned NaN_or_not = exponent_full_1 && fraction;
  unsigned infinite_or_not = exponent_full_1 && (! fraction);
  unsigned zero_or_not = ! (uf & 0x7fffffff);
  unsigned not_normalized_or_not = (! exponent) && fraction;
  /* If meeting the first three cases, return it self */
  if(NaN_or_not || infinite_or_not || zero_or_not)
    {
      return uf;
    }
  /* If uf a non-normalized single float, check the fraction part */
  if(not_normalized_or_not)
    {
      unsigned temp_fraction = fraction << 1;
      fraction = temp_fraction & 0x7fffff;
      /* That if there's a carry of 1 */
      if(temp_fraction & 0x800000)
	{
	  /* The value of non-normalized is (-1)^(s) * 0.xxx * 2^(-126),
	   * the exponent is always -126 implicitly;
	   * Now make the exponent 0x1, make it -126 explicitly
	   */
	  return (uf & 0x80000000) | 0x00800000 | fraction;
	}
      /* If no carry, just change the fraction */
      return (uf & 0xff800000) | fraction;
    }
  /* Notice that if the original exponent is 0xfe
   * double f would make exponent 0xff
   * with such a exponent and a non-zero fraction
   * the return value is a NaN
   * it's out of the single float representation range
   */
  return uf + 0x00800000;
}

