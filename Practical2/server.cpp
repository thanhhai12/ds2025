#include <grpcpp/grpcpp.h>
#include "file_transfer.grpc.pb.h"
#include <fstream>

class FileTransferServiceImpl final : public FileTransfer::Service {
public:
    grpc::Status UploadFile(grpc::ServerContext* context, const FileRequest* request, FileResponse* response) override {
        std::ofstream out(request->file_name(), std::ios::binary);
        out.write(request->file_data().data(), request->file_data().size());
        out.close();

        response->set_status("Success");
        response->set_message("File uploaded successfully.");
        return grpc::Status::OK;
    }

    grpc::Status DownloadFile(grpc::ServerContext* context, const FileRequest* request, FileResponse* response) override {
        std::ifstream in(request->file_name(), std::ios::binary);
        if (!in) {
            response->set_status("Failure");
            response->set_message("File not found.");
            return grpc::Status::OK;
        }

        std::vector<char> file_data((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
        response->set_status("Success");
        response->set_message("File downloaded successfully.");
        response->set_file_data(file_data.data(), file_data.size());
        return grpc::Status::OK;
    }
};

int main() {
    std::string server_address("0.0.0.0:50051");
    FileTransferServiceImpl service;

    grpc::ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;

    server->Wait();
    return 0;
}
