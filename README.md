# LineBasedTime Client
A simple protocol & its reference client implementation used to fetch time information from a remote server. The server implementation can be found [here](/ethernetlord/linebasedtime-server/).

The documentation of the LineBasedTime protocol can be found in [this repository](/ethernetlord/linebasedtime-protocol/).

---

## Setup
A complete setup of the client consists of 3 steps:
* **compiling**,
* **installing** and
* **cleanup**.

### Compiling
Change your working directory to this repository's root and run this command:
```
make
```
The executable file ```linebasedtime_client``` will be created in your working directory. If you don't want to install it, it can be run directly from there.

### Installing
After compiling, run:
```
sudo make install
```
The executable will be available as ```lbtime``` in the ```/usr/local/bin/``` directory.

### Cleanup
After compiling or installing, run:
```
make clean
```
This will remove the ```linebasedtime_client``` file from your working directory. You can perform this cleanup without installing the program. If you wished to install it in the future, you would need to compile the program again.

---

## Usage
The compiled and/or installed client executable takes exactly 2 arguments:
1. IPv4/v6 address or hostname to connect to
2. TCP port number or service name to connect to

It can also make use of the following options:

| Option           | Explanation                             |
| ---------------- | --------------------------------------- |
| -v, --version    | print version and quit                  |
| -h, --help       | print usage and quit                    |
| -e, --extra-info | print info about client connection      |
| -u, --unix-only  | print only unix time                    |
| -c, --compare    | compare remote time with your machine's |

#### Example 1
If you installed the program and ```/usr/local/bin``` is in your ```PATH``` environment variable, you can run it as following:
```
lbtime localhost 5567
```
It will connect to the host ```localhost``` (usually *::1* or *127.0.0.1*) and port ```5567```. If the server is running on the specified host and port, the output will look like this:
```
Server's time:
   Unix:      1592759142
   UTC:       Sun Jun 21 17:05:42 2020
   Local:     Sun Jun 21 19:05:42 2020
```
The output format & amount of information outputted can be affected by using any of the available options, as specified above.

#### Example 2
If you compile the program (and don't perform a cleanup), you can also run it directly from the working directory (the repository's root) without its installation:
```
./linebasedtime_client localhost 5567
```
The output will be the same as in the [first example](#Example%201).

---

## Author
Made by **EthernetLord**.
* Website: https://ethernetlord.eu/
* GitHub: @ethernetlord (https://github.com/ethernetlord/)
* Twitter: @ethernetlordcz (https://twitter.com/ethernetlordcz)


## Licensing
This project is licensed under the **[MIT License](/ethernetlord/linebasedtime-client/blob/master/LICENSE)**.


## Contributing
If you want to remind me of any bugs or fix them right away, add some new functionality or just make something better, feel free to create a pull request or an issue.
