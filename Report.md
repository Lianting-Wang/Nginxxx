## Overview
The concept of this project was inspired by popular webservers such as Apache HTTP Server and Nginx (of which the project is named after). Under the currently web development environment, the concept of web server has been largely abstracted away, whether that is by App Engine offered on Google Cloud, or Lambda function as a service on AWS or Firebase by Google.

These changes may be indicating a new direction the development workflow web applications have to adapt with. However, just as the spawn of Python and other high-level language alike does not mean the obsolescence of assembly, the new solutions offered on the big cloud providers does not mean the death of web servers, but simply an abstraction.

The purpose of this project is then to explore part of the infrastructures that had been abstracted away in recent years. The project’s main objective is to implement a basic HTTP server. This means to implement the following functionalities:
- Accept HTTP requests
- Parse request method, URI, protocol, header, body
- Respond request with reasonable responses (code, header, body)
  
Above are some of the functionalities a basic HTTP servers is expected to be able to process. We will be following [RFC2616: Hypertext Transfer Protocol -- HTTP/1.1](https://www.rfc-editor.org/rfc/rfc2616) guideline to the best of our understanding and ability as we implement our server, to ensure the best compatibility.

This is however a rather vague description, there are a couple more points we would like to achieve to push our project beyond the absolute bare minimum:

- Non-blocking request handling
  - This is a technical hurdle that came up during discussion with professor Marcelo that we deemed important.
  - Consider the Google homepage. Countless individual make request to the URL at the same time, yet no user would need to another user's request to finish before receiving their response. We intend to achieve this
- Virtual Hosting
  - It is a waste to use an entire machine to only handle HTTP requests to for one domain. If possible, we would like to leverage all the computing power available for handle multiple domains

Beside the messurable objective, we also have things we hope to take away from:
- Improve proficiency with C
- Better understanding of interaction between C and Linux, such as `unistd, sys/*, fcntl`, etc.
- System programming experiences with socket, processes, file descriptor, etc.
- Deep understanding of the structure of HTTP protocol
- Ability to filter through complex documentation, and implement standards at low-level

## Members and contributions

## Setup & Testing

### Running
Execute `make`, to build gcc file "nginxxx" properly.
Execute `./run.sh`
If terminal outputs:
```
I am pid xxxx for port xx
Socket success! sock_fd=x listening...
```
Then the server is running and listening to port/s

## Main workflow:
![Workflow](Flow-Transparent.png)

## Implementation Detail

### Virtual Hosting
The user can edit the `conf/default.conf` file to setup for more hosts. On web server start, the server attempt to read the said file, and with the correct format, would be able to extinguish virtual host, each identified by a `server { ... }` block in the config file.
The server then proceed to start a child process for each of the host identified, each with its own socket to listen for requests meant for it.

### Non-blocking Request Handling
On each `accept` operation with socket, the server spawns a child process, and deligate all further handling of the request to the said child. The child would proceed with parsing, processing, and responding of the said request. 
Another way to understand the workflow is that on each new request, a new child process is spawned. The child process would handle the request as needed, and exit, freeing up resources.

### HTTP Method Implementations
As the project is a basic HTTP server, and mainly a proof of concept, we mapped URI to the file system of the host machine. We append the URI of the request to the root of the host, and consider the operations that can be done on the path
- `GET`
  - If the requested path is a file, the server would respond with `200 OK` and the content of the file
  - If the requested path is a directory, the server would check if there exists an index file at the requested directory. If so, the said index file would be returned along with `200 OK`. Otherwise, a `404 NOT FOUND` would be returned
  - If the requested path doesn't exist in any form, a `404 NOT FOUND` would be returned
- `POST`
  - Quoting the RFC standard: 
    >  The actual function performed by the POST method is determined by the server and is usually dependent on the Request-URI
  - Thus we decided to have our POST have a simple behaviour like the following:
  - On receiving request, the server would process the headers looking for header `Content-Length`. If one does not exists, a `411 LENGTH REQUIRED` response would be returned
  - Given the request path, the server would attempt to create a file at path requested, creating parent directories recursively as needed.
  - During the process of creating parent directories, if a conflict is reached, namely trying to create a directory when a regular file of the same name already exists, a `409 CONFLICT` would be returned.
  - If the requested path endpoint is occupied, as in either there already exists a file or directory of the same name, a `409 CONFLICT` would be returned, and the POST operation dropped
  - If the writing is done successfully, a `201 CREATED` response would be returned along with the URI that can be used to GET the content in the future
- `DELETE`
  - If no file / directory exists at the requested path, a `404 NOT FOUND` would be returned
  - If the requested path is a directory, a `405 METHOD NOT ALLOWED`
  - If the requested path is a file, the file would be deleted, and a `204 NO CONTENT` response returned
- `OPTIONS`
  - Given our interpretation of methods, we categorized possible operations as following:
    - If requested path does not exists, then the method possible are: `HEAD, POST, OPTIONS`
    - If requested path is a directory, then the methods possible are: `GET, HEAD, OPTIONS`
    - If requested path is a file, then the methods possible are: `GET, HEAD, DELETE, OPTIONS`
  

## Analysis and Discussion
Overall, we are proud of the prototype we have completed. Technical challenges such as the virtual host and non-blocking request handling have been a success. We were also able to implement some of the major HTTP methods that are used often in the real world, within the context that we've defined for ourselves.
Due to time constrain, we were not able to implement all HTTP methods, namely `PUT, TRACE, CONNECT`. We expect `PUT` to be easy to implement if we were given slightly more time, as its logic would be similar to `POST`, with some checking of file existing, heading. 
`TRACE` and `CONNECT` are going to be much more of a challenge, as it involves other HTTP servers, and sadly we did not have time to put great amount of consideration into the subject, but from reading the standard, we expect them to be rather different from the previous methods (such as SSL tunneling specified in RFC). 
Although not proven with statistic, a consequence of our child process approach is that it might be difficult to scale, as a machine could potentially need to manage tens of thousands of child processes. An alternative to our solution could be using `epoll` to handle request as it comes, and limit to a small number of processes.

However, we deemed the project an success overall. We've sieved through the complex RFC2616 and were able to implement functions we deem necessary, and were able to interact with our web server to get desired results. The project ends up an great exercise for us to refine our knowledge with system programming, and exploration of internet protocol.

The team also discussed areas to explore if this project were to continue. The current server implementation mostly return content back to client as it is stored in the server. An upgrade to the server would be to support a sense of "server side rendering". An example would be support of PHP rendering: On client request for `/.../xxx.php`, instead of returning the file content, we first evaluate the PHP tag with in, and return the resulting HTML.

Another would be the exploration into HTTPS, however, with our lack of experiences with security, this would likely require exponentially more time, and thus mostly remained as an after thought.

## Conclusion
Throughout the course, we've gone through the layers of computer networks, and have had hands on experiences on multiple of them. Though we have touched socket back in CSCB09, only now were we able to appreciate the physical, link, network, and transport that were all needed for us to scaffold up to the application layer that our project revolves around.
It was also interesting to see the flexibility application protocol allowed in comparison to IP, ICMP and ARP that we explored in the Simple-Router assignment. The flexibility is double sided, as it allows for implementation to design what is necessary, however it also makes the standard less rigid, and more likely for two implementation to be incompatible (consider all the possible headers an HTTP request could potentially contain). We experienced this during our development, and had to make decisions on what we believe to be important, and focus manpower on those aspects.
The project had been an interesting journey, however we are also aware that the protocol is on the way out, and replaced completely by either newer version of HTTP or HTTPS. Still, the project serves as a great exercise, and with our experience, we hope we would rise up to the challenge of new protocols when needed.
