mbase - basic of moon

moon is a highy concurrency dynamic web solution write in c
(currently by bigmaliang@gmail.com)



#### The main skeleton of moon ####

![skeleton](https://raw.githubusercontent.com/bigml/mbase/master/doc/pic/skeleton.png)



#### The moon's code modules ####

![modules](https://raw.githubusercontent.com/bigml/mbase/master/doc/pic/module.png)



### mbase ###
Mbase is the basic component of moon. Moon's business worker (mgate, mevent, moc server) all depend on it.

You can checkout business worker with mbase together use --recursive option,
or, checkout mbase partly for personal use.

On fresh system, You need to excute premake.sh before mbase can compile ok.
This script just tested on Linux platform (currently, mbase also works on OSX, but not the premake.sh).


#### directories ####
    mbase
    |-- doc                    documentation about programming and system
    |   `-- spec_c.c           moon's c code style guidline
    |
    |   |-- mnl                moon's library document
    |   |-- book               books
    |   |-- config             config files
    |   |-- learning           notes and summary for programming
    |   |-- pic                picturs of moon's document
    |   |-- script             shell script
    |   `-- webbench           web loadtest results
    |-- lib
    |   |-- js                 javascript modlues
    |   |-- mnl                moon's c library
    |   `-- patches            patches for other libraries
    |
    |-- maketool               tools to build mbase
    |
    |-- lab                    pre coding laboratory
    |
    |-- test                   unit test
    |
    `-- tut                    tutorial


#### attention ####

1. code style guideline is in doc/spe_c.c (ident with 4 space, not tab)
   (setq-default indent-tabs-mode nil) in ~/.emacs
