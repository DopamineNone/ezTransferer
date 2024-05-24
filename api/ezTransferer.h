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
*   otherwise, buffer is the file content
*/
typedef struct {
    unsigned int code;
    unsigned int size;
    char* buffer;
} FetchResponse;

/*
* code: status code
*   0: failed
*   1: success
* size: length of the buffer
*   when code == 0, size == 1
* filelist: the file list
*   when code == 0, filelist[0] is the detail message of the error
*/
typedef struct {
    unsigned int code;
    unsigned int size;
    char** filelist;
} ViewResponse;

#ifdef __cplusplus
}
#endif

#endif