# TRAFFIC-SNIFFER SERVER

## Prerequisites

❯ Make

❯ GCC

❯ PCAP Lib

❯ Git

❯ C/C++ IDE (optional). CLion is recommended.


> **Note**
>
> To install `PCAP Lib` on Linux you can use simple code:
> ```bash
> $ sudo apt-get update
> $ sudo apt-get install libpcap-dev
> ```
>
> To install `Make` you should run this code on your computer:
> ```bash
> $ sudo apt-get update
> $ sudo apt-get install make
> ```
>
> For compiler installation, please, refer to the official documentation of your compiler.
> For example GCC <a href="https://gcc.gnu.org/">link</a>.

## Compilation

1. You should compile our project run the `Makefile`:

```bash
$ make
# or
$ make -f Makefile
```

> **Note**
> 
> All object and executable files are saved in `./bin`
> 

2. If you want to get rid of executable files, run the `Makefile`:

```bash
$ make clean
```

## Installation

To install our project, you need to clone the repository first:

```bash
$ mkdir ~/workdir
$ cd ~/workdir
$ git clone https://github.com/CaCuCkA/traffic-server.git
$ cd traffic-server
```

> Replace `~/workdir` with the path to your workspace.

## Usage

In order to start the programme, first the sniffer and then the client programme must be started:
```bash
$ ./daemon
$ ./client
```

## Important

The function was only tested on `eth0` as I made the code under `WSL`.
````c
static void
select_iface(const char* iface_name);
````


