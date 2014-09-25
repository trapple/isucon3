#!/bin/bash

HOME=/home/isucon
PATH=$HOME/local/perl-5.18/bin:$PATH:$HOME/bin
export PATH 


DIR="$(cd "$(dirname "${BASH_SOURCE:-$0}")"; pwd)"

SQL=()
SQL[0]=$DIR/../sql/000_memos_add_column_title.sql
SQL[1]=$DIR/../sql/001_memos_update_title.sql
SQL[2]=$DIR/../sql/002_create_index_memos.sql
SQL[3]=$DIR/../sql/003_create_table_pub_memos.sql

for sql in ${SQL[@]}; do
    echo "loading ${sql}"
    mysql -u isucon isucon < ${sql}
done

PERL_CARTON_CPANFILE=/home/isucon/webapp/perl/cpanfile
cd $DIR/../webapp/perl
carton exec mysql-warmup isucon -u isucon  1>/dev/null 2>/dev/null
carton exec perl $DIR/../script/load_username.pl
exit 0
