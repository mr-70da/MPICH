#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char* argv[]) {
    int rank, np;
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&np);
    if (rank == 0) {
        if(np <2){
            printf("You should run this on 2 or more processes.\n");
            return 0;
        }
        printf("Hello from master process.\n");
        printf("Number of slave process is %d\n",np-1);
        printf("Please enter size of array...\n");
        int size_of_array;
        scanf("%d", &size_of_array);

        int* array = (int*) malloc(size_of_array * sizeof(int));
        int i = 0;
        printf("Please enter array element...\n");
        while (i < size_of_array) {
            scanf("%d", &array[i]);
            i++;
        }
        int sub_size = size_of_array / (np - 1);
        int remaining_elements = size_of_array % (np - 1);
        int offset = 0;
        i = 1;
        while (i < np) {
            int sent_count = sub_size + (i <= remaining_elements ? 1 : 0);
            MPI_Send(&sent_count, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(array+offset, sent_count, MPI_INT, i, 0, MPI_COMM_WORLD);
            i++;
            offset += sent_count;
        }
       
        int max_element = array[0], index = 0;

        i = 1;
        while (i < np) {
            int curr_index, curr_max,sent_count = (sub_size + (i <= remaining_elements ? 1 : 0));
            MPI_Recv(&curr_max, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(&curr_index, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            
            if (curr_max > max_element) {
                max_element =  curr_max;
                index = curr_index + offset;
            }
            offset += sent_count;
            i++;
        }
        printf("Master process announce the final max which is %d and its index is %d.\nThanks for using our program",max_element,index);
    }
    else
    {
        int recv_count,*sub_array;
        MPI_Recv(&recv_count, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        sub_array = (int*)malloc(recv_count * sizeof(int));
        MPI_Recv(sub_array, recv_count, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        int max_element = sub_array[0];
        int index;
        int i = 1 ;
        while (i < recv_count) {
            if (sub_array[i] > max_element) {
                max_element = sub_array[i];
                index =  i ;
            }
            i++;
        }
        printf("Hello from slave#%d Max number in my partition is %d and index is %d.\n",
            rank, max_element, index);
            
        MPI_Send(&max_element, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Send(&index, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        free(sub_array);
    }
    MPI_Finalize();
    return 0;
}