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





