/*#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include <set>

#include "def.h"*/
#include "udp_server_socket.h"
/*#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
*/
using namespace std;

int main()
{
	sockaddr_in addr;

	int fd = create_udp_server_socket(addr);

	udp_server_io(fd, addr);
}
