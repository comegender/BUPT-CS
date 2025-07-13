#include "header.h"
#include "system.h"
#include "server.h"

int main(int argc, char *argv[])
{
    init(argc, argv);

    poll();

    /* 关闭连接 */
    closeServer();

    return 0;
}
