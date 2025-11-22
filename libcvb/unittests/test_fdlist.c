#include <assert.h>
#include <stdlib.h>

#include <cvb/fdlist.h>

int main(void)
{
        struct fdlist fdl = FDLIST_INIT;

        assert(fdl_add(&fdl, 0, 0) == 0);
        assert(fdl.nfds == 1);

        fdl_add(&fdl, 1, 1);
        fdl_add(&fdl, 3, 2);
        assert(fdl.nfds == 3);

        assert(fdl_get(&fdl, 1) != NULL);
        assert(fdl_get(&fdl, 1)->fd == 1);
        assert(fdl_get(&fdl, 1)->events == 1);

        assert(fdl_get(&fdl, 5) == NULL);
        assert(fdl_remove(&fdl, 5) != 0);

        assert(fdl_remove(&fdl, 1) == 0);
        assert(fdl_get(&fdl, 1) == NULL);

        assert(fdl.fds[1].fd == -1);

        fdl_destroy(&fdl);
        assert(fdl.fds == NULL);
        assert(fdl.nfds == 0);

        return EXIT_SUCCESS;
}
