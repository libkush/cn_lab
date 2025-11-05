/* ip_info_short.c
 * Usage:
 *   ./ip_info_short 192.168.1.10/24
 *   ./ip_info_short 10.1.2.3 255.255.255.0
 *   ./ip_info_short 192.168.1.5 /31
 */

#include <arpa/inet.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int cidr_from_mask_uint32(uint32_t m) {
  int ones = 0;
  for (int i = 31; i >= 0; --i) {
    if ((m >> i) & 1U)
      ++ones;
    else
      break;
  }
  uint32_t recon = (ones == 0) ? 0U : (0xFFFFFFFFu << (32 - ones));
  return (recon == m) ? ones : -1;
}

static const char *ip_class(uint32_t ip_h) {
  unsigned first = (ip_h >> 24) & 0xFFu;
  if (first == 0)
    return "Reserved (0.x.x.x)";
  if (first == 127)
    return "Loopback (127.x.x.x)";
  if (first <= 126)
    return "Class A";
  if (first <= 191)
    return "Class B";
  if (first <= 223)
    return "Class C";
  if (first <= 239)
    return "Class D (Multicast)";
  if (first <= 254)
    return "Class E (Reserved)";
  return "Unknown";
}

static void ipv4_to_str(uint32_t h, char *buf, size_t len) {
  struct in_addr a;
  a.s_addr = htonl(h);
  inet_ntop(AF_INET, &a, buf, len);
}

int main(int argc, char **argv) {
  char ip_input[64] = {0}, mask_input[64] = {0};
  int cidr = -1;

  if (argc == 1) {
    char line[128];
    printf("Enter IP (ip/cidr  OR  ip mask): ");
    if (!fgets(line, sizeof(line), stdin))
      return 1;
    line[strcspn(line, "\n")] = 0;
    char *t = strtok(line, " \t");
    if (!t)
      return 1;
    strncpy(ip_input, t, sizeof(ip_input) - 1);
    t = strtok(NULL, " \t");
    if (t)
      strncpy(mask_input, t, sizeof(mask_input) - 1);
  } else if (argc == 2) {
    strncpy(ip_input, argv[1], sizeof(ip_input) - 1);
    char *s = strchr(ip_input, '/');
    if (s) {
      *s = 0;
      cidr = atoi(s + 1);
      if (cidr < 0 || cidr > 32) {
        fprintf(stderr, "Bad CIDR\n");
        return 1;
      }
    }
  } else {
    strncpy(ip_input, argv[1], sizeof(ip_input) - 1);
    strncpy(mask_input, argv[2], sizeof(mask_input) - 1);
  }

  if (mask_input[0] == '/') {
    cidr = atoi(mask_input + 1);
    mask_input[0] = 0;
  }

  struct in_addr ip_addr;
  if (inet_pton(AF_INET, ip_input, &ip_addr) != 1) {
    fprintf(stderr, "Invalid IP: %s\n", ip_input);
    return 1;
  }
  uint32_t ip_h = ntohl(ip_addr.s_addr);

  uint32_t mask_h = 0;
  if (cidr >= 0) {
    mask_h = (cidr == 0)
                 ? 0U
                 : (cidr == 32 ? 0xFFFFFFFFu : (0xFFFFFFFFu << (32 - cidr)));
  } else if (mask_input[0]) {
    struct in_addr maddr;
    if (inet_pton(AF_INET, mask_input, &maddr) != 1) {
      fprintf(stderr, "Invalid mask: %s\n", mask_input);
      return 1;
    }
    mask_h = ntohl(maddr.s_addr);
    cidr = cidr_from_mask_uint32(mask_h);
    if (cidr < 0) {
      fprintf(stderr, "Mask not contiguous: %s\n", mask_input);
      return 1;
    }
  } else {
    unsigned first = (ip_h >> 24) & 0xFFu;
    cidr = (first <= 126) ? 8 : (first <= 191 ? 16 : (first <= 223 ? 24 : 32));
    mask_h = (cidr == 0)
                 ? 0U
                 : (cidr == 32 ? 0xFFFFFFFFu : (0xFFFFFFFFu << (32 - cidr)));
  }

  if (cidr_from_mask_uint32(mask_h) < 0) {
    fprintf(stderr, "Derived mask invalid\n");
    return 1;
  }

  uint32_t net = ip_h & mask_h;
  uint32_t bcast = net | (~mask_h);
  int host_bits = 32 - cidr;
  uint64_t total = (host_bits >= 64)
                       ? 0
                       : (1ULL << host_bits); /* host_bits is 0..32 so safe */
  uint64_t usable = (host_bits == 0) ? 1 : (host_bits == 1 ? 0 : total - 2);
  uint32_t first = (cidr == 32) ? ip_h : (cidr == 31 ? net : net + 1);
  uint32_t last = (cidr == 32) ? ip_h : (cidr == 31 ? bcast : bcast - 1);

  char s_ip[INET_ADDRSTRLEN], s_mask[INET_ADDRSTRLEN], s_net[INET_ADDRSTRLEN],
      s_bcast[INET_ADDRSTRLEN], s_first[INET_ADDRSTRLEN],
      s_last[INET_ADDRSTRLEN];

  ipv4_to_str(ip_h, s_ip, sizeof(s_ip));
  ipv4_to_str(mask_h, s_mask, sizeof(s_mask));
  ipv4_to_str(net, s_net, sizeof(s_net));
  ipv4_to_str(bcast, s_bcast, sizeof(s_bcast));
  ipv4_to_str(first, s_first, sizeof(s_first));
  ipv4_to_str(last, s_last, sizeof(s_last));

  char out[1024];
  snprintf(out, sizeof(out),
           "Input IP: %s\n"
           "Validated IP: %s\n"
           "Class: %s\n"
           "CIDR: /%d    Mask: %s\n"
           "Network: %s    Broadcast: %s\n"
           "Total addresses: %" PRIu64 "    Usable hosts: %" PRIu64 "\n"
           "First usable: %s    Last usable: %s\n",
           ip_input, s_ip, ip_class(ip_h), cidr, s_mask, s_net, s_bcast, total,
           usable, s_first, s_last);

  puts(out);
  return 0;
}
