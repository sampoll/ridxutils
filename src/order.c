#include <stdio.h>
#include <stdlib.h>

void gen_all_subscripts_rmajor(int *D, int nd, int ***S, int ns); 
void gen_all_subscripts_cmajor(int *D, int nd, int ***S, int ns); 
void subscripts_for_offset_rmajor(int *D, int nd, int idx, int **sbs);
void subscripts_for_offset_cmajor(int *D, int nd, int idx, int **sbs);
int offset_for_subscripts_rmajor(int *D, int nd, int *sbs);
int offset_for_subscripts_cmajor(int *D, int nd, int *sbs);

void subscripts_for_offset_rmajor2(int *D, int *Dcpr, int nd, int idx, int **sbs);
void subscripts_for_offset_cmajor2(int *D, int *Dcpl, int nd, int idx, int **sbs);
int offset_for_subscripts_rmajor2(int *D, int *Dcpr, int nd, int *sbs);
int offset_for_subscripts_cmajor2(int *D, int *Dcpl, int nd, int *sbs);

int main(int argc, char **argv)  {

  int nd = 4;
  int *D = (int *)malloc(nd*sizeof(int));
  D[0] = 3;
  D[1] = 5;
  D[2] = 2;
  D[3] = 4;

  int *Dcpr = (int *)malloc(nd*sizeof(int));
  Dcpr[nd-1] = 1;
  for(int i=nd-2;i>=0;i--)
    Dcpr[i] = Dcpr[i+1]*D[i+1];

  int *Dcpl = (int *)malloc(nd*sizeof(int));
  Dcpl[0] = 1;
  for(int i=1;i<nd;i++)
    Dcpl[i] = Dcpl[i-1]*D[i-1];

  printf("D    = [");
  for(int i=0;i<nd;i++)  
    printf("%4d ", D[i]);
  printf("]\nDcpr = [");
  for(int i=0;i<nd;i++)  
    printf("%4d ", Dcpr[i]);
  printf("]\nDcpl = [");
  for(int i=0;i<nd;i++)  
    printf("%4d ", Dcpl[i]);
  printf("]\n");
  
  // number of linear indices is product of all elements of D
  int nidx = 1;
  for(int i=0;i<nd;i++)
    nidx *= D[i];

  // S[i][j] is the jth subscript in the ith linear index
  int **S = (int **)malloc(nidx*sizeof(int *));  
  for(int i=0;i<nidx;i++)
    S[i] = (int *)malloc(nd*sizeof(int));

  int *sbs = (int *)malloc(nd*sizeof(int));

  // generate and print all subscript sets
  // note: A must be zeroed out before calling

  printf("Row-major order:\n");
  gen_all_subscripts_rmajor(D, nd, &S, nidx);
  for(int i=0;i<nidx;i++)  {

    // check subscript computation
    subscripts_for_offset_rmajor2(D, Dcpr, nd, i, &sbs);
    int ok = 1;
    for(int j=0;j<nd;j++)  {
      if (sbs[j] != S[i][j])
        ok = 0;
    }
  
    // check index computation
    int jdx = offset_for_subscripts_rmajor2(D, Dcpr, nd, sbs);
    if (jdx != i)
      ok = 0;

    if (!ok)  {
      printf("%4d:  [", i);
      for(int j=0;j<nd;j++)  {   
        printf("%4d ", S[i][j]);
      }
      printf("]  [");
      for(int j=0;j<nd;j++)  {   
        printf("%4d ", sbs[j]);
      }
      printf("]  %4d\n", jdx);
    }
  }

  printf("\nColumn-major order:\n");
  gen_all_subscripts_cmajor(D, nd, &S, nidx);
  for(int i=0;i<nidx;i++)  {
    int ok = 1;
    subscripts_for_offset_cmajor2(D, Dcpl, nd, i, &sbs);
    for(int j=0;j<nd;j++)  {
      if (sbs[j] != S[i][j])
        ok = 0;
    }

    // check index computation
    int jdx = offset_for_subscripts_cmajor2(D, Dcpl, nd, sbs);
    if (jdx != i)
      ok = 0;

    if (!ok)  {
      printf("%4d:  [", i);
      for(int j=0;j<nd;j++)  {   
        printf("%4d ", S[i][j]);
      }
      printf("]  [");
      for(int j=0;j<nd;j++)  {   
        printf("%4d ", sbs[j]);
      }
      printf("] %4d\n", jdx);
    }
  }

  // clean up
  for(int i=0;i<nidx;i++)
    free(S[i]);
  free(S);
  free(sbs);

  return 0;
}


/* For testing only: generate all subscript combinations
   in last-fastest order for a given dimensionality vector D 

   D  : dimensionality array (length nd)
   S  : pointer to 2D result array 
       (in scope of calling routine)
   ns : total number of indices to compute === prod(D)

*/
void gen_all_subscripts_rmajor(int *D, int nd, int ***S, int ns)  { 

  // convenience: reduce asterisk profusion
  int **sp = *S;
  int *A = (int *)malloc(nd*sizeof(int));
  for(int i=0;i<nd;i++)
    A[i] = 0;   

  for(int ms=0;ms<ns;ms++)  {
    
    for(int i=0;i<nd;i++)  // copy A into S
      sp[ms][i] = A[i];

    // advance A to next subscript set
    int mm = nd-1;
    while (A[mm] == D[mm]-1 && mm >= 0)  {
      A[mm] = 0;
      mm--;
    }

    if (mm >= 0)   // mm is -1 on last iteration
      A[mm] = A[mm] + 1;
  }
  free(A);
}

void gen_all_subscripts_cmajor(int *D, int nd, int ***S, int ns)  { 
  int **sp = *S;
  int *DR = (int *)malloc(nd*sizeof(int));   // reverse dimensionality vector
  for(int i=0;i<nd;i++)
    DR[i] = D[nd-1-i];
  gen_all_subscripts_rmajor(DR, nd, S, ns);
  for(int i=0;i<ns;i++)  {    // reverse subscripts
    for(int j=0;j<nd/2;j++)  {
      int tmp = sp[i][j];
      sp[i][j] = sp[i][nd-1-j];
      sp[i][nd-1-j] = tmp;
    }
  }
}


void subscripts_for_offset_rmajor(int *D, int nd, int idx, int **subs)  {
  for(int j=0;j<nd;j++)  {
    int s;
    if (j == 0)  {
      s = idx;
      for(int i=1;i<nd;i++)  
        s = s / D[i];
    }
    else if (j == nd-1)  
      s = idx % D[nd-1]; 
    else  {
      s = idx;
      for(int i=j+1;i<nd;i++)
        s = s / D[i];
      s = s % D[j];
    }
    (*subs)[j] = s;
  }
}

// Performance enhancement: save integer divisions
// Precompute: Dcpr[k] := product of D[k+1]*D[k+2]...*D[nd-1]
// (cumulative product to the right)
void subscripts_for_offset_rmajor2(int *D, int *Dcpr, int nd, int idx, int **subs)  {
  for(int j=0;j<nd;j++)  {
    int s;
    if (j == 0)  
      s = idx / Dcpr[0];
    else if (j == nd-1)  
      s = idx % D[nd-1]; 
    else  
      s = (idx / Dcpr[j]) % D[j];
    (*subs)[j] = s;
  }
}


void subscripts_for_offset_cmajor(int *D, int nd, int idx, int **subs)  {
  for(int j=0;j<nd;j++)  {
    int s;
    if (j == 0)
      s = idx % D[0];
    else if (j == nd-1)  {
      s = idx;
      for(int i=0;i<nd-1;i++)
        s = s / D[i];
    }
    else  {
      s = idx;
      for(int i=0;i<j;i++)
        s = s / D[i];
      s = s % D[j];
    }
    (*subs)[j] = s;
  }
}

// Performance enhancement: save integer divisions
// Precompute: Dcpl[k] := product of D[0]*D[1]...*D[k-1]
// (cumulative product to the left)
void subscripts_for_offset_cmajor2(int *D, int *Dcpl, int nd, int idx, int **subs)  {
  for(int j=0;j<nd;j++)  {
    int s;
    if (j == 0)
      s = idx % D[0];
    else if (j == nd-1)  
      s = idx / Dcpl[nd-1];
    else  
      s = (idx / Dcpl[j]) % D[j];
    (*subs)[j] = s;
  }
}


int offset_for_subscripts_rmajor(int *D, int nd, int *sbs)  {
  int idx = 0;
  for(int j=0;j<nd;j++)  {
    int p = 1;
    for(int i=j+1;i<nd;i++)
      p = p * D[i];
    idx = idx + p*sbs[j];
  }
  return idx;
}

int offset_for_subscripts_rmajor2(int *D, int *Dcpr, int nd, int *sbs)  {
  int idx = 0;
  for(int j=0;j<nd;j++)  
    idx = idx + Dcpr[j]*sbs[j];
  return idx;
}



int offset_for_subscripts_cmajor2(int *D, int *Dcpl, int nd, int *sbs)  {
  int idx = 0;
  for(int j=0;j<nd;j++)  
    idx = idx + Dcpl[j]*sbs[j];
  return idx;
}







