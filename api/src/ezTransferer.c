#include "../include/ezTransferer.h"

void MarshalRequest(char* buffer, unsigned int op, char* filename) {
    Request req;
    req.op = op;
    if (filename!= NULL) {
        memcpy(req.filename, filename, MAX_FILENAME_SIZE);
    }
    memcpy(buffer, &req, sizeof(Request));
}

void UnmarshalRequest(char* buffer, unsigned int* op, const char* filename) { 
    Request req;
    memcpy(&req, buffer, sizeof(Request));
    *op = req.op;
    if (req.filename!= NULL) {
        memcpy(filename, req.filename, MAX_FILENAME_SIZE);
    }
}   

void MarshalResponse(char* buffer, unsigned int code, unsigned int size, char* data) {
    Response resp;
    resp.code = code;
    resp.size = size;
    if (data!= NULL) {
        memcpy(resp.buffer, data, size);
    }
    memcpy(buffer, &resp, sizeof(Response));
}

void UnmarshalResponse(char* buffer, unsigned int* code, unsigned int *size, const char* data) {
    Response resp;
    memcpy(&resp, buffer, sizeof(Response));
    *code = resp.code;
    *size = resp.size;
    if (resp.buffer!= NULL) {
        memcpy(data, resp.buffer, *size);
    }
}