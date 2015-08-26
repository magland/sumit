# sumit
Command-line computation of hash checksums. Like sha1sum except applies to folders as well as files and automatically caches computations on the local disk.

Usage: sumit [path_to_file_or_directory]

* In the case of files, outputs the sha1 checksum, equivalent to the output of sha1sum. Local caching is performed (in /tmp/sumit/sha1) so that checksums do not need to be recomputed on subsequent calls with large files. The cache indexing is by device/inode/size/modification_time so there is no problem if files are moved or renamed within the same file system.

* In the case of directories, outputs a unique sha1 checksum that depends only on the contents of the directory (not the name or location of the directory). The computation depends on the checksum of each and every file within the directory tree, but again checksums do not need to be recomputed for the files in subsequent calls. 
