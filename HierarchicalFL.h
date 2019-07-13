/*
Hierarchical Facility Location Instance is handled using HierarchicalFL class 

Description:
M = No of Facilities
N = No of Clients
L = No of Services

facilityCost = Facility cost of M Facilities
serviceCost = Service cost of L services
facilityAssign = Assignment of N Clients
isfacOpen = Boolean array to denote if facility is open or not

servicePref = L * N matrix denoting clients preferring every service
isserviceOpen = M * L matrix denoting type of serives open on all facilities
connectionCost = N * M matrix denoting the distance between clients and facilities
clients_of_service = maintains similar information as servicePref

*/

#ifndef HIERARCHICALFL_H
#define HIERARCHICALFL_H

#include "FacilityLocation.h"
#include <iostream>
#include <vector>
#include <list>
#include <map>
using namespace std;

class HierarchicalFL
{
  long M, N, L;      
  double cost, epsilon;

  double *facilityCost;
  double *serviceCost;
  long *facilityAssign;
  bool *isfacOpen;
  
  long *servicePref;   
  bool **isserviceOpen;  
  double **connectionCost; 
  vector < list <long> > clients_of_service;

public:

  void updateAssignment(bool**, long *);
  void initAssignment(bool**, long *);
  double computeCost(bool**, long *);
  void initFeasibleSol();
  void printDetails();

  bool addMove(long);
  bool deleteMove(long);
  bool swapMove(long, long);
  void localSearch();

  void setclientPref(long*);
  void setserviceCost(double*);
  void setfacilityCost(double*);
  void setconnectionCost(double**);

  HierarchicalFL(long, long, long);
  ~HierarchicalFL();
  
};

// Constructor of HierarchicalFL class
HierarchicalFL::HierarchicalFL(long _M, long _N, long _L)
{
  M = _M;
  N = _N;
  L = _L;

  epsilon = 0.99;
  isfacOpen = new bool[M];
  facilityCost = new double[M];
  serviceCost = new double[L];
  facilityAssign = new long[N];

  servicePref = new long[N];
  isserviceOpen = new bool*[M];
  connectionCost = new double*[N];

  for (long n = 0; n < N; n++)
    connectionCost[n] = new double[M];

  for (long m = 0; m < M; m++)
    isserviceOpen[m] = new bool[L]; 

  clients_of_service.resize(L);
}

//Random Feasible Assignment of clients
void HierarchicalFL::initFeasibleSol()
{
  isfacOpen[0]=true;

  for (long j = 0; j < N; j++)
  {
    facilityAssign[j] = 0;
    isserviceOpen[0][servicePref[j]]=true;

  }
}

// initialization of setting
void HierarchicalFL::initAssignment(bool **_isserviceOpen, long *_facilityAssign)
{

  for (long n = 0; n < N; n++)
  {
    _facilityAssign[n] = facilityAssign[n];
  }

  for (long m = 0; m < M; m++)
  {
    for (long l = 0; l < L; l++)
    {
      _isserviceOpen[m][l] = isserviceOpen[m][l];
    }
  }

}

// update LOC Solution
void HierarchicalFL::updateAssignment(bool **_isserviceOpen, long *_facilityAssign)
{

  for (long n = 0; n < N; n++)
  {
    facilityAssign[n] = _facilityAssign[n];
  }

  for (long m = 0; m < M; m++)
  {
    for (long l = 0; l < L; l++)
    {
      isserviceOpen[m][l] = _isserviceOpen[m][l];
    }
  }

}

// swap(fId1, fId2) i.e. shutting facility fId1 and opening fId2
bool HierarchicalFL::swapMove(long fId1, long fId2)
{

  double oldCost = computeCost(isserviceOpen, facilityAssign);

  bool **_isserviceOpen = new bool*[M]; 
  long *_facilityAssign = new long[N];

  for (long m = 0; m < M; m++)
  {
    _isserviceOpen[m] = new bool[L];
  }

  initAssignment(_isserviceOpen, _facilityAssign);
  isfacOpen[fId1] = false;
  isfacOpen[fId2] = true;

  for (long j = 0; j < N; j++)
  {
    if(facilityAssign[j] == fId1)
    {
      _facilityAssign[j] = fId2;
    }
  }

  for (long l = 0; l < L; l++)
  {
    if(isserviceOpen[fId1][l])
    {
      _isserviceOpen[fId1][l] = false;
      _isserviceOpen[fId2][l] = true;
    }
  }
  bool flag = false;
  double newCost = computeCost(_isserviceOpen, _facilityAssign);

  if(newCost < epsilon*oldCost)
  {
    updateAssignment(_isserviceOpen, _facilityAssign);
    flag = true;
  }
  else
  {
    isfacOpen[fId1] = true;
    isfacOpen[fId2] = false;
  }

  delete _facilityAssign;

  for (long m = 0; m < M; m++)
  {
    delete _isserviceOpen[m];
  }

  delete _isserviceOpen;
  return flag;
}

//delete(fId) i.e. shutting facility fId
bool HierarchicalFL::deleteMove(long fId)
{

  vector <long> sList;
  list <long>::iterator it;

  double oldCost = computeCost(isserviceOpen, facilityAssign);
  
  bool **_isserviceOpen = new bool*[M]; 
  long *_facilityAssign = new long[N];

  for (long m = 0; m < M; m++)
  {
    _isserviceOpen[m] = new bool[L];
  }

  initAssignment(_isserviceOpen, _facilityAssign);
  isfacOpen[fId] = false;
  long fCount = 0;

  map <long, long> mapF;

  for (long m = 0; m < M; m++)
  {
    if(isfacOpen[m])
    {
      mapF[fCount] = m;
      fCount += 1;
    }
  }

  if(fCount == 0) 
  {
    isfacOpen[fId] = true;
    return false;
  }

  for (long l = 0; l < L; l++)
  {
    if(isserviceOpen[fId][l])
    {
      sList.push_back(l);
      _isserviceOpen[fId][l] = false;

      long cCount = 0, clients = 0;
      map <long, long> mapC;

      for (it =  clients_of_service[l].begin(); it!= clients_of_service[l].end() ;it++)
      {
        mapC[cCount]=*it;
        if(facilityAssign[*it] == fId)
          clients += 1;
        cCount += 1;
      }
      if(clients == 0)
        continue;

      double *fCost = new double[fCount];
      double **cCost = new double*[cCount];

      for (long n = 0; n < cCount; n++)
      {
        cCost[n] = new double[fCount];
      }

      for (long f = 0; f < fCount; f++)
      {

        fCost[f] = (isserviceOpen[mapF[f]][l]) ? 0 : serviceCost[l];
      }

      for (long c = 0; c < cCount; c++)
      {
        for (long f = 0; f < fCount; f++)
        {     
          cCost[c][f] = connectionCost[mapC[c]][mapF[f]];
        }
      }
      // Dispersing clients by solving an instance of Facility Location Problem
      FacilityLocation *fl = new FacilityLocation(fCount, cCount);
      fl->setfacilityCost(fCost);
      fl->setconnectionCost(cCost);
      fl->initFeasibleSol();
      fl->localSearch();

      for (long i = 0; i < fCount; i++)
      {
        _isserviceOpen[mapF[i]][l] = (fl->isFacilityOpen[i]) ? true : false;
      }

      for (long i = 0; i < cCount; i++)
      {
        _facilityAssign[mapC[i]] = mapF[fl->facilityAssign[i]];
      }

    }
  }

  bool flag = false;
  double newCost = computeCost(_isserviceOpen, _facilityAssign);

  if (newCost < epsilon*oldCost)
  {
    updateAssignment(_isserviceOpen, _facilityAssign);
    flag = true;
  }
  else
  {
    isfacOpen[fId] = true;
  }

  delete _facilityAssign;

  for (long m = 0; m < M; m++)
  {
    delete _isserviceOpen[m];
  }

  delete _isserviceOpen;
  return flag;
}

//add(fId) i.e. adding facility fID
bool HierarchicalFL::addMove(long fId)
{

  long cId ;
  double serviceSavings = 0.0, newCost, oldCost;
  double cost = computeCost(isserviceOpen, facilityAssign);

  double savings = -facilityCost[fId];

  list <long>::iterator it;
  vector <long>::iterator it2;
  vector<long> sClients, fClients, sList;

  for (long l = 0; l < L; l++)
  {
    serviceSavings = -serviceCost[l];
    for (it =  clients_of_service[l].begin(); it!= clients_of_service[l].end() ;it++)
    {
      cId = *it ;
      newCost = connectionCost[cId][fId] ;
      oldCost = connectionCost[cId][facilityAssign[cId]] ;
      if(oldCost > newCost)
      {
        serviceSavings += (oldCost - newCost) ;
        sClients.push_back(cId);
      }
    }
    if(serviceSavings > 0)
    {
      for(auto c : sClients)
      {
        fClients.push_back(c);
      }
      savings += serviceSavings ;
      sList.push_back(l) ;
    }
    sClients.clear();
  }

  if(cost - savings < epsilon*cost)               
  {
    for(auto c : fClients)
    {
      facilityAssign[c] = fId ;
    }

    for(it2 = sList.begin() ; it2!= sList.end() ; it2++)
    {
      isserviceOpen[fId][*it2] = true ;
    }
    isfacOpen[fId] = true;
    return true ;
  }
  return false ;
}

//Local Search Algorithm
void HierarchicalFL::localSearch()
{
  int i = 0 ;
  while(true)
  {
      i++ ;
      bool add_move_done = false;
      bool delete_move_done = false;
      bool swap_move_done = false;

      
      for (long m = 0; m < M; m++)
      {
        if(!isfacOpen[m])
        {
          add_move_done = addMove(m)||add_move_done;
        }
      }

      for(long i = 0; i < M; i++)
      {
        if(isfacOpen[i])
        {
          delete_move_done = deleteMove(i)||delete_move_done;
        }

      }

      for(long i = 0; i < M; i++)
      {
        for(long j = 0; j < M; j++)
        {
          if(i!=j)
          {
            if(isfacOpen[i] && !isfacOpen[j])
            {
              swap_move_done = swapMove(i,j) || swap_move_done;
            }
          }
        }
      }
      if(!(add_move_done|delete_move_done|swap_move_done))
        break;
  }
}

//printing LOC Solution
void HierarchicalFL::printDetails()
{
  cout << "Open facilities are:" << endl;

  for (long m = 0; m < M; m++)
  {
    cout << "facility no" << m << "  :  " <<isfacOpen[m]; 

    if(isfacOpen[m])
    {
      cout << " : ";
      for (long l = 0; l < L; l++)
      {
        cout << isserviceOpen[m][l] << " ";
      }
    }
    cout << endl;
  }

  cout << "Clients Assignment : " << endl;

  for (long n = 0; n < N; n++)
  {
    cout <<n<<":"<< facilityAssign[n] << " ";
  }
  cout << endl;

  cout << "Cost is :" << computeCost(isserviceOpen, facilityAssign) << endl;
}

// Methods for setting the properties of HierarchicalFLclass
void HierarchicalFL::setclientPref(long *clientPref)
{
  for(int i=0;i<N;i++)
  {
    servicePref[i] = clientPref[i];
    clients_of_service[clientPref[i]].push_back(i);
  }
}

void HierarchicalFL::setserviceCost(double *_serviceCost)
{
  for (long l = 0; l < L; l++)
    serviceCost[l] = _serviceCost[l];
}

void HierarchicalFL::setfacilityCost(double *_facilityCost)
{
  for (long m = 0; m < M; m++)
    facilityCost[m] = _facilityCost[m];
}

void HierarchicalFL::setconnectionCost(double **_connectionCost)
{
  for (long n = 0; n < N; n++)
  {
    for (long m = 0; m < M; m++)
    {
      connectionCost[n][m] = _connectionCost[n][m];
    }
  }
}

//compute cost of LOC Solution
double HierarchicalFL::computeCost(bool **_isserviceOpen, long *_facilityAssign)
{ 
  long double cost = 0.0;
  for (long m = 0; m < M; m++)
  {
    if(isfacOpen[m])
    {
      cost += facilityCost[m];
      for (long l = 0; l < L; l++)
      {
        if(_isserviceOpen[m][l])
          cost += serviceCost[l];
      }
    }
  }
  for (long n = 0; n < N; n++)
  {
    cost += connectionCost[n][_facilityAssign[n]];
  }
  return cost;
}

// Destructor of HierarchicalFLclass
HierarchicalFL::~HierarchicalFL()
{
  delete isfacOpen;
  delete facilityCost;
  delete serviceCost;
  delete facilityAssign;

  for (long n = 0; n < N; n++){
    delete connectionCost[n];
  }
  list<long>::iterator pd;
  for(long l = 0; l < L; l++)
  {
    for(list<long>::iterator it = clients_of_service[l].begin(); it != clients_of_service[l].end();)
    {
      pd = it;
      ++it;
        delete &pd;
    }
    delete &clients_of_service[l];
  }
  delete &clients_of_service;
  delete servicePref;
  
  for (long m = 0; m < M; m++)
  {
    delete isserviceOpen[m];
  }
  delete isserviceOpen;
  delete connectionCost;
}

#endif