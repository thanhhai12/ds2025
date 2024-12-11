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

    if (rank == 1) {
        // Client: Receive file chunks from server
        std::ofstream output_file("received_file.txt", std::ios::binary);
        if (!output_file) {
            std::cerr << "Failed to open file for writing.\n";
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        int bytes_read = 0;
        char buffer[CHUNK_SIZE];

        while (true) {
            MPI_Recv(&bytes_read, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // Receive chunk size
            if (bytes_read == 0) break; // End of file signal
            
            MPI_Recv(buffer, bytes_read, MPI_BYTE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // Receive chunk data
            output_file.write(buffer, bytes_read);
        }

        output_file.close();
    }

    MPI_Finalize();
    return 0;
}
