Copyright (C) 2017- Robert Valler - All rights reserved.   
This file is part of the project: NetStack   
This project can not be copied and/or distributed   
without the express permission of the copyright holder   
*****************************************************************   

# NetStack
Network communication classes.   
Work in progress.... sooooo much work in progress.   

## Supports
* TCP/IP 
* UDP
* POSIX
* google protobuf
* OpenSSL AES
* OPenSSL TLS

## Usage
run the install script in root:   
```
./create.sh
```
## OpenSSL
Creation of pem files for testing:  
```
openssl req -x509 -newkey rsa:2048 -keyout key.pem -out cert.pem -days 365 -nodes
```

Test an active server:  
```
openssl s_client -connect 127.0.0.1:2001 -debug -msg -state
```


