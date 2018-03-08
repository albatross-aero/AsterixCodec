# AsterixCodec
This C++ project is made of a library to decode and encode ASTERIX messages and a client application, which uses the 
above-mentioned library.

### Getting started

#### Prerequisites
* GCC 7.2.0
* CMake 3.2
* [Boost v1.65.1](http://www.boost.org/users/history/version_1_65_1.html)
    * _program_options_
    * _serialization_
    
    Be careful that, even if the most part of Boost is a _header-only_ library, these two are NOT and they need to be 
    compiled on the development machine you are working with. To get a general idea about how to use them, please follow 
    the instruction on Boost [Getting Started](http://www.boost.org/doc/libs/1_65_1/more/getting_started/unix-variants.html#prepare-to-use-a-boost-library-binary)
    guide.
    
    In the next paragraph, you will find the exact command to run.
    
* [nlohmann_json](http://www.github.com/nlohmann/json)
* [thread_pool](http://www.github.com/progschj/ThreadPool)
* [GoogleTest](http://www.github.com/google/googletest)
    
#### Installing
##### Boost v1.65.1
After you have successfully downloaded [Boost v1.65.1](http://www.boost.org/users/history/version_1_65_1.html), please 
follow the next steps, to successfully compile the _program-options_ and _serialization_ modules on your machine.
Be aware that the '_$_' symbol represents the shell prompt and shall not be written.

* $ **cd path/to/boost_1_65_1**
* $ **./bootstrap.sh --help**

    The option you are going to be most interested in will be:
    * --prefix=path/to/installation/prefix
    * --with-libraries=program_options,serialization
    
* $ **./b2 install**

    Boost binaries will be placed in the _/lib_ subdirectory of your installation prefix. Moreover, in the _/include_
    subdirectory of your installation prefix a copy of Boost headers will be copied, so that you may use that directory
    as _#include_ path, in place of the Boost root directory.
    
##### nlohmann_json
It is a _header-only_ library, **already included** in the project, in the following path: 
_/src/asterix_codec_lib/lib/nlohmann_json_.
It is used during the loading of the ASTERIX Category description JSON files.

##### thread_pool
It is a _header-only_ library, **already included** in the project, in the following path: 
_/src/asterix_codec_lib/lib/thread_pool_.
It is used to handle the multi-threading processing of the data stream.

### Compiling the project
Please follow the next steps, to successfully compile AsterixCodec project on your machine (both the 
library and the client application).
Be aware that the '_$_' symbol represents the shell prompt and shall not be written.

* $ **cd path/to/AsterixCodec**
* $ **cmake .**
* $ **make all**

### Running the client application
Once you have compiled the sources, you will find in the build path the binary called _AsterixCodecClient_.
To give it a try, please run the following command, which uses a preloaded CAT021 ASTERIX Record:
* $ **./AsterixCodecClient**
Be aware that the content of that Record is totally random (e.g. you will find un-acceptable coordinates values), so that you would be able to get a _failure_report.log_ file, where all problem faced during the _decoding_ phase are logged.

### Running the tests
"Explain how to run the automated tests for this system"

### Contributing

### Authors
* Federico Orta (active maintainer)
* Giulia Rivella
* Nadia Berloffa

### License
This project is licensed under the GNU General Public License v3.0 - see the LICENSE.md file for details
