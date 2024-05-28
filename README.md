
# My Distributed File System

MyDFS (Distributed File System) is a simplified distributed file system that mimics some functionalities of larger, more complex systems like Hadoop's HDFS. 

MyDFS is running through a process. In this process, the name server and data servers are different threads.

Implemented File striping (Slicing a file into several chunks) , Each chunk has three replications, these replicas are distributed in different data servers.

Used MD5 (Message Digest Algorithm 5) to ensure data integrity and consistency.

## Basic functions of MyDFS

* Read/write a file
* Upload a file: upload success and return the ID of the file
* Read the location of a file based on the file ID and the offset

## Complile

```
make
```

## Run

```
./myDFS
put source_file_path dest_file_path
ls
locate FileID Offset
fetch FileID Offset dest_file_path
...
read source_file_path dest_file_path
...
```




