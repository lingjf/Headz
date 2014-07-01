#ifndef __RANKZ_H_2010_0815__
#define __RANKZ_H_2010_0815__

#ifdef __cplusplus
extern "C" {
#endif

int Pnm(int n, int m);
int Ann(int n);
int Cnm(int n, int m);

int seq_2_idx(int seq[], int n);

int idx_2_seq(int seq[], int n, int idx);

void seq_gen(int seq[], int n, int x, void(*cb)(int*, int, int));

#ifdef __cplusplus
}
#endif

#endif

