#include "../head/header.h"
#include "../head/system.h"
#include "../head/server.h"

int main(int argc, char *argv[]){
    init (argc, argv);

    poll();

    /* 关闭连接 */
   closeServer();

   return 0;
}
