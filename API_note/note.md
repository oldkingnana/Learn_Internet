### 1 字节序问题

* 那么我们知道,不同设备可能会采用不同的字节序,所以在不同设备之间的数据互通的过程中必须要做字节序调整

* 那么有以下几个接口可以用
```CPP
#include <arpa/inet.h>

uint32_t htonl(uint32_t hostlong);
uint16_t htons(uint16_t hostshort);

uint32_t ntohl(uint32_t netlong);
uint16_t ntohs(uint16_t netshort);
```

* `h`表示本机/主机`IP`,即`host`,`n`表示`network`,表示网络`IP`,`l`/`s`表示过来的`IP`的长度,`l`就是`long`,`s`就是`short`

* 那么`htonl`就表示把`32`位长度的`hostlong`(本机`IP`)转为网络使用的字节序并返回

* 值得一提的是,网络中的字节序规定了只能使用大端序

* 实操:
```CPP
#include <arpa/inet.h>
#include <netinet/in.h>
#include <iostream>

int main()
{
        uint32_t IP = 0b11000000101010000000000000000001;
        // 192.168.0.1
        // 11000000 10101000 00000000 00000001

        std::cout << "host_IP: " << IP << "  ->" << "  network_IP: " << htonl(IP) << std::endl;
        // host_IP: 3232235521  ->  network_IP: 16820416
        // 11000000 10101000 00000000 00000001 -> 00000001 00000000 10101000 11000000

        std::cout << "network_IP: " << IP << "  ->" << "  host_IP: " << ntohl(IP) << std::endl;
        // network_IP: 3232235521  ->  host_IP: 16820416
        // 11000000 10101000 00000000 00000001 -> 00000001 00000000 10101000 11000000

        return 0;
}
```

* 如果本机用的是大端序,那么这些接口不会做任何转换,如果本机用的小端序,那么这些接口就会将大端序转换成小端序
* 当前我使用的是`Ubuntu 24.04`,那么你能看到`Ubuntu 24.04`用的是小端序,所以顺序被调转了

### 2 `POSIX API`


#### 2.1 `POSIX API`接口设计的核心思想
* 与`system V`的通信接口有所区别,`system V`主张用一个通用接口涵盖不同的功能,而不是像`system V`一样使用不同的接口

* 带来的问题是,学习的难度曲线会比较奇怪,因为起点很高,所以你会觉得`POSIX`的接口更加复杂一点,主要是可接受的参数类型非常多,但实际上我们常用的其实就那么几个而已,所以无伤大雅

* 但优势在于,总体的学习成本其实是降低了的,因为接口就那么几个,实现不同功能只需要传不同参数,所以实现不同功能的时候使用接口的方式其实没什么区别


#### 2.2 常用的`POSIX API`

##### 2.2.1 `socket`

```text
#include <sys/socket.h>

int socket(int domain, int type, int protocol);
```

* `domain`表示你要用的协议族
* `type`表示你要用的套接字语义
* `protocol`表示你要用的具体协议,如果填`0`,那就是默认设置

* 我们换成简单口语
* `domain`,或者说协议族包含网络层协议,但不等于网络层协议
* `type`,或者说套接字包含传输层协议,但不等于传输层协议
* 这样做的原因是,`socket`不仅仅需要支持网络通信,还需要支持本地通信,所以协议族和套接字中还包含其他通信方式的协议
* 所以"协议族"和"套接字"其实是一个"更大的抽象"

* 至于`protocol`就比较特殊,他帮助你获取具体协议,一般情况下,我们在网络中都只需要填`0`让接口帮我们自动获取默认协议,有特殊情况的时候,我们需要他帮我们获取特殊协议,比方说`DCCP`(类似于支持拥塞控制的`UDP`,`IPPROTO_DCCP`),或者是获取自定义协议

* 实际使用:
```CPP
int16_t socket_fd_ = socket(AF_INET, SOCK_DGRAM, 0);
```

* 网络常用`domain`: `AF_INET`(`IPv4`), `AF_INET6`(`IPv6`)
* 网络常用`type`: `SOCK_STREAM`(`TCP`), `SOCK_DGRAM`(`UDP`)

* 那么宏观来看,`socket`只是帮助我们打通了协议标准,确定了双方沟通的语言
* 但从哪里获取或者发送资源,还没有确定,所以我们需要`bind`

##### 2.2.2 `bind` & `sockaddr` & `sockaddr_in`

```text
#include <sys/socket.h>

int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```

* `bind`帮助我们将获得到的文件描述符绑定到一个具体的端口,`IP`,以及网卡
* `struct sockaddr_in`用于描述绑定到哪个端口,`IP`,网卡,用于`IPv4`
* `struct sockaddr_in6`用于`IPv6`

* `sockaddr`我们可以看作是一个通用类型,因为`C`风格的接口意味着没有继承,所以如果通过统一的对象描述不同信息就需要一个通用类型
* 实际在使用的时候我们需要强转,就类似于多态(实际上能强转是因为实际开辟的空间中布局一样)

```text
#include <netinet/in.h>

struct sockaddr_in {
   sa_family_t     sin_family;     /* AF_INET */
   in_port_t       sin_port;       /* Port number */
   struct in_addr  sin_addr;       /* IPv4 address */
};

struct sockaddr_in6 {
   sa_family_t     sin6_family;    /* AF_INET6 */
   in_port_t       sin6_port;      /* Port number */
   uint32_t        sin6_flowinfo;  /* IPv6 flow info */
   struct in6_addr sin6_addr;      /* IPv6 address */
   uint32_t        sin6_scope_id;  /* Set of interfaces for a scope */
};
```

* `socklen_t`就是一个长度类型

* 实际使用:
```CPP
struct sockaddr_in info;
bzero(&info, sizeof(info));

info.sin_port = htons(port_);
info.sin_family = AF_INET;
info.sin_addr.s_addr = INADDR_ANY;

int64_t bind_result = bind(socket_fd_, (struct sockaddr*)&info, sizeof(info));
```

##### 2.2.3 `bzero`

```text
#include <strings.h>

void bzero(void s[.n], size_t n);
```

* 用于清空一段空间


##### 2.2.4 `recvfrom`

```text
#include <sys/socket.h>

ssize_t recvfrom(int sockfd, void buf[restrict .len], size_t len, int flags, struct sockaddr *_Nullable restrict src_addr, socklen_t *_Nullable restrict addrlen);
```

* 用于从某个`socket`中获取数据,同时还能获取发送者的信息(其实就是获取发送方的包的`header`)到`src_addr`,同时还会帮你阻塞等待

* 实操
```CPP
// recv
struct sockaddr_in peer;
socklen_t peer_len = sizeof(peer);

int16_t recv_len = recvfrom(socket_fd_, buff, sizeof(buff) - 1, 0, (struct sockaddr *)&peer, (socklen_t *)&peer_len);
```

##### 2.2.5 `sendto`

```text
#include <sys/socket.h>

ssize_t sendto(int sockfd, const void buf[.len], size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen);
```

* 帮你发送东西到目标`IP`+目标`port`
* 如果没有`bind`的话,第一次调用它的时候它会自动帮你随机`bind`一个端口(自动`bind`方便了客户端的设计)

```CPP
sendto(socket_fd_, result.c_str(), result.size(), 0, (struct sockaddr *)&peer, peer_len);
```


##### 2.2.6 `inet_aton`,``

```text
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int inet_aton(const char *cp, struct in_addr *inp);
char *inet_ntoa(struct in_addr in);
```

* 用于转换用一个点分制`IPv4`地址(字符串)和纯机器看得懂的结构体(其实本质就是一个空间中存的二进制)





