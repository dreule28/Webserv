*This project has been created as part of the 42 curriculum by dreule, 4n4k1n, gzovkic.*

# Webserv

## Description

A non-blocking HTTP/1.1 web server written in C++17nspired by Nginx. The server handles multiple simultaneous clients using `poll()` for I/O multiplexing and supports virtual hosting, CGI script execution, file uploads, and configurable routing — all driven by a Nginx-style configuration file.

Key features:
- Non-blocking I/O with `poll()`
- Multiple server blocks and virtual hosts (routing by `Host` header)
- GET, POST, DELETE methods
- CGI support (Python, etc.)
- File upload with configurable upload directory
- Directory auto-indexing
- HTTP redirects
- Custom error pages
- Configurable `client_max_body_size`

## Instructions

### Compilation

```bash
make
```

Requires a C++98-compatible compiler (`c++`). No external dependencies.

### Running

```bash
./webserv <config_file>
```

Example:

```bash
./webserv configs/default.conf
```

### Configuration

The config file follows Nginx-style syntax. A minimal example:

```nginx
server {
    listen 8080;
    host localhost;
    root ./test;
    index index.html;
    error_page 404 test/404.html;

    location / {
        methods GET POST;
        auto_index true;
        upload_dir test/upload;
    }

    location /cgi-bin {
        methods GET POST;
        cgi_extension .py;
        cgi_path /usr/bin/python3;
    }
}
```

Multiple `server` blocks on the same `host:port` are supported as virtual hosts — the server routes requests by matching the `Host` header against `server_name`.

Supported directives:

| Directive | Scope | Description |
|---|---|---|
| `listen` | server | Port to bind |
| `host` | server | IP/hostname to bind |
| `server_name` | server | Virtual host name |
| `root` | server, location | Document root |
| `index` | server, location | Default index file |
| `auto_index` | server, location | Enable directory listing |
| `error_page` | server | Custom error pages |
| `client_max_body_size` | server, location | Max request body size |
| `methods` | location | Allowed HTTP methods |
| `cgi_extension` | location | File extension for CGI |
| `cgi_path` | location | CGI interpreter path |
| `upload_dir` | location | Directory for uploaded files |
| `return` | location | HTTP redirect |

### Testing

Sample config files are in `configs/`. The `test/` directory contains static files and CGI scripts.

```bash
# Basic GET
curl http://localhost:8080/

# Virtual host routing
curl -H "Host: example.com" http://localhost:8080/

# File upload
curl -X POST -F "file=@myfile.txt" http://localhost:8080/upload
```

## Resources

- [RFC 7230 — HTTP/1.1 Message Syntax and Routing](https://datatracker.ietf.org/doc/html/rfc7230)
- [RFC 7231 — HTTP/1.1 Semantics and Content](https://datatracker.ietf.org/doc/html/rfc7231)
- [RFC 3875 — The Common Gateway Interface (CGI/1.1)](https://datatracker.ietf.org/doc/html/rfc3875)
- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/)
- [Nginx documentation](https://nginx.org/en/docs/)
- [poll(2) man page](https://man7.org/linux/man-pages/man2/poll.2.html)

### AI Usage

- Documentation