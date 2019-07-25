///////////////////////////////////////////////
// My backend system is in the "include/" directory, and my front end system is in the "http/jslib/" directory.
//
// The Backend file for the app i made you is Nitin.cpp, and the front end is "http/main_1.js"
//
// I already coded the web server to use the certification and private key in this directory.



/////////////////////////////////////////////
// Build from this directory: (Need gcc and openSSL installed)

gcc Nitin.cpp -std=c++11 -lstdc++ -Iinclude  -o NitinRunMe -lssl -lcrypto -lz



////////////////////////////////////////////
// Run after successfully built from this directory:

./NitinRunMe



//////////////////////////////////////////////
/ Access via broswer from same computer that is running it:

https://localhost:8555ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
