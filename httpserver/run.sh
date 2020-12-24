docker build -t cpphttp . && docker run -e CONTROLLER_ACCESS_KEY=<your_access_key> -v /Users/tdiwate/Git/cpphttp/httptmp:/tmp -p 8000:8000 cpphttp
