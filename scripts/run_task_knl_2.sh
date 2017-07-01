#!/usr/bin/env bash
#
#export KMP_AFFINITY=granularity=fine,compact
#export MIC_ENV_PREFIX=PHI
#export PHI_KMP_AFFINITY=granularity=fine,compact


base_dir="/home/lwangay/workspace"
data_dir="${base_dir}/graphs"
bin_dir="${base_dir}/mic_bc/build"
bin_name="mic_bc"

flags="0x0002"
threshold="0.3"

numaCTL=numactl
numaPRE="$numaCTL --membind 1"



normal_graphs=( "kron_g500-simple-logn16.graph " "generate/rmat_17.txt" "smallworld.graph" )

cd ${bin_dir}


for i in "${normal_graphs[@]}"
do

    echo $numaPRE ${bin_dir}/${bin_name} -i ${data_dir}/$i -f ${flags} -t ${threshold} -o "100%" |gawk '{ print strftime("[%Y-%m-%d %H:%M:%S]"), $0 }'
    $numaPRE ${bin_dir}/${bin_name} -i ${data_dir}/$i -f ${flags} -t ${threshold} -o "100%" |gawk '{ print strftime("[%Y-%m-%d %H:%M:%S]"), $0 }'
done
