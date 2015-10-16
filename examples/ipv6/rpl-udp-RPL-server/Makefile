CONTIKI_PROJECT=rpl-nbr-server
all: $(CONTIKI_PROJECT) 


CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"

UIP_CONF_IPV6=1

CFLAGS+= -DUIP_CONF_IPV6_RPL

CONTIKI = ../../..
include $(CONTIKI)/Makefile.include
