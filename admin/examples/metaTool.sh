#!/bin/sh

BASEPATH=`pwd`

export PYTHONPATH=/u1/lsst/lib/python2.5/site-packages:$BASEPATH/admin/python

PYTHON=/usr/bin/python # Use OS-default python, not SLAC /usr/local/bin/python
$PYTHON $BASEPATH/admin/bin/metaTool.py $*
