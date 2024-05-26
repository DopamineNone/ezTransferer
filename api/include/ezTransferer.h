// api for communication between ezTransferer client and server
#ifndef _EZTRANSFERER_H_
#define _EZTRANSFERER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>

/*
* op: operation code
*   0: fetch file
*   1: view directory
* filename: file name to fetch or directory to view
*   when op == 1, filename == NULL
*/
#define FETCH_FILE 0
#define VIEW_DIRECTORY 1

#define MAX_FILENAME_SIZE 255
typedef struct {
    unsigned int op;
    char filename[MAX_FILENAME_SIZE];
} Request;

void MarshalRequest(char* buffer, unsigned int op, char* filename);
void UnmarshalRequest(char* buffer, unsigned int* op, char* filename);


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

#define FAILED 0
#define START_TRANSFER 1
#define TRANSFERING 2
#define FINISHED_SUCCESS 3

#define MAX_BUFFER_SIZE 1024

typedef struct {
    unsigned int code;
    unsigned int size;
    char buffer[MAX_BUFFER_SIZE];
} Response;

void MarshalResponse(char* buffer, unsigned int code, unsigned int size, char* data);
void UnmarshalResponse(char* buffer, unsigned int* code, unsigned int* size, char* data);

#ifdef __cplusplus
}
#endif

#endif