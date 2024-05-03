#!/usr/bin/bash

docker run --network host -it --volume "$(pwd)":/dts --volume /var/run/docker.sock:/var/run/docker.sock -w /dts pashandor789/dts_ci_env /bin/bash