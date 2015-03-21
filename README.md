# forward.cgi

A link forwarder implemented in CGI

## How to build forward.cgi

To build forward.cgi, you need the following dependencies (names are Ubuntu
package names):

* [libjson-c-dev](https://github.com/json-c/json-c)
* [libfcgi-dev](https://github.com/toshic/libfcgi) (optional, may not work)

To build, run:

    make

There is no `make install` as you should copy `forward.cgi` to your `cgi-bin` or
document root.

## How to use forward.cgi

In the simplest way, create the file `/etc/forward.json` and populate it with
your link targets using short links as keys and target address as values.

