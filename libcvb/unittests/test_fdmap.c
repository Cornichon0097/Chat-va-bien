#include <assert.h>
#include <stdlib.h>
#include <string.h>

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

        assert(fdm_put(&fdm, 0, fdnames[0]) == NULL);
        assert(fdm_put(&fdm, 1, fdnames[1]) == NULL);
        assert(fdm_put(&fdm, 2, fdnames[2]) == NULL);

        assert(fdm_get(&fdm, 0) == fdnames[0]);
        assert(fdm_get(&fdm, 1) == fdnames[1]);
        assert(fdm_get(&fdm, 2) == fdnames[2]);

        assert(fdm_contains(&fdm, fdnames[0]) == 0);
        assert(fdm_contains(&fdm, fdnames[1]) == 1);
        assert(fdm_contains(&fdm, fdnames[2]) == 2);
        assert(fdm_contains(&fdm, fdnames[3]) == -1);

        assert(fdm_remove(&fdm, 1) == fdnames[1]);
        assert(fdm_get(&fdm, 1) == NULL);
        assert(fdm_contains(&fdm, fdnames[1]) == -1);

        assert(fdm_put(&fdm, 1, fdnames[3]) == NULL);
        assert(fdm_contains(&fdm, fdnames[3]) == 1);
        assert(fdm_put(&fdm, 1, fdnames[1]) == fdnames[3]);

        assert(fdm_get(&fdm, 0) == fdnames[0]);
        assert(fdm_get(&fdm, 1) == fdnames[1]);
        assert(fdm_get(&fdm, 2) == fdnames[2]);

        assert(fdm_contains(&fdm, fdnames[0]) == 0);
        assert(fdm_contains(&fdm, fdnames[1]) == 1);
        assert(fdm_contains(&fdm, fdnames[2]) == 2);

        fdm_destroy(&fdm);

        return EXIT_SUCCESS;
}
