#! /usr/bin/env bash

ps -ef | grep '^ *yxc' | grep results | grep -v grep | perl -ne 'split ; print `kill $_[1]`, "\n";'

/work/price/falcon/Linux/bin/shmclear yxc
