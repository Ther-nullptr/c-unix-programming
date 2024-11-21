# homework 6

## 8

### usage

```bash
$ g++ 8.cpp -o build/8
$ ./build/8
```

### output

```
CSV to binary conversion completed.
Contents of the binary file:
ID: 2014310512, Name: Donald Trump, Department: Politics, Age: 70
ID: 2014310513, Name: Hillary Clinton, Department: Politics, Age: 69
ID: 2014310514, Name: Barack Obama, Department: Law, Age: 55
ID: 2014310515, Name: George Bush, Department: History, Age: 70
ID: 2014310516, Name: Bill Clinton, Department: History, Age: 70
ID: 2014310517, Name: George Bush, Department: History, Age: 72
ID: 2014310518, Name: Ronald Reagan, Department: Politics, Age: 89
ID: 2014310519, Name: Jimmy Carter, Department: Politics, Age: 92
ID: 2014310520, Name: Gerald Ford, Department: Politics, Age: 93
ID: 1974310521, Name: Chiang Kai-shek, Department: Politics, Age: 87
ID: 1974310523, Name: Chiang Ching-kuo, Department: Law, Age: 90
ID: 1974310524, Name: Lee Teng-hui, Department: Law, Age: 94
ID: 1974310525, Name: Chen Shui-bian, Department: Politics, Age: 66
ID: 1974310526, Name: Ma Ying-jeou, Department: History, Age: 66
ID: 1974310527, Name: Tsai Ing-wen, Department: Politics, Age: 60
```

## 9

### 什么是 Linux Container？

Linux 容器（Linux Containers）是一种轻量级、便捷的虚拟化技术，允许你在一个主机操作系统内运行多个独立的、隔离的应用环境。容器共享宿主操作系统的内核，但每个容器都有独立的用户空间，包括文件系统、进程、网络和挂载点等。

### 为什么要使用 Linux Container？

容器提供了一种更加高效、灵活的方式来部署、管理和扩展应用，避免了环境配置的复杂性，提高了开发和运维的效率。容器还可以提供更好的隔离性，保护应用免受其他应用的影响。

### 功能和典型应用场景

Linux 容器的主要功能包括：

- **隔离性**：容器提供了一种轻量级的虚拟化技术，可以隔离应用之间的资源，避免相互干扰。
- **便携性**：容器可以在不同的环境中运行，无需担心环境配置的问题。
- **快速部署**：容器可以快速启动和停止，提高了开发和运维的效率。

典型应用场景包括：

- **微服务架构**：容器可以帮助将应用拆分成多个独立的服务，提高了应用的可维护性和扩展性。
- **持续集成/持续部署**：容器可以帮助快速部署和测试应用，加速开发周期。
- **云原生应用**：容器可以与容器编排工具（如 k8s）结合使用，实现自动化部署和管理。

示例dockerfile见`Dockerfile-python2`和`Dockerfile-python3`。使用方法如下：

```bash
$ docker build -t python2-image -f Dockerfile-python2 .
$ docker build -t python3-image -f Dockerfile-python3 .
$ docker run python2-image
$ docker run python3-image
```