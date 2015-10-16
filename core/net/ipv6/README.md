README file for Contiki's IPv6 Neighbor Discovery core

Author: 

What does it do
===============
These files, alongside some core modifications, add support for IPv6 Neighbor Discovery
to contiki's uIPv6 engine.

Currently, two modes are supported:

* '6Lo-ND' ()
    
* 'RPL-based ND' according to the algorithm described
    in the internet draft:

The Big Gotcha
==============


Where to Start
==============
The best place in `examples/ipv6/ND`

There is a cooja example demonstrating basic functionality

How to Use
==========
Look in `core/net/ipv6/multicast/uip-nd6-engines.h` for a list of supported
Neighbor Discovery engines.

To turn on multicast support, add this line in your `project-` or `contiki-conf.h`

        #define UIP_ND6_CONF_ENGINE xyz

  where xyz is a value from `uip-nd6-engines.h`

To disable:

        #define UIP_ND6_CONF_ENGINE 0

You also need to make sure the neighbor discovery code gets built. 
How to extend
=============
Let's assume you want to write an engine called foo.
The multicast API defines a multicast engine driver in a fashion similar to
the various NETSTACK layer drivers. This API defines functions for basic
multicast operations (init, in, out).
In order to extend multicast with a new engine, perform the following steps:

- Open `uip-mcast6-engines.h` and assign a unique integer code to your engine

        #define UIP_MCAST6_ENGINE_FOO        xyz

  - Include your engine's `foo.h`

- In `foo.c`, implement:
  * `init()`
  * `in()`
  * `out()`
  * Define your driver like so:

        `const struct uip_mcast6_driver foo_driver = { ... }`

 