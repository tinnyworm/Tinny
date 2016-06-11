#!/bin/sh -e

# usage: $0 api-id 
#    
# 		api-id: api.api-id.in.gz api.api-id.out.gz scr.api-id.out.gz

usage="Usage: $0 [-p storeprofile,mp=70] api-id \n"

MODEL_DIR=/work/price/falcon/ifal_models/uk/hsbc_debit_v2.0/rel40
API_DIR=/work/ffp5/FFM_HSBC_DEBIT_v2.0/crisis/productionAPI
RST_DIR=/work/ffp5/FFM_HSBC_DEBIT_v2.0/crisis/productionAPI

while getopts ":p:" opt; do
	case $opt in
		p  ) profopt=$OPTARG ;;
  		\? ) echo $usage
				exit 1 ;;
	esac
done

shift $(($OPTIND-1))

if [ -z "$@" ]; then
	echo $usage
	exit 1
fi

cd ${MODEL_DIR}
falconer4 \
	-R silent \
	-p $profopt \
	-a ${API_DIR}/api.$1.in.gz \
	-A ${RST_DIR}/api.$1.out.gz \
	-s ${RST_DIR}/scr.$1.out.gz \
	ukswtv20.cnf

# -p storeprofile	
