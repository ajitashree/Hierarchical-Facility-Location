
/* 
Test Case generation:
1st multiplier : connection Cost
2nd multiplier : service Cost
*/ 

#include <iostream>
#include <sstream>
#include <cstdio>
#include <cmath>
#include <string>
#include <cstring>
#include <ctime>

using namespace std;

int main(int argc,char ** argv){


  long N, M, L;
  N = atoi(argv[1]);
  M = atoi(argv[2]);
  L = atoi(argv[3]);

  string fld = argv[4];
  //cin >> N >> M >> L;
  long **cor = new long*[M];
  double **connection_matrix = new double*[N];
  long *facility_cost = new long[M];
  double *service_cost = new double[L];
  long *client_preference = new long[N];

  for(long i = 0; i < M; i++)
  {
    cor[i] = new long[2];
    cor[i][0] = rand()%1000;
    cor[i][1] = rand()%1000;
    facility_cost[i]=rand()%1000;
  }

  for(long i = 0; i < L; i++)
  {
    service_cost[i] = rand()%1000;
  }

  for(long i = 0; i < N; i++)
  {
    connection_matrix[i] = new double[M];
    client_preference[i] = rand()%L;

    long x = rand()%1000;
    long y = rand()%1000;

    for(long j=0;j<M;j++)
    {
      connection_matrix[i][j]=sqrt((cor[j][0]-x)*(cor[j][0]-x)+(cor[j][1]-y)*(cor[j][1]-y));
    }
      
  }
  
  string alphabets = "abcdef";
  double multiplier[6] = {0.01, 0.1, 1.0, 10.0, 100.0, 1000.0};
  string s;
  char tab2[1024];

  for (long i1 = 0; i1 < 6; i1++)
  {
    for (long j1 = 0; j1 < 6; j1++)
    {
      s = fld + to_string(N) +"_"+ to_string(M) +"_"+ to_string(L) +"_"+ alphabets[i1] + alphabets[j1] + ".txt";
      strcpy(tab2, s.c_str());
      freopen (tab2,"w",stdout);

      cout << N << " " << M << " " << L << endl;

      for(long i = 0; i < M; i++)
        cout << facility_cost[i] << " ";
      cout << endl;

      for(long i = 0; i < L; i++)
        cout << service_cost[i] * multiplier[i1] << " ";
      cout << endl;

      for(long i = 0; i < N; i++)
        cout << client_preference[i] << " ";
      cout << endl;

      for(long i = 0; i < N; i++, cout << endl)
      {
        for(long j = 0; j < M; j++)
        {
          cout << connection_matrix[i][j] * multiplier[j1]<<" ";
        }
      }
    }
  }
  fclose(stdout); 
}
