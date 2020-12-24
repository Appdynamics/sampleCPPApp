# sampleCPPApp
A sample dockerized C++ application with an upstream and downstream


Setup:

1. HTTP server
2. An upstream server, which calls an endpoint in for loop and passes the correlation header
3. An exit call to an external application
Please refer setup.png for setup view


Instructions:

Edit .env file and update CONTROLLER_ACCESS_KEY to your access key, by default the controller host is: master-saas-controller.e2e.appd-test.com, you can accordingly if you wish to.
To run the application, run 
docker-compose build
docker-compose up
