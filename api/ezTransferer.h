// api for communication between ezTransferer client and server
#ifndef _EZTRANSFERER_H_
#define _EZTRANSFERER_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
* op: operation code
*   0: fetch file
*   1: view directory
* filename: file name to fetch or directory to view
*   when op == 1, filename == NULL
*/
typedef struct {
    unsigned int op;
    char* filename;
} Request;


/*
* code: status code
*   0: failed
*   1: start transferring
*   2: transferring
*   3: finished with success
* size: 
*   when code == 1, size is the total size of the file to transfer
*   otherwise, size is length of the buffer
* buffer: 
*   when code == 0, buffer is detail message of the error
*   otherwise, buffer is the file content or file list
*/
typedef struct {
    unsigned int code;
    unsigned int size;
    char* buffer;
} Response;

#ifdef __cplusplus
}
#endif

#endif