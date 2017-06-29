/*****************************************************************************
 *
 *     Author: Xilinx, Inc.
 *
 *     This text contains proprietary, confidential information of
 *     Xilinx, Inc. , is distributed by under license from Xilinx,
 *     Inc., and may be used, copied and/or disclosed only pursuant to
 *     the terms of a valid license agreement with Xilinx, Inc.
 *
 *     XILINX IS PROVIDING THIS DESIGN, CODE, OR INFORMATION "AS IS"
 *     AS A COURTESY TO YOU, SOLELY FOR USE IN DEVELOPING PROGRAMS AND
 *     SOLUTIONS FOR XILINX DEVICES.  BY PROVIDING THIS DESIGN, CODE,
 *     OR INFORMATION AS ONE POSSIBLE IMPLEMENTATION OF THIS FEATURE,
 *     APPLICATION OR STANDARD, XILINX IS MAKING NO REPRESENTATION
 *     THAT THIS IMPLEMENTATION IS FREE FROM ANY CLAIMS OF INFRINGEMENT,
 *     AND YOU ARE RESPONSIBLE FOR OBTAINING ANY RIGHTS YOU MAY REQUIRE
 *     FOR YOUR IMPLEMENTATION.  XILINX EXPRESSLY DISCLAIMS ANY
 *     WARRANTY WHATSOEVER WITH RESPECT TO THE ADEQUACY OF THE
 *     IMPLEMENTATION, INCLUDING BUT NOT LIMITED TO ANY WARRANTIES OR
 *     REPRESENTATIONS THAT THIS IMPLEMENTATION IS FREE FROM CLAIMS OF
 *     INFRINGEMENT, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *     FOR A PARTICULAR PURPOSE.
 *
 *     Xilinx products are not intended for use in life support appliances,
 *     devices, or systems. Use in such applications is expressly prohibited.
 *
 *     (c) Copyright 2011 Xilinx Inc.
 *     All rights reserved.
 *
 *****************************************************************************/

/*
 * Video Header File
 */

#ifndef ___AP__VIDEO__
#define ___AP__VIDEO__

#include "ap_int.h"
#include "stdio.h"


/* Parametrized RGB structure */
template <int A, int D, int C>
  struct ap_rgb{
    ap_uint<A> B;
    ap_uint<D> G;
    ap_uint<C> R;
  };

/* Parametrized YUV structure */
template <int A, int B, int C>
  struct ap_yuv{
    ap_uint<A> Y;
    ap_int<B> U;
    ap_int<C> V;
  };

/* Line buffer class definition */
template <typename T, int LROW, int LCOL>
  class ap_linebuffer{
 public:
  T M[LROW][LCOL];

  ap_linebuffer(){
#pragma AP ARRAY_PARTITION variable=M dim=1 complete
#pragma AP data_pack variable=M
};
  ~ap_linebuffer(){};
  void shift_up(int col);
  void shift_down(int col);
  void insert_top(T value, int col);
  void insert_bottom(T value, int col);
  void print(int StartCol, int EndCol);
  T getval(int RowIndex,int ColIndex);
  void shift_up2(int col);
    void shift_down2(int col);
    void insert_top2(T value, int col);
    void insert_bottom2(T value, int col);
    void print2(int StartCol, int EndCol);
    T getval2(int RowIndex,int ColIndex);
};

/* Line buffer print function.
 * Prints the values of all rows in the line buffer
 * between StartCol and EndCol
 */
template <typename T, int LROW, int LCOL>
  void ap_linebuffer<T,LROW,LCOL>::print(int StartCol, int EndCol)
{
  int i, j;
  for(i = LROW-1; i > -1; i--){
    printf("Line %d:\t",i);
    for(j=StartCol; j < EndCol; j++){
      printf("%d\t",M[i][j]);
    }
    printf("\n");
  }
  printf("\n");
}

/* Line buffer shift up
 * Assumes new data pixel will be entered at the bottom of the line buffer
 * The bottom is row = 0
 */
template <typename T, int LROW, int LCOL>
  void ap_linebuffer<T,LROW,LCOL>::shift_up(int col)
{
#pragma AP inline 
  int i;
  for(i = LROW-1; i > 0; i--){
#pragma AP unroll
    M[i][col] = M[i-1][col];
  }
}

template <typename T, int LROW, int LCOL>
  void ap_linebuffer<T,LROW,LCOL>::shift_up2(int col)
{
//#pragma AP inline
  int i;
  for(i = LROW-1; i > 0; i--){
//#pragma AP unroll
    M[i][col] = M[i-1][col];
  }
}

/* Line buffer shift down
 * Assumes new data pixel will be entered at the top of the line buffer
 * The bottom is row = LROW - 1
 */
template <typename T, int LROW, int LCOL>
  void ap_linebuffer<T,LROW,LCOL>::shift_down(int col)
{
#pragma AP inline
  int i;
  for(i = 0; i < LROW-1; i++){
#pragma AP unroll
    M[i][col] = M[i+1][col];
  }
}

/* Line buffer insert bottom
 * Inserts a new value in the bottom row of the line buffer at column = col
 * The bottom is row = 0
 */
template <typename T, int LROW, int LCOL>
  void ap_linebuffer<T,LROW,LCOL>::insert_bottom(T value, int col)
{
#pragma AP inline

  M[0][col] = value;
}

template <typename T, int LROW, int LCOL>
  void ap_linebuffer<T,LROW,LCOL>::insert_bottom2(T value, int col)
{
//#pragma AP inline

  M[0][col] = value;
}

/* Line buffer insert top
 * Inserts a new value in the top row of the line buffer at column = col
 * The bottom is row = LROW - 1
 */
template <typename T, int LROW, int LCOL>
  void ap_linebuffer<T,LROW,LCOL>::insert_top(T value, int col)
{
#pragma AP inline

  M[LROW-1][col] = value;
}

/* Line buffer getval
 * Returns the data value in the line buffer at position RowIndex, ColIndex
 */
template <typename T, int LROW, int LCOL>
  T ap_linebuffer<T,LROW,LCOL>::getval(int RowIndex,int ColIndex)
{
#pragma AP inline

  T return_value;
  return_value = M[RowIndex][ColIndex];
  return return_value;
}

template <typename T, int LROW, int LCOL>
  T ap_linebuffer<T,LROW,LCOL>::getval2(int RowIndex,int ColIndex)
{
//#pragma AP inline

  T return_value;
  return_value = M[RowIndex][ColIndex];
  return return_value;
}

/* Memory window class definition */
template <typename T, int LROW, int LCOL>
  class ap_window{
 public:
  T M[LROW][LCOL];

  ap_window(){
#pragma AP ARRAY_PARTITION variable=M dim=0 complete
#pragma AP data_pack variable=M
};
  ~ap_window(){};
  void shift_right();
  void shift_right2();
  void shift_left();
  void shift_up();
  void shift_down();
  void insert(T value, int row,int col);
  void insert2(T value, int row,int col);
  void print();
  T getval(int RowIndex,int ColIndex);
  T getval2(int RowIndex,int ColIndex);
};

/* Window print
 * Prints the entire contents of the memory window
 */
template <typename T, int LROW, int LCOL>
  void ap_window<T,LROW,LCOL>::print()
{
  int i, j;
  printf("Window Size = %d x %d\n",LROW,LCOL);
  printf("Col \t");
  for(j = 0; j < LCOL; j++){
    printf("%d \t",j);
  }
  printf("\n");
  for(i = LROW-1; i > -1; i--){
    printf("Row %d: \t",i);
    for(j=0; j < LCOL; j++){
      printf("%d\t",M[i][j]);
    }
    printf("\n");
  }
  printf("\n");
}

/* Window shift right
 * Moves all the contents of the window horizontally
 * Assumes new values will be placed in column = LCOL-1
 */
template <typename T, int LROW, int LCOL>
  void ap_window<T,LROW,LCOL>::shift_right()
{
#pragma AP inline
  int i, j;
  for(i = 0; i < LROW; i++){
#pragma AP unroll
    for(j=0; j < LCOL-1; j++){
#pragma AP unroll
      M[i][j] = M[i][j+1];
    }
  }
}

template <typename T, int LROW, int LCOL>
  void ap_window<T,LROW,LCOL>::shift_right2()
{
//#pragma AP inline
  int i, j;
  for(i = 0; i < LROW; i++){
//#pragma AP unroll
    for(j=0; j < LCOL-1; j++){
//#pragma AP unroll
      M[i][j] = M[i][j+1];
    }
  }
}

/* Window shift left
 * Moves all the contents of the window horizontally
 * Assumes new values will be placed in column = 0
 */
template <typename T, int LROW, int LCOL>
  void ap_window<T,LROW,LCOL>::shift_left()
{
#pragma AP inline
  int i, j;
  for(i = 0; i < LROW; i++){
#pragma AP unroll
    for(j=LCOL-1; j > 0; j--){
#pragma AP unroll
      M[i][j] = M[i][j-1];
    }
  }
}

/* Window shift up
 * Moves all the contents of the window vertically
 * Assumes new values will be placed in row = 0
 */
template <typename T, int LROW, int LCOL>
  void ap_window<T,LROW,LCOL>::shift_up()
{
#pragma AP inline
  int i, j;
  for(i = LROW-1; i > 0; i--){
#pragma AP unroll
    for(j=0; j < LCOL; j++){
#pragma AP unroll
      M[i][j] = M[i-1][j];
    }
  }
}

/* Window shift down
 * Moves all the contents of the window vertically
 * Assumes new values will be placed in row = LROW - 1
 */
template <typename T, int LROW, int LCOL>
  void ap_window<T,LROW,LCOL>::shift_down()
{
#pragma AP inline
  int i, j;
  for(i = 0; i < LROW-1; i++){
#pragma AP unroll
    for(j=0; j < LCOL; j++){
#pragma AP unroll
      M[i][j] = M[i+1][j];
    }
  }
}

/* Window insert
 * Inserts a new value at any location of the window
 */
template <typename T, int LROW, int LCOL>
  void ap_window<T,LROW,LCOL>::insert(T value, int row, int col)
{
#pragma AP inline
  M[row][col] = value;
}


template <typename T, int LROW, int LCOL>
  void ap_window<T,LROW,LCOL>::insert2(T value, int row, int col)
{
//#pragma AP inline
  M[row][col] = value;
}
/* Window getval
 * Returns the value of any window location
 */
template <typename T, int LROW, int LCOL>
  T ap_window<T,LROW,LCOL>::getval(int RowIndex, int ColIndex)
{
#pragma AP inline
  T return_value;
  return_value = M[RowIndex][ColIndex];
  return return_value;
}

template <typename T, int LROW, int LCOL>
  T ap_window<T,LROW,LCOL>::getval2(int RowIndex, int ColIndex)
{
//#pragma AP inline
  T return_value;
  return_value = M[RowIndex][ColIndex];
  return return_value;
}


#endif
