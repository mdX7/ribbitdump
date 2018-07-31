# RibbitDump

### Info
The dumper is based on information from wowdev wiki: http://wowdev.wiki/Ribbit  
Some CMake scripts are copied from TrinityCore (http://github.com/TrinityCore/TrinityCore)

### Configuration
The config files have to lie in the same directory as the binary itself.  
For some sample config files are found in the 'sampleconfigs' directory here.

You should set up a database and execute the query in the sql directory in there. This is used for caching purposes.

### Discord
The dumper supports discord notifications, you only need to set up the discord config (take a look at sampleconfig directory).

### Required 3rd party libs
libcurl: https://curl.haxx.se/libcurl/  
OpenSSL: https://slproweb.com/products/Win32OpenSSL.html  
Boost: https://www.boost.org/

### Alternative
Ribbit.NET: https://github.com/wowdev/Ribbit.NET

### Relevant repositories
ribbitdump(this): https://github.com/mdX7/ribbitdump  
ribbit_data: https://github.com/mdX7/ribbit_data