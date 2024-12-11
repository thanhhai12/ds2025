#include <grpcpp/grpcpp.h>
#include "file_transfer.grpc.pb.h"
#include <fstream>
#include <iostream>

void UploadFile(std::shared_ptr<grpc::Channel> channel, const std::string& file_path) {
    std::unique_ptr<FileTransfer::Stub> stub_ = FileTransfer::NewStub(channel);

    FileRequest request;
    std::ifstream in(file_path, std::ios::binary);
    std::vector<char> file_data((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    request.set_file_name(file_path);
    request.set_file_data(file_data.data(), file_data.size());

    FileResponse response;
    grpc::ClientContext context;
    grpc::Status status = stub_->UploadFile(&context, request, &response);

    if (status.ok()) {
        std::cout << "Upload Status: " << response.status() << "\n";
        std::cout << "Message: " << response.message() << "\n";
    } else {
        std::cout << "RPC failed: " << status.error_message() << "\n";
    }
}

void DownloadFile(std::shared_ptr<grpc::Channel> channel, const std::string& file_name) {
    std::unique_ptr<FileTransfer::Stub> stub_ = FileTransfer::NewStub(channel);

    FileRequest request;
    request.set_file_name(file_name);

    FileResponse response;
    grpc::ClientContext context;
    grpc::Status status = stub_->DownloadFile(&context, request, &response);

    if (status.ok() && response.status() == "Success") {
        std::ofstream out("downloaded_" + file_name, std::ios::binary);
        out.write(response.file_data().data(), response.file_data().size());
        out.close();
        std::cout << "File downloaded successfully.\n";
    } else {
        std::cout << "RPC failed: " << status.error_message() << "\n";
    }
}

int main(int argc, char** argv) {
    std::string server_address("localhost:50051");
    std::shared_ptr<grpc::Channel> channel = grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials());

    if (argc < 3) {
        std::cerr << "Usage: client <upload|download> <file_path>\n";
        return 1;
    }

    std::string command = argv[1];
    std::string file_path = argv[2];

    if (command == "upload") {
        UploadFile(channel, file_path);
    } else if (command == "download") {
        DownloadFile(channel, file_path);
    } else {
        std::cerr << "Unknown command: " << command << "\n";
    }

    return 0;
}
