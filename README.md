### mbase ###
#### base library of moon ####

moon is a highy concurrency dynamic web solution write in c

The main skeleton of moon as following

![skeleton](https://raw.githubusercontent.com/bigml/mbase/master/doc/pic/skeleton.png)

The moon's code modules as following

![modules](https://raw.githubusercontent.com/bigml/mbase/master/doc/pic/module.png)

Mbase is the basic component of moon. Moon's business worker (mgate, mevent, moc server) all depend on it.

You can checkout business worker with mbase together use --recursive option,
or, checkout mbase partly for personal use.

On fresh system, You need to excute premake.sh before mbase can compile ok.
This script just tested on Linux platform (currently, mbase also works on OsX, but not the premake.sh).


#### code attention ####

1. code guideline is in doc/spec.c (ident with 4 space, not tab)
   (setq-default indent-tabs-mode nil) in ~/.emacs
