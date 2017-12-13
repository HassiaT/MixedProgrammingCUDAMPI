#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include<curand.h>
#include<curand_kernel.h>
#include<cuda_runtime.h>
#include<cuda.h>
#include "header.h"

__device__ void generateNormal(int n, float * dev,curandState_t *st)	   
{
	const int blockId = blockIdx.x+blockIdx.y*gridDim.x;
	const int threadId = (blockId*(blockDim.x*blockDim.y)+(threadIdx.y*blockDim.x)+threadIdx.x);
	curand_init(n,blockIdx.x,0,st);
	for (int i=0; i<n;i++)
	{
		
          dev[i] = curand_normal(&st[blockIdx.x]);
	  //printf("val %f\n",dev[i]);
	}
}

__device__ float endOptionValue(float S,float K,
	   	 		float i, float mu,
				float v, float time,float type)
{

	float optVal, result;
	optVal = S * __expf(mu*time+v*sqrt(time)*i);

	if ( type == 1.0)
 	result = optVal -K;

	if (type == 2.0)
 	result = K - optVal;
	return (result > 0.0) ? result :0.0;
}       	      


__global__ void __MonteCarlo__ (float*d_price,float *d_strike, float*d_type, int n_opt,float*dev,curandState_t *states,float*temp)
{	const int numBlocks = gridDim.x*gridDim.y;
	const int thrPer = blockDim.x * blockDim.y;
	const int steps=365;
	float sum;
	const int blockId = blockIdx.x+blockIdx.y*gridDim.x;
	const int threadId = (blockId*(blockDim.x*blockDim.y)+(threadIdx.y*blockDim.x)+threadIdx.x);
	
	//printf("value threadId %d\n",threadId);	
	
	generateNormal(steps,temp,&states[threadId]);
	__syncthreads();

	for (int i =0;i<n_opt;i++)
	{   sum=0.0f;
	    //printf("value of %d, %f\n",threadId,temp[365]);
	    //{
		     for (int j=0;j<steps;j++)
		    { 
		
			//printf("val  %f\n",temp[j]);
		     	sum+=endOptionValue(100.0,d_strike[i],(blockIdx.x*temp[j]),0.1f,0.2f,1.0,d_type[i]);
		    
		     }
		/* average solution */
		d_price[blockId]= sum/(thrPer*gridDim.x);
		
	}
}

extern "C" void launchKernel(float*price,float *strike, float*type, int n_opt)
{
	int numBlocks =0;
	int thrPer =0;
	const int blocksPerOption = (n_opt < 16)? 64 : 16;
	const int thr = 16;
	curandState_t *states;
	float *d_price,*d_strike,*d_type,*dev,*temp;
	cudaMalloc((void**)&d_price, n_opt*sizeof(float));
	cudaMalloc((void**)&d_strike,n_opt*sizeof(float));
	cudaMalloc((void**)&d_type,n_opt*sizeof(float));
	cudaMalloc((void**)&dev,365*sizeof(float));
	cudaMalloc((void**)&temp,365*sizeof(float));
	const int doMultiBlock = (blocksPerOption *thrPer/n_opt)>=8192;
	
	//cudaEvent_t start, stop;
	//cudaEventCreate(&start);
	//cudaEventCreate(&stop);
	/* we use multiple blocks to price one option */
	if ( doMultiBlock )
	{
		/*2D grid */
		dim3 grid(blocksPerOption,n_opt);
		dim3 block(thr, thr);
		numBlocks = n_opt*blocksPerOption;
		thrPer = thr*thr;
	}

	
        else 
	{
		/* we use one block per option */
       		dim3 blocks(n_opt);
		dim3 threads(thr*thr);
		numBlocks = n_opt;
		thrPer = thr*thr;
	}

	cudaMalloc((void**)&states,numBlocks*thrPer*365*sizeof(curandState_t));
	

	cudaMemcpy(d_strike,strike,n_opt*sizeof(float),cudaMemcpyHostToDevice);
	cudaMemcpy(d_type,type,n_opt*sizeof(float),cudaMemcpyHostToDevice);
	//cudaEventRecord(start,0);			
	__MonteCarlo__<<<1024,256>>>(d_price,strike,d_type,n_opt,dev,states,temp);
	//cudaEventRecord(stop,0);
	//cudaEventSynchronize(stop);
	//float Time;
	//cudaEventElapsedTime(&Time,start,stop);
	//printf("Time to price options %f ms\n",Time);

	/* copy prices of options to CPU/Host*/
	cudaMemcpy(price,d_price, n_opt*sizeof(float),cudaMemcpyDeviceToHost);
	cudaFree(states);
	cudaFree(dev);
	cudaFree(d_type);
	cudaFree(d_strike);
	cudaFree(d_price);
	cudaFree(temp);
}

