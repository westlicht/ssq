#ifndef __FILEDEFS_H__
#define __FILEDEFS_H__

#define FILE_MAGIC "ssq32pat"
#define FILE_VERSION 0

/** file header */
typedef struct {
	char magic[8];
	int version;
} file_header_t;

#endif /*__FILEDEFS_H__*/
