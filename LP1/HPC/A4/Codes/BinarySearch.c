
#include<mpi.h>
#include<stdio.h>

#define n 12

#define key 55

int a[] = {1,2,3,4,7,9,13,24,55,56,67,88};

int a2[20];

int binarySearch(int *array, int start, int end, int value) {
    int mid;
    
    while(start <= end) {
        mid = (start + end) / 2;
        if(array[mid] == value) 
            return mid;
        else if(array[mid] > value)
            end = mid - 1;
        else
            start = mid + 1;
    }
    return -1;
}

int main(int argc, char* argv[]) {
    int pid, np, elements_per_process, n_elements_received;
    
    MPI_Status status;
    
    MPI_Init(&argc, &argv);
    
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    MPI_Comm_size(MPI_COMM_WORLD, &np);
    
    if(pid == 0) {
        int index, i;
        
        if(np > 1) {
            for(i=1; i<np-1; i++) {
                
                index = i * elements_per_process;
                //element count
                MPI_Send(&elements_per_process, 1, MPI_INT, i, 0, MPI_COMM_WORLD);

                MPI_Send(&a[index], elements_per_process, MPI_INT, i, 0, MPI_COMM_WORLD);
            
            }
            
            index = i* elements_per_process;
            
            int elements_left = n - index;
            
            MPI_Send(&elements_left, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&a[index], elements_left, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
        
        int position = binarySearch(a, 0, elements_per_process-1, key);
        
        if(position != -1)
          printf("Found at: %d", position);
        
        int temp;
        
        for(i=1; i<np; i++) {
            MPI_Recv(&temp, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            int sender = status.MPI_SOURCE;
            
            if(temp != -1)
                printf("Found at: %d by %d", (sender*elements_per_process)+temp, sender);
        }
    }
    
    else {
        MPI_Recv(&n_elements_received, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        
        MPI_Recv(&a2, n_elements_received, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
    
        int position = binarySearch(a2, 0, n_elements_received-1, key);
        
        MPI_Send(&position, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }
    
    MPI_Finalize();
    
    return 0;
}
