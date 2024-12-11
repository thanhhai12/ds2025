#include <mpi.h>
#include <fstream>
#include <iostream>
#include <vector>

#define CHUNK_SIZE 1024 // Define the size of each chunk

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        // Server: Receive file chunks from client
        std::ifstream input_file("large_file.txt", std::ios::binary);
        if (!input_file) {
            std::cerr << "Failed to open file.\n";
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        int chunk_number = 0;
        char buffer[CHUNK_SIZE];

        while (!input_file.eof()) {
            input_file.read(buffer, CHUNK_SIZE);
            int bytes_read = input_file.gcount();
            
            // Send the file chunk to client (rank 1)
            MPI_Send(&bytes_read, 1, MPI_INT, 1, 0, MPI_COMM_WORLD); // Send chunk size
            MPI_Send(buffer, bytes_read, MPI_BYTE, 1, 0, MPI_COMM_WORLD); // Send chunk data
            
            chunk_number++;
        }

        input_file.close();
    }

    MPI_Finalize();
    return 0;
}
