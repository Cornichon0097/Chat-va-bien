#include <assert.h>
#include <stdlib.h>

#include <cvb/fdmap.h>

int main(void)
{
        struct fdmap fdm = FDMAP_INIT;
        char *fdnames[] = {
                "stdin",
                "stdout",
                "stderr",
                "test"
        };

        assert(fdm_puts(&fdm, 0, fdnames[0]) == NULL);
        assert(fdm_puts(&fdm, 1, fdnames[1]) == NULL);
        assert(fdm_puts(&fdm, 2, fdnames[2]) == NULL);

        assert(fdm_remove(&fdm, 1) == fdnames[1]);

        fdm_destroy(&fdm);

        return EXIT_SUCCESS;
}
