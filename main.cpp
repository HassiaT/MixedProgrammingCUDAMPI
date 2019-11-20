#include<stdio.h>
#include<stdlib.h>
#include "mpi.h"
#include<iostream>
#include<fstream>
#include "header.h"
#include <time.h>


using namespace std;


/******* DECLARATIONS OF FUNCTIONS **********/
extern "C" void launchKernel(float*,float *, float*,int);

int main (int argc, char*argv[]) 
{
  int ab,i,n_options, node, dest, totalnodes, master =0, request;
  int *np, nthreads , nparam = 2;
  float *d_opt; /*h=host, d=device contains elements for options */  float *h_opt_price;
  float *h_opt_type;
  double t1, t2;
  MPI_Status status,stat;
  MPI_Request req,req1;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD,&totalnodes);
  MPI_Comm_rank(MPI_COMM_WORLD,&node);
  np = (int *) malloc(sizeof(int));

  if ( node == master ) 
    {
      // ask for number of options to generate 
      printf("Enter the number of options:64? 128? 256?More? \n");
      scanf("%d",&n_options);
      printf("You have chosen to generate %d options\n",n_options);
      h_opt_price =(float *) malloc(n_options * sizeof(float));
      h_opt_type = (float *) malloc(n_options * sizeof(float));
      t1 = MPI_Wtime();
      for(i =0;i<n_options;i++)
	{
	  srand(i);
	  // strike price:
	  h_opt_price[i] =(float)(rand()% 21+80);
	  //srand(i+1);
	  //type of option: Put or Call
	  h_opt_type[i] = (float)(rand()%2+1);
	}
      t2= MPI_Wtime();
      printf("Time for generating data with Usual Rand %f\n",t2-t1);
      for (i=0;i<n_options;i++) 
	{
	  printf("option %d: ",i);
	  printf("strike: %f, type %f\n",h_opt_price[i],h_opt_type[i]);
	}      
	
	//number of options that each slave process will receive:
	*np = n_options/(totalnodes-1);
	MPI_Bcast(np,1,MPI_INT,master,MPI_COMM_WORLD);
	float *buffer = h_opt_price;
	float *buffer1 = h_opt_type;
	float prices[totalnodes-1][*np];
	for (i=1;i<totalnodes;i++) 
	{
	    MPI_Isend(buffer,*(np),MPI_DOUBLE,i,0,MPI_COMM_WORLD,&req);
	    MPI_Wait(&req,&status);
	    buffer = buffer+(*(np));
	}
	for (i=1;i<totalnodes;i++)
	  {
	    MPI_Isend(buffer1,*(np),MPI_INT,i,0,MPI_COMM_WORLD,&req);
	    MPI_Wait(&req,&stat);
	    buffer1 = buffer1+(*(np));
	  }
    }
  
  /* all the slaves processes execute following code :*/
  else {
    float    *dev,*h_price,*d_price,*d_type,*d_strike;
    float    *sendbuf = (float*) malloc(*np * sizeof(float));
    float    *sendbufI=(float *) malloc(*np * sizeof(float));
    MPI_Status status1,status2;
    MPI_Request request1,request2;
    
    h_price = (float*) malloc((*np)* sizeof(float));
    MPI_Bcast(np,1,MPI_INT, master,MPI_COMM_WORLD);
    MPI_Irecv(sendbuf,*(np),MPI_DOUBLE,master,0,MPI_COMM_WORLD,&request1);
    MPI_Wait(&request1,&status1);
    MPI_Irecv(sendbufI,*(np),MPI_INT,master,0,MPI_COMM_WORLD,&request2);
    MPI_Wait(&request2,&status2);
    /* check data is really received */
    printf("process %d, type of option %f\n",node, sendbufI[2]);
    
    /* launch Kernel */    
    launchKernel(h_price,sendbuf,sendbufI,(*np));
  
    
    for (int k=0 ; k<(*np) ; k++)
    {
      printf("option type:%f,strikePrice:%f, pr%f\n",sendbufI[k],sendbuf[k],(0.5*0.8*(k+1))); 
    }
  }  
  
  //freeing memory:
   if( node == master )
     free(np);
  
  MPI_Finalize();
  return 0;// EXIT_SUCCESS;
}
