#ifndef ETHERNETIF_H
#define ETHERNETIF_H

#include <lwip/netif.h>
err_t ethernetif_init(struct netif *netif);

#endif /* ETHERNETIF_H */
