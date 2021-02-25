#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "fp.h"

int computeFP (float val){ //input: float value to be represented
  int e = 0;
  int exp = 0; 
  int bias = 15; //bias = 2^(5-1) - 1
  int result = 0; 
  float frac = 0.0;
  int frac_int = 0;
  
  //if val < 1, multiply so val >= 1
  while (val < 1){
    val = val * 2;
    e--;
  }
  //else if val >= 2, divide so val >= 1
  while (val >= 2){
    val = val / 2;
    e++;
  }
  
  //check for over/underflow
  if (exp > 15){
    return -1;
  }
  else if (exp < 1){
    return 0;
  }
  
  //now that val is 1.xxx solve
  exp = e + bias;
  frac = (val - 1) * pow(2,9);
  frac_int = (int)frac;
  
  //result = 0
  result = result | exp; //result = exp 
  result = result << 9; //result = exp000000000
  result = result | frac_int; //result = exp frac
  
  return result;
}

float getFP (int val){
  int e = 0;
  int exp = 0;
  int bias = 15; //bias = 2^(5-1) - 1
  float mantissa = 0.0;
  
  //extract exponent
  exp = (val >> 9);
  //extract mantissa
  mantissa = ((val & 0x1FF) / (float)pow(2,9)) + 1;
  //extract the exponent
  e = exp - bias; 
  //check for denormalized number
  if (exp < 1){
    return 0;
  }
  //check for a special number
  if (exp > 30){
    return -1;
  }
  //float = M * 2^E
  //if e < 0, divide mantissa
  while (e < 0){
    mantissa = mantissa / 2;
    e++;
  }
  //if e > 0, multiply
  while (e > 0){
    mantissa = mantissa * 2;
    e--;
  }
  
  return mantissa; 
}

int multVals (int source1, int source2){
  int result;
  int e = 0;
  int bias = 15;
  float m, m1, m2;
  int exp = 0, exp1 = 0, exp2 = 0;
  float frac = 0.0, frac1 = 0.0, frac2 = 0.0;
  
  //extract exp1 from source1
  exp1 = source1 >> 9;
  //extract exp2 from source2
  exp2 = source2 >> 9;
  //check for overflow
  if (exp1 > 30 || exp2 > 30){
    return -1;
  }
  else if (exp1 < 1 || exp2 < 1){
    return 0;
  }
  //find e of result by adding e1 + e2
  e = (exp1 - bias) + (exp2 - bias);
  //extract frac1 and m1 from source1
  frac1 = (source1 & 0x1FF) / (float)pow(2, 9);
  m1 = frac1 + 1;
  //extract frac2 and m2 from source2
  frac2 = (source2 & 0x1FF) / (float)pow(2, 9);
  m2 = frac2 + 1;
  //find m of result by multiplying m1 * m2
  m = m1 * m2;
  //if m >= 2 divide by 2 and increment e
  while (m >= 2){
    m = m / 2;
    e++;
  }
  //find exp of result
  exp = e + bias;
  //check for overflow again
  if (exp > 30){
    return -1;
  }
  else if (exp < 1){
    return 0;
  }
  //convert result
  frac = m - 1;
  frac = frac * pow(2,9);
  result = result | (int)frac;
  result = result | (exp << 9);
  
  return result;
}

int addVals (int source1, int source2){
  int bias = 15;
  int e1 = 0, e2 = 0;
  int exp1 = 0, exp2 = 0;
  float man = 0.0, man1 = 0.0, man2 = 0.0;
  int new_exp = 0;
  int new_frac = 0;
  
  //extract exp1 from source1
  exp1 = source1 >> 9;
  //extract exp2 from source2
  exp2 = source2 >> 9;
  //extract e1 from exp1
  e1 = exp1 - bias;
  //extract e2 from exp2
  e2 = exp2 - bias;
  //extract mantissa1 from sourc2
  man1 = ((source1 & 0x1FF) / (float)pow(2,9)) + 1;
  //extract mantissa2 from source2
  man2 = ((source2 & 0x1FF) / (float)pow(2,9)) + 1;
  
  //if e1 > e2, make e1 = e2
  if (e1 > e2){
    int difference = e1 - e2;
    for (int i = 0; i < difference; i++){
      man2 = man2 / 2;
      e2++;
    }
  }
  //if e2 > e1, make e2 = e1
  else if (e2 > e1){
    int difference = e2 - e1;
    for (int i = 0; i < difference; i++){
      man1 = man1 / 2;
      e1++;
    }
  }
  
  //add both mantissa's together
  man = man1 + man2;
  //check if mantissa is 1 <= man < 2 
  while (man >= 2){
    man = man / 2;
    e1++;
  }
  while (man < 1){
    man = man * 2;
    e1++;
  }
  
  //check for overflow
  if ((e1 + 15) > 30){
    return -1;
  }
  
  //create the new exp
  new_exp = (bias + e1) * pow(2,9);
  //convert mantissa to frac by -1
  man = man - 1;
  man = man * pow(2,9);
  //create an int representation of frac
  new_frac = (int)man;
  int answer = new_exp + new_frac;
  
  return answer;
}

