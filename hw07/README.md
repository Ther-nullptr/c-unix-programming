# homework 7

## 10A

### usage

```bash
$ g++ server.cpp -o build/server
$ g++ client.cpp -o build/client
$ ./build/server
$ ./build/client # client 1
$ ./build/client # client 2
```

### output

nickname setting:

```bash
# server
Server started on port 8080
New client connected: 4
New client connected: 5
4 is now known as alice
5 is now known as bob
```

```bash
# client 1
Connected to server. Use /nick <name> to set your nickname.
/nick alice
Nickname set to alice
```

```bash
# client 2
Connected to server. Use /nick <name> to set your nickname.
/nick bob
Nickname set to bob
```

dialogue:

```bash
# client 1
hello!
[bob]: hi!
I am alice.
[bob]: I am bob, nice to meet you.
```

```bash
[alice]: hello!
hi!
[alice]: I am alice.
I am bob, nice to meet you.
```

heartbeat and exit(exit client 1 as example):

```bash
# server
Server started on port 8080
New client connected: 4
New client connected: 5
4 is now known as alice
5 is now known as bob
Client 4 timed out. Closing connection.

```

```bash
# client 2
Client 4 has left the chat.
```
