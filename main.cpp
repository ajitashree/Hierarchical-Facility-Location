#include "HierarchicalFL.h"
using namespace std;

int main(int argc, char const *argv[])
{
  double value;
  long var, N, M, L;
  cin >> N >> M >> L;

  double *facilityCost = new double[M];
  double *serviceCost = new double[L];
  long *clientPref = new long[N];
  double **connectionCost = new double*[N];

  for (long m = 0; m < M; m++)
  {
    cin >> value;
    facilityCost[m] = value;
  }
  
  for (long l = 0; l < L; l++)
  {
    cin >> value;
    serviceCost[l] = value;
  }

  for (long n = 0; n < N; n++)
  {
    cin >> var;
    clientPref[n] = var;
  }
  for (long n = 0; n < N; n++)
  {
    connectionCost[n] = new double[M];
    for (long m = 0; m < M; m++)
    {
      cin >> value;
      connectionCost[n][m] = value;
    }
  }
  HierarchicalFL *hFl = new HierarchicalFL(M, N, L);
  hFl->setclientPref(clientPref);
  hFl->setserviceCost(serviceCost);
  hFl->setfacilityCost(facilityCost);
  hFl->setconnectionCost(connectionCost);
  
  hFl->initFeasibleSol();
  hFl->localSearch();
  hFl->printDetails();
  return 0;
}