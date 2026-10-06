# What is it?

This is a simple include to parse GTA:SA "pedgrp.dat" file. This file contains information about which ped models belong to a certain ped group. These ped groups represent singleplayer's population groups such as business workers, golfers, ballas and others or in other words, if you want to spawn a random ped that belongs to a population group, then this parser will allow you to do that. https://github.com/wg1337/omp-gta-peds-ide

# How to use it?

1) Place "pedgrp.dat" in your "scriptfiles/" folder

2) Include it in your script:
```
#include <omp_gta_pedgrp>

public OnFilterScriptInit() {
    ....
    if(!LoadPedGrp()) {
        print("ERROR: Failed to load pedgrp.dat");
        return false;
    }
    ....
    return true;
}
```

3) Use the provided functions, for example:
```
//Get a random ped modelid that would a golfer have
new modelid = GetRandomPedGrpPed(GTA_PEDGRP_GOLFERS);
```
