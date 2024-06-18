A rudimentary torrent client implementing the BEP_0003 specification: https://www.bittorrent.org/beps/bep_0003.html

This client does not have any modern optimizations like a distributed hash table for peer discovery or an adaptive queue size for pipelining requests. Currently, seeding files is unsupported and this application can only download files. 

## Usage:
A basic command line interface is provided: ```./<exe name> [file path] ```
